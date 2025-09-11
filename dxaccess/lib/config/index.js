import { common } from 'dxlib';
import { onConfigKeyChangeService } from './service.js';
import { saveToFile as systemSaveToFile } from '../utils/index.js';

class ConfigManager {
    constructor() {
        this.configPath = "/data/config/config.json";
        this.config = this.getDefaultConfig();
        this.validators = this.createValidators();
        this.permissions = this.createPermissions();
        this.initialized = false;

        // 回调系统
        this.callbacks = {
            global: [], // 全局配置变更回调
            specific: new Map(), // 特定配置项变更回调
            init: [] // 初始化完成回调
        };
    }

    /**
     * 静态异步工厂方法
     * @returns {Promise<ConfigManager>} 初始化完成的配置管理器实例
     */
    static async create() {
        const instance = new ConfigManager();
        await instance.initialize();
        return instance;
    }

    /**
     * 异步初始化配置管理器
     */
    async initialize() {
        try {
            // 尝试加载持久化配置
            await this.loadFromFile();

            // 检查UUID是否为空字符串，如果是则自动生成
            await this.checkAndGenerateUuid();

            this.initialized = true;
            console.log("配置管理器初始化完成");

            // 执行初始化完成回调
            this._executeInitCallbacks();
        } catch (error) {
            console.warn("配置管理器初始化失败，使用默认配置:", error);
            this.initialized = true;

            // 即使初始化失败，也执行初始化完成回调
            this._executeInitCallbacks();
        }
    }

    /**
     * 检查并生成UUID
     */
    async checkAndGenerateUuid() {
        try {
            const currentUuid = this.get('sys.uuid');

            // 如果UUID为空字符串，则生成新的UUID
            if (currentUuid === '') {
                console.log('检测到UUID为空字符串，正在生成新的UUID...');

                // 调用common模块的getUuid函数，参数为19
                const newUuid = common.getUuid(19);

                if (newUuid && typeof newUuid === 'string' && newUuid.length > 0) {
                    // 直接设置UUID到配置中（绕过只读权限检查，因为这是初始化过程）
                    this.config.sys.uuid = newUuid;

                    // 保存配置到文件
                    await this.saveToFile();

                    console.log(`UUID已自动生成并设置: ${newUuid}`);
                } else {
                    console.error('UUID生成失败，返回值为空或无效');
                }
            } else {
                console.log(`UUID已存在: ${currentUuid}`);
            }
        } catch (error) {
            console.error('检查并生成UUID时发生错误:', error);
        }
    }

    /**
     * 获取默认配置
     */
    getDefaultConfig() {
        return {
            // 设备基础配置
            base: {
                language: "CN",
                password: "1",
                firstLogin: 0,
                appMode: 1,
                appVersion: "",
                releaseTime: "",
                devType: 3,
                restartCount: 0,
                volume: 50,
                heartEn: 0,
                heartTime: 30,
                heartChannel: 0,
                userdata: ""
            },
            // 显示界面配置
            ui: {
                screenOff: 0,
                screensaver: 0,
                brightness: 70,
                brightnessAuto: 1,
                showIp: 1,
                showSn: 1,
                showNir: 1,
                showWXPro: 1,
                showIdCard: 1,
                showLogo: 0,
                logoImage: "",
                showCRZ: 0
            },
            // 网络通信配置
            net: {
                type: 1,
                ssid: "",
                psk: "",
                ssidENC: "",
                dhcp: 2,
                ip: "",
                gateway: "",
                mask: "",
                dns: "",
                mac: "",
                ntp: 1,
                server: "182.92.12.11",
                hour: 3,
                gmt: 8
            },
            // MQTT通信配置
            mqtt: {
                addr: "mqtt://192.168.10.166:1883",
                clientId: "",
                username: "",
                password: "",
                qos: 1,
                prefix: "",
                willTopic: "access_device/v2/event/offline",
                cleanSession: 0
            },
            // 人脸识别配置
            face: {
                similarity: 0.6,
                livenessOff: 1,
                livenessVal: 10,
                detectMask: 0,
                stranger: 1,
                voiceMode: 1,
                voiceSucCum: "欢迎光临"
            },
            // 门禁控制配置
            access: {
                offlineAcsNum: 2000,
                reportTime: 5,
                tamperAlarm: 0,
                relayTime: 2,
                onlinecheck: 0,
                timeout: 5000,
                showPwd: 1,
                nfc: 1,
                Idcard: 1,
                strangerImg: 1,
                accessImageType: 1
            },
            // 系统标识配置
            sys: {
                devmac: "",
                uuid: ""
            }
        };
    }

