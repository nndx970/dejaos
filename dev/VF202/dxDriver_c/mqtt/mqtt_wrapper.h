#ifndef MQTT_WRAPPER_H
#define MQTT_WRAPPER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

// MQTT连接状态
typedef enum {
    MQTT_STATUS_DISCONNECTED = 0,
    MQTT_STATUS_CONNECTING,
    MQTT_STATUS_CONNECTED,
    MQTT_STATUS_ERROR
} mqtt_status_t;

// MQTT消息结构
typedef struct {
    char* topic;
    char* payload;
    int payload_len;
    int qos;
    int retained;
} mqtt_message_t;

// 创建MQTT客户端
int mqtt_create_client(const char* client_id);

// 设置连接参数和认证信息
int mqtt_set_connection_params(const char* host, 
                              int port, 
                              int keepalive_interval,
                              int clean_session,
                              const char* username,
                              const char* password,
                              const char* will_topic,
                              const char* will_message,
                              int will_qos,
                              int will_retained);

// 连接到MQTT服务器
int mqtt_connect(void);

// 主动重连（使用之前的连接参数）
int mqtt_reconnect(void);

// 断开连接
int mqtt_disconnect(void);

// 发布消息
int mqtt_publish(const char* topic, 
                 const char* payload, 
                 int payload_len, 
                 int qos, 
                 int retained);

// 订阅主题
int mqtt_subscribe(const char* topic, int qos);

// 取消订阅
int mqtt_unsubscribe(const char* topic);

// 获取连接状态
mqtt_status_t mqtt_get_status(void);

// 处理网络事件（需要在主循环中调用）
int mqtt_loop(int timeout_ms);

// 销毁客户端
void mqtt_destroy_client(void);

// 轮询接收消息（非阻塞）
int mqtt_receive_message(mqtt_message_t* message);

// 轮询连接状态变化（非阻塞）
int mqtt_check_connection_change(mqtt_status_t* status);

// 释放消息内存（用于mqtt_receive_message获取的消息）
void mqtt_free_message(mqtt_message_t* message);

// 获取错误信息
const char* mqtt_get_error_string(int error_code);

#ifdef __cplusplus
}
#endif

#endif // MQTT_WRAPPER_H
