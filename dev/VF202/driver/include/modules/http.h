#ifndef __VBAR_M_CHANNEL_HTTP_H__
#define __VBAR_M_CHANNEL_HTTP_H__

#include <stdio.h>
#include <vbar/debug.h>
#include <vbar/export.h>

/* 请求失败错误码 */
#define VBAR_M_CHANNEL_HTTP_ERROR_REQUEST_FAILED            -1
/* 请求中断错误码 */
#define VBAR_M_CHANNEL_HTTP_ERROR_REQUEST_INTERRUPTION      -2
/* 安装失败错误码 */
#define VBAR_M_CHANNEL_HTTP_ERROR_INSTALL_FAILED            -3

struct vbar_m_channel_http_ack {
    /* 接收到数据时会malloc，注意释放 */
    char *data;
    /* 数据长度 */
    size_t dlen;
};

struct vbar_m_channel_http_download {
    /* 下载文件的文件句柄 */
    int fd;
    /* http请求超时时间 */
    uint64_t timeout_ms;
    /* 用于查看是否需要终止当前下载 */
    bool (*cancel)(void *pdata);
    /* 用于存放下载任务句柄 */
    void *pdata;
    /* 用于处理下载文件回调, 工作是MD5校验、文件解压安装。若仅想使用下载文件功能，无需解压可以不用做处理 */
    int (*deal_result)(bool http_ok, int http_code, void *deal_pdata);
    /* 用于存放安装文件时所需参数 */
    void *deal_pdata;
    /* 检测当前下载进度的回调 */
    void (*progress_callback)(void *pdl, long long dltotal, long long dlnow, long long ultotal, long long ulnow);
};

/* http 请求方法枚举 */
enum vbar_m_channel_http_req_method {
    /* HTTP 的 POST 请求方法 */
    VBAR_M_CHANNEL_HTTP_REQ_POST,
    /* HTTP 的 GET 请求方法 */
    VBAR_M_CHANNEL_HTTP_REQ_GET,
};

/* http 请求时，所需参数结构体 */
struct vbar_m_channel_http_params {
    /* http请求路径 */
    char url[512];
    /* dns解析地址 */
    char dns[128];
    /* 请求超时时间，注意需大于0 */
    size_t timeout_ms;
    /* http 请求方法 */
    enum vbar_m_channel_http_req_method method;
};


/* http 请求所需回调以及回调的入参 */
struct vbar_m_channel_http_cb {
    /* 组建 http 请求头 */
    void (*build_headers)(unsigned long, void **header, void *pdata);
    void *header_pdata;

    /* 私有 curl 参数设置，可以覆盖原有的设置 */
    void (*private_settings)(unsigned long, void *);
    void *private_pdata;

    /* 回写处理 http 接收到的流数据 */
    size_t (*write_callback)(void *write_pdata, size_t sz, size_t nmemb, void *ResInfo);
    void *write_pdata;

    /* http 请求结束后的处理，包括请求成功/失败后动作，响应数据的处理 */
    int (*deal_result_callback)(bool http_ok, int http_code, void *deal_pdata);
    void *deal_pdata;

    /* 数据传输进度回调 */
    int (*progress_callback)(void *clientp, long long dltotal, long long dlnow, long long ultotal, long long ulnow);
    void *progress_pdata;

    /* 请求前检查网络状态是否正常 */
    bool (*check_net_state)(void *pdata);
    void *net_pdata;
};


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 向请求头部，添加新的字符串信息
 *
 * @param headers   请求头句柄
 * @param text      字符串信息
 *
 * @return  请求头句柄
 */
VBARLIB_API struct curl_slist *vbar_m_channel_http_header_append(struct curl_slist *headers, const char *text);

/**
 * @brief   用于发起http请求处理响应
 *
 * @param http_params   http参数结构体句柄
 * @param data          指向字节数组的常量指针, 表示POST请求的数据内容
 * @param dlen          数据长度
 * @param ack           用于接收响应数据
 * @param http_cb       指向各回调函数结构体指针(构建请求头回调，对http请求获取数据的写回调，请求结果以及下载文件处理的回调等)
 *
 * @return
 */
VBARLIB_API int vbar_m_channel_http_request(struct vbar_m_channel_http_params *http_params, const uint8_t *data,
                                            size_t dlen, struct vbar_m_channel_http_ack *ack,
                                            struct vbar_m_channel_http_cb *http_cb);

/**
 * @brief   http_request接口的再一次封装，对http请求所需参数，以及各回调参数的注册
 *
 * @param url   http请求路径
 * @param dns   dns解析地址
 * @param dl    文件下载所需的各参数以及各回调函数
 *
 * @return
 */
VBARLIB_API int vbar_m_channel_http_download_file(const char *url, const char *dns, struct vbar_m_channel_http_download *dl);

#ifdef __cplusplus
}
#endif
#endif //__VBAR_M_CHANNEL_HTTP_H__
