import { face } from 'dxDriver';

export const onConfigKeyChangeService = async (onConfigChange, onConfigKeyChange) => {
    // onConfigChange(async (key, oldValue, newValue, configManager) => {
    //     console.log(`[全局回调] 配置项 ${key} 已变更:`);
    //     console.log(`  旧值: ${JSON.stringify(oldValue)}`);
    //     console.log(`  新值: ${JSON.stringify(newValue)}`);
    // });
    onConfigKeyChange('face.livenessOff', async (oldValue, newValue, configManager) => {
        console.log(`[特定配置项回调] 配置项 face.livenessOff 已变更:`);
        console.log(`  旧值: ${JSON.stringify(oldValue)}`);
        console.log(`  新值: ${JSON.stringify(newValue)}`);
        face.faceUpdateConfig({ living_check_enable: newValue });
    });
};
