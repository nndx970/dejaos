#include "face.h"
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include "../capturer/capturer.h"

// 定义全局变量
static struct vbar_m_capturer_handle *nirCapturer = NULL;
static struct vbar_m_capturer_handle *rgbCapturer = NULL;

static int register_flag = 0;
static char pending_userid[256] = {0};
static int register_result = -1;

struct track_t
{
    int x1;
    int y1;
    int x2;
    int y2;
    int id;
};
static struct track_t g_track = {0};
static struct track_t g_last_track = {0};

static char g_userid[256] = {0};

struct recognition_t
{
    float score;
    int is_living;
    int living_score;
};
static struct recognition_t g_recognition = {0};
static struct recognition_t g_last_recognition = {0};

struct face_config_t
{
    int living_check_enable;
};

// static char g_recognition_result[256] = {0};

// static int last_id = -1;

static struct vbar_drv_image *nir_capturer_image_read()
{
    if (nirCapturer)
    {
        return vbar_m_capturer_read(nirCapturer);
    }
    return NULL;
}

static struct vbar_drv_image *rgb_capturer_image_read()
{
    if (rgbCapturer)
    {
        return vbar_m_capturer_read(rgbCapturer);
    }
    return NULL;
}

static void nir_rgb_capturer_image_destroy(struct vbar_drv_image *image)
{
    if (image)
    {
        vbar_m_capturer_image_destroy(image);
    }
}

static struct vbar_drv_face_config config = {
    .detect_max_num = 1,
    .detect_max_pixel = 512,
    .detect_min_pixel = 30,
    .rgb_detect_width = 704,
    .rgb_detect_height = 1228,
    .nir_detect_enable = 0,
    .nir_detect_width = 576,
    .nir_detect_height = 800,
    .detect_mask_enable = 0,
    .track_max_frames = 20,
    .track_threshold = 50,
    .track_fast_enable = 0,
    .headpose_enable = 1,
    .recg_enable = 1,
    .recg_mask_enable = 0,
    .recg_faceattr_enable = 1,
    .living_check_enable = 1,
    .living_color_enable = 1,
    .living_yolo_enable = 0,
    .distance_enable = 0,
    .x_map = {0.456646, 0.000970, 107.832702, -0.017890, -0.019316},
    .y_map = {-0.000715, 0.457083, 85.48871, -0.001631, -0.002117},
    .rgb_path = "/dev/video3",
    .rgb_frame_width = 1280,
    .rgb_frame_height = 720,
    .rgb_rotation = 90,
    .rgb_preview_x = 0,
    .rgb_preview_y = 0,
    .rgb_preview_width = 600,
    .rgb_preview_height = 1024,
    .rgb_preview_rotation = 90,
    .nir_path = "/dev/video0",
    .nir_frame_width = 800,
    .nir_frame_height = 600,
    .nir_rotation = 90,
    .rgb_image_read = rgb_capturer_image_read,
    .nir_image_read = nir_capturer_image_read,
    .image_destory = nir_rgb_capturer_image_destroy,
    .db_max = 5000,
    .db_path = "/data/db/face.db",
};

int face_detection(struct vbar_drv_face_analysis_result *analysis_result, void *pdata);
int face_recognition(struct vbar_drv_face_analysis_result *analysis_result, void *pdata);

int face_init(void *rgb, void *nir, struct face_config_t *options)
{
    rgbCapturer = (struct vbar_m_capturer_handle *)rgb;
    nirCapturer = (struct vbar_m_capturer_handle *)nir;

    // 创建/data/face目录
    mkdir("/data/face", 0777);

    config.living_check_enable = options->living_check_enable;

    int ret = vbar_drv_face_init(&config);
    if (ret != 0)
    {
        return ret;
    }

    ret = vbar_drv_face_detection_callback_register("face_detection", face_detection, NULL);
    if (ret != 0)
    {
        return ret;
    }

    ret = vbar_drv_face_recognition_callback_register("face_recognition", face_recognition, NULL);
    if (ret != 0)
    {
        return ret;
    }

    return 0;
}

