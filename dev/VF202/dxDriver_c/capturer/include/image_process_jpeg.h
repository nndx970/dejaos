#ifndef __VBAR_DRV_CAPTURER_IMAGE_PROCESS_SUPPORT_JPEG_H__
#define __VBAR_DRV_CAPTURER_IMAGE_PROCESS_SUPPORT_JPEG_H__

#include <vbar/export.h> /* for VBARLIB_API */

#ifdef	__cplusplus
extern "C" {
#endif

VBARLIB_API int vbar_drv_image_process_jpeg_file_to_rgb888(const char *filename, uint32_t *width, uint32_t *height, uint8_t **rgb_data);

VBARLIB_API int vbar_drv_image_process_jpeg_data_to_rgb888(const uint8_t *jpeg_data, size_t data_len, uint32_t *width, uint32_t *height, uint8_t **rgb_data);

VBARLIB_API int vbar_drv_image_process_rgb888_to_jpeg_file(const char *filename, uint8_t *rgb,
                                                         uint32_t image_width, uint32_t image_height, int quality);

VBARLIB_API int vbar_drv_image_process_rgb888_to_jpeg_data(uint8_t *rgb, uint32_t image_width, uint32_t image_height,
                                                         uint8_t **jpeg_data, uint32_t *data_len, int quality);

VBARLIB_API int vbar_drv_image_process_image_to_jpeg_file(struct vbar_drv_image *image, enum image_type type, const char *save_path, int quality);

VBARLIB_API int vbar_drv_image_process_image_to_jpeg_data(struct vbar_drv_image *image, enum image_type type, int quality,
                                                        uint8_t **jpeg_data, uint32_t *data_len);

VBARLIB_API struct vbar_drv_image *vbar_drv_image_process_jpeg_file_to_image(const char *filename, enum image_type type);

VBARLIB_API struct vbar_drv_image *vbar_drv_image_process_jpeg_data_to_image(const uint8_t *jpeg_data, size_t data_len, enum image_type type);

#ifdef	__cplusplus
}
#endif


#endif
