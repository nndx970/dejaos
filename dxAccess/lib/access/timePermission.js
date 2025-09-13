
/**
 * 时间权限验证通用方法
 * 根据MQTT协议文档中的时间区间规则进行权限验证
 * 
 * @param {Object} timeConfig - 时间配置对象
 * @param {number} timeConfig.type - 时间类型：0-缺省模式，1-通常模式，2-每日模式，3-周重复模式
 * @param {Object} [timeConfig.range] - 时间范围（type=1,2,3时必填）
 * @param {number} timeConfig.range.beginTime - 开始时间戳（Unix时间戳）
 * @param {number} timeConfig.range.endTime - 结束时间戳（Unix时间戳）
 * @param {number} [timeConfig.beginTime] - 每日开始时间（type=2时必填，当日零点到该时间的秒数）
 * @param {number} [timeConfig.endTime] - 每日结束时间（type=2时必填，当日零点到该时间的秒数）
 * @param {Object} [timeConfig.weekPeriodTime] - 周重复时间配置（type=3时必填）
 * @param {string} [timeConfig.weekPeriodTime.1] - 周一时间段，格式："HH:MM-HH:MM|HH:MM-HH:MM"
 * @param {string} [timeConfig.weekPeriodTime.2] - 周二时间段，格式："HH:MM-HH:MM|HH:MM-HH:MM"
 * @param {string} [timeConfig.weekPeriodTime.3] - 周三时间段，格式："HH:MM-HH:MM|HH:MM-HH:MM"
 * @param {string} [timeConfig.weekPeriodTime.4] - 周四时间段，格式："HH:MM-HH:MM|HH:MM-HH:MM"
 * @param {string} [timeConfig.weekPeriodTime.5] - 周五时间段，格式："HH:MM-HH:MM|HH:MM-HH:MM"
 * @param {string} [timeConfig.weekPeriodTime.6] - 周六时间段，格式："HH:MM-HH:MM|HH:MM-HH:MM"
 * @param {string} [timeConfig.weekPeriodTime.7] - 周日时间段，格式："HH:MM-HH:MM|HH:MM-HH:MM"
 * @param {number} [currentTime] - 当前时间戳，默认为当前时间
 * @returns {boolean} 是否有权限
 */
