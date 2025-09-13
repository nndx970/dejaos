#include "face.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <time.h>
#include "../capturer/capturer.h"
#include "../capturer/include/image_process.h"

// 定义全局变量
static struct vbar_m_capturer_handle *nirCapturer = NULL;
static struct vbar_m_capturer_handle *rgbCapturer = NULL;

static int register_flag = 0;
static char register_userid[256] = {0};
static int register_result = -99;
static char saved_picture_path[256] = {0};
static char saved_picture_thumb_path[256] = {0};

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

static int g_recognition_success = -1;

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

static int face_init_flag = 0;
int face_init(void *rgb, void *nir, struct face_config_t *options)
{
    if (face_init_flag)
    {
        printf("face already initialized\n");
        return 0;
    }
    face_init_flag = 1;

    rgbCapturer = (struct vbar_m_capturer_handle *)rgb;
    nirCapturer = (struct vbar_m_capturer_handle *)nir;

    // 创建/data/face目录（递归创建多级目录）
    system("mkdir -p /data/face");

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

void face_deinit(void)
{
    if (face_init_flag)
    {
        vbar_drv_face_deinit();
        face_init_flag = 0;
    }
    else
    {
        printf("face has not been initialized\n");
    }
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
void __save_image(char *dir, char *userid, struct vbar_drv_face_recongition_info *recognition);

int face_recognition(struct vbar_drv_face_analysis_result *analysis_result, void *pdata)
{
    (void)pdata; // Suppress unused parameter warning

    if (analysis_result->face_info_num <= 0)
        return 0;
    struct vbar_drv_face_info face_info = analysis_result->face_infos[0];
    struct vbar_drv_face_cmp_result result;
    // 人脸识别特征值比对
    vbar_drv_face_feature_compare(face_info.recognition.feature, &result);

    strncpy(g_userid, result.userid, sizeof(g_userid) - 1);
    g_recognition.score = result.score;
    g_recognition.is_living = face_info.recognition.is_living_check_success;
    g_recognition.living_score = face_info.recognition.score_living;
    if (register_flag)
    {
        int ret = vbar_drv_face_features_register(register_userid, face_info.recognition.feature);
        // 保存注册结果
        register_result = ret;
        register_flag = 0; // 清除注册标志
        if (ret == 0)
        {
            __save_image("/data/user/register/picture", register_userid, &face_info.recognition);
        }
    }else{
        // 使用更高效的等待机制，避免CPU占用过高
        int wait_count = 0;
        const int max_wait_count = 50; // 最多等待5秒 (50 * 100ms)
        
        while (g_recognition_success == -1 && wait_count < max_wait_count)
        {
            usleep(100000); // 100毫秒
            wait_count++;
        }
        
        // 如果超时仍未获得结果，跳过保存
        if (wait_count >= max_wait_count) {
            printf("警告：等待识别结果超时，跳过图片保存\n");
            g_recognition_success = -1;
            return 0;
        }
        if (g_recognition_success == 1)
        {
            // 生成带时间戳的用户名
            char userid_with_timestamp[512];
            time_t current_time = time(NULL);
            snprintf(userid_with_timestamp, sizeof(userid_with_timestamp), "%s_%ld", g_userid, current_time);
            __save_image("/data/user/access/picture", userid_with_timestamp, &face_info.recognition);
        }
        g_recognition_success = -1;
    }
    return 0;
}

void __save_image(char *dir, char *userid, struct vbar_drv_face_recongition_info *recognition)
{
    // 检查参数有效性
    if (!dir || !userid || !recognition) {
        printf("错误：参数无效，无法保存图像\n");
        return;
    }

    // 递归创建指定目录
    char mkdir_cmd[512];
    snprintf(mkdir_cmd, sizeof(mkdir_cmd), "mkdir -p %s", dir);
    system(mkdir_cmd);

    // 生成主图片文件路径
    char picture_path[512];
    snprintf(picture_path, sizeof(picture_path), "%s/%s.jpeg", dir, userid);

    // 如果文件存在则先删除
    if (access(picture_path, F_OK) == 0) {
        remove(picture_path);
    }

    // 调整图像尺寸并保存主图片
    struct vbar_drv_image *resized_image = vbar_drv_image_resize_resolution(recognition->rgb_image, 480, 854, FILTER_MODE_BOX);
    vbar_drv_image_process_image_to_picture_file(resized_image, IMAGE_YUV420SP, TYPE_JPEG, picture_path, 100);
    vbar_drv_capturer_image_destroy(resized_image); // 释放内存

    // 生成缩略图文件路径
    char thumb_path[512];
    snprintf(thumb_path, sizeof(thumb_path), "%s/%s_thumb.jpeg", dir, userid);

    // 如果缩略图文件存在则先删除
    if (access(thumb_path, F_OK) == 0) {
        remove(thumb_path);
    }

    struct vbar_drv_image *thumb_image = vbar_drv_image_process_yuv420sp_cut(recognition->rgb_image, recognition->rect_smooth[0], recognition->rect_smooth[1], recognition->rect_smooth[2] - recognition->rect_smooth[0], recognition->rect_smooth[3] - recognition->rect_smooth[1]);
    vbar_drv_image_process_image_to_picture_file(thumb_image, IMAGE_YUV420SP, TYPE_JPEG, thumb_path, 100);
    vbar_drv_capturer_image_destroy(thumb_image); // 释放内存

    printf("保存图片成功：%s\n", picture_path);
    printf("保存缩略图成功：%s\n", thumb_path);

    strncpy(saved_picture_path, picture_path, sizeof(saved_picture_path) - 1);
    saved_picture_path[sizeof(saved_picture_path) - 1] = '\0';
    strncpy(saved_picture_thumb_path, thumb_path, sizeof(saved_picture_thumb_path) - 1);
    saved_picture_thumb_path[sizeof(saved_picture_thumb_path) - 1] = '\0';
}

int face_set_pause(bool pause)
{
    int ret = vbar_drv_face_set_pause(pause);
    return ret;
}

void face_register(char *userid)
{
    if (!userid)
        return;
    // 设置注册标志和用户ID
    register_flag = 1;
    strncpy(register_userid, userid, sizeof(register_userid) - 1);
    register_userid[sizeof(register_userid) - 1] = '\0';
}

void face_register_reset(void)
{
    register_flag = 0;
    register_userid[0] = '\0';
    register_result = -99;
}

int get_face_register_result(void)
{
    int result = register_result;
    if (result != -99)
        register_result = -99;
    return result;
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

void set_face_recognition_result(int is_success)
{
    g_recognition_success = is_success;
}

void get_saved_picture_path(char *path, char *thumb_path)
{
    strncpy(path, saved_picture_path, sizeof(saved_picture_path) - 1);
    saved_picture_path[0] = '\0';
    strncpy(thumb_path, saved_picture_thumb_path, sizeof(saved_picture_thumb_path) - 1);
    saved_picture_thumb_path[0] = '\0';
}

void face_update_config(struct face_config_t *options)
{
    config.living_check_enable = options->living_check_enable;
    vbar_drv_face_update_config(&config);
}
