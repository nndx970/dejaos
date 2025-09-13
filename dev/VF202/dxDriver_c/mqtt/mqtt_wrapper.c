#include "mqtt_wrapper.h"
#include "MQTTAsync.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

#define MAX_CLIENT_ID_LEN 256
#define MAX_HOST_LEN 256
#define MAX_USERNAME_LEN 256
#define MAX_PASSWORD_LEN 256
#define MAX_MESSAGE_QUEUE 100
#define MAX_WILL_TOPIC_LEN 256
#define MAX_WILL_MESSAGE_LEN 1024

// 内部客户端结构
struct mqtt_client {
    MQTTAsync paho_client;
    char client_id[MAX_CLIENT_ID_LEN];
    char host[MAX_HOST_LEN];
    int port;
    int keepalive_interval;
    char username[MAX_USERNAME_LEN];
    char password[MAX_PASSWORD_LEN];
    mqtt_status_t status;
    pthread_mutex_t status_mutex;
    int auto_reconnect;
    int clean_session;
    int min_retry_interval;
    int max_retry_interval;
    
    // 遗嘱配置
    char will_topic[MAX_WILL_TOPIC_LEN];
    char will_message[MAX_WILL_MESSAGE_LEN];
    int will_qos;
    int will_retained;
    MQTTAsync_willOptions will;
    
    // 消息队列（用于轮询接收）
    mqtt_message_t message_queue[MAX_MESSAGE_QUEUE];
    int message_queue_head;
    int message_queue_tail;
    int message_queue_count;
    pthread_mutex_t message_mutex;
    
    // 状态变化标志
    int status_changed;
    mqtt_status_t last_status;
    
    // 连接开始时间（用于超时检查）
    time_t connect_start_time;
};

// 全局客户端实例
static struct mqtt_client* g_mqtt_client = NULL;
static pthread_mutex_t g_client_mutex = PTHREAD_MUTEX_INITIALIZER;

// 内部回调函数
static int message_arrived(void* context, char* topicName, int topicLen, MQTTAsync_message* message) {
    struct mqtt_client* client = (struct mqtt_client*)context;
    
    if (client) {
        pthread_mutex_lock(&client->message_mutex);
        
        // 检查队列是否已满
        if (client->message_queue_count < MAX_MESSAGE_QUEUE) {
            int idx = client->message_queue_tail;
            
            // 复制主题
            int topic_len = (topicLen > 0) ? (int)topicLen : (int)strlen(topicName);
            client->message_queue[idx].topic = malloc(topic_len + 1);
            if (client->message_queue[idx].topic) {
                strncpy(client->message_queue[idx].topic, topicName, topic_len);
                client->message_queue[idx].topic[topic_len] = '\0';
            }
            
            // 复制载荷
            client->message_queue[idx].payload = malloc(message->payloadlen + 1);
            if (client->message_queue[idx].payload) {
                memcpy(client->message_queue[idx].payload, message->payload, message->payloadlen);
                client->message_queue[idx].payload[message->payloadlen] = '\0';
                client->message_queue[idx].payload_len = message->payloadlen;
            } else {
                client->message_queue[idx].payload_len = 0;
            }
            
            // 设置其他属性
            client->message_queue[idx].qos = message->qos;
            client->message_queue[idx].retained = message->retained;
            
            // 更新队列
            client->message_queue_tail = (client->message_queue_tail + 1) % MAX_MESSAGE_QUEUE;
            client->message_queue_count++;
        } else {
            // 队列已满，丢弃消息但需要释放可能已分配的内存
            // 注意：这里不需要释放内存，因为队列满时不会分配新内存
        }
        
        pthread_mutex_unlock(&client->message_mutex);
    }
    
    MQTTAsync_freeMessage(&message);
    MQTTAsync_free(topicName);
    return 1;
}

static void connection_lost(void* context, char* cause) {
    (void)cause; // 未使用参数
    struct mqtt_client* client = (struct mqtt_client*)context;
    
    if (client) {
        pthread_mutex_lock(&client->status_mutex);
        client->last_status = client->status;
        client->status = MQTT_STATUS_DISCONNECTED;
        client->status_changed = 1;
        pthread_mutex_unlock(&client->status_mutex);
    }
}

