#ifndef LOMBO_MALLOC_H
#define LOMBO_MALLOC_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#include <stdio.h>

/* note: this lib is used for alloc large continuous buffer for special usage
 * if you just need to alloc small buffer in your application, just use malloc */

enum mem_type_t {
	MEM_VIRT = 0, /* normal malloc, not recommended to use */
	MEM_WB, /* write back */
	MEM_WT, /* write throught,  */
	MEM_UC, /* uncached buffer */
};

/* old api used by old code */
void *lombo_malloc(int size, int flag, unsigned long *phy_addr,
			char *file, int line);
void lombo_free(void *ptr, int flag);
void *lombo_memcpy(void *dst, void *src, int size);
void *lombo_memset(void *prt, int val, int size);
void *lombo_calloc(int num, int size, char *file, int line);
/* use ion sync cmd to flush buf with phy addr */
int lombo_cache_flush(unsigned long phy_addr);
/* use ion sync cmd to flush buf with vir addr */
int lombo_cache_flush_vir(void *vir_addr_align);
/* flush part of the buffer */
int lombo_flush_part(void *virt, unsigned int size);
/* flush all the cache in the hardware cache unit */
int lombo_flush_all(void);
unsigned int lombo_vir_to_fd(void *vir_addr_align);
unsigned long lombo_vir_to_phy(void *vir_addr);
unsigned long lombo_fd_to_phy(unsigned int fd);

/* dump all buffer unfreed */
void lombo_malloc_dump();

/* recommended alloc & free api for malloc large continuous buffer */
#define lombo_malloc_cached(size) \
	lombo_malloc(size, MEM_WT, NULL, __FILE__, __LINE__)

#define lombo_malloc_unca(size) \
	lombo_malloc(size, MEM_UC, NULL, __FILE__, __LINE__)

#define lombo_free_cached(ptr) \
	lombo_free(ptr, MEM_WT)

#define lombo_free_unca(ptr) \
	lombo_free(ptr, MEM_UC)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* LOMBO_MALLOC_H */
