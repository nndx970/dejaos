#ifndef __VBAR_COMMON_HMAC_MD5_H__
#define __VBAR_COMMON_HMAC_MD5_H__

#include <stdint.h>
#include <vbar/export.h>

#ifdef	__cplusplus
extern "C" {
#endif

VBAR_EXPORT_API int vbar_md5_hash(const uint8_t *input, uint32_t inputlen, uint8_t *output);

VBAR_EXPORT_API int vbar_md5_hash_file(const char *file, uint8_t *md5);

VBAR_EXPORT_API int vbar_hmac_md5_hash(const uint8_t* input, uint32_t inputlen,
                                   const uint8_t* key, uint32_t keylen, uint8_t* output);

VBAR_EXPORT_API int vbar_hmac_md5_hash_file(const char *file, const uint8_t *key, uint32_t keylen, uint8_t *output);

#ifdef	__cplusplus
}
#endif



#endif
