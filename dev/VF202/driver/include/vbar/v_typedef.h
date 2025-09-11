#ifndef __VBAR_INCLUDE_V_TYPEDEF_H__
#define __VBAR_INCLUDE_V_TYPEDEF_H__

#include <vbar/export.h>

/* 求数组长度 */
#define VBAR_ARRAY_LEN(v)       ((int)(sizeof(v) / sizeof((v)[0])))
#ifndef offsetof
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#endif

#define vb_likely(x)  __builtin_expect(!!(x), 1)
#define vb_unlikely(x)    __builtin_expect(!!(x), 0)
/**
 * container_of - cast a member of a structure out to the containing structure
 * @ptr:	the pointer to the member.
 * @type:	the type of the container struct this is embedded in.
 * @member:	the name of the member within the struct.
 *
 */
#define container_of(ptr, type, member) ({			\
	const typeof( ((type *)0)->member ) *__mptr = (ptr);	\
	(type *)( (char *)__mptr - offsetof(type,member) );})


#ifdef  __cplusplus
extern "C" {
#endif
typedef unsigned long vb_mj_serialno_t;

typedef struct _list_head {
	struct _list_head *next;
    struct _list_head *prev;
} list_head_t;

typedef int vbar_ret_t;

typedef union vuint16_t {
    unsigned char _tmp[2];
    unsigned short val;
} v_uint16_t;


typedef union vuint32_t {
    unsigned char _tmp[4];
    unsigned int val;
} v_uint32_t;

typedef union vuint64_t {
    unsigned char _tmp[8];
    unsigned long long val;
} v_uint64_t;

typedef union vint64_t {
    unsigned char _tmp[8];
    long long val;
} v_int64_t;



typedef unsigned char v_byte_cblock8_t[8];
typedef unsigned char v_byte_cblock16_t[16];
typedef unsigned char v_byte_cblock24_t[24];

typedef struct {
    const void *buf;
    int len;
} v_bufvc;
typedef  int identity_type_t;

typedef struct {
    unsigned short t;
    unsigned short l;
    unsigned char v[0];
} vg_tlv16t;/*小端*/


#define MIN_ALIGN    __attribute__((packed))
#ifdef  __cplusplus
}
#endif


#endif//__VBAR_INCLUDE_V_TYPEDEF_H__

