import db from '../access/AccessControlDB.js';
import { initConfigManager, getAll } from '../config/index.js';
import { downloadFile } from '../utils/index.js';
import { mqtt, common } from 'dxDriver';

const { subscribe, publish, setConnectedCallback, setMessageCallback } = mqtt;
const { md5HashFile } = common;

/**
 * 初始化MQTT协议
 */
async function mqttAccessInit() {
    try {
        const configManager = await initConfigManager();
        const deviceUuid = configManager.get('sys.uuid');

        setConnectedCallback(async () => {
            // 订阅所有命令主题
            subscribe(`access_device/v2/cmd/${deviceUuid}/getConfig`);
            subscribe(`access_device/v2/cmd/${deviceUuid}/setConfig`);

            subscribe(`access_device/v2/cmd/${deviceUuid}/upgradeFirmware`);
            subscribe(`access_device/v2/cmd/${deviceUuid}/control`);

            subscribe(`access_device/v2/cmd/${deviceUuid}/insertUser`);
            subscribe(`access_device/v2/cmd/${deviceUuid}/delUser`);
            subscribe(`access_device/v2/cmd/${deviceUuid}/clearUser`);
            subscribe(`access_device/v2/cmd/${deviceUuid}/getUser`);

            subscribe(`access_device/v2/cmd/${deviceUuid}/insertKey`);
            subscribe(`access_device/v2/cmd/${deviceUuid}/delKey`);
            subscribe(`access_device/v2/cmd/${deviceUuid}/clearKey`);
            subscribe(`access_device/v2/cmd/${deviceUuid}/getKey`);

            subscribe(`access_device/v2/cmd/${deviceUuid}/insertPermission`);
            subscribe(`access_device/v2/cmd/${deviceUuid}/delPermission`);
            subscribe(`access_device/v2/cmd/${deviceUuid}/clearPermission`);
            subscribe(`access_device/v2/cmd/${deviceUuid}/getPermission`);

            // 连接上报：发送所有设备配置
            try {
                console.log('MQTT连接成功，开始上报设备配置');
                const allConfig = await getAll();
                const connectMessage = await createEventMessage(allConfig);

                publish('access_device/v2/event/connect', JSON.stringify(connectMessage));
                console.log('设备配置上报完成');
            } catch (error) {
                console.error('连接上报失败:', error);
            }
        });
        setMessageCallback((topic, payload) => {
            // 解析payload为JSON对象
            let messageData;
            try {
                messageData = JSON.parse(payload);
            } catch (error) {
                console.error('解析MQTT消息失败:', error);
                return;
            }

            // 根据主题路由到相应的处理函数
            if (topic.includes('/control')) {
                handleControl(messageData);
            } else if (topic.includes('/insertUser')) {
                handleInsertUser(messageData);
            } else if (topic.includes('/delUser')) {
                handleDelUser(messageData);
            } else if (topic.includes('/clearUser')) {
                handleClearUser(messageData);
            } else if (topic.includes('/getUser')) {
                handleGetUser(messageData);
            } else if (topic.includes('/getConfig')) {
                handleGetConfig(messageData);
            } else if (topic.includes('/setConfig')) {
                handleSetConfig(messageData);
            } else if (topic.includes('/insertKey')) {
                handleInsertKey(messageData);
            } else if (topic.includes('/delKey')) {
                handleDelKey(messageData);
            } else if (topic.includes('/clearKey')) {
                handleClearKey(messageData);
            } else if (topic.includes('/getKey')) {
                handleGetKey(messageData);
            } else if (topic.includes('/insertPermission')) {
                handleInsertPermission(messageData);
            } else if (topic.includes('/delPermission')) {
                handleDelPermission(messageData);
            } else if (topic.includes('/clearPermission')) {
                handleClearPermission(messageData);
            } else if (topic.includes('/getPermission')) {
                handleGetPermission(messageData);
            } else if (topic.includes('/upgradeFirmware')) {
                handleUpgradeFirmware(messageData);
            } else if (topic.includes('_reply')) {
                // 处理回复消息，用于客户端请求的响应
                handleReplyMessage(topic, messageData);
            }
        });
    } catch (error) {
        console.error('MQTT协议初始化失败:', error);
    }
}



// 存储等待回复的请求
const pendingRequests = new Map();

// 固件升级状态跟踪
let isUpgrading = false;

/**
 * 处理回复消息
 * @param {string} topic - 主题
 * @param {Object} messageData - 消息数据
 */
function handleReplyMessage(topic, messageData) {
    const { serialNo } = messageData;
    if (serialNo && pendingRequests.has(serialNo)) {
        const { resolve } = pendingRequests.get(serialNo);
        pendingRequests.delete(serialNo);
        resolve(messageData);
    }
}

/**
 * 生成响应消息
 * @param {string} serialNo - 序列号
 * @param {string} requestUuid - 请求中的UUID（用于验证，但不用于响应）
 * @param {string} code - 结果码
 * @param {any} data - 数据（成功时为实际数据，失败时为英文错误信息）
 * @returns {Promise<Object>} 响应消息对象
 */
async function createResponse(serialNo, requestUuid, code = '000000', data = null) {
    // 获取设备的实际UUID
    const configManager = await initConfigManager();
    const deviceUuid = configManager.get('sys.uuid');

    const response = {
        serialNo,
        uuid: deviceUuid, // 使用设备的实际UUID
        time: Math.floor(Date.now() / 1000),
        sign: '',
        code
    };

    if (data !== null) {
        response.data = data;
    }

    return response;
}

/**
 * 生成事件消息（主动上报）
 * @param {any} data - 数据内容
 * @returns {Promise<Object>} 事件消息对象
 */
async function createEventMessage(data = null) {
    // 获取设备的实际UUID
    const configManager = await initConfigManager();
    const deviceUuid = configManager.get('sys.uuid');

    // 生成序列号（使用时间戳+随机数）
    const serialNo = `${Math.floor(Date.now() / 1000)}${Math.floor(Math.random() * 1000).toString().padStart(3, '0')}`;

    const eventMessage = {
        serialNo,
        uuid: deviceUuid,
        time: Math.floor(Date.now() / 1000),
        sign: ''
    };

    if (data !== null) {
        eventMessage.data = data;
    }

    return eventMessage;
}

/**
 * 处理添加人员请求
 * @param {Object} payload - MQTT消息载荷
 */
