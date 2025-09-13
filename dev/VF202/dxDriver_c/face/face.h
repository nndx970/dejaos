#ifndef __VBAR_DRV_FACE_H__
#define __VBAR_DRV_FACE_H__

#include <stdint.h>
#include <pthread.h>

#include <vbar/export.h>

/**
 * @struct vbar_drv_face_config
 * @brief 人脸算法全链路配置结构体
 * @details 包含人脸检测、追踪、识别等模块的全链路配置参数
 */
struct vbar_drv_face_config {
    /* 检测参数 */
    /** @brief 最大检测人脸数量 (范围：1-5) */
    int detect_max_num;
    /** @brief 检测区域最大像素值 (单位：像素) */
    int detect_max_pixel;
    /** @brief 检测区域最小像素值 (单位：像素) */
    int detect_min_pixel;
    /** @brief RGB摄像头检测区域宽度 (单位：像素) */
    int rgb_detect_width;
    /** @brief RGB摄像头检测区域高度 (单位：像素) */
    int rgb_detect_height;
    /** @brief NIR摄像头检测开关 (0-禁用 1-启用) */
    int nir_detect_enable;
    /** @brief NIR摄像头检测区域宽度 (单位：像素) */
    int nir_detect_width;
    /** @brief NIR摄像头检测区域高度 (单位：像素) */
    int nir_detect_height;
    /** @brief 口罩检测开关 (0-禁用 1-启用) */
    int detect_mask_enable;

    /* 追踪参数 */
    /** @brief 持续追踪最大帧数 */
    int track_max_frames;
    /** @brief 人脸追踪相似度阈值 */
    int track_threshold;
    /** @brief 快速追踪模式开关 */
    int track_fast_enable;
    /** @brief 头部姿态检测开关 (0-禁用 1-启用) */
    int headpose_enable;

    /* 识别参数 */
    /** @brief 人脸识别总开关 (0-禁用 1-启用) */
    int recg_enable;
    /** @brief 口罩状态下识别开关 (0-禁用 1-启用) */
    int recg_mask_enable;
    /** @brief 人脸属性识别开关 (0-禁用 1-启用) */
    int recg_faceattr_enable;

    /* 活检参数 */
    /** @brief 活体检测总开关 (0-禁用 1-启用) */
    uint8_t living_check_enable;
    /** @brief 基于颜色分析的活体检测 (0-禁用 1-启用) */
    int living_color_enable;
    /** @brief 基于YOLO模型的活体检测 (0-禁用 1-启用) */
    int living_yolo_enable;
    /** @brief 人脸距离检测开关 (0-禁用 1-启用) */
    int distance_enable;

    /* 标定参数 */
    /** @brief 标定点X坐标映射表 (长度固定为5) */
    double x_map[5];
    /** @brief 标定点Y坐标映射表 (长度固定为5) */
    double y_map[5];

    /* 坐标处理/ISP 参数 */
    /** @brief RGB图像输入路径 */
    const char *rgb_path;
    /** @brief RGB原始图像宽度 (需与摄像头输出一致) */
    uint32_t rgb_frame_width;
    /** @brief RGB原始图像高度 (需与摄像头输出一致) */
    uint32_t rgb_frame_height;
    /** @brief RGB图像旋转角度 (有效值：0/90/180/270) */
    uint32_t rgb_rotation;
    /** @brief RGB预览窗口X坐标 (相对原始图像左上角) */
    uint32_t rgb_preview_x;
    /** @brief RGB预览窗口Y坐标 (相对原始图像左上角) */
    uint32_t rgb_preview_y;
    /** @brief RGB预览窗口宽度 */
    uint32_t rgb_preview_width;
    /** @brief RGB预览窗口高度 */
    uint32_t rgb_preview_height;
    /** @brief RGB预览旋转角度 (有效值：0/90/180/270) */
    uint32_t rgb_preview_rotation;
    /** @brief NIR图像输入路径 */
    const char *nir_path;
    /** @brief NIR原始图像宽度 (需与摄像头输出一致) */
    uint32_t nir_frame_width;
    /** @brief NIR原始图像高度 (需与摄像头输出一致) */
    uint32_t nir_frame_height;
    /** @brief NIR图像旋转角度 (有效值：0/90/180/270) */
    uint32_t nir_rotation;

