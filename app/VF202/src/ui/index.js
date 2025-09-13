import { trackInit } from "./track.js";
import { initMain } from "./main.js";
import { initResult } from "./result.js";
import { initPasswordPass } from "./password.js";
import { initRegister } from "./register.js";

export function uiInit() {
    // 初始化跟踪框
    trackInit();
    // 初始化主界面
    initMain();
    // 初始化结果界面
    initResult();
    // 密码通行
    initPasswordPass();
    // 初始化注册界面
    initRegister();

}


