import FFI from 'tjs:ffi';


let sopath = '/os/driver/';
sopath = sopath + './libpwm_wrapper.so';
const pwmLib = new FFI.Lib(sopath);

const pwmInit1 = new FFI.CFunction(pwmLib.symbol('pwm_init'), FFI.types.sint, []);

function pwmInit() {
    pwmInit1.call();
}

export { pwmInit };