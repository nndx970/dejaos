#ifndef __VBAR_PERIPH_NFC_H__
#define __VBAR_PERIPH_NFC_H__

#include <stdint.h>
#include <pthread.h>
#include <vbar/v_typedef.h>
#include <vbar/export.h>

#ifdef  __cplusplus
extern "C" {
#endif

struct vbar_p_nfc_handle;

#define VBAR_NFC_AUTH_MODE_KEYA 0x60
#define VBAR_NFC_AUTH_MODE_KEYB 0x61

#define VBAR_NFC_CARD_PROTOCL_ISO14443A           (0x01)
#define VBAR_NFC_CARD_PROTOCL_ISO14443B           (0x02)
#define VBAR_NFC_CARD_PROTOCL_ISO15693            (0x04)

/* 非命令模式 */
#define NFC_WORK_MODE_AUTO                  (1)
/* 命令模式 */
/* 表示模块一旦识别到卡，接下来一定会有用户交互 */
#define NFC_WORK_MODE_INTERACTIVE           (0)


//卡的五种状态 + iso14443-4
enum vbar_nfc_card_state {
    VBAR_NFC_CARD_STATE_POWER_OFF = -1, //卡处在未未进场状态
    VBAR_NFC_CARD_STATE_IDLE = 0,       //卡处在空闲状态, 响应:: wup/req cmd,  halt cmd
    VBAR_NFC_CARD_STATE_READY,          //卡处在准备状态::  ATQA ATQB
    VBAR_NFC_CARD_STATE_READY_X,        //卡处在准备状态::  ATTRIB 失败之后
    VBAR_NFC_CARD_STATE_ACTIVE,         //卡处在选中状态::  获取卡号之后: A卡->ATS, ATTRIB之后：B卡->none
    VBAR_NFC_CARD_STATE_HALT,           //卡处在未激活状态, 响应:: wup/req cmd,  halt cmd
    VBAR_NFC_CARD_STATE_I14443P4,       //卡处在iso14443-4, AB卡->APDU
    VBAR_NFC_CARD_STATE_AUTHENTICATED,  //卡处在读写状态::  M1卡
    VBAR_NFC_CARD_STATE_RW,             //卡处在读写状态::  M1卡
    VBAR_NFC_CARD_STATE_ACTIVE_ID,      //身份证在获GUID之后的状态
};

enum psam_workmode {
    VBAR_NFC_PSAM_READ_ENCRYPT_OR_NORNAL = 0, //成功输出加密卡号；失败输出物理卡号
    VBAR_NFC_PSAM_READ_ONLY_ENCRYPT      = 1, //成功输出加密卡号；失败无输出
    VBAR_NFC_PSAM_READ_ONLY_NORNAL       = 2, //成功输出物理卡号；失败无输出
};


/**
 * @brief vbar_nfc_gpio nfc 各引脚属性信息
 * gpio:            nfc引脚(reset/standby/Mode)
 * active_level:    nfc引脚的起始电平
 */
struct vbar_nfc_gpio {
    uintptr_t gpio;
    uint32_t active_level;
};

/**
 * @brief nfc_psam_host_ops: nfc_psam 类型的操作方法
 */
struct vbar_nfc_psam_host_ops {
    int (*recv)(struct vbar_nfc_psam_host_ops *hops, uint8_t *recv_data, uint32_t recv_size, uint32_t timeout_ms);
    int (*send)(struct vbar_nfc_psam_host_ops *hops, uint8_t *send_data, uint32_t send_dlen);
    void (*flush)(struct vbar_nfc_psam_host_ops *hops);
    int (*exchange)(struct vbar_nfc_psam_host_ops *ops, const uint8_t *tx, uint32_t tlen, uint8_t *rx, uint32_t rlen);
    void (*destroy)(struct vbar_nfc_psam_host_ops *ops);
    void *pdata; //主控私有句柄
};

typedef int (*vbar_p_nfc_calcu_authen_id)(const uint8_t *card_id, uint8_t *other_id, uint32_t other_buf_len);

/*
*   vbar_nfc_host_ops 主控芯片操作函数接口(分为chip和mcu类型)
*   当nfc为mcu类型时，需要以串口类型接口进行收发
*   当nfc为chip类型时，需要以spi类型接口进行收发
*/
struct vbar_nfc_host_ops {
    //gpio操作
    void (*gpio_ctrl)(struct vbar_nfc_host_ops *ops, uintptr_t gpio, int val);
    struct vbar_nfc_gpio *gpio_reset;
    struct vbar_nfc_gpio *gpio_mode;
    struct vbar_nfc_gpio *gpio_standby;

    //有主控芯片提供延时方式
    void (*delay_us)(uint32_t us);
    void (*delay_ms)(uint32_t ms);

    //寄存器读写回调
    int (*read)(struct vbar_nfc_host_ops *ops, uint8_t *rx, uint32_t rlen, uint16_t timeout_ms);
    int (*write)(struct vbar_nfc_host_ops *ops, const uint8_t *tx, uint32_t tlen);
    void (*flush)(struct vbar_nfc_host_ops *ops);
    int (*exchange)(struct vbar_nfc_host_ops *ops, const uint8_t *tx, uint32_t tlen, uint8_t *rx, uint32_t rlen);
    void (*destroy)(struct vbar_nfc_host_ops *ops);

    /* psam */
    struct vbar_nfc_psam_host_ops *psam_ops;

    /* 计算认证卡 id 回调 */
    pthread_mutex_t calcu_authen_id_lock;
    vbar_p_nfc_calcu_authen_id calcu_authen_id;
    void *pdata; //主控私有句柄
};

/* periph 层 */
struct vbar_nfc_ops_config {
    uint8_t afi_enable;
    uint8_t afi;                  // 应用族识别符，应用的卡预选准则
    //    uint8_t cid;                  // 卡逻辑标识符，CID 由 PCD 规定，在 PICC被激活期间是固定的
    //    uint8_t iso14443_tx_baudrate; // APDU 操作时的波特率, PCD 发送速率
    //    uint8_t iso14443_rx_baudrate; // APDU 操作时的波特率, PICC 接受速率
    uint32_t card_protocol; /*bit0: ISO14443-A 106kbps  bit1: ISO14443-B 106kbps bit3:ISO15693*/
    uint8_t identity_card_enable;  // 是否读身份证物理卡号
    uint32_t read_timeout_ms;      /* 读取数据超时 */
    uint8_t braud_tx;
    uint8_t braud_rx;
    char i14443p4_switch;
    uint8_t felica_braud;
    uint8_t check_m1_virtual_card;
    v_byte_cblock8_t m1vri_blk0_array[16];
    unsigned char m1vri_blk0_array_size;
    uint8_t nfc_sak28;          /*cpu卡和m1卡二合一. 0: 默认当作m1卡, 1:当作cpu卡*/
    uint16_t felica_syscode;
    uint8_t felica_request_code;
    char m1_sector0_key[17];
    uint8_t sector_switch;
    uint8_t key_type;
    uint8_t blk_num;
    char keycpt[32];
    char keya[32];
    char keyb[32];
};

struct vbar_nfc_config {
    /* periph 层的配置对象 */
    struct vbar_nfc_ops_config ops_config;

    /* modules 层的配置对象 */
    char enable;                   /* nfc开关 */
    char psam_en;                  /* psam开关 */
    uint32_t work_mode;/*1：  主动模式 0:   用户交互式*/
    char version[64];
    uint8_t version_is_set;
    enum psam_workmode psam_workmode; /* psam模式 */

    /* 从这里开始是 hal 层的配置对象 */
    /* 增益 */
    uint8_t card_gain;
    /* N 寄存器输出功率 */
    uint8_t N_stength_output;
    /* N 寄存器输出时间 */
    uint8_t N_stength_timer;
    /* P 寄存器输出功率 */
    uint8_t P_stength_output;
    /* P 寄存器输出时间 */
    uint8_t P_stength_timer;
};


//enum vbar_psam_type {
//    VBAR_PSAM_TYPE_CHIP,
//    VBAR_PSAM_TYPE_MCU,
//};

struct vbar_psam_ops {
    int (*psam_reset)(struct vbar_psam_ops *ops, uint8_t *atr,
                      uint16_t *atr_len, int force); //复位(包含上电激活)、获取ATR
    int (*psam_apdu)(struct vbar_psam_ops *ops, const uint8_t *apdu, uint16_t apdulen,
                     uint8_t *recvdata, uint16_t *recvlen); //复位、获取ATR
    int (*psam_change_baud)(struct vbar_psam_ops *ops);
    int (*psam_power_down)(struct vbar_psam_ops *ops); //下电去激活
    int (*update_firmware)(struct vbar_psam_ops *ops, const char *file_path, bool reboot); //升级
    void (*destroy)(struct vbar_psam_ops *ops);
    void *pdata;
};

struct vbar_nfc_ops {
    /**
     * @brief A卡REQA命令, ATQA长度固定为两字节
     *        该函数用于向ISO 14443 Type A卡发送REQA（Request A）命令
     *        这个命令用于请求卡片响应，并获取卡片的ATQA（Answer to Request A）响应
     *        ATQA响应通常包含一些卡片的基本信息，如卡片的类型和制造商代码
     *
     * @param nfc nfc句柄
     * @param atqa[] ATQA缓存数组
     *
     * @return 0 成功，其他错误码
     */
    int (*iso14443_type_a_request)(struct vbar_nfc_ops *, uint8_t atqa[]);
    /**
     * @brief TYPE_A防冲突+选卡
     *
     * @param nfc nfc句柄
     * @param uid[] 卡号缓存数组
     * @param uid_len 卡号长度
     * @param sak 卡标识
     *
     * @return 0 成功，其他错误码
     */
    int (*iso14443_type_a_active_card)(struct vbar_nfc_ops *nfc, uint8_t uid[], uint16_t *uid_len, uint8_t *sak);
    /**
     * @brief 通过卡号选卡
     *
     * @param nfc nfc句柄
     * @param uid 卡号
     * @param uid_len 卡号长度
     * @param sak 返回卡标识
     *
     * @return 0 成功，其他错误码
     */
    int (*iso14443_type_a_select_by_uid)(struct vbar_nfc_ops *nfc, uint8_t *uid, uint16_t uid_len, uint8_t *sak);
    /**
     * @brief TYPE_A获取ATS(Answer to Select)响应
     *        ATS响应是在卡片被选中后，卡片返回的一组数据
     *        包含了卡片的详细信息和通信参数
     *
     * @param nfc nfc句柄
     * @param ats_buf[] ats缓存数组
     * @param ats_size ats数组大小
     * @param ats_len ats实际长度
     *
     * @return 0 成功，其他错误码
     */
    int (*iso14443_type_a_get_ats)(struct vbar_nfc_ops *nfc, uint8_t ats_buf[], uint16_t *ats_len);
    /**
     * @brief ISO14443协议COS操作
     *        执行ISO/IEC 14443 协议中的APDU（Application Protocol Data Unit）操作
     *        APDU是一种用于智能卡和读卡器之间的通信的数据单元，通常用于发送命令和接受响应
     * @param nfc nfc句柄
     * @param sendbuf 发送数据
     * @param sendlen 发送数据长度
     * @param recvbuf[] 接收数据缓存数组
     * @param ret_len 入参时为recvbuf 数组大小，出参时为接收数据长度
     *
     * @return 0 成功，其他错误码
     */
    int (*iso14443_apdu)(struct vbar_nfc_ops *nfc, const uint8_t *sendbuf, uint16_t sendlen, uint8_t ret_buf[], uint16_t *ret_len);
    /**
     * @brief TYPE_B请求
     *        向Type B卡发送请求命令（REQB），并接收卡片的响应（ATQB）
     *        ATQB响应包含卡片的一些基本信息，如卡片类型和制造商代码
     *
     * @param nfc nfc句柄
     * @param atqb[] atqb缓存数组
     * @param atqb_leni atqb实际大小
     *
     * @return 0 成功，其他错误码
     */
    int (*iso14443_type_b_request)(struct vbar_nfc_ops *nfc, uint8_t atqb[], uint16_t *atqb_len);
    /**
     * @brief TYPE_B选卡
     *
     * @param nfc nfc句柄
     * @param uid 卡号
     * @param uid_size 卡号数组大小
     * @param uid_len 卡号长度
     * @param card_type 卡片类型
     *
     * @return 0 成功，其他错误码
     */
    int (*iso14443_type_b_attrib)(struct vbar_nfc_ops *nfc, uint8_t dsi, uint8_t dri, uint8_t cid, uint8_t outbuf[], uint16_t *outlen);

    int (*iso14443_type_b_get_idcard_uid)(struct vbar_nfc_ops *nfc, uint8_t uid[], uint16_t *uid_len);
    /**
     * @brief M1卡认证
     *        用于对M1卡（也称为MIFARE Classic 卡）进行认证。M1是Type A卡
     *        广泛应用于门禁控制、公共交通和支付系统等领域。
     *        认证过程是确保只有拥有正确密钥的设备才能访问卡片上的数据
     *
     * @param nfc nfc句柄
     * @param block_num 认证的块号（此块号会被认证）
     * @param auth_mode 认证的密钥类型（密钥A或密钥B）
     * @param key[] 密钥
     *
     * @return 0 成功，其他错误码
     */
    int (*crypto1_authen)(struct vbar_nfc_ops *nfc, uint8_t block_num, uint8_t auth_mode, uint8_t key[]);
    /**
     * @brief M1读1块
     *
     * @param nfc nfc句柄
     * @param block_num 读取的块号
     * @param read_buf[] 读取数据缓存数组(至少为16字节)
     *
     * @return 0 成功，其他错误码
     */
    int (*crypto1_read)(struct vbar_nfc_ops *nfc, uint8_t block_num, uint8_t read_buf[]);
    /**
     * @brief M1写1块
     *
     * @param nfc nfc句柄
     * @param block_num 写入的块号
     * @param write_buf 写入的数据(16字节)
     *
     * @return 0 成功，其他错误码
     */
    int (*crypto1_write)(struct vbar_nfc_ops *nfc, uint8_t block_num, uint8_t *write_buf);

    //*** 以下暂时没调试 均为NULL不支持
    //INCREMENT 对存储在数值块中的数值做加法操作，并将结果存到临时数据寄存器
    int (*crypto1_increment)(struct vbar_nfc_ops *,uint16_t address, int value);
    //DECREMENT 对存储在数值块中的数值做减法操作，并将结果存到临时数据寄存器
    int (*crypto1_decrement)(struct vbar_nfc_ops *,uint16_t address, int value);
    //TRANSFER 将临时数据寄存器的内容写入数值块
    int (*crypto1_restore)(struct vbar_nfc_ops *,uint16_t address);
    //RESTORE 将数值块内容存入临时数据寄存器
    int (*crypto1_transfer)(struct vbar_nfc_ops *,uint16_t address);

    int (*iso14443_type_a_wakeup)(struct vbar_nfc_ops *, uint8_t atqa[]);
    int (*iso14443_type_b_wakeup)(struct vbar_nfc_ops *nfc, uint8_t atqb[], uint16_t *atqb_len);
    void (*iso14443_type_a_halt)(struct vbar_nfc_ops *);
    void (*iso14443_type_b_halt)(struct vbar_nfc_ops *);
    //*** 以上暂时没调试 均为NULL不支持

    const char *(*get_name)(struct vbar_nfc_ops *);
    void (*antenna_control)(struct vbar_nfc_ops *, bool is_on);
    enum vbar_nfc_card_state (*get_card_state)(struct vbar_nfc_ops *);
    int (*update_config)(struct vbar_nfc_ops *, const struct vbar_nfc_config *config);
    void (*chip_reset)(struct vbar_nfc_ops *);
    void (*destroy)(struct vbar_nfc_ops *);
    pthread_mutex_t config_lock;

    /* periph 层的配置 */
    struct vbar_nfc_ops_config periph_config;
    /* hal 层的句柄 */
    void *nfc_hdl;
    void *pdata;
};

//输出到 output 的卡类型
enum vbar_nfc_output_card_type{
    OUTPUT_CARD_TYPE_A                 = 0x40,
    OUTPUT_CARD_TYPE_MIFARE_ULTRALIGHT = 0x41,
    OUTPUT_CARD_TYPE_MF1S503           = 0x42,  //4字节的mifare classic ev1 1k
    OUTPUT_CARD_TYPE_MF1S500           = 0x52,  //7字节的mifare classic ev1 1k
    OUTPUT_CARD_TYPE_MF1S703           = 0x43,  //4字节的mifare classic ev1 4k
    OUTPUT_CARD_TYPE_CPU_A             = 0x44,
    OUTPUT_CARD_TYPE_MIFARE_DESFIRE    = 0x45,
    OUTPUT_CARD_TYPE_IDENTITY_CARD     = 0x46, // 身份证物理卡号
    OUTPUT_CARD_TYPE_15693_CARD        = 0x47,
    OUTPUT_CARD_TYPE_B                 = 0x4a,
    OUTPUT_CARD_TYPE_CPU_B             = 0x4b,
    OUTPUT_CARD_TYPE_M1                = 0x4c,
    OUTPUT_CARD_TYPE_FELICA            = 0x4d,
    OUTPUT_CARD_TYPE_MIFARE_PLUS       = 0x4e,
    OUTPUT_CARD_TYPE_IDCARD            = 0x61, // 身份证云证卡号
    OUTPUT_CARD_TYPE_NOT_SUPPORT       = 0x7f,
};

//类型可能会调整
enum vbar_nfc_card_type {
    VBAR_NFC_CARD_TYPE_B,
    VBAR_NFC_CARD_TYPE_CPU_B,
    VBAR_NFC_CARD_TYPE_IDCARD,
    VBAR_NFC_CARD_TYPE_A,
    VBAR_NFC_CARD_TYPE_A_M1,
    VBAR_NFC_CARD_TYPE_A_MIFARE_CLASSIC_1K,
    VBAR_NFC_CARD_TYPE_A_MIFARE_CLASSIC_2K,
    VBAR_NFC_CARD_TYPE_A_MIFARE_CLASSIC_4K,
    VBAR_NFC_CARD_TYPE_CPU_A,
    VBAR_NFC_CARD_TYPE_A_MIFARE_MINI,
    VBAR_NFC_CARD_TYPE_A_MIFARE_PLUS_4K,
    VBAR_NFC_CARD_TYPE_A_MIFARE_PLUS_2K,
    VBAR_NFC_CARD_TYPE_A_MIFARE_PLUS_EV1_2K,
    VBAR_NFC_CARD_TYPE_A_MIFARE_PLUS_EV1_4K,
    VBAR_NFC_CARD_TYPE_A_MIFARE_PLUS_S,
    VBAR_NFC_CARD_TYPE_A_MIFARE_PLUS_S_2K,
    VBAR_NFC_CARD_TYPE_A_MIFARE_PLUS_S_4K,
    VBAR_NFC_CARD_TYPE_A_MIFARE_PLUS_X,
    VBAR_NFC_CARD_TYPE_A_MIFARE_PLUS_X_2K,
    VBAR_NFC_CARD_TYPE_A_MIFARE_PLUS_X_4K,
    VBAR_NFC_CARD_TYPE_A_MIFARE_PLUS_SE_1K_17PF,
    VBAR_NFC_CARD_TYPE_A_MIFARE_PLUS_SE_1K_70PF,
    VBAR_NFC_CARD_TYPE_A_MIFARE_DESFIRE_EV1_256B,
    VBAR_NFC_CARD_TYPE_A_MIFARE_DESFIRE_EV1_2K,
    VBAR_NFC_CARD_TYPE_A_MIFARE_DESFIRE_EV1_4K,
    VBAR_NFC_CARD_TYPE_A_MIFARE_DESFIRE_EV1_8K,
    VBAR_NFC_CARD_TYPE_A_MIFARE_DESFIRE_EV2_2K,
    VBAR_NFC_CARD_TYPE_A_MIFARE_DESFIRE_EV2_4K,
    VBAR_NFC_CARD_TYPE_A_MIFARE_DESFIRE_EV2_8K,
    VBAR_NFC_CARD_TYPE_A_MIFARE_DESFIRE_EV2_16K,
    VBAR_NFC_CARD_TYPE_A_MIFARE_DESFIRE_EV2_32K,
    VBAR_NFC_CARD_TYPE_A_MIFARE_DESFIRE_LIGHT,
    VBAR_NFC_CARD_TYPE_A_MIFARE_CLASSIC_1K_WITH_SMART_MX,
    VBAR_NFC_CARD_TYPE_A_MIFARE_ULTRALIGHT,
    VBAR_NFC_CARD_TYPE_A_TAGNPLAY,
    VBAR_NFC_CARD_TYPE_A_NTAG,
    VBAR_NFC_CARD_RFU,
    VBAR_NFC_CARD_NEED_GET_VERSION_L4, //暂不支持继续分析类型
    VBAR_NFC_CARD_NEED_GET_ATS_L1,
    VBAR_NFC_CARD_NEED_GET_ATS_L2,
    VBAR_NFC_CARD_NEED_GET_ATS_L3,
};

struct vbar_nfc_card_info {
    uint8_t card_type;
    uint8_t id[0x10];
    uint16_t id_len;
    uint8_t type;

    uint64_t timestamp;
    uint64_t monotonic_timestamp;/* 相对开机的时间 */
};

enum vbar_nfc_type {
    VBAR_NFC_TYPE_MCU,
    VBAR_NFC_TYPE_CHIP,
};

enum vbar_nfc_cb_mode {
    VBAR_NFC_CB_SHARED_MODE,
    VBAR_NFC_CB_EXCLUSIVE_MODE,
};

/**
 * @brief  解码后回调 ,注：不可阻塞
 *
 * @param name 回调标签，用于查询\删除已注册回调
 * @param cinfo 卡片数据
 * @param ud  用户私有数据
 *
 * @return 0：成功，其他失败
 */
typedef int (*vbar_nfc_callback)(struct vbar_p_nfc_handle *hdl, const struct vbar_nfc_card_info *cinfo, void *ud);

/**
 * @brief  解码后回调 ,注：不可阻塞
 *
 * @param host_ops 主控芯片操作函数接口
 * @param config NFC 配置
 * @param type  NFC 类型
 *
 * @return 成功返回 NFC 句柄，失败返回: NULL
 */
VBARLIB_API struct vbar_p_nfc_handle *vbar_nfc_init(struct vbar_nfc_host_ops *host_ops,
                                               struct vbar_nfc_config *config,
                                               enum vbar_nfc_type type);


/**
 * @brief  m1读扇区中 计算 认证扇区所需要card_id 的回调
 * 例如七字节卡号的M1卡需要卡号的后4位，进行扇区认证
 * @param vbar_nfc_ops nfc  操作函数接口
 * @param cb                回调函数
 *
 * @return 成功返回 0，失败返回: -1
 */

VBARLIB_API int vbar_nfc_calcu_authen_id(struct vbar_nfc_ops *nops,
                                        vbar_p_nfc_calcu_authen_id cb);

VBARLIB_API void vbar_nfc_deinit(struct vbar_p_nfc_handle *handle);

VBARLIB_API struct vbar_nfc_ops *vbar_nfc_get_nfs_ops(struct vbar_p_nfc_handle *handle);

VBARLIB_API struct vbar_nfc_card_info *vbar_nfc_card_info_create(uint8_t card_type,
                                                                         const uint8_t id[], uint16_t id_len,
                                                                         uint8_t type);

VBARLIB_API void vbar_nfc_card_info_destory(struct vbar_nfc_card_info *cinfo);

VBARLIB_API struct vbar_nfc_card_info *vbar_nfc_card_info_copy(const struct vbar_nfc_card_info *cinfo);

VBARLIB_API int vbar_nfc_cb_register(struct vbar_p_nfc_handle *nfc_hdl, const char *name,
                                vbar_nfc_callback cb, enum vbar_nfc_cb_mode cb_mode,
                                void *pdata);

VBARLIB_API int vbar_nfc_cb_unregister(struct vbar_p_nfc_handle *nfc_hdl, const char *name);

VBARLIB_API int vbar_nfc_update_config(struct vbar_p_nfc_handle *handle,
                                                const struct vbar_nfc_config *config);

VBARLIB_API int vbar_nfc_get_config(struct vbar_p_nfc_handle *handle,
                                                 struct vbar_nfc_config *config);
/**
 * @brief vbar_nfc_m1card_read_sector 读M1卡扇区
 *
 * @param hdl               nfc句柄
 * @param task_flg          任务标志：
 *                              0x00->AUTO 告知扫码器该指令可单独执行，无指令间的依赖关系。
 *                              0x01->START 告知扫码器开始对卡操作或对卡操作尚未结束，且指令间可能存在依赖关系。
 *                              0x02->FINISH 告知扫码器本条指令是操作卡的最后一条指令，将卡片操作环境恢复到默态。
 * @param sec_num           扇区号
 * @param logic_blk_num     块号（在扇区内的逻辑号0~4)
 * @param blk_nums          块数
 * @param key               密钥, 长度6bytes
 * @param key_type          密钥类型: A:0x60 B:0x61
 * @param out_buf[]         读取数据的buffer
 *
 * @return                  读取数据长度:>0, 读取失败:-1
 */
VBARLIB_API int vbar_nfc_m1card_read_sector(struct vbar_p_nfc_handle *hdl, uint8_t task_flg, uint8_t sec_num, uint8_t logic_blk_num, uint8_t blk_nums, uint8_t *key, uint8_t key_type, uint8_t out_buf[]);
/**
 * @brief vbar_nfc_m1card_write_sector 写M1卡扇区
 *
 * @param hdl               nfc句柄
 * @param task_flg          任务标志：
 *                              0x00->AUTO 告知扫码器该指令可单独执行，无指令间的依赖关系。
 *                              0x01->START 告知扫码器开始对卡操作或对卡操作尚未结束，且指令间可能存在依赖关系。
 *                              0x02->FINISH 告知扫码器本条指令是操作卡的最后一条指令，将卡片操作环境恢复到默态。
 * @param sec_num           扇区号
 * @param logic_blk_num     块号（在扇区内的逻辑号0~4)
 * @param blk_nums          块数
 * @param key               密钥, 长度6bytes
 * @param key_type          密钥类型: A:0x60 B:0x61
 * @param in_buf[]          待写入数据buffer
 *
 * @return                  写入数据长度:>0, 读取失败:-1
 */
VBARLIB_API int vbar_nfc_m1card_write_sector(struct vbar_p_nfc_handle *hdl, uint8_t task_flg, uint8_t sec_num, uint8_t logic_blk_num, uint8_t blk_nums, uint8_t *key, uint8_t key_type, uint8_t in_buf[]);
/**
 * @brief vbar_nfc_m1card_read_blk 读M1块
 *
 * @param hdl               nfc句柄
 * @param task_flg          任务标志：
 *                              0x00->AUTO 告知扫码器该指令可单独执行，无指令间的依赖关系。
 *                              0x01->START 告知扫码器开始对卡操作或对卡操作尚未结束，且指令间可能存在依赖关系。
 *                              0x02->FINISH 告知扫码器本条指令是操作卡的最后一条指令，将卡片操作环境恢复到默态。
 * @param blk_num           块号
 * @param key               密钥, 长度6bytes
 * @param key_type          密钥类型: A:0x60 B:0x61
 * @param out_buf[]         读取数据的buffer, 最小16bytes
 *
 * @return                  读取数据长度:16, 读取失败:-1
 */
VBARLIB_API int vbar_nfc_m1card_read_blk(struct vbar_p_nfc_handle *hdl, uint8_t task_flg, uint8_t blk_num, uint8_t *key, uint8_t key_type, uint8_t out_buf[]);

/**
 * @brief vbar_p_nfc_ntag_read_version 读NTAG版本号
 * @param hdl               nfc句柄
 * @param out_buf[]         读取数据的buffer
 * @param out_len           要读取的版本号长度16bytes
 * @return                  成功返回0, 失败返回其他值
 */
VBARLIB_API int vbar_p_nfc_ntag_read_version(struct vbar_p_nfc_handle *hdl, uint8_t *out_buf, uint8_t *out_len);


/**
 * @brief vbar_p_nfc_ntag_read_page 读NTAG页内容
 * @param hdl               nfc句柄
 * @param out_buf[]         读取数据的buffer, 最小16bytes
 * @param page_num          起始页地址：
 *                             每次读取四个页
 *                             如果地址(Addr)是04h，则返回页04h、05h、06h、07h内容
 * @return                  成功返回0, 失败返回其他值
 */
VBARLIB_API int vbar_p_nfc_ntag_read_page(struct vbar_p_nfc_handle *hdl, uint8_t page_num, uint8_t *out_buf);

/**
 * @brief vbar_p_nfc_ntag_fast_read_page 读NTAG多页内容
 * @param hdl               nfc句柄
 * @param start_addr        起始页地址
 * @param end_addr          结束页地址
 * @param out_buf[]         读取数据的buffer,最小为 页数*4
 * @param out_len           要读取的数据长度 页数*4
 * @return                  成功返回0, 失败返回其他值
 */
VBARLIB_API int vbar_p_nfc_ntag_fast_read_page(struct vbar_p_nfc_handle *hdl, uint8_t start_addr, uint8_t end_addr, uint8_t *out_buf, uint16_t *out_len);


/**
 * @brief vbar_p_nfc_ntag_write_page 读NTAG页内容
 * @param hdl               nfc句柄
 * @param page_num          写入的页号 ：有效Addr参数
 *                              对于NTAG213，页地址02h至2Ch
 *                              对于NTAG215，页地址02h至86h
 *                              对于NTAG216，页地址02h至E6h
 * @param page_data         写入页的内容：四字节
 * @return                  成功返回0, 失败返回其他值
 */
VBARLIB_API int vbar_p_nfc_ntag_write_page(struct vbar_p_nfc_handle *hdl, uint8_t page_num, uint8_t *page_data);

/**
 * @brief vbar_nfc_m1card_write_blk 写M1块
 *
 * @param hdl               nfc句柄
 * @param task_flg          任务标志：
 *                              0x00->AUTO 告知扫码器该指令可单独执行，无指令间的依赖关系。
 *                              0x01->START 告知扫码器开始对卡操作或对卡操作尚未结束，且指令间可能存在依赖关系。
 *                              0x02->FINISH 告知扫码器本条指令是操作卡的最后一条指令，将卡片操作环境恢复到默态。
 * @param blk_num           块号
 * @param key               密钥, 长度6bytes
 * @param key_type          密钥类型: A:0x60 B:0x61
 * @param in_buf[]          待写入数据buffer
 *
 * @return                  写入数据长度:16, 读取失败:-1
 */
VBARLIB_API int vbar_nfc_m1card_write_blk(struct vbar_p_nfc_handle *hdl, uint8_t task_flg, uint8_t blk_num, uint8_t *key, uint8_t key_type, uint8_t in_buf[]);
/**
 * @brief vbar_nfc_iso14443_apdu 给卡发送COS指令
 *
 * @param hdl               nfc句柄
 * @param task_flg          任务标志：
 *                              0x00->AUTO 告知扫码器该指令可单独执行，无指令间的依赖关系。
 *                              0x01->START 告知扫码器开始对卡操作或对卡操作尚未结束，且指令间可能存在依赖关系。
 *                              0x02->FINISH 告知扫码器本条指令是操作卡的最后一条指令，将卡片操作环境恢复到默态。
 * @param sendbuf           发送的COS指令
 * @param sendlen           发送COS指令长度
 * @param recvbuf[]         应答数据
 * @param recvlen           应答数据长度
 *
 * @return                  应答数据长度:>0, 失败:-1
 */
VBARLIB_API int vbar_nfc_iso14443_apdu(struct vbar_p_nfc_handle *hdl, uint8_t task_flg, uint8_t *sendbuf, uint16_t sendlen, uint8_t recvbuf[], uint16_t *recvlen);
/**
 * @brief vbar_p_nfc_is_card_in 判断当前是否有卡
 *
 * @param hdl               nfc句柄
 *
 * @return                  有卡：true, 无卡：false
 */
VBARLIB_API bool vbar_p_nfc_is_card_in(struct vbar_p_nfc_handle *nfc_hdl);
VBARLIB_API struct vbar_psam_ops *vbar_nfc_get_psam_ops(struct vbar_p_nfc_handle *handle);
VBARLIB_API int vbar_nfc_psam_card_reset(struct vbar_p_nfc_handle *handle, uint8_t *atr, uint16_t size, uint16_t *atr_len, int force);
VBARLIB_API int vbar_nfc_psam_card_apdu(struct vbar_p_nfc_handle *handle, const uint8_t *apdu, uint16_t apdulen, uint8_t *recvdata, uint16_t *recvlen);
VBARLIB_API int vbar_nfc_psam_change_baud(struct vbar_p_nfc_handle *handle);
VBARLIB_API int vbar_nfc_psam_power_down(struct vbar_p_nfc_handle *handle);
VBARLIB_API int vbar_nfc_psam_update_firmware(struct vbar_p_nfc_handle *handle, const char *file_path, bool reboot);
VBARLIB_API int vbar_nfc_psam_ops_func(struct vbar_p_nfc_handle *handle, uint8_t *data, uint16_t dlen, uint8_t *ack, uint16_t *acklen);
VBARLIB_API int vbar_nfc_psam_check_vgcard_callback(struct vbar_p_nfc_handle *hdl, const struct vbar_nfc_card_info *cinfo, void *ud);

#ifdef  __cplusplus
}
#endif

#endif//__VBAR_PERIPH_NFC_H__