    /**
     * 创建验证器
     */
    createValidators() {
        return {
            // 设备基础配置验证
            "base.language": (value) => {
                if (!["CN", "EN"].includes(value)) {
                    return { valid: false, message: `语言设置无效，当前值: ${value}，有效值: CN, EN` };
                }
                return { valid: true };
            },
            "base.password": (value) => {
                if (typeof value !== "string") {
                    return { valid: false, message: `密码必须是字符串类型，当前类型: ${typeof value}` };
                }
                if (value.length < 1 || value.length > 20) {
                    return { valid: false, message: `密码长度无效，当前长度: ${value.length}，有效范围: 1-20字符` };
                }
                return { valid: true };
            },
            "base.firstLogin": (value) => {
                if (![0, 1].includes(value)) {
                    return { valid: false, message: `首次登录标识无效，当前值: ${value}，有效值: 0, 1` };
                }
                return { valid: true };
            },
            "base.appMode": (value) => {
                if (![0, 1].includes(value)) {
                    return { valid: false, message: `应用模式无效，当前值: ${value}，有效值: 0, 1` };
                }
                return { valid: true };
            },
            "base.appVersion": (value) => {
                if (typeof value !== "string") {
                    return { valid: false, message: `应用版本必须是字符串类型，当前类型: ${typeof value}` };
                }
                if (value.length < 1 || value.length > 32) {
                    return { valid: false, message: `应用版本长度无效，当前长度: ${value.length}，有效范围: 1-32字符` };
                }
                return { valid: true };
            },
            "base.releaseTime": (value) => {
                if (typeof value !== "string") {
                    return { valid: false, message: `发布时间必须是字符串类型，当前类型: ${typeof value}` };
                }
                if (value.length < 1 || value.length > 32) {
                    return { valid: false, message: `发布时间长度无效，当前长度: ${value.length}，有效范围: 1-32字符` };
                }
                return { valid: true };
            },
            "base.devType": (value) => {
                if (![0, 1, 2, 3].includes(value)) {
                    return { valid: false, message: `设备类型无效，当前值: ${value}，有效值: 0, 1, 2, 3` };
                }
                return { valid: true };
            },
            "base.restartCount": (value) => {
                if (!Number.isInteger(value)) {
                    return { valid: false, message: `重启次数必须是整数，当前值: ${value}，类型: ${typeof value}` };
                }
                if (value < 0 || value > 999999) {
                    return { valid: false, message: `重启次数超出范围，当前值: ${value}，有效范围: 0-999999` };
                }
                return { valid: true };
            },
            "base.volume": (value) => {
                if (!Number.isInteger(value)) {
                    return { valid: false, message: `音量必须是整数，当前值: ${value}，类型: ${typeof value}` };
                }
                if (value < 0 || value > 100) {
                    return { valid: false, message: `音量超出范围，当前值: ${value}，有效范围: 0-100` };
                }
                return { valid: true };
            },
            "base.heartEn": (value) => {
                if (![0, 1].includes(value)) {
                    return { valid: false, message: `心跳使能无效，当前值: ${value}，有效值: 0, 1` };
                }
                return { valid: true };
            },
            "base.heartTime": (value) => {
                if (!Number.isInteger(value)) {
                    return { valid: false, message: `心跳时间必须是整数，当前值: ${value}，类型: ${typeof value}` };
                }
                if (value < 10 || value > 300) {
                    return { valid: false, message: `心跳时间超出范围，当前值: ${value}，有效范围: 10-300秒` };
                }
                return { valid: true };
            },
            "base.heartChannel": (value) => {
                if (![0, 1].includes(value)) {
                    return { valid: false, message: `心跳通道无效，当前值: ${value}，有效值: 0, 1` };
                }
                return { valid: true };
            },
            "base.userdata": (value) => {
                if (typeof value !== "string") {
                    return { valid: false, message: `用户数据必须是字符串类型，当前类型: ${typeof value}` };
                }
                return { valid: true };
            },

            // 显示界面配置验证
            "ui.screenOff": (value) => {
                if (!Number.isInteger(value)) {
                    return { valid: false, message: `屏幕关闭时间必须是整数，当前值: ${value}，类型: ${typeof value}` };
                }
                if (value < 0 || value > 86400) {
                    return { valid: false, message: `屏幕关闭时间超出范围，当前值: ${value}，有效范围: 0-86400秒` };
                }
                return { valid: true };
            },
            "ui.screensaver": (value) => {
                if (!Number.isInteger(value)) {
                    return { valid: false, message: `屏保时间必须是整数，当前值: ${value}，类型: ${typeof value}` };
                }
                if (value < 0 || value > 86400) {
                    return { valid: false, message: `屏保时间超出范围，当前值: ${value}，有效范围: 0-86400秒` };
                }
                return { valid: true };
            },
            "ui.brightness": (value) => {
                if (!Number.isInteger(value)) {
                    return { valid: false, message: `亮度必须是整数，当前值: ${value}，类型: ${typeof value}` };
                }
                if (value < 0 || value > 100) {
                    return { valid: false, message: `亮度超出范围，当前值: ${value}，有效范围: 0-100` };
                }
                return { valid: true };
            },
            "ui.brightnessAuto": (value) => {
                if (![0, 1].includes(value)) {
                    return { valid: false, message: `自动亮度无效，当前值: ${value}，有效值: 0, 1` };
                }
                return { valid: true };
            },
            "ui.showIp": (value) => {
                if (![0, 1].includes(value)) {
                    return { valid: false, message: `显示IP无效，当前值: ${value}，有效值: 0, 1` };
                }
                return { valid: true };
            },
            "ui.showSn": (value) => {
                if (![0, 1].includes(value)) {
                    return { valid: false, message: `显示序列号无效，当前值: ${value}，有效值: 0, 1` };
                }
                return { valid: true };
            },
            "ui.showNir": (value) => {
                if (![0, 1].includes(value)) {
                    return { valid: false, message: `显示NIR无效，当前值: ${value}，有效值: 0, 1` };
                }
                return { valid: true };
            },
            "ui.showWXPro": (value) => {
                if (![0, 1].includes(value)) {
                    return { valid: false, message: `显示WXPro无效，当前值: ${value}，有效值: 0, 1` };
                }
                return { valid: true };
            },
            "ui.showIdCard": (value) => {
                if (![0, 1].includes(value)) {
                    return { valid: false, message: `显示身份证无效，当前值: ${value}，有效值: 0, 1` };
                }
                return { valid: true };
            },
            "ui.showLogo": (value) => {
                if (![0, 1].includes(value)) {
                    return { valid: false, message: `显示Logo无效，当前值: ${value}，有效值: 0, 1` };
                }
                return { valid: true };
            },
            "ui.logoImage": (value) => {
                if (typeof value !== "string") {
                    return { valid: false, message: `Logo图片路径必须是字符串类型，当前类型: ${typeof value}` };
                }
                if (value.length < 0 || value.length > 64) {
                    return { valid: false, message: `Logo图片路径长度无效，当前长度: ${value.length}，有效范围: 0-64字符` };
                }
                return { valid: true };
            },
            "ui.showCRZ": (value) => {
                if (![0, 1].includes(value)) {
                    return { valid: false, message: `显示CRZ无效，当前值: ${value}，有效值: 0, 1` };
                }
                return { valid: true };
            },

            // 网络通信配置验证
            "net.type": (value) => {
                if (![1, 2, 4].includes(value)) {
                    return { valid: false, message: `网络类型无效，当前值: ${value}，有效值: 1, 2, 4` };
                }
                return { valid: true };
            },
            "net.ssid": (value) => {
                if (typeof value !== "string") {
                    return { valid: false, message: `SSID必须是字符串类型，当前类型: ${typeof value}` };
                }
                if (value.length < 1 || value.length > 32) {
                    return { valid: false, message: `SSID长度无效，当前长度: ${value.length}，有效范围: 1-32字符` };
                }
                return { valid: true };
            },
            "net.psk": (value) => {
                if (typeof value !== "string") {
                    return { valid: false, message: `密码必须是字符串类型，当前类型: ${typeof value}` };
                }
                if (value.length < 8 || value.length > 64) {
                    return { valid: false, message: `密码长度无效，当前长度: ${value.length}，有效范围: 8-64字符` };
                }
                return { valid: true };
            },
            "net.ssidENC": (value) => {
                if (typeof value !== "string") {
                    return { valid: false, message: `加密SSID必须是字符串类型，当前类型: ${typeof value}` };
                }
                if (value.length < 1 || value.length > 32) {
                    return { valid: false, message: `加密SSID长度无效，当前长度: ${value.length}，有效范围: 1-32字符` };
                }
                return { valid: true };
            },
            "net.dhcp": (value) => {
                if (![1, 2].includes(value)) {
                    return { valid: false, message: `DHCP设置无效，当前值: ${value}，有效值: 1, 2` };
                }
                return { valid: true };
            },
            "net.ip": (value) => {
                if (!this.isValidIPv4(value)) {
                    return { valid: false, message: `IP地址格式无效，当前值: ${value}，格式要求: xxx.xxx.xxx.xxx` };
                }
                return { valid: true };
            },
            "net.gateway": (value) => {
                if (!this.isValidIPv4(value)) {
                    return { valid: false, message: `网关地址格式无效，当前值: ${value}，格式要求: xxx.xxx.xxx.xxx` };
                }
                return { valid: true };
            },
            "net.mask": (value) => {
                if (!this.isValidIPv4(value)) {
                    return { valid: false, message: `子网掩码格式无效，当前值: ${value}，格式要求: xxx.xxx.xxx.xxx` };
                }
                return { valid: true };
            },
            "net.dns": (value) => {
                if (!this.isValidIPv4(value)) {
                    return { valid: false, message: `DNS地址格式无效，当前值: ${value}，格式要求: xxx.xxx.xxx.xxx` };
                }
                return { valid: true };
            },
            "net.mac": (value) => {
                if (!this.isValidMAC(value)) {
                    return { valid: false, message: `MAC地址格式无效，当前值: ${value}，格式要求: XX:XX:XX:XX:XX:XX` };
                }
                return { valid: true };
            },
            "net.ntp": (value) => {
                if (![0, 1].includes(value)) {
                    return { valid: false, message: `NTP设置无效，当前值: ${value}，有效值: 0, 1` };
                }
                return { valid: true };
            },
            "net.server": (value) => {
                if (!this.isValidIPv4(value)) {
                    return { valid: false, message: `NTP服务器地址格式无效，当前值: ${value}，格式要求: xxx.xxx.xxx.xxx` };
                }
                return { valid: true };
            },
            "net.hour": (value) => {
                if (!Number.isInteger(value)) {
                    return { valid: false, message: `小时必须是整数，当前值: ${value}，类型: ${typeof value}` };
                }
                if (value < 0 || value > 23) {
                    return { valid: false, message: `小时超出范围，当前值: ${value}，有效范围: 0-23` };
                }
                return { valid: true };
            },
            "net.gmt": (value) => {
                if (!Number.isInteger(value)) {
                    return { valid: false, message: `GMT时区必须是整数，当前值: ${value}，类型: ${typeof value}` };
                }
                if (value < -12 || value > 14) {
                    return { valid: false, message: `GMT时区超出范围，当前值: ${value}，有效范围: -12到14` };
                }
                return { valid: true };
            },

            // MQTT通信配置验证
            "mqtt.addr": (value) => {
                if (!this.isValidMQTTAddr(value)) {
                    return { valid: false, message: `MQTT地址格式无效，当前值: ${value}，格式要求: tcp://, ssl://, mqtt://, mqtts://开头` };
                }
                return { valid: true };
            },
            "mqtt.clientId": (value) => {
                if (typeof value !== "string") {
                    return { valid: false, message: `MQTT客户端ID必须是字符串类型，当前类型: ${typeof value}` };
                }
                if (value.length < 1 || value.length > 23) {
                    return { valid: false, message: `MQTT客户端ID长度无效，当前长度: ${value.length}，有效范围: 1-23字符` };
                }
                return { valid: true };
            },
            "mqtt.username": (value) => {
                if (typeof value !== "string") {
                    return { valid: false, message: `MQTT用户名必须是字符串类型，当前类型: ${typeof value}` };
                }
                if (value.length < 0 || value.length > 12) {
                    return { valid: false, message: `MQTT用户名长度无效，当前长度: ${value.length}，有效范围: 0-12字符` };
                }
                return { valid: true };
            },
            "mqtt.password": (value) => {
                if (typeof value !== "string") {
                    return { valid: false, message: `MQTT密码必须是字符串类型，当前类型: ${typeof value}` };
                }
                if (value.length < 0 || value.length > 12) {
                    return { valid: false, message: `MQTT密码长度无效，当前长度: ${value.length}，有效范围: 0-12字符` };
                }
                return { valid: true };
            },
            "mqtt.qos": (value) => {
                if (![0, 1, 2].includes(value)) {
                    return { valid: false, message: `MQTT QoS无效，当前值: ${value}，有效值: 0, 1, 2` };
                }
                return { valid: true };
            },
            "mqtt.prefix": (value) => {
                if (typeof value !== "string") {
                    return { valid: false, message: `MQTT前缀必须是字符串类型，当前类型: ${typeof value}` };
                }
                if (value.length < 0 || value.length > 64) {
                    return { valid: false, message: `MQTT前缀长度无效，当前长度: ${value.length}，有效范围: 0-64字符` };
                }
                return { valid: true };
            },
            "mqtt.willTopic": (value) => {
                if (typeof value !== "string") {
                    return { valid: false, message: `MQTT遗嘱主题必须是字符串类型，当前类型: ${typeof value}` };
                }
                if (value.length < 1 || value.length > 64) {
                    return { valid: false, message: `MQTT遗嘱主题长度无效，当前长度: ${value.length}，有效范围: 1-64字符` };
                }
                return { valid: true };
            },
            "mqtt.cleanSession": (value) => {
                if (![0, 1].includes(value)) {
                    return { valid: false, message: `MQTT清理会话无效，当前值: ${value}，有效值: 0, 1` };
                }
                return { valid: true };
            },

            // 人脸识别配置验证
            "face.similarity": (value) => {
                if (typeof value !== "number") {
                    return { valid: false, message: `人脸相似度必须是数字类型，当前类型: ${typeof value}` };
                }
                if (value < 0.0 || value > 1.0) {
                    return { valid: false, message: `人脸相似度超出范围，当前值: ${value}，有效范围: 0.0-1.0` };
                }
                return { valid: true };
            },
            "face.livenessOff": (value) => {
                if (![0, 1].includes(value)) {
                    return { valid: false, message: `活体检测关闭无效，当前值: ${value}，有效值: 0, 1` };
                }
                return { valid: true };
            },
            "face.livenessVal": (value) => {
                if (!Number.isInteger(value)) {
                    return { valid: false, message: `活体检测值必须是整数，当前值: ${value}，类型: ${typeof value}` };
                }
                if (value < 0 || value > 100) {
                    return { valid: false, message: `活体检测值超出范围，当前值: ${value}，有效范围: 0-100` };
                }
                return { valid: true };
            },
            "face.detectMask": (value) => {
                if (![0, 1].includes(value)) {
                    return { valid: false, message: `口罩检测无效，当前值: ${value}，有效值: 0, 1` };
                }
                return { valid: true };
            },
            "face.stranger": (value) => {
                if (![0, 1, 2, 3].includes(value)) {
                    return { valid: false, message: `陌生人处理无效，当前值: ${value}，有效值: 0, 1, 2, 3` };
                }
                return { valid: true };
            },
            "face.voiceMode": (value) => {
                if (![0, 1, 2, 3].includes(value)) {
                    return { valid: false, message: `语音模式无效，当前值: ${value}，有效值: 0, 1, 2, 3` };
                }
                return { valid: true };
            },
            "face.voiceSucCum": (value) => {
                if (typeof value !== "string") {
                    return { valid: false, message: `成功语音提示必须是字符串类型，当前类型: ${typeof value}` };
                }
                if (value.length < 1 || value.length > 64) {
                    return { valid: false, message: `成功语音提示长度无效，当前长度: ${value.length}，有效范围: 1-64字符` };
                }
                return { valid: true };
            },

            // 门禁控制配置验证
            "access.offlineAcsNum": (value) => {
                if (!Number.isInteger(value)) {
                    return { valid: false, message: `离线门禁数量必须是整数，当前值: ${value}，类型: ${typeof value}` };
                }
                if (value < 100 || value > 10000) {
                    return { valid: false, message: `离线门禁数量超出范围，当前值: ${value}，有效范围: 100-10000` };
                }
                return { valid: true };
            },
            "access.reportTime": (value) => {
                if (!Number.isInteger(value)) {
                    return { valid: false, message: `上报时间必须是整数，当前值: ${value}，类型: ${typeof value}` };
                }
                if (value < 2 || value > 60) {
                    return { valid: false, message: `上报时间超出范围，当前值: ${value}，有效范围: 2-60秒` };
                }
                return { valid: true };
            },
            "access.tamperAlarm": (value) => {
                if (![0, 1].includes(value)) {
                    return { valid: false, message: `防拆报警无效，当前值: ${value}，有效值: 0, 1` };
                }
                return { valid: true };
            },
            "access.relayTime": (value) => {
                if (!Number.isInteger(value)) {
                    return { valid: false, message: `继电器时间必须是整数，当前值: ${value}，类型: ${typeof value}` };
                }
                if (value < 2 || value > 30) {
                    return { valid: false, message: `继电器时间超出范围，当前值: ${value}，有效范围: 2-30秒` };
                }
                return { valid: true };
            },
            "access.onlinecheck": (value) => {
                if (![0, 1].includes(value)) {
                    return { valid: false, message: `在线检查无效，当前值: ${value}，有效值: 0, 1` };
                }
                return { valid: true };
            },
            "access.timeout": (value) => {
                if (!Number.isInteger(value)) {
                    return { valid: false, message: `超时时间必须是整数，当前值: ${value}，类型: ${typeof value}` };
                }
                if (value < 1000 || value > 60000) {
                    return { valid: false, message: `超时时间超出范围，当前值: ${value}，有效范围: 1000-60000毫秒` };
                }
                return { valid: true };
            },
            "access.showPwd": (value) => {
                if (![0, 1].includes(value)) {
                    return { valid: false, message: `显示密码无效，当前值: ${value}，有效值: 0, 1` };
                }
                return { valid: true };
            },
            "access.nfc": (value) => {
                if (![0, 1].includes(value)) {
                    return { valid: false, message: `NFC功能无效，当前值: ${value}，有效值: 0, 1` };
                }
                return { valid: true };
            },
            "access.Idcard": (value) => {
                if (![1, 2, 3].includes(value)) {
                    return { valid: false, message: `身份证功能无效，当前值: ${value}，有效值: 1, 2, 3` };
                }
                return { valid: true };
            },
            "access.strangerImg": (value) => {
                if (![0, 1].includes(value)) {
                    return { valid: false, message: `陌生人图片无效，当前值: ${value}，有效值: 0, 1` };
                }
                return { valid: true };
            },
            "access.accessImageType": (value) => {
                if (![0, 1].includes(value)) {
                    return { valid: false, message: `门禁图片类型无效，当前值: ${value}，有效值: 0, 1` };
                }
                return { valid: true };
            },

            // 系统标识配置验证
            "sys.devmac": (value) => {
                if (!this.isValidMAC(value)) {
                    return { valid: false, message: `设备MAC地址格式无效，当前值: ${value}，格式要求: XX:XX:XX:XX:XX:XX` };
                }
                return { valid: true };
            },
            "sys.uuid": (value) => {
                if (typeof value !== "string") {
                    return { valid: false, message: `设备UUID必须是字符串类型，当前类型: ${typeof value}` };
                }
                if (value.length < 6 || value.length > 32) {
                    return { valid: false, message: `设备UUID长度无效，当前长度: ${value.length}，有效范围: 6-32字符` };
                }
                return { valid: true };
            }
        };
    }

