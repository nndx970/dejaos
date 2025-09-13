#ifndef __VBAR_M_ALSA_H__
#define __VBAR_M_ALSA_H__

#include <pthread.h>
#include <stdint.h>
#include <vbar/export.h>

#ifdef  __cplusplus
extern "C" {
#endif

/* modules alsa 句柄 */
struct vbar_m_alsa_h;

/* 音频配置结构体 */
struct vbar_m_audio_config {
    /* 播放音量 */
    uint32_t volume;
    /* 录音音量（预留） */
    uint32_t record_volume;};

/* ALSA 配置结构体 */
struct vbar_m_alsa_config {
    /* ALSA 周期大小 */
    uint32_t period_size;
    /* ALSA 缓冲区大小 */
    uint32_t buffer_size;
    /* PCM 设备名称 */
    char pcm_name[32];
};

/**
 * 播放 WAV 文件
 *
 * @param handle ALSA 设备句柄
 * @param wav_path WAV 文件路径
 */
VBARLIB_API void vbar_m_alsa_wav(struct vbar_m_alsa_h *handle, const char *wav_path);

/**
 * 播放 WAV 数据
 *
 * @param handle ALSA 设备句柄
 * @param wav_data WAV 数据指针
 * @param data_len WAV 数据长度
 */
VBARLIB_API void vbar_m_alsa_wav_data(struct vbar_m_alsa_h *handle, const char *wav_data, uint32_t data_len);

/**
 * 设置音频配置
 *
 * @param handle ALSA 设备句柄
 * @param config 音频配置指针
 * @return 成功返回0，失败返回负值
 */
VBARLIB_API int vbar_m_alsa_set_config(struct vbar_m_alsa_h *handle, const struct vbar_m_audio_config *config);

/**
 * 获取音频配置
 *
 * @param handle ALSA 设备句柄
 * @param config 音频配置指针
 * @return 成功返回0，失败返回负值
 */
VBARLIB_API int vbar_m_alsa_get_config(struct vbar_m_alsa_h *handle, struct vbar_m_audio_config *config);

/**
 * 获取当前音量
 *
 * @param handle ALSA 设备句柄
 * @return 返回当前音量值
 */
VBARLIB_API int vbar_m_alsa_get_volume(struct vbar_m_alsa_h *handle);

/**
 * 设置音量
 *
 * @param handle ALSA 设备句柄
 * @param volume 需要设置的音量值
 * @return 成功返回0，失败返回负值
 */
VBARLIB_API int vbar_m_alsa_set_volume(struct vbar_m_alsa_h *handle, uint8_t volume);

/**
 * 初始化 ALSA 设备
 *
 * @param config 音频配置指针
 * @param handle_config ALSA 配置指针
 * @return 返回 ALSA 设备句柄
 */
VBARLIB_API struct vbar_m_alsa_h *vbar_m_alsa_init(const struct vbar_m_audio_config *config,
                                                   const struct vbar_m_alsa_config *alsa_config);

/**
 * 反初始化 ALSA 设备
 *
 * @param handle ALSA 设备句柄
 */
VBARLIB_API void vbar_m_alsa_deinit(struct vbar_m_alsa_h *handle);

#ifdef  __cplusplus
}
#endif


#endif
