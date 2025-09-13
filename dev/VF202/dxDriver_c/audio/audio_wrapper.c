#include "audio.h"
#include <string.h>
#include <stdio.h>

/* 音频配置结构体 */
static struct vbar_m_audio_config config = {
    /*** ALSAPLAY 配置成员 (如未用alsaplay, 则给默认值) ***/
    /* 声卡编号 默认值: 0 */
    .card = 0,
    /* 设备编号 默认值: 0 */
    .device = 0,
    /* 混音器控制编号 默认值: 3 */
    .mixer_ctl_num = 3,

    /*** AUDIO 配置成员(如未用alsa, 则给默认值) ***/
    /* AUDIO 周期大小 */
    .period_size = 512,
    /* AUDIO 缓冲区大小 */
    .buffer_size = 2048,
    /* PCM 设备名称 */
    .pcm_name = "default",

    /* 播放的初始默认音量 音量大小（0-255） */
    .volume = 30};

struct vbar_m_audio_handle *audio_handle;

void audio_init(int volume)
{
    if (audio_handle)
    {
        return;
    }
    config.volume = volume;
    audio_handle = vbar_m_audio_init(&config);
}

void audio_deinit()
{
    if (audio_handle)
    {
        vbar_m_audio_deinit(audio_handle);
        audio_handle = NULL;
    }
}

void audio_play(char *wav_path)
{
    struct vbar_m_audio_play_params play_params = {
        .play_mode = VBAR_M_AUDIO_PLAY_BY_WAV_PATH,
        .wav_data = wav_path,
        .wav_data_len = strlen(wav_path),
        .language_type = VBAR_M_AUDIO_CHINESE_DATA,
    };
    vbar_m_audio_play(audio_handle, &play_params);
}

void audio_playing_interrupt()
{
    vbar_m_audio_playing_interrupt(audio_handle);
}

int audio_get_volume()
{
    return vbar_m_audio_get_volume(audio_handle);
}

void audio_set_volume(int volume)
{
   vbar_m_audio_set_volume(audio_handle, volume);
}

int audio_get_volume_range(int *max, int *min)
{
    uint32_t max_val, min_val;
    int result = vbar_m_audio_get_volume_range(audio_handle, &max_val, &min_val);
    if (result == 0) {
        *max = (int)max_val;
        *min = (int)min_val;
    }
    return result;
}