#ifndef __LV_PORT_INDEV_H__
#define __LV_PORT_INDEV_H__

#include <common/common.hpp>
#ifdef	__cplusplus
extern "C" {
#endif
int vbar_m_lvgl_port_indev_init(void);

void vbar_m_lvgl_port_indev_deinit(void);
#ifdef __cplusplus
}
#endif
#endif
