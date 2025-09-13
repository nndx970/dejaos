#ifndef __VBAR_COMMON_FILEOPS_H__
#define __VBAR_COMMON_FILEOPS_H__

#include <vbar/export.h>
#include <stdint.h>
#include <fcntl.h>

#ifdef	__cplusplus
extern "C" {
#endif

VBAR_EXPORT_API long vbar_fileops_get_size(const char* path);

VBAR_EXPORT_API ssize_t vbar_fileops_save_binary(const char *path, const unsigned char *data, size_t datalen);

VBAR_EXPORT_API int vbar_fileops_write2file(const char *path, const unsigned char *data, size_t datalen);

VBAR_EXPORT_API int vbar_fileops_read_file(const char *file_path, unsigned char *buf, int read_len);

VBAR_EXPORT_API int vbar_fileops_open(const char *path, int flags);

VBAR_EXPORT_API int vbar_fileops_write(int fd, const uint8_t *data, size_t datalen);

VBAR_EXPORT_API int vbar_fileops_lseek(int fd, off_t offset, int whence);

VBAR_EXPORT_API void vbar_fileops_close(int fd);

VBAR_EXPORT_API int vbar_fileops_read(int fd, uint8_t *data, size_t datalen);

VBAR_EXPORT_API int vbar_fileops_exist(const char *path);

VBAR_EXPORT_API int vbar_fileops_remove(const char *path);

VBAR_EXPORT_API int vbar_fileops_file_read(char *file_path, char *buf, int buf_len);

VBAR_EXPORT_API int vbar_fileops_file_is_image(const char *file, const char *ext);

#ifdef	__cplusplus
}
#endif

#endif
