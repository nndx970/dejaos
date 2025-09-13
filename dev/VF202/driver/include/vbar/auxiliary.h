#ifndef __VBAR_AUXILIARY_H__
#define __VBAR_AUXILIARY_H__

/*
 * 辅助函数/宏定义, 兼容Windows/类Linux等平台
 */

/* 毫秒级睡眠函数void vbar_msleep(int ms)定义 */
#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
static inline void vbar_msleep(int ms) {
    Sleep(ms);
}
#elif defined(__linux__)
#include <time.h>
static inline void vbar_msleep(int ms) {
    struct timespec req, rem;
    req.tv_sec = ms / 1000;
    req.tv_nsec = (ms % 1000) * 1000000;

    while (nanosleep(&req, &rem) != 0)
        if ((rem.tv_sec != 0) || (rem.tv_nsec != 0))
            req = rem;
}
#else
#error "未支持的操作系统"
#endif

/* socket */
#if defined(_WIN32) || defined(_WIN64)
#include <winsock.h>
#else
#include <arpa/inet.h>
#endif

#include <stdint.h>
#include <sys/time.h>
#include <time.h>

/* 滴答时间类型 */
typedef uint64_t vbar_clock_t;

/* 全局滴答数 */
static inline vbar_clock_t vbar_clock(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (vbar_clock_t)(tv.tv_sec) * 1000 + (vbar_clock_t)(tv.tv_usec) / 1000;
}

/* 相对开机的滴答数(ms) */
static inline vbar_clock_t vbar_clock_ticks(void) {
    struct timespec tmspec;
    clock_gettime(CLOCK_MONOTONIC, &tmspec);
    return tmspec.tv_sec * 1000 + tmspec.tv_nsec / 1000000;
}


/* 每秒滴答数, 默认1000 */
#define VBAR_CLOCKS_PER_SEC     ((vbar_clock_t)1000)

/* 屏蔽禁用clock相关函数(不同平台易误用) */
#define clock_t
#define clock
#undef CLOCKS_PER_SEC

#include "vbar/export.h" /* for VBARLIB_API */

#ifdef	__cplusplus
extern "C" {
#endif

/* 当前进程保活(每进程只能运行一次) */
VBARLIB_API int vbar_process_keepalive(void);

/* 函数安全调用(新进程中调用并保活) */
VBARLIB_API int vbar_async_pcall(int (*pmain)(int, char **), int argc, char **argv);

/* 关闭进程 */
VBARLIB_API int vbar_async_kill(int pid);

#ifdef __cplusplus
}
#endif

#endif /* __VBAR_OWN_AUXILIARY_H__ */