async function handleInsertUser(payload) {
    try {
        console.log('收到添加人员请求:', payload);

        const { serialNo, uuid, data } = payload;

        if (!data || !Array.isArray(data)) {
            const response = await createResponse(serialNo, uuid, '100001', 'Invalid data format: data field must be an array');
            publish(`access_device/v2/cmd/insertUser_reply`, response);
            return;
        }

        const results = [];
        const errors = [];

        for (const userData of data) {
            try {
                const { id, name, extra } = userData;

                if (!name) {
                    errors.push(`User name cannot be empty`);
                    continue;
                }

                // 创建用户
                const userId = db.createUser(name, { id, extra });
                results.push({ id: userId, name, status: 'success' });

            } catch (error) {
                console.error('添加用户失败:', error);
                errors.push(error.message || 'Failed to add user');
            }
        }

        if (errors.length > 0) {
            const response = await createResponse(serialNo, uuid, '100001', `Some users failed to add: ${errors.join('; ')}`);
            publish(`access_device/v2/cmd/insertUser_reply`, response);
        } else {
            const response = await createResponse(serialNo, uuid);
            publish(`access_device/v2/cmd/insertUser_reply`, response);
        }

    } catch (error) {
        console.error('处理添加人员请求失败:', error);
        const response = await createResponse(payload.serialNo, payload.uuid, '100000', `Internal server error: ${error.message}`);
        publish(`access_device/v2/cmd/insertUser_reply`, response);
    }
}

/**
 * 处理删除人员请求
 * @param {Object} payload - MQTT消息载荷
 */
async function handleDelUser(payload) {
    try {
        console.log('收到删除人员请求:', payload);

        const { serialNo, uuid, data } = payload;

        if (!data || !Array.isArray(data)) {
            const response = await createResponse(serialNo, uuid, '100001', 'Invalid data format: data field must be an array');
            publish(`access_device/v2/cmd/delUser_reply`, response);
            return;
        }

        const results = [];
        const errors = [];

        for (const userId of data) {
            try {
                const success = db.deleteUser(userId);
                if (success) {
                    results.push({ id: userId, status: 'deleted' });
                } else {
                    errors.push(`User ID ${userId} does not exist or deletion failed`);
                }
            } catch (error) {
                console.error('删除用户失败:', error);
                errors.push(`Failed to delete user ${userId}: ${error.message}`);
            }
        }

        if (errors.length > 0) {
            const response = await createResponse(serialNo, uuid, '100001', `Some users failed to delete: ${errors.join('; ')}`);
            publish(`access_device/v2/cmd/delUser_reply`, response);
        } else {
            const response = await createResponse(serialNo, uuid);
            publish(`access_device/v2/cmd/delUser_reply`, response);
        }

    } catch (error) {
        console.error('处理删除人员请求失败:', error);
        const response = await createResponse(payload.serialNo, payload.uuid, '100000', `Internal server error: ${error.message}`);
        publish(`access_device/v2/cmd/delUser_reply`, response);
    }
}

/**
 * 处理清空人员请求
 * @param {Object} payload - MQTT消息载荷
 */
async function handleClearUser(payload) {
    try {
        console.log('收到清空人员请求:', payload);

        const { serialNo, uuid } = payload;

        // 获取所有用户
        const allUsers = db.getAllUsers();

        if (allUsers.length === 0) {
            const response = await createResponse(serialNo, uuid);
            publish(`access_device/v2/cmd/clearUser_reply`, response);
            return;
        }

        const errors = [];

        for (const user of allUsers) {
            try {
                db.deleteUser(user.id);
            } catch (error) {
                console.error('删除用户失败:', error);
                errors.push(`Failed to delete user ${user.id}: ${error.message}`);
            }
        }

        if (errors.length > 0) {
            const response = await createResponse(serialNo, uuid, '100001', `Some users failed to clear: ${errors.join('; ')}`);
            publish(`access_device/v2/cmd/clearUser_reply`, response);
        } else {
            const response = await createResponse(serialNo, uuid);
            publish(`access_device/v2/cmd/clearUser_reply`, response);
        }

    } catch (error) {
        console.error('处理清空人员请求失败:', error);
        const response = await createResponse(payload.serialNo, payload.uuid, '100000', `Internal server error: ${error.message}`);
        publish(`access_device/v2/cmd/clearUser_reply`, response);
    }
}

/**
 * 处理查询人员请求
 * @param {Object} payload - MQTT消息载荷
 */
async function handleGetUser(payload) {
    try {
        console.log('收到查询人员请求:', payload);

        const { serialNo, uuid, data } = payload;

        if (!data || typeof data !== 'object') {
            const response = await createResponse(serialNo, uuid, '100001', 'Invalid data format: data field must be an object');
            publish(`access_device/v2/cmd/getUser_reply`, response);
            return;
        }

        const { id, name, page = 0, size = 20 } = data;

        if (size <= 0 || size > 100) {
            const response = await createResponse(serialNo, uuid, '100001', 'Invalid parameter: size must be between 1 and 100');
            publish(`access_device/v2/cmd/getUser_reply`, response);
            return;
        }

        let users = [];
        let total = 0;

        if (id) {
            // 根据ID查询单个用户
            const user = db.getUserById(id);
            if (user) {
                users = [user];
                total = 1;
            }
        } else if (name) {
            // 根据姓名模糊查询
            const result = db.getUsersByNamePaginated({
                page: page + 1, // 转换为1开始的页码
                pageSize: size,
                name
            });
            users = result.data;
            total = result.pagination.total;
        } else {
            // 查询所有用户（分页）
            const result = db.getUsersPaginated({
                page: page + 1, // 转换为1开始的页码
                pageSize: size
            });
            users = result.data;
            total = result.pagination.total;
        }

        // 格式化返回数据
        const content = users.map(user => ({
            id: user.id,
            name: user.name,
            extra: user.extra
        }));

        const responseData = {
            page,
            size,
            total,
            totalPage: Math.ceil(total / size),
            count: content.length,
            content
        };

        const response = await createResponse(serialNo, uuid, '000000', responseData);
        publish(`access_device/v2/cmd/getUser_reply`, response);

    } catch (error) {
        console.error('处理查询人员请求失败:', error);
        const response = await createResponse(payload.serialNo, payload.uuid, '100000', `Internal server error: ${error.message}`);
        publish(`access_device/v2/cmd/getUser_reply`, response);
    }
}


/**
 * 处理配置查询请求
 * @param {Object} payload - MQTT消息载荷
 */
