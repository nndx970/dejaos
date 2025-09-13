#ifndef __VBAR_DRV_ISP_H__
#define __VBAR_DRV_ISP_H__

#include <stdint.h>

#include <vbar/export.h> /* for VBARLIB_API */
#include <vbar/auxiliary.h>
#include <drivers/capturer.h>

struct vbar_drv_isp {
    int (*image_process)(struct vbar_drv_isp *, uint8_t *);
    int (*update_config)(struct vbar_drv_isp *, void *);
    void (*destroy)(struct vbar_drv_isp *);
    void *pdata;
};

enum {
    VBAR_DRV_ISP_TYPE_SISP = 1,
    VBAR_DRV_ISP_TYPE_HISP = 2
};

/* sisp 相关 */
enum {
	VBAR_DRV_SISP_METHOD_HISTOGRAM_CUMULATIVE,
};

enum {
	VBAR_DRV_SISP_SUB_METHOD_CENTER_WEIGHTED,               /*center weighted*/
	VBAR_DRV_SISP_SUB_METHOD_NORMAL,                       /*normal*/
	VBAR_DRV_SISP_SUB_METHOD_CENTER_ONLY,	                   /*center only*/
	VBAR_DRV_SISP_SUB_METHOD_CENTER_ROI	                   /*center only*/
};

struct vbar_drv_sisp_roi {
	uint32_t left;
	uint32_t top;
	uint32_t right;
	uint32_t bottom;
};

struct vbar_drv_sisp_config {
    uint32_t method;             /* 采用的算法 */
    uint32_t sub_method;
    uint32_t width;
    uint32_t widthbytes;
    uint32_t height;
    uint32_t pixel_format;
    uint8_t target_luminance;    /* 目标图像的亮度 */
    uint32_t target_percentile;  /* 目标亮度所占的百分比 */
    uint32_t sample_gap;         /* 采样间隔 单位: 行*/
    uint32_t min_exp;
    uint32_t max_exp;
    uint32_t min_gain;
    uint32_t max_gain;
    uint32_t frame_rate;
    struct vbar_drv_sisp_roi roi;
};

/* hisp 相关 */




#ifdef	__cplusplus
extern "C" {
#endif



/**
* @brief vbar_drv_isp_create 创建isp 设备
*
* @param type       isp设备类型，eg:VBAR_DRV_ISP_TYPE_SISP
* @param capturer   采集设备句柄
* @param param      isp 配置参数，sisp_config or hisp_config
*
* @return 成功：isp 设备句柄指针，失败：NULL
*/

VBARLIB_API struct vbar_drv_isp *vbar_drv_isp_create(uint32_t type, struct vbar_drv_capturer *capturer, void *param);


/**
 * @brief vbar_drv_isp_destroy 销毁isp设备
 *
 * @param isp  isp设备句柄指针
 *
 * @return 无
 */
VBARLIB_API void vbar_drv_isp_destroy(struct vbar_drv_isp *isp);


/**
 * @brief vbar_drv_isp_update_config  更新isp配置
 *
 * @param isp      isp设备句柄指针
 * @param config   isp对应的配置结构体指针
 *
 * @return 成功：0， 失败：-1
 */
VBARLIB_API int vbar_drv_isp_update_config(struct vbar_drv_isp *isp, void *config);


/**
 * @brief vbar_drv_isp_image_process isp 图像处理函数
 *
 * @param isp       isp设备句柄指针
 * @param image     图像数据结构指针
 *
 * @return 成功：0，失败：-1
 */
VBARLIB_API int vbar_drv_isp_image_process(struct vbar_drv_isp *isp, uint8_t *image);

#ifdef	__cplusplus
}
#endif

#endif
