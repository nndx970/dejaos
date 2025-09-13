#ifndef __VBAR_DRV_PWM_H__
#define __VBAR_DRV_PWM_H__

#include <stdint.h>

#include <vbar/export.h>


enum vbar_drv_pwm_mode {
    VBAR_DRV_PWM_MODE_CPU,
    VBAR_DRV_PWM_MODE_DMA,
    VBAR_DRV_PWM_MODE_DMA_CONTINUE
};

#ifdef	__cplusplus
extern "C" {
#endif

/**
 * @brief 请求指定的PWM通道
 *
 * 该函数请求并初始化指定的PWM通道，以便后续使用
 *
 * @param channel 要请求的PWM通道编号
 *
 * @return 成功时返回0，失败时返回负的错误码
 */
VBARLIB_API int vbar_drv_pwm_request(uint32_t channel);

/**
 * @brief 设置PWM模式
 *
 * 该函数设置PWM的模式
 *
 * @param mode PWM模式的值
 *
 * @return 成功时返回0，失败时返回负的错误码
 */
VBARLIB_API int vbar_drv_pwm_set_mode(enum vbar_drv_pwm_mode mode);

/**
 * @brief 设置PWM周期
 *
 * 该函数设置PWM信号的周期，单位为纳秒
 *
 * @param period_ns PWM信号的周期，单位为纳秒
 *
 * @return 成功时返回0，失败时返回负的错误码
 */
VBARLIB_API int vbar_drv_pwm_set_period(uint32_t period_ns);

/**
 * @brief 设置PWM占空比
 *
 * 该函数设置PWM信号的占空比，单位为纳秒
 *
 * @param duty_ns PWM信号的占空比，单位为纳秒
 *
 * @return 成功时返回0，失败时返回负的错误码
 */
VBARLIB_API int vbar_drv_pwm_set_duty(uint32_t duty_ns);

/**
 * @brief 设置PWM DMA占空比
 *
 * 该函数设置PWM信号的DMA占空比，单位为纳秒
 *
 * @param duty_ns PWM信号的DMA占空比，单位为纳秒
 *
 * @return 成功时返回0，失败时返回负的错误码
 */
VBARLIB_API int vbar_drv_pwm_set_dma_duty(uint32_t duty_ns);

/**
 * @brief 使能或禁用指定的PWM通道
 *
 * 该函数控制指定的PWM通道的使能状态
 *
 * @param channel 要控制的PWM通道编号
 * @param on 如果为true则使能通道，如果为false则禁用通道
 *
 * @return 成功时返回0，失败时返回负的错误码
 */
VBARLIB_API int vbar_drv_pwm_enable(uint32_t channel, bool on);

/**
 * @brief 释放指定的PWM通道
 *
 * 该函数释放并清理指定的PWM通道
 *
 * @param channel 要释放的PWM通道编号
 *
 * @return 成功时返回0，失败时返回负的错误码
 */
VBARLIB_API int vbar_drv_pwm_free(uint32_t channel);

/**
 * @brief 设置指定PWM通道的周期
 *
 * 该函数设置指定PWM通道的周期，单位为纳秒
 *
 * @param channel 要设置的PWM通道编号
 * @param period_ns PWM信号的周期，单位为纳秒
 *
 * @return 成功时返回0，失败时返回负的错误码
 */
VBARLIB_API int vbar_drv_pwm_set_period_by_channel(uint32_t channel, uint32_t period_ns);

/**
 * @brief 获取指定PWM通道的enable状态
 *
 *
 * @param channel   要获取的PWM通道编号
 * @param period_ns 输出参数, 返回的使能状态（true表示使能，false表示未使能）
 *
 * @return 成功时返回0，失败时返回负的错误码
 */
VBARLIB_API int vbar_drv_pwm_get_enable_status_by_channel(uint32_t channel, bool *status);

/**
 * @brief 设置指定PWM通道的占空比
 *
 * 该函数设置指定PWM通道的占空比，单位为纳秒
 *
 * @param channel 要设置的PWM通道编号
 * @param duty_ns PWM信号的占空比，单位为纳秒
 *
 * @return 成功时返回0，失败时返回负的错误码
 */
VBARLIB_API int vbar_drv_pwm_set_duty_by_channel(uint32_t channel, uint32_t duty_ns);

/**
 * @brief 请求并初始化指定的PWM通道，并设置其属性
 *
 * 该函数请求并初始化指定的PWM通道，并设置其使能状态、周期和占空比
 *
 * @param channel 要请求的PWM通道编号
 * @param en 如果为true则使能通道，如果为false则禁用通道
 * @param period_ns PWM信号的周期，单位为纳秒
 * @param duty_ns PWM信号的占空比，单位为纳秒
 *
 * @return 成功时返回0，失败时返回负的错误码
 */
VBARLIB_API int vbar_drv_pwm_request_with_attribute(uint32_t channel, bool en,
                                                    uint32_t period_ns, uint32_t duty_ns);

#ifdef	__cplusplus
}
#endif
#endif