function timePermission(timeConfig, currentTime = null) {
    if (!timeConfig || typeof timeConfig !== 'object') {
        console.log('[时间权限验证失败] 失败原因: 时间配置对象无效或为空');
        return false;
    }

    const now = currentTime || Math.floor(Date.now() / 1000);
    const currentDate = new Date(now * 1000);
    const currentWeekday = currentDate.getDay() || 7; // 将周日从0改为7
    const currentHour = currentDate.getHours();
    const currentMinute = currentDate.getMinutes();
    const currentTimeOfDay = currentHour * 3600 + currentMinute * 60; // 当日零点到当前时间的秒数
    
    const timeStr = currentDate.toLocaleString('zh-CN', { timeZone: 'Asia/Shanghai' });
    const weekdayNames = ['周日', '周一', '周二', '周三', '周四', '周五', '周六'];
    const currentWeekdayName = weekdayNames[currentWeekday === 7 ? 0 : currentWeekday];
    const currentTimeStr = formatTimeOfDay(currentTimeOfDay);
    
    console.log(`[时间权限验证] 当前时间: ${timeStr} (${currentWeekdayName}), 权限类型: ${getPermissionTypeName(timeConfig.type)}`);

    switch (timeConfig.type) {
        case 0:
            // 缺省模式，不限时间，一直有效
            console.log('[时间权限验证成功] 权限类型: 缺省模式, 说明: 不限时间，一直有效');
            return true;

        case 1:
            // 通常模式，从开始到结束时间内有效
            if (!timeConfig.range || !timeConfig.range.beginTime || !timeConfig.range.endTime) {
                console.log('[时间权限验证失败] 权限类型: 通常模式, 失败原因: 时间范围配置不完整');
                return false;
            }
            
            const beginTimeStr = new Date(timeConfig.range.beginTime * 1000).toLocaleString('zh-CN', { timeZone: 'Asia/Shanghai' });
            const endTimeStr = new Date(timeConfig.range.endTime * 1000).toLocaleString('zh-CN', { timeZone: 'Asia/Shanghai' });
            
            if (now >= timeConfig.range.beginTime && now <= timeConfig.range.endTime) {
                console.log(`[时间权限验证成功] 权限类型: 通常模式, 说明: 在指定时间范围内有效, 时间范围: ${beginTimeStr} 至 ${endTimeStr}`);
                return true;
            } else {
                if (now < timeConfig.range.beginTime) {
                    console.log(`[时间权限验证失败] 权限类型: 通常模式, 失败原因: 当前时间早于开始时间, 开始时间: ${beginTimeStr}`);
                } else {
                    console.log(`[时间权限验证失败] 权限类型: 通常模式, 失败原因: 当前时间晚于结束时间, 结束时间: ${endTimeStr}`);
                }
                return false;
            }

        case 2:
            // 每日模式，每日特定时间到特定时间内有效
            if (!timeConfig.range || !timeConfig.range.beginTime || !timeConfig.range.endTime ||
                timeConfig.beginTime === undefined || timeConfig.endTime === undefined) {
                console.log('[时间权限验证失败] 权限类型: 每日模式, 失败原因: 时间配置不完整');
                return false;
            }

            const dailyBeginTimeStr = formatTimeOfDay(timeConfig.beginTime);
            const dailyEndTimeStr = formatTimeOfDay(timeConfig.endTime);
            
            // 检查是否在总体时间范围内
            if (now < timeConfig.range.beginTime || now > timeConfig.range.endTime) {
                const rangeBeginStr = new Date(timeConfig.range.beginTime * 1000).toLocaleString('zh-CN', { timeZone: 'Asia/Shanghai' });
                const rangeEndStr = new Date(timeConfig.range.endTime * 1000).toLocaleString('zh-CN', { timeZone: 'Asia/Shanghai' });
                if (now < timeConfig.range.beginTime) {
                    console.log(`[时间权限验证失败] 权限类型: 每日模式, 失败原因: 当前时间早于总体开始时间, 总体开始时间: ${rangeBeginStr}`);
                } else {
                    console.log(`[时间权限验证失败] 权限类型: 每日模式, 失败原因: 当前时间晚于总体结束时间, 总体结束时间: ${rangeEndStr}`);
                }
                return false;
            }

            // 检查是否在每日时间段内
            if (currentTimeOfDay >= timeConfig.beginTime && currentTimeOfDay <= timeConfig.endTime) {
                console.log(`[时间权限验证成功] 权限类型: 每日模式, 说明: 在每日时间段内有效, 每日时间段: ${dailyBeginTimeStr} 至 ${dailyEndTimeStr}, 当前时间: ${currentTimeStr}`);
                return true;
            } else {
                if (currentTimeOfDay < timeConfig.beginTime) {
                    console.log(`[时间权限验证失败] 权限类型: 每日模式, 失败原因: 当前时间早于每日开始时间, 每日开始时间: ${dailyBeginTimeStr}, 当前时间: ${currentTimeStr}`);
                } else {
                    console.log(`[时间权限验证失败] 权限类型: 每日模式, 失败原因: 当前时间晚于每日结束时间, 每日结束时间: ${dailyEndTimeStr}, 当前时间: ${currentTimeStr}`);
                }
                return false;
            }

        case 3:
            // 周重复模式，周一到周日特定时间到特定时间内有效
            if (!timeConfig.range || !timeConfig.range.beginTime || !timeConfig.range.endTime ||
                !timeConfig.weekPeriodTime) {
                console.log('[时间权限验证失败] 权限类型: 周重复模式, 失败原因: 时间配置不完整');
                return false;
            }

            const rangeBeginStr = new Date(timeConfig.range.beginTime * 1000).toLocaleString('zh-CN', { timeZone: 'Asia/Shanghai' });
            const rangeEndStr = new Date(timeConfig.range.endTime * 1000).toLocaleString('zh-CN', { timeZone: 'Asia/Shanghai' });
            
            // 检查是否在总体时间范围内
            if (now < timeConfig.range.beginTime || now > timeConfig.range.endTime) {
                if (now < timeConfig.range.beginTime) {
                    console.log(`[时间权限验证失败] 权限类型: 周重复模式, 失败原因: 当前时间早于总体开始时间, 总体开始时间: ${rangeBeginStr}`);
                } else {
                    console.log(`[时间权限验证失败] 权限类型: 周重复模式, 失败原因: 当前时间晚于总体结束时间, 总体结束时间: ${rangeEndStr}`);
                }
                return false;
            }

            // 检查当前星期几是否有权限
            const weekdayKey = currentWeekday.toString();
            const weekdayTime = timeConfig.weekPeriodTime[weekdayKey];

            if (!weekdayTime) {
                console.log(`[时间权限验证失败] 权限类型: 周重复模式, 失败原因: ${currentWeekdayName}无权限配置`);
                return false; // 当天无权限
            }

            // 解析当天的时间段
            const timeSlots = weekdayTime.split('|');
            
            for (const slot of timeSlots) {
                if (isTimeInSlot(currentTimeOfDay, slot)) {
                    console.log(`[时间权限验证成功] 权限类型: 周重复模式, 说明: 在${currentWeekdayName}的时间段内有效, 时间段: ${slot}, 当前时间: ${currentTimeStr}`);
                    return true;
                }
            }
            
            console.log(`[时间权限验证失败] 权限类型: 周重复模式, 失败原因: 当前时间不在${currentWeekdayName}的允许时间段内, 允许时间段: ${weekdayTime}, 当前时间: ${currentTimeStr}`);
            return false;

        default:
            console.log(`[时间权限验证失败] 失败原因: 未知的权限类型 ${timeConfig.type}`);
            return false;
    }
}