    /**
     * 创建权限控制
     */
    createPermissions() {
        return {
            // 只读权限
            readonly: [
                "base.firstLogin", "base.appVersion", "base.releaseTime", "base.devType",
                "base.restartCount", "base.userdata", "sys.devmac", "sys.uuid"
            ],
            // 读写权限
            readwrite: [
                "base.language", "base.password", "base.appMode", "base.volume",
                "base.heartEn", "base.heartTime", "base.heartChannel",
                "ui.screenOff", "ui.screensaver", "ui.brightness", "ui.brightnessAuto",
                "ui.showIp", "ui.showSn", "ui.showNir", "ui.showWXPro", "ui.showIdCard",
                "ui.showLogo", "ui.logoImage", "ui.showCRZ",
                "net.type", "net.ssid", "net.psk", "net.ssidENC", "net.dhcp",
                "net.ip", "net.gateway", "net.mask", "net.dns", "net.mac",
                "net.ntp", "net.server", "net.hour", "net.gmt",
                "mqtt.addr", "mqtt.clientId", "mqtt.username", "mqtt.password",
                "mqtt.qos", "mqtt.prefix", "mqtt.willTopic", "mqtt.cleanSession",
                "face.similarity", "face.livenessOff", "face.livenessVal", "face.detectMask",
                "face.stranger", "face.voiceMode", "face.voiceSucCum",
                "access.offlineAcsNum", "access.reportTime", "access.tamperAlarm",
                "access.relayTime", "access.onlinecheck", "access.timeout",
                "access.showPwd", "access.nfc", "access.Idcard", "access.strangerImg",
                "access.accessImageType"
            ]
        };
    }

