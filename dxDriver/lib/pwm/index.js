import FFI from 'tjs:ffi';

let sopath = '/os/driver/';
sopath = sopath + './libpwm_wrapper.so';
const pwmLib = new FFI.Lib(sopath);

const pwmRequest1 = new FFI.CFunction(pwmLib.symbol('pwm_request'), FFI.types.sint, [FFI.types.uint32]);
const pwmSetPeriodByChannel1 = new FFI.CFunction(pwmLib.symbol('pwm_set_period_by_channel'), FFI.types.sint, [FFI.types.uint32, FFI.types.uint32]);
const pwmEnable1 = new FFI.CFunction(pwmLib.symbol('pwm_enable'), FFI.types.sint, [FFI.types.uint32, FFI.types.sint]);
const pwmSetDutyByChannel1 = new FFI.CFunction(pwmLib.symbol('pwm_set_duty_by_channel'), FFI.types.sint, [FFI.types.uint32, FFI.types.uint32]);
const pwmFree1 = new FFI.CFunction(pwmLib.symbol('pwm_free'), FFI.types.sint, [FFI.types.uint32]);

function pwmRequest(channel) {
    return pwmRequest1.call(channel);
}

function pwmSetPeriodByChannel(channel, period_ns) {
    return pwmSetPeriodByChannel1.call(channel, period_ns);
}

function pwmEnable(channel, on) {
    return pwmEnable1.call(channel, on);
}

function pwmSetDutyByChannel(channel, duty_ns) {
    return pwmSetDutyByChannel1.call(channel, duty_ns);
}

function pwmFree(channel) {
    return pwmFree1.call(channel);
}

const pwmMap = new Map();
// 设置红外补光灯亮度
const pwmChannel = 7;
const pwmPeriod = 2400000;
const pwmDuty = 2400000 * 255 / 255;
function setIrLedBrightness(brightness = 255) {
    if (brightness > 255) {
        brightness = 255;
    }
    if (brightness < 0) {
        brightness = 0;
    }
    if (pwmMap.has(pwmChannel)) {
        pwmSetDutyByChannel(pwmChannel, pwmDuty * brightness / 255);
        return;
    }
    pwmMap.set(pwmChannel, brightness);
    pwmRequest(pwmChannel);
    pwmSetPeriodByChannel(pwmChannel, pwmPeriod);
    pwmEnable(pwmChannel, 1);
    pwmSetDutyByChannel(pwmChannel, pwmDuty * brightness / 255);
}

// 白色补光灯
const pwmChannelWhite = 4;
const pwmPeriodWhite = 2400000;
const pwmDutyWhite = 2400000 * 255 / 255;
function setWhiteLedBrightness(brightness = 255) {
    if (brightness > 255) {
        brightness = 255;
    }
    if (brightness < 0) {
        brightness = 0;
    }
    
    // 反转参数：0-255 映射为 255-0
    const invertedBrightness = 255 - brightness;
    
    if (pwmMap.has(pwmChannelWhite)) {
        pwmSetDutyByChannel(pwmChannelWhite, pwmDutyWhite * invertedBrightness / 255);
        return;
    }
    pwmMap.set(pwmChannelWhite, brightness);
    pwmRequest(pwmChannelWhite);
    pwmSetPeriodByChannel(pwmChannelWhite, pwmPeriodWhite);
    pwmEnable(pwmChannelWhite, 1);
    pwmSetDutyByChannel(pwmChannelWhite, pwmDutyWhite * invertedBrightness / 255);
}

export { pwmRequest, pwmSetPeriodByChannel, pwmEnable, pwmSetDutyByChannel, pwmFree, setIrLedBrightness, setWhiteLedBrightness };