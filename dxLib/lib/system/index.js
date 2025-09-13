/**
 * 执行命令行参数
 * @param {*} args 命令行参数 如 ['ping', '-c', '3', '8.8.8.8']
 * @param {*} callbacks 回调函数 如 { onStdout: (data) => console.log(`[STDOUT] ${data}`), onStderr: (data) => console.log(`[STDERR] ${data}`), onFinish: (exitData) => console.log("Process finished! Exit code:", exitData.exit_status), onError: (error, type) => console.log(`${type} read finished`) }
 */
export async function cmdAsync(args, callbacks = {}) {
    const proc = tjs.spawn(args, { stdout: 'pipe', stderr: 'pipe' })
    // 创建缓冲区用于读取输出
    const stdoutBuf = new Uint8Array(4096);
    const stderrBuf = new Uint8Array(4096);
    const decoder = new TextDecoder();

    // 默认回调函数
    const defaultCallbacks = {
        onStdout: (data) => console.log(`[STDOUT] ${data}`),
        onStderr: (data) => console.log(`[STDERR] ${data}`),
        onFinish: (exitData) => console.log("Process finished! Exit code:", exitData.exit_status),
        onError: (error, type) => console.log(`${type} read finished`)
    };

    // 合并用户提供的回调
    const finalCallbacks = { ...defaultCallbacks, ...callbacks };

    // 异步读取标准输出
    (async () => {
        try {
            while (true) {
                const nread = await proc.stdout.read(stdoutBuf);
                if (nread === null) break;
                const output = decoder.decode(stdoutBuf.subarray(0, nread)).trim();
                if (output) {
                    finalCallbacks.onStdout(output);
                }
            }
        } catch (error) {
            finalCallbacks.onError(error, 'stdout');
        }
    })();

    // 异步读取错误输出
    (async () => {
        try {
            while (true) {
                const nread = await proc.stderr.read(stderrBuf);
                if (nread === null) break;
                const error = decoder.decode(stderrBuf.subarray(0, nread)).trim();
                if (error) {
                    finalCallbacks.onStderr(error);
                }
            }
        } catch (error) {
            finalCallbacks.onError(error, 'stderr');
        }
    })();

    // 等待进程结束并返回退出码
    const exitData = await proc.wait();
    finalCallbacks.onFinish(exitData);
}

// async function test() {
//     // 使用自定义回调
//     await system.cmdAsync(['ping', '-c', '3', '8.8.8.8'], {
//         onStdout: (data) => {
//             console.log('收到标准输出:', data);
//         },
//         onStderr: (data) => {
//             console.log('收到错误输出:', data);
//         },
//         onFinish: (exitData) => {
//             console.log('进程完成，退出码:', exitData.exit_status);
//         },
//         onError: (error, type) => {
//             console.log(`${type} 读取出错:`, error.message);
//         }
//     });
// }

// test();