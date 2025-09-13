#pragma once
#include <common/common.hpp>

#ifdef	__cplusplus
extern "C" {
#endif
void WindowInit();

void NativeComponentInit(JSContext *ctx, JSValue ns);

void NativeEnumInit(JSContext *ctx, JSValue ns);

void NativeMathInit(JSContext *ctx, JSValue ns);

void NativeBasicComponentInit(JSContext *ctx, JSValue ns);

void NativeEventInit(JSContext *ctx, JSValue ns);

void NativeDispInit(JSContext *ctx, JSValue ns);

void NativeDrawInit(JSContext *ctx, JSValue ns);

void NativeAnimInit(JSContext *ctx, JSValue ns);

void NativeAreaInit(JSContext *ctx, JSValue ns);

void NativeMemInit(JSContext *ctx, JSValue ns);

void NativeGroupInit(JSContext *ctx, JSValue ns);

void NativeTimerInit(JSContext *ctx, JSValue ns);

void NativeColorInit(JSContext *ctx, JSValue ns);

void NativeFontInit(JSContext *ctx, JSValue ns);

void NativeThemeInit(JSContext *ctx, JSValue ns);

void NativeStyleInit(JSContext *ctx, JSValue ns);

void NativeButtonInit(JSContext *ctx, JSValue ns);

void NativeLabelInit(JSContext *ctx, JSValue ns);

void NativeCheckboxInit(JSContext *ctx, JSValue ns);

void NativeDropdownInit(JSContext *ctx, JSValue ns);

void NativeLineInit(JSContext *ctx, JSValue ns);

void NativeRollerInit(JSContext *ctx, JSValue ns);

void NativeImageInit(JSContext *ctx, JSValue ns);

void NativeCanvasInit(JSContext *ctx, JSValue ns);

void NativeArcInit(JSContext *ctx, JSValue ns);

void NativeBarInit(JSContext *ctx, JSValue ns);

void NativeBtnmatrixInit(JSContext *ctx, JSValue ns);

void NativeTableInit(JSContext *ctx, JSValue ns);

void NativeTextareaInit(JSContext *ctx, JSValue ns);

void NativeCalendarInit(JSContext *ctx, JSValue ns);

void NativeChartInit(JSContext *ctx, JSValue ns);

void NativeColorwheelInit(JSContext *ctx, JSValue ns);

void NativeImgbtnInit(JSContext *ctx, JSValue ns);

void NativeIndevInit(JSContext *ctx, JSValue ns);

void NativeKeyboardInit(JSContext *ctx, JSValue ns);

void NativeMeterInit(JSContext *ctx, JSValue ns);

void NativeListInit(JSContext *ctx, JSValue ns);

void NativeTabviewInit(JSContext *ctx, JSValue ns);

void NativeSliderInit(JSContext *ctx, JSValue ns);

void NativeSwitchInit(JSContext *ctx, JSValue ns);

void NativeTabviewInit(JSContext *ctx, JSValue ns);

void NativeWinInit(JSContext *ctx, JSValue ns);

void NativeMenuInit(JSContext *ctx, JSValue ns);

void NativeSpanInit (JSContext *ctx, JSValue ns);

void NativeSpinboxInit (JSContext *ctx, JSValue ns);

void NativeLedInit (JSContext *ctx, JSValue ns);

void NativeMsgboxInit (JSContext *ctx, JSValue ns);

void NativeSpinnerInit (JSContext *ctx, JSValue ns);

void NativeTileviewInit (JSContext *ctx, JSValue ns);

void VbarStyleInit (JSContext *ctx, JSValue ns);

void LvglUtilsInit (JSContext *ctx, JSValue ns);
#ifdef __cplusplus
}
#endif