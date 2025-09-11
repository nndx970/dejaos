#ifndef __EVB_H__
#define __EVB_H__

#include <drivers/gpio.h>

#include <board/common_phy_define.h>

#include <linux/input.h>

/* 硬件信道的设备文件路径
 * 因为不同驱动定义设备文件路径可能不一致
 * 所以在不同板级头文件内进行重写(重定义)
 * */
#define VBAR_BOARD_CHANNEL_USBHID_PATH      "/dev/hidg0"
#define VBAR_BOARD_CHANNEL_USBKBW_PATH      "/dev/hidg1"
#define VBAR_BOARD_CHANNEL_WIEGAND0_PATH    "/dev/vg-wiegand0"
#define VBAR_BOARD_CHANNEL_UART0_PATH       "/dev/ttyS0"
#define VBAR_BOARD_CHANNEL_UART1_PATH       "/dev/ttyS1"
#define VBAR_BOARD_CHANNEL_UART2_PATH       "/dev/ttyS2"
#define VBAR_BOARD_CHANNEL_UART3_PATH       "/dev/ttyS3"

/* 信道的功能性定义, 即软件将硬件用作什么功能，如：
 *
 *   #define VBAR_BOARD_RS485_0_INDEX      VBAR_BOARD_CHANNEL_UART4_INDEX
 *   将 uart4 这个硬件定义为软件的 0 号 485 通道
 *
 *   #define VBAR_BOARD_4G_UART_INDEX      VBAR_BOARD_CHANNEL_UART4_INDEX
 *   将 uart4 这个硬件定义为软件的 4G 通信通道
 *
 * */
/* usb 定义 */
#define VBAR_BOARD_USB0_INDEX         VBAR_BOARD_CHANNEL_USBHID_INDEX
#define VBAR_BOARD_USB1_INDEX         VBAR_BOARD_CHANNEL_USBKBW_INDEX

/* 485 定义 */
#define VBAR_BOARD_RS485_0_INDEX      VBAR_BOARD_CHANNEL_UART2_INDEX

/* wiegand 定义 */
#define VBAR_BOARD_WIEGAND_0_INDEX    VBAR_BOARD_CHANNEL_WIEGAND0_INDEX

/* 定义蓝牙的通信通道 */
#define VBAR_BOARD_BLUETOOTH_UART_INDEX VBAR_BOARD_CHANNEL_UART3_INDEX
/* 定义 4G 的通信串口 */
#define VBAR_BOARD_4G_UART_INDEX        VBAR_BOARD_CHANNEL_UART1_INDEX

/* 信道类型定义 */
#if defined (VBAR_BOARD_RS485_0_INDEX)   || defined (VBAR_BOARD_RS485_1_INDEX)   || \
    defined (VBAR_BOARD_RS485_2_INDEX)   || defined (VBAR_BOARD_RS485_3_INDEX)
#define VBAR_BOARD_RS485 (1)
#endif
#if defined (VBAR_BOARD_RS232_0_INDEX)   || defined (VBAR_BOARD_RS232_1_INDEX)   || \
    defined (VBAR_BOARD_RS232_2_INDEX)   || defined (VBAR_BOARD_RS232_3_INDEX)
#define VBAR_BOARD_RS232 (1)
#endif
#if defined (VBAR_BOARD_TTL_0_INDEX)     || defined (VBAR_BOARD_TTL_1_INDEX)     || \
    defined (VBAR_BOARD_TTL_2_INDEX)     || defined (VBAR_BOARD_TTL_3_INDEX)
#define VBAR_BOARD_TTL (1)
#endif
#if defined (VBAR_BOARD_WIEGAND_0_INDEX) || defined (VBAR_BOARD_WIEGAND_1_INDEX) || \
    defined (VBAR_BOARD_WIEGAND_2_INDEX) || defined (VBAR_BOARD_WIEGAND_3_INDEX)
#define VBAR_BOARD_WIEGAND (1)
#endif

/* 信道参数定义(仅指该信道在open时除路径以外所需的额外参数) */
#define VBAR_BOARD_CHANNEL_WIEGAND0_D0      GPIO_PG(07)
#define VBAR_BOARD_CHANNEL_WIEGAND0_D1      GPIO_PG(08)

/* gpio 硬件资源定义，如：
 *
 * #define VBAR_BOARD_GPIO_WATCHDOG_HARDWARE   GPIO_PA(13)
 * 即代表定义了一个硬件看门狗，不区分看门狗的硬件类型，即不区分 gpio 类型和 pwm 类型
 * 宏值都为该硬件看门狗的 gpio 号，为 pa13
 *
 * #define VBAR_BOARD_GPIO_LED_RED_0           GPIO_PD(21)
 * 即代表定义了一个 0 号 gpio 红颜色灯， 宏值为该红颜色灯的 gpio 号，为 pd21
 *
 * */
#define VBAR_BOARD_GPIO_RELAY_0             GPIO_PD(0)