    /**
     * 验证IPv4地址格式
     */
    isValidIPv4(ip) {
        if (typeof ip !== "string") return false;
        const parts = ip.split(".");
        if (parts.length !== 4) return false;
        return parts.every(part => {
            const num = parseInt(part, 10);
            return !isNaN(num) && num >= 0 && num <= 255;
        });
    }

    /**
     * 验证MAC地址格式
     */
    isValidMAC(mac) {
        if (typeof mac !== "string") return false;
        const macRegex = /^([0-9A-Fa-f]{2}[:-]){5}([0-9A-Fa-f]{2})$/;
        return macRegex.test(mac) && mac.length === 17;
    }

    /**
     * 验证MQTT地址格式
     */
    isValidMQTTAddr(addr) {
        if (typeof addr !== "string") return false;
        if (addr.length < 1 || addr.length > 128) return false;
        const protocols = ["tcp://", "ssl://", "mqtt://", "mqtts://"];
        return protocols.some(protocol => addr.startsWith(protocol));
    }

    /**
     * 获取配置值
     * @param {string} key - 配置项键名，格式如 "base.language"
     * @returns {*} 配置值
     */
    get(key) {
        const keys = key.split(".");
        let value = this.config;

        for (const k of keys) {
            if (value && typeof value === "object" && k in value) {
                value = value[k];
            } else {
                return undefined;
            }
        }

        return value;
    }

