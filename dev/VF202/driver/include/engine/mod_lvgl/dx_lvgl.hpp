#ifndef __DX_LVGL_H__
#define __DX_LVGL_H__

#include <common/common.hpp>

#ifdef	__cplusplus
extern "C" {
#endif
uint32_t custom_tick_get();

int vbar_m_lvgl_init();

void vbar_m_lvgl_deinit(void);
#ifdef __cplusplus
}
#endif

#endif