/* pwm 硬件资源定义，如：
 *
 * #define VBAR_BOARD_PWM_WATCHDOG_HARDWARE  1
 * 即代表定义了一个硬件看门狗，宏值为该硬件看门狗的 pwm 通道，为 1
 *
 * #define VBAR_BOARD_PWM_LED_RED_0          2
 * 即代表定义了一个 0 号 pwm 红颜色灯， 宏值为该红颜色灯的 pwm 通道，为 2
 *
 * */
#define VBAR_BOARD_PWM_LED_WHITE                    4
#define VBAR_BOARD_PWM_NIR_LED_0                    7

/* pwm 硬件资源定义，定义了该 pwm 硬件资源对应的 goio 号：
 *
 * #define VBAR_BOARD_PWM_WATCHDOG_HARDWARE         1
 * 即代表定义了一个硬件看门狗，宏值为该硬件看门狗的 pwm 通道，为 1
 *
 * #define VBAR_BOARD_PWM_LED_RED_0          2
 * #define VBAR_BOARD_PWM_LED_RED_0_GPIO     GPIO_PD(08)
 * 即代表定义了一个 0 号 pwm 红颜色灯， 宏值为该红颜色灯的 pwm 通道，为 2
 * 同时定义了这个 pwm 通道控制的 gpio 的 gpio 号为 pd08
 *
 * */
#define VBAR_BOARD_PWM_LED_WHITE_GPIO           GPIO_PB(02)
#define VBAR_BOARD_PWM_NIR_LED_0_GPIO           GPIO_PE(13)

/* nfc reset gpio*/
#define VBAR_BOARD_GPIO_NFC_RESET               GPIO_PE(14)
/* nfc reset 有效电平 */
#define VBAR_BOARD_GPIO_NFC_RESET_ACTIVE_LEVEL      0
/* nfc standby gpio*/
#define VBAR_BOARD_GPIO_NFC_STANDBY             GPIO_PE(10)
/* nfc standby 有效电平 */
#define VBAR_BOARD_GPIO_NFC_STANDBY_ACTIVE_LEVEL    1
/* bluetooth reset gpio*/
#define VBAR_BOARD_GPIO_BLE_RESET               GPIO_PD(03)
/* bluetooth reset 有效电平 */
#define VBAR_BOARD_GPIO_BLE_RESET_ACTIVE_LEVEL      1

/* 硬件资源的功能性定义, 即软件将该硬件资源用作什么功能，如：
 *
 *   #define VBAR_BOARD_WATCHDOG_HARDWARE  VBAR_BOARD_GPIO_WATCHDOG_HARDWARE
 *   将 wdt 这个硬件定义为软件层面的硬件看门狗
 *
 *   #define VBAR_BOARD_GREEN_LED          VBAR_BOARD_PWM_LED_RED_0_GPIO
 *   将 pwm 红颜色灯 这个硬件定义为软件的 绿灯
 *   (注意如果硬件为 pwm， 宏值就应该为 pwm 控制的 gpio 的 gpio 号)
 *
 * */
#define VBAR_BOARD_RELAY_0              VBAR_BOARD_GPIO_RELAY_0
#define VBAR_BOARD_FLUSH_LED            VBAR_BOARD_PWM_LED_WHITE_GPIO
#define VBAR_BOARD_NIR_LED              VBAR_BOARD_PWM_NIR_LED_0_GPIO


/* 看门狗参数定义 */
#define VBAR_BOARD_WATCHDOG_TYPE                VBAR_DRV_WATCHDOG_TYPE_SOFTWARE | VBAR_DRV_WATCHDOG_TYPE_HARDWARE
#define VBAR_BOARD_WATCHDOG_TIMEOUT_MS          20000
#define VBAR_BOARD_WATCHDOG_CHANNEL_MAIN        1
#define VBAR_BOARD_WATCHDOG_CHANNEL_CARTURER    2

/* 定义配置文件最大数量 */
#define VBAR_BOARD_MAX_CONFIG_FILE              15

/* 定义 hardware 可注册的 gpio 数量 */
#define VBAR_BOARD_GPIO_AMOUNT                  4

/* 近红外补光灯的周期大小 */
#define VBAR_BOARD_PWM_NIR_LED_PERIOD_NS        2400000
/* 补光灯的周期大小 */
#define VBAR_BOARD_PWM_FLUSH_LED_PERIOD_NS      2400000

/* 按键输入 0 */
#define VBAR_BOARD_GPIO_KEY_IN_0                    BTN_0   /* kernel alarm,  PD02, code:0x101 */
/* 出门开关 */
#define VBAR_BOARD_GPIO_KEY_EXIT_0                  BTN_1   /* kernel open,   PD01, code:0x100 */
/* 防拆 */
#define VBAR_BOARD_GPIO_KEY_TAMPER                  BTN_2   /* kernel tamper, PC22, code:0x102 */
#endif
