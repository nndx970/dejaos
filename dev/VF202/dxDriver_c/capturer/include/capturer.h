#ifndef __VBAR_BASE_CAPTURER_H__
#define __VBAR_BASE_CAPTURER_H__

#include <stdint.h>
#include <pthread.h>
#include <linux/videodev2.h>

#include <vbar/export.h> /* for VBARLIB_API */

/*  图像采集设备ioctl命令请求，模块 ioctl 的函数原型为：
    VBARLIB_API int vbar_drv_capturer_ioctl(struct vbar_drv_capturer *capturer, int request, unsigned long arg); */
enum {
    VBAR_DRV_CAPTURER_IOC_S_REGISTER = 1,   /* 设置sensor寄存器值, arg: (unsigned long)const struct vbar_drv_capturer_reg *, 成功返回0 */
    VBAR_DRV_CAPTURER_IOC_G_REGISTER = 2,   /* 获取sensor寄存器值, arg: (unsigned long)struct vbar_drv_capturer_reg *, 成功返回0 */
    VBAR_DRV_CAPTURER_IOC_BEGIN = VBAR_DRV_CAPTURER_IOC_S_REGISTER,     /* 标识枚举开始 */
    VBAR_DRV_CAPTURER_IOC_END = VBAR_DRV_CAPTURER_IOC_G_REGISTER + 1,   /* 标识枚举结束 */
};


enum {  /* 定义 sensor 的型号 */
    VBAR_DRV_CAPTURER_SENSOR_TYPE_UNKNOWN = 0,
    VBAR_DRV_CAPTURER_SENSOR_TYPE_SC031GS = 1,
    VBAR_DRV_CAPTURER_SENSOR_TYPE_OV7251  = 2,
    VBAR_DRV_CAPTURER_SENSOR_TYPE_OV7725  = 3,
    VBAR_DRV_CAPTURER_SENSOR_TYPE_SC132GS = 4,
    VBAR_DRV_CAPTURER_SENSOR_TYPE_SC230AI = 5,
    VBAR_DRV_CAPTURER_SENSOR_TYPE_SC201CS = 6,
    VBAR_DRV_CAPTURER_SENSOR_TYPE_IMX307  = 7,
    VBAR_DRV_CAPTURER_SENSOR_TYPE_GC2145  = 8
};

struct vbar_drv_capturer_reg {    /* 寄存器结构体 */
    unsigned int reg;   /* 寄存器地址 */
    unsigned int val;   /* 寄存器值 */
};

struct vbar_drv_image_mplane {
    uint8_t mplane_num;
    uint8_t *addr[3];
    uint8_t *phyaddr[3];
    uint32_t length[3];
    uint32_t dmafd[3];
};

enum vbar_drv_image_layout_type {
    VBAR_DRV_IMAGE_LAYOUT_CAPTURER,
    VBAR_DRV_IMAGE_LAYOUT_CAPTURER_PLANE
};

struct vbar_drv_image {   /* 图像结构定义 */
    enum vbar_drv_image_layout_type layout_type;
    uint32_t width;           /* 水平像素度(长) */
    uint32_t height;          /* 垂直像素度(宽) */
    uint32_t widthbytes;      /* 位深，标识使用几个 bit 表示灰度 */
    unsigned char *data;      /* 原始数据 */
    unsigned long datalen;    /* 原始数据长度 */
    struct vbar_drv_image_mplane mplane;
    uint64_t timestamp;       /* 时间戳 */
    void *pdata;
};

struct vbar_drv_capturer_config {
    const char *path;
    uint32_t width;
    uint32_t height;
    uint32_t widthbytes;
    uint32_t sensor_type;
    uint32_t pixel_format;
    uint32_t rotation;
    uint8_t  frame_num;
    uint8_t  frame_rate;
    uint8_t  preview_enable;
    uint32_t preview_left;
    uint32_t preview_top;
    uint32_t preview_width;
    uint32_t preview_height;
    uint32_t preview_rotation;
    uint32_t preview_mode;
    uint32_t preview_screen_index;
};

struct vbar_drv_capturer {    /* b-capture 句柄 */
    uint32_t type;          /* sensor type 从内核获取 驱动需要添加相关的支持*/
    uint8_t name[16];       /* sensor name 从内核获取 驱动需要添加相关的支持*/
    uint32_t width;         /* 水平像素度(长) */
    uint32_t height;        /* 垂直像素度(宽) */
    uint32_t widthbytes;    /* 位深，标识使用几个 bit 表示灰度 */
    uint32_t pixel_format;  /* 像素格式 */
    struct vbar_drv_image *(*read)(struct vbar_drv_capturer *);         /* 读取图像数据的回调指针 */
    int (*streamon)(struct vbar_drv_capturer *, bool);                /* 使能/关闭 sensor 数据流的回调指针 */
    int (*ioctl)(struct vbar_drv_capturer *, int, unsigned long);     /* 视频流驱动 ioctl 接口 */
    void (*close)(struct vbar_drv_capturer *);                        /* 销毁本句柄的回调指针 */
    void *private_data;     /* 私有数据 */
};

