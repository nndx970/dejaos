#ifndef __VBAR_DRV_FACE_H__
#define __VBAR_DRV_FACE_H__

#include <stdint.h>
#include <pthread.h>

#include <vbar/export.h> /* for VBARLIB_API */

struct vbar_drv_face_config {
    /*detect*/
	int detect_enable;
	int detect_max_num;
	int detect_max_pixel;
	int detect_min_pixel;
	int rgb_detect_width;
	int rgb_detect_height;
	int nir_detect_width;
	int nir_detect_height;
	int detect_mode; /*normal 0, ipc 1*/
	int detect_mask_enable;
    int nir_detect_enable;
    //track
	int track_max_frames;
	int track_threshold;
	int track_fast_enable;
	int headpose_enable;

    /*recgnize */
	int recg_enable;
	int recg_mask_enable;
	int recg_faceattr_enable;
    uint32_t recg_repeat_interval;
    uint8_t  recg_repeat;
    /*living*/
    uint8_t living_check_enable;
	int living_color_enable;
    int living_score_threshold;
	int living_yolo_enable;
    int living_confidence_threshold;
	int distance_enable;
    uint32_t living_detect_timeout_ms;

    double x_map[5];
    double y_map[5];

    /*图像读取函数*/
    uint32_t rgb_frame_width;
    uint32_t rgb_frame_height;
    uint32_t nir_frame_width;
    uint32_t nir_frame_height;
    uint32_t rgb_rotation;
    uint32_t nir_rotation;
    const char *rgb_path;
    const char *nir_path;
    uint32_t rgb_x_margin;
    uint32_t rgb_y_margin;
    uint32_t nir_y_expand;
    uint32_t nir_x_expand;
    uint32_t rgb_preview_x;
    uint32_t rgb_preview_y;
    uint32_t rgb_preview_width;
    uint32_t rgb_preview_height;
    uint32_t rgb_preview_rotation;
    struct vbar_drv_image *(*nir_image_read)(void);
    struct vbar_drv_image *(*rgb_image_read)(void);
    void (*image_destory)(struct vbar_drv_image *);
};

#define VBAR_DRV_FACE_MAX_DETECT_NUM 10
struct vbar_drv_face_info {
    int rect[4];
    int rect_smooth[4];
    int quality_score;
    int id;
    float kpts[10];
    int score;
    uint8_t flag_is_living;
    uint8_t flag_is_recognized;
};

struct vbar_drv_face_detect_info {
    struct vbar_drv_face_info faces[VBAR_DRV_FACE_MAX_DETECT_NUM];
    uint8_t num;
};
typedef int (*vbar_drv_face_detect_callback)(struct vbar_drv_face_detect_info *, void *);

struct vbar_drv_face_features {
    signed char feature[1024];
    /* TODO: flag_is_living flag_is_recognized score id rect rect_smooth 使用检测的结果 */
    bool flag_is_living;
    bool flag_is_recognized;
    int quality_score;
    int score;
    int id;
    int rect_smooth[4]; /* recalcu poisition, for ui */
    int rect[4];        /* no recalcu position, for face */
    int is_wear_mask;
    int is_living;
};

enum {
    VBAR_DRV_FACE_RECG_EVENT_NORMAL,
    VBAR_DRV_FACE_RECG_EVENT_REGISTER
};

struct vbar_drv_face_recg_info {
    struct vbar_drv_face_features features[VBAR_DRV_FACE_MAX_DETECT_NUM];
    uint8_t num;
    uint8_t event_type;
    struct vbar_drv_image *rgb_image;
};
typedef int (*vbar_drv_face_recg_callback)(struct vbar_drv_face_recg_info *, void *);

#define VBAR_DRV_FACE_DB_PATH_MAXLEN  (256)
struct vbar_drv_face_flist_config {
    int max;
    char db_path[VBAR_DRV_FACE_DB_PATH_MAXLEN];
};

#define VBAR_DRV_FACE_USERID_SIZE     (sizeof(char)  * 256)
struct vbar_drv_face_cmp_result {
    float score;                    /* 与人脸特征值列表对应的对比得分数组 */
    char userid[VBAR_DRV_FACE_USERID_SIZE];
};

struct vbar_drv_face_flist_select_result {
    int max;
    int total;
    int current;
    char userid[VBAR_DRV_FACE_USERID_SIZE];
};

typedef int (*vbar_drv_face_flist_select_cb)(struct vbar_drv_face_flist_select_result *, void *);

#ifdef	__cplusplus
extern "C" {
#endif

VBARLIB_API int vbar_drv_face_init(struct vbar_drv_face_config *config, int fflist_max, const char *db_path);

VBARLIB_API void vbar_drv_face_deinit(void);

VBARLIB_API int vbar_drv_face_update_config(struct vbar_drv_face_config *config);

VBARLIB_API int vbar_drv_face_register_detect_callback(vbar_drv_face_detect_callback func_cb, void *pdata, const char *name);

VBARLIB_API int vbar_drv_face_unregister_detect_callback(const char *name);

VBARLIB_API int vbar_drv_face_register_nir_detect_callback(vbar_drv_face_detect_callback func_cb, void *pdata, const char *name);

VBARLIB_API int vbar_drv_face_unregister_nir_detect_callback(const char *name);

VBARLIB_API int vbar_drv_face_register_recg_callback(vbar_drv_face_recg_callback func_cb, void *pdata, const char *name);

VBARLIB_API int vbar_drv_face_unregister_recg_callback(const char *name);

VBARLIB_API int vbar_drv_face_set_recg_mode(uint8_t mode);

VBARLIB_API int vbar_drv_face_features_register(struct vbar_drv_face_features *feature, const char *userid);

VBARLIB_API int vbar_drv_face_features_unregister(const char *userid);

VBARLIB_API int vbar_drv_face_features_update(void);

VBARLIB_API int vbar_drv_face_features_clean(void);

VBARLIB_API int vbar_drv_face_feature_compare(struct vbar_drv_face_features *current_feature, struct vbar_drv_face_cmp_result *result);

VBARLIB_API int vbar_drv_face_detect_set_enable(bool en);

VBARLIB_API void vbar_drv_face_features_register_dump(void);

VBARLIB_API int vbar_drv_face_get_isp_brightness(int *brightness);

VBARLIB_API int vbar_dev_face_face_feature_select(vbar_drv_face_flist_select_cb select_cb, void *pdata);

VBARLIB_API int vbar_drv_face_get_detection_info_by_image(struct vbar_drv_image *image,
                                                          struct vbar_drv_face_detect_info *info);

VBARLIB_API int vbar_drv_face_get_recognition_info_by_image(struct vbar_drv_image *image,
                                                            struct vbar_drv_face_recg_info *info);

#ifdef __cplusplus
}
#endif

#endif /* __VBAR_CAPTURER_H__ */
