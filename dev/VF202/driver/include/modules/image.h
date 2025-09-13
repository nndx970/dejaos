#ifndef __VBAR_M_IMAGE_H__
#define __VBAR_M_IMAGE_H__

#include <stdint.h>

#include <vbar/export.h> /* for VBARLIB_API */
#include <vbar/auxiliary.h>

struct vbar_m_image_mplane {
    uint8_t mplane_num;
    uint8_t *addr[3];
    uint32_t length[3];
};

enum vbar_m_image_layout_type {
    VBAR_M_IMAGE_LAYOUT_CAPTURER,
    VBAR_M_IMAGE_LAYOUT_CAPTURER_PLANE
};
/* 图像结构定义 */
struct vbar_m_image {
    enum vbar_m_image_layout_type layout_type;
    uint32_t width;
    uint32_t widthbytes;
    uint32_t height;          /* 图像宽/长    */
    unsigned char *data;        /* 原始数据     */
    unsigned long datalen;      /* 原始数据长度 */
    struct vbar_m_image_mplane mplane;
    uint64_t timestamp;     /* 时间戳       */
};

#ifdef	__cplusplus
extern "C" {
#endif

/* 创建图像(复制缓冲区数据) */
VBARLIB_API struct vbar_m_image *vbar_m_image_create(const unsigned char *data, uint32_t width, uint32_t widthbytes, uint32_t height);

VBARLIB_API struct vbar_m_image *vbar_m_image_copy(const struct vbar_m_image *rawimage);
/* 把缓冲区包装为图像结构(不拷贝、不管理缓冲区内存) */
VBARLIB_API struct vbar_m_image *vbar_m_image_wrap(unsigned char *data, uint32_t width, uint32_t widthbytes,  uint32_t height);

/* 销毁图像 */
VBARLIB_API void vbar_m_image_destroy(struct vbar_m_image *image);

/* 获取图像宽度 */
VBARLIB_API uint32_t vbar_m_image_get_width(const struct vbar_m_image *image);

/* 获取图像宽度 */
VBARLIB_API uint32_t vbar_m_image_get_widthbytes(const struct vbar_m_image *image);
/* 获取图像长度 */
VBARLIB_API uint32_t vbar_m_image_get_height(const struct vbar_m_image *image);

/* 获取图像数据缓冲区指针 */
VBARLIB_API unsigned char *vbar_m_image_get_data(const struct vbar_m_image *image);

/* 获取图像数据长度 */
VBARLIB_API unsigned long vbar_m_image_get_data_length(const struct vbar_m_image *image);

/* 获取图像时间戳 */
VBARLIB_API vbar_clock_t vbar_m_image_get_timestamp(const struct vbar_m_image *image);

/*设置图像时间戳*/
VBARLIB_API void vbar_m_image_set_timestamp(struct vbar_m_image *image, vbar_clock_t timestamp);

/* 图像反显(黑白反色) */
VBARLIB_API struct vbar_m_image *vbar_m_image_invert(const struct vbar_m_image *rawimage);

/* 图像镜像翻转 */
VBARLIB_API struct vbar_m_image *vbar_m_image_mirror(const struct vbar_m_image *rawimage);

/* 真随机数生成(通过图像获得) */
VBARLIB_API unsigned int vbar_m_image_rand(const struct vbar_m_image *image);

/* 图像HASH(64bit)值计算 */
VBARLIB_API bool vbar_m_image_hash(const struct vbar_m_image *image, uint64_t *hash, int *mean);

#if defined(VBAR_CONFIG_HAVE_IMAGEPLUS)

/* 打开图像文件 */
VBARLIB_API struct vbar_m_image *vbar_m_image_open(const char *path);

/* 保存图像到指定路径 */
VBARLIB_API bool vbar_m_image_save(const struct vbar_m_image *image, const char *path);

/* 显示图像 */
VBARLIB_API bool vbar_m_image_show(const struct vbar_m_image *image, const char *title);

#endif /* VBAR_CONFIG_HAVE_IMAGEPLUS */

#ifdef __cplusplus
}
#endif

#endif /* __VBAR_M_IMAGE_H__ */