async function handleGetConfig(payload) {
    try {
        console.log('收到配置查询请求:', payload);

        const { serialNo, uuid, data } = payload;

        if (!serialNo || !uuid) {
            const response = await createResponse(serialNo, uuid, '100000', 'Parameter error: missing required parameters');
            publish(`access_device/v2/cmd/getConfig_reply`, response);
            return;
        }

        let configData = null;
        let errorMessage = null;

        try {
            const configManager = await initConfigManager();

            if (!data || data === '') {
                // 查询所有配置
                configData = configManager.getAll();
            } else if (typeof data === 'string') {
                if (data.includes('.')) {
                    // 查询单个配置项，如 "sys.uuid"
                    const value = configManager.get(data);
                    if (value !== undefined) {
                        const keys = data.split('.');
                        configData = {};
                        let current = configData;
                        for (let i = 0; i < keys.length - 1; i++) {
                            current[keys[i]] = {};
                            current = current[keys[i]];
                        }
                        current[keys[keys.length - 1]] = value;
                    } else {
                        errorMessage = `Configuration item ${data} does not exist`;
                    }
                } else {
                    // 查询配置组，如 "sys"
                    const allConfig = configManager.getAll();
                    if (allConfig[data]) {
                        configData = { [data]: allConfig[data] };
                    } else {
                        errorMessage = `Configuration group ${data} does not exist`;
                    }
                }
            } else {
                errorMessage = 'Invalid configuration query parameter format';
            }
        } catch (error) {
            errorMessage = `Configuration query failed: ${error.message}`;
        }

        if (errorMessage) {
            const response = await createResponse(serialNo, uuid, '100000', errorMessage);
            publish(`access_device/v2/cmd/getConfig_reply`, response);
        } else {
            const response = await createResponse(serialNo, uuid, '000000', configData);
            publish(`access_device/v2/cmd/getConfig_reply`, response);
        }
    } catch (error) {
        console.error('处理配置查询请求失败:', error);
        const response = await createResponse(payload.serialNo, payload.uuid, '100000', `Internal server error: ${error.message}`);
        publish(`access_device/v2/cmd/getConfig_reply`, response);
    }
}

/**
 * 处理配置修改请求
 * @param {Object} payload - MQTT消息载荷
 */
async function handleSetConfig(payload) {
    try {
        console.log('收到配置修改请求:', payload);

        const { serialNo, uuid, data } = payload;

        if (!serialNo || !uuid) {
            const response = await createResponse(serialNo, uuid, '100000', 'Parameter error: missing required parameters');
            publish(`access_device/v2/cmd/setConfig_reply`, response);
            return;
        }

        if (!data || typeof data !== 'object') {
            const response = await createResponse(serialNo, uuid, '100000', 'Parameter error: configuration data format is invalid');
            publish(`access_device/v2/cmd/setConfig_reply`, response);
            return;
        }

        // 将嵌套配置对象转换为扁平化键值对
        const flattenedData = flattenConfigObject(data);
        console.log('扁平化后的配置数据:', flattenedData);

        // 获取配置管理器实例
        const configManager = await initConfigManager();

        // 批量设置配置
        const setResults = await configManager.setBatch(flattenedData);
        console.log('配置设置结果:', setResults);

        // 检查设置结果
        const failedItems = [];
        const readonlyItems = [];

        for (const [key, success] of Object.entries(setResults)) {
            console.log(`配置项 ${key} 设置结果: ${success}`);
            if (!success) {
                const configInfo = configManager.getConfigInfo(key);
                console.log(`配置项 ${key} 信息:`, configInfo);
                if (configInfo.readonly) {
                    readonlyItems.push(key);
                } else {
                    failedItems.push(key);
                }
            }
        }

        if (failedItems.length > 0) {
            const response = await createResponse(serialNo, uuid, '100000', `Configuration items failed to set: ${failedItems.join(', ')}`);
            publish(`access_device/v2/cmd/setConfig_reply`, response);
        } else if (readonlyItems.length > 0) {
            const response = await createResponse(serialNo, uuid, '100001', `Configuration items are read-only and cannot be modified: ${readonlyItems.join(', ')}`);
            publish(`access_device/v2/cmd/setConfig_reply`, response);
        } else {
            const response = await createResponse(serialNo, uuid, '000000');
            publish(`access_device/v2/cmd/setConfig_reply`, response);
        }
    } catch (error) {
        console.error('处理配置修改请求失败:', error);
        const response = await createResponse(payload.serialNo, payload.uuid, '100000', `Internal server error: ${error.message}`);
        publish(`access_device/v2/cmd/setConfig_reply`, response);
    }
}


/**
 * 将嵌套配置对象转换为扁平化键值对
 * @param {Object} obj - 嵌套配置对象
 * @param {string} prefix - 键名前缀
 * @returns {Object} 扁平化的键值对对象
 */
function flattenConfigObject(obj, prefix = '') {
    const flattened = {};

    for (const [key, value] of Object.entries(obj)) {
        const newKey = prefix ? `${prefix}.${key}` : key;

        if (typeof value === 'object' && value !== null && !Array.isArray(value)) {
            // 递归处理嵌套对象
            Object.assign(flattened, flattenConfigObject(value, newKey));
        } else {
            // 直接赋值
            flattened[newKey] = value;
        }
    }

    return flattened;
}

/**
 * 处理远程控制请求
 * @param {Object} payload - MQTT消息载荷
 */
async function handleControl(payload) {
    try {
        console.log('收到远程控制请求:', payload);

        const { serialNo, uuid, data } = payload;

        if (!serialNo || !uuid) {
            const response = await createResponse(serialNo, uuid, '100000', 'Parameter error: missing required parameters');
            publish(`access_device/v2/cmd/control_reply`, response);
            return;
        }

        if (!data || typeof data !== 'object') {
            const response = await createResponse(serialNo, uuid, '100000', 'Parameter error: control data format is invalid');
            publish(`access_device/v2/cmd/control_reply`, response);
            return;
        }

        const { command, extra = {} } = data;

        if (typeof command !== 'number' || ![0, 1, 4, 5, 6, 7].includes(command)) {
            const response = await createResponse(serialNo, uuid, '100000', 'Invalid command: command must be 0, 1, 4, 5, 6, or 7');
            publish(`access_device/v2/cmd/control_reply`, response);
            return;
        }

        let result = false;
        let errorMessage = '';

        try {
            switch (command) {
                case 0: // 重启
                    result = await executeRestart();
                    break;
                case 1: // 远程开门
                    result = await executeRemoteOpen();
                    break;
                case 4: // 设备重置
                    result = await executeDeviceReset();
                    break;
                case 5: // 播放语音
                    result = await executePlayAudio(extra.wav);
                    break;
                case 6: // 屏幕展示图片
                    result = await executeShowImage(extra.image, extra.imageTimeout);
                    break;
                case 7: // 屏幕展示文字
                    result = await executeShowText(extra.txt, extra.txtTimeout);
                    break;
                default:
                    errorMessage = 'Unknown command';
                    break;
            }
        } catch (error) {
            console.error(`执行控制命令 ${command} 失败:`, error);
            errorMessage = error.message || 'Command execution failed';
        }

        if (result) {
            const response = await createResponse(serialNo, uuid, '000000');
            publish(`access_device/v2/cmd/control_reply`, response);
        } else {
            const response = await createResponse(serialNo, uuid, '100000', errorMessage || 'Command execution failed');
            publish(`access_device/v2/cmd/control_reply`, response);
        }

    } catch (error) {
        console.error('处理远程控制请求失败:', error);
        const response = await createResponse(payload.serialNo, payload.uuid, '100000', `Internal server error: ${error.message}`);
        publish(`access_device/v2/cmd/control_reply`, response);
    }
}

