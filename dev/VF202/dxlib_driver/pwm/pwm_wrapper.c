#include "pwm.h"

int pwm_init(void)
{
    // int ret = vbar_drv_pwm_request(4);
    // ret = vbar_drv_pwm_set_period_by_channel(4, 2400000);
    // ret = vbar_drv_pwm_enable(4, 1);
    // ret = vbar_drv_pwm_set_duty_by_channel(4, 2400000 * (1 / 255));

    int ret = vbar_drv_pwm_request(7);
    ret = vbar_drv_pwm_set_period_by_channel(7, 2400000);
    ret = vbar_drv_pwm_enable(7, 1);
    ret = vbar_drv_pwm_set_duty_by_channel(7, 2400000 * 255 / 255);

    return ret;
}