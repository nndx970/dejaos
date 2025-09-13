#ifndef __VBAR_M_GPIO_KEY_H__
#define __VBAR_M_GPIO_KEY_H__

#include <vbar/export.h>

#define VBAR_M_GPIO_KEY_MAX_KEYBOARD_CALLBACK           (10)

typedef void (*vbar_m_gpio_key_press_cb)(int code, int type, int value, void *pdata);

#ifdef	__cplusplus
extern "C" {
#endif

/**
 * @brief 初始化 GPIO 按键模块
 *
 * @return
 * - 0：初始化成功。
 * - 非0：初始化失败。
 */
VBARLIB_API int vbar_m_gpio_key_init();

/* @brief 释放 GPIO 按键模块的资源 */
VBARLIB_API void vbar_m_gpio_key_deinit(void);

/**
 * @brief 注册 GPIO 按键的按键事件回调函数
 *
 * @param name 按键的名称。用于标识不同的按键。
 * @param func 按键事件的回调函数指针。该函数将在按键被按下时被调用。
 * @param pdata 传递给回调函数的用户数据指针。该指针可以在回调函数中使用，用于传递额外的数据。
 *
 * @return
 * - 0：注册成功。
 * - 非0：注册失败。
 */
VBARLIB_API int vbar_m_gpio_key_register_cb(const char *name,
                        vbar_m_gpio_key_press_cb func, void *pdata);

/**
 * @brief 注销 GPIO 按键的按键事件回调函数
 *
 * @param name 按键的名称。用于标识要注销的按键。
 *
 * @return
 * - 0：注销成功。
 * - 非0：注销失败。
 */
VBARLIB_API int vbar_m_gpio_key_unregister_cb(const char *name);

#ifdef	__cplusplus
}
#endif

#endif
