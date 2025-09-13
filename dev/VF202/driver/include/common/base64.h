#ifndef __VBAR_COMMON_BASE64_H__
#define __VBAR_COMMON_BASE64_H__

#include <stdio.h>
#include <vbar/export.h>

#ifdef	__cplusplus
extern "C" {
#endif

VBAR_EXPORT_API unsigned char *vbar_base64_decode(const char *src, size_t len, size_t *decsize);

VBAR_EXPORT_API char *vbar_base64_encode(const unsigned char *src, size_t len);

VBAR_EXPORT_API int vbar_base64_2binfile(const char *file_path, const char *base64);

VBAR_EXPORT_API int vbar_binfile_2base64(const char *file_path, char **base64data);

#ifdef	__cplusplus
}
#endif

#endif
