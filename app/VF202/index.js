import path from "tjs:path";
import { capturer, face, pwm, mqtt, display, common, audio } from "dxDriver";
import { config, mqttAccess } from "dxAccess";
import configJson from './config.json';
import { uiInit } from './src/ui/index.js';

async function main() {
    const configManager = await config.initConfigManager();
    // 初始化摄像头
    const { rgbCapturer, nirCapturer } = capturer.capturerInit();
    // 初始化人脸识别
    faceInit1(rgbCapturer, nirCapturer, configManager);
    // 初始化PWM
    pwmInit();
    // 初始化音频
    audioInit1();
    // 初始化MQTT
    mqttInit1(configManager);
    // 初始化UI
    uiInit();
    // 初始化显示
    displayInit();

    // 内存检测
    setInterval(() => {
        tjs.spawn(['free', '-m'])
    }, 1000);
}

main().catch(console.error);

export default main;

function faceInit1(rgbCapturer, nirCapturer, configManager) {
    // face.faceInit(rgbCapturer, nirCapturer, { living_check_enable: configManager.get('face.livenessOff') });
    face.faceInit(rgbCapturer, nirCapturer, { living_check_enable: 0 });
}

function mqttInit1(configManager) {
    configJson.mqtt.clientId = (configManager.get('mqtt.clientId') || common.getUuid(19)) + Math.floor(Math.random() * 1000).toString().padStart(3, '0');
    // configJson.mqtt.addr = configManager.get('mqtt.addr');
    // configJson.mqtt.username = configManager.get('mqtt.username');
    // configJson.mqtt.password = configManager.get('mqtt.password');
    configJson.mqtt.willTopic = configManager.get('mqtt.willTopic');
    console.log(configJson.mqtt);

    mqtt.mqttInit(configJson.mqtt);
    mqttAccess.mqttAccessInit();
}

function displayInit() {
    // 不自动熄屏，状态常亮，亮度100
    display.setEnableStatus(1);
    display.setBacklight(100);
    display.setPowerMode(0);
}

function pwmInit() {
    pwm.setIrLedBrightness(255);
    pwm.setWhiteLedBrightness(255);
}

function audioInit1() {
    // audio.audioInit(100);
    // let resourcePath = path.join(import.meta.dirname, '/resource');
    // audio.audioPlay(resourcePath + '/audio/welcome.wav');
}