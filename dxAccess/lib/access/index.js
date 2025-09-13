import db from './AccessControlDB.js';
import { timePermission } from './timePermission.js';

/**
 * 核心权限验证逻辑
 * @param {string} userId - 用户ID
 * @param {Object} credential - 凭证对象（可选）
 * @returns {Object} 验证结果
 */
function validateUserPermissions(userId, credential = null) {
    try {
        // 1. 根据用户ID查询权限表记录
        const permissions = db.getPermissionsByUserId(userId);
        if (!permissions || permissions.length === 0) {
            console.log(`[权限验证失败] 用户ID: ${userId}, 失败原因: 用户无任何权限`);
            return {
                success: false,
                message: "用户无任何权限",
                result: -1 // 拒绝
            };
        }

        // 2. 使用timePermission.js规则判断每条权限记录是否有权限
        const currentTime = Math.floor(Date.now() / 1000);
        let hasValidPermission = false;
        let validPermission = null;

        for (const permission of permissions) {
            // 构建时间配置对象
            const timeConfig = db.buildTimeConfig(permission);

            console.log("permission", permission);
            console.log(timeConfig, currentTime);

            // 使用timePermission函数判断是否有权限
            if (timePermission(timeConfig, currentTime)) {
                hasValidPermission = true;
                validPermission = permission;
                break; // 找到一个有效权限即可
            }
        }

        if (hasValidPermission) {
            const result = {
                success: true,
                message: "权限验证通过",
                result: 1, // 成功
                permission: validPermission
            };
            // 如果有凭证信息，也包含在结果中
            if (credential) {
                result.credential = credential;
            }
            return result;
        } else {
            console.log(`[权限验证失败] 用户ID: ${userId}, 失败原因: 当前时间无权限, 权限记录数: ${permissions.length}`);
            return {
                success: false,
                message: "当前时间无权限",
                result: 0 // 失败
            };
        }

    } catch (error) {
        console.error(`[权限验证失败] 用户ID: ${userId}, 失败原因: 系统错误`, error);
        return {
            success: false,
            message: "系统错误",
            result: -1 // 拒绝
        };
    }
}

/**
 * 门禁权限验证函数
 * @param {number} type - 凭证类型
 * @param {string} code - 凭证值/编码
 * @returns {Object} 验证结果
 */
function access(type, code) {
    console.log(`[门禁验证] 凭证类型: ${type}, 凭证值: ${code}`);

    try {
        // 1. 根据type和code查询凭证记录
        const credential = db.getCredentialByTypeAndCodeAccess(type, code);
        if (!credential) {
            console.log(`[门禁验证失败] 凭证类型: ${type}, 凭证值: ${code}, 失败原因: 凭证不存在或已失效`);
            return {
                success: false,
                message: "凭证不存在或已失效",
                result: -1 // 拒绝
            };
        }

        // 2. 调用核心权限验证逻辑
        return validateUserPermissions(credential.userId, credential);

    } catch (error) {
        console.error(`[门禁验证失败] 凭证类型: ${type}, 凭证值: ${code}, 失败原因: 系统错误`, error);
        return {
            success: false,
            message: "系统错误",
            result: -1 // 拒绝
        };
    }
}

/**
 * 根据用户ID进行门禁权限验证函数
 * @param {string} userId - 用户ID
 * @returns {Object} 验证结果
 */
function accessByUserId(userId) {
    console.log(`[用户ID验证] 用户ID: ${userId}`);
    const result = validateUserPermissions(userId);
    
    // 记录验证结果
    if (!result.success) {
        console.log(`[用户ID验证失败] 用户ID: ${userId}, 失败原因: ${result.message}, 结果码: ${result.result}`);
    } else {
        console.log(`[用户ID验证成功] 用户ID: ${userId}`);
    }
    
    return result;
}

/**
 * 根据用户姓名进行门禁权限验证函数
 * @param {string} userName - 用户姓名
 * @returns {Object} 验证结果
 */
function accessByUserName(userName) {
    console.log(`[用户名验证] 用户名: ${userName}`);

    try {
        // 1. 根据用户姓名查询用户信息
        const user = db.getUserByName(userName);
        if (!user) {
            console.log(`[用户名验证失败] 用户名: ${userName}, 失败原因: 用户不存在`);
            return {
                success: false,
                message: "用户不存在",
                result: -1 // 拒绝
            };
        }

        // 2. 调用核心权限验证逻辑
        const result = validateUserPermissions(user.id);

        // 3. 在结果中包含用户信息
        if (result.success) {
            result.user = user;
            console.log(`[用户名验证成功] 用户名: ${userName}, 用户ID: ${user.id}`);
        } else {
            console.log(`[用户名验证失败] 用户名: ${userName}, 用户ID: ${user.id}, 失败原因: ${result.message}, 结果码: ${result.result}`);
        }

        return result;

    } catch (error) {
        console.error(`[用户名验证失败] 用户名: ${userName}, 失败原因: 系统错误`, error);
        return {
            success: false,
            message: "系统错误",
            result: -1 // 拒绝
        };
    }
}

export { access, accessByUserId, accessByUserName, db };