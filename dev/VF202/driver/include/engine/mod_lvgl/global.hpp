#ifndef JSAPI_H
#define JSAPI_H

#include <common/common.hpp>
#ifdef	__cplusplus
extern "C" {
#endif
BOOL GlobalObjectInit(JSContext *ctx);

JSValue GetGlobalObject(JSContext* ctx);
#ifdef	__cplusplus
}
#endif
#endif
