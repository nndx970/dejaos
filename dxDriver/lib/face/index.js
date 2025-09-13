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


const faceSetPause = new FFI.CFunction(
    faceLib.symbol('face_set_pause'),
    FFI.types.sint,
    [FFI.types.sint]
);

const faceRegister1 = new FFI.CFunction(
    faceLib.symbol('face_register'),
    FFI.types.void,
    [FFI.types.string]
);

const faceRegisterReset1 = new FFI.CFunction(
    faceLib.symbol('face_register_reset'),
    FFI.types.void,
    []
);

const faceGetRegisterResult1 = new FFI.CFunction(
    faceLib.symbol('get_face_register_result'),
    FFI.types.sint,
    []
);

const faceDeinit1 = new FFI.CFunction(
    faceLib.symbol('face_deinit'),
    FFI.types.void,
    []
);

const faceSetRecognitionResult1 = new FFI.CFunction(
    faceLib.symbol('set_face_recognition_result'),
    FFI.types.void,
    [FFI.types.sint]
);

const faceGetSavedPicturePath1 = new FFI.CFunction(
    faceLib.symbol('get_saved_picture_path'),
    FFI.types.void,
    [FFI.types.buffer, FFI.types.buffer]
);

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
 * @param {*} callback 回调函数,参数为recognition_t(userid: string, score: number, is_living: number, living_score: number),返回值为boolean,如果返回true,则保存图片
 * @returns 
 */
function getFaceRecognitionResult(callback) {
    const userid = new Uint8Array(256);
    let recognitionData = faceGetRecognitionResult1.call(userid);
    if (recognitionData.score == 0) {
        recognitionData = null;
        return
    }
    recognitionData.userid = FFI.bufferToString(userid);
    if (callback) {
        let result = callback(recognitionData);
        if (result === true) {
            faceSetRecognitionResult1.call(1);
        } else {
            faceSetRecognitionResult1.call(0);
        }
    } else {
        faceSetRecognitionResult1.call(0);
    }
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

/**
 * 人脸注册
 * @param {*} userName 用户名
 * @returns 
 */
async function faceRegister(userName) {
    // 开始注册
    faceRegister1.call(userName);

    // 设置超时时间为5秒
    const timeout = 5000;
    const startTime = Date.now();

    while (true) {
        const currentTime = Date.now();
        const elapsedTime = currentTime - startTime;

        // 检查是否超时
        if (elapsedTime >= timeout) {
            faceRegisterReset1.call();
            return -99; // 超时错误
        }

        // 获取当前注册结果
        const result = faceGetRegisterResult1.call();

        // 如果结果不等于-99，说明注册完成（成功或失败）
        if (result !== -99) {
            return result;
        }

        // 等待50ms后继续检查
        await new Promise(resolve => setTimeout(resolve, 50));
    }
}

function faceGetSavedPicturePath() {
    const path = new Uint8Array(256);
    const thumbPath = new Uint8Array(256);
    faceGetSavedPicturePath1.call(path, thumbPath);
    return { path: FFI.bufferToString(path), thumbPath: FFI.bufferToString(thumbPath) };
}

function faceDeinit() {
    faceDeinit1.call();
}

export { faceInit, getFaceTrackData, getFaceRecognitionResult, faceUpdateConfig, setFacePause, faceRegister, faceDeinit, faceGetSavedPicturePath };