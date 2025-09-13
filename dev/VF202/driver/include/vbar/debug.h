#ifndef __VBAR_DEBUG_H__
#define __VBAR_DEBUG_H__

#include <errno.h>
#include <stdint.h>
#include <stdarg.h>

#ifdef CONFIG_VBAR_MOD_SYSLOG
#include <modules/vbar_log.h>
#endif

#include <vbar/config.h>
#include <vbar/vbar_status.h>
/*
 * 调试工具定义(日志/断言)
 *      VBAR_APP_DEBUG定义日志输出级别:
 *      1(仅error: 影响程序正常运行，如句柄打开失败等),
 *      2(error/warn: 重要api调用失败等情况),
 *      3(error/warn/info：调试log)...
 *      模块宏定义eg:
 *      #ifdef XXX_DEBUG
 *          "ERROR: redefine XXX_DEBUG"
 *      #else
 *          #define XXX_DEBUG  1
 *      #endif
 */

/*终端文字颜色控制*/
#define PRINT_COL_NONE            "\e[0m"
#define PRINT_COL_BLACK           "\e[0;30m"
#define PRINT_COL_L_BLACK         "\e[1;30m"
#define PRINT_COL_RED             "\e[0;31m"
#define PRINT_COL_L_RED           "\e[1;31m"
#define PRINT_COL_GREEN           "\e[0;32m"
#define PRINT_COL_L_GREEN         "\e[1;32m"
#define PRINT_COL_BROWN           "\e[0;33m"
#define PRINT_COL_YELLOW          "\e[1;33m"
#define PRINT_COL_BLUE            "\e[0;34m"
#define PRINT_COL_L_BLUE          "\e[1;34m"
#define PRINT_COL_PURPLE          "\e[0;35m"
#define PRINT_COL_L_PURPLE        "\e[1;35m"
#define PRINT_COL_CYAN            "\e[0;36m"
#define PRINT_COL_L_CYAN          "\e[1;36m"
#define PRINT_COL_GRAY            "\e[0;37m"
#define PRINT_COL_WHITE           "\e[1;37m"
#define PRINT_COL_BOLD            "\e[1m"
#define PRINT_COL_UNDERLINE       "\e[4m"
#define PRINT_COL_BLINK           "\e[5m"
#define PRINT_COL_REVERSE         "\e[7m"
#define PRINT_COL_HIDE            "\e[8m"
#define PRINT_COL_CLEAR           "\e[2J"
#define PRINT_COL_CLRLINE         "\r\e[K" //or "\e[1K\r"

#define UNUSED __attribute__((__unused__))

#define VBAR_APP_DEBUG 0

#ifdef VBAR_NDEBUG
#undef VBAR_APP_DEBUG
#endif

#if defined(VBAR_APP_DEBUG) && defined(NDEBUG)
#error "调试开关冲突, 请检查编译参数"
#endif

