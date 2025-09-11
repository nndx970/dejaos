#ifndef __VBAR_COMMON_STROPS_H__
#define __VBAR_COMMON_STROPS_H__

#include <stdint.h>
#include <stdio.h>
#include <vbar/export.h>

#ifdef	__cplusplus
extern "C" {
#endif
/**
 * @brief vbar_strops_str2hex 16进制格式字符串转16进制BYTE
 * eg: "12345678" -> 0x12345678
 *
 * @param raw     字符串buffer
 * @param rawlen  字符串长度
 * @param data    转换后的数据buffer
 * @param datalen 数据buffer长度
 *
 * @return 成功：转换后的字节数，失败：-1
 */
VBAR_EXPORT_API int vbar_strops_str2hex(const char *raw, uint32_t rawlen, uint8_t *data, uint32_t datalen);


/**
 * @brief vbar_strops_char2hexchar BYTE数组转换成16进制格式字符串
 *
 * @param raw     BYTE数据
 * @param rawlen  BYTE数据长度
 * @param data    转换后的字符串存储buffer
 * @param datalen 字符串存储buffer长度
 *
 * @return 成功：转换后的字符串长度，失败：-1
 */
VBAR_EXPORT_API int vbar_strops_char2hexchar(const uint8_t *raw, uint32_t rawlen, char *data, uint32_t datalen);

VBAR_EXPORT_API int vbar_strops_char2hexcharup(const uint8_t *raw, uint32_t rawlen, char *data, uint32_t datalen);


VBAR_EXPORT_API int vbar_strops_hexstr2hexbyte(const uint8_t *raw, uint32_t rawlen, uint8_t *res, uint32_t *reslen);

VBAR_EXPORT_API void vbar_get_str_serialno(char *out, size_t out_size);

/************************************
*       10进制字符串转16进制byte
*       暂时只支持最大长度:10
*       e.g:
*           1234567 to 12D687
************************************/

VBAR_EXPORT_API int vbar_strops_decimal_str2hex_byte(const uint8_t *raw, uint32_t rawlen, uint8_t *res, uint32_t *reslen);

VBAR_EXPORT_API void vbar_strops_reverse_data(const uint8_t *raw, uint32_t rawlen, uint8_t *data);

VBAR_EXPORT_API void vbar_strops_reverse_data_v2(uint8_t *source_data, uint32_t len);

/************************************
*       16进制字符串转16进制byte
*       暂时只支持最大长度:10
*       e.g:
*           123456 to 0x123456(3个字节)
************************************/

VBAR_EXPORT_API int vbar_strops_hex_str2hex_byte(const uint8_t *raw, uint32_t rawlen,
                          uint8_t *res, uint32_t *reslen);

/************************************
*       pidvid格式
*       10进制字符串转16进制byte
*       暂时只支持最大长度:10
*       e.g:
*           123456 to 0x123456(3个字节)
************************************/

VBAR_EXPORT_API int vbar_strops_pidvid_decstr2hex(const uint8_t *raw, uint32_t rawlen,
                                uint8_t *res, uint32_t *reslen);

VBAR_EXPORT_API int vbar_get_urandom(uint8_t *result, int len);

/**
 * @brief 16进制字符串转无符号整数
 * eg: char a[4] = "0101" to 257
 *
 * @param str   最长八个字节
 *
 * @return 成功 返回uint32_t 失败 0
 */
VBAR_EXPORT_API uint32_t vbar_strops_strhex2num(const char *str);
VBAR_EXPORT_API void vbar_strops_strup(char *str);

/**
 * @brief 十六进制字符串转字节数组
 *
 * @param hex_str 输入的16进制字符串
 * @param bytes   输出的字节数组
 *
 * @return  成功 0 ， 失败 -1
 */
VBAR_EXPORT_API int vbar_hex_string_to_bytes(const char *hex_str, unsigned char *bytes);
#ifdef	__cplusplus
}
#endif

#endif