static void on_connect(void* context, MQTTAsync_successData* response) {
    (void)response; // 未使用参数
    struct mqtt_client* client = (struct mqtt_client*)context;
    
    if (client) {
        pthread_mutex_lock(&client->status_mutex);
        client->last_status = client->status;
        client->status = MQTT_STATUS_CONNECTED;
        client->status_changed = 1;
        pthread_mutex_unlock(&client->status_mutex);
    }
}

static void on_connect_failure(void* context, MQTTAsync_failureData* response) {
    (void)response; // 未使用参数
    struct mqtt_client* client = (struct mqtt_client*)context;
    
    if (client) {
        pthread_mutex_lock(&client->status_mutex);
        client->last_status = client->status;
        client->status = MQTT_STATUS_ERROR;
        client->status_changed = 1;
        pthread_mutex_unlock(&client->status_mutex);
    }
}

static void on_disconnect(void* context, MQTTAsync_successData* response) {
    (void)response; // 未使用参数
    struct mqtt_client* client = (struct mqtt_client*)context;
    
    if (client) {
        pthread_mutex_lock(&client->status_mutex);
        client->last_status = client->status;
        client->status = MQTT_STATUS_DISCONNECTED;
        client->status_changed = 1;
        pthread_mutex_unlock(&client->status_mutex);
    }
}

static void on_publish(void* context, MQTTAsync_successData* response) {
    (void)context; // 未使用参数
    (void)response; // 未使用参数
    // 发布成功的回调，可以在这里添加日志或其他处理
}

static void on_subscribe(void* context, MQTTAsync_successData* response) {
    (void)context; // 未使用参数
    (void)response; // 未使用参数
    // 订阅成功的回调
}

static void on_unsubscribe(void* context, MQTTAsync_successData* response) {
    (void)context; // 未使用参数
    (void)response; // 未使用参数
    // 取消订阅成功的回调
}

// API实现
int mqtt_create_client(const char* client_id) {
    if (!client_id) {
        return -1;
    }
    
    pthread_mutex_lock(&g_client_mutex);
    
    // 检查是否已存在客户端
    if (g_mqtt_client != NULL) {
        pthread_mutex_unlock(&g_client_mutex);
        return -1; // 客户端已存在
    }
    
    g_mqtt_client = (struct mqtt_client*)malloc(sizeof(struct mqtt_client));
    if (!g_mqtt_client) {
        pthread_mutex_unlock(&g_client_mutex);
        return -1;
    }
    
    memset(g_mqtt_client, 0, sizeof(struct mqtt_client));
    strncpy(g_mqtt_client->client_id, client_id, MAX_CLIENT_ID_LEN - 1);
    g_mqtt_client->client_id[MAX_CLIENT_ID_LEN - 1] = '\0';
    g_mqtt_client->status = MQTT_STATUS_DISCONNECTED;
    g_mqtt_client->port = 1883; // 默认端口
    g_mqtt_client->keepalive_interval = 60; // 默认保活间隔
    g_mqtt_client->auto_reconnect = 1; // 默认启用自动重连
    g_mqtt_client->clean_session = 0; // 默认不启用清理会话，需要通过mqtt_set_connection_params设置
    g_mqtt_client->min_retry_interval = 1; // 默认最小重连间隔1秒
    g_mqtt_client->max_retry_interval = 60; // 默认最大重连间隔60秒
    g_mqtt_client->status_changed = 0;
    g_mqtt_client->last_status = MQTT_STATUS_DISCONNECTED;
    
    // 初始化遗嘱配置
    g_mqtt_client->will_topic[0] = '\0';
    g_mqtt_client->will_message[0] = '\0';
    g_mqtt_client->will_qos = 0;
    g_mqtt_client->will_retained = 0;
    
    // 初始化遗嘱选项结构体
    strcpy(g_mqtt_client->will.struct_id, "MQTW");
    g_mqtt_client->will.struct_version = 1;
    g_mqtt_client->will.topicName = NULL;
    g_mqtt_client->will.message = NULL;
    g_mqtt_client->will.retained = 0;
    g_mqtt_client->will.qos = 0;
    g_mqtt_client->will.payload.len = 0;
    g_mqtt_client->will.payload.data = NULL;
    
    // 初始化互斥锁
    if (pthread_mutex_init(&g_mqtt_client->status_mutex, NULL) != 0 ||
        pthread_mutex_init(&g_mqtt_client->message_mutex, NULL) != 0) {
        free(g_mqtt_client);
        g_mqtt_client = NULL;
        pthread_mutex_unlock(&g_client_mutex);
        return -1;
    }
    
    pthread_mutex_unlock(&g_client_mutex);
    return 0;
}

