import path from "tjs:path";
import { Image, EAlignType, Text, Window } from "./const.js";
import { hide, show } from "./utils.js";

let successSideImg
let failSideImg

let successBarImg
let failBarImg

let successCircleImg
let failCircleImg

let successMsg
let failMsg


let resourcePath = path.join(import.meta.dirname, '/resource');


let styleResult = {
    'position': 'absolute',
    'top': 0,
    'left': 0,
}

function initResult() {
    successSideImg = new Image({ uid: "successSideImg" });
    successSideImg.nativeSetStyle(styleResult, Object.keys(styleResult), Object.keys(styleResult).length, 0, true)
    tjs.readFile(resourcePath + '/image/success_side.png', { encoding: "binary" }).then((data) => {
        successSideImg.setImageBinary(data.buffer);
    });
    successSideImg.moveToBackground();
    hide(successSideImg);

    failSideImg = new Image({ uid: "failSideImg" });
    failSideImg.nativeSetStyle(styleResult, Object.keys(styleResult), Object.keys(styleResult).length, 0, true)
    tjs.readFile(resourcePath + '/image/fail_side.png', { encoding: "binary" }).then((data) => {
        failSideImg.setImageBinary(data.buffer);
    });
    failSideImg.moveToBackground();
    hide(failSideImg);

    successBarImg = new Image({ uid: "successBarImg" });
    successBarImg.align(EAlignType.ALIGN_TOP_MID, [0, 580]);
    tjs.readFile(resourcePath + '/image/success.png', { encoding: "binary" }).then((data) => {
        successBarImg.setImageBinary(data.buffer);
    });
    hide(successBarImg);

    failBarImg = new Image({ uid: "failBarImg" });
    failBarImg.align(EAlignType.ALIGN_TOP_MID, [0, 580]);
    tjs.readFile(resourcePath + '/image/fail.png', { encoding: "binary" }).then((data) => {
        failBarImg.setImageBinary(data.buffer);
    });
    hide(failBarImg);

    successCircleImg = new Image({ uid: "successCircleImg" });
    successCircleImg.align(EAlignType.ALIGN_TOP_LEFT, [47, 636]);
    tjs.readFile(resourcePath + '/image/success_circle.png', { encoding: "binary" }).then((data) => {
        successCircleImg.setImageBinary(data.buffer);
    });
    hide(successCircleImg);

    failCircleImg = new Image({ uid: "failCircleImg" });
    failCircleImg.align(EAlignType.ALIGN_TOP_LEFT, [47, 636]);
    tjs.readFile(resourcePath + '/image/fail_circle.png', { encoding: "binary" }).then((data) => {
        failCircleImg.setImageBinary(data.buffer);
    });

    hide(failCircleImg);

    let msgStyle = {
        'font-size-1': resourcePath + "/font/AlibabaPuHuiTi-3-65-Medium.ttf#25#0",
        'text-color': 0xFFFFFFFF,
    }

    successMsg = new Text({ uid: "successMsg" });
    successMsg.align(EAlignType.ALIGN_TOP_LEFT, [122, 643]);
    successMsg.setText("人脸识别成功，请通行！");
    successMsg.nativeSetStyle(msgStyle, Object.keys(msgStyle), Object.keys(msgStyle).length, 0, true)
    hide(successMsg);

    failMsg = new Text({ uid: "failMsg" });
    failMsg.align(EAlignType.ALIGN_TOP_LEFT, [122, 643]);
    failMsg.setText("人脸识别失败，请重试！");
    failMsg.nativeSetStyle(msgStyle, Object.keys(msgStyle), Object.keys(msgStyle).length, 0, true)
    hide(failMsg);
}

let accessAccessTimer = null;
function accessAccess(type, message) {
    switch (type) {
        case 300:
            successMsg.setText(message || "人脸识别成功，请通行！");
            break;
        case 400:
            successMsg.setText(message || "密码开门成功，请通行！");
            break;
        default:
            break;
    }
    show(successSideImg);
    show(successBarImg);
    show(successCircleImg);
    show(successMsg);
    if (accessAccessTimer) {
        clearTimeout(accessAccessTimer);
        accessAccessTimer = null;
    }
    accessAccessTimer = setTimeout(() => {
        hide(successSideImg);
        hide(successBarImg);
        hide(successCircleImg);
        hide(successMsg);
    }, 2000);
}

let accessFailTimer = null;
function accessFail(type, message) {
    switch (type) {
        case 300:
            failMsg.setText(message || "人脸识别失败，请重试！");
            break;
        case 400:
            failMsg.setText(message || "密码开门失败，请重试！");
            break;
        default:
            break;
    }
    show(failSideImg);
    show(failBarImg);
    show(failCircleImg);
    show(failMsg);
    if (accessFailTimer) {
        clearTimeout(accessFailTimer);
        accessFailTimer = null;
    }
    accessFailTimer = setTimeout(() => {
        hide(failSideImg);
        hide(failBarImg);
        hide(failCircleImg);
        hide(failMsg);
    }, 2000);
}


export { initResult, accessAccess, accessFail };
