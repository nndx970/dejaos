#ifndef __VBAR_COMMON_RSA_H__
#define __VBAR_COMMON_RSA_H__

#ifdef	__cplusplus
extern "C" {
#endif

VBARLIB_API int vbar_rsa_decrypt(uint8_t *encrypt_data, int data_len,
                                 char *public_key, int key_len,
                                 uint8_t **out);

#ifdef	__cplusplus
}
#endif

#endif
