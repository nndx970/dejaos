#include "./include/display.h"

int get_enable_status()
{
    bool enable = 0;
    vbar_drv_display_get_enable_status(&enable);
    return enable;
}

void set_enable_status(bool enable)
{
    vbar_drv_display_set_enable_status(enable);
}

int get_backlight()
{
    int backlight = 0;
    vbar_drv_display_get_backlight(&backlight);
    return backlight;
}

void set_backlight(int backlight)
{
    vbar_drv_display_set_backlight(backlight);
}

int get_power_mode()
{
    enum vbar_drv_display_power_mode mode = 0;
    vbar_drv_display_get_power_mode(&mode);
    return mode;
}

void set_power_mode(int mode)
{
    vbar_drv_display_set_power_mode((enum vbar_drv_display_power_mode)mode);
}