    /* 图像读取函数 */
    /** @brief RGB图像读取回调函数指针 */
    struct vbar_drv_image *(*rgb_image_read)(void);
    /** @brief NIR图像读取回调函数指针 */
    struct vbar_drv_image *(*nir_image_read)(void);
    /** @brief 图像资源销毁回调函数指针 */
    void (*image_destory)(struct vbar_drv_image *);

    /* 人脸特征值列表参数 */
    /** @brief 特征库最大容量 */
    int db_max;
    /** @brief 特征库文件路径 */
    const char *db_path;
};

struct vbar_drv_face_analysis_result;

/**
 * @struct vbar_drv_face_process_mode
 * @brief 人脸处理模式配置结构体
 * @details 控制算法处理流程中的关键功能开关
 */
struct vbar_drv_face_process_mode {
    /** @brief 活体检测开关 (true-启用活体检测流程，false-跳过活体检测) */
    bool is_living_check;
    /** @brief 人脸识别开关 (true-启用特征值提取流程，false-仅进行人脸检测) */
    bool is_recognition;
};

#ifdef	__cplusplus
extern "C" {
#endif

/**
 * @brief 初始化人脸算法模块
 * @param config 算法配置参数指针（需在调用前完成配置）
 * @return 操作结果状态码
 * @retval 0 初始化成功
 * @retval -1 初始化失败（参数无效或资源分配失败）
 */
VBARLIB_API int vbar_drv_face_init(struct vbar_drv_face_config *config);

/**
 * @brief 注销人脸算法模块
 * @note 该函数会释放所有算法相关资源，调用后需重新初始化才能使用
 */
VBARLIB_API void vbar_drv_face_deinit(void);

/**
 * @brief 动态更新算法配置参数
 * @param config 新的配置参数指针
 * @return 操作结果状态码
 * @retval 0 更新成功
 * @retval -1 更新失败（参数无效或模块未初始化）
 */
VBARLIB_API int vbar_drv_face_update_config(struct vbar_drv_face_config *config);

/**
 * @brief 人脸服务启停控制接口
 * @param pause 启停控制标志
 * @return 操作结果状态码
 * @retval 0 成功设置开关
 * @retval -1 无效参数或模块未初始化
 */
VBARLIB_API int vbar_drv_face_set_pause(bool pause);

/**
 * @brief 通过图像获取人脸检测信息
 * @param image 输入图像指针（需包含有效图像数据）
 * @param info 输出参数，接收分析结果的结构体指针
 * @return 操作结果状态码
 * @retval 0 成功获取检测信息
 * @retval -1 输入参数无效或检测失败
 */
VBARLIB_API int vbar_drv_face_get_detection_info_by_image(struct vbar_drv_image *image,
                                                          struct vbar_drv_face_analysis_result *info);

/**
 * @brief 通过图像获取人脸识别信息
 * @param image 输入图像指针（需包含有效人脸区域）
 * @param info 输出参数，接收识别结果的结构体指针
 * @return 操作结果状态码
 * @retval 0 成功获取识别信息
 * @retval -1 输入参数无效或识别失败
 */
VBARLIB_API int vbar_drv_face_get_recognition_info_by_image(struct vbar_drv_image *image,
                                                            struct vbar_drv_face_analysis_result *info);

/**
 * @brief 获取指定ID的人脸处理模式配置
 * @param id 人脸跟踪ID（0表示默认模式）
 * @param mode 输出参数，接收处理模式配置的结构体指针
 * @return 操作结果状态码
 * @retval 0 成功获取配置
 * @retval -1 无效ID或指针为空
 */
VBARLIB_API int vbar_drv_face_get_face_process_mode(int id, struct vbar_drv_face_process_mode *mode);

/**
 * @brief 设置指定ID的人脸处理模式
 * @param id 人脸跟踪ID（0表示默认模式）
 * @param mode 新的处理模式配置指针
 * @return 操作结果状态码
 * @retval 0 成功设置配置
 * @retval -1 无效ID或配置无效
 */
VBARLIB_API int vbar_drv_face_set_face_process_mode(int id, struct vbar_drv_face_process_mode *mode);

/**
 * @brief 获取环境亮度值
 * @param brightness 输出参数，接收亮度值的指针（范围0-100）
 * @return 操作结果状态码
 * @retval 0 成功获取亮度值
 * @retval -1 传感器不可用或指针无效
 */
VBARLIB_API int vbar_drv_face_get_environment_brightness(int *brightness);

#ifdef	__cplusplus
}
#endif