/**
 * 执行重启命令
 * @returns {Promise<boolean>} 执行结果
 */
async function executeRestart() {
    console.log('执行重启命令...');
    // TODO: 实现重启逻辑
    // 这里可以调用系统重启接口
    // 例如: await systemManager.restart();
    return true; // 预留位置，返回成功
}

/**
 * 执行远程开门命令
 * @returns {Promise<boolean>} 执行结果
 */
async function executeRemoteOpen() {
    console.log('执行远程开门命令...');
    // TODO: 实现远程开门逻辑
    // 这里可以调用门禁控制接口
    // 例如: await accessControl.openDoor();
    return true; // 预留位置，返回成功
}

/**
 * 执行设备重置命令
 * @returns {Promise<boolean>} 执行结果
 */
async function executeDeviceReset() {
    console.log('执行设备重置命令...');
    // TODO: 实现设备重置逻辑
    // 这里可以调用设备重置接口
    // 例如: await systemManager.reset();
    return true; // 预留位置，返回成功
}

/**
 * 执行播放语音命令
 * @param {string} wav - 音频资源名称
 * @returns {Promise<boolean>} 执行结果
 */
async function executePlayAudio(wav) {
    console.log('执行播放语音命令:', wav);

    if (!wav || typeof wav !== 'string') {
        throw new Error('Invalid audio resource name');
    }

    if (wav.length > 64) {
        throw new Error('Audio resource name too long (max 64 characters)');
    }

    // TODO: 实现播放语音逻辑
    // 这里可以调用音频播放接口
    // 例如: await audioManager.play(wav);
    return true; // 预留位置，返回成功
}

/**
 * 执行屏幕展示图片命令
 * @param {string} image - 图片资源名称
 * @param {number} timeout - 显示时间（秒）
 * @returns {Promise<boolean>} 执行结果
 */
async function executeShowImage(image, timeout) {
    console.log('执行屏幕展示图片命令:', image, 'timeout:', timeout);

    if (!image || typeof image !== 'string') {
        throw new Error('Invalid image resource name');
    }

    if (image.length > 64) {
        throw new Error('Image resource name too long (max 64 characters)');
    }

    if (timeout !== undefined && (typeof timeout !== 'number' || timeout < 0 || timeout > 86400)) {
        throw new Error('Invalid timeout value (must be 0-86400 seconds)');
    }

    // TODO: 实现屏幕展示图片逻辑
    // 这里可以调用显示接口
    // 例如: await displayManager.showImage(image, timeout);
    return true; // 预留位置，返回成功
}

/**
 * 执行屏幕展示文字命令
 * @param {string} txt - 文字内容
 * @param {number} timeout - 显示时间（秒）
 * @returns {Promise<boolean>} 执行结果
 */
async function executeShowText(txt, timeout) {
    console.log('执行屏幕展示文字命令:', txt, 'timeout:', timeout);

    if (!txt || typeof txt !== 'string') {
        throw new Error('Invalid text content');
    }

    if (txt.length > 64) {
        throw new Error('Text content too long (max 64 characters)');
    }

    if (timeout !== undefined && (typeof timeout !== 'number' || timeout < 0 || timeout > 86400)) {
        throw new Error('Invalid timeout value (must be 0-86400 seconds)');
    }

    // TODO: 实现屏幕展示文字逻辑
    // 这里可以调用显示接口
    // 例如: await displayManager.showText(txt, timeout);
    return true; // 预留位置，返回成功
}

/**
 * 处理添加凭证请求
 * @param {Object} payload - MQTT消息载荷
 */
async function handleInsertKey(payload) {
    try {
        console.log('收到添加凭证请求:', payload);

        const { serialNo, uuid, data } = payload;

        if (!serialNo || !uuid) {
            const response = await createResponse(serialNo, uuid, '100000', 'Parameter error: missing required parameters');
            publish(`access_device/v2/cmd/insertKey_reply`, response);
            return;
        }

        if (!data || !Array.isArray(data)) {
            const response = await createResponse(serialNo, uuid, '100001', 'Invalid data format: data field must be an array');
            publish(`access_device/v2/cmd/insertKey_reply`, response);
            return;
        }

        if (data.length > 100) {
            const response = await createResponse(serialNo, uuid, '100001', 'Too many credentials: maximum 100 per request');
            publish(`access_device/v2/cmd/insertKey_reply`, response);
            return;
        }

        const results = [];
        const errors = [];

        for (const keyData of data) {
            try {
                const { keyId, userId, type, code, extra } = keyData;

                // 验证必要字段
                if (!keyId || !userId || !type || !code) {
                    errors.push(`Missing required fields: keyId, userId, type, code`);
                    continue;
                }

                // 验证字段长度
                if (keyId.length < 6 || keyId.length > 128) {
                    errors.push(`Invalid keyId length: must be 6-128 characters`);
                    continue;
                }

                if (userId.length < 6 || userId.length > 128) {
                    errors.push(`Invalid userId length: must be 6-128 characters`);
                    continue;
                }

                if (code.length > 2048) {
                    errors.push(`Invalid code length: must be 0-2048 characters`);
                    continue;
                }

                // TODO: 实现添加凭证逻辑
                // 这里可以调用凭证管理接口
                // 例如: await credentialManager.addCredential(keyId, userId, type, code, extra);

                console.log(`添加凭证: keyId=${keyId}, userId=${userId}, type=${type}`);
                results.push({ keyId, userId, status: 'success' });

            } catch (error) {
                console.error('添加凭证失败:', error);
                errors.push(`Failed to add credential ${keyData.keyId}: ${error.message}`);
            }
        }

        if (errors.length > 0) {
            const response = await createResponse(serialNo, uuid, '100001', `Some credentials failed to add: ${errors.join('; ')}`);
            publish(`access_device/v2/cmd/insertKey_reply`, response);
        } else {
            const response = await createResponse(serialNo, uuid, '000000');
            publish(`access_device/v2/cmd/insertKey_reply`, response);
        }

    } catch (error) {
        console.error('处理添加凭证请求失败:', error);
        const response = await createResponse(payload.serialNo, payload.uuid, '100000', `Internal server error: ${error.message}`);
        publish(`access_device/v2/cmd/insertKey_reply`, response);
    }
}

/**
 * 处理删除凭证请求
 * @param {Object} payload - MQTT消息载荷
 */
