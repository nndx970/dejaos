#ifndef __QUICKJS_COMMON_H__
#define __QUICKJS_COMMON_H__

#include <quickjs/quickjs.h>

typedef struct JSContructorDef_s {
    JSCFunction *fn;
    int args_count;
} JSContructorDef;

typedef struct JSFullClassDef_s
{
    JSClassID id;
    JSClassID baseClass;
    JSClassDef def;
    JSContructorDef constructor;
    size_t funcs_len;
    JSCFunctionListEntry *funcs;
} JSFullClassDef;

typedef struct BASE_REF
{
    void *base;
    const char *uid;
} BASE_REF;

#endif
