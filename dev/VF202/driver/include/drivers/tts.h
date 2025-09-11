#ifndef __VBAR_BASE_TTS_H__
#define __VBAR_BASE_TTS_H__

#include <stdint.h>
#include <vbar/export.h>
#ifdef  __cplusplus
extern "C" {
#endif

/* 该枚举用于指定输出中问候语的位置 */
enum vbar_drv_tts_greeting_pos {
    /* 不输出问候语 */
    VBAR_DRV_TTS_GREET_CLOSE = -1,
    /* 问候语在输出前面 */
    VBAR_DRV_TTS_GREET_HEAD_OPEN,
    /* 问候语在输出末尾 */
    VBAR_DRV_TTS_GREET_END_OPEN
};

/* 该结构体用于存储语音合成中问候语的相关参数 */
struct vbar_drv_tts_greet_config {
    /* 语言类型：0 表示中文，1 表示英文 */
    int language_type;
    /* 问候语 ID */
    int greet_id;
    /* 问候语的位置 */
    enum vbar_drv_tts_greeting_pos pos;
};

/* 该枚举用于指定语音合成输入文本的格式 */
enum vbar_drv_tts_txt_format {
    /* UTF-8 格式 */
    VBAR_DRV_TTS_FORMAT_UTF8 = 0,
    /* 枚举的最大值，用于范围检查 */
    VBAR_DRV_TTS_FORMAT_MAX
};

/* 该结构体用于存储语音合成所需的数据，包括文本格式、文本字符串、字符串长度和问候参数 */
struct vbar_drv_tts_config {
    /* 文本格式 */
    enum vbar_drv_tts_txt_format format;
    /* 文本字符串 */
    char *str;
    /* 文本字符串的长度 */
    int str_len;
    /* 问候参数 */
    struct vbar_drv_tts_greet_config greet_param;
};

struct vbar_drv_tts_handle {
    int (*str2wav)(struct vbar_drv_tts_config *tts, char *wav, int *wav_len, int buf_size);
    void (*deinit)(struct vbar_drv_tts_handle *);
    uint32_t ref_count;
};

/**
 * @brief 字符串转wav
 *
 * @param tts
 * @param wav_data wav数据buf，不可为空指针
 * @param buf_size wav_buf大小
 *
 * @return wav_len 成功 <=0 失败
 */
VBARLIB_API int vbar_drv_tts_get_wav_data(struct vbar_drv_tts_config *tts, char *wav_data, int buf_size);

/**
 * @brief 初始化语音合成
 *
 * 该函数用于初始化语音合成，准备进行文本到语音的转换。
 *
 * @return 成功返回 0，失败返回负值
 */
VBARLIB_API int vbar_drv_tts_init(void);

/**
 * @brief 反初始化语音合成
 *
 * 该函数用于反初始化语音合成，释放相关资源。
 */
VBARLIB_API void vbar_drv_tts_deinit(void);
#ifdef  __cplusplus
}
#endif


#endif
