import { system } from '../system/index.js';

/**
 * 快速网络管理工具
 * 简化的网络配置和监听功能
 * 使用 system.cmdAsync 异步方法执行命令
 */

class QuickNetwork {
    constructor(options = {}) {
        this.monitoring = false;
        this.callbacks = [];
        this.interval = null;

        // 支持传入回调函数
        this.onStatusChange = options.onStatusChange || null;
        this.onError = options.onError || null;
        this.onConnect = options.onConnect || null;
        this.onDisconnect = options.onDisconnect || null;

        // 如果传入了回调，自动注册
        if (this.onStatusChange) {
            this.addCallback(this.onStatusChange);
        }
    }

    /**
     * 添加状态变化回调
     * @param {Function} callback 回调函数，参数为 (currentState, lastState)
     */
    addCallback(callback) {
        if (typeof callback === 'function') {
            this.callbacks.push(callback);
        } else {
            console.warn('⚠️  回调函数必须是函数类型');
        }
    }

    /**
     * 移除状态变化回调
     * @param {Function} callback 要移除的回调函数
     */
    removeCallback(callback) {
        const index = this.callbacks.indexOf(callback);
        if (index > -1) {
            this.callbacks.splice(index, 1);
        } else {
            console.warn('⚠️  未找到要移除的回调函数');
        }
    }

    /**
     * 清空所有回调
     */
    clearCallbacks() {
        this.callbacks = [];
    }

    /**
     * 触发状态变化回调
     * @param {Object} currentState 当前状态
     * @param {Object} lastState 上次状态
     */
    triggerCallbacks(currentState, lastState) {
        // 默认回调函数（参照 system.cmdAsync 的模式）
        const defaultCallbacks = {
            onStatusChange: (current, last) => {
                console.log(`📡 状态变化: ${current.interface} ${last ? `${last.isUp ? 'UP' : 'DOWN'} -> ` : ''}${current.isUp ? 'UP' : 'DOWN'}`);
            },
            onConnect: (state) => {
                console.log(`🔗 连接成功: ${state.interface} ${state.hasIP ? '(有IP)' : '(无IP)'}`);
            },
            onDisconnect: (state) => {
                console.log(`🔌 连接断开: ${state.interface}`);
            },
            onError: (error, type) => {
                console.error(`❌ 错误 [${type}]:`, error.message);
            }
        };

        // 合并用户提供的回调（用户回调会覆盖默认回调）
        const finalCallbacks = { ...defaultCallbacks, ...this.getUserCallbacks() };

        // 触发通用状态变化回调
        this.callbacks.forEach(callback => {
            try {
                callback(currentState, lastState);
            } catch (error) {
                finalCallbacks.onError(error, 'callback');
            }
        });

        // 触发特定事件回调
        if (currentState && typeof currentState.isUp === 'boolean') {
            // 连接状态变化 - 只有在有上次状态且状态确实从DOWN变为UP时才触发
            if (lastState && typeof lastState.isUp === 'boolean' && !lastState.isUp && currentState.isUp) {
                try {
                    finalCallbacks.onConnect(currentState);
                } catch (error) {
                    finalCallbacks.onError(error, 'onConnect');
                }
            }

            // 断开连接状态变化
            if (lastState && typeof lastState.isUp === 'boolean' && lastState.isUp && !currentState.isUp) {
                try {
                    finalCallbacks.onDisconnect(currentState);
                } catch (error) {
                    finalCallbacks.onError(error, 'onDisconnect');
                }
            }
        }

        // 触发通用状态变化回调
        try {
            finalCallbacks.onStatusChange(currentState, lastState);
        } catch (error) {
            finalCallbacks.onError(error, 'onStatusChange');
        }
    }

    /**
     * 获取用户回调对象
     */
    getUserCallbacks() {
        return {
            onStatusChange: this.onStatusChange,
            onConnect: this.onConnect,
            onDisconnect: this.onDisconnect,
            onError: this.onError
        };
    }

