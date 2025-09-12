import FFI from 'tjs:ffi';

let sopath = '/os/driver/';
sopath = sopath + './libmqtt_wrapper.so';
const mqttLib = new FFI.Lib(sopath);

const mqtt_create_client = new FFI.CFunction(mqttLib.symbol('mqtt_create_client'), FFI.types.sint, [FFI.types.string]);
const mqtt_destroy_client = new FFI.CFunction(mqttLib.symbol('mqtt_destroy_client'), FFI.types.void, []);

const mqtt_set_connection_params = new FFI.CFunction(mqttLib.symbol('mqtt_set_connection_params'), FFI.types.sint, [FFI.types.string, FFI.types.sint, FFI.types.sint, FFI.types.sint, FFI.types.string, FFI.types.string, FFI.types.string, FFI.types.string, FFI.types.sint, FFI.types.sint]);

const mqtt_connect = new FFI.CFunction(mqttLib.symbol('mqtt_connect'), FFI.types.sint, []);

const mqtt_reconnect = new FFI.CFunction(mqttLib.symbol('mqtt_reconnect'), FFI.types.sint, []);

const mqtt_get_status = new FFI.CFunction(mqttLib.symbol('mqtt_get_status'), FFI.types.sint, []);

const mqtt_subscribe1 = new FFI.CFunction(mqttLib.symbol('mqtt_subscribe'), FFI.types.sint, [FFI.types.string, FFI.types.sint]);

const mqtt_publish1 = new FFI.CFunction(mqttLib.symbol('mqtt_publish'), FFI.types.sint, [FFI.types.string, FFI.types.string, FFI.types.sint, FFI.types.sint, FFI.types.sint]);

const mqtt_message_t = new FFI.StructType([
    ['topic', FFI.types.string],      // char* topic
    ['payload', FFI.types.string],    // char* payload
    ['payload_len', FFI.types.sint],  // int payload_len
    ['qos', FFI.types.sint],         // int qos
    ['retained', FFI.types.sint]     // int retained
], 'mqtt_message_t');

const mqtt_receive_message = new FFI.CFunction(mqttLib.symbol('mqtt_receive_message'), FFI.types.sint, [new FFI.PointerType(mqtt_message_t, 1)]);


let callbacks = [];

let statusCheckInterval = null;
let statusCallbacks = [];
let onMessageCallback = null;

const statusMap = {
    'MQTT_STATUS_DISCONNECTED': 0,
    'MQTT_STATUS_CONNECTING': 1,
    'MQTT_STATUS_CONNECTED': 2,
    'MQTT_STATUS_ERROR': 3
};