    /**
     * 获取默认配置值
     * @param {string} key - 配置项键名，格式如 "base.language"
     * @returns {*} 默认配置值
     */
    getDefaultValue(key) {
        const keys = key.split(".");
        let value = this.getDefaultConfig();

        for (const k of keys) {
            if (value && typeof value === "object" && k in value) {
                value = value[k];
            } else {
                return undefined;
            }
        }

        return value;
    }

    /**
     * 设置配置值
     * @param {string} key - 配置项键名
     * @param {*} value - 配置值
     * @returns {boolean} 设置是否成功
     */
    async set(key, value) {
        // 等待初始化完成
        if (!this.initialized) {
            console.log("等待配置管理器初始化完成...");
            while (!this.initialized) {
                await new Promise(resolve => setTimeout(resolve, 10));
            }
        }

        // 检查权限
        if (this.permissions.readonly.includes(key)) {
            console.warn(`配置项 ${key} 为只读，无法修改`);
            return false;
        }

        // 验证值
        if (this.validators[key]) {
            const validationResult = this.validators[key](value);
            let isValid = false;
            let errorMessage = '';

            if (typeof validationResult === 'object') {
                isValid = validationResult.valid;
                errorMessage = validationResult.message;
            } else if (typeof validationResult === 'boolean') {
                isValid = validationResult;
                errorMessage = `配置项 ${key} 的值 ${value} 不符合要求`;
            }

            if (!isValid) {
                // 检查是否与默认值相同
                const defaultValue = this.getDefaultValue(key);
                if (defaultValue !== undefined && JSON.stringify(value) === JSON.stringify(defaultValue)) {
                    console.log(`配置项 ${key} 验证失败但与默认值相同，允许设置: ${errorMessage}`);
                    // 允许设置，但记录警告
                } else {
                    console.error(`配置项 ${key} 验证失败: ${errorMessage}`);
                    return false;
                }
            }
        }

        // 获取旧值用于回调
        const oldValue = this.get(key);

        // 设置值
        const keys = key.split(".");
        let config = this.config;

        for (let i = 0; i < keys.length - 1; i++) {
            const k = keys[i];
            if (!(k in config)) {
                config[k] = {};
            }
            config = config[k];
        }

        config[keys[keys.length - 1]] = value;

        // 自动保存配置
        try {
            await this.saveToFile();

            // 配置保存成功后，异步执行回调（不阻塞主流程）
            this._executeCallbacks(key, oldValue, value);
        } catch (error) {
            console.warn("自动保存配置失败:", error);
            return false;
        }

        return true;
    }