/**
 * 获取权限类型名称
 * @param {number} type - 权限类型
 * @returns {string} 权限类型名称
 */
function getPermissionTypeName(type) {
    const typeNames = {
        0: '缺省模式',
        1: '通常模式', 
        2: '每日模式',
        3: '周重复模式'
    };
    return typeNames[type] || `未知类型(${type})`;
}

/**
 * 将当日零点到指定时间的秒数格式化为时间字符串
 * @param {number} timeOfDay - 当日零点到指定时间的秒数
 * @returns {string} 格式化的时间字符串 "HH:MM"
 */
function formatTimeOfDay(timeOfDay) {
    const hours = Math.floor(timeOfDay / 3600);
    const minutes = Math.floor((timeOfDay % 3600) / 60);
    return `${hours.toString().padStart(2, '0')}:${minutes.toString().padStart(2, '0')}`;
}

/**
 * 检查当前时间是否在指定时间段内
 * @param {number} currentTimeOfDay - 当前时间（当日零点到当前时间的秒数）
 * @param {string} timeSlot - 时间段，格式："HH:MM-HH:MM"
 * @returns {boolean} 是否在时间段内
 */
function isTimeInSlot(currentTimeOfDay, timeSlot) {
    try {
        const [startTime, endTime] = timeSlot.split('-');
        if (!startTime || !endTime) {
            console.log(`[时间段验证失败] 时间段: ${timeSlot}, 失败原因: 时间段格式不正确`);
            return false;
        }

        const [startHour, startMinute] = startTime.split(':').map(Number);
        const [endHour, endMinute] = endTime.split(':').map(Number);

        const startTimeOfDay = startHour * 3600 + startMinute * 60;
        const endTimeOfDay = endHour * 3600 + endMinute * 60;
        const currentTimeStr = formatTimeOfDay(currentTimeOfDay);

        // 处理跨天的情况（如23:00-01:00）
        if (endTimeOfDay < startTimeOfDay) {
            const result = currentTimeOfDay >= startTimeOfDay || currentTimeOfDay <= endTimeOfDay;
            if (result) {
                console.log(`[时间段验证成功] 时间段: ${timeSlot} (跨天), 当前时间: ${currentTimeStr}`);
            } else {
                console.log(`[时间段验证失败] 时间段: ${timeSlot} (跨天), 当前时间: ${currentTimeStr}, 失败原因: 不在跨天时间段内`);
            }
            return result;
        } else {
            const result = currentTimeOfDay >= startTimeOfDay && currentTimeOfDay <= endTimeOfDay;
            if (result) {
                console.log(`[时间段验证成功] 时间段: ${timeSlot}, 当前时间: ${currentTimeStr}`);
            } else {
                if (currentTimeOfDay < startTimeOfDay) {
                    console.log(`[时间段验证失败] 时间段: ${timeSlot}, 当前时间: ${currentTimeStr}, 失败原因: 早于开始时间 ${startTime}`);
                } else {
                    console.log(`[时间段验证失败] 时间段: ${timeSlot}, 当前时间: ${currentTimeStr}, 失败原因: 晚于结束时间 ${endTime}`);
                }
            }
            return result;
        }
    } catch (error) {
        console.error(`[时间段验证失败] 时间段: ${timeSlot}, 失败原因: 解析时间段失败`, error);
        return false;
    }
}

