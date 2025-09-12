import path from "tjs:path";
import { EAlignType, EVENTTYPE_MAP, View, Textarea, Text, Button, Image, Dropdownlist, Keyboard, Window, ETextOverflow } from "./const.js";
import { hide, show } from "./utils.js";
import { mqtt, common, face } from "dxDriver";
import { showPasswordPass, hidePasswordPass, confirmPassword } from "./password.js";
import { confirmRegister } from "./register.js";

// 顶部图片
let topImg
// 底部图片
let bottomImg
// 密码图片
let passwordImg

// 时间文本
let timeText
let timeText1
let timeUpdateInterval

// 网络图标
let networkImg
let mqttImg

let qrcodeImg
let snText

let ipText


let resourcePath = path.join(import.meta.dirname, '/resource');

// 格式化时间显示
function formatTime() {
    const now = new Date();
    const hours = now.getHours().toString().padStart(2, '0');
    const minutes = now.getMinutes().toString().padStart(2, '0');
    const seconds = now.getSeconds().toString().padStart(2, '0');

    return {
        time: `${hours}:${minutes}`,
        seconds: `:${seconds}`
    };
}

// 更新时间显示
function updateTime() {
    const timeData = formatTime();
    if (timeText) {
        timeText.setText(timeData.time);
    }
    if (timeText1) {
        timeText1.setText(timeData.seconds);
    }
}

let flag = false;
let mode = "password";