function mqttInit(params) {
    mqttDeinit();
    // 处理连接请求
    const {
        clientId,
        addr,
        username = '',
        password = '',
        keepAliveInterval = 60,
        cleansession = 1,
        willTopic = '',
        willMessage = '',
        willQos = 1,
        willRetained = 0
    } = params;

    if (statusCheckInterval) {
        clearInterval(statusCheckInterval);
        statusCheckInterval = null;
    }

    if (!clientId || !addr) {
        console.log("1.❌ 创建客户端失败");
        return;
    }

    let index = addr.lastIndexOf(':');
    if (index < 0) {
        console.log("1.❌ 创建客户端失败");
        return;
    }
    let host = addr.substring(0, index);
    let port = addr.substring(index + 1);

    if (mqtt_create_client.call(clientId) === 0) {
        console.log("1.✅ 创建客户端成功");
    } else {
        console.log("1.❌ 创建客户端失败");
        mqtt_destroy_client.call();
        return;
    }

    if (mqtt_set_connection_params.call(host, port, keepAliveInterval, cleansession, username, password, willTopic, willMessage, willQos, willRetained) === 0) {
        console.log("2.✅ 设置连接参数成功");
    } else {
        console.log("2.❌ 设置连接参数失败");
        mqtt_destroy_client.call();
        return;
    }

    if (mqtt_connect.call() === 0) {
        console.log("3.✅ 连接请求成功");
    } else {
        console.log("3.❌ 连接请求失败");
        mqtt_destroy_client.call();
        return;
    }

    let lastStatus = -1;
    statusCheckInterval = setInterval(() => {
        let status = mqtt_get_status.call()
        if (status !== lastStatus) {
            lastStatus = status;
            switch (status) {
                case statusMap['MQTT_STATUS_DISCONNECTED']:
                    console.log("4.❌ 连接状态为断开");
                    mqtt_reconnect.call()
                    break;
                case statusMap['MQTT_STATUS_CONNECTING']:
                    console.log("4.⚠️ 连接状态为连接中");
                    break;
                case statusMap['MQTT_STATUS_CONNECTED']:
                    console.log("4.✅ 连接状态为连接成功");
                    callbacks.forEach(callback => {
                        if (callback.onConnectedCallback) {
                            callback.onConnectedCallback();
                        }
                    });
                    break;
                case statusMap['MQTT_STATUS_ERROR']:
                    console.log("4.❌ 连接状态为连接失败");
                    mqtt_reconnect.call()
                    break;
                default:
                    break;
            }
            // 调用所有状态回调
            statusCallbacks.forEach(callback => {
                if (callback) {
                    callback(status);
                }
            });
        }

        // 创建消息结构体实例（初始化为空值）
        const message = {
            topic: null,
            payload: null,
            payload_len: 0,
            qos: 0,
            retained: 0
        };
        const messageBuffer = mqtt_message_t.toBuffer(message);
        const messagePointer = FFI.Pointer.createRefFromBuf(mqtt_message_t, messageBuffer);
        const result = mqtt_receive_message.call(messagePointer);
        if (result > 0) {
            const receivedMessage = mqtt_message_t.fromBuffer(messageBuffer);
            if (onMessageCallback) {
                console.log('接收到MQTT消息:');
                console.log('  主题:', receivedMessage.topic);
                console.log('  内容:', receivedMessage.payload);
                onMessageCallback(receivedMessage.topic, receivedMessage.payload);
            }
        }
    }, 5);
}

function mqttDeinit() {
    mqtt_destroy_client.call();
}

function setConnectedCallback(callback) {
    callbacks.push({ onConnectedCallback: callback });
}

function setStatusCallback(callback) {
    if (callback && typeof callback === 'function') {
        statusCallbacks.push(callback);
    }
}


function setMessageCallback(callback) {
    onMessageCallback = callback;
}

function subscribe(topic) {
    console.log('订阅主题:', topic);
    mqtt_subscribe1.call(topic, 1);
}

function publish(topic, payload) {
    console.log('发布主题:', topic);
    console.log('发布内容:', payload);

    // 如果payload是对象，将其序列化为JSON字符串
    let payloadStr = payload;
    if (typeof payload === 'object' && payload !== null) {
        payloadStr = JSON.stringify(payload);
    } else if (payload === undefined || payload === null) {
        payloadStr = '';
    } else {
        payloadStr = String(payload);
    }

    // 计算UTF-8字节长度（中文字符在UTF-8中占用3个字节）
    // 使用TextEncoder来计算字节长度，兼容性更好
    let byteLength;
    try {
        // 优先使用TextEncoder（现代浏览器和Node.js支持）
        if (typeof TextEncoder !== 'undefined') {
            const encoder = new TextEncoder();
            byteLength = encoder.encode(payloadStr).length;
        } else if (typeof Buffer !== 'undefined') {
            // 回退到Buffer（Node.js环境）
            byteLength = Buffer.byteLength(payloadStr, 'utf8');
        } else {
            // 最后的回退方案：估算字节长度
            // 中文字符通常占用3个字节，英文字符占用1个字节
            byteLength = payloadStr.split('').reduce((length, char) => {
                const code = char.charCodeAt(0);
                return length + (code > 127 ? 3 : 1);
            }, 0);
        }
    } catch (error) {
        console.warn('计算字节长度失败，使用字符长度:', error);
        byteLength = payloadStr.length;
    }

    console.log('发布内容类型:', typeof payloadStr);
    console.log('发布内容长度（字符）:', payloadStr.length);
    console.log('发布内容长度（字节）:', byteLength);

    // 使用正确的payloadStr和字节长度
    mqtt_publish1.call(topic, payloadStr, byteLength, 1, 0);
}

export { mqttInit, mqttDeinit, setConnectedCallback, setStatusCallback, setMessageCallback, subscribe, publish };