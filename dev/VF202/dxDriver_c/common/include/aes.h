#ifndef __VBAR_COMMON_AES_H__
#define __VBAR_COMMON_AES_H__

#include <vbar/export.h>

#ifdef	__cplusplus
extern "C" {
#endif

VBAR_EXPORT_API void vbar_aes128_ecb_encrypt(const uint8_t* input, const uint8_t* key, uint8_t* output);

VBAR_EXPORT_API void vbar_aes128_ecb_decrypt(const uint8_t* input, const uint8_t* key, uint8_t *output);

VBAR_EXPORT_API void vbar_aes128_cbc_encrypt_buffer(uint8_t* output, uint8_t* input, uint32_t length, const uint8_t* key, const uint8_t* iv);

VBAR_EXPORT_API void vbar_aes128_cbc_decrypt_buffer(uint8_t* output, uint8_t* input, uint32_t length, const uint8_t* key, const uint8_t* iv);

#ifdef	__cplusplus
}
#endif

#endif
