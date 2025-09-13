import FFI from 'tjs:ffi';

let sopath = '/os/driver/';
sopath = sopath + './libdisplay_wrapper.so';
const displayLib = new FFI.Lib(sopath);

const getEnableStatus1 = new FFI.CFunction(displayLib.symbol('get_enable_status'), FFI.types.sint, []);
const setEnableStatus1 = new FFI.CFunction(displayLib.symbol('set_enable_status'), FFI.types.void, [FFI.types.sint]);
const getBacklight1 = new FFI.CFunction(displayLib.symbol('get_backlight'), FFI.types.sint, []);
const setBacklight1 = new FFI.CFunction(displayLib.symbol('set_backlight'), FFI.types.void, [FFI.types.sint]);
const getPowerMode1 = new FFI.CFunction(displayLib.symbol('get_power_mode'), FFI.types.sint, []);
const setPowerMode1 = new FFI.CFunction(displayLib.symbol('set_power_mode'), FFI.types.void, [FFI.types.sint]);

function getEnableStatus() {
    return getEnableStatus1.call();
}

function setEnableStatus(enable) {
    setEnableStatus1.call(enable);
}

function getBacklight() {
    return getBacklight1.call();
}

function setBacklight(backlight) {
    setBacklight1.call(backlight);
}

function getPowerMode() {
    return getPowerMode1.call();
}

function setPowerMode(powerMode) {
    setPowerMode1.call(powerMode);
}

export { getEnableStatus, setEnableStatus, getBacklight, setBacklight, getPowerMode, setPowerMode };    