/* 描述人脸特征属性的结构体，
 * 包含性别属性，遮挡属性，角度属性等
 */
/* TODO: 暂时不明晰是检测结果还是识别结果，等待进一步追代码 */
struct vbar_drv_face_attribute {
    /* 是否为男性 */
    bool is_male;

    /* 是否佩戴口罩(mask), 帽子(hat), 眼镜(glasses); true 为佩戴 */
    bool is_wear_glasses;
    bool is_wear_hat;
    bool is_wear_mask;

    /* 是否遮挡左眼(left_eye), 右眼(right_eye), 嘴巴(mouth), true 为遮挡 */
    bool is_cover_left_eye;
    bool is_cover_right_eye;
    bool is_cover_mouth;
};

/**
 * @struct vbar_drv_face_rgb_detection_info
 * @brief RGB摄像头人脸检测信息结构体
 * @details 包含RGB图像中人脸检测的详细结果数据
 */
struct vbar_drv_face_rgb_detection_info {
    /** @brief 人脸检测置信度分数 (范围：0-100) */
    int score;
    /** @brief 人脸质量评估分数 (范围：0-100) */
    int score_quality;

    /** @brief 人脸坐标数组 [左上x, 左上y, 右下x, 右下y] 用于识别流程 */
    int rect[4];
    /** @brief 人脸坐标数组 [左上x, 左上y, 右下x, 右下y] 用于界面渲染 */
    int rect_render[4];
    /** @brief 平滑处理后的人脸坐标 [左上x, 左上y, 右下x, 右下y] 用于缩略图截取 */
    int rect_smooth[4];
};

/**
 * @struct vbar_drv_face_recongition_info
 * @brief 人脸识别信息结构体
 * @details 包含人脸识别流程中的关键数据
 */
#define VBAR_DRV_FACE_FEATURE_SIZE  (1024)
struct vbar_drv_face_recongition_info {
    /** @brief RGB原始图像指针 (用于后续处理) */
    struct vbar_drv_image *rgb_image;

    /** @brief 活体检测是否通过标志 */
    bool is_living_check_success;
    /** @brief 活体检测分数 (范围：0-100) */
    int score_living;

    /** @brief 人脸识别是否成功标志 */
    bool is_recognition_sccess;
    /** @brief 平滑处理后的人脸坐标 [左上x, 左上y, 右下x, 右下y] 用于缩略图截取 */
    int rect_smooth[4];
    /** @brief 人脸特征值数组 (1024维特征向量) */
    uint8_t feature[VBAR_DRV_FACE_FEATURE_SIZE];
    /** @brief 有效特征值长度 (单位：字节) */
    uint16_t feature_len;
};

/**
 * @struct vbar_drv_face_nir_detection_info
 * @brief NIR摄像头人脸检测信息结构体
 * @details 包含近红外图像中人脸检测的基础数据
 */
struct vbar_drv_face_nir_detection_info {
    /** @brief 人脸坐标数组 [左上x, 左上y, 右下x, 右下y] */
    int rect[4];
};

/**
 * @enum vbar_drv_face_info_type
 * @brief 人脸信息类型枚举
 * @details 标识不同算法模块产生的数据类型
 */
enum vbar_drv_face_info_type {
    VBAR_DRV_FACE_INFO_TYPE_RGB_DETECTION,  /**< RGB摄像头检测结果 */
    VBAR_DRV_FACE_INFO_TYPE_NIR_DETECTION,  /**< NIR摄像头检测结果 */
    VBAR_DRV_FACE_INFO_TYPE_RECOGNITION,    /**< 人脸识别流程结果 */
};

/**
 * @struct vbar_drv_face_info
 * @brief 人脸信息容器结构体
 * @details 整合不同算法模块产生的数据结果
 */
struct vbar_drv_face_info {
    /** @brief 人脸跟踪ID (用于多目标追踪) */
    int id;