export function initMain() {
    topImg = new Image({ uid: "topImg" });
    topImg.align(EAlignType.ALIGN_TOP_LEFT, [26, 12]);
    tjs.readFile(resourcePath + '/image/top.png', { encoding: "binary" }).then((data) => {
        topImg.setImageBinary(data.buffer);
    });
    bottomImg = new Image({ uid: "bottomImg" });
    bottomImg.align(EAlignType.ALIGN_BOTTOM_MID, [0, 0]);
    tjs.readFile(resourcePath + '/image/bottom.png', { encoding: "binary" }).then((data) => {
        bottomImg.setImageBinary(data.buffer);
    });
    passwordImg = new Image({ uid: "passwordImg" });
    passwordImg.align(EAlignType.ALIGN_TOP_LEFT, [202, 755]);
    tjs.readFile(resourcePath + '/image/lock_circle.png', { encoding: "binary" }).then((data) => {
        passwordImg.setImageBinary(data.buffer);
    });
    passwordImg.addEventListener(EVENTTYPE_MAP.EVENT_CLICKED);
    passwordImg.addEventListener(EVENTTYPE_MAP.EVENT_LONG_PRESSED);
    globalThis.FIRE_QEVENT_CALLBACK = (targetUid, currentTargetUid, eventType, e, point) => {
        if (currentTargetUid == "passwordImg" && eventType == EVENTTYPE_MAP.EVENT_CLICKED) {
            if (flag) {
                flag = false;
                return;
            }
            mode = "password";
            showPasswordPass();
            face.setFacePause(true);
        } else if (currentTargetUid == "passwordImg" && eventType == EVENTTYPE_MAP.EVENT_LONG_PRESSED) {
            mode = "register";
            showPasswordPass("register");
            flag = true;
        } else if (targetUid == "mask" && eventType == EVENTTYPE_MAP.EVENT_CLICKED) {
            hidePasswordPass();
        } else if (currentTargetUid == "keyboard" && eventType == EVENTTYPE_MAP.EVENT_CANCEL) {
            hidePasswordPass();
        } else if (currentTargetUid == "keyboard" && eventType == EVENTTYPE_MAP.EVENT_READY) {
            hidePasswordPass();
            if (mode == "password") {
                confirmPassword();
            } else {
                confirmRegister();
            }
        }
    }

    timeText = new Text({ uid: "timeText" });
    timeText.align(EAlignType.ALIGN_TOP_LEFT, [43, 17]);
    timeText.setText("00:00");
    let timeTextStyle = {
        'font-size-1': resourcePath + "/font/AlibabaPuHuiTi-3-65-Medium.ttf#25#0",
        'text-color': 0xFFFFFFFF
    }
    timeText.nativeSetStyle(timeTextStyle, Object.keys(timeTextStyle), Object.keys(timeTextStyle).length, 0, true)

    timeText1 = new Text({ uid: "timeText1" });
    timeText1.align(EAlignType.ALIGN_TOP_LEFT, [113, 22]);
    timeText1.setText(":00");
    let timeTextStyle1 = {
        'font-size-1': resourcePath + "/font/AlibabaPuHuiTi-3-65-Medium.ttf#19#0",
        'text-color': 0xFFFFFFFF
    }
    timeText1.nativeSetStyle(timeTextStyle1, Object.keys(timeTextStyle1), Object.keys(timeTextStyle1).length, 0, true)

    // 立即显示当前时间
    updateTime();

    // 设置定时器每秒更新时间
    timeUpdateInterval = setInterval(updateTime, 1000);

    networkImg = new Image({ uid: "networkImg" });
    networkImg.align(EAlignType.ALIGN_TOP_LEFT, [391, 27]);
    tjs.readFile(resourcePath + '/image/wifi.png', { encoding: "binary" }).then((data) => {
        networkImg.setImageBinary(data.buffer);
    });
    // hide(networkImg);
    mqttImg = new Image({ uid: "mqttImg" });
    mqttImg.align(EAlignType.ALIGN_TOP_LEFT, [422, 22]);
    tjs.readFile(resourcePath + '/image/mqtt_icon.png', { encoding: "binary" }).then((data) => {
        mqttImg.setImageBinary(data.buffer);
    });
    hide(mqttImg);

    mqtt.setStatusCallback((status) => {
        if (status === 2) {
            show(mqttImg);
        } else {
            hide(mqttImg);
        }
    });

    qrcodeImg = new Image({ uid: "qrcodeImg" });
    qrcodeImg.align(EAlignType.ALIGN_TOP_LEFT, [21, 819]);
    tjs.readFile(resourcePath + '/image/qrcode.png', { encoding: "binary" }).then((data) => {
        qrcodeImg.setImageBinary(data.buffer);
    });

    snText = new Text({ uid: "snText" });
    snText.align(EAlignType.ALIGN_TOP_LEFT, [61, 823]);
    snText.setText("SN: " + common.getUuid(19));
    let snTextStyle = {
        'font-size-1': resourcePath + "/font/AlibabaPuHuiTi-3-65-Medium.ttf#15#0",
        'text-color': 0xFFFFFFFF,
        'text-overflow': ETextOverflow.LV_LABEL_LONG_SCROLL_CIRCULAR,
        'width': 118
    }
    snText.nativeSetStyle(snTextStyle, Object.keys(snTextStyle), Object.keys(snTextStyle).length, 0, true)


    ipText = new Text({ uid: "ipText" });
    ipText.align(EAlignType.ALIGN_BOTTOM_RIGHT, [-20, -13]);
    let ipTextStyle = {
        'font-size-1': resourcePath + "/font/AlibabaPuHuiTi-3-65-Medium.ttf#15#0",
        'text-color': 0xFFFFFFFF,
    }
    ipText.nativeSetStyle(ipTextStyle, Object.keys(ipTextStyle), Object.keys(ipTextStyle).length, 0, true)

    let proc = tjs.spawn(['sh', '-c', `ifconfig | grep "inet " | grep -v "127.0.0.1" | awk '{print $2}' | head -1 | grep -oE '[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}'`], { stdout: 'pipe', stderr: 'pipe' })
    // 创建缓冲区用于读取输出
    const stdoutBuf = new Uint8Array(4096);
    const decoder = new TextDecoder();
    // 异步读取标准输出
    (async () => {
        try {
            while (true) {
                const nread = await proc.stdout.read(stdoutBuf);
                if (nread === null) break;
                const output = decoder.decode(stdoutBuf.subarray(0, nread)).trim();
                if (output) {
                    ipText.setText("IP: " + output);
                    break;
                }
            }
        } catch (error) {
            console.log("stdout读取结束");
        }
    })();
}

// 清理资源函数
export function cleanupMain() {
    if (timeUpdateInterval) {
        clearInterval(timeUpdateInterval);
        timeUpdateInterval = null;
    }
}
