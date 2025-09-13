/**
 * @file quickjs-callback.h
 */
#ifndef __QUICKJS_CALLBACK_H__
#define __QUICKJS_CALLBACK_H__

#include <stdlib.h>
#include <stdio.h>
#include <quickjs/quickjs.h>

#define GETCBPROP(obj, opt, cb_ptr) GETCB(JS_GetPropertyStr(ctx, obj, opt), cb_ptr)
#define GETCB(opt, cb_ptr) GETCBTHIS(opt, cb_ptr, this_val)
#define GETCBTHIS(opt, cb_ptr, this_obj) \
  if(JS_IsFunction(ctx, opt)) { \
    cb_ptr = (JSCallback){opt, JS_DupValue(ctx, this_obj), ctx, #cb_ptr}; \
  }

#define FREECB(cb_ptr) \
  do { \
    JS_FreeValue(ctx, cb_ptr.this_obj); \
    JS_FreeValue(ctx, cb_ptr.func_obj); \
  } while(0);

#define FREECB_RT(cb_ptr) \
  do { \
    JS_FreeValueRT(rt, cb_ptr.this_obj); \
    JS_FreeValueRT(rt, cb_ptr.func_obj); \
  } while(0);

#define CALLBACK_INIT(ctx, func_obj, this_obj) \
  (JSCallback) { (func_obj), (this_obj), (ctx), 0 }

typedef struct js_callback {
  JSValue func_obj, this_obj;
  JSContext* ctx;
  const char* name;
} JSCallback;

static inline void
callback_zero(JSCallback* cb) {
  cb->ctx = 0;
  cb->this_obj = JS_UNDEFINED;
  cb->func_obj = JS_NULL;
  cb->name = 0;
}

static inline void
callback_clear(JSCallback* cb) {

  if(cb->ctx) {
    JS_FreeValue(cb->ctx, cb->this_obj);
    cb->this_obj = JS_UNDEFINED;

    JS_FreeValue(cb->ctx, cb->func_obj);
    cb->func_obj = JS_NULL;
  }

  cb->ctx = 0;
}

#undef ERROR

// 定义一个辅助函数来打印异常信息
void print_exception(JSContext *ctx) {
    JSValue exception = JS_GetException(ctx);
    const char *exception_str = JS_ToCString(ctx, exception);
    if (exception_str) {
        printf("Exception: %s\n", exception_str);
        JS_FreeCString(ctx, exception_str);
    } else {
        printf("Exception: <failed to get exception string>\n");

        // 检查是否为对象类型并尝试获取 "message" 属性
        if (JS_IsObject(exception)) {
            JSValue message = JS_GetPropertyStr(ctx, exception, "message");
            if (!JS_IsUndefined(message)) {
                const char *message_str = JS_ToCString(ctx, message);
                if (message_str) {
                    printf("Message: %s\n", message_str);
                    JS_FreeCString(ctx, message_str);
                } else {
                    printf("Message: <failed to get message string>\n");
                }
                JS_FreeValue(ctx, message);
            }
        }
    }

    // 尝试获取并打印堆栈信息
    if (JS_IsObject(exception)) {
        JSValue stack = JS_GetPropertyStr(ctx, exception, "stack");
        if (!JS_IsUndefined(stack)) {
            const char *stack_str = JS_ToCString(ctx, stack);
            if (stack_str) {
                printf("Stack: %s\n", stack_str);
                JS_FreeCString(ctx, stack_str);
            } else {
                printf("Stack: <failed to get stack string>\n");
            }
            JS_FreeValue(ctx, stack);
        }
    }

    if (JS_IsObject(exception)) {
        JSValue name = JS_GetPropertyStr(ctx, exception, "name");
        if (!JS_IsUndefined(name)) {
            const char *name_str = JS_ToCString(ctx, name);
            if (name_str) {
                printf("Stack: %s\n", name_str);
                JS_FreeCString(ctx, name_str);
            } else {
                printf("Stack: <failed to get stack string>\n");
            }
            JS_FreeValue(ctx, name);
        }
    }

    JS_FreeValue(ctx, exception);
}


typedef enum callback_e { MESSAGE = 0, CONNECT, CLOSE, ERROR, PONG, FD, HTTP, READ, POST, WRITEABLE, NUM_CALLBACKS } CallbackType;

typedef struct callbacks {
  union {
    struct {
      JSCallback message, connect, close, error, pong, fd, http, read, post, writeable;
    };
    JSCallback cb[NUM_CALLBACKS];
  };
} CallbackList;

static inline void
callbacks_zero(CallbackList* cbs) {
  for(int i = 0; i < NUM_CALLBACKS; i++)
    callback_zero(&cbs->cb[i]);
}

static inline void
callbacks_clear(CallbackList* cbs) {
  for(int i = 0; i < NUM_CALLBACKS; i++)
    callback_clear(&cbs->cb[i]);
}

static inline int
callback_valid(JSCallback const* cb) {
  return cb->ctx != 0 && JS_IsObject(cb->func_obj);
}

static inline JSValue callback_emit_this(const JSCallback* cb, JSValueConst this_obj, int argc, JSValue* argv) {
  JSValue ret = JS_UNDEFINED;
  if(cb->ctx){
    JS_UpdateStackTop(JS_GetRuntime(cb->ctx));
    ret = JS_Call(cb->ctx, cb->func_obj, this_obj, argc, argv);
    for (int i = 0; i < argc; i++)
    {
        JS_FreeValue(cb->ctx, argv[i]);
    }
  }
  if(JS_IsException(ret)) {
    print_exception(cb->ctx);
    printf("callback_emit_this func: %s excute error%s\n", cb->name);
  }
  return ret;
}

static inline JSValue callback_emit(JSCallback* cb, int argc, JSValue* argv) {
  return callback_emit_this(cb, cb->this_obj, argc, argv);
}

#endif /* __QUICKJS_CALLBACK_H__ */
