import FFI from 'tjs:ffi';

let sopath = '/os/driver/';
sopath = sopath + './libgpio_wrapper.so';
const gpioLib = new FFI.Lib(sopath);

const init_gpio = new FFI.CFunction(gpioLib.symbol('init_gpio'), FFI.types.sint, []);
const deinit_gpio = new FFI.CFunction(gpioLib.symbol('deinit_gpio'), FFI.types.void, []);
const request_gpio = new FFI.CFunction(gpioLib.symbol('request_gpio'), FFI.types.sint, [FFI.types.uint32]);
const free_gpio = new FFI.CFunction(gpioLib.symbol('free_gpio'), FFI.types.void, [FFI.types.uint32]);
const set_func_gpio = new FFI.CFunction(gpioLib.symbol('set_func_gpio'), FFI.types.void, [FFI.types.uint32, FFI.types.uint32]);
const set_pull_state_gpio = new FFI.CFunction(gpioLib.symbol('set_pull_state_gpio'), FFI.types.void, [FFI.types.uint32, FFI.types.uint32]);
const get_pull_state_gpio = new FFI.CFunction(gpioLib.symbol('get_pull_state_gpio'), FFI.types.sint, [FFI.types.uint32]);
const set_value_gpio = new FFI.CFunction(gpioLib.symbol('set_value_gpio'), FFI.types.void, [FFI.types.uint32, FFI.types.uint8]);
const get_value_gpio = new FFI.CFunction(gpioLib.symbol('get_value_gpio'), FFI.types.sint, [FFI.types.uint32]);
const set_drive_strength_gpio = new FFI.CFunction(gpioLib.symbol('set_drive_strength_gpio'), FFI.types.void, [FFI.types.uint32, FFI.types.uint16]);
const get_drive_strength_gpio = new FFI.CFunction(gpioLib.symbol('get_drive_strength_gpio'), FFI.types.uint16, [FFI.types.uint32]);

let hasInit = false;
function initGpio() {
    if (hasInit) {
        return;
    }
    hasInit = true;
    return init_gpio.call();
}

function deinitGpio() {
    if (!hasInit) {
        return;
    }
    hasInit = false;
    deinit_gpio.call();
}

const requestGpioMap = new Map();
function requestGpio(gpio) {
    if (requestGpioMap.has(gpio)) {
        return;
    }
    requestGpioMap.set(gpio, true);
    return request_gpio.call(gpio);
}

function freeGpio(gpio) {
    free_gpio.call(gpio);
}

function setFuncGpio(gpio, func) {
    set_func_gpio.call(gpio, func);
}

function setPullStateGpio(gpio, state) {
    set_pull_state_gpio.call(gpio, state);
}

function getPullStateGpio(gpio) {
    return get_pull_state_gpio.call(gpio);
}

function setValueGpio(gpio, value) {
    set_value_gpio.call(gpio, value);
}

function getValueGpio(gpio) {
    return get_value_gpio.call(gpio);
}

function setDriveStrengthGpio(gpio, strength) {
    set_drive_strength_gpio.call(gpio, strength);
}

function getDriveStrengthGpio(gpio) {
    return get_drive_strength_gpio.call(gpio);
}

// VF202-继电器GPIO
const relayGpio = 67;
function setRelayStatus(status) {
    initGpio();
    requestGpio(relayGpio);
    setValueGpio(relayGpio, status);
}

export { initGpio, deinitGpio, requestGpio, freeGpio, setFuncGpio, setPullStateGpio, getPullStateGpio, setValueGpio, getValueGpio, setDriveStrengthGpio, getDriveStrengthGpio, setRelayStatus };