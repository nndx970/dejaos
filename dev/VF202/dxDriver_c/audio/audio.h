#ifndef __VBAR_M_AUDIO_H__
#define __VBAR_M_AUDIO_H__

#include <stdint.h>
#include <vbar/export.h>

#ifdef  __cplusplus
extern "C" {
#endif

#define AUDIO_PLAY_FAILED               -1
#define AUDIO_PLAY_QUEUE_IS_FULL        -2
#define AUDIO_PLAY_SUCCESS               0

/* 音频配置结构体 */
struct vbar_m_audio_config {
    /*** ALSAPLAY 配置成员 (如未用alsaplay, 则给默认值) ***/
    /* 声卡编号 默认值: 0 */
    uint32_t card;
    /* 设备编号 默认值: 0 */
    uint32_t device;
    /* 混音器控制编号 默认值: 3 */
    uint32_t mixer_ctl_num;

    /*** ALSA 配置成员(如未用alsa, 则给默认值) ***/
    /* ALSA 周期大小 */
    uint32_t period_size;
    /* ALSA 缓冲区大小 */
    uint32_t buffer_size;
    /* PCM 设备名称 */
    char pcm_name[32];

    /* 播放的初始默认音量 */
    uint8_t volume;
};

/**
 * @brief 音频播放模式枚举
 *
 */
enum vbar_m_audio_play_mode {
    VBAR_M_AUDIO_PLAY_BY_WAV_PATH = 0, /** 使用 WAV 文件路径进行播放 */
    VBAR_M_AUDIO_PLAY_BY_TXT_DATA,      /** 使用文本数据合成音频播放 */
    VBAR_M_AUDIO_PLAY_BY_WAV_DATA     /** 使用 wav 文件，音频数据进行播放 */
};

/**
 * @brief 音频播放语言类型枚举
 *
 */
enum vbar_m_audio_play_language_type {
    VBAR_M_AUDIO_CHINESE_DATA = 0,     /** 中文 */
    VBAR_M_AUDIO_ENGLISH_DATA          /** 英文 */
};

/* 音频播放时，需要的音频参数 */
struct vbar_m_audio_play_params {
    /* 播放模式 */
    enum vbar_m_audio_play_mode play_mode;
    /* 播放数据(音频路径/tts转好的wav数据/txt数据) */
    char *wav_data;
    /* 数据长度(传wav_data时，传入wav_data_len长度。 传路径或txt_data时, 使用其字符串长度) */
    uint32_t wav_data_len;
    /* 语言类型(仅有txt播放需要赋予对应值，其他播放方式给个默认值即可) */
    enum vbar_m_audio_play_language_type language_type;
};


/**
 * @brief modules 模块音频句柄结构体
 *
 */
struct vbar_m_audio_handle;

/**
 * @brief audio 初始化接口
 *
 * @param audio_config      音频播放所需配置
 *
 * @return  0 成功 -1 失败
 */
VBARLIB_API struct vbar_m_audio_handle *vbar_m_audio_init(struct vbar_m_audio_config *audio_config);

/**
 * @brief 句柄销毁函数
 *
 * @param handle 句柄
 *
 * @return
 */
VBARLIB_API void vbar_m_audio_deinit(struct vbar_m_audio_handle *handle);

/**
 * @brief 判断是否是有效音量
 *
 * @param volume 输入的音量
 *
 * @return true 有效  false 无效
 */
VBARLIB_API bool vbar_m_audio_is_valid_volume(uint32_t volume);

/**
 * @brief 获取音频音量调节的范围
 *
 * @param handle 音频句柄
 * @param max    音频音量最大值
 * @param min    音频音量最小值
 *
 * @return 成功 0, 失败 -1
 */
VBARLIB_API int vbar_m_audio_get_volume_range(struct vbar_m_audio_handle *handle, uint32_t *max, uint32_t *min);

/**
 * @brief 获取音频音量
 *
 * @param handle 句柄
 *
 * @return 返回音频当前音量值
 */
VBARLIB_API int vbar_m_audio_get_volume(struct vbar_m_audio_handle *handle);

/**
 * @brief 设置音频音量
 *
 * @param handle 音频句柄
 * @param volume_num 音量大小（0-255）
 *
 * @return 0 设置音量成功 -1 设置音量失败
 */
VBARLIB_API int vbar_m_audio_set_volume(struct vbar_m_audio_handle *handle, uint32_t volume_num);

/**
 * @brief 播放音频接口
 *
 * @param handle            音频句柄
 * @param play_params       播放时所需参数
 *
 * @return 成功返回AUDIO_PLAY_SUCCESS , 失败返回AUDIO_PLAY_FAILED, 播放队列已满AUDIO_PLAY_QUEUE_IS_FULL
 */
VBARLIB_API int vbar_m_audio_play(struct vbar_m_audio_handle *handle, struct vbar_m_audio_play_params *play_params);

/**
 * @brief 中断当前正在播放的音频
 *
 * @param handle 音频句柄
 *
 * @return 0 设置中断成功 -1 失败
 */
VBARLIB_API int vbar_m_audio_playing_interrupt(struct vbar_m_audio_handle *handle);

/**
 * @brief 清空播放缓存(这个接口要与vbar_m_audio_play/vbar_m_audio_play_data, 做互斥操作)
 *
 * @param handle 音频句柄
 *
 * @return 0 成功， -1 失败
 */
VBARLIB_API int vbar_m_audio_clear_play_cache(struct vbar_m_audio_handle *handle);

#ifdef  __cplusplus
}
#endif

#endif