    /**
     * 快速配置动态网络
     */
    async dhcp(iface = 'eth0') {
        console.log(`🚀 配置动态网络: ${iface}`);

        try {
            // 清空配置
            await this.run(['ip', 'address', 'flush', 'dev', iface]);
            await this.run(['ip', 'link', 'set', iface, 'up']);

            // 运行DHCP - 使用 system.cmdAsync
            await this.run(['busybox', 'udhcpc', '-q', '-i', iface]);

            console.log('✅ DHCP配置成功');
            return true;
        } catch (error) {
            console.error('❌ DHCP配置失败:', error.message);
            return false;
        }
    }

    /**
     * 快速配置静态网络
     */
    async static(iface = 'eth0', ip = '192.168.1.100', gateway = '192.168.1.1', netmask = '24', dns0 = '8.8.8.8', dns1 = '223.5.5.5') {
        // 支持子网掩码格式转换
        const cidr = this.netmaskToCidr(netmask);
        console.log(`🔧 配置静态网络: ${iface} -> ${ip}/${cidr} (${netmask})`);

        try {
            // 清空配置
            await this.run(['ip', 'address', 'flush', 'dev', iface]);

            // 启用接口
            await this.run(['ip', 'link', 'set', iface, 'up']);
            await this.sleep(1000);

            // 设置IP
            await this.run(['ip', 'address', 'add', `${ip}/${cidr}`, 'dev', iface]);

            // 设置网关
            await this.run(['ip', 'route', 'add', 'default', 'via', gateway]);

            // 设置DNS到 /etc/resolv.conf
            await this.setDNS(dns0, dns1);

            console.log('✅ 静态网络配置成功');
            return true;
        } catch (error) {
            console.error('❌ 静态网络配置失败:', error.message);
            return false;
        }
    }

    /**
     * 开始监听网络状态
     * @param {string} iface 网络接口名称
     * @param {Function} callback 可选的状态变化回调函数
     */
    async watch(iface = 'eth0', callback = null) {
        if (this.monitoring) {
            console.log('⚠️  网络监听已在运行');
            return;
        }

        console.log(`👁️  开始监听: ${iface}`);

        // 如果传入了回调，添加到回调列表
        if (callback) {
            this.addCallback(callback);
        }

        this.monitoring = true;
        let lastState = null;

        this.interval = setInterval(async () => {
            try {
                const currentState = await this.getState(iface);

                if (lastState && this.hasChanged(lastState, currentState)) {
                    // 使用统一的回调系统（参照 system.cmdAsync 模式）
                    this.triggerCallbacks(currentState, lastState);
                }

                lastState = currentState;
            } catch (error) {
                console.error('状态检查失败:', error.message);
                if (this.onError) {
                    this.onError(error, 'watch');
                }
            }
        }, 3000);

        console.log('✅ 监听已启动');
    }

    /**
     * 停止监听
     */
    stop() {
        if (this.interval) {
            clearInterval(this.interval);
            this.interval = null;
        }
        this.monitoring = false;
        console.log('✅ 监听已停止');
    }

    /**
     * 获取网络状态
     */
    async getState(iface) {
        try {
            const linkResult = await this.run(['ip', 'link', 'show', iface]);
            const addrResult = await this.run(['ip', 'addr', 'show', iface]);

            // 更精确的状态解析
            const isUp = linkResult.includes('state UP') && !linkResult.includes('state DOWN');
            const hasCarrier = !linkResult.includes('NO-CARRIER');
            const hasIP = addrResult.includes('inet ') && addrResult.includes('scope global');

            // 调试信息（仅在需要时启用）
            // console.log(`📊 状态 ${iface}: UP=${isUp}, 载波=${hasCarrier}, IP=${hasIP}`);

            return {
                interface: iface,
                isUp: isUp,
                hasCarrier: hasCarrier,
                hasIP: hasIP,
                timestamp: new Date().toISOString(),
                rawLink: linkResult,
                rawAddr: addrResult
            };
        } catch (error) {
            console.error(`❌ 获取状态失败 ${iface}:`, error.message);
            return {
                interface: iface,
                isUp: false,
                hasCarrier: false,
                hasIP: false,
                error: error.message,
                timestamp: new Date().toISOString()
            };
        }
    }

    /**
     * 检查状态是否变化
     */
    hasChanged(last, current) {
        if (!last || !current) return false;

        const changed = last.isUp !== current.isUp ||
            last.hasCarrier !== current.hasCarrier ||
            last.hasIP !== current.hasIP;

        // 状态变化检测（调试信息已简化）
        // if (changed) {
        //     console.log(`🔍 状态变化: ${last.isUp ? 'UP' : 'DOWN'} -> ${current.isUp ? 'UP' : 'DOWN'}`);
        // }

        return changed;
    }