async function handleDelKey(payload) {
    try {
        console.log('收到删除凭证请求:', payload);

        const { serialNo, uuid, data } = payload;

        if (!serialNo || !uuid) {
            const response = await createResponse(serialNo, uuid, '100000', 'Parameter error: missing required parameters');
            publish(`access_device/v2/cmd/delKey_reply`, response);
            return;
        }

        if (!data || typeof data !== 'object') {
            const response = await createResponse(serialNo, uuid, '100000', 'Parameter error: data field must be an object');
            publish(`access_device/v2/cmd/delKey_reply`, response);
            return;
        }

        const { keyIds = [], userIds = [] } = data;

        if (!Array.isArray(keyIds) || !Array.isArray(userIds)) {
            const response = await createResponse(serialNo, uuid, '100000', 'Invalid parameter: keyIds and userIds must be arrays');
            publish(`access_device/v2/cmd/delKey_reply`, response);
            return;
        }

        if (keyIds.length === 0 && userIds.length === 0) {
            const response = await createResponse(serialNo, uuid, '100000', 'Invalid parameter: at least one of keyIds or userIds must be provided');
            publish(`access_device/v2/cmd/delKey_reply`, response);
            return;
        }

        const results = [];
        const errors = [];

        // 删除指定凭证ID
        for (const keyId of keyIds) {
            try {
                // TODO: 实现删除凭证逻辑
                // 例如: await credentialManager.deleteCredential(keyId);
                console.log(`删除凭证: keyId=${keyId}`);
                results.push({ keyId, status: 'deleted' });
            } catch (error) {
                console.error('删除凭证失败:', error);
                errors.push(`Failed to delete credential ${keyId}: ${error.message}`);
            }
        }

        // 删除指定用户的所有凭证
        for (const userId of userIds) {
            try {
                // TODO: 实现删除用户所有凭证逻辑
                // 例如: await credentialManager.deleteUserCredentials(userId);
                console.log(`删除用户所有凭证: userId=${userId}`);
                results.push({ userId, status: 'deleted' });
            } catch (error) {
                console.error('删除用户凭证失败:', error);
                errors.push(`Failed to delete credentials for user ${userId}: ${error.message}`);
            }
        }

        if (errors.length > 0) {
            const response = await createResponse(serialNo, uuid, '100001', `Some credentials failed to delete: ${errors.join('; ')}`);
            publish(`access_device/v2/cmd/delKey_reply`, response);
        } else {
            const response = await createResponse(serialNo, uuid, '000000');
            publish(`access_device/v2/cmd/delKey_reply`, response);
        }

    } catch (error) {
        console.error('处理删除凭证请求失败:', error);
        const response = await createResponse(payload.serialNo, payload.uuid, '100000', `Internal server error: ${error.message}`);
        publish(`access_device/v2/cmd/delKey_reply`, response);
    }
}

/**
 * 处理清空凭证请求
 * @param {Object} payload - MQTT消息载荷
 */
async function handleClearKey(payload) {
    try {
        console.log('收到清空凭证请求:', payload);

        const { serialNo, uuid } = payload;

        if (!serialNo || !uuid) {
            const response = await createResponse(serialNo, uuid, '100000', 'Parameter error: missing required parameters');
            publish(`access_device/v2/cmd/clearKey_reply`, response);
            return;
        }

        // TODO: 实现清空所有凭证逻辑
        // 例如: await credentialManager.clearAllCredentials();
        console.log('清空所有凭证');

        const response = await createResponse(serialNo, uuid, '000000');
        publish(`access_device/v2/cmd/clearKey_reply`, response);

    } catch (error) {
        console.error('处理清空凭证请求失败:', error);
        const response = await createResponse(payload.serialNo, payload.uuid, '100000', `Internal server error: ${error.message}`);
        publish(`access_device/v2/cmd/clearKey_reply`, response);
    }
}

/**
 * 处理查询凭证请求
 * @param {Object} payload - MQTT消息载荷
 */
async function handleGetKey(payload) {
    try {
        console.log('收到查询凭证请求:', payload);

        const { serialNo, uuid, data } = payload;

        if (!serialNo || !uuid) {
            const response = await createResponse(serialNo, uuid, '100000', 'Parameter error: missing required parameters');
            publish(`access_device/v2/cmd/getKey_reply`, response);
            return;
        }

        if (!data || typeof data !== 'object') {
            const response = await createResponse(serialNo, uuid, '100000', 'Parameter error: data field must be an object');
            publish(`access_device/v2/cmd/getKey_reply`, response);
            return;
        }

        const { keyId, userId, type, page = 0, size = 20 } = data;

        if (size <= 0 || size > 100) {
            const response = await createResponse(serialNo, uuid, '100001', 'Invalid parameter: size must be between 1 and 100');
            publish(`access_device/v2/cmd/getKey_reply`, response);
            return;
        }

        // 实现查询凭证逻辑
        try {
            // 构建查询条件
            const queryOptions = {
                page: page + 1, // 数据库分页从1开始，前端从0开始
                pageSize: size,
                orderBy: 'created_at',
                order: 'DESC'
            };

            // 添加筛选条件
            if (userId) {
                queryOptions.userId = userId;
            }
            if (type !== undefined && type !== null) {
                queryOptions.type = parseInt(type) || null;
            }
            if (keyId) {
                // 如果提供了keyId，可以按凭证ID查询
                // 这里可以根据需要添加按ID查询的逻辑
            }
            queryOptions.status = 1; // 只查询正常状态的凭证

            // 调用数据库查询凭证
            const result = db.getCredentialsPaginated(queryOptions);

            // 格式化返回数据
            const formattedData = {
                page,
                size,
                total: result.pagination.total,
                totalPage: result.pagination.totalPages,
                count: result.data.length,
                content: result.data.map(credential => {
                    // 返回凭证表的所有字段
                    return {
                        keyId: credential.id,           // 凭证ID
                        userId: credential.userId,      // 用户ID
                        type: credential.type,          // 凭证类型
                        code: credential.code,          // 凭证值/编码
                        name: credential.name,          // 凭证名称/描述
                        status: credential.status,      // 凭证状态
                        expires_at: credential.expires_at, // 过期时间戳
                        extra: credential.extra,        // 扩展信息
                        created_at: credential.created_at, // 创建时间戳
                        updated_at: credential.updated_at  // 更新时间戳
                    };
                })
            };

            console.log(`查询凭证成功: keyId=${keyId}, userId=${userId}, type=${type}, page=${page}, size=${size}, 找到${result.data.length}条记录`);

            const response = await createResponse(serialNo, uuid, '000000', formattedData);
            publish(`access_device/v2/cmd/getKey_reply`, response);

        } catch (dbError) {
            console.error('数据库查询凭证失败:', dbError);
            const response = await createResponse(serialNo, uuid, '100000', `Database error: ${dbError.message}`);
            publish(`access_device/v2/cmd/getKey_reply`, response);
        }

    } catch (error) {
        console.error('处理查询凭证请求失败:', error);
        const response = await createResponse(payload.serialNo, payload.uuid, '100000', `Internal server error: ${error.message}`);
        publish(`access_device/v2/cmd/getKey_reply`, response);
    }
}

