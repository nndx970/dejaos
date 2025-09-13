#ifndef __VBAR_M_EID_H__
#define __VBAR_M_EID_H__

#include <vbar/export.h>
#include <jlreader/JLReader.h>

#define VBAR_M_EID_DATA_HEAD "___VBAR_ID_ACTIVE_V"

/* 表示 VBAR_M_EID 模块的操作执行成功 */
#define VBAR_M_EID_SUCCESS              (0)
/* 表示传递给函数的参数无效或不正确 */
#define VBAR_M_EID_ERR_PARAM            (-1)
/* 表示在操作过程中内存分配失败 */
#define VBAR_M_EID_ERR_MEM              (-2)
/* 表示操作在规定时间内未能完成，可能是因为等待响应超时或其他原因 */
#define VBAR_M_EID_ERR_TIMEOUT          (-3)
/* 表示在处理 JSON 数据时发生错误，如解析失败或生成无效的 JSON 内容 */
#define VBAR_M_EID_ERR_JSON             (-4)
/* 表示激活过程中的某个步骤失败，例如激活码无效或激活服务器不可用 */
#define VBAR_M_EID_ERR_ACTIVE           (-5)
/* 表示当前设备或资源正在被其他操作占用，无法立即处理新的请求 */
#define VBAR_M_EID_ERR_BUSY             (-6)
/* 表示发生了未具体分类的错误，通常用于捕捉未知或未预见的错误情况 */
#define VBAR_M_EID_ERR_CODE             (-7)
/* 表示初始化过程中发生错误，可能是由于配置文件缺失、初始化参数无效等原因 */
#define VBAR_M_EID_ERR_JLINIT           (-8)
/* 表示注册回调函数时发生错误 */
#define VBAR_M_EID_ERR_REGISTER_CB      (-9)

/* HTTP请求参数 */
struct vbar_m_eid_http_param {
    /* url 请求地址 */
    const char *url;
    /* sdata 请求数据，格式：Content-Type:application/json */
    const unsigned char *sdata;
    /* sdlen 请求数据长度 */
    unsigned int sdlen;
    /* timeout_ms 请求超时时间 */
    int timeout_ms;
};

/* HTTP应答 */
struct vbar_m_eid_http_ack {
    /* data http 应答数据 */
    char data[1024];
    /* dlen http 应答数据长度 */
    int dlen;
};

/* 设备激活 */
struct vbar_m_eid_active_param {
    /* 设备 sn */
    char meId[128];
    /* 设备 应用版本号 */
    char version[128];
    /* 设备 网卡MAC */
    char macAddr[32];
    /* 激活码内容 包含码头 VBAR_M_EID_DATA_HEAD */
    char codeMsg[256];

    /*
    * http_send 调用者http请求方法
    * pdata 回调私有数据 为 struct vbar_m_eid_active_param::pdata
    * param http请求参数
    * ack http应答参数
    * return 失败：错误码 VBAR_M_EID_ERR_XXX, 成功返回：VBAR_M_EID_SUCCESS
    **/
    int (*http_send)(void *pdata, const struct vbar_m_eid_http_param *param, struct vbar_m_eid_http_ack *ack);
    void *pdata;
};

struct vbar_m_eid_param {
    /* 平台分配给应用的appid */
    char appid[128];
    /* 单次读卡长度，默认0x80 */
    int read_len;
    /* 是否读取照片，1为不读取，2为读取 */
    int declevel;
    /* 日志级别，支持0，1，2 */
    int loglevel;
    /* 是否直接查出信息 0是  1否 （即0是原路返回，返回身份信息，1是转发，返回reqid） */
    int model;
    /* 卡片类型：0 身份证 1电子证照 */
    int type;
    /* 照片解码数据类型 0 wlt 1 jpg */
    int pic_type;
    /* 环境识别码 */
    int envCode;
    /* 设备序列号 */
    char sn[128];
    /* 设备型号 */
    char device_model[128];
    /* 信息返回类型，0 身份信息结构体 ，1原始数据 char */
    int info_type;
};

/* vbar_m_eid_param 结构体参数初始化器 */
#define VBAR_M_EID_PARAM_INITIALIZER {{0}, 0xF0, 2, 0, 0, 0, 1, 26814, {0}, {0}, 0}

/* 该结构体用于存储回调函数及其相关数据指针，适用于 NFC APDU 和 TCP 客户端通信 */
struct vbar_m_eid_cb {
    /* 处理 NFC APDU（应用协议数据单元）相关的事件或响应的回调函数 */
    card_callback nfc_apdu;
    /* 指向 NFC APDU 相关数据的指针 */
    void *npdata;

    /* 处理 TCP 客户端通信相关的事件或响应 */
    socket_callback tcp_client;
    /* 指向 TCP 客户端相关数据的指针 */
    void *tpdata;
};

/* vbar_m_eid_cb 结构体参数初始化器 */
#define VBAR_M_EID_CB_INITIALIZER {NULL, NULL, NULL, NULL}

#ifdef  __cplusplus
extern "C" {
#endif

/**
 * @brief  vbar_m_eid_active 云证激活接口
 *
 * @param param 激活需要的参数详见 struct vbar_m_eid_active_param
 * @param id_pic 激活类型，1：带照片， 0：不带照片
 *
 * @return 成功返回：VBAR_M_EID_SUCCESS，失败返回：VBAR_M_EID_ERR_XXX
 */
VBARLIB_API int vbar_m_eid_active(const struct vbar_m_eid_active_param *param, int *id_pic);

/**
 * @brief  vbar_m_eid_read_info 云证读取身份证信息
 *
 * @param param 激活需要的参数详见 struct vbar_m_eid_param
 * @param info 身份信息结构体 ，返回数据, 可为 idinfo * 或 char *
 *
 * @return 成功返回：VBAR_M_EID_SUCCESS，失败返回：VBAR_M_EID_ERR_XXX
 */
VBARLIB_API int vbar_m_eid_read_info(struct vbar_m_eid_param *param,
                                     const struct vbar_m_eid_cb *cb, void *info);

/**
 * @brief  vbar_m_eid_version 云证接口版本
 *
 * @param param 激活需要的参数详见 struct vbar_m_eid_param
 * @param info 读取到的身份证信息存入此参数
 *
 * @return 成功返回：版本号，失败返回：NULL
 */
VBARLIB_API const char *vbar_m_eid_version(void);

/**
 * @brief 打印电子证照模块参数配置详情
 *
 * @param param 需要调试输出的参数结构体指针
 *
 * 该函数以易读格式输出vbar_m_eid_param结构体的全部配置参数，用于调试时验证参数设置。
 * 当传入NULL指针时会输出错误信息并立即返回。
 *
 * @note 输出内容通过stderr流打印，包含以下信息：
 * - 应用标识(appid)
 * - 读卡长度(read_len)十六进制显示
 * - 照片读取标志(declevel)
 * - 日志级别(loglevel)
 * - 工作模式(model)
 * - 证照类型(type)
 * - 照片编码格式(pic_type)
 * - 环境识别码(envCode)
 * - 设备序列号(sn)
 * - 设备型号(device_model)
 * - 返回数据类型(info_type)
 *
 * @see struct vbar_m_eid_param
 */
VBARLIB_API void vbar_m_eid_param_dump(struct vbar_m_eid_param *param);

#ifdef  __cplusplus
}
#endif

#endif//__VBAR_M_EID_H__