    /**
     * 测试连通性
     */
    async ping(host = '8.8.8.8') {
        try {
            const result = await this.run(['ping', '-c', '3', host]);
            return result.includes('3 received') || result.includes('3 packets transmitted');
        } catch (error) {
            return false;
        }
    }

    /**
     * 执行命令 - 使用 system.cmdAsync
     */
    async run(args) {
        return new Promise((resolve, reject) => {
            let output = '';
            let errorOutput = '';
            let exitCode = -1;

            const callbacks = {
                onStdout: (data) => {
                    output += data + '\n';
                },
                onStderr: (data) => {
                    errorOutput += data + '\n';
                },
                onFinish: (exitData) => {
                    exitCode = exitData.exit_status;
                    if (exitCode === 0) {
                        resolve(output.trim());
                    } else {
                        reject(new Error(`Command failed with exit code ${exitCode}: ${errorOutput.trim()}`));
                    }
                },
                onError: (error, type) => {
                    reject(new Error(`${type} read error: ${error.message}`));
                }
            };

            system.cmdAsync(args, callbacks).catch(reject);
        });
    }

    /**
     * 子网掩码转换为CIDR格式
     * @param {string} netmask 子网掩码，支持 '255.255.255.0' 或 '24' 格式
     * @returns {string} CIDR格式的子网掩码
     */
    netmaskToCidr(netmask) {
        // 如果已经是数字格式（CIDR），直接返回
        if (/^\d+$/.test(netmask)) {
            return netmask;
        }

        // 如果是点分十进制格式，转换为CIDR
        if (/^\d+\.\d+\.\d+\.\d+$/.test(netmask)) {
            const parts = netmask.split('.').map(Number);
            let cidr = 0;

            for (let part of parts) {
                // 将每个部分转换为二进制并计算1的个数
                cidr += part.toString(2).split('1').length - 1;
            }

            return cidr.toString();
        }

        // 默认返回24
        return '24';
    }

    /**
     * 设置DNS到 /etc/resolv.conf
     * @param {string} dns1 主DNS服务器
     * @param {string} dns2 备用DNS服务器
     */
    async setDNS(dns1 = '8.8.8.8', dns2 = '223.5.5.5') {
        try {
            const resolvContent = `nameserver ${dns1}\nnameserver ${dns2}\n`;
            await this.run(['sh', '-c', `echo '${resolvContent}' > /etc/resolv.conf`]);
            console.log(`✅ DNS设置成功: ${dns1}, ${dns2}`);
        } catch (error) {
            console.error('❌ DNS设置失败:', error.message);
            throw error;
        }
    }

    /**
     * 延时
     */
    sleep(ms) {
        return new Promise(resolve => setTimeout(resolve, ms));
    }
}

// // 如果直接运行，执行示例
// if (import.meta.main) {
//     // 创建网络管理实例
//     const net = new QuickNetwork({
//         onStatusChange: (current, last) => {
//             console.log(`📡 状态变化: ${current.interface} ${last ? `${last.isUp ? 'UP' : 'DOWN'} -> ` : ''}${current.isUp ? 'UP' : 'DOWN'}`);
//         },
//         onConnect: (state) => {
//             console.log(`🔗 连接成功: ${state.interface} ${state.hasIP ? '(有IP)' : '(无IP)'}`);
//         },
//         onDisconnect: (state) => {
//             console.log(`🔌 连接断开: ${state.interface}`);
//         },
//         onError: (error, type) => {
//             console.error(`❌ 错误 [${type}]:`, error.message);
//         }
//     });
//     net.watch('eth0');
//     net.dhcp('eth0').then((res) => {
//         console.log("DHCP配置成功", res);
//         net.static('eth0', '192.168.10.100', '192.168.10.1', '255.255.255.0').then((res) => {
//             console.log("静态网络配置成功", res);
//             net.ping().then((res) => {
//                 console.log("ping成功", res);
//             });
//         });
//     });
//     setTimeout(() => {
//         net.stop();
//     }, 5000);
// }

export { QuickNetwork };