/**
 * 处理添加权限请求
 * @param {Object} payload - MQTT消息载荷
 */
async function handleInsertPermission(payload) {
    try {
        console.log('收到添加权限请求:', payload);

        const { serialNo, uuid, data } = payload;

        if (!serialNo || !uuid) {
            const response = await createResponse(serialNo, uuid, '100000', 'Parameter error: missing required parameters');
            publish(`access_device/v2/cmd/insertPermission_reply`, response);
            return;
        }

        if (!data || !Array.isArray(data)) {
            const response = await createResponse(serialNo, uuid, '100001', 'Invalid data format: data field must be an array');
            publish(`access_device/v2/cmd/insertPermission_reply`, response);
            return;
        }

        if (data.length > 100) {
            const response = await createResponse(serialNo, uuid, '100001', 'Too many permissions: maximum 100 per request');
            publish(`access_device/v2/cmd/insertPermission_reply`, response);
            return;
        }

        const results = [];
        const errors = [];

        for (const permissionData of data) {
            try {
                const { permissionId, userId, time, extra } = permissionData;

                // 验证必要字段
                if (!permissionId || !userId || !time) {
                    errors.push(`Missing required fields: permissionId, userId, time`);
                    continue;
                }

                // 验证字段长度
                if (permissionId.length < 6 || permissionId.length > 128) {
                    errors.push(`Invalid permissionId length: must be 6-128 characters`);
                    continue;
                }

                if (userId.length < 6 || userId.length > 128) {
                    errors.push(`Invalid userId length: must be 6-128 characters`);
                    continue;
                }

                // 验证时间对象
                if (typeof time !== 'object' || time === null) {
                    errors.push(`Invalid time format: must be an object`);
                    continue;
                }


                // 实现添加权限逻辑
                try {
                    // 解析time对象中的字段
                    const {
                        type: timeType,
                        range: {
                            beginTime: repeatBeginTime,
                            endTime: repeatEndTime
                        } = {},
                        beginTime,
                        endTime,
                        weekPeriodTime: period
                    } = time;

                    // 验证必要的时间字段 - 只有type是必传的，type可能为0
                    if (timeType === undefined || timeType === null) {
                        errors.push(`Missing required time field: type`);
                        continue;
                    }

                    // 确保所有参数都是正确的类型
                    const doorNumber = parseInt(permissionId) || 1; // 将permissionId转换为数字作为门号
                    const timeTypeNum = parseInt(timeType) || 0;
                    const beginTimeNum = parseInt(beginTime) || 0;
                    const endTimeNum = parseInt(endTime) || 0;
                    const repeatBeginTimeNum = repeatBeginTime ? parseInt(repeatBeginTime) : null;
                    const repeatEndTimeNum = repeatEndTime ? parseInt(repeatEndTime) : null;

                    // 处理period参数，确保是字符串格式
                    let periodStr = null;
                    if (period) {
                        if (Array.isArray(period)) {
                            periodStr = period.join(',');
                        } else if (typeof period === 'string') {
                            periodStr = period;
                        } else {
                            periodStr = String(period);
                        }
                    }

                    // 调用数据库创建权限
                    const createdPermissionId = db.createPermission(
                        userId,           // userId
                        doorNumber,       // door (转换为数字)
                        timeTypeNum,      // timeType (转换为数字)
                        beginTimeNum,     // beginTime (转换为数字)
                        endTimeNum,       // endTime (转换为数字)
                        {
                            id: permissionId,           // 使用传入的permissionId作为权限ID
                            repeatBeginTime: repeatBeginTimeNum,  // repeatBeginTime (转换为数字或null)
                            repeatEndTime: repeatEndTimeNum,      // repeatEndTime (转换为数字或null)
                            period: periodStr,                    // period (处理后的字符串或null)
                            status: 1,                  // 默认状态为正常
                            extra: extra || null        // 扩展信息
                        }
                    );

                    console.log(`添加权限成功: permissionId=${createdPermissionId}, userId=${userId}`);
                    results.push({ permissionId: createdPermissionId, userId, status: 'success' });

                } catch (dbError) {
                    console.error('数据库添加权限失败:', dbError);
                    errors.push(`Database error for permission ${permissionId}: ${dbError.message}`);
                }

            } catch (error) {
                console.error('添加权限失败:', error);
                errors.push(`Failed to add permission ${permissionData.permissionId}: ${error.message}`);
            }
        }

        if (errors.length > 0) {
            const response = await createResponse(serialNo, uuid, '100001', `Some permissions failed to add: ${errors.join('; ')}`);
            publish(`access_device/v2/cmd/insertPermission_reply`, response);
        } else {
            const response = await createResponse(serialNo, uuid, '000000');
            publish(`access_device/v2/cmd/insertPermission_reply`, response);
        }

    } catch (error) {
        console.error('处理添加权限请求失败:', error);
        const response = await createResponse(payload.serialNo, payload.uuid, '100000', `Internal server error: ${error.message}`);
        publish(`access_device/v2/cmd/insertPermission_reply`, response);
    }
}

/**
 * 处理删除权限请求
 * @param {Object} payload - MQTT消息载荷
 */
