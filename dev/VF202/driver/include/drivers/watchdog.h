#ifndef __VBAR_DRV_WATCHDOG_H__
#define __VBAR_DRV_WATCHDOG_H__

#include <stdint.h>
#include <vbar/export.h>

/* 看门狗类型定义 */
enum vbar_drv_watchdog_type {
    /* 软件看门狗， bit 1 */
    VBAR_DRV_WATCHDOG_TYPE_SOFTWARE = 1,
    /* 硬件看门狗， bit 2 */
    VBAR_DRV_WATCHDOG_TYPE_HARDWARE = 2,
};

/* 看门狗句柄 */
struct vbar_drv_watchdog;

/* 看门狗配置结构体 */
struct vbar_drv_watchdog_config {
    /* 看门狗类型 */
    enum vbar_drv_watchdog_type type;
    /* 硬件看门狗 gpio ctrl 方法 */
    void (*gpio_set_value)(uint8_t, void *);
    /* 硬件看门狗 gpio_init 方法 */
    void (*gpio_init)(void *);
    /* 硬件看门狗 gpio_deinit 方法 */
    void (*gpio_deinit)(void *);
    /* 私有数据 */
    void *pdata;
};

#ifdef	__cplusplus
extern "C" {
#endif

/**
 * @brief 打开看门狗设备
 *
 * 该函数根据提供的配置初始化并打开一个看门狗设备
 *
 * @param config 指向看门狗配置结构体的指针，包含设备初始化所需的信息
 *
 * @return 成功时返回指向初始化的看门狗设备结构体的指针，失败时返回NULL
 */
VBARLIB_API struct vbar_drv_watchdog *vbar_drv_watchdog_open(struct vbar_drv_watchdog_config *config);

/**
 * @brief 检查看门狗设备是否已通电
 *
 * 该函数检查指定的看门狗设备是否当前处于通电状态
 *
 * @param wdt 指向看门狗设备结构体的指针
 *
 * @return 如果设备已通电则返回true，否则返回false
 */
VBARLIB_API bool vbar_drv_watchdog_is_poweron(struct vbar_drv_watchdog *wdt);

/**
 * @brief 启动看门狗计时器
 *
 * 该函数启动看门狗设备的计时器，如果在指定的超时时间内没有重新启动计时器，系统将复位
 *
 * @param wdt 指向看门狗设备结构体的指针
 * @param timeout_ms 计时器的超时时间，单位为毫秒
 *
 * @return 成功时返回true，否则返回false
 */
VBARLIB_API bool vbar_drv_watchdog_start(struct vbar_drv_watchdog *wdt, int timeout_ms);

/**
 * @brief 停止看门狗计时器
 *
 * 该函数停止看门狗设备的计时器，防止系统复位
 *
 * @param wdt 指向看门狗设备结构体的指针
 */
VBARLIB_API void vbar_drv_watchdog_stop(struct vbar_drv_watchdog *wdt);

/**
 * @brief 使能或禁用指定通道的看门狗功能
 *
 * 该函数可以设置看门狗设备的指定通道为使能或禁用状态
 *
 * @param wdt 指向看门狗设备结构体的指针
 * @param chan 要设置的通道编号
 * @param toset 如果为true则使能通道，如果为false则禁用通道
 *
 * @return 成功时返回true，否则返回false
 */
VBARLIB_API bool vbar_drv_watchdog_enable(struct vbar_drv_watchdog *wdt, unsigned int chan, bool toset);

/**
 * @brief 重启指定通道的看门狗计时器
 *
 * 该函数重启看门狗设备的指定通道的计时器，以防止该通道的系统复位
 *
 * @param wdt 指向看门狗设备结构体的指针
 * @param chan 要重启计时器的通道编号
 *
 * @return 成功时返回true，否则返回false
 */
VBARLIB_API bool vbar_drv_watchdog_restart(struct vbar_drv_watchdog *wdt, unsigned int chan);

/**
 * @brief 关闭看门狗设备
 *
 * 该函数释放与看门狗设备相关的资源并关闭设备
 *
 * @param wdt 指向看门狗设备结构体的指针
 */
VBARLIB_API void vbar_drv_watchdog_close(struct vbar_drv_watchdog *wdt);

#ifdef __cplusplus
}
#endif
#endif /* __VBAR_WATCHDOG_H__ */
