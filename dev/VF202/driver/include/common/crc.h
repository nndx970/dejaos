#ifndef __VBAR_COMMON_CRC_H__
#define __VBAR_COMMON_CRC_H__

#define VBAR_LIB
#include <vbar/export.h>

#ifdef	__cplusplus
extern "C" {
#endif
#define VBAR_CRC8_TABLE_SIZE			256

VBAR_EXPORT_API void vbar_crc8_populate_msb(uint8_t table[VBAR_CRC8_TABLE_SIZE],
                       uint8_t polynomial);

VBAR_EXPORT_API void vbar_crc8_populate_lsb(uint8_t table[VBAR_CRC8_TABLE_SIZE],
                       uint8_t polynomial);

VBAR_EXPORT_API uint8_t vbar_crc8(const uint8_t table[VBAR_CRC8_TABLE_SIZE],
             uint8_t *pdata, size_t nbytes,
             uint8_t crc);

VBAR_EXPORT_API uint16_t vbar_crc16(uint16_t crc, const uint8_t *buffer, size_t len);

#ifdef	__cplusplus
}
#endif

#endif