async function handleDelPermission(payload) {
    try {
        console.log('收到删除权限请求:', payload);

        const { serialNo, uuid, data } = payload;

        if (!serialNo || !uuid) {
            const response = await createResponse(serialNo, uuid, '100000', 'Parameter error: missing required parameters');
            publish(`access_device/v2/cmd/delPermission_reply`, response);
            return;
        }

        if (!data || typeof data !== 'object') {
            const response = await createResponse(serialNo, uuid, '100000', 'Parameter error: data field must be an object');
            publish(`access_device/v2/cmd/delPermission_reply`, response);
            return;
        }

        const { userIds = [], permissionIds = [] } = data;

        if (!Array.isArray(userIds) || !Array.isArray(permissionIds)) {
            const response = await createResponse(serialNo, uuid, '100000', 'Invalid parameter: userIds and permissionIds must be arrays');
            publish(`access_device/v2/cmd/delPermission_reply`, response);
            return;
        }

        if (userIds.length === 0 && permissionIds.length === 0) {
            const response = await createResponse(serialNo, uuid, '100000', 'Invalid parameter: at least one of userIds or permissionIds must be provided');
            publish(`access_device/v2/cmd/delPermission_reply`, response);
            return;
        }

        const results = [];
        const errors = [];

        // 删除指定权限ID
        for (const permissionId of permissionIds) {
            try {
                // TODO: 实现删除权限逻辑
                // 例如: await permissionManager.deletePermission(permissionId);
                console.log(`删除权限: permissionId=${permissionId}`);
                results.push({ permissionId, status: 'deleted' });
            } catch (error) {
                console.error('删除权限失败:', error);
                errors.push(`Failed to delete permission ${permissionId}: ${error.message}`);
            }
        }

        // 删除指定用户的所有权限
        for (const userId of userIds) {
            try {
                // TODO: 实现删除用户所有权限逻辑
                // 例如: await permissionManager.deleteUserPermissions(userId);
                console.log(`删除用户所有权限: userId=${userId}`);
                results.push({ userId, status: 'deleted' });
            } catch (error) {
                console.error('删除用户权限失败:', error);
                errors.push(`Failed to delete permissions for user ${userId}: ${error.message}`);
            }
        }

        if (errors.length > 0) {
            const response = await createResponse(serialNo, uuid, '100001', `Some permissions failed to delete: ${errors.join('; ')}`);
            publish(`access_device/v2/cmd/delPermission_reply`, response);
        } else {
            const response = await createResponse(serialNo, uuid, '000000');
            publish(`access_device/v2/cmd/delPermission_reply`, response);
        }

    } catch (error) {
        console.error('处理删除权限请求失败:', error);
        const response = await createResponse(payload.serialNo, payload.uuid, '100000', `Internal server error: ${error.message}`);
        publish(`access_device/v2/cmd/delPermission_reply`, response);
    }
}

/**
 * 处理清空权限请求
 * @param {Object} payload - MQTT消息载荷
 */
async function handleClearPermission(payload) {
    try {
        console.log('收到清空权限请求:', payload);

        const { serialNo, uuid } = payload;

        if (!serialNo || !uuid) {
            const response = await createResponse(serialNo, uuid, '100000', 'Parameter error: missing required parameters');
            publish(`access_device/v2/cmd/clearPermission_reply`, response);
            return;
        }

        // TODO: 实现清空所有权限逻辑
        // 例如: await permissionManager.clearAllPermissions();
        console.log('清空所有权限');

        const response = await createResponse(serialNo, uuid, '000000');
        publish(`access_device/v2/cmd/clearPermission_reply`, response);

    } catch (error) {
        console.error('处理清空权限请求失败:', error);
        const response = await createResponse(payload.serialNo, payload.uuid, '100000', `Internal server error: ${error.message}`);
        publish(`access_device/v2/cmd/clearPermission_reply`, response);
    }
}

/**
 * 处理查询权限请求
 * @param {Object} payload - MQTT消息载荷
 */
async function handleGetPermission(payload) {
    try {
        console.log('收到查询权限请求:', payload);

        const { serialNo, uuid, data } = payload;

        if (!serialNo || !uuid) {
            const response = await createResponse(serialNo, uuid, '100000', 'Parameter error: missing required parameters');
            publish(`access_device/v2/cmd/getPermission_reply`, response);
            return;
        }

        if (!data || typeof data !== 'object') {
            const response = await createResponse(serialNo, uuid, '100000', 'Parameter error: data field must be an object');
            publish(`access_device/v2/cmd/getPermission_reply`, response);
            return;
        }

        const { permissionId, userId, page = 0, size = 20 } = data;

        if (size <= 0 || size > 100) {
            const response = await createResponse(serialNo, uuid, '100001', 'Invalid parameter: size must be between 1 and 100');
            publish(`access_device/v2/cmd/getPermission_reply`, response);
            return;
        }

        // 实现查询权限逻辑
        try {
            // 构建查询条件
            const queryOptions = {
                page: page + 1, // 数据库分页从1开始，前端从0开始
                pageSize: size,
                orderBy: 'created_at',
                order: 'DESC'
            };

            // 添加筛选条件
            if (userId) {
                queryOptions.userId = userId;
            }
            if (permissionId) {
                queryOptions.door = parseInt(permissionId) || null; // 使用permissionId作为门号查询
            }
            queryOptions.status = 1; // 只查询正常状态的权限

            // 调用数据库查询权限
            const result = db.getPermissionsPaginated(queryOptions);

            // 按照新增时的模板格式化返回数据
            const formattedData = {
                page,
                size,
                total: result.pagination.total,
                totalPage: result.pagination.totalPages,
                count: result.data.length,
                content: result.data.map(permission => {
                    // 将数据库字段映射回新增时的格式
                    return {
                        permissionId: permission.id,
                        userId: permission.userId,
                        time: {
                            type: permission.timeType,
                            beginTime: permission.beginTime,
                            endTime: permission.endTime,
                            range: {
                                beginTime: permission.repeatBeginTime,
                                endTime: permission.repeatEndTime
                            },
                            weekPeriodTime: permission.period
                        },
                        extra: permission.extra
                    };
                })
            };

            console.log(`查询权限成功: permissionId=${permissionId}, userId=${userId}, page=${page}, size=${size}, 找到${result.data.length}条记录`);

            const response = await createResponse(serialNo, uuid, '000000', formattedData);
            publish(`access_device/v2/cmd/getPermission_reply`, response);

        } catch (dbError) {
            console.error('数据库查询权限失败:', dbError);
            const response = await createResponse(serialNo, uuid, '100000', `Database error: ${dbError.message}`);
            publish(`access_device/v2/cmd/getPermission_reply`, response);
        }

    } catch (error) {
        console.error('处理查询权限请求失败:', error);
        const response = await createResponse(payload.serialNo, payload.uuid, '100000', `Internal server error: ${error.message}`);
        publish(`access_device/v2/cmd/getPermission_reply`, response);
    }
}

/**
 * 处理固件升级请求
 * @param {Object} payload - MQTT消息载荷
 */
