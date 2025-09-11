#ifndef __VBAR_COMMON_SPI_H__
#define __VBAR_COMMON_SPI_H__

#include <vbar/debug.h>
#include <vbar/export.h>

#ifdef __cplusplus
extern "C" {
#endif

enum {
    VBAR_SPI_MODE_0,
    VBAR_SPI_MODE_1,
    VBAR_SPI_MODE_2,
    VBAR_SPI_MODE_3,
};
/**
 * @brief vbar_spi_open 打开spi设备
 *
 * @param path      spi设备绝对路径
 * @param speed_hz     spi设备的最大频率单位hz
 * @param mode      spi的工作模式
 * @param bits      spi的每字的比特数
 *
 * @return 成功：spi设备句柄，失败：-1
*/
VBAR_EXPORT_API int vbar_spi_open(const char *path, uint32_t speed_hz, uint8_t mode, uint8_t bits);


/**
 * @brief vbar_spi_close 关闭spi设备
 *
 * @param fd        有效的spi句柄
 *
 * @return 无
 */
VBAR_EXPORT_API void vbar_spi_close(int fd);


/**
 * @brief vbar_spi_exchange spi通讯函数
 *
 * @param fd       spi有效句柄
 * @param bits     spi的每字的比特数
 * @param speed_hz spi的通讯频率，单位hz
 * @param tdata    发送数据buffer指针
 * @param rdata    接收数据buffer指针
 * @param datalen  buffer长度
 *
 * @return 成功：发送数据的长度，失败：-1
 */
VBAR_EXPORT_API int vbar_spi_exchange(int fd, uint8_t bits, uint32_t speed,
                         const uint8_t *tdata, uint8_t *rdata, uint32_t datalen);
#ifdef __cplusplus
}
#endif


#endif
