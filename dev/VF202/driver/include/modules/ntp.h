#ifndef __VBAR_M_NTP_H__
#define __VBAR_M_NTP_H__

#include <stdint.h>
#include <vbar/export.h>

#ifdef  __cplusplus
extern "C" {
#endif

enum ntp_mode {
    NTP_BY_INTERVAL = 1,    /* ntp 服务模式，固定间隔时间进行同步 */
    NTP_BY_24HOUR = 2       /* ntp 服务模式，24小时定点同步 */
};

struct vbar_m_ntp_config {
    uint8_t ntp_en;         /* NTP_BY_INTERVAL：固定间隔同步；NTP_BY_24HOUR：24小时定点同步； */
    char host[256];         /* ntp 远端服务器（时间通过此服务器下发） */
    uint32_t port;          /* ntp 远端服务器端口号 */
    uint32_t timeout_s;     /* 同步超时时间 */
    uint32_t interval_s;    /* 间隔模式间隔时间 */
    uint8_t hour;           /* 24小时定点模式中设定的时间，比如此值为 13 则每天下午一点进行一次同步 */
};

/**
 * @brief 获取ntp配置
 *
 * @param config(不为空)
 *
 * @return 0 成功 -1 失败
 */
VBARLIB_API int vbar_m_ntp_get_config(struct vbar_m_ntp_config *config);

/**
 * @brief 设置ntp配置
 *
 * @param config(不为空)
 *
 * @return 0 成功 -1 失败
 */
VBARLIB_API int vbar_m_ntp_set_config(const struct vbar_m_ntp_config *config);


/**
 * @brief 发信号同步一次时间（ntp_init之后可用）
 *
 * @return 0 成功 -1 失败
 */
VBARLIB_API int vbar_m_ntp_adjtime_by_signal(void);

/**
 * @brief ntp同步时间
 *
 * @param config(不为空)
 *
 * @return 0 成功 -1 失败
 */
VBARLIB_API int vbar_m_ntp_adjtime(const struct vbar_m_ntp_config *config);

/**
 * @brief ntp服务初始化
 *
 * @param config(不为空)
 *
 * @return 0 成功 -1 失败
 */
VBARLIB_API int vbar_m_ntp_init(const struct vbar_m_ntp_config *config,
                                        bool (*is_net_connect)(void));

/**
 * @brief ntp服务销毁
 */
VBARLIB_API void vbar_m_ntp_deinit(void);

#ifdef  __cplusplus
}
#endif

#endif
