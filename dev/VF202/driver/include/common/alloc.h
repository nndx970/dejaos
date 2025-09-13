#ifndef __VBAR_COMMON_ALLOC_H__
#define __VBAR_COMMON_ALLOC_H__

/*
 * 内存分配相关函数
 *      XXX : 还缺少内存检测相关
 */

#include <vbar/export.h>
#include <stdlib.h>

#ifdef	__cplusplus
extern "C" {
#endif

/* 使用同malloc */
VBAR_EXPORT_API void *vbar_malloc(size_t size);

/* 使用同calloc */
VBAR_EXPORT_API void *vbar_calloc(size_t n, size_t size);

/* 使用同calloc */
VBAR_EXPORT_API void *vbar_realloc(void* m, size_t size);

/* 使用同free */
VBAR_EXPORT_API void vbar_free(void *mem);
#ifdef __cplusplus
}
#endif
#if 0
/* 屏蔽默认分配函数, 避免混用 */
#define malloc(_s)          vbar_assert(0)
#define calloc(_n, _s)      vbar_assert(0)
#define realloc(_m, _n)     vbar_assert(0)
#define free(_m)            vbar_assert(0)
#endif // 0
#ifdef CONFIG_USE_SYS_MALLOC
#define vbar_malloc   malloc
#define vbar_free     free
#define vbar_calloc   calloc
#define vbar_realloc   realloc
#endif

#endif /* __VBAR_ALLOC_H__ */
