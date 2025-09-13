#ifndef __VBAR_M_VGMQTT_H__
#define __VBAR_M_VGMQTT_H__

#include <vbar/export.h>
#include <sqlite/sqlite3.h>
#include <mqtt/MQTTClient.h>
#include <mqtt/MQTTAsync.h>

#define VBAR_M_MQTT_DEFAULT_CLIENT_ID    "default_client_id"

struct vbar_m_mqtt {
    MQTTAsync mqtt_h;
};

#define VBAR_M_MQTT_URI_TCP     "tcp://"
#define VBAR_M_MQTT_URI_MQTT    "mqtt://"
#define VBAR_M_MQTT_URI_WS      "ws://"

#define VBAR_M_MQTT_URI_SSL     "ssl://"
#define VBAR_M_MQTT_URI_MQTTS   "mqtts://"
#define VBAR_M_MQTT_URI_WSS     "wss://"

enum ssl_auth_type {
    /*
    * 根据服务地址选择是否启用ssl加密,不做认证
    * 1.不启用SSL addr头：
    *    VBAR_M_MQTT_URI_TCP
    *    VBAR_M_MQTT_URI_MQTT
    *    VBAR_M_MQTT_URI_WS
    * 2.启用SSL不做认证 addr头：
    *    VBAR_M_MQTT_URI_SSL
    *    VBAR_M_MQTT_URI_MQTTS
    *    VBAR_M_MQTT_URI_WSS
    */
    VBAR_M_MQTT_DISABLE_SSL = 0,
    /* 启用ssl加密不做认证 */
    VBAR_M_MQTT_SSL_NO_AUTH = 1,
    /* 单向认证 */
    VBAR_M_MQTT_SSL_ONE_WAY = 2,
    /* 双向认证 */
    VBAR_M_MQTT_SSL_TWO_WAY = 3,
};

struct ssl {
    enum ssl_auth_type ssl_auth_type;
    /* PEM格式的文件，包含客户端信任的公共数字证书 */
    char trust_store[128];

    /* PEM格式的文件，包含客户端的公共证书链。它还可能包括客户端的私钥 */
    char key_store[128];

    /*  如果未包含在 key_store 中，则此设置指向包含客户端私钥的PEM格式的文件 */
    char private_key[128];
    char ssl_ip[16];
    char ssl_domain[255];
};

struct vbar_m_mqtt_config {
    char addr[256];
    char client_id[256];
    char user_name[256];
    char user_password[256];
    char topic_prefix[256];
    char qos;
    struct ssl ssl;
};

enum vbar_mqtt_status {
    VBAR_MQTT_DISCONNECT,
    VBAR_MQTT_CONNECTED,
};

struct vbar_m_mqtt_ops {
    bool (*is_net_connected)(void *npdate);
    void *npdate;

    int (*subscribe_topics)(const struct vbar_m_mqtt_config *config, void *tpdata);
    void *tpdata;

    int (*msgarrvd)(const char *topicName, int topicLen,const MQTTAsync_message *message,
                    const struct vbar_m_mqtt_config *config, void *mpdata);
    void *mpdata;

    int (*status_change)(enum vbar_mqtt_status sta, void *spdata);
    void *spdata;


    int (*will_ops_init)(MQTTAsync_connectOptions *conn_opts, MQTTAsync_willOptions *will_opts,
                        const struct vbar_m_mqtt_config *config, void *wpdata);
    void *wpdata;
};

struct vbar_m_mqtt_msg {
    char *topic;
    int topiclen;
    char *data;
    int dlen;

    void (*on_success)(void* context, MQTTAsync_successData* response);
    void (*on_failure)(void* context, MQTTAsync_failureData* response);
	void *context;
};


#ifdef  __cplusplus
extern "C" {
#endif

VBARLIB_API void vbar_mqtt_get_str_serialno(char *out, size_t out_size);

VBARLIB_API struct vbar_m_mqtt *vbar_m_mqtt_init(const struct vbar_m_mqtt_config *config, const struct vbar_m_mqtt_ops *ops);

VBARLIB_API void vbar_m_mqtt_deinit(struct vbar_m_mqtt *client);
VBARLIB_API int vbar_m_mqtt_recreate(struct vbar_m_mqtt *client);

VBARLIB_API int vbar_m_mqtt_subscribe(struct vbar_m_mqtt *client, const char *topic, int qos);

VBARLIB_API int vbar_m_mqtt_subscribe_manytopic(struct vbar_m_mqtt *client, char * const*topicarry, int amount, int qos);

VBARLIB_API enum vbar_mqtt_status vbar_m_mqtt_status(struct vbar_m_mqtt *client);

VBARLIB_API int vbar_m_mqtt_send_msg(struct vbar_m_mqtt *client, const struct vbar_m_mqtt_msg *msg);

VBARLIB_API int vbar_m_mqtt_async_send_msg(struct vbar_m_mqtt *client, const struct vbar_m_mqtt_msg *msg);

VBARLIB_API int vbar_m_mqtt_get_config(struct vbar_m_mqtt *client, struct vbar_m_mqtt_config *config);

VBARLIB_API int vbar_m_mqtt_set_config(struct vbar_m_mqtt *client, const struct vbar_m_mqtt_config *config);

#ifdef  __cplusplus
}
#endif

#endif//__VBAR_M_VGMQTT_H__

