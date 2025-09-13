/*************************************************************************
*
* File Name: include/base/random.h
* Author: hpl-vguang
* mail: hepanlong@vguang.cn
* Created Time: 2023年09月05日 星期二 09时21分55秒
*
*************************************************************************/

#ifndef __VBAR_BASE_RANDOM_H__
#define __VBAR_BASE_RANDOM_H__

#include <vbar/export.h>

#ifdef	__cplusplus
extern "C" {
#endif
VBARLIB_API int vbar_drv_get_random(char *buf, int blen);

#ifdef	__cplusplus
}
#endif
#endif