/**
 * 创建时间配置对象的辅助方法
 */
const TimePermissionHelper = {
    /**
     * 创建缺省模式配置（一直有效）
     * @returns {Object} 时间配置对象
     */
    createDefault() {
        return { type: 0 };
    },

    /**
     * 创建通常模式配置（指定时间范围内有效）
     * @param {number} beginTime - 开始时间戳
     * @param {number} endTime - 结束时间戳
     * @returns {Object} 时间配置对象
     */
    createRange(beginTime, endTime) {
        return {
            type: 1,
            range: { beginTime, endTime }
        };
    },

    /**
     * 创建每日模式配置（每日特定时间段内有效）
     * @param {number} rangeBeginTime - 总体开始时间戳
     * @param {number} rangeEndTime - 总体结束时间戳
     * @param {number} dailyBeginTime - 每日开始时间（当日零点到该时间的秒数）
     * @param {number} dailyEndTime - 每日结束时间（当日零点到该时间的秒数）
     * @returns {Object} 时间配置对象
     */
    createDaily(rangeBeginTime, rangeEndTime, dailyBeginTime, dailyEndTime) {
        return {
            type: 2,
            range: { beginTime: rangeBeginTime, endTime: rangeEndTime },
            beginTime: dailyBeginTime,
            endTime: dailyEndTime
        };
    },

    /**
     * 创建周重复模式配置
     * @param {number} rangeBeginTime - 总体开始时间戳
     * @param {number} rangeEndTime - 总体结束时间戳
     * @param {Object} weekPeriodTime - 周重复时间配置
     * @returns {Object} 时间配置对象
     */
    createWeekly(rangeBeginTime, rangeEndTime, weekPeriodTime) {
        return {
            type: 3,
            range: { beginTime: rangeBeginTime, endTime: rangeEndTime },
            weekPeriodTime
        };
    },

    /**
     * 将时间字符串转换为当日零点到该时间的秒数
     * @param {string} timeStr - 时间字符串，格式："HH:MM"
     * @returns {number} 秒数
     */
    timeToSeconds(timeStr) {
        const [hour, minute] = timeStr.split(':').map(Number);
        return hour * 3600 + minute * 60;
    }
};

// 导出方法
export { timePermission, TimePermissionHelper };