#ifndef __VBAR_M_CHANNEL_TCP_H__
#define __VBAR_M_CHANNEL_TCP_H__

/* for VBARLIB_API */
#include <vbar/export.h>

/* tcp通道的句柄 */
struct vbar_m_channel_tcp_handle;
/* tcp客户端的句柄 */
struct vbar_m_channel_tcp_client_handle;

/* TCP 通道的连接状态 */
enum vbar_m_channel_tcp_status {
    /* 客户端断开连接 */
    VBAR_M_CHANNEL_TCP_CLIENT_DISCONNECT,
    /* 客户端连接 */
    VBAR_M_CHANNEL_TCP_CLIENT_CONNECT,
};

/* TCP 通道参数结构体 */
struct vbar_m_channel_tcp_params {
    /* 服务器 IP 地址 */
    char ip[300];
    /* 服务器端口 */
    unsigned short port;
    /* 接收模式标志 */
    int recv_in_pro_mode;
    /* 请求超时时间，注意需大于0 */
    unsigned int timeout_ms;
    /**
     * @brief 请求前检查网络状态是否正常
     *
     * @param pdata 用户自定义的数据指针。
     *
     * @return 返回 true 表示网络状态正常，返回 false 表示网络状态异常。
     */
    bool (*check_net_state)(void *pdata);
    /* 传递给 `check_net_state` 回调函数的网络状态检查用户自定义数据指针 */
    void *net_pdata;
    /**
     * @brief 状态变化回调
     *
     * @param tcp_sta 当前的 TCP 通道状态。
     * @param pdata 用户自定义的数据指针。
     *
     * @return 成功时返回 0，失败时返回负值错误码。
     */
    int (*status_change)(enum vbar_m_channel_tcp_status tcp_sta, void *pdata);
    /* 状态变化回调的用户数据指针 */
    void *status_pdata;
};

/* TCP 通道接收数据结构体 */
struct vbar_m_channel_tcp_ack {
    /* 接收到数据时会malloc，注意释放 */
    unsigned char *data;
    /* 数据长度 */
    unsigned int dlen;
};

/* tcp 客户端配置结构体 */
struct vbar_m_channel_tcp_client_config {
    /* 是否启用 TCP 客户端。0 表示禁用，非 0 表示启用 */
    char enable;
    /* TCP 通道参数 */
    struct vbar_m_channel_tcp_params param;
    /* 是否启用心跳包机制。0 表示禁用，非 0 表示启用 */
    int heart_en;
    /* 心跳包发送的间隔时间，单位为秒 */
    int heart_time;
    /* 心跳包数据 */
    char heart_data[300];
};



#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 用于解析URL，并获取IP地址
 *
 * @param url       需要解析的URL
 * @param ip        存储解析得到的ip地址
 * @param iplen     ip字符串的最大长度
 * @param timeouts_ms DNS解析的超时时间
 *
 * @return -1 失败, 0 成功
 */
VBARLIB_API int vbar_m_channel_tcp_url_dns_parse(char *url, char *ip, uint32_t iplen, uint32_t timeouts_ms);

/**
 * @brief 创建channel_tcp句柄函数
 *
 * @param param 所需的参数结构体
 *
 * @return 成功:返回句柄， 失败:返回NULL
 */
VBARLIB_API struct vbar_m_channel_tcp_handle *vbar_m_channel_tcp_create(struct vbar_m_channel_tcp_params *param);

/**
 * @brief 销毁句柄函数
 *
 * @param handle 需要销毁的channel_tcp句柄
 *
 * @return
 */
VBARLIB_API void vbar_m_channel_tcp_destory(struct vbar_m_channel_tcp_handle *handle);

/**
 * @brief channel_tcp 发送函数
 *
 * @param handle channel_tcp句柄
 * @param sbuf   发送数据缓冲区
 * @param slen   发送数据缓冲区长度
 *
 * @return
 */
VBARLIB_API int vbar_m_channel_tcp_send(struct vbar_m_channel_tcp_handle *handle, const uint8_t *sbuf, int slen);