int mqtt_set_connection_params(const char* host, int port, int keepalive_interval, int clean_session, const char* username, const char* password, const char* will_topic, const char* will_message, int will_qos, int will_retained) {
    if (!host || !g_mqtt_client) {
        return -1;
    }
    
    // 设置连接参数
    strncpy(g_mqtt_client->host, host, MAX_HOST_LEN - 1);
    g_mqtt_client->host[MAX_HOST_LEN - 1] = '\0';
    g_mqtt_client->port = port;
    g_mqtt_client->keepalive_interval = keepalive_interval;
    g_mqtt_client->clean_session = clean_session;
    
    // 设置认证信息
    if (username) {
        strncpy(g_mqtt_client->username, username, MAX_USERNAME_LEN - 1);
        g_mqtt_client->username[MAX_USERNAME_LEN - 1] = '\0';
    } else {
        g_mqtt_client->username[0] = '\0';
    }
    
    if (password) {
        strncpy(g_mqtt_client->password, password, MAX_PASSWORD_LEN - 1);
        g_mqtt_client->password[MAX_PASSWORD_LEN - 1] = '\0';
    } else {
        g_mqtt_client->password[0] = '\0';
    }
    
    // 设置遗嘱配置
    if (will_topic && strlen(will_topic) > 0) {
        strncpy(g_mqtt_client->will_topic, will_topic, MAX_WILL_TOPIC_LEN - 1);
        g_mqtt_client->will_topic[MAX_WILL_TOPIC_LEN - 1] = '\0';
        
        if (will_message) {
            strncpy(g_mqtt_client->will_message, will_message, MAX_WILL_MESSAGE_LEN - 1);
            g_mqtt_client->will_message[MAX_WILL_MESSAGE_LEN - 1] = '\0';
        } else {
            g_mqtt_client->will_message[0] = '\0';
        }
        
        g_mqtt_client->will_qos = will_qos;
        g_mqtt_client->will_retained = will_retained;
    } else {
        // 清除遗嘱配置
        g_mqtt_client->will_topic[0] = '\0';
        g_mqtt_client->will_message[0] = '\0';
        g_mqtt_client->will_qos = 0;
        g_mqtt_client->will_retained = 0;
    }
    
    // 自动重连参数保持默认值（在 mqtt_create_client 中已设置）
    // g_mqtt_client->auto_reconnect = 1; // 默认启用自动重连
    // g_mqtt_client->min_retry_interval = 1; // 默认最小重连间隔1秒
    // g_mqtt_client->max_retry_interval = 60; // 默认最大重连间隔60秒
    
    return 0;
}



