#ifdef	__cplusplus
extern "C" {
#endif
#ifdef CONFIG_DXJS_MODE_LVGL_SUPPORT
#include <lvgl/lvgl.h>
#endif
#include <quickjs/quickjs.h>
#include <quickjs/quickjs-libc.h>
#include <quickjs/cutils.h>
#ifdef __cplusplus
}
#endif

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <inttypes.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#if defined(__APPLE__)
#include <malloc/malloc.h>
#elif defined(__linux__)
#include <malloc.h>
#endif

#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/fb.h>
#include <linux/input.h>
// #include <uv.h>
#include <pthread.h>