#ifdef	__cplusplus
extern "C" {
#endif

/* 宏函数，调用 handle 在创建时注册的 ioctl 函数 */
#define _capturer_ioctl_(capturer, request, arg) \
            (((capturer) && (capturer)->ioctl) ? (capturer)->ioctl((capturer), (request), (arg)): -1)

/**
* @brief vbar_drv_capturer_open 打开图像采集设备
*
* @param config struct vbar_drv_capturer_config
*
* @return 成功：capturer 指针，失败：NULL
*/
VBARLIB_API struct vbar_drv_capturer *vbar_drv_capturer_open(struct vbar_drv_capturer_config *config);

/**
 * @brief vbar_drv_capturer_close 关闭图像采集设备
 *
 * @param capturer  采集设备句柄指针
 *
 * @return 无
 */
VBARLIB_API void vbar_drv_capturer_close(struct vbar_drv_capturer *capturer);


/**
 * @brief vbar_drv_capturer_streamon 开始/停止采集
 *
 * @param capturer    采集设备句柄指针
 * @param on          开始/停止
 *
 * @return 成功：0， 失败：-1
 */
VBARLIB_API int vbar_drv_capturer_streamon(struct vbar_drv_capturer *capturer, bool on);

/**
 * @brief vbar_drv_capturer_ioctl 图像采集设备控制接口
 *
 * @param capturer            采集设备句柄指针
 * @param request             ioctl 类型
 * @param arg                 ioctl 参数
 *
 * @return 成功：0， 失败：-1
 */
VBARLIB_API int vbar_drv_capturer_ioctl(struct vbar_drv_capturer *capturer, int request, unsigned long arg);

/**
 * @brief vbar_drv_capturer_read 读取指定channel的数据
 *
 * @param capturer           图像采集设备控制接口
 *
 * @return 成功：返回图像指针，失败：NULL
 */
VBARLIB_API struct vbar_drv_image *vbar_drv_capturer_read(struct vbar_drv_capturer *capturer);

/**
 * @brief vbar_drv_capturer_read 创建图像
 *
 * @param data             图像数据
 * @param width            图像宽度
 * @param widthbytes       图像位深
 * @param height           图像高度
 *
 * @return 成功：返回图像指针，失败：NULL
 */
VBARLIB_API struct vbar_drv_image *vbar_drv_capturer_image_create(const unsigned char *data,
                                                     uint32_t width, uint32_t widthbytes, uint32_t height);

/**
 * @brief vbar_drv_capturer_image_create_plane 创建图像
 *
 * @param plane            vbar_drv_image_plane图像数据结构
 * @param width            图像宽度
 * @param widthbytes       图像位深
 * @param height           图像高度
 *
 * @return 成功：返回图像指针，失败：NULL
 */
VBARLIB_API struct vbar_drv_image *vbar_drv_capturer_image_create_mplane(const struct vbar_drv_image_mplane *mplane,
                                                     uint32_t width, uint32_t widthbytes, uint32_t height);


/**
 * @brief vbar_drv_capturer_read 拷贝图像
 *
 * @param image     待拷贝图像
 *
 * @return 成功：返回图像指针，失败：NULL
 */
VBARLIB_API struct vbar_drv_image *vbar_drv_capturer_image_copy(struct vbar_drv_image *image);

/**
 * @brief vbar_drv_capturer_read 销毁图像
 *
 * @param image           待销毁图像
 *
 * @return 无
 */
VBARLIB_API void vbar_drv_capturer_image_destroy(struct vbar_drv_image *image);


/**
 * @brief 使能/关闭capture预览
 *
 * @param capturer  期望进行使能/关闭的capture对象
 * @param enable    期望设置使能还是关闭
 *
 * @return 成功：0；失败：-1
 */
VBARLIB_API int vbar_drv_capturer_preview_enable(struct vbar_drv_capturer *capturer, bool enable);

VBARLIB_API int vbar_drv_capturer_mplane_set_data(struct vbar_drv_image_mplane *mplane, uint32_t index, uint8_t *data, uint32_t datalen);

VBARLIB_API struct vbar_drv_image_mplane *vbar_drv_capturer_mplane_create(uint32_t width, uint32_t height, uint32_t format);

VBARLIB_API void vbar_drv_capturer_mplane_destroy(struct vbar_drv_image_mplane *mplane);
#ifdef __cplusplus
}
#endif

#endif /* __VBAR_CAPTURER_H__ */
