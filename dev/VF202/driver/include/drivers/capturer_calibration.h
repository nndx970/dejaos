#ifndef __VBAR_BASE_CAM_CAL_H__
#define __VBAR_BASE_CAM_CAL_H__

#include <vbar/export.h> /* for VBARLIB_API */

struct camera_cal_info {
    struct vbar_drv_image *rgb_image;
    struct vbar_drv_image *nir_image;
    int rgb_x;
    int rgb_y;
    int rgb_w;
    int rgb_h;
    int nir_width;
    int nir_height;
};

struct map_t {
    double x_map[5];
    double y_map[5];
};


struct vbar_drv_camera_cal {
    int (*cal_point_from_image)(struct camera_cal_info *);
    int (*cal_map)(struct camera_cal_info *, struct map_t *, const char *);
    void (*cal_destroy)(struct vbar_drv_camera_cal *);
};


#ifdef	__cplusplus
extern "C" {
#endif

VBARLIB_API int camera_calibration_init(void);
VBARLIB_API int camera_calibration_from_image(struct camera_cal_info *info);
VBARLIB_API int camera_calibration_get_map(struct camera_cal_info *info, struct map_t *map, const char *path);
VBARLIB_API void camera_calibration_deinit(void);
#ifdef	__cplusplus
}
#endif

#endif