async function handleUpgradeFirmware(payload) {
    try {
        console.log('收到固件升级请求:', payload);

        // 检查是否正在升级
        if (isUpgrading) {
            console.log('设备正在升级中，拒绝新的升级请求');
            const response = await createResponse(payload.serialNo, payload.uuid, '100000', 'Device is currently upgrading, please try again later');
            publish(`access_device/v2/cmd/upgradeFirmware_reply`, response);
            return;
        }

        const { serialNo, uuid, data } = payload;

        if (!serialNo || !uuid) {
            const response = await createResponse(serialNo, uuid, '100000', 'Parameter error: missing required parameters');
            publish(`access_device/v2/cmd/upgradeFirmware_reply`, response);
            return;
        }

        if (!data || typeof data !== 'object') {
            const response = await createResponse(serialNo, uuid, '100000', 'Parameter error: upgrade data format is invalid');
            publish(`access_device/v2/cmd/upgradeFirmware_reply`, response);
            return;
        }

        const { type, url, md5, extra } = data;

        // 验证必要字段
        if (type === undefined || type === null) {
            const response = await createResponse(serialNo, uuid, '100000', 'Parameter error: type field is required');
            publish(`access_device/v2/cmd/upgradeFirmware_reply`, response);
            return;
        }

        if (![0, 10].includes(type)) {
            const response = await createResponse(serialNo, uuid, '100000', 'Invalid upgrade type: must be 0 (firmware) or 10 (user resource)');
            publish(`access_device/v2/cmd/upgradeFirmware_reply`, response);
            return;
        }

        if (type === 0) {
            // 固件升级
            if (!url || typeof url !== 'string') {
                const response = await createResponse(serialNo, uuid, '100000', 'Parameter error: url field is required for firmware upgrade');
                publish(`access_device/v2/cmd/upgradeFirmware_reply`, response);
                return;
            }

            if (url.length > 2048) {
                const response = await createResponse(serialNo, uuid, '100000', 'Parameter error: url too long (max 2048 characters)');
                publish(`access_device/v2/cmd/upgradeFirmware_reply`, response);
                return;
            }

            if (!md5 || typeof md5 !== 'string') {
                const response = await createResponse(serialNo, uuid, '100000', 'Parameter error: md5 field is required for firmware upgrade');
                publish(`access_device/v2/cmd/upgradeFirmware_reply`, response);
                return;
            }

            if (md5.length > 128) {
                const response = await createResponse(serialNo, uuid, '100000', 'Parameter error: md5 too long (max 128 characters)');
                publish(`access_device/v2/cmd/upgradeFirmware_reply`, response);
                return;
            }

            console.log(`固件升级: url=${url}, md5=${md5}`);

            // 设置升级状态
            isUpgrading = true;
            console.log('设置升级状态为进行中');

            const filePath = '/data/upgrade/app.zip';
            console.log(`开始异步下载固件文件到: ${filePath}`);

            // 确保目录存在
            try {
                await tjs.spawn(['mkdir', '-p', '/data/upgrade']).wait();
                console.log('确保升级目录存在: /data/upgrade');
            } catch (mkdirError) {
                console.error('创建升级目录失败:', mkdirError);
                isUpgrading = false;
                const response = await createResponse(serialNo, uuid, '100000', 'Failed to create upgrade directory');
                publish(`access_device/v2/cmd/upgradeFirmware_reply`, response);
                return;
            }

            const exitData = await downloadFile(url, filePath);
            console.log(`异步下载完成，退出状态: ${exitData.exit_status}`);

            // 检查下载是否成功
            if (exitData.exit_status !== 0) {
                // 下载失败，删除文件并清除升级状态
                isUpgrading = false;
                console.log('文件下载失败，删除下载文件');
                try {
                    await tjs.spawn(['rm', '-rf', filePath]).wait();
                    console.log('已删除下载失败的文件');
                } catch (deleteError) {
                    console.error('删除下载失败的文件时出错:', deleteError);
                }
                const response = await createResponse(serialNo, uuid, '100000', `File download failed with exit status: ${exitData.exit_status}`);
                publish(`access_device/v2/cmd/upgradeFirmware_reply`, response);
                return;
            }

            const fileMd5 = md5HashFile(filePath);
            console.log(`文件MD5: ${fileMd5}, 期望MD5: ${md5}`);

            if (fileMd5 !== md5) {
                // 清除升级状态
                isUpgrading = false;
                console.log('MD5验证失败，删除下载文件并清除升级状态');
                try {
                    await tjs.spawn(['rm', '-rf', filePath]).wait();
                    console.log('已删除MD5校验失败的文件');
                } catch (deleteError) {
                    console.error('删除MD5校验失败的文件时出错:', deleteError);
                }
                const response = await createResponse(serialNo, uuid, '100000', `MD5 verification failed, file MD5: ${fileMd5}, expected MD5: ${md5}`);
                publish(`access_device/v2/cmd/upgradeFirmware_reply`, response);
                return;
            }

            // 升级成功，准备重启
            console.log('固件升级成功，准备重启设备');
            // 注意：这里不清除isUpgrading状态，因为设备即将重启
            // 这样可以防止在重启前的1秒延迟期间接受新的升级请求
            setTimeout(() => {
                tjs.spawn(['reboot'])
            }, 1000);

        } else if (type === 10) {
            // 用户资源升级
            if (!extra || typeof extra !== 'object') {
                const response = await createResponse(serialNo, uuid, '100000', 'Parameter error: extra field is required for user resource upgrade');
                publish(`access_device/v2/cmd/upgradeFirmware_reply`, response);
                return;
            }

            const { name, mode } = extra;

            if (!name || typeof name !== 'string') {
                const response = await createResponse(serialNo, uuid, '100000', 'Parameter error: name field is required for user resource upgrade');
                publish(`access_device/v2/cmd/upgradeFirmware_reply`, response);
                return;
            }

            if (name.length > 64) {
                const response = await createResponse(serialNo, uuid, '100000', 'Parameter error: name too long (max 64 characters)');
                publish(`access_device/v2/cmd/upgradeFirmware_reply`, response);
                return;
            }

            if (mode === undefined || mode === null) {
                const response = await createResponse(serialNo, uuid, '100000', 'Parameter error: mode field is required for user resource upgrade');
                publish(`access_device/v2/cmd/upgradeFirmware_reply`, response);
                return;
            }

            if (![0, 1].includes(mode)) {
                const response = await createResponse(serialNo, uuid, '100000', 'Invalid resource mode: must be 0 (delete) or 1 (add)');
                publish(`access_device/v2/cmd/upgradeFirmware_reply`, response);
                return;
            }

            // TODO: 实现用户资源升级逻辑
            // 这里可以调用用户资源管理接口
            // 例如: await resourceManager.updateUserResource(name, mode);
            console.log(`用户资源升级: name=${name}, mode=${mode}`);
        }

        // 返回成功响应
        const response = await createResponse(serialNo, uuid, '000000');
        publish(`access_device/v2/cmd/upgradeFirmware_reply`, response);

    } catch (error) {
        console.error('处理固件升级请求失败:', error);
        // 清除升级状态
        isUpgrading = false;
        console.log('升级过程中发生错误，清除升级状态');
        const response = await createResponse(payload.serialNo, payload.uuid, '100000', `Internal server error: ${error.message}`);
        publish(`access_device/v2/cmd/upgradeFirmware_reply`, response);
    }
}

/**
 * 生成序列号
 * @returns {string} 序列号
 */
function generateSerialNo() {
    return Math.random().toString(36).substring(2, 12);
}


export {
    mqttAccessInit
};