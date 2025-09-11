// #!/usr/bin/env tjs

// /**
//  * 快速网络管理工具
//  * 简化的网络配置和监听功能
//  */

// class QuickNetwork {
//     constructor() {
//         this.monitoring = false;
//         this.callbacks = [];
//         this.interval = null;
//     }

//     /**
//      * 快速配置动态网络
//      */
//     async dhcp(iface = 'eth0') {
//         console.log(`🚀 快速配置动态网络: ${iface}`);
        
//         try {
//             // 清空配置
//             await this.run(['ip', 'address', 'flush', 'dev', iface], true);
//             await this.run(['ip', 'link', 'set', iface, 'up'], true);
            
//             // 运行DHCP
//             const proc = tjs.spawn(['busybox', 'udhcpc', '-q', '-i', iface]);
//             const status = await proc.wait();
            
//             if (status.exit_status === 0) {
//                 console.log('✅ 动态IP获取成功');
//                 return true;
//             } else {
//                 console.log('❌ 动态IP获取失败');
//                 return false;
//             }
//         } catch (error) {
//             console.error('❌ DHCP配置失败:', error.message);
//             return false;
//         }
//     }

//     /**
//      * 快速配置静态网络
//      */
//     async static(iface = 'eth0', ip = '192.168.1.100', gateway = '192.168.1.1') {
//         console.log(`🔧 快速配置静态网络: ${iface} -> ${ip}`);
        
//         try {
//             // 清空配置
//             await this.run(['ip', 'address', 'flush', 'dev', iface], true);
            
//             // 启用接口
//             await this.run(['ip', 'link', 'set', iface, 'up'], true);
//             await this.sleep(1000);
            
//             // 设置IP
//             await this.run(['ip', 'address', 'add', `${ip}/24`, 'dev', iface], true);
            
//             // 设置网关
//             await this.run(['ip', 'route', 'add', 'default', 'via', gateway], true);
            
//             console.log('✅ 静态网络配置成功');
//             return true;
//         } catch (error) {
//             console.error('❌ 静态网络配置失败:', error.message);
//             return false;
//         }
//     }

//     /**
//      * 开始监听网络状态
//      */
//     async watch(iface = 'eth0', callback = null) {
//         if (this.monitoring) {
//             console.log('⚠️  网络监听已在运行');
//             return;
//         }

//         console.log(`👁️  开始监听网络接口: ${iface}`);
        
//         if (callback) {
//             this.callbacks.push(callback);
//         }

//         this.monitoring = true;
//         let lastState = null;

//         this.interval = setInterval(async () => {
//             try {
//                 const currentState = await this.getState(iface);
                
//                 if (lastState && this.hasChanged(lastState, currentState)) {
//                     console.log(`🔔 网络状态变化: ${iface}`);
//                     console.log(`   状态: ${currentState.isUp ? 'UP' : 'DOWN'}`);
//                     console.log(`   载波: ${currentState.hasCarrier ? '有' : '无'}`);
//                     console.log(`   IP: ${currentState.hasIP ? '已获取' : '未获取'}`);
                    
//                     // 触发回调
//                     this.callbacks.forEach(cb => {
//                         try {
//                             cb(currentState, lastState);
//                         } catch (error) {
//                             console.error('回调执行失败:', error.message);
//                         }
//                     });
//                 }
                
//                 lastState = currentState;
//             } catch (error) {
//                 console.error('状态检查失败:', error.message);
//             }
//         }, 3000);

//         console.log('✅ 网络监听已启动');
//     }

//     /**
//      * 停止监听
//      */
//     stop() {
//         if (this.interval) {
//             clearInterval(this.interval);
//             this.interval = null;
//         }
//         this.monitoring = false;
//         console.log('✅ 网络监听已停止');
//     }

//     /**
//      * 获取网络状态
//      */
//     async getState(iface) {
//         try {
//             const linkResult = await this.run(['ip', 'link', 'show', iface], true);
//             const addrResult = await this.run(['ip', 'addr', 'show', iface], true);
            
//             // 更精确的状态解析
//             const isUp = linkResult.includes('state UP') && !linkResult.includes('state DOWN');
//             const hasCarrier = !linkResult.includes('NO-CARRIER');
//             const hasIP = addrResult.includes('inet ') && addrResult.includes('scope global');
            
//             // 调试：显示原始数据的关键部分
//             console.log(`🔍 原始数据解析:`);
//             console.log(`   link结果包含 'state UP': ${linkResult.includes('state UP')}`);
//             console.log(`   link结果包含 'state DOWN': ${linkResult.includes('state DOWN')}`);
//             console.log(`   link结果包含 'NO-CARRIER': ${linkResult.includes('NO-CARRIER')}`);
//             console.log(`   addr结果包含 'inet ': ${addrResult.includes('inet ')}`);
//             console.log(`   addr结果包含 'scope global': ${addrResult.includes('scope global')}`);
            