/**
 * @brief 接受函数
 *
 * @param handle    channel_tcp句柄
 * @param rbuf      接收数据缓冲区
 * @param rlen      数据缓冲区长度
 * @param timeout_ms 接收超时时间
 *
 * @return
 */
VBARLIB_API int vbar_m_channel_tcp_recv(struct vbar_m_channel_tcp_handle *handle, uint8_t *rbuf , int rlen, int timeout_ms);

/**
 * @brief 刷新tcp通道的接收缓冲区
 *
 * @param handle channel_tcp句柄
 *
 * @return
 */
VBARLIB_API int vbar_m_channel_tcp_flush(struct vbar_m_channel_tcp_handle *handle);

/**
 * @brief   tcp的发送和接收响应接口(若需要接收响应，ack参数在用完之后需要做内存释放)
 *
 * @param param 指向 TCP 参数结构体的指针
 * @param data  需要传输数据的指针
 * @param dlen  数据长度
 * @param ack   指向应答结构体的指针
 *
 * @return
 */
VBARLIB_API int vbar_m_channel_tcp_single_send(struct vbar_m_channel_tcp_params *param, const uint8_t *data,
                                               size_t dlen, struct vbar_m_channel_tcp_ack *ack);

/**
 * @brief 创建tcp客户端的句柄
 *
 * @param config 指向tcp客户端配置结构体的指针
 *
 * @return tcp客户端的句柄
 */
VBARLIB_API struct vbar_m_channel_tcp_client_handle *vbar_m_channel_tcp_client_create(struct vbar_m_channel_tcp_client_config *config);

/**
 * @brief 销毁tcp客户端的句柄
 *
 * @param tcp_client 需要销毁的客户端句柄
 *
 * @return
 */
VBARLIB_API void vbar_m_channel_tcp_client_destory(struct vbar_m_channel_tcp_client_handle *tcp_client);

/**
 * @brief 获取tcp客户端的配置
 *
 * @param tcp_client tcp客户端句柄
 * @param conifg     获取配置的结构体指针
 *
 * @return 0:成功   -1失败
 */
VBARLIB_API int vbar_m_channel_tcp_client_get_config(struct vbar_m_channel_tcp_client_handle *tcp_client, struct vbar_m_channel_tcp_client_config *conifg);

/**
 * @brief 设置配置
 *
 * @param tcp_client tcp客户端句柄
 * @param conifg     配置的结构体指针
 *
 * @return 0:成功  -1:失败
 */
VBARLIB_API int vbar_m_channel_tcp_client_set_config(struct vbar_m_channel_tcp_client_handle *tcp_client, struct vbar_m_channel_tcp_client_config *conifg);

/**
 * @brief 客户端发送
 *
 * @param tcp_client tcp客户端句柄
 * @param sbuf       发送缓冲区
 * @param slen       缓冲区大小
 *
 * @return 0:成功， -1:失败
 */
VBARLIB_API int vbar_m_channel_tcp_client_send(struct vbar_m_channel_tcp_client_handle *tcp_client, const uint8_t *sbuf , int slen);

/**
 * @brief 客户端接收
 *
 * @param tcp_client  tcp客户端句柄
 * @param rbuf        接收缓冲区
 * @param rlen        接收缓冲区大小
 * @param timeout_ms  接收超时时间
 *
 * @return 0:成功, -1:失败
 */
VBARLIB_API int vbar_m_channel_tcp_client_recv(struct vbar_m_channel_tcp_client_handle *tcp_client, uint8_t *rbuf, int rlen, int timeout_ms);


/**
 * @brief 客户端接受缓冲区刷新
 *
 * @param tcp_client tcp客户端句柄
 *
 * @return 0:成功, -1:失败
 */
VBARLIB_API int vbar_m_channel_tcp_client_flush(struct vbar_m_channel_tcp_client_handle *tcp_client);

/**
 * @brief 获取当前客户端是否连接
 *
 * @param tcp_client 客户端句柄
 *
 * @return true:连接， false:未连接
 */
VBARLIB_API bool vbar_m_channel_tcp_client_is_connect(struct vbar_m_channel_tcp_client_handle *tcp_client);

#ifdef __cplusplus
}
#endif
#endif //__VBAR_CHANNEL_TCP_H__