int mqtt_connect() {
    if (!g_mqtt_client || strlen(g_mqtt_client->host) == 0) {
        return -1;
    }
    
    char server_uri[512];
    snprintf(server_uri, sizeof(server_uri), "%s:%d", g_mqtt_client->host, g_mqtt_client->port);
    
    int rc = MQTTAsync_create(&g_mqtt_client->paho_client, server_uri, g_mqtt_client->client_id,
                               MQTTCLIENT_PERSISTENCE_NONE, NULL);
    if (rc != MQTTASYNC_SUCCESS) {
        return rc;
    }
    
    // 设置回调函数
    rc = MQTTAsync_setCallbacks(g_mqtt_client->paho_client, g_mqtt_client, connection_lost, message_arrived, NULL);
    if (rc != MQTTASYNC_SUCCESS) {
        MQTTAsync_destroy(&g_mqtt_client->paho_client);
        return rc;
    }
    
    MQTTAsync_connectOptions conn_opts = MQTTAsync_connectOptions_initializer;
    conn_opts.keepAliveInterval = g_mqtt_client->keepalive_interval;
    conn_opts.cleansession = g_mqtt_client->clean_session;
    conn_opts.automaticReconnect = g_mqtt_client->auto_reconnect; // 启用自动重连
    conn_opts.minRetryInterval = g_mqtt_client->min_retry_interval; // 最小重连间隔
    conn_opts.maxRetryInterval = g_mqtt_client->max_retry_interval; // 最大重连间隔
    conn_opts.onSuccess = on_connect;
    conn_opts.onFailure = on_connect_failure;
    conn_opts.context = g_mqtt_client;
    
    // 设置遗嘱配置
    if (strlen(g_mqtt_client->will_topic) > 0) {
        conn_opts.will = &g_mqtt_client->will;
        g_mqtt_client->will.topicName = g_mqtt_client->will_topic;
        g_mqtt_client->will.message = g_mqtt_client->will_message;
        g_mqtt_client->will.qos = g_mqtt_client->will_qos;
        g_mqtt_client->will.retained = g_mqtt_client->will_retained;
        g_mqtt_client->will.payload.len = strlen(g_mqtt_client->will_message);
        g_mqtt_client->will.payload.data = g_mqtt_client->will_message;
    }
    
    if (strlen(g_mqtt_client->username) > 0) {
        conn_opts.username = g_mqtt_client->username;
        if (strlen(g_mqtt_client->password) > 0) {
            conn_opts.password = g_mqtt_client->password;
        }
    }
    
    pthread_mutex_lock(&g_mqtt_client->status_mutex);
    g_mqtt_client->status = MQTT_STATUS_CONNECTING;
    g_mqtt_client->connect_start_time = time(NULL); // 记录连接开始时间
    pthread_mutex_unlock(&g_mqtt_client->status_mutex);
    
    rc = MQTTAsync_connect(g_mqtt_client->paho_client, &conn_opts);
    if (rc != MQTTASYNC_SUCCESS) {
        pthread_mutex_lock(&g_mqtt_client->status_mutex);
        g_mqtt_client->status = MQTT_STATUS_ERROR;
        pthread_mutex_unlock(&g_mqtt_client->status_mutex);
        MQTTAsync_destroy(&g_mqtt_client->paho_client);
        return rc;
    }
    
    return 0;
}

int mqtt_reconnect(void) {
    if (!g_mqtt_client || !g_mqtt_client->paho_client) {
        return -1;
    }
    
    // 使用 Paho MQTT 库的主动重连功能
    int rc = MQTTAsync_reconnect(g_mqtt_client->paho_client);
    if (rc != MQTTASYNC_SUCCESS) {
        pthread_mutex_lock(&g_mqtt_client->status_mutex);
        g_mqtt_client->status = MQTT_STATUS_ERROR;
        pthread_mutex_unlock(&g_mqtt_client->status_mutex);
        return rc;
    }
    
    // 不手动设置状态为连接中，让回调函数处理状态更新
    // 这样可以避免状态不一致的问题
    
    return 0;
}

int mqtt_disconnect(void) {
    if (!g_mqtt_client || !g_mqtt_client->paho_client) {
        return -1;
    }
    
    MQTTAsync_disconnectOptions opts = MQTTAsync_disconnectOptions_initializer;
    opts.onSuccess = on_disconnect;
    opts.context = g_mqtt_client;
    
    int rc = MQTTAsync_disconnect(g_mqtt_client->paho_client, &opts);
    if (rc != MQTTASYNC_SUCCESS) {
        return rc;
    }
    
    return 0;
}

int mqtt_publish(const char* topic, const char* payload, 
                 int payload_len, int qos, int retained) {
    if (!g_mqtt_client || !g_mqtt_client->paho_client || !topic) {
        return -1;
    }
    
    MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;
    opts.onSuccess = on_publish;
    opts.context = g_mqtt_client;
    
    // 检查payload参数
    if (!payload) {
        return -1;
    }
    
    int rc = MQTTAsync_send(g_mqtt_client->paho_client, topic, (payload_len > 0) ? payload_len : (int)strlen(payload),
                            payload, qos, retained, &opts);
    return rc;
}