#include "vbar/auxiliary.h"
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#ifdef  __cplusplus
extern "C" {
#endif

#ifdef VBAR_APP_DEBUG

/* 日志打印宏定义 */
#if defined(_WIN32) || defined(_WIN64)
#define __vbar_debug_printf(flag, fmt, ...) do { \
    fprintf(stderr, "[%12I64u.%03I64u]: %d|%s()|%s: ", \
                    (vbar_clock() / VBAR_CLOCKS_PER_SEC), (vbar_clock() % VBAR_CLOCKS_PER_SEC), \
                    __LINE__, __func__, flag); \
    fprintf(stderr, fmt, ##__VA_ARGS__); \
} while (0)
#else
#define __vbar_debug_printf(flag, fmt, ...) do { \
    fprintf(stderr, "[%12" PRIu64 ".%03" PRIu64 "]: %d|%s()|%s: ", \
                    vbar_clock() / VBAR_CLOCKS_PER_SEC, vbar_clock() % VBAR_CLOCKS_PER_SEC, \
                    __LINE__, __func__, flag); \
    fprintf(stderr, fmt, ##__VA_ARGS__); \
} while (0)
#endif

#ifdef CONFIG_VBAR_MOD_SYSLOG
#define vbar_syslog(level, fmt, ...)
#else
#define vbar_syslog(level, fmt, ...)
#endif

/* 这里是syslog测试------------------------------------------------------------- */
#ifdef CONFIG_VBAR_MOD_SYSLOG

/* 错误级别日志打印 */
#define vbar_error_test(marker, fmt, ...) do { \
    if (vbar_m_vbarlog_get_module_level(marker) >= 1) {\
        __vbar_debug_printf(PRINT_COL_RED "error" PRINT_COL_NONE, fmt, ##__VA_ARGS__); \
        vbar_m_syslog_error("%s |%s():%d:error| " fmt, marker, __func__,  __LINE__,##__VA_ARGS__); \
    }\
} while (0)

/* 警告级别日志打印 */
#define vbar_warn_test(marker, fmt, ...) do { \
    if (vbar_m_vbarlog_get_module_level(marker) >= 2) {\
        __vbar_debug_printf(PRINT_COL_YELLOW "warn" PRINT_COL_NONE, fmt, ##__VA_ARGS__); \
        vbar_m_syslog_warn("%s |%s():%d:warn| " fmt, marker, __func__,  __LINE__,##__VA_ARGS__); \
    }\
} while (0)

/* 调试级别日志打印 */
#define vbar_info_test(marker, fmt, ...) do { \
    if (vbar_m_vbarlog_get_module_level(marker) >= 3) {\
        __vbar_debug_printf(PRINT_COL_GREEN "info" PRINT_COL_NONE, fmt, ##__VA_ARGS__); \
        vbar_m_syslog_info("%s |%s():%d:info| " fmt, marker, __func__,  __LINE__,##__VA_ARGS__); \
    }\
} while (0)

#else

#define vbar_error_test(marker, fmt, ...) do { \
    if (marker > 0) {\
        __vbar_debug_printf(PRINT_COL_RED "error" PRINT_COL_NONE, fmt, ##__VA_ARGS__);\
    }\
} while (0)

/* 警告级别日志打印 */
#define vbar_warn_test(marker, fmt, ...) do { \
    if (marker > 1) {\
        __vbar_debug_printf(PRINT_COL_YELLOW "warn" PRINT_COL_NONE, fmt, ##__VA_ARGS__);\
    }\
} while (0)

/* 调试级别日志打印 */
#define vbar_info_test(marker, fmt, ...) do { \
    if (marker > 2) {\
        __vbar_debug_printf(PRINT_COL_GREEN "info" PRINT_COL_NONE, fmt, ##__VA_ARGS__);\
    }\
} while (0)

#endif /* CONFIG_VBAR_MOD_SYSLOG */
/*------------------------------------------------------------- */



/* 错误级别日志打印 */
#define vbar_error(level, fmt, ...) do { \
    if (level > 0) {\
        __vbar_debug_printf(PRINT_COL_RED"error" PRINT_COL_NONE, fmt, ##__VA_ARGS__); \
        vbar_syslog(LOG_ERR, "%s()|%d|ERR: " fmt, __func__, __LINE__,##__VA_ARGS__); \
    }\
} while (0)

#if (VBAR_APP_RELEASE==1)
#define vbar_warn(level, fmt, ...) vbar_syslog(LOG_WARNING, "%s()|%d|WARNING: " fmt, __func__, __LINE__,##__VA_ARGS__);
#else
/* 警告级别日志打印 */
#define vbar_warn(level, fmt, ...) do { \
    if (level > 1) {\
        __vbar_debug_printf(PRINT_COL_YELLOW"warn" PRINT_COL_NONE, fmt, ##__VA_ARGS__); \
        vbar_syslog(LOG_WARNING, "%s()|%d|WARNING: " fmt, __func__, __LINE__,##__VA_ARGS__); \
    }\
} while (0)
#endif

#if (VBAR_APP_RELEASE==1)
#define vbar_info(level, fmt, ...)
#else
/* 调试级别日志打印 */
#define vbar_info(level, fmt, ...) do { \
    if (level > 2) {\
        __vbar_debug_printf(PRINT_COL_GREEN"info" PRINT_COL_NONE, fmt, ##__VA_ARGS__); \
        vbar_syslog(LOG_INFO, "%s()|%d|INFO: " fmt, __func__, __LINE__,##__VA_ARGS__); \
    }\
} while (0)
#endif

/* 断言 - 通用 */
#if VBAR_APP_DEBUG > 0
#define vbar_assert(_e) do { \
    if (!(_e)) \
        __vbar_debug_printf("VBar Assert failed:", "%s\n", #_e);\
        abort();\
} while (0)
#else
#define vbar_assert(_e) do { \
    if (!(_e)) \
        __vbar_debug_printf("VBar Assert failed:", "%s\n", #_e);\
} while (0)
#endif
/* 断言 - 入口参数及返回结果测试 */
#define vbar_assert_param(_e)   vbar_assert(_e)

/* 出现BUG */
#define VBAR_BUG_ON()           vbar_assert(0)

/* 警告 */
#define VBAR_WARN_ON()          vbar_warn("0\n")

/* 标记 */
#define VBAR_TRACE()            __vbar_debug_printf("trace", " ")

/* 终止函数定义 */
#define vbar_abort()            VBAR_BUG_ON()

static inline void __vbar_hexdump(const unsigned char *data, int length) {
    int i;
    fprintf(stderr, "       ");
    for (i = 0; i < length; ++i) {
        fprintf(stderr, "%02x ", data[i]);
        if (((i + 1) % 8) == 0 && length > 8)
            fprintf(stderr, "\n       ");
    }
    fprintf(stderr, "\n");
}

#define __vbar_hexdump_(data_point, length, fmt, ...)  do {\
    fprintf(stderr, fmt, ##__VA_ARGS__); \
    long long i; \
    fprintf(stderr, "       "); \
    for (i = 0; i < length; ++i) { \
        fprintf(stderr, "%02x ", (data_point)[i]);\
        if (((i + 1) % 8) == 0 && length > 8)\
            fprintf(stderr, "\n       ");\
    }\
    fprintf(stderr, "\n");\
}while(0)

#if (VBAR_APP_RELEASE==1)
#define vbar_hexdump(level, title, data, len)
#define vbar_hexdump_var(level, data, len, fmt, ...)
#else
#define vbar_hexdump(level, title, data, len) do { \
    if (level > 2) {\
        __vbar_debug_printf(PRINT_COL_GREEN"info" PRINT_COL_NONE, "%s: \n", title); \
        __vbar_hexdump(data, len); \
    }\
} while (0)

#define vbar_hexdump_var(level, data, len, fmt, ...) do {\
    if (level > 2) \
    __vbar_hexdump_(data, len, fmt, ##__VA_ARGS__);\
} while(0)
#endif /* (VBAR_APP_RELEASE==1) */

#define vbar_hexdump_var_error(data, len, fmt, ...) do {\
    fprintf(stderr, "[%12" PRIu64 ".%03" PRIu64 "]: %d|%s()|%s: ", \
                    vbar_clock() / VBAR_CLOCKS_PER_SEC, vbar_clock() % VBAR_CLOCKS_PER_SEC, \
                    __LINE__, __func__, "error"); \
    __vbar_hexdump_(data, len, fmt, ##__VA_ARGS__);\
} while(0)

#endif /* VBAR_APP_DEBUG */

#include <stdlib.h>

#ifndef vbar_error
#define vbar_error(fmt, ...)
#endif

#ifndef vbar_warn
#define vbar_warn(fmt, ...)
#endif

#ifndef vbar_info
#define vbar_info(fmt, ...)
#endif

#ifndef vbar_assert
#define vbar_assert(_e)
#endif

#ifndef vbar_assert_param
#define vbar_assert_param(_e)
#endif

#ifndef VBAR_BUG_ON
#define VBAR_BUG_ON()
#endif

#ifndef VBAR_WARN_ON
#define VBAR_WARN_ON()
#endif

#ifndef vbar_hexdump
#define vbar_hexdump(lv, t, d, l)
#endif

/* 终止函数定义 */
#ifndef  vbar_abort
#define vbar_abort()    abort()
#endif

#if defined(_WIN32) || defined(_WIN64)
#define __vbar_vbar_log(module, status) do { \
    fprintf(stderr, "[%12I64u.%03I64u]:%s %s\n", \
                    vbar_clock() / VBAR_CLOCKS_PER_SEC, vbar_clock() % VBAR_CLOCKS_PER_SEC, \
                    module, status); \
} while (0)
#else
#define __vbar_vbar_log(module, status) do { \
    fprintf(stderr, "[%12" PRIu64 ".%03" PRIu64 "]: %s %s\n", \
                    vbar_clock() / VBAR_CLOCKS_PER_SEC, vbar_clock() % VBAR_CLOCKS_PER_SEC, \
                    module, status); \
} while (0)
#endif

/* 初始化成功log */
#ifndef  vbar_log_ok
#define vbar_log_ok(module) __vbar_vbar_log(module, "init success")
#endif

/* 初始化失败log */
#ifndef  vbar_log_no
#define vbar_log_no(module) __vbar_vbar_log(module, "init failed !!!!")
#endif

#if defined(__GNUC__) && defined(__linux__)
#define SYSTEM_EXE(str_cmd)     system(str_cmd)
#else
#define SYSTEM_EXE(str_cmd)
#endif

#include <string.h>
#include <mcheck.h>

#define vbar_mtrace() \
do { \
    if (strncmp("/vgapp/debug/mem.log", getenv("MALLOC_TRACE"), strlen("/vgapp/debug/mem.log")) != 0) { \
        setenv("MALLOC_TRACE", "/vgapp/debug/mem.log", true); \
    } \
    vbar_error(3, "mtrace\n"); \
    mtrace(); \
} while (0);

#define vbar_muntrace() \
do { \
    muntrace(); \
    if (strncmp("/vgapp/debug/mem.log", getenv("MALLOC_TRACE"), strlen("/vgapp/debug/mem.log")) != 0) { \
        unsetenv("MALLOC_TRACE"); \
    } \
    vbar_error(3, "muntrace\n"); \
} while (0);




#ifdef  __cplusplus
}
#endif

#endif /* __VBAR_APP_DEBUG_H__ */
