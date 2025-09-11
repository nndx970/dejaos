/*************************************************************************
*
* File Name: common/include/vbar_math.h
* Author: hpl-vguang
* mail: hepanlong@vguang.cn
* Created Time: 2025年04月01日 星期二 11时45分48秒
*
*************************************************************************/

#ifndef __VBAR_COMMON_MATH_H__
#define __VBAR_COMMON_MATH_H__

#include <stdio.h>
#include <stdint.h>

#ifndef __GNUC__
/* __typeof__ 是GCC扩展，用于获取表达式的类型 */
#error "vbar_math only support GCC"
#endif

/* 使用 __typeof__ 完成类型的兼容 */
/* 使用 _x 临时变量，避免自增参数可能导致的问题 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 获取两个值的最大值（类型安全）
 * @param a 第一个比较值
 * @param b 第二个比较值
 * @return 两个值中的较大者
 *
 * @code
 * int a = 5, b = 8;
 * int max = VBAR_MATH_MAX(a, b); // 返回8
 * @endcode
 */
#define VBAR_MATH_MAX(a, b) ({ \
    __typeof__(a) _a = (a); \
    __typeof__(b) _b = (b); \
    _a < _b ? _b : _a; \
})

/**
 * @brief 获取两个值的最小值（类型安全）
 * @param a 第一个比较值
 * @param b 第二个比较值
 * @return 两个值中的较小者
 */
#define VBAR_MATH_MIN(a, b) ({ \
    __typeof__(a) _a = (a); \
    __typeof__(b) _b = (b); \
    _a < _b ? _a : _b; \
})

/**
 * @brief 将值限制在指定范围内 [min, max]
 * @param val 要限制的值
 * @param min 允许的最小值
 * @param max 允许的最大值
 * @return 被限制后的值（min ≤ val ≤ max）
 */
#define VBAR_MATH_RANGE(val, min, max) ({ \
    __typeof__(val) _val = (val); \
    __typeof__(min) _min = (min); \
    __typeof__(max) _max = (max); \
    (__typeof__(val))(VBAR_MATH_MAX(VBAR_MATH_MIN(_val, _max), _min)); \
})

/**
 * @brief 获取数组最大值
 * @param arr 目标数组指针
 * @param size 数组元素个数
 * @return 数组中的最大值
 *
 * @code
 * int arr[] = {3,1,4,1,5};
 * int max = VBAR_MATH_ARRAY_MAX(arr, 5); // 返回5
 * @endcode
 */
#define VBAR_MATH_ARRAY_MAX(arr, size) ({ \
    __typeof__(*(arr)) *_arr = (arr); \
    size_t _size = (size); \
    __typeof__(*_arr) _max_val = _arr[0]; \
    for (size_t _i = 1; _i < _size; ++_i) { \
        if (_arr[_i] > _max_val) { \
            _max_val = _arr[_i]; \
        } \
    } \
    _max_val; \
})

/**
 * @brief 获取数组最小值
 * @param arr 目标数组指针
 * @param size 数组元素个数
 * @return 数组中的最小值
 *
 * @code
 * int arr[] = {3,1,4,1,5};
 * int min = VBAR_MATH_ARRAY_MIN(arr, 5); // 返回1
 * @endcode
 */
#define VBAR_MATH_ARRAY_MIN(arr, size) ({ \
    __typeof__(*(arr)) *_arr = (arr); \
    size_t _size = (size); \
    __typeof__(*_arr) _min_val = _arr[0]; \
    for (size_t _i = 1; _i < _size; ++_i) { \
        if (_arr[_i] < _min_val) { \
            _min_val = _arr[_i]; \
        } \
    } \
    _min_val; \
})

/**
 * @brief 获取数组最大值的索引
 * @param arr 目标数组指针
 * @param size 数组元素个数
 * @return 最大值元素的索引
 *
 * @code
 * int arr[] = {3,1,4,1,5};
 * int idx = VBAR_MATH_ARRAY_MAX_IDX(arr, 5); // 返回4
 * @endcode
 */
#define VBAR_MATH_ARRAY_MAX_IDX(arr, size) ({ \
    __typeof__(*(arr)) *_arr = (arr); \
    size_t _size = (size); \
    size_t _max_idx = 0; \
    __typeof__(*_arr) _max_val = _arr[0]; \
    for (size_t _i = 1; _i < _size; ++_i) { \
        if (_arr[_i] > _max_val) { \
            _max_val = _arr[_i]; \
            _max_idx = _i; \
        } \
    } \
    _max_idx; \
})

/**
 * @brief 获取数组最小值的索引
 * @param arr 目标数组指针
 * @param size 数组元素个数
 * @return 最小值元素的索引
 *
 * @code
 * int arr[] = {3,1,4,1,5};
 * int idx = VBAR_MATH_ARRAY_MIN_IDX(arr, 5); // 返回1
 * @endcode
 */
#define VBAR_MATH_ARRAY_MIN_IDX(arr, size) ({ \
    __typeof__(*(arr)) *_arr = (arr); \
    size_t _size = (size); \
    size_t _min_idx = 0; \
    __typeof__(*_arr) _min_val = _arr[0]; \
    for (size_t _i = 1; _i < _size; ++_i) { \
        if (_arr[_i] < _min_val) { \
            _min_val = _arr[_i]; \
            _min_idx = _i; \
        } \
    } \
    _min_idx; \
})

/* 判断是否为 2 的倍数 */
#define VBAR_MATH_IS_MULTIPLE_OF_2(x) ((x & 1) == 0)

/* 判断是否为 2 的 n 次方 */
#define VBAR_MATH_IS_POWER_OF_2(x) ((x) != 0 && (((x) & ((x) - 1)) == 0))

/**
 * @brief 向上对齐内存地址
 * @param addr 目标地址
 * @param align 对齐值（必须是2的幂）
 * @return 对齐后的地址
 */
#define VBAR_MATH_ALIGN_UP_ADDR(addr, align) ({ \
    __typeof__(addr) _addr = (addr); \
    __typeof__(align) _align = (align); \
    ((_addr + _align - 1) & ~(_align - 1)); \
})

/**
 * @brief 向下对齐内存地址
 * @param addr 目标地址
 * @param align 对齐值（必须是2的幂）
 * @return 对齐后的地址
 */
#define VBAR_MATH_ALIGN_DOWN_ADDR(addr, align) ({ \
    __typeof__(addr) _addr = (addr); \
    __typeof__(align) _align = (align); \
    (_addr & ~(_align - 1)); \
})

/**
 * @brief 向上对齐大小
 * @param size 目标大小
 * @param align 对齐值（必须是2的幂）
 * @return 对齐后的大小
 */
#define VBAR_MATH_ALIGN_UP_SIZE(size, align) ({ \
    __typeof__(size) _size = (size); \
    __typeof__(align) _align = (align); \
    ((_size + _align - 1) & ~(_align - 1)); \
})

/**
 * @brief 向下对齐大小
 * @param size 目标大小
 * @param align 对齐值（必须是2的幂）
 * @return 对齐后的大小
 */
#define VBAR_MATH_ALIGN_DOWN_SIZE(size, align) ({ \
    __typeof__(size) _size = (size); \
    __typeof__(align) _align = (align); \
    (_size & ~(_align - 1)); \
})

#ifdef __cplusplus
}
#endif

#endif
