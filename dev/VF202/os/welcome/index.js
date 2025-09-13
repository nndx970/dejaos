let bridge = globalThis[Symbol.for("lvgljs")];
const { width, height } = bridge.NativeRender.dimensions.window;
var View = bridge.NativeRender.NativeComponents.View;
var Text = bridge.NativeRender.NativeComponents.Text;
var Button = bridge.NativeRender.NativeComponents.Button;


let welcome = new Text({ uid: "welcome" });
let style = {
    'text-color': 0xffffff,
    'font-size-1': 15
};
welcome.nativeSetStyle(style, Object.keys(style), Object.keys(style).length, 0, true)
welcome.align(9, [0, -100])
welcome.setText("Welcome to VF202")

let ip = new Text({ uid: "ip" });
style = {
    'text-color': 0xffffff,
    'font-size-1': 10
};
ip.nativeSetStyle(style, Object.keys(style), Object.keys(style).length, 0, true)
ip.align(9, [0, -50])
ip.setText("Waiting for IP: ...")


let countNum = 120;

let count = new Text({ uid: "count" });
style = {
    'text-color': 0xffffff,
    'font-size-1': 10
};
count.nativeSetStyle(style, Object.keys(style), Object.keys(style).length, 0, true)
count.align(3, [0, 0])
count.setText(countNum + "")

let btn = new Button({ uid: "btn" });
style = {
    'width': 150,
    'height': 60,
    'background-color': 0xffffff,
    'background-opacity': 125,
};
btn.nativeSetStyle(style, Object.keys(style), Object.keys(style).length, 0, true)
btn.align(9, [0, 20])
btn.addEventListener(7)


let selected = {
    'background-color': 0x95ec69,
}
let unselected = {
    'background-color': 0xffffff,
    'background-opacity': 125,
}





const encoder = new TextEncoder();
const decoder = new TextDecoder();

// 可以写字符换或二进制数据
async function saveToFile(filePath, content, append = false) {
    const file = await tjs.open(filePath, append ? 'a' : 'w');
    await file.write(encoder.encode(content));
    await file.datasync(); // 同步数据到磁盘
    await file.sync();     // 同步文件系统
    await file.close();
}



globalThis.FIRE_QEVENT_CALLBACK = async (targetUid, currentTargetUid, eventType, e, point) => {
    if (currentTargetUid == "btn" && eventType == 7) {
        tjs.exit(0)
    } else if (currentTargetUid == "devBtn" && eventType == 7) {
        console.log("devBtn clicked");
        devBtn.nativeSetStyle(selected, Object.keys(selected), Object.keys(selected).length, 0, true)
        prodBtn.nativeSetStyle(unselected, Object.keys(unselected), Object.keys(unselected).length, 0, true)
        saveToFile("/os/.mode", "development")
    } else if (currentTargetUid == "prodBtn" && eventType == 7) {
        console.log("prodBtn clicked");
        prodBtn.nativeSetStyle(selected, Object.keys(selected), Object.keys(selected).length, 0, true)
        devBtn.nativeSetStyle(unselected, Object.keys(unselected), Object.keys(unselected).length, 0, true)
        saveToFile("/os/.mode", "production")
    }
};

let exitLbl = new Text({ uid: "exitLbl" });
style = {
    'text-color': 0xffffff,
    'font-size-1': 10
};
exitLbl.nativeSetStyle(style, Object.keys(style), Object.keys(style).length, 0, true)
exitLbl.setText("Exit")
btn.appendChild(exitLbl)
exitLbl.align(9, [0, 0])



let devBtn = new Button({ uid: "devBtn" });
style = {
    'width': 220,
    'height': 60,
    'background-color': 0xffffff,
    'background-opacity': 125,
};
devBtn.nativeSetStyle(style, Object.keys(style), Object.keys(style).length, 0, true)
devBtn.align(9, [0, 100])
devBtn.addEventListener(7)


let devLbl = new Text({ uid: "devLbl" });
style = {
    'text-color': 0xffffff,
    'font-size-1': 10
};
devLbl.nativeSetStyle(style, Object.keys(style), Object.keys(style).length, 0, true)
devLbl.setText("Development")
devBtn.appendChild(devLbl)
devLbl.align(9, [0, 0])



let prodBtn = new Button({ uid: "prodBtn" });
style = {
    'width': 220,
    'height': 60,
    'background-color': 0xffffff,
    'background-opacity': 125,
};
prodBtn.nativeSetStyle(style, Object.keys(style), Object.keys(style).length, 0, true)
prodBtn.align(9, [0, 180])
prodBtn.addEventListener(7)


let prodLbl = new Text({ uid: "prodLbl" });
style = {
    'text-color': 0xffffff,
    'font-size-1': 10
};
prodLbl.nativeSetStyle(style, Object.keys(style), Object.keys(style).length, 0, true)
prodLbl.setText("Production")
prodBtn.appendChild(prodLbl)
prodLbl.align(9, [0, 0])




let ipDetected = false;
setInterval(async () => {
    countNum--
    if (countNum < 0) {
        tjs.exit(0)
        return
    }
    count.setText(countNum + "")
    if (ipDetected) {
        return
    }
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
                    ipDetected = true;
                    ip.setText("IP: " + output);
                    countNum = 5;
                    break;
                }
            }
        } catch (error) {
            console.log("stdout读取结束");
        }
    })();
}, 1000);