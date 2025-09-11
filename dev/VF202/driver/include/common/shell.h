#ifndef __VBAR_COMMON_SHELL_H__
#define __VBAR_COMMON_SHELL_H__

#include <sys/stat.h>
#include <sys/types.h>
#include <vbar/export.h>
#include <stdint.h>

#ifdef	__cplusplus
extern "C" {
#endif

VBAR_EXPORT_API int vbar_mkdir(const char *path, mode_t mode);

VBAR_EXPORT_API int vbar_system(const char *cmd);

VBAR_EXPORT_API int vbar_system_brief(const char *cmd);

VBAR_EXPORT_API int vbar_system_with_res(const char *cmd, char *result, uint32_t res_len);

/**
 * @brief vbar_system_blocked 阻塞执行cmd，并获取cmd exit code
 *
 * @param cmd 执行的命令
 *
 * @return >0 cmd exit code , -1 出错
 */
VBAR_EXPORT_API int vbar_system_blocked(const char *cmd);

/**
 * @brief vbar_async_reboot 异步重启，在、非阻塞执行sync 和 reboot
 *
 * @param delay_s 延时重启的时间
 *
 * @return :0 成功 , -1 出错
 */
VBAR_EXPORT_API int vbar_async_reboot(int delay_s);

VBAR_EXPORT_API int vbar_async_exec(void (*func)(void *), void *pdata, int delay_ms);

/**
 * @brief vbar_path_dir_check 检查一个文件路径的目录是否存在，不存在则创建
 *
 * @param file_path 文件的路径
 *
 * @return :0 成功 , -1 出错
 */
VBAR_EXPORT_API int vbar_filepath_dir_check(const char *file_path);
#ifdef	__cplusplus
}
#endif

#endif