    /**
     * 注册全局配置变更回调
     * @param {Function} callback - 异步回调函数，参数为 (key, oldValue, newValue, configManager)
     * @returns {Function} 取消注册的函数
     */
    onConfigChange(callback) {
        if (typeof callback !== 'function') {
            throw new Error('回调函数必须是函数类型');
        }

        this.callbacks.global.push(callback);

        // 返回取消注册的函数
        return () => {
            const index = this.callbacks.global.indexOf(callback);
            if (index > -1) {
                this.callbacks.global.splice(index, 1);
            }
        };
    }

    /**
     * 注册特定配置项变更回调
     * @param {string} key - 配置项键名
     * @param {Function} callback - 异步回调函数，参数为 (oldValue, newValue, configManager)
     * @returns {Function} 取消注册的函数
     */
    onConfigKeyChange(key, callback) {
        if (typeof callback !== 'function') {
            throw new Error('回调函数必须是函数类型');
        }

        if (!this.callbacks.specific.has(key)) {
            this.callbacks.specific.set(key, []);
        }

        this.callbacks.specific.get(key).push(callback);

        // 返回取消注册的函数
        return () => {
            const callbacks = this.callbacks.specific.get(key);
            if (callbacks) {
                const index = callbacks.indexOf(callback);
                if (index > -1) {
                    callbacks.splice(index, 1);
                }
                // 如果没有回调了，删除这个键
                if (callbacks.length === 0) {
                    this.callbacks.specific.delete(key);
                }
            }
        };
    }

    /**
     * 注册初始化完成回调
     * @param {Function} callback - 异步回调函数，参数为 (configManager)
     * @returns {Function} 取消注册的函数
     */
    onInit(callback) {
        if (typeof callback !== 'function') {
            throw new Error('回调函数必须是函数类型');
        }

        this.callbacks.init.push(callback);

        // 如果已经初始化完成，立即执行回调
        if (this.initialized) {
            this._safeExecuteCallback(callback, this);
        }

        // 返回取消注册的函数
        return () => {
            const index = this.callbacks.init.indexOf(callback);
            if (index > -1) {
                this.callbacks.init.splice(index, 1);
            }
        };
    }

    /**
     * 异步执行配置变更回调
     * @param {string} key - 配置项键名
     * @param {*} oldValue - 旧值
     * @param {*} newValue - 新值
     * @private
     */
    async _executeCallbacks(key, oldValue, newValue) {
        const callbackPromises = [];

        // 执行全局回调
        for (const callback of this.callbacks.global) {
            callbackPromises.push(
                this._safeExecuteCallback(callback, key, oldValue, newValue, this)
            );
        }

        // 执行特定配置项回调
        const specificCallbacks = this.callbacks.specific.get(key);
        if (specificCallbacks) {
            for (const callback of specificCallbacks) {
                callbackPromises.push(
                    this._safeExecuteCallback(callback, oldValue, newValue, this)
                );
            }
        }

        // 等待所有回调完成（不阻塞主流程）
        if (callbackPromises.length > 0) {
            Promise.allSettled(callbackPromises).then(results => {
                results.forEach((result, index) => {
                    if (result.status === 'rejected') {
                        console.warn(`配置变更回调执行失败 (${index}):`, result.reason);
                    }
                });
            });
        }
    }

