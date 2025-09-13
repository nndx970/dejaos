#ifndef __VBAR_BASE_GAIN_H___
#define __VBAR_BASE_GAIN_H___

#include <stdint.h>

#include <vbar/debug.h>

struct vbar_drv_audio_gain {
    void (*set_volume)(uint32_t);
    uint32_t (*get_volume)(void);
    int (*apply)(void *src_addr, void *dst_addr, int size);
    void (*close)(struct vbar_drv_audio_gain *);
};

struct audio_gain_device {
    uint32_t channel;
    uint32_t bit_per_sample;
};

#ifdef	__cplusplus
extern "C" {
#endif

VBARLIB_API void vbar_drv_audio_gain_set_volume(uint32_t volume);

VBARLIB_API int vbar_drv_audio_gain_get_volume(void);

VBARLIB_API int vbar_drv_audio_gain_apply(void *src_addr, void *dst_addr, int size);

VBARLIB_API int vbar_drv_audio_gain_init(struct audio_gain_device *dev);

VBARLIB_API void vbar_drv_audio_gain_exit(void);

#ifdef	__cplusplus
}
#endif

#endif
