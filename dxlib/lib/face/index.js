import FFI from 'tjs:ffi';
import path from 'tjs:path';
const ffiInt = globalThis[Symbol.for('tjs.internal.core')].ffi_load_native();

let onFaceTrackCallback = null
let onFaceRecognitionCallback = null


let sopath = '/os/driver/';
sopath = sopath + './libface_wrapper.so';
const faceLib = new FFI.Lib(sopath);


faceLib.parseCProto(`
    struct track_t{
        int x1;
        int y1;
        int x2;
        int y2;
        int id;
    };
    struct recognition_t{
        float score;
        int is_living;
        int living_score;
    };
    struct face_config_t
    {
        int living_check_enable;
    };
    void face_init(void *rgb, void *nir, struct face_config_t *options);
    void face_update_config(struct face_config_t *options);
`);


const structTrack = faceLib.getType('struct track_t');
const structRecognition = faceLib.getType('struct recognition_t');
const structFaceConfig = faceLib.getType('struct face_config_t');


const faceGetTrackData1 = new FFI.CFunction(faceLib.symbol('get_face_track_data'), structTrack, []);

const faceGetRecognitionResult1 = new FFI.CFunction(faceLib.symbol('get_face_recognition_result'), structRecognition, [FFI.types.buffer]);

// const faceInit1 = new FFI.CFunction(faceLib.symbol('face_init'), FFI.types.sint, [FFI.types.pointer, FFI.types.pointer, structFaceConfig]);



// const faceGetTrackArray = new FFI.CFunction(
//     faceLib.symbol('face_get_track_array'),
//     new FFI.PointerType(FFI.types.sint, 4),
//     []
// );

const faceSetPause = new FFI.CFunction(
    faceLib.symbol('face_set_pause'),
    FFI.types.sint,
    [FFI.types.sint]
);

const faceRegister1 = new FFI.CFunction(
    faceLib.symbol('face_register'),
    FFI.types.sint,
    [FFI.types.string]
);

// const faceGetRecognitionResult = new FFI.CFunction(
//     faceLib.symbol('face_get_recognition_result'),
//     FFI.types.string,
//     []
// );

// const faceUpdateConfig1 = new FFI.CFunction(
//     faceLib.symbol('face_update_config'),
//     FFI.types.void,
//     [FFI.types.sint]
// );

/**
 * 获取人脸跟踪数据
 * @returns {x1: number, y1: number, x2: number, y2: number, id: number}
 */
function getFaceTrackData() {
    let trackData = faceGetTrackData1.call();
    if (trackData.x1 == 0) {
        return null;
    }
    return trackData;
}

/**
 * 获取人脸识别结果
 * @returns {userid: string, score: number, is_living: number, living_score: number}
 */
function getFaceRecognitionResult() {
    const userid = new Uint8Array(256);
    let recognitionData = faceGetRecognitionResult1.call(userid);
    if (recognitionData.score == 0) {
        return null;
    }
    recognitionData.userid = FFI.bufferToString(userid);
    return recognitionData;
}

/**
 * 初始化人脸识别
 * @param {pointer} rgbCapturer 彩色摄像头
 * @param {pointer} nirCapturer 红外摄像头
 * @returns {void}
 */
function faceInit(rgbCapturer, nirCapturer, options = { living_check_enable: 0 }) {
    if (!rgbCapturer || !nirCapturer)
        return;
    faceLib.call('face_init', rgbCapturer, nirCapturer, FFI.Pointer.createRef(structFaceConfig, {
        living_check_enable: options.living_check_enable,
    }))
}

function faceUpdateConfig(options = { living_check_enable: 0 }) {
    faceLib.call('face_update_config', FFI.Pointer.createRef(structFaceConfig, {
        living_check_enable: options.living_check_enable,
    }))
}

function setFacePause(pause) {
    faceSetPause.call(pause);
}

// function setFaceTrackCallback(callback) {
//     onFaceTrackCallback = callback;
// }

// function setFaceRecognitionCallback(callback) {
//     onFaceRecognitionCallback = callback;
// }

function faceRegister(userName) {
    return faceRegister1.call(userName);
}

// function faceUpdateConfig(livenessOff) {
//     faceUpdateConfig1.call(livenessOff);
// }

export { faceInit, getFaceTrackData, getFaceRecognitionResult, faceUpdateConfig, setFacePause, faceRegister };