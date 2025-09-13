/*************************************************************************
*
* File Name: ../include/common/time.h
* Author: hpl-vguang
* mail: hepanlong@vguang.cn
* Created Time: 2024年04月02日 星期二 11时44分01秒
*
*************************************************************************/

#ifndef __VBAR_COMMON_TIME_H__
#define __VBAR_COMMON_TIME_H__

#include <vbar/export.h>

#ifdef	__cplusplus
extern "C" {
#endif

VBAR_EXPORT_API void vbar_set_abs_timeout(long timeout_ms, struct timespec *dest);

#ifdef __cplusplus
}
#endif

#endif
