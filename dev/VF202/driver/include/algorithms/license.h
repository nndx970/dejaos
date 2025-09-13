#ifndef __VBAR_LICENSE_H__
#define __VBAR_LICENSE_H__

#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#ifdef	__cplusplus
extern "C" {
#endif

#define  vbar_license_check_old   mfssfsdfsdfdfse3p
extern uint32_t vbar_license_check_old(uint8_t level);

#define vbar_license_val_valid_point    as22dfas2df3asd
#define vbar_license_val_valid_data     ahtrthrtassad3asd
#define vbar_license_val_point_vector   iojkjlj24a5dg42
#define vbar_license_val_ret            jtj5fg4h5dfj5rty
#define _vbar_license_check(level) \
    true; \
    do { \
    uint8_t vbar_license_val_valid_point[25] = {0}; \
    uint8_t vbar_license_val_valid_data[25] = {0}; \
    uint8_t *vbar_license_val_point_vector[55] = {NULL}; \
    vbar_license_val_point_vector[5] = vbar_license_val_valid_point; \
    uint32_t vbar_license_val_ret = vbar_license_check_old((level)); \
    memcpy(vbar_license_val_point_vector[vbar_license_val_ret], vbar_license_val_valid_data, 20); \
}while(0)

#define vbar_license_check(level) ({bool _ok_ = _vbar_license_check((level)); _ok_;})

#ifdef	__cplusplus
}
#endif

#endif
