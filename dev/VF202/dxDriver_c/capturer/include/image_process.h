#ifndef __VBAR_DRV_CAPTURER_IMAGE_PROCESS_SUPPORT_H__
#define __VBAR_DRV_CAPTURER_IMAGE_PROCESS_SUPPORT_H__

enum image_type {
    IMAGE_YUV420P,
    IMAGE_YUV420SP,
};

enum vbar_drv_picture_type {
    TYPE_JPEG = 0,
    TYPE_BMP,
    TYPE_PNG,
    TYPE_UNKNOE
};

enum vbar_drv_filter_mode {
    FILTER_MODE_NONE     = 0, /* 不进行滤波，直接采样；速度最快。 */
    FILTER_MODE_LINEAR   = 1, /* 只沿水平方向滤波。 */
    FILTER_MODE_BILINEAR = 2, /* 双线性滤波；比盒滤波更快，但在缩小图像时质量较低。 */
    FILTER_MODE_BOX      = 3  /* 盒滤波；提供最高的缩放质量 */
};


#include <vbar/export.h> /* for VBARLIB_API */
#include <drivers/image_process_bmp.h>
#include <drivers/image_process_png.h>
#include <drivers/image_process_jpeg.h>

#ifdef	__cplusplus
extern "C" {
#endif

/**
 * @brief yuv420p_to_rgb888
 *  YU12 YV12
 * @param image
 * @param rgb (need malloc and free by yourself, malloc size is image width x height x 3)
 *
 * @return
 */
VBARLIB_API void vbar_drv_image_process_yuv420p_to_rgb888(struct vbar_drv_image *image, unsigned char *rgb);

/**
 * @brief yuv420sp_to_rgb888
 * NV21 NV12属于YUV420SP格式
 * @param image
 * @param rgb (need malloc and free by yourself, malloc size is image width x height x 3)
 */
VBARLIB_API void vbar_drv_image_process_yuv420sp_to_rgb888(struct vbar_drv_image *image, uint8_t *rgb);

/**
 * @brief image yuv420p 转bgr888
 *
 * @param image yuv420p 图像数据
 * @param bgr 数据(need malloc and free by yourself, malloc size is image width x height x 3)
 *
 * @return
 */
VBARLIB_API void vbar_drv_image_process_yuv420p_to_bgr888(struct vbar_drv_image *image, unsigned char *bgr);

/**
 * @brief image yuv420sp 转bgr888
 *
 * @param image yuv420sp 图像数据
 * @param bgr 数据(need malloc and free by yourself, malloc size is image width x height x 3)
 *
 * @return
 */
VBARLIB_API void vbar_drv_image_process_yuv420sp_to_bgr888(struct vbar_drv_image *image, uint8_t *bgr);

/**
 * @brief yuv 420sp cut
 *
 * @param in_image 原始图像数据
 * @param dest_x 裁剪起始点x坐标
 * @param dest_y 裁剪起始点y坐标
 * @param dest_width 裁剪后图像宽
 * @param dest_height 裁剪后图像高
 *
 * @return 成功 返回struct vbar_drv_image *image(需要使用完destroy), 失败返回 NULL
 */
VBARLIB_API struct vbar_drv_image *vbar_drv_image_process_yuv420sp_cut(struct vbar_drv_image *in_image,
                                                                   uint32_t dest_x, uint32_t dest_y,
                                                                   uint32_t dest_width, uint32_t dest_height);
/**
 * @brief yuv 420p cut
 *
 * @param in_image 原始图像数据
 * @param dest_x 裁剪起始点x坐标
 * @param dest_y 裁剪起始点y坐标
 * @param dest_width 裁剪后图像宽
 * @param dest_height 裁剪后图像高
 *
 * @return 成功 返回struct vbar_drv_image *image(需要使用完destroy), 失败返回 NULL
 */
VBARLIB_API struct vbar_drv_image *vbar_drv_image_process_yuv420p_cut(struct vbar_drv_image *in_image,
                                                                  uint32_t dest_x, uint32_t dest_y,
                                                                  uint32_t dest_width, uint32_t dest_height);

/**
 * @brief rgb888 数据转 image yuv420p 格式
 *
 * @param rgb rgb888数据
 * @param width 图像宽
 * @param height 图像高
 *
 * @return 成功 返回struct vbar_drv_image *image(需要使用完destroy), 失败返回 NULL
 */
VBARLIB_API struct vbar_drv_image *vbar_drv_image_process_rgb888_to_yuv420p_image(const uint8_t *rgb, uint32_t width, uint32_t height);

/**
 * @brief rgb888 数据转 image yuv420sp 格式
 *
 * @param rgb rgb888数据
 * @param width 图像宽
 * @param height 图像高
 *
 * @return 成功 返回struct vbar_drv_image *image(需要使用完destroy), 失败返回 NULL
 */
VBARLIB_API struct vbar_drv_image *vbar_drv_image_process_rgb888_to_yuv420sp_image(const uint8_t *rgb, uint32_t width, uint32_t height);

/**
 * @brief bgr888 数据转 image yuv420p 格式
 *
 * @param bgr bgr888数据
 * @param width 图像宽
 * @param height 图像高
 *
 * @return 成功 返回struct vbar_drv_image *image(需要使用完destroy), 失败返回 NULL
 */
VBARLIB_API struct vbar_drv_image *vbar_drv_image_process_bgr888_to_yuv420p_image(const uint8_t *bgr, uint32_t width, uint32_t height);

/**
 * @brief bgr888 数据转 image yuv420sp 格式
 *
 * @param bgr bgr888数据
 * @param width 图像宽
 * @param height 图像高
 *
 * @return 成功 返回struct vbar_drv_image *image(需要使用完destroy), 失败返回 NULL
 */
VBARLIB_API struct vbar_drv_image *vbar_drv_image_process_bgr888_to_yuv420sp_image(const uint8_t *bgr, uint32_t width, uint32_t height);

/**
 * @brief 图片文件转image
 *
 * @param filename 文件路径
 * @param type 转换类型
 *
 * @return 成功 返回struct vbar_drv_image *image(需要使用完destroy), 失败返回 NULL
 */
VBARLIB_API struct vbar_drv_image *vbar_drv_image_process_picture_file_to_image(const char *filename, enum image_type type);

/**
 * @brief 图片数据转image (图片base64 decode后的数据转image可以用此接口)
 *
 * @param data 图片数据
 * @param data_len 数据长度
 * @param type 转换类型
 *
 * @return 成功 返回struct vbar_drv_image *image(需要使用完destroy), 失败返回 NULL
 */
VBARLIB_API struct vbar_drv_image *vbar_drv_image_process_picture_data_to_image(const uint8_t *data, size_t data_len, enum image_type type);

/**
 * @brief image 转图片数据
 *
 * @param image 图像数据
 * @param type 图像类型
 * @param save_type 转换后的图片类型
 * @param quality 压缩比，jpeg 0-100， png 无损压缩无需此参数， bmp位图无需此参数
 * @param pic_data 转换后的图片数据(需自己释放)
 * @param data_len 数据长度
 *
 * @return 成功 0 失败 -1
 */
VBARLIB_API int vbar_drv_image_process_image_to_picture_data(struct vbar_drv_image *image, enum image_type type, enum vbar_drv_picture_type save_type, int quality, uint8_t **pic_data, uint32_t *data_len);

/**
 * @brief image 转图片文件
 *
 * @param image 图像数据
 * @param type 图像类型
 * @param save_type 转换后的图片类型
 * @param save_path 图片保存路径
 * @param quality 压缩比，jpeg 0-100， png 无损压缩无需此参数， bmp位图无需此参数
 *
 * @return 成功 0 失败 -1
 */
VBARLIB_API int vbar_drv_image_process_image_to_picture_file(struct vbar_drv_image *image, enum image_type type, enum vbar_drv_picture_type save_type, const char *save_path, int quality);

/**
 * \brief 重设图片分辨率
 *
 * 此函数根据源图像生成指定分辨率的目标图像
 *
 * @param src_image  源图像结构指针
 * @param dst_width  目标图像宽度
 * @param dst_height 目标图像高度
 * @param mode       滤波器模式
 *                   FILTER_MODE_NONE     不进行滤波，直接采样；速度最快。
 *                   FILTER_MODE_LINEAR   只沿水平方向滤波。
 *                   FILTER_MODE_BILINEAR 双线性滤波；比盒滤波更快，但在缩小图像时质量较低。
 *                   FILTER_MODE_BOX      盒滤波；提供最高的缩放质量
 *
 * @return 返回指向生成的目标分辨率图像的指针(需要手动释放)
 */
VBARLIB_API struct vbar_drv_image *vbar_drv_image_resize_resolution(const struct vbar_drv_image *src_image,
                                                                    int dst_width, int dst_height, enum vbar_drv_filter_mode mode);

#ifdef	__cplusplus
}
#endif




#endif
