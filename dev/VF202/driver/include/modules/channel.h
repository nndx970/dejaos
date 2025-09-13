#ifndef __VBAR_M_CHANNEL_H__
#define __VBAR_M_CHANNEL_H__

#include <unistd.h>
#include <stddef.h>
#include <stdint.h>

#include <vbar/export.h>

/* 通道类型定义 for vbar_channel_open */
enum vbar_m_channel_type {
    /* 表示使用USB接口的键盘通信通道 */
    VBAR_M_CHANNEL_TYPE_USBKBW              = 1,
    /* 表示使用USB接口的HID（人机交互接口设备）通信通道 */
    VBAR_M_CHANNEL_TYPE_USBHID              = 2,
    /* 表示使用UART（通用异步收发传输器）通信通道 */
    VBAR_M_CHANNEL_TYPE_UART                = 3,
	/* 表示使用韦根协议的通信通道 */
    VBAR_M_CHANNEL_TYPE_WIEGAND             = 4,
    /* 表示使用TCP协议的服务器端通信通道 */
    VBAR_M_CHANNEL_TYPE_TCPSERVER           = 5,
    /* 表示使用TCP协议的客户端通信通道 */
    VBAR_M_CHANNEL_TYPE_TCPCLIENT           = 6,
};

/* 请求定义 for vbar_channel_ioctl */

/* 各类通道 IO 控制操作的设置选项枚举 */
enum vbar_m_channel_ioc_set_cmd {
    /* 设置KBW通道的配置参数 */
    VBAR_M_CHANNEL_IOC_SET_KBW_CONFIG      = 1,
    /* 设置KBW通道的上位机参数 */
    VBAR_M_CHANNEL_IOC_SET_KBW_UPPER       = 2,
    /* KBW上线时间 */
    VBAR_M_CHANNEL_IOC_SET_KBW_UPTIME      = 3,
    /* KBW下线时间 */
    VBAR_M_CHANNEL_IOC_SET_KBW_DOWNTIME    = 4,
    /* 设置HID通道的报告长度 */
    VBAR_M_CHANNEL_IOC_SET_HID_REPORT_LEN  = 5,
    /* 设置UART通道的参数 */
    VBAR_M_CHANNEL_IOC_SET_UART_PARAM      = 6,
    /* 设置韦根通道的工作模式 */
    VBAR_M_CHANNEL_IOC_SET_WIEGAND_MODE    = 7,
    /* 设置韦根通道的GPIO配置 */
    VBAR_M_CHANNEL_IOC_SET_WIEGAND_GPIO    = 8,
    /* 设置韦根通道的延迟时间 */
    VBAR_M_CHANNEL_IOC_SET_WIEGAND_DELAY   = 9,
    /* 设置韦根通道的日志记录功能 */
    VBAR_M_CHANNEL_IOC_SET_WIEGAND_LOG     = 10

};

/* 韦根通道的不同工作模式 */
enum vbar_m_channel_wiegand_mode {
    /* 韦根模式初始化值 */
    VBAR_M_CHANNEL_WIEGAND_MODE_INIT      = 0,
    /* 韦根 26 位模式 */
    VBAR_M_CHANNEL_WIEGAND_MODE_26		  = 1,
    /* 韦根 34 位模式 */
    VBAR_M_CHANNEL_WIEGAND_MODE_34		  = 2,
    /* 韦根 128 位模式 */
    VBAR_M_CHANNEL_WIEGAND_MODE_128       = 3,
    /* 韦根 256 位模式 */
    VBAR_M_CHANNEL_WIEGAND_MODE_256       = 4,
    /* 韦根 2048 位模式 */
    VBAR_M_CHANNEL_WIEGAND_MODE_2048      = 5,
    /* 自定义的韦根模式, 最大发送 6400 位 */
    VBAR_M_CHANNEL_WIEGAND_MODE_CUSTOM    = 6
};

