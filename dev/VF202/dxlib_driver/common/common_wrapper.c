#include "./include/sysinfo.h"
#include "./include/hmac.h"
#include <stdio.h>

static char uuid[128];
static char hexstr[33];


char *get_uuid(uint32_t len)
{
    vbar_sysinfo_get_uuid(uuid, len);
    return uuid;
}

char *md5_hash_file(char *file)
{
    uint8_t output[16] = {0};
    int ret = vbar_md5_hash_file(file, output);
    if (ret != 0) {
        // Handle error case - return empty string or NULL
        hexstr[0] = '\0';
        return hexstr;
    }
    for (int i = 0; i < 16; ++i) {
        sprintf(hexstr + i * 2, "%02x", output[i]);
    }
    hexstr[32] = '\0';
    return hexstr;
}
