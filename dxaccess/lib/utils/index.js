async function downloadFile(url, filePath, connectTimeout = 30, maxTime = 300) {
    if (!url || !filePath) {
        console.error("url和filePath不能为空");
        return -1;
    }
    console.log("开始下载...");

    const proc = tjs.spawn(['curl', '-L', '-o', filePath, url, '--connect-timeout', connectTimeout, '--max-time', maxTime], { stdout: 'pipe', stderr: 'pipe' })

    console.log("curl进程已启动，PID:", proc.pid, url, filePath);

    // 创建缓冲区用于读取输出
    const stdoutBuf = new Uint8Array(4096);
    const stderrBuf = new Uint8Array(4096);
    const decoder = new TextDecoder();

    // 异步读取标准输出
    (async () => {
        try {
            while (true) {
                const nread = await proc.stdout.read(stdoutBuf);
                if (nread === null) break;
                const output = decoder.decode(stdoutBuf.subarray(0, nread)).trim();
                if (output) {
                    console.log(`[STDOUT] ${output}`);
                }
            }
        } catch (error) {
            console.log("stdout读取结束");
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
                    console.log(`[STDERR] ${error}`);
                }
            }
        } catch (error) {
            console.log("stderr读取结束");
        }
    })();

    console.log("异步下载已启动，继续执行其他任务...");

    // 等待进程结束并返回退出码
    const exitData = await proc.wait();
    console.log("下载结束！退出码:", exitData);

    return exitData;
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

export { saveToFile };

export { downloadFile };