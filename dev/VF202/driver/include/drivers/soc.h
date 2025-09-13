#ifndef __VBAR_DRV_SOC_H__
#define __VBAR_DRV_SOC_H__

#include <stdint.h>
#include <vbar/export.h>

#ifdef	__cplusplus
extern "C" {
#endif

VBARLIB_API int vbar_drv_soc_init(void);

VBARLIB_API void vbar_drv_soc_deinit(void);

VBARLIB_API int vbar_drv_soc_get_chip_id(char *id, uint32_t idlen);

VBARLIB_API int vbar_drv_soc_get_chip_userid(char *id, uint32_t idlen);

VBARLIB_API int vbar_drv_soc_get_chip_id2device_uuid(uint8_t *id, uint32_t idlen);
#ifdef	__cplusplus
}
#endif

#endif
