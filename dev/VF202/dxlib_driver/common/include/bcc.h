#ifndef __VBAR_COMMON_BCC_H__
#define __VBAR_COMMON_BCC_H__

#include <vbar/export.h>
#include <stdint.h>

#ifdef	__cplusplus
extern "C" {
#endif

VBAR_EXPORT_API uint8_t vbar_calculate_bcc(uint8_t bcc, uint8_t *data, uint32_t datalen);

#ifdef __cplusplus
}
#endif

#endif
