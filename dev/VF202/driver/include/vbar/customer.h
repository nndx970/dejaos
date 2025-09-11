#ifndef __APP_CUSTOMER_H__
#define __APP_CUSTOMER_H__

#include <vbar/config.h>
#include <vbar/export.h>

#ifdef  __cplusplus
extern "C" {
#endif

#ifdef CONFIG_VBAR_CUSTOMER_SUPPORT
int app_customer_init(void);

void app_customer_deinit(void);

#else

#ifdef CONFIG_VBAR_APP_CONTROL_BOARD
int app_vgmj_init(void);
void app_vgmj_deinit(void);

#define app_customer_init() app_vgmj_init()
#define app_customer_deinit()  app_vgmj_deinit()

#else
#define app_customer_init() (0)
#define app_customer_deinit()
#endif

#endif//CONFIG_VBAR_CUSTOMER_SUPPORT


#ifdef  __cplusplus
}
#endif
#endif//__APP_CUSTOMER_H__

