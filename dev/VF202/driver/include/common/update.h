 /*************************************************************************
 *
 * File Name: include/update.h
 * Author: hpl-vguang
 * mail: hepanlong@vguang.cn
 * Created Time: 2023年06月03日 星期六 15时37分16秒
 *
 *************************************************************************/
 #ifndef __VBAR_M_UPDATE_H__
 #define __VBAR_M_UPDATE_H__

 #include <stdint.h>
 #include <vbar/export.h>

 /* 此宏用于判断是否为加密升级包 */
 /* 如果package_mark与此宏相等，则为加密 */
 #define VBAR_M_UPDATE_PACKAGE_MARK  ("vguang")

 enum vbar_m_update_package_type {
     PACKAGE_TYPE_XZ = 0,
     PACKAGE_TYPE_ZIP,
 };

 #define VBAR_M_UPDATE_RETAIN_MEM_FLAGS     (1)
 #define VBAR_M_UPDATE_RETAIN_DISK_SPACE   (256 * 1024)

 #ifdef  __cplusplus
 extern "C" {
 #endif

 /// @brief 安装升级包
 /// @param package_path    安装包路径
 /// @param package_type    安装包的压缩类型
 /// @param is_power_on     是否重启
 /// @param reboot_delay_s  重启延时时间
 /// @return 成功：0；失败：-1；
 VBARLIB_API int vbar_m_update_install(const char *package_path, enum vbar_m_update_package_type package_type, \
                                         uint8_t is_power_on, int reboot_delay_s);
 #ifdef  __cplusplus
 }
 #endif
 #endif