    /**
     * 异步执行初始化完成回调
     * @private
     */
    async _executeInitCallbacks() {
        const callbackPromises = [];

        // 执行初始化回调
        for (const callback of this.callbacks.init) {
            callbackPromises.push(
                this._safeExecuteCallback(callback, this)
            );
        }

        // 等待所有回调完成（不阻塞主流程）
        if (callbackPromises.length > 0) {
            Promise.allSettled(callbackPromises).then(results => {
                results.forEach((result, index) => {
                    if (result.status === 'rejected') {
                        console.warn(`初始化回调执行失败 (${index}):`, result.reason);
                    }
                });
            });
        }
    }

    /**
     * 安全执行回调函数
     * @param {Function} callback - 回调函数
     * @param {...any} args - 回调参数
     * @returns {Promise} 回调执行结果
     * @private
     */
    async _safeExecuteCallback(callback, ...args) {
        try {
            const result = callback(...args);
            // 如果返回Promise，等待它完成
            if (result && typeof result.then === 'function') {
                return await result;
            }
            return result;
        } catch (error) {
            console.error('配置变更回调执行出错:', error);
            throw error;
        }
    }

    /**
     * 获取所有配置
     * @returns {Object} 完整配置对象
     */
    getAll() {
        return JSON.parse(JSON.stringify(this.config));
    }

    /**
     * 批量设置配置
     * @param {Object} configs - 配置对象
     * @returns {Object} 设置结果
     */
    async setBatch(configs) {
        const results = {};
        const changes = []; // 记录所有变更用于回调
        let hasChanges = false;

        // 等待初始化完成
        if (!this.initialized) {
            console.log("等待配置管理器初始化完成...");
            while (!this.initialized) {
                await new Promise(resolve => setTimeout(resolve, 10));
            }
        }

        // 批量设置配置（不触发保存）
        for (const [key, value] of Object.entries(configs)) {
            // 检查权限
            if (this.permissions.readonly.includes(key)) {
                console.warn(`配置项 ${key} 为只读，无法修改`);
                results[key] = false;
                continue;
            }

            // 验证值
            if (this.validators[key]) {
                const validationResult = this.validators[key](value);
                let isValid = false;
                let errorMessage = '';

                if (typeof validationResult === 'object') {
                    isValid = validationResult.valid;
                    errorMessage = validationResult.message;
                } else if (typeof validationResult === 'boolean') {
                    isValid = validationResult;
                    errorMessage = `配置项 ${key} 的值 ${value} 不符合要求`;
                }

                if (!isValid) {
                    // 检查是否与默认值相同
                    const defaultValue = this.getDefaultValue(key);
                    if (defaultValue !== undefined && JSON.stringify(value) === JSON.stringify(defaultValue)) {
                        console.log(`配置项 ${key} 验证失败但与默认值相同，允许设置: ${errorMessage}`);
                    } else {
                        console.error(`配置项 ${key} 验证失败: ${errorMessage}`);
                        results[key] = false;
                        continue;
                    }
                }
            }

            // 获取旧值
            const oldValue = this.get(key);

            // 设置值
            const keys = key.split(".");
            let config = this.config;

            for (let i = 0; i < keys.length - 1; i++) {
                const k = keys[i];
                if (!(k in config)) {
                    config[k] = {};
                }
                config = config[k];
            }

            config[keys[keys.length - 1]] = value;
            results[key] = true;
            hasChanges = true;

            // 记录变更用于回调
            changes.push({ key, oldValue, newValue: value });
        }

        // 如果有任何配置被成功修改，保存一次
        if (hasChanges) {
            try {
                await this.saveToFile();

                // 配置保存成功后，异步执行所有回调
                for (const change of changes) {
                    this._executeCallbacks(change.key, change.oldValue, change.newValue);
                }
            } catch (error) {
                console.warn("批量设置后保存配置失败:", error);
                return results;
            }
        }

        return results;
    }

    /**
     * 重置配置为默认值
     * @param {string} [group] - 可选，指定重置的配置组
     */
    reset(group = null) {
        if (group) {
            const defaultConfig = this.getDefaultConfig();
            if (defaultConfig[group]) {
                this.config[group] = JSON.parse(JSON.stringify(defaultConfig[group]));
            }
        } else {
            this.config = this.getDefaultConfig();
        }
    }

    /**
     * 获取配置项信息
     * @param {string} key - 配置项键名
     * @returns {Object} 配置项信息
     */
    getConfigInfo(key) {
        const info = {
            key: key,
            value: this.get(key),
            readonly: this.permissions.readonly.includes(key),
            writable: this.permissions.readwrite.includes(key),
            hasValidator: !!this.validators[key]
        };

        return info;
    }