    /** @brief 信息类型标识 */
    enum vbar_drv_face_info_type info_type;
    /** @brief RGB检测结果数据 */
    struct vbar_drv_face_rgb_detection_info rgb_detection;
    /** @brief NIR检测结果数据 */
    struct vbar_drv_face_nir_detection_info nir_detection;
    /** @brief 人脸识别结果数据 */
    struct vbar_drv_face_recongition_info recognition;
};

/** @brief 最大人脸信息数量限制 */
#define VBAR_DRV_FACE_INFO_MAX 10

/**
 * @struct vbar_drv_face_analysis_result
 * @brief 人脸分析结果容器结构体
 * @details 包含单帧图像中检测到的所有人脸信息
 */
struct vbar_drv_face_analysis_result {
    /** @brief 检测到的人脸数量 (不超过VBAR_DRV_FACE_INFO_MAX) */
    int face_info_num;
    /** @brief 人脸信息数组 */
    struct vbar_drv_face_info face_infos[VBAR_DRV_FACE_INFO_MAX];
};

/**
 * @typedef vbar_drv_face_callback
 * @brief 人脸分析回调函数类型
 * @param result 分析结果指针
 * @param pdata 用户自定义数据指针
 * @return 状态码
 * @retval 0 处理成功
 * @retval -1 处理失败
 */
typedef int (*vbar_drv_face_callback)(struct vbar_drv_face_analysis_result *, void *);

#ifdef	__cplusplus
extern "C" {
#endif

/**
 * @brief 注册RGB人脸检测回调函数
 * @param name 回调标识名称（需保证唯一性）
 * @param func_cb 回调函数指针（非空）
 * @param pdata 用户自定义数据指针（可空）
 * @return 操作结果状态码
 * @retval 0 注册成功
 * @retval -1 参数无效或名称重复
 */
VBARLIB_API int vbar_drv_face_detection_callback_register(const char *name, vbar_drv_face_callback func_cb, void *pdata);

/**
 * @brief 注销RGB人脸检测回调函数
 * @param name 已注册的回调标识名称
 * @return 操作结果状态码
 * @retval 0 注销成功
 * @retval -1 未找到对应名称的注册项
 */
VBARLIB_API int vbar_drv_face_detection_callback_unregister(const char *name);

/**
 * @brief 注册NIR人脸检测回调函数
 * @param name 回调标识名称（需保证唯一性）
 * @param func_cb 回调函数指针（非空）
 * @param pdata 用户自定义数据指针（可空）
 * @return 操作结果状态码
 * @retval 0 注册成功
 * @retval -1 参数无效或名称重复
 */
VBARLIB_API int vbar_drv_face_nir_detection_callback_register(const char *name, vbar_drv_face_callback func_cb, void *pdata);

/**
 * @brief 注销NIR人脸检测回调函数
 * @param name 已注册的回调标识名称
 * @return 操作结果状态码
 * @retval 0 注销成功
 * @retval -1 未找到对应名称的注册项
 */
VBARLIB_API int vbar_drv_face_nir_detection_callback_unregister(const char *name);

/**
 * @brief 注册人脸识别回调函数
 * @param name 回调标识名称（需保证唯一性）
 * @param func_cb 回调函数指针（非空）
 * @param pdata 用户自定义数据指针（可空）
 * @return 操作结果状态码
 * @retval 0 注册成功
 * @retval -1 参数无效或名称重复
 */
VBARLIB_API int vbar_drv_face_recognition_callback_register(const char *name, vbar_drv_face_callback func_cb, void *pdata);

/**
 * @brief 注销人脸识别回调函数
 * @param name 已注册的回调标识名称
 * @return 操作结果状态码
 * @retval 0 注销成功
 * @retval -1 未找到对应名称的注册项
 */
VBARLIB_API int vbar_drv_face_recognition_callback_unregister(const char *name);

#ifdef	__cplusplus
}
#endif

/**
 * @def VBAR_DRV_FACE_USERID_SIZE
 * @brief 用户ID最大字节长度 (包含结束符)
 */
#define VBAR_DRV_FACE_USERID_SIZE     (sizeof(char)  * 256)