//             // 调试信息
//             console.log(`📊 获取状态 ${iface}: UP=${isUp}, 载波=${hasCarrier}, IP=${hasIP}`);
            
//             return {
//                 interface: iface,
//                 isUp: isUp,
//                 hasCarrier: hasCarrier,
//                 hasIP: hasIP,
//                 timestamp: new Date().toISOString(),
//                 rawLink: linkResult,
//                 rawAddr: addrResult
//             };
//         } catch (error) {
//             console.error(`❌ 获取状态失败 ${iface}:`, error.message);
//             return {
//                 interface: iface,
//                 error: error.message,
//                 timestamp: new Date().toISOString()
//             };
//         }
//     }

//     /**
//      * 检查状态是否变化
//      */
//     hasChanged(last, current) {
//         if (!last || !current) return false;
        
//         const changed = last.isUp !== current.isUp ||
//                        last.hasCarrier !== current.hasCarrier ||
//                        last.hasIP !== current.hasIP;
        
//         if (changed) {
//             console.log(`🔍 状态变化检测:`);
//             console.log(`   上次状态: UP=${last.isUp}, 载波=${last.hasCarrier}, IP=${last.hasIP}`);
//             console.log(`   当前状态: UP=${current.isUp}, 载波=${current.hasCarrier}, IP=${current.hasIP}`);
//         }
        
//         return changed;
//     }

//     /**
//      * 测试连通性
//      */
//     async ping(host = '8.8.8.8') {
//         try {
//             const result = await this.run(['ping', '-c', '3', host], true);
//             return result.includes('3 received');
//         } catch (error) {
//             return false;
//         }
//     }

//     /**
//      * 执行命令
//      */
//     async run(args, silent = false) {
//         return new Promise((resolve, reject) => {
//             try {
//                 const proc = tjs.spawn(args);
//                 let stdout = '';
//                 let stderr = '';
                
//                 // 监听标准输出
//                 if (proc.stdout) {
//                     proc.stdout.on('data', data => {
//                         stdout += data;
//                     });
//                 }
                
//                 // 监听标准错误输出
//                 if (proc.stderr) {
//                     proc.stderr.on('data', data => {
//                         stderr += data;
//                     });
//                 }
                
//                 proc.wait().then((status) => {
//                     // 优先返回 stdout，如果为空则返回 stderr
//                     const output = stdout.trim() || stderr.trim();
                    
//                     // 只在非静默模式或出错时显示调试信息
//                     if (!silent || status.exit_status !== 0 || output.length > 0) {
//                         console.log(`🔍 命令执行: ${args.join(' ')}`);
//                         console.log(`   退出状态: ${status.exit_status}`);
//                         console.log(`   stdout长度: ${stdout.length}`);
//                         console.log(`   stderr长度: ${stderr.length}`);
//                         console.log(`   最终输出长度: ${output.length}`);
                        
//                         if (status.exit_status !== 0) {
//                             console.log(`   ⚠️  命令执行失败，退出码: ${status.exit_status}`);
//                         }
//                     }
                    
//                     // 如果命令失败，抛出错误
//                     if (status.exit_status !== 0) {
//                         const errorMsg = output || `命令执行失败，退出码: ${status.exit_status}`;
//                         reject(new Error(errorMsg));
//                     } else {
//                         resolve(output);
//                     }
//                 }).catch(reject);
                
//             } catch (error) {
//                 reject(error);
//             }
//         });
//     }

//     /**
//      * 延时
//      */
//     sleep(ms) {
//         return new Promise(resolve => setTimeout(resolve, ms));
//     }
// }

// // 如果直接运行，执行示例
// if (import.meta.main) {
//     const net = new QuickNetwork();
    
//     console.log('=== 快速网络管理工具 ===\n');
    
//     // 注册状态变化回调
//     net.callbacks.push((current, last) => {
//         console.log(`\n📡 网络状态变化通知:`);
//         console.log(`   接口: ${current.interface}`);
//         console.log(`   时间: ${current.timestamp}`);
//         if (last) {
//             console.log(`   变化: ${last.isUp ? 'UP' : 'DOWN'} -> ${current.isUp ? 'UP' : 'DOWN'}`);
//         }
//     });
    
//     async function demo() {
//         // 1. 配置动态网络
//         console.log('1. 配置动态网络...');
//         await net.dhcp('eth0');
//         await net.sleep(2000);
        
//         // 2. 测试连通性
//         console.log('\n2. 测试网络连通性...');
//         const canPing = await net.ping('8.8.8.8');
//         console.log(`连通性测试: ${canPing ? '✅ 正常' : '❌ 异常'}`);
        
//         // 3. 开始监听
//         console.log('\n3. 开始网络状态监听...');
//         await net.watch('eth0');
        
//         // 运行20秒
//         setTimeout(() => {
//             net.stop();
//             console.log('\n=== 演示完成 ===');
//         }, 20000);
//     }
    
//     demo().catch(console.error);
// }

// export { QuickNetwork };
