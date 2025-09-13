#ifndef __VBAR_COMMON_IOREMAP_H__
#define __VBAR_COMMON_IOREMAP_H__

#include <vbar/export.h>

#ifdef	__cplusplus
extern "C" {
#endif


VBAR_EXPORT_API void *vbar_ioremap(unsigned long addr, unsigned int offset);


VBAR_EXPORT_API void vbar_ioumap(void *addr, unsigned int offset);


#ifdef	__cplusplus
}
#endif

#endif
