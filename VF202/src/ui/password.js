import { Window, View, Textarea, EAlignType, Keyboard } from "./const.js";
import { hide, show } from "./utils.js";
import { EVENTTYPE_MAP } from "./const.js";
import { face } from "dxDriver";
import { access } from "dxAccess";
import { accessAccess, accessFail } from "./result.js";
import path from "tjs:path";
import { setPasswordNow } from "./track.js";


let mask

let textarea
let keyboard

let resourcePath = path.join(import.meta.dirname, '/resource');

export function initPasswordPass() {
    mask = new View({ uid: "mask" });
    let style = {
        'width': Window.width,
        'height': Window.height,
        'padding-left': 0,
        'padding-right': 0,
        'padding-top': 0,
        'padding-bottom': 0,
        'border-width': 0,
        'border-radius': 0,
        'background-opacity': 125,
        'background-color': 0x00000000,
    }
    mask.nativeSetStyle(style, Object.keys(style), Object.keys(style).length, 0, true);
    hide(mask);
    mask.addEventListener(EVENTTYPE_MAP.EVENT_CLICKED);

    textarea = new Textarea({ uid: "textarea" });
    mask.appendChild(textarea);
    textarea.align(EAlignType.ALIGN_CENTER, [0, 0]);
    textarea.setPlaceHolder("请输入密码");
    textarea.setPasswordMode(true);
    textarea.setMaxLength(6);
    textarea.setOneLine(true);
    textarea.setAutoKeyboard(false);
    style = {
        'font-size-1': resourcePath + "/font/AlibabaPuHuiTi-3-65-Medium.ttf#20#0",
    }
    textarea.nativeSetStyle(style, Object.keys(style), Object.keys(style).length, 0, true);


    keyboard = new Keyboard({ uid: "keyboard" });
    style = {
        'width': Window.width,
        'border-width': 0,
        'height': Window.height / 3,
        'font-size': 8,
    }
    keyboard.nativeSetStyle(style, Object.keys(style), Object.keys(style).length, 0, true);
    keyboard.setMode(3);
    keyboard.setTextarea(textarea)
    mask.appendChild(keyboard);
    keyboard.addEventListener(EVENTTYPE_MAP.EVENT_CANCEL)
    keyboard.addEventListener(EVENTTYPE_MAP.EVENT_READY)



}


export function showPasswordPass(type) {
    face.setFacePause(true);
    textarea.setText("");

    if (type == "register") {
        textarea.setPlaceHolder("请输入姓名");
        keyboard.setMode(1);

    } else {
        textarea.setPlaceHolder("请输入密码");
        keyboard.setMode(3);
    }


    console.log("showPasswordPass");
    show(mask);
    mask.moveToFront();
}

export function hidePasswordPass() {
    console.log("hidePasswordPass");
    hide(mask);
    face.setFacePause(false);
}


export function confirmPassword() {
    setPasswordNow(true);
    let password = textarea.getText();
    const { success } = access.access(400, password);
    if (success) {
        accessAccess(400);
    } else {
        accessFail(400);
    }
    setTimeout(() => {
        setPasswordNow(false);
    }, 2000);
}

export function getText() {
    return textarea.getText();
}