int mqtt_subscribe(const char* topic, int qos) {
    if (!g_mqtt_client || !g_mqtt_client->paho_client || !topic) {
        return -1;
    }
    
    MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;
    opts.onSuccess = on_subscribe;
    opts.context = g_mqtt_client;
    
    return MQTTAsync_subscribe(g_mqtt_client->paho_client, topic, qos, &opts);
}

int mqtt_unsubscribe(const char* topic) {
    if (!g_mqtt_client || !g_mqtt_client->paho_client || !topic) {
        return -1;
    }
    
    MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;
    opts.onSuccess = on_unsubscribe;
    opts.context = g_mqtt_client;
    
    return MQTTAsync_unsubscribe(g_mqtt_client->paho_client, topic, &opts);
}

mqtt_status_t mqtt_get_status(void) {
    if (!g_mqtt_client) {
        return MQTT_STATUS_ERROR;
    }
    
    // 如果没有创建 Paho 客户端，返回断开状态
    if (!g_mqtt_client->paho_client) {
        return MQTT_STATUS_DISCONNECTED;
    }
    
    // 优先从原始客户端对象查询连接状态
    int is_connected = MQTTAsync_isConnected(g_mqtt_client->paho_client);
    
    if (is_connected) {
        // Paho 客户端已连接，更新缓存状态并返回
        pthread_mutex_lock(&g_mqtt_client->status_mutex);
        g_mqtt_client->status = MQTT_STATUS_CONNECTED;
        pthread_mutex_unlock(&g_mqtt_client->status_mutex);
        return MQTT_STATUS_CONNECTED;
    } else {
        // Paho 客户端未连接，检查缓存状态
        pthread_mutex_lock(&g_mqtt_client->status_mutex);
        mqtt_status_t cached_status = g_mqtt_client->status;
        pthread_mutex_unlock(&g_mqtt_client->status_mutex);
        
        // 如果缓存状态是连接中，检查是否超时
        if (cached_status == MQTT_STATUS_CONNECTING) {
            // 检查连接超时（30秒超时）
            time_t current_time = time(NULL);
            time_t connect_start_time;
            pthread_mutex_lock(&g_mqtt_client->status_mutex);
            connect_start_time = g_mqtt_client->connect_start_time;
            pthread_mutex_unlock(&g_mqtt_client->status_mutex);
            time_t connect_duration = current_time - connect_start_time;
            
            if (connect_duration > 30) { // 30秒超时
                // 连接超时，更新状态为错误
                pthread_mutex_lock(&g_mqtt_client->status_mutex);
                g_mqtt_client->status = MQTT_STATUS_ERROR;
                pthread_mutex_unlock(&g_mqtt_client->status_mutex);
                return MQTT_STATUS_ERROR;
            } else {
                // 仍在连接中
                return MQTT_STATUS_CONNECTING;
            }
        } else {
            // 更新缓存状态为断开连接
            pthread_mutex_lock(&g_mqtt_client->status_mutex);
            g_mqtt_client->status = MQTT_STATUS_DISCONNECTED;
            pthread_mutex_unlock(&g_mqtt_client->status_mutex);
            return MQTT_STATUS_DISCONNECTED;
        }
    }
}

int mqtt_loop(int timeout_ms) {
    (void)timeout_ms; // 未使用参数
    if (!g_mqtt_client || !g_mqtt_client->paho_client) {
        return -1;
    }
    
    // 对于异步客户端，这个函数主要用于处理网络事件
    // 异步客户端会自动处理网络事件，所以这里只需要返回成功
    return 0;
}

