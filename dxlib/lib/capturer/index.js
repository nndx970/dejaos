import FFI from 'tjs:ffi';

let sopath = '/os/driver/';
sopath = sopath + './libcapturer_wrapper.so';
const capturerLib = new FFI.Lib(sopath);

// 定义两个 capturer init 函数的 FFI 接口
const capturerRgbInit = new FFI.CFunction(
    capturerLib.symbol('capturer_rgb_init'),
    FFI.types.pointer,
    []
);

const capturerNirInit = new FFI.CFunction(
    capturerLib.symbol('capturer_nir_init'),
    FFI.types.pointer,
    []
);

let rgbCapturer = null;
let nirCapturer = null;

function capturerInit() {
    if (rgbCapturer && nirCapturer) {
        return { rgbCapturer, nirCapturer };
    }
    rgbCapturer = capturerRgbInit.call();
    nirCapturer = capturerNirInit.call();
    if (!rgbCapturer || !nirCapturer) {
        return { rgbCapturer: null, nirCapturer: null };
    }
    return { rgbCapturer, nirCapturer };
}

export { capturerInit };