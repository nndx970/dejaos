#ifndef _ELEC_INFO_H_
#define _ELEC_INFO_H_

//对外接口定义

#ifdef __cplusplus
extern "C" {
#endif

//身份信息结构体
typedef struct {
	int type; 				//01 代表二代证  07 代表港澳台居住证 13 外国人永久居留身份证
    char name[32];          //姓名
	char sex[24];            //性别
	char nation[30];        //民族 (二代证 港澳台居住证有效）
	char birthday[17];      //出生
	char address[142];      //住址 (二代证 港澳台居住证有效）
	char idCardNo[37];      //身份证号
	char grantDept[60];     //签发机关
	char userLifeBegin[17]; //有效开始时间
	char userLifeEnd[17];   //有效结束时间
    unsigned char picCharData[5120]; //照片数据
	int pic_len;
	char biz_id[64];		//出错返回业务流水号
	char reqid[64];         //reqid

	int signingTimes;     	//签发次数（港澳台居住证有效）
	char otherIdNum[20];	//通行证号码（港澳台居住证有效）

	char countryCode[8];	//国籍或所在地区编码（外国人永久居留身份证有效）
	char enName[128];		//英文名称（外国人永久居留身份正有效）
}idinfo;

/**
 * @brief            读卡的回调用函数 ,应用方法去实现NFC的调用
 * @param deviceSn   设备的序列号，通过用户实现的getDeviceSN获取
 * @param SNLen      设备序列号的长度
 * @param cmd        操作指令
 * @param cmdLen     下发指令的长度
 * @param retData    返回的数据
 * @param retDataLen retData的长度
 * @param pData      读卡回调函数的私有数据
 * @return           成功返回数据的长度，失败返回< 0
 */
typedef int(*card_callback)(unsigned char* cmd,int cmd_len,\
        unsigned char *retData,int retData_size,void *pData);

/**
 * @brief               soket操作的回调函数,应用方法去实现socket的调用
 * @param type          操作类型 （1 建立socket连接，2 发送数据，3.接受数据，4关闭socket连接）
 * @param sendData      需发送的数据 （连接和关闭操作时，无需读取）
 * @param sendDataLen   需发送数据的长度 （连接和关闭操作时，无需读取）
 * @param recvData      接受服务器返回的数据 （连接和关闭操作时，无需操作）
 * @param recvDataLen   recvData的大小
 * @param pData         socket回调函数的私有数据
 * @return              成功返回数据的长度，失败返回 < 0
 */
typedef int(*socket_callback)(int type ,unsigned char *sendData,int sendData_len,\
        unsigned char *recvData,int recvData_len,void *pData);

/**
 * @brief           	初始化参数
 * @param appid     	平台分配给应用的appid
 * @param len       	单次读卡长度，默认0x80
 * @param declevel  	是否读取照片，1为不读取，2为读取
 * @param loglevel  	日志级别，支持0，1，2
 * @param model     	是否直接查出信息 0是  1否 （即0是原路返回，返回身份信息，1是转发，返回reqid），
 * @param type      	卡片类型：0 身份证 1电子证照
 * @param pic_type 		照片解码数据类型 0 wlt 1 jpg
 * @param envIdCode 	环境识别码
 * @param sn 			设备序列号
 * @param device_model 	设备型号
 * @param info_type 	信息返回类型，0 身份信息结构体 ，1原始数据 char *
 * @return          成功返回0，失败返回错误码
 */
int JL_init(char *appid, int len, int declevel, int loglevel,\
                int model,int type,int pic_type,int envIdCode,char *sn,char *device_model,int info_type);


/**
 * @brief           注册读卡回调函数
 * @param func      读卡的回调用函数
 * @param pData     读卡回调函数的私有数据
 * @return          成功返回0，失败返回错误码
 */
int JL_register_card_callback(card_callback func, void *pData);

/**
 * @brief           注册soket回调函数
 * @param func      socket的回调用函数
 * @param pData     socket回调函数的私有数据
 * @return          成功返回0，失败返回错误码
 */
int JL_register_socket_callback(socket_callback func, void *pData);

/**
 * @brief       实证读取证件主函数
 * @param info  身份信息结构体 ，返回数据
 * @return      成功返回0，失败返回错误码
 */
int JL_id_info(void *info);

/**
 * @brief 		获取当前的库的版本信息
 * @return 		返回的版本信息
 */
const char *JL_getVersion();


#ifdef __cplusplus
}
#endif
#endif