struct vbar_m_channel_wiegand_ioctl_param {
    /* wiegand 输出模式 */
    enum vbar_m_channel_wiegand_mode mode;
    /* 输出引脚忙延时时间 */
    int busy_time;
    /* 输出引脚闲延时时间 */
    int free_time;
    /* D0 gpio */
    int wiegand_d0;
    /* D1 gpio */
    int wiegand_d1;
    /* 1:输出log， 0:关闭log */
    int log_level;
};

/* 信道结构 */
struct vbar_m_channel_handle {
    uint32_t type;
    int (*send)(struct vbar_m_channel_handle *, const unsigned char *, size_t);
    int (*recv)(struct vbar_m_channel_handle *, unsigned char *, size_t, int);
    int (*ioctl)(struct vbar_m_channel_handle *, enum vbar_m_channel_ioc_set_cmd, unsigned long);
	int (*flush)(struct vbar_m_channel_handle *, int);
    void (*close)(struct vbar_m_channel_handle *);
    void *private_data;
};

#ifdef	__cplusplus
extern "C" {
#endif

/**
 * @brief 打开指定类型的通道, 返回一个指向通道结构的指针。
 *
 * @param type 通道类型，参见 `enum vbar_m_channel_type`。
 * @param arg 通道的附加参数, 具体含义取决于通道类型。
 *
 * @return 成功时返回指向打开的通道结构的指针, 失败时返回 NULL。
 */
VBARLIB_API struct vbar_m_channel_handle *vbar_m_channel_open(int type, unsigned long arg);

/**
 * @brief 通过指定的通道发送数据
 *
 * @param channel 指向要使用的通道结构的指针。
 * @param buffer 指向要发送的数据缓冲区的指针。
 * @param length 要发送的数据长度（以字节为单位）。
 *
 * @return 成功时返回 0, 失败时返回负值错误码。
 */
VBARLIB_API int vbar_m_channel_send(struct vbar_m_channel_handle *channel,
                                  const unsigned char *buffer, size_t length);
/**
 * @brief 从指定的通道接收数据，并将其存储到提供的缓冲区中。
 *
 * @param channel 指向要使用的通道结构的指针。
 * @param buffer 指向接收数据的缓冲区的指针。
 * @param size 缓冲区的大小（以字节为单位）。
 * @param milliseconds 等待数据的最大时间（以毫秒为单位）。
 *
 * @return 成功时返回接收到的数据长度（以字节为单位），超时或失败时返回负值错误码。
 */
VBARLIB_API int vbar_m_channel_recv(struct vbar_m_channel_handle *channel,
                                  unsigned char *buffer, size_t size, int milliseconds);
/**
 * @brief 控制指定通道的IO操作, 通常用于配置通道或获取通道的状态信息。
 *
 * @param channel 指向要操作的通道结构的指针。
 * @param request IO控制请求的类型，参见 `enum vbar_m_channel_ioc_set_cmd`。
 * @param arg 请求的附加参数，具体含义取决于请求类型。
 *
 * @return 成功时返回 0，失败时返回负值错误码。
 */
VBARLIB_API int vbar_m_channel_ioctl(struct vbar_m_channel_handle *channel, enum vbar_m_channel_ioc_set_cmd request, unsigned long arg);

/**
 * @brief 关闭指定的通道
 *
 * 此函数用于关闭指定的通道，并释放相关资源。
 *
 * @param channel 指向要关闭的通道结构的指针。
 */
VBARLIB_API void vbar_m_channel_close(struct vbar_m_channel_handle *channel);

/**
 * @brief 清空指定通道的缓冲区
 *
 * 此函数用于清空指定通道的输入或输出缓冲区。
 *
 * @param channel 指向要操作的通道结构的指针。
 * @param queue_selector 队列选择器，用于指定要清空的缓冲区在发送或接收队列
 *
 * @return 成功时返回 0，失败时返回负值错误码。
 */
VBARLIB_API int vbar_m_channel_flush(struct vbar_m_channel_handle *channel, int queue_selector);
#ifdef __cplusplus
}
#endif

#endif /* __VBAR_M_CHANNEL_H__ */
