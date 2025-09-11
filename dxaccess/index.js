import { initConfigManager } from './lib/config/index.js';
import { saveToFile, downloadFile } from './lib/utils/index.js';
import { access as accessFunc, accessByUserId, accessByUserName, db} from './lib/access/index.js';
import { mqttAccessInit } from './lib/mqtt/index.js';

export const config = {
    initConfigManager
};

export const utils = {
    saveToFile,
    downloadFile
};

export const access = {
    access: accessFunc,
    accessByUserId,
    accessByUserName,
    db
};

export const mqttAccess = {
    mqttAccessInit
};