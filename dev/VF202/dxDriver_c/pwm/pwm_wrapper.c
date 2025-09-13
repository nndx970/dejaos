#include "pwm.h"

int pwm_request(uint32_t channel)
{
    return vbar_drv_pwm_request(channel);
}

int pwm_set_period_by_channel(uint32_t channel, uint32_t period_ns)
{
    return vbar_drv_pwm_set_period_by_channel(channel, period_ns);
}

int pwm_enable(uint32_t channel, bool on)
{
    return vbar_drv_pwm_enable(channel, on);
}

int pwm_set_duty_by_channel(uint32_t channel, uint32_t duty_ns)
{
    return vbar_drv_pwm_set_duty_by_channel(channel, duty_ns);
}

void pwm_free(uint32_t channel)
{
    vbar_drv_pwm_free(channel);
}