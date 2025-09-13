#ifndef __VBAR_INCLUDE_VBAR_STATUS_H__
#define __VBAR_INCLUDE_VBAR_STATUS_H__

#include <vbar/v_typedef.h>

 /*
 *  返回值宏定义
 */

#define VBAR_RET_NORMAL_ACT                 ((vbar_ret_t)1)
#define VBAR_RET_NORMAL                     ((vbar_ret_t)0)
#define VBAR_RET_OTHER_ERR                  ((vbar_ret_t)(-1))
/*参数类*/
#define VBAR_RET_PARAM_ERR                  ((vbar_ret_t)(-128))
#define VBAR_RET_WORKMODE_FAULT             ((vbar_ret_t)(-129))     /*当前的工作模式与执行的处理方式不匹配*/

#define VBAR_RET_OTH_CHANNEL                ((vbar_ret_t)(-4000))
#define VBAR_RET_DEAL_FAULT                 ((vbar_ret_t)(-4001))
#define VBAR_RET_TIME_OUT                   ((vbar_ret_t)(-4002))    /*属超时错误*/
#define VBAR_RET_OVERFLOW                   ((vbar_ret_t)(-4003))    /*属越界错误*/
#define VBAR_RET_NO_MEM                     ((vbar_ret_t)(-4004))    /*malloc类错误*/

/*业务类*/
#define VBAR_RET_BUSINESS_OTHER_ERROR       ((vbar_ret_t)(-3000))    /**/
#define VBAR_RET_ILLEGAL_SYMBOL             ((vbar_ret_t)(-3001))    /*无效码*/
#define VBAR_RET_UNDEFINED_SYMBOL           ((vbar_ret_t)(-3002))    /**/
#define VBAR_RET_DATA_FORMAT_ERROR          ((vbar_ret_t)(-3003))    /**/

#define VBAR_RET_HTTPDOWNLOAD_ERROR         ((vbar_ret_t)(-3004))    /**/
#define VBAR_RET_CHECKMD5_ERROR             ((vbar_ret_t)(-3005))    /**/

/*系统类*/
#define VBAR_RET_MODULE_CFGTABLE_NULL                       ((vbar_ret_t)(-5000)) /*对应模块的配置表为空*/
#define VBAR_RET_CFGINFO_NOT_SUPPORT                        ((vbar_ret_t)(-5001))
#define VBAR_RET_MODULE_UNDEF                               ((vbar_ret_t)(-5002))
#define VBAR_RET_FILE_OCCUPY                                ((vbar_ret_t)(-5002)) //文件被占用
#define VBAR_RET_DEVICETIME_FAULT                           ((vbar_ret_t)(-5003))
#define VBAR_RET_SQLIT_FAULT                                ((vbar_ret_t)(-5004))
#define VBAR_RET_REQUIRE_MODULECFG_FAILED                   ((vbar_ret_t)(-5005))
#define VBAR_RET_WR_MODULECFG_FAILED                        ((vbar_ret_t)(-5006))
#define VBAR_RET_SQLIT_BUSY                                 ((vbar_ret_t)(-5007))
#define VBAR_RET_SQLITE_CONSTRAINT                          ((vbar_ret_t)(-5008))

#define VBAR_RET_DEAL_FIRMWAREFILE_FAILED                   ((vbar_ret_t)(-5009))


#define DEVICE_APP_MACADD_MAXLEN    (32)

#ifdef  __cplusplus
extern "C" {
#endif


#ifdef  __cplusplus
}
#endif

#endif//__VBAR_INCLUDE_VBAR_STATUS_H__

