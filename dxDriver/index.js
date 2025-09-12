// 导入各个模块
import { capturerInit, capturerDeinit } from './lib/capturer/index.js';
import { getUuid, md5HashFile } from './lib/common/index.js';
import {
    getEnableStatus,
    setEnableStatus,
    getBacklight,
    setBacklight,
    getPowerMode,
    setPowerMode
} from './lib/display/index.js';
import { faceInit, faceUpdateConfig, getFaceRecognitionResult, getFaceTrackData, setFacePause, faceRegister, faceDeinit } from './lib/face/index.js';
import { mqttInit, mqttDeinit, setConnectedCallback, setStatusCallback, setMessageCallback, subscribe, publish } from './lib/mqtt/index.js';
import { pwmRequest, pwmSetPeriodByChannel, pwmEnable, pwmSetDutyByChannel, pwmFree, setIrLedBrightness, setWhiteLedBrightness } from './lib/pwm/index.js';
import { initGpio, deinitGpio, requestGpio, freeGpio, setFuncGpio, setPullStateGpio, getPullStateGpio, setValueGpio, getValueGpio, setDriveStrengthGpio, getDriveStrengthGpio, setRelayStatus } from './lib/gpio/index.js';
import { audioInit, audioDeinit, audioPlay, audioPlayingInterrupt, audioGetVolume, audioSetVolume, audioGetVolumeRange } from './lib/audio/index.js';


// 摄像头模块
export const capturer = {
    capturerInit,
    capturerDeinit
};

// 人脸识别模块
export const face = {
    faceInit,
    faceUpdateConfig,
    getFaceRecognitionResult,
    getFaceTrackData,
    setFacePause,
    faceRegister,
    faceDeinit
};

// MQTT模块
export const mqtt = {
    mqttInit,
    mqttDeinit,
    setConnectedCallback,
    setStatusCallback,
    setMessageCallback,
    subscribe,
    publish
};

// PWM模块
export const pwm = {
    pwmRequest,
    pwmSetPeriodByChannel,
    pwmEnable,
    pwmSetDutyByChannel,
    pwmFree,
    setIrLedBrightness,
    setWhiteLedBrightness
};

// 显示模块
export const display = {
    getEnableStatus,
    setEnableStatus,
    getBacklight,
    setBacklight,
    getPowerMode,
    setPowerMode
};

// 通用工具模块
export const common = {
    getUuid,
    md5HashFile
};

// GPIO模块
export const gpio = {
    initGpio,
    deinitGpio,
    requestGpio,
    freeGpio,
    setFuncGpio,
    setPullStateGpio,
    getPullStateGpio,
    setValueGpio,
    getValueGpio,
    setDriveStrengthGpio,
    getDriveStrengthGpio,
    setRelayStatus
};

// 音频模块
export const audio = {
    audioInit,
    audioDeinit,
    audioPlay,
    audioPlayingInterrupt,
    audioGetVolume,
    audioSetVolume,
    audioGetVolumeRange
};