/**
 * @struct vbar_drv_face_cmp_result
 * @brief 人脸特征值对比结果结构体
 * @details 包含特征值对比结果和匹配的用户ID
 */
struct vbar_drv_face_cmp_result {
    /** @brief 特征值相似度得分 (范围：0.0-1.0，1.0表示完全匹配) */
    float score;
    /** @brief 匹配成功的用户ID (VBAR_DRV_FACE_USERID_SIZE 长度) */
    char userid[VBAR_DRV_FACE_USERID_SIZE];
};

/**
 * @struct vbar_drv_face_flist_select_result
 * @brief 特征库查询结果结构体
 * @details 用于遍历特征库时的回调参数
 */
struct vbar_drv_face_flist_select_result {
    /** @brief 特征库最大容量 */
    int max;
    /** @brief 当前特征库条目总数 */
    int total;
    /** @brief 当前遍历索引位置 */
    int current;
    /** @brief 当前用户ID (VBAR_DRV_FACE_USERID_SIZE 长度) */
    char userid[VBAR_DRV_FACE_USERID_SIZE];
};

/**
 * @typedef vbar_drv_face_flist_select_cb
 * @brief 特征库遍历回调函数类型
 * @param result 查询结果指针
 * @param pdata 用户自定义数据指针
 * @return 操作状态码
 * @retval 0 继续遍历
 * @retval 非零 终止遍历
 */
typedef int (*vbar_drv_face_flist_select_cb)(struct vbar_drv_face_flist_select_result *, void *);

#ifdef	__cplusplus
extern "C" {
#endif

/**
 * @brief 注册人脸特征值到特征库
 * @param userid 用户标识符（长度不超过VBAR_DRV_FACE_USERID_SIZE）
 * @param feature 人脸特征值指针（1024字节数组）
 * @return 操作状态码
 * @retval 0 注册成功
 * @retval -1 参数无效或特征库已满
 */
VBARLIB_API int vbar_drv_face_features_register(const char *userid, const uint8_t *feature);

/**
 * @brief 从特征库注销指定用户
 * @param userid 要注销的用户标识符
 * @return 操作状态码
 * @retval 0 注销成功
 * @retval -1 用户不存在
 */
VBARLIB_API int vbar_drv_face_features_unregister(const char *userid);

/**
 * @brief 更新特征库索引
 * @return 操作状态码
 * @retval 0 更新成功
 * @retval -1 特征库未初始化
 */
VBARLIB_API int vbar_drv_face_features_update(void);

/**
 * @brief 遍历特征库条目
 * @param select_cb 遍历回调函数（每次调用返回一个条目）
 * @param pdata 用户自定义数据指针
 * @return 操作状态码
 * @retval 0 遍历完成
 * @retval -1 回调函数为空
 */
VBARLIB_API int vbar_drv_face_face_feature_select(vbar_drv_face_flist_select_cb select_cb, void *pdata);

VBARLIB_API int vbar_drv_face_feature_select_feature_by_userid(const char *userid, uint8_t *feature);

/**
 * @brief 清空特征库所有条目
 * @return 操作状态码
 * @retval 0 清空成功
 * @retval -1 特征库未初始化
 */
VBARLIB_API int vbar_drv_face_features_clean(void);

/**
 * @brief 调试用-打印特征库注册信息
 * @note 该函数仅在调试版本有效
 */
VBARLIB_API void vbar_drv_face_features_register_dump(void);

/**
 * @brief 进行人脸特征值比对
 * @param feature 要比对的特征值指针（1024字节数组）
 * @param result 输出参数，接收比对结果
 * @return 操作状态码
 * @retval 0 比对成功
 * @retval -1 参数无效或特征库为空
 */
VBARLIB_API int vbar_drv_face_feature_compare(const uint8_t *feature,
                                              struct vbar_drv_face_cmp_result *result);

#ifdef __cplusplus
}
#endif

/* 内部函数，不对外 */
int vbar_drv_face_detection_callback(struct vbar_drv_face_analysis_result *result);
int vbar_drv_face_recognition_callback(struct vbar_drv_face_analysis_result *result);
int vbar_drv_face_nir_detection_callback(struct vbar_drv_face_analysis_result *result);

#endif /* __VBAR_CAPTURER_H__ */
