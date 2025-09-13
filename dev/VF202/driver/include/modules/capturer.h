#ifndef __VBAR_M_CAPTURER_H__
#define __VBAR_M_CAPTURER_H__

#include <stdint.h>
#include <vbar/export.h>
#include <drivers/capturer.h>

#ifdef  __cplusplus
extern "C" {
#endif
/* 取图信息 */
struct vbar_m_capturer_info {
    /* 图像宽度 */
    uint32_t width;
    /* 每个像素所占字节数 */
    uint32_t widthbytes;
    /* 图像高度 */
    uint32_t height;
    /* 取图摄像头名称 */
    uint8_t name[32];
    /* 取图摄像头类型 */
    uint32_t type;
};

/* ISP 配置结构体 */
struct vbar_m_isp_config {
    /* isp drv config */
    /* 处理方法 */
    uint32_t method;
    /* 子处理方法 */
    uint32_t sub_method;
    /* 目标亮度值 */
    uint8_t target_luminance;
    /* 目标百分位数值 */
    uint32_t target_percentile;
    /* 采样间隔 */
    uint32_t sample_gap;
    /* 最小曝光时间 */
    uint32_t min_exp;
    /* 最大曝光时间 */
    uint32_t max_exp;
    /* ISP的最小增益值 */
    uint32_t min_gain;
    /* ISP的最大增益值 */
    uint32_t max_gain;
};

/* 取图摄像头配置 */
struct vbar_m_capturer_config {
    /* capturer drv config */
    /* 取图摄像头节点 */
    const char *path;
    /* 图像宽度(以像素为单位) */
    uint32_t width;
    /* 图像高度(以像素为单位) */
    uint32_t height;
    /* 每个像素所占字节数 */
    uint32_t widthbytes;
    /* 传感器类型 */
    uint32_t sensor_type;
    /* 像素格式 */
    uint32_t pixel_format;
    /* 图像旋转角度 */
    uint32_t rotation;
    /* 帧数 */
    uint8_t  frame_num;
    /* 最大通道数 */
    uint8_t max_channels;
    /* 是否启用预览 */
    uint8_t  preview_enable;
    /* 预览区域左边界(以像素为单位) */
    uint32_t preview_left;
    /* 预览区域上边界(以像素为单位) */
    uint32_t preview_top;
    /* 预览区域宽度(以像素为单位) */
    uint32_t preview_width;
    /* 预览区域高度(以像素为单位) */
    uint32_t preview_height;
    /* 预览区域旋转角度(以度为单位) */
    uint32_t preview_rotation;
    /* 预览模式 */
    uint32_t preview_mode;
    /* 预览屏幕索引 */
    uint32_t preview_screen_index;
    /* capturer modules config */
    /* 帧率阈值 */
    uint8_t fps_threshold;
};

/* ISP 类型枚举 */
enum vbar_m_isp_type {
    /* 无ISP */
    VBAR_M_ISP_TYPE_NONE,
    /* 软件ISP */
    VBAR_M_ISP_TYPE_SISP,
    /* 硬件ISP */
    VBAR_M_ISP_TYPE_HISP,
};

/**
 * 一个取图模块最大回调数
 */
#define VBAR_M_CAPTURER_MAX_CALLBACK (5)

struct vbar_m_capturer_handle;
struct vbar_m_isp_handle;

/**
 * @brief  摄像头取图后回调 ,注：不可阻塞
 *
 * @param name 回调标签，用于查询\删除已注册回调
 * @param image 图像数据
 * @param pdata  私有数据
 *
 * @return 无
 */
typedef void (*vbar_m_capturer_callback)(const char *name, const struct vbar_drv_image *image, void *pdata);

/**
 * @brief  摄像头取图后异常之后的回调

 * @param pdata  私有数据
 *
 * @return 无
 */
typedef void (*vbar_m_capturer_abnormal_callback)(const struct vbar_drv_image *image, void *pdata);


/**
 * @brief  取图模块初始化
 *
 * @param path        图像采集设备路径，eg:/dev/video0
 * @param width       图像宽
 * @param widthbytes  每个像素所占字节数 GREY : 1， YUV : 2
 * @param height      图像高
 * @param type        图像类型， eg: V4L2_PIX_FMT_GREY
 * @param max_channels 最大支持的同步输出channel数量
 *
 * @return 成功: capturer 句柄， 失败：NULL
 */
VBARLIB_API struct vbar_m_capturer_handle *vbar_m_capturer_init(struct vbar_m_capturer_config *config,
                                                           enum vbar_m_isp_type isp_type,
                                                           const struct vbar_m_isp_config *isp_config);

/**
 * @brief vbar_m_capturer_deinit 取图模块去初始化
 *
 * @return 无
 */
VBARLIB_API void vbar_m_capturer_deinit(struct vbar_m_capturer_handle *capturer);

/**
 * @brief vbar_m_capturer_update_isp_config 更新 isp 配置参数
 *
 * @param capturer  采集设备句柄指针
 * @param isp_config  isp 配置参数
 *
 * @return 无
 */
VBARLIB_API int vbar_m_capturer_update_isp_config(struct vbar_m_capturer_handle *capturer,
                                                                  const struct vbar_m_isp_config *isp_config);

/**
 * @brief  获取摄像头信息
 *
 * @param capturer     capturer 句柄
 * @param info      信息获取后存放地址
 *
 * @return 成功: 0， 失败：-1
 */
VBARLIB_API int vbar_m_capturer_get_info(struct vbar_m_capturer_handle *capturer, struct vbar_m_capturer_info *info);

/**
 * @brief   取图模块处理取图异常注册回调
 *
 * @param capturer      capturer 句柄
 * @param fps_cb        取图异常处理函数
 * @param fps_pdata     私有数据
 * @return 成功：0，失败：-1
 */
VBARLIB_API int vbar_m_capturer_abnormal_cb_register(struct vbar_m_capturer_handle *capturer,
                                                vbar_m_capturer_abnormal_callback abnormal_cb, void *pdata);


/**
 * @brief 取消取图异常回调函数
 *
 * @param capturer  capturer 句柄
 *
 * @return 成功: 0， 其他失败
 */
VBARLIB_API int vbar_m_capturer_abnormal_cb_unregister(struct vbar_m_capturer_handle *capturer);
/**
 * @brief  取图模块注册回调
 *
 * @param capturer      capturer 句柄
 * @param name        回调标签，用于查询\删除已注册回调
 * @param cb          图像回调函数
 * @param pdata       私有数据
 *
 * @return 成功: 0， 重复注册：1，失败：-1
 */
VBARLIB_API int vbar_m_capturer_cb_register(struct vbar_m_capturer_handle *capturer, const char *name,
                                vbar_m_capturer_callback cb, void *pdata);
/**
 * @brief  取图模块删除回调
 *
 * @param capturer      capturer 句柄
 * @param name     回调标签，用于删除已注册回调
 *
 * @return 成功: 0， 其他失败
 */
VBARLIB_API int vbar_m_capturer_cb_unregister(struct vbar_m_capturer_handle *capturer, const char *name);
/**
 * @brief  获取图像数据，轮询可调用此接口
 *
 * @param capturer      capturer 句柄
 *
 * @return 成功: 图像数据指针需自行销毁， 失败：NULL
 */
VBARLIB_API struct vbar_drv_image *vbar_m_capturer_read(struct vbar_m_capturer_handle *capturer);

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
VBARLIB_API struct vbar_drv_image *vbar_m_capturer_image_create(const unsigned char *data,
                                                     uint32_t width, uint32_t widthbytes, uint32_t height);

/**
 * @brief vbar_drv_capturer_read 拷贝图像
 *
 * @param image     待拷贝图像
 *
 * @return 成功：返回图像指针，失败：NULL
 */
VBARLIB_API struct vbar_drv_image *vbar_m_capturer_image_copy(struct vbar_drv_image *image);
/**
 * @brief vbar_drv_capturer_read 销毁图像
 *
 * @param image           待销毁图像
 *
 * @return 无
 */
VBARLIB_API void vbar_m_capturer_image_destroy(struct vbar_drv_image *image);

/**
 * @brief 设置取图的相关配置
 *
 * @param capturer 取图句柄。
 * @param config 取图配置信息结构体指针。
 *
 * @return 成功时返回 0，失败时返回负值错误码。
 */
VBARLIB_API int vbar_m_capturer_set_config(struct vbar_m_capturer_handle *capturer,
                                           struct vbar_m_capturer_config *config);

/**
 * @brief 获取取图的配置
 *
 * @param capturer 取图句柄。
 * @param config 取图配置信息结构体指针，用于存储获取的配置信息。
 *
 * @return 成功时返回 0，失败时返回负值错误码。
 */
VBARLIB_API int vbar_m_capturer_get_config(struct vbar_m_capturer_handle *capturer,
                                           struct vbar_m_capturer_config *config);

/**
 * @brief 使能/关闭capture预览
 *
 * @param capturer  期望进行使能/关闭的capture对象
 * @param enable    期望设置使能还是关闭
 *
 * @return 成功：0；失败：-1
 */
VBARLIB_API int vbar_m_capturer_preview_enable(struct vbar_m_capturer_handle *capturer, bool enable);
#ifdef  __cplusplus
}
#endif

#endif//__VBAR_M_CAPTURER_H__

