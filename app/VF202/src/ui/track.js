import path from "tjs:path";
import { EAlignType, EVENTTYPE_MAP, View, Textarea, Text, Button, Image, Dropdownlist, Keyboard, Window } from "./const.js";
import { face } from "dxDriver";
const { getFaceRecognitionResult, getFaceTrackData } = face;
import { hide, show } from "./utils.js";
import { access, config } from "dxAccess";
import { accessAccess, accessFail } from "./result.js";

// 跟踪框白色
let trackWhiteImg
// 跟踪框绿色
let trackGreenImg
// 跟踪框红色
let trackRedImg
// 当前跟踪框
let trackBoxNow

let userNameText

let resourcePath = path.join(import.meta.dirname, '/resource');

// 当前识别状态
let statusNow = null;
let statusNowTimer = null;
// 当前识别姓名
let userNameNow = null;

// 注册状态
let registerNow = null;

// 密码校验状态
let passwordNow = null;

export function setRegisterNow(now) {
    registerNow = now;
}
export function setPasswordNow(now) {
    passwordNow = now;
}
let configManager = await config.initConfigManager();
setInterval(async () => {
    let trackData = getFaceTrackData();
    if (trackData) {
        // console.log(trackData);
        drawTrackBox(trackData.x1, trackData.y1, trackData.x2, trackData.y2);
    }
    let recognitionData = getFaceRecognitionResult();
    if (recognitionData) {
        // console.log(recognitionData);
        if (registerNow || passwordNow) {
            return;
        }
        if (recognitionData.score > 0.6) {
            userNameNow = recognitionData.userid;
        } else {
            return
        }
        if (statusNow !== null) {
            return;
        }

        let success = null;

        if (configManager.get('face.livenessOff') === 1) {
            if (!recognitionData.is_living) {
                success = false
                console.log(`[用户名验证失败] 用户名: ${recognitionData.userid}, 失败原因: 人脸识别失败，活体检测失败`);
            } else if (recognitionData.living_score < configManager.get('face.livenessVal')) {
                success = false
                console.log(`[用户名验证失败] 用户名: ${recognitionData.userid}, 失败原因: 人脸识别失败，活体检测分数小于${configManager.get('face.livenessVal')}`);
            }
        }
        if (success === null) {
            const result = access.accessByUserName(recognitionData.userid);
            success = result.success;
        }

        statusNow = success;
        if (success) {
            if (trackBoxNow !== trackGreenImg) {
                hide(trackBoxNow);
                trackBoxNow = trackGreenImg;
            }
            accessAccess(300);
        } else {
            if (trackBoxNow !== trackRedImg) {
                hide(trackBoxNow);
                trackBoxNow = trackRedImg;
            }
            accessFail(300);
        }

        statusNowTimer = setTimeout(() => {
            statusNow = null;
        }, 5000);
    }
}, 5);


let trackBoxTimer = null;
let picWidth = 367;
let picHeight = 373;
function drawTrackBox(x1, y1, x2, y2) {
    show(trackBoxNow)
    let width = x2 - x1;
    let height = y2 - y1;
    // 中心坐标对齐
    let centerX = x1 + width / 2 - picWidth / 2;
    let centerY = y1 + height / 2 - picHeight / 2 + 20;
    trackBoxNow.align(EAlignType.ALIGN_TOP_LEFT, [centerX, centerY]);
    trackBoxNow.nativeSetStyle({ 'img-scale': 256 * width / picHeight * 1.2 }, ['img-scale'], 1, 0, true)


    if (userNameNow) {
        userNameText.setText(userNameNow);
        show(userNameText);
        userNameText.alignTo(EAlignType.ALIGN_CENTER, [0, 0], trackBoxNow);
    }


    if (trackBoxTimer) {
        clearTimeout(trackBoxTimer);
        trackBoxTimer = null;
    }
    trackBoxTimer = setTimeout(() => {
        userNameNow = null;
        hide(trackBoxNow)
        trackBoxNow = trackWhiteImg;
        hide(userNameText);
        statusNow = null
        if (statusNowTimer) {
            clearTimeout(statusNowTimer);
            statusNowTimer = null;
        }
    }, 400);

}


export function trackInit() {
    trackWhiteImg = new Image({ uid: "trackWhiteImg" });
    tjs.readFile(resourcePath + '/image/track_white.png', { encoding: "binary" }).then((data) => {
        trackWhiteImg.setImageBinary(data.buffer);
    });
    hide(trackWhiteImg);
    trackGreenImg = new Image({ uid: "trackGreenImg" });
    tjs.readFile(resourcePath + '/image/track_green.png', { encoding: "binary" }).then((data) => {
        trackGreenImg.setImageBinary(data.buffer);
    });
    hide(trackGreenImg);
    trackRedImg = new Image({ uid: "trackRedImg" });
    tjs.readFile(resourcePath + '/image/track_red.png', { encoding: "binary" }).then((data) => {
        trackRedImg.setImageBinary(data.buffer);
    });
    hide(trackRedImg);


    userNameText = new Text({ uid: "userNameText" });
    let userNameTextStyle = {
        'font-size-1': resourcePath + "/font/AlibabaPuHuiTi-3-65-Medium.ttf#40#0",
        'text-color': 0xFFFFFFFF,
    }
    userNameText.nativeSetStyle(userNameTextStyle, Object.keys(userNameTextStyle), Object.keys(userNameTextStyle).length, 0, true)
    hide(userNameText);


    trackBoxNow = trackWhiteImg
}