    /**
     * 验证配置完整性
     * @returns {Object} 验证结果
     */
    validateAll() {
        const results = {
            valid: true,
            errors: []
        };

        for (const [key, validator] of Object.entries(this.validators)) {
            const value = this.get(key);
            if (value !== undefined) {
                const validationResult = validator(value);
                let isValid = false;
                let errorMessage = '';

                if (typeof validationResult === 'object') {
                    isValid = validationResult.valid;
                    errorMessage = validationResult.message;
                } else if (typeof validationResult === 'boolean') {
                    isValid = validationResult;
                    errorMessage = `配置项 ${key} 的值不符合要求`;
                }

                if (!isValid) {
                    // 检查是否与默认值相同
                    const defaultValue = this.getDefaultValue(key);
                    if (defaultValue !== undefined && JSON.stringify(value) === JSON.stringify(defaultValue)) {
                        // 与默认值相同，记录警告但不作为错误
                        console.log(`配置项 ${key} 验证失败但与默认值相同，允许: ${errorMessage}`);
                    } else {
                        results.valid = false;
                        results.errors.push({
                            key: key,
                            value: value,
                            message: errorMessage
                        });
                    }
                }
            }
        }

        return results;
    }

    /**
     * 确保配置目录存在
     * @returns {Promise<boolean>} 是否成功
     */
    async ensureConfigDirectory() {
        try {
            const dirPath = "/data/config";
            // 尝试创建目录（如果不存在）
            await tjs.mkdir(dirPath, { recursive: true });
            return true;
        } catch (error) {
            console.error("创建配置目录失败:", error);
            return false;
        }
    }

    /**
     * 从文件加载配置
     * @returns {Promise<boolean>} 是否成功加载
     */
    async loadFromFile() {
        try {
            // 确保配置目录存在
            await this.ensureConfigDirectory();

            const file = await tjs.open(this.configPath, 'r');
            const buf = new Uint8Array(8192);
            const nread = await file.read(buf);
            await file.close();

            if (nread > 0) {
                const content = new TextDecoder().decode(buf.subarray(0, nread));
                const loadedConfig = JSON.parse(content);

                // 验证加载的配置
                const tempConfig = this.config;
                this.config = loadedConfig;
                const validation = this.validateAll();

                if (validation.valid) {
                    console.log("成功加载持久化配置");
                    return true;
                } else {
                    console.warn("加载的配置验证失败，使用默认配置:", validation.errors);
                    this.config = tempConfig;
                    return false;
                }
            }
        } catch (error) {
            console.log("配置文件不存在或读取失败，使用默认配置创建新配置文件:", error.message);

            // 配置文件不存在，使用默认配置创建新文件
            try {
                await this.saveToFile();
                console.log("已使用默认配置创建新配置文件:", this.configPath);
                return true;
            } catch (saveError) {
                console.error("创建默认配置文件失败:", saveError);
                return false;
            }
        }
        return false;
    }

    /**
     * 保存配置到文件
     * @returns {Promise<boolean>} 是否成功保存
     */
    async saveToFile() {
        try {
            // 确保配置目录存在
            await this.ensureConfigDirectory();

            // 将配置转换为JSON字符串
            const configJson = JSON.stringify(this.config, null, 2);

            // 保存到文件
            await systemSaveToFile(this.configPath, configJson);
            console.log("配置已保存到:", this.configPath);
            return true;
        } catch (error) {
            console.error("保存配置失败:", error);
            return false;
        }
    }

    /**
     * 导出配置
     * @returns {string} JSON格式的配置字符串
     */
    export() {
        return JSON.stringify(this.config, null, 2);
    }

    /**
     * 导入配置
     * @param {string} configJson - JSON格式的配置字符串
     * @returns {boolean} 导入是否成功
     */
    import(configJson) {
        try {
            const importedConfig = JSON.parse(configJson);
            const validation = this.validateAll();

            if (validation.valid) {
                this.config = importedConfig;
                return true;
            } else {
                console.error("导入的配置验证失败:", validation.errors);
                return false;
            }
        } catch (error) {
            console.error("配置导入失败:", error);
            return false;
        }
    }
}

// 创建异步配置管理器实例
let configManager = null;

// 异步初始化配置管理器
async function initConfigManager() {
    if (!configManager) {
        configManager = await ConfigManager.create();
        onConfigKeyChangeService(onConfigChange, onConfigKeyChange)
    }
    return configManager;
}

// 导出配置管理器类
export { ConfigManager };

// 导出异步初始化函数
export { initConfigManager };

// 导出便捷方法（支持异步初始化）
export const get = async (key) => {
    const manager = await initConfigManager();
    return manager.get(key);
};
export const set = async (key, value) => {
    const manager = await initConfigManager();
    return await manager.set(key, value);
};
export const getAll = async () => {
    const manager = await initConfigManager();
    return manager.getAll();
};
export const setBatch = async (configs) => {
    const manager = await initConfigManager();
    return await manager.setBatch(configs);
};
export const reset = async (group) => {
    const manager = await initConfigManager();
    return manager.reset(group);
};
export const getConfigInfo = async (key) => {
    const manager = await initConfigManager();
    return manager.getConfigInfo(key);
};
export const validateAll = async () => {
    const manager = await initConfigManager();
    return manager.validateAll();
};
export const exportConfig = async () => {
    const manager = await initConfigManager();
    return manager.export();
};
export const importConfig = async (configJson) => {
    const manager = await initConfigManager();
    return manager.import(configJson);
};
export const loadFromFile = async () => {
    const manager = await initConfigManager();
    return await manager.loadFromFile();
};
export const saveToFile = async () => {
    const manager = await initConfigManager();
    return await manager.saveToFile();
};

// 导出回调方法
export const onConfigChange = async (callback) => {
    const manager = await initConfigManager();
    return manager.onConfigChange(callback);
};

export const onConfigKeyChange = async (key, callback) => {
    const manager = await initConfigManager();
    return manager.onConfigKeyChange(key, callback);
};

export const onInit = async (callback) => {
    const manager = await initConfigManager();
    return manager.onInit(callback);
};

// 默认导出异步初始化函数
export default initConfigManager;