int face_detection(struct vbar_drv_face_analysis_result *analysis_result, void *pdata)
{
    (void)pdata; // Suppress unused parameter warning

    if (analysis_result->face_info_num <= 0)
        return 0;

    // 暂且支持单一人脸识别
    struct vbar_drv_face_info face_info = analysis_result->face_infos[0];
    // if (face_info.rgb_detection.score_quality < 40)
    //     return;

    g_track.x1 = face_info.rgb_detection.rect_render[0];
    g_track.y1 = face_info.rgb_detection.rect_render[1];
    g_track.x2 = face_info.rgb_detection.rect_render[2];
    g_track.y2 = face_info.rgb_detection.rect_render[3];
    g_track.id = face_info.id;

    struct vbar_drv_face_process_mode mode = {
        .is_living_check = config.living_check_enable,
        .is_recognition = true};
    vbar_drv_face_set_face_process_mode(face_info.id, &mode);

    return 0;
}

int face_recognition(struct vbar_drv_face_analysis_result *analysis_result, void *pdata)
{
    (void)pdata; // Suppress unused parameter warning

    if (analysis_result->face_info_num <= 0)
        return 0;
    struct vbar_drv_face_info face_info = analysis_result->face_infos[0];
    struct vbar_drv_face_cmp_result result;
    // 人脸识别特征值比对
    vbar_drv_face_feature_compare(face_info.recognition.feature, &result);
    // strncpy(g_recognition.userid, result.userid, sizeof(g_recognition.userid) - 1);
    strncpy(g_userid, result.userid, sizeof(g_userid) - 1);
    g_recognition.score = result.score;
    g_recognition.is_living = face_info.recognition.is_living_check_success;
    g_recognition.living_score = face_info.recognition.score_living;

    // printf("g_recognition: %s:%f\n", g_recognition.userid, g_recognition.score);
    // printf("g_recognition: %s:%f\n", result.userid, result.score);

    // snprintf(g_recognition_result, sizeof(g_recognition_result), "%s:%f:%d:%d:%d", result.userid, result.score, face_info.id, face_info.recognition.is_living_check_success, face_info.recognition.score_living);

    if (register_flag)
    {
        int ret = vbar_drv_face_features_register(pending_userid, face_info.recognition.feature);
        // 保存注册结果
        register_result = ret;
        register_flag = 0; // 清除注册标志
    }

    return 0;
}

int face_set_pause(bool pause)
{
    int ret = vbar_drv_face_set_pause(pause);
    return ret;
}

int face_register(char *userid)
{
    if (!userid)
        return -1;

    // 设置注册标志和用户ID
    register_flag = 1;
    strncpy(pending_userid, userid, sizeof(pending_userid) - 1);
    pending_userid[sizeof(pending_userid) - 1] = '\0';
    register_result = -1; // 重置结果

    // 简单的轮询等待（单线程使用）
    int timeout = 0;
    while (register_flag == 1 && timeout < 1000)
    {                 // 最多等待1秒
        usleep(1000); // 等待1毫秒
        timeout++;
    }

    if (timeout >= 1000)
    {
        register_flag = 0; // 重置标志
        return -2;         // 超时错误
    }

    return register_result;
}

struct track_t get_face_track_data(void)
{
    if (g_track.x1 == g_last_track.x1 && g_track.y1 == g_last_track.y1)
    {
        return (struct track_t){0};
    }
    g_last_track = g_track;
    return g_track;
}

struct recognition_t get_face_recognition_result(char *userid)
{
    if (g_recognition.score == g_last_recognition.score)
    {
        return (struct recognition_t){0};
    }
    strncpy(userid, g_userid, sizeof(g_userid) - 1);
    g_last_recognition = g_recognition;
    return g_recognition;
}

// void face_update_config(bool liveness_off)
// {
//     config.living_check_enable = liveness_off;
//     vbar_drv_face_update_config(&config);
// }

void face_update_config(struct face_config_t *options)
{
    config.living_check_enable = options->living_check_enable;
    vbar_drv_face_update_config(&config);
}
