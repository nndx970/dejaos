#ifndef __VBAR_NET_H__
#define __VBAR_NET_H__

#include <stdint.h>
#include <pthread.h>

#include <vbar/export.h>

#define VBAR_NET_CALLBACK_MAX   10

enum vbar_m_net_type {
    VBAR_M_NET_TYPE_NONE  =  0x00,     /* 网络类型 未知   */
    VBAR_M_NET_TYPE_ETH   =  0x01,     /* 网络类型 以太网 */
    VBAR_M_NET_TYPE_WIFI  =  0x02,     /* 网络类型 WIFI   */
    VBAR_M_NET_TYPE_MODEM =  0x04,     /* 网络类型 调解器 */
};

enum vbar_m_net_status {
    VBAR_M_NET_STATUS_NONE             = 0,   /* 未初始态 */
    VBAR_M_NET_STATUS_DOWN             = 1,   /* 网卡处于关闭状态 */
    VBAR_M_NET_STATUS_UP               = 2,   /* 网卡处于打开状态 */
    VBAR_M_NET_STATUS_CONNECTING_ROUTE = 3,   /* 网线已插入或者wifi已连接ssid 但未分配ip */
    VBAR_M_NET_STATUS_CONNECTED_ROUTE  = 4,   /* 已成功分配ip */
    VBAR_M_NET_STATUS_CONNECTED_SERVER = 5    /* 已连接指定服务或者通过测试可以连接到广域网 */
};

enum vbar_m_net_netlink_card_status {
    VBAR_M_NET_NETLINK_CARD_STATUS_DOWN    = 1,    /* 网络状态：下线 */
    VBAR_M_NET_NETLINK_CARD_STATUS_UP      = 2,    /* 网络状态：上线 */
    VBAR_M_NET_NETLINK_CARD_STATUS_RUNNING = 3,    /* 网络状态：运行中 */
    VBAR_M_NET_NETLINK_CARD_STATUS_ROUTE   = 4     /* 网络状态：路由中 */
};

enum vbar_m_net_mode {
    VBAR_M_NET_MODE_STATIC     = 1,   /* 使用静态 ip 地址配置网络 */
    VBAR_M_NET_MODE_DHCP       = 2,   /* 使用 DHCP 协议自动获取网络配置 */
    VBAR_M_NET_MODE_WIFI_AP    = 3    /* 使用 Wifi 热点模式作为网络连接方式 */
};

