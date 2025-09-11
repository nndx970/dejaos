#include "capturer.h"

static struct vbar_m_capturer_config config1 =
    {
        .path = "/dev/video3",
        .width = 1280,
        .height = 720,
        .widthbytes = 2,
        .sensor_type = 7,
        .pixel_format = V4L2_PIX_FMT_NV12M,
        .rotation = 90,
        .frame_num = 3,
        .max_channels = 3,
        .preview_enable = 1,
        .preview_left = 0,
        .preview_top = 0,
        .preview_width = 600,
        .preview_height = 1024,
        .preview_rotation = 90,
        .preview_mode = 2,
        .preview_screen_index = 0,
};

struct vbar_m_capturer_handle *capturer_rgb_init(void)
{
    struct vbar_m_capturer_handle *app_rgb_capturer = vbar_m_capturer_init(&config1, VBAR_M_ISP_TYPE_NONE, NULL);
    return app_rgb_capturer;
}

static struct vbar_m_capturer_config config2 =
    {
        .path = "/dev/video0",
        .width = 800,
        .height = 600,
        .widthbytes = 2,
        .sensor_type = 7,
        .pixel_format = V4L2_PIX_FMT_NV12M,
        .rotation = 90,
        .frame_num = 3,
        .max_channels = 3,
        .preview_enable = 0,
        .preview_width = 150,
        .preview_height = 200,
        .preview_rotation = 90,
        .preview_mode = 1,
        .preview_left = 330, // 480-150
        .preview_top = 0,
        .preview_screen_index = 1,
};

struct vbar_m_capturer_handle *capturer_nir_init(void)
{
    struct vbar_m_capturer_handle *app_nir_capturer = vbar_m_capturer_init(&config2, VBAR_M_ISP_TYPE_NONE, NULL);
    return app_nir_capturer;
}
