#ifndef __VBAR_VERSION_H__
#define __VBAR_VERSION_H__

#include <vbar/config.h>

#define CONNECT(x,y)        x y

//板级命名，如：m600_v10
#ifdef CONFIG_VBAR_BOARD_TYPE
#define VBAR_BOARD_TYPE CONFIG_VBAR_BOARD_TYPE
#else
#define VBAR_BOARD_TYPE "unknow"
#endif

#ifdef CONFIG_VBAR_APP_FEATURES_CUSTOM
#define VBAR_APP_FEATURES CONFIG_VBAR_APP_FRATURES_CUSTOM_NAME
#else
#define VBAR_APP_FEATURES CONFIG_VBAR_APP_FEATURES
#endif  /* #ifdef CONFIG_VBAR_CUSTOMER_SUPPORT */

/* 主版本号 */
#define VBAR_MAJOR_VER  "1"

/* 次版本号 */
#define VBAR_MINOR_VER  "0"

/* 发布版本号 */
#ifdef CONFIG_VBAR_RELEASE_VERSION
#define VBAR_RELEASE_VER CONFIG_VBAR_RELEASE_VERSION
#else
#define VBAR_RELEASE_VER "0"
#endif

/* 开发版本号 */
#ifdef CONFIG_VBAR_BUGFIX_VERSION_SUPPORT
#define VBAR_BUGFIX_VERSION CONFIG_VBAR_BUGFIX_VERSION
#define CONNECT_ALL(a,b,c,d,e,f)  a b c d e f
#define SOFT_VERSION(board_type, app_features, major_ver, minor_ver, release_var, bugfix_var) \
                    CONNECT_ALL(board_type, \
                    CONNECT("_", app_features), \
                    CONNECT("_", major_ver), \
                    CONNECT(".", minor_ver), \
                    CONNECT(".", release_var), \
                    CONNECT(".", bugfix_var))


//完整固件版本号，如：dw200_v10_v3.1.0.3、dw200_v10_v3.1.0.3.vgmj
#define VBAR_SOFT_VERSION \
    SOFT_VERSION(VBAR_BOARD_TYPE,   \
                 VBAR_APP_FEATURES, \
                 VBAR_MAJOR_VER,    \
                 VBAR_MINOR_VER,    \
                 VBAR_RELEASE_VER,  \
                 VBAR_BUGFIX_VERSION)
#else
#define CONNECT_ALL(a,b,c,d,e)  a b c d e
#define SOFT_VERSION(board_type, app_features, major_ver, minor_ver, release_var) \
                    CONNECT_ALL(board_type, \
                    CONNECT("_", app_features), \
                    CONNECT("_", major_ver), \
                    CONNECT(".", minor_ver), \
                    CONNECT(".", release_var))


//完整固件版本号，如：dw200_v10_v3.1.0.3、dw200_v10_v3.1.0.3.vgmj
#define VBAR_SOFT_VERSION \
    SOFT_VERSION(VBAR_BOARD_TYPE,   \
                 VBAR_APP_FEATURES, \
                 VBAR_MAJOR_VER,    \
                 VBAR_MINOR_VER,    \
                 VBAR_RELEASE_VER)
#endif



#endif