void mqtt_destroy_client(void) {
    pthread_mutex_lock(&g_client_mutex);
    
    if (!g_mqtt_client) {
        pthread_mutex_unlock(&g_client_mutex);
        return;
    }
    
    if (g_mqtt_client->paho_client) {
        if (g_mqtt_client->status == MQTT_STATUS_CONNECTED) {
            MQTTAsync_disconnectOptions opts = MQTTAsync_disconnectOptions_initializer;
            opts.timeout = 1000;
            MQTTAsync_disconnect(g_mqtt_client->paho_client, &opts);
        }
        MQTTAsync_destroy(&g_mqtt_client->paho_client);
    }
    
    // 清理消息队列
    pthread_mutex_lock(&g_mqtt_client->message_mutex);
    for (int i = 0; i < g_mqtt_client->message_queue_count; i++) {
        int idx = (g_mqtt_client->message_queue_head + i) % MAX_MESSAGE_QUEUE;
        if (g_mqtt_client->message_queue[idx].topic) {
            free(g_mqtt_client->message_queue[idx].topic);
        }
        if (g_mqtt_client->message_queue[idx].payload) {
            free(g_mqtt_client->message_queue[idx].payload);
        }
    }
    pthread_mutex_unlock(&g_mqtt_client->message_mutex);
    
    // 销毁互斥锁
    pthread_mutex_destroy(&g_mqtt_client->status_mutex);
    pthread_mutex_destroy(&g_mqtt_client->message_mutex);
    
    free(g_mqtt_client);
    g_mqtt_client = NULL;
    
    pthread_mutex_unlock(&g_client_mutex);
}

// 轮询接收消息（非阻塞）
int mqtt_receive_message(mqtt_message_t* message) {
    if (!g_mqtt_client || !message) {
        return -1;
    }
    
    pthread_mutex_lock(&g_mqtt_client->message_mutex);
    
    if (g_mqtt_client->message_queue_count == 0) {
        pthread_mutex_unlock(&g_mqtt_client->message_mutex);
        return 0; // 没有消息
    }
    
    // 获取队列头部的消息
    int idx = g_mqtt_client->message_queue_head;
    *message = g_mqtt_client->message_queue[idx];
    
    // 更新队列
    g_mqtt_client->message_queue_head = (g_mqtt_client->message_queue_head + 1) % MAX_MESSAGE_QUEUE;
    g_mqtt_client->message_queue_count--;
    
    pthread_mutex_unlock(&g_mqtt_client->message_mutex);
    return 1; // 有消息
}

// 轮询连接状态变化（非阻塞）
int mqtt_check_connection_change(mqtt_status_t* status) {
    if (!g_mqtt_client || !status) {
        return -1;
    }
    
    pthread_mutex_lock(&g_mqtt_client->status_mutex);
    
    if (!g_mqtt_client->status_changed) {
        pthread_mutex_unlock(&g_mqtt_client->status_mutex);
        return 0; // 状态未变化
    }
    
    *status = g_mqtt_client->status;
    g_mqtt_client->status_changed = 0;
    
    pthread_mutex_unlock(&g_mqtt_client->status_mutex);
    return 1; // 状态已变化
}

// 释放消息内存
void mqtt_free_message(mqtt_message_t* message) {
    if (!message) {
        return;
    }
    
    if (message->topic) {
        free(message->topic);
        message->topic = NULL;
    }
    
    if (message->payload) {
        free(message->payload);
        message->payload = NULL;
    }
    
    message->payload_len = 0;
    message->qos = 0;
    message->retained = 0;
}

const char* mqtt_get_error_string(int error_code) {
    switch (error_code) {
        case MQTTASYNC_SUCCESS:
            return "Success";
        case MQTTASYNC_FAILURE:
            return "Generic failure";
        case MQTTASYNC_DISCONNECTED:
            return "Client is disconnected";
        case MQTTASYNC_MAX_MESSAGES_INFLIGHT:
            return "Maximum number of messages in flight";
        case MQTTASYNC_BAD_UTF8_STRING:
            return "Bad UTF8 string";
        case MQTTASYNC_NULL_PARAMETER:
            return "NULL parameter";
        case MQTTASYNC_TOPICNAME_TRUNCATED:
            return "Topic name truncated";
        case MQTTASYNC_BAD_STRUCTURE:
            return "Bad structure";
        case MQTTASYNC_BAD_QOS:
            return "Bad QoS";
        case MQTTASYNC_SSL_NOT_SUPPORTED:
            return "SSL not supported";
        case MQTTASYNC_PERSISTENCE_ERROR:
            return "Persistence error";
        default:
            return "Unknown error";
    }
}

