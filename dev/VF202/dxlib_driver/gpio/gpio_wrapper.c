#include "./include/gpio.h"
#include <unistd.h>

int init_gpio()
{
    return vbar_drv_gpio_init();
}

void deinit_gpio()
{
    vbar_drv_gpio_deinit();
}

int request_gpio(uint32_t gpio)
{
    return vbar_drv_gpio_request(gpio);
}

void free_gpio(uint32_t gpio)
{
    vbar_drv_gpio_free(gpio);
}

void set_func_gpio(uint32_t gpio, enum gpio_function func)
{
    vbar_drv_gpio_set_func(gpio, func);
}

void set_pull_state_gpio(uint32_t gpio, uint32_t state)
{
    vbar_drv_gpio_set_pull_state(gpio, state);
}

int get_pull_state_gpio(uint32_t gpio)
{
    return vbar_drv_gpio_get_pull_state(gpio);
}

void set_value_gpio(uint32_t gpio, uint8_t value)
{
    vbar_drv_gpio_set_value(gpio, value);
}

int get_value_gpio(uint32_t gpio)
{
    return vbar_drv_gpio_get_value(gpio);
}

void set_drive_strength_gpio(uint32_t gpio, uint16_t strength)
{
    vbar_drv_gpio_set_drive_strength(gpio, strength);
}

uint16_t get_drive_strength_gpio(uint32_t gpio)
{
    return vbar_drv_gpio_get_drive_strength(gpio);
}


int main()
{
    init_gpio();
    request_gpio(67);
    for (int i = 0; i < 10; i++) {
        set_value_gpio(67, 1);
        usleep(100000);
        set_value_gpio(67, 0);
        usleep(100000);
    }
    return 0;
}