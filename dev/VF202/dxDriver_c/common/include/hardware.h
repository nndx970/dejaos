#ifndef __VBAR_HARDWARE__H__
#define __VBAR_HARDWARE__H__

#include <stdint.h>
#include <pthread.h>
#include <vbar/export.h>

enum hw_mode {
    HW_MODE_INIT            = 0xff,
    HW_MODE_OFF             = 0,
    HW_MODE_ON              = 1,
    HW_MODE_GROUP           = 3,    /* 根据组概念参数执行*/
    HW_MODE_GROUP_REVERSE   = 4,    /* 根据组概念参数执行 反向*/
    HW_MODE_STAGGER         = 5,    /* 按照on_ms 和 off_ms 持续交错执行*/
    HW_MODE_STAGGER_REVERSE = 6,    /* 按照on_ms 和 off_ms 持续交错执行 反向*/
};

#define HW_GPIO_LEVEL_HIGH  1   // 有效
#define HW_GPIO_LEVEL_LOW   0   // 无效
#define HW_GPIO_ACTIVE_HIGH 1   // 有效
#define HW_GPIO_ACTIVE_LOW  0   // 无效

typedef void (*gpio_ctl_func)(uint32_t gpio, bool is_active);

#define HW_PARAM_INIT_DEFAULT                                                  \
    {                                                                          \
        .gpio = 0, .on_ms = 0, .off_ms = 0, .group_count = 0,                  \
        .num_per_group = 0, .group_interval = 0, .start_level = 0,             \
        .mode = HW_MODE_INIT, .ctl_func = NULL                                 \
    }

struct hw_param {
    uint32_t gpio;
    uint32_t on_ms;     // each on time
    uint32_t off_ms;

    uint32_t group_count;
    uint32_t num_per_group;
    uint32_t group_interval;
    uint8_t start_level;    //初始状态，由外部维护
    enum hw_mode mode;      //动作方式，由外部维护
    gpio_ctl_func ctl_func;
};

#ifdef	__cplusplus
extern "C" {
#endif

/** vbar_hw_init
 *  可控制硬件列表：用于存储一些通用可用gpio参数，通过调用统一控制api
 * 控制已经调用hw_control_register_hw注册的gpio
 * 成功返回 0 失败返回负值
 *
 */
VBAR_EXPORT_API int vbar_hardware_init(uint32_t amount);

/** vbar_hw_exit
 * 销毁通过hw_control_table_create生成的table资源
 */
VBAR_EXPORT_API void vbar_hardware_exit(void);


/** vbar_hw_update
 * 更新硬件资源控制参数
 *
 * hw: struct hw_param 类型指针
 *
 * 成功返回0， 失败返回负值
 */
VBAR_EXPORT_API int vbar_hardware_update(const struct hw_param *hw);

/** vbar_hw_update
 * 更新硬件资源控制参数
 *
 * hw: struct hw_param 类型指针
 *
 * 成功返回0， 失败返回负值
 */

VBAR_EXPORT_API int vbar_hardware_register(struct hw_param *hw);

#ifdef __cplusplus
}
#endif
#endif
