import path from "tjs:path";
import { EAlignType, Image, Text } from "./const.js";
import { setRegisterNow } from "./track.js";
import { getText } from "./password.js";
import { hide, show } from "./utils.js";
import { face } from "dxDriver";
import { accessAccess, accessFail } from "./result.js";
import { access } from "dxAccess";


let bar
let tipIcon
let tipText

let resourcePath = path.join(import.meta.dirname, '/resource');

export function initRegister() {
    bar = new Image({ uid: "bar" });
    bar.align(EAlignType.ALIGN_TOP_MID, [0, 540]);
    tjs.readFile(resourcePath + '/image/tip.png', { encoding: "binary" }).then((data) => {
        bar.setImageBinary(data.buffer);
    });
    hide(bar);

    tipText = new Text({ uid: "tipText" });
    tipText.align(EAlignType.ALIGN_TOP_MID, [10, 612]);
    tipText.setText("请将面部对准屏幕 3 秒后注册人脸");
    let style = {
        'font-size-1': resourcePath + "/font/AlibabaPuHuiTi-3-65-Medium.ttf#25#0",
        'text-color': 0xFFFFFF
    }
    tipText.nativeSetStyle(style, Object.keys(style), Object.keys(style).length, 0, true);
    hide(tipText);


    tipIcon = new Image({ uid: "tipIcon" });
    tipIcon.align(EAlignType.ALIGN_TOP_LEFT, [23, 613]);
    tjs.readFile(resourcePath + '/image/face.png', { encoding: "binary" }).then((data) => {
        tipIcon.setImageBinary(data.buffer);
    });
    hide(tipIcon);

}


export function confirmRegister() {
    let userName = getText();
    setRegisterNow(true);

    show(tipText);
    show(tipIcon);
    show(bar);

    let count = 3;
    let countInterval = setInterval(() => {
        count--;
        console.log("count", count);

        tipText.setText("请将面部对准屏幕 " + count + " 秒后注册人脸");
        if (count <= 0) {
            count = 3
            tipText.setText("请将面部对准屏幕 " + count + " 秒后注册人脸");
            hide(tipText);
            hide(tipIcon);
            hide(bar);
            clearInterval(countInterval);

            face.faceRegister(userName).then((ret) => {
                let userId = null;
                let credentialId = null;
                if (ret == 0) {
                    // 注册成功后，创建用户和凭证
                    userId = access.db.createUser(userName);
                    console.log("用户创建成功，ID:", userId);
                    credentialId = access.db.createCredential(userId, 300, "");
                    console.log("人脸凭证创建成功，ID:", credentialId);
                }
                if (!userId || !credentialId) {
                    ret = -99;
                }
                if (ret == 0) {
                    accessAccess(300, "*****人脸注册成功*****")
                } else {
                    accessFail(300, "人脸注册失败，错误码：" + ret)
                }
                setTimeout(() => {
                    setRegisterNow(false);
                }, 2000);
            });
        }
    }, 1000);
}