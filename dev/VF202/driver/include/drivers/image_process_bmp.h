#ifndef __VBAR_DRV_CAPTURER_IMAGE_PROCESS_SUPPORT_BMP_H__
#define __VBAR_DRV_CAPTURER_IMAGE_PROCESS_SUPPORT_BMP_H__

#include <vbar/export.h> /* for VBARLIB_API */

#ifdef	__cplusplus
extern "C" {
#endif



VBARLIB_API struct vbar_drv_image *vbar_drv_image_process_bmp_file_to_image(const char *filename, enum image_type type);

VBARLIB_API struct vbar_drv_image *vbar_drv_image_process_bmp_data_to_image(const uint8_t *bmp_data, size_t data_len, enum image_type type);

VBARLIB_API int vbar_drv_image_process_image_to_bmp_data(struct vbar_drv_image *image, enum image_type type, uint8_t **bmp_data, uint32_t *data_len);

VBARLIB_API int vbar_drv_image_process_image_to_bmp_file(struct vbar_drv_image *image, enum image_type type, const char *save_path);
#ifdef	__cplusplus
}
#endif


#endif
