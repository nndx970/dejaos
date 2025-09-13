#ifndef __VBAR_DRV_CAPTURER_IMAGE_PROCESS_SUPPORT_PNG_H__
#define __VBAR_DRV_CAPTURER_IMAGE_PROCESS_SUPPORT_PNG_H__

#include <vbar/export.h> /* for VBARLIB_API */

#ifdef	__cplusplus
extern "C" {
#endif

VBARLIB_API struct vbar_drv_image *vbar_drv_image_process_png_file_to_image(const char *filename, enum image_type type);

VBARLIB_API struct vbar_drv_image *vbar_drv_image_process_png_data_to_image(const uint8_t *png_data, size_t data_len, enum image_type type);

/**
 * @brief image save png file
 *
 * @param image 图像数据
 * @param type yuv420p yuv420sp
 * @param save_path 保存路径
 * @param quality  -1 ~ 90, 建议压缩比率设为-1默认比率即可 ,压缩比率依赖zlib，zlib的压缩率为-1 ~ 9,实测0和9两种情况压缩时间为2s和12s
 *
 * @return 0 成功 -1 失败
 */
VBARLIB_API int vbar_drv_image_process_image_to_png_file(struct vbar_drv_image *image, enum image_type type, const char *save_path, int quality);

VBARLIB_API int vbar_drv_image_process_image_to_png_data(struct vbar_drv_image *image, enum image_type type, int quality, uint8_t **png_data, uint32_t *data_len);
#ifdef	__cplusplus
}
#endif


#endif
