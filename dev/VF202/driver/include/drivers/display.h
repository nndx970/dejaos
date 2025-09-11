/*************************************************************************
*
* File Name: include/display.h
* Author: hpl-vguang
* mail: hepanlong@vguang.cn
* Created Time: 2024年04月11日 星期四 16时14分40秒
*
*************************************************************************/
#ifndef __VBAR_DRV_DISPLAY__
#define __VBAR_DRV_DISPLAY__

#include <vbar/export.h>

enum vbar_drv_display_power_mode {
    VBAR_DRV_DISPLAY_POWER_MODE_NORMAL  = 0,
    VBAR_DRV_DISPLAY_POWER_MODE_STANDBY
};

#ifdef	__cplusplus
extern "C" {
#endif

/**
 * @brief 获取显示设备使能状态
 * @param[out] enable 输出参数，用于接收当前使能状态（0-禁用 1-启用）
 * @return 0 成功，其他值表示错误码
 */
VBARLIB_API int vbar_drv_display_get_enable_status(bool *enable);

/**
 * @brief 设置显示设备使能状态
 * @param[in] enable 输入参数，要设置的使能状态（0-禁用 1-启用）
 * @return 0 成功，其他值表示错误码
 */
VBARLIB_API int vbar_drv_display_set_enable_status(bool enable);

/**
 * @brief 获取显示背光亮度值
 * @param[out] backlight 输出参数，用于接收当前背光亮度值（范围0-100）
 * @return 0 成功，其他值表示错误码
 */
VBARLIB_API int vbar_drv_display_get_backlight(int *backlight);

/**
 * @brief 设置显示背光亮度值
 * @param[in] backlight 输入参数，要设置的背光亮度值（范围0-100）
 * @return 0 成功，其他值表示错误码
 */
VBARLIB_API int vbar_drv_display_set_backlight(int backlight);

/**
 * @brief 获取显示设备电源模式
 * @param[out] mode 输出参数，接收当前电源模式
 * @return 0 成功，其他值表示错误码
 * @note 实际支持的电源模式取决于硬件实现
 * @return 0 成功，其他值表示错误码
 */
VBARLIB_API int vbar_drv_display_get_power_mode(enum vbar_drv_display_power_mode *mode);

/**
 * @brief 设置显示设备电源模式
 * @param[in] mode 输入参数，设置电源模式
 * @return 0 成功，其他值表示错误码
 * @warning 直接切换到OFF模式可能导致显示内容丢失
 * @return 0 成功，其他值表示错误码
 */
VBARLIB_API int vbar_drv_display_set_power_mode(enum vbar_drv_display_power_mode mode);

#ifdef __cplusplus
}
#endif

#endif /* __VBAR_DRV_DISPLAY__ */