#ifdef	__cplusplus
extern "C" {
#endif


/* 下列是对网络状态回调的声明，声明了回调函数类型，封装了回调函数结构体，最后回调函数
    表结构体继承回调结构体，实现对回调表的定义 */

/// @brief 网络状态回调函数类型定义
typedef void (*vbar_m_net_status_cb)(enum vbar_m_net_type type, enum vbar_m_net_status status, void *pdata);

struct net_data {   /* 回调结构体 */
    vbar_m_net_status_cb func_cb; /* 回调函数 */
    void *pdata;                /* 私有数据 */
};

struct net_callback_table { /* 回调表定义 */
    unsigned int index;             /* 下标 */
    unsigned int amount;            /* 总数 */
    struct net_data *ndatas;        /* 回调结构体 */
    pthread_mutex_t index_mutex;    /* 回调锁 */
};

struct vbar_m_net_param_comm {
    enum vbar_m_net_mode mode;
    bool ping;
    char ping_ip[20];
    char ip[20];
    char gateway[20];
    char netmask[20];
    char dns0[20];
    char dns1[20];
    char macaddr[20];
};

/* 继承上面的 struct net_param_comm 得到 4g、2g、以太网和 WIFI 的参数结构体 */
struct vbar_m_net_param_eth {
    struct vbar_m_net_param_comm common;
};

struct vbar_m_net_param_wifi {
    struct vbar_m_net_param_comm common;
    int rssi;
    char ssid[64];
    char psk[64];
};

struct vbar_m_net_param_modem {
    struct vbar_m_net_param_comm common;
    int rssi;
    char apn[24];
    char user[64];
    char password[64];
    char iccid[32];
    char imei[32];
    char version[32];
};

/* 静态网络参数结构体 */
struct vbar_m_net_static_param {
    char ip[20];
    char gateway[20];
    char netmask[20];
    char dns0[20];
    char dns1[20];
};

/*wifi扫码周围网络的结果*/
struct vbar_m_net_wifi_scan_result {
    uint32_t freq;      /* wifi信道值 */
    int32_t level;   /* 信号强度，值越大信号越强 */
    uint8_t ssid[256];  /* 网络名 */
    uint8_t bssid[24];  /* wifi热点MAC地址 */
    uint8_t flags[256]; /* 加密相关信息 */
};

/*wifi已保存网络的结果*/
struct vbar_m_net_wifi_save_network {
    uint16_t id;        /* 网络编号 */
    uint8_t ssid[256];  /* ssid无线网络名 */
    uint8_t bssid[24];  /* wifi热点MAC地址 */
    uint8_t flags[256]; /* 加密相关信息 */
};

/*wifi信息*/
struct vbar_m_net_wifi_info {
    uint16_t id;                 /* 网络编号 */
    uint32_t freq;               /* wifi信道值 */
    int32_t rssi;                /* 无线接收信号强度，值为负数 */
    uint16_t noise;              /* 噪声*/
    uint16_t link_speed;         /* 连接速率 */
    uint16_t wifi_generation;    /* wifi代数, WIFI 4, WIFI 5, WIFI 6 */
    uint8_t wpa_state[64];       /* WPA状态 */
    uint8_t ip[16];              /* 设备IP地址 */
    uint8_t bssid[24];           /* wifi热点MAC地址 */
    uint8_t ssid[256];           /* 网络名 */
    uint8_t mode[32];            /* 网络类型 */
    uint8_t pairwise_cipher[64]; /* 单播加密相关信息 */
    uint8_t group_cipher[64];    /* 组播加密相关信息 */
    uint8_t key_mgmt[64];        /* 密钥管理类型 */
    uint8_t p2p_addr[32];
    uint8_t address[32];
    uint8_t uuid[64];
};

/*wifi扫码周围网络的列表*/
struct vbar_m_net_wifi_scan_list {
    int counts;                              /* 扫描wifi总数 */
    struct vbar_m_net_wifi_scan_result results[20]; /* 扫描wifi结果 */
};

/*wifi已保存网络的列表*/
struct vbar_m_net_wifi_save_list {
    int counts;                               /* 扫描wifi总数 */
    struct vbar_m_net_wifi_save_network results[5]; /* 扫描wifi结果 */
};

/* 网络方法集 */
/**
 * @brief 网络方法结构体， 主要是对reset脚和power on脚的控制方法
 * (若外接的模块，根据原理图需求查看是否需要控制这两个脚, 再定义方法)
 */
struct vbar_m_net_options {
    void (*eth_phy_power_on)(bool on);
    void (*eth_phy_reset)(void);
    void (*modem_phy_power_on)(bool on);
    void (*modem_phy_reset)(void);
};

/**
 * @brief vbar_net_init 网络模块初始化
 *
 * @return 0：成功，<0 失败
 */
VBARLIB_API int vbar_m_net_init(struct vbar_m_net_options *options);

/**
 * @brief vbar_net_exit 清理网络模块资源
 *
 * @return 无
 */
VBARLIB_API void vbar_m_net_exit(void);

/**
 * @brief vbar_m_net_card_enable 使能网卡，并添加到网络管理模块
 *
 * @param type   网络类型
 * @param on     开启/关闭
 *
 * @return 0：成功，<0 失败
 */
VBARLIB_API int vbar_m_net_card_enable(uint8_t type, bool on);

/**
 * @brief vbar_m_net_card_reset 重新使能网卡
 *
 * @param type   网络类型
 * @param phy_reset     开启/关闭
 *
 * @return 0：成功，<0 失败
 */
VBARLIB_API int vbar_m_net_card_reset(uint8_t type, bool phy_reset);

/**
 * @brief vbar_m_net_card_isup 判断网卡是否up
 *
 * @param type   网络类型
 *
 * @return true：up，false unup
 */
VBARLIB_API bool vbar_m_net_card_isup(uint8_t type);

/**
 * @brief vbar_m_net_status_callback_register 注册网络状态回调函数
 *
 * @param func  回调函数
 * @param pdata 私有数据
 *
 * @return 0：成功，<0 失败
 */
VBARLIB_API int vbar_m_net_status_callback_register(vbar_m_net_status_cb func, void *pdata);

/**
 * @brief vbar_m_net_set_mode_by_card 设置指定网卡的模式及对应参数网络参数
 *
 * @param type   网卡类型
 * @param mode   网络模式
 * @param param  网络参数
 *
 * @return 0：成功，<0 失败
 */
 VBARLIB_API int vbar_m_net_set_mode_by_card(uint8_t type, enum vbar_m_net_mode mode, struct vbar_m_net_static_param *param);


/**
 * @brief vbar_m_net_get_mode_by_card 获取指定网卡的模式及对应参数网络参数
 *
 * @param type     网卡类型
 * @param param    网络参数缓存，传NULL则只返回模式
 *
 * @return >0：成功,具体的网络模式，<0 失败
 */
VBARLIB_API int vbar_m_net_get_mode_by_card(uint8_t type, struct vbar_m_net_static_param *param);

/**
 * @brief vbar_m_net_set_master_card 设置主网卡，应用程序网络状态由次网卡决定
 *
 * @param type  网卡类型
 *
 * @return 0：成功，<0 失败
 */
VBARLIB_API int vbar_m_net_set_master_card(uint8_t type);

/**
 * @brief vbar_m_net_get_master_card 获取主网卡
 *
 * @return >0：成功 主网卡类型，<0 失败
 */
VBARLIB_API int vbar_m_net_get_master_card(void);


/**
 * @brief vbar_m_net_get_status 获取网络状态
 *
 * @return 网络状态
 */
VBARLIB_API enum vbar_m_net_status vbar_m_net_get_status(void);

/**
 * @brief vbar_m_net_set_status 设置网络状态
 *
 * @param status  网络状态
 *
 * @return 无
 */
VBARLIB_API void vbar_m_net_set_status(enum vbar_m_net_status status);


/**
 * @brief vbar_net_get_wifi_ssid_list 获取wifi热点列表
 *
 * @param list  WIFI热点列表
 * @param timeout_ms  超时时间
 *
 * @return >=0 成功 热点总数，<0 失败
 */
VBARLIB_API int vbar_m_net_get_wifi_ssid_list(struct vbar_m_net_wifi_scan_list *list, uint32_t timeout_ms, uint32_t interval_ms);

/**
 * @brief vbar_m_net_get_wifi_saved_list 获取已保存的热点列表
 *
 * @param list  已保存的列表
 *
 * @return >=0 成功 热点总数，<0 失败
 */
VBARLIB_API int vbar_m_net_get_wifi_saved_list(struct vbar_m_net_wifi_save_list *list);

/**
 * @brief vbar_m_net_get_current_wifi_info 获取当前热点的信息
 *
 * @param info  热点信息
 * @param timeout_ms  超时时间
 *
 * @return 0 成功，<0 失败
 */
VBARLIB_API int vbar_m_net_get_current_wifi_info(struct vbar_m_net_wifi_info *info, uint32_t timeout_ms);

/**
 * @brief vbar_net_connect_saved_wifi 连接到已保存的wifi热点
 *
 * @param id  网络编号
 *
 * @return 0 成功，<0 失败
 */
VBARLIB_API int vbar_m_net_connect_saved_wifi(uint16_t id);

/**
 * @brief vbar_net_disconnet_wifi 断开当前连接的wifi热点
 *
 * @return 无
 */
VBARLIB_API void vbar_m_net_disconnet_wifi(void);

/// @brief 根据类型设置 mac 地址
/// @param type     网络接口类型
/// @param macaddr  要进行设置的网络地址
/// @return
VBARLIB_API int vbar_m_net_set_macaddr(uint8_t type, const char *macaddr);

/// @brief 根据类型获取 mac 地址
/// @param type     网络接口类型
/// @param macaddr
/// @return
VBARLIB_API int vbar_m_net_get_macaddr(uint8_t type, char *macaddr);

/// @brief 根据类型获取 csq 值
/// @param type     网络接口类型
/// @return
VBARLIB_API int vbar_m_net_get_rssi(uint8_t type);

/**
 * @brief vbar_m_net_set_param_by_card 设置网络参数，在card enable之前执行
 * @param type 网络接口类型
 * @param 对应网络类型的参数结构体指针
 * @param param_len 结构体大小
 * @return 0：成功，1：失败
 */
VBARLIB_API int vbar_m_net_set_param_by_card(uint8_t type, void *param, uint32_t param_len);

/**
 * @brief vbar_net_get_param_by_card 获取网络参数，在网络链接成功后调用
 * @param type 网络接口类型
 * @param 对应网络类型的参数结构体指针
 * @param param_len 结构体大小
 * @return 0：成功，1：失败
 */
VBARLIB_API int vbar_m_net_get_param_by_card(uint8_t type, void *param, uint32_t param_len);
/* modules对外头文件应做好配置层面的屏蔽
 * 这里把格式写好，在维护到这里时进行更改 */
/**
 * @brief vbar_net_connect_wifi_ssid 连接到wifi热点
 *
 * @param ssid
 * @param psk
 * @param params
 *
 * @return >=0 成功 网络编号，<0 失败
 */
VBARLIB_API int vbar_m_net_connect_wifi_ssid(const char *ssid, const char *psk, const char *params);

#ifdef	__cplusplus
}
#endif

#endif
