#include "./mongoose.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <pthread.h>

struct mg_mgr *mgr = NULL;
static int input_thread_running = 1;               // 控制输入线程运行状态
static struct mg_connection *ws_connection = NULL; // WebSocket连接

// 输入处理线程函数
void *input_thread_func(void *arg)
{
    (void)arg; // Suppress unused parameter warning
    char buffer[1024];

    printf("输入线程已启动，等待输入数据...\n");

    while (input_thread_running)
    {
        if (fgets(buffer, sizeof(buffer), stdin) != NULL)
        {
            // 处理接收到的数据
            // printf("Received: %s", buffer);

            // 发送到WebSocket客户端
            if (ws_connection != NULL && ws_connection->is_websocket)
            {
                // 移除换行符并发送
                size_t len = strlen(buffer);
                if (len > 0 && buffer[len - 1] == '\n')
                {
                    buffer[len - 1] = '\0';
                }
                mg_ws_send(ws_connection, buffer, strlen(buffer), WEBSOCKET_OP_TEXT);
            }

            // 可以在这里添加更多的数据处理逻辑
            // 例如：解析命令、发送响应等
        }
    }

    printf("输入线程已退出\n");
    return NULL;
}

// HTTP请求处理函数
static void fn(struct mg_connection *c, int ev, void *ev_data)
{
    if (ev == MG_EV_HTTP_MSG)
    {
        struct mg_http_message *hm = (struct mg_http_message *)ev_data;
        printf("uri: %s\n", hm->uri.buf);

        // 处理WebSocket升级请求
        if (mg_match(hm->uri, mg_str("/ws"), NULL))
        {
            mg_ws_upgrade(c, hm, NULL);
            ws_connection = c; // 保存WebSocket连接
            printf("WebSocket连接已建立\n");
            return;
        }
        else if (mg_match(hm->uri, mg_str("/api/upload"), NULL))
        {
            //  * 请求格式：/upload?file=firmware.bin&offset=2048，file是保存文件名，offset是偏移字节
            // * 优点：可以断点续传，缺点：暂无
            mkdir("/data/upgrade/", 0777);
            long result = mg_http_upload(c, hm, &mg_fs_posix, "/data/upgrade", MG_MAX_RECV_SIZE);

            // 检查上传是否成功完成
            if (result > 0)
            {
                printf("Upload completed successfully\n");
            }
        }
        else if (mg_match(hm->uri, mg_str("/api/updateApp"), NULL))
        {
            printf("收到重启程序请求\n");

            // 返回成功响应
            mg_http_reply(c, 200, "Content-Type: application/json\r\n",
                          "{\"message\": \"程序重启请求已接收\"}\n");

            system("thread_pid=$(pgrep -f \"lvgljs\") && [ -n \"$thread_pid\" ] && echo \"Killing thread with PID: $thread_pid\" && kill -9 $thread_pid || echo \"Thread not found.\"");
            system("[ -f /data/upgrade/app.zip ] && rm -rf /app/* && unzip -o /data/upgrade/app.zip -d /app && rm -rf /data/upgrade/app.zip");
            system("lvgljs run /app/index.js > /tmp/program_pipe &");
        }
        else
        {
            struct mg_http_serve_opts opts = {.root_dir = "/os/webserver/src/"};
            mg_http_serve_dir(c, hm, &opts);
        }
    }
    else if (ev == MG_EV_WS_MSG)
    {
        // 处理WebSocket消息
        struct mg_ws_message *wm = (struct mg_ws_message *)ev_data;
        printf("WebSocket消息: %.*s\n", (int)wm->data.len, wm->data.buf);
    }
    else if (ev == MG_EV_CLOSE)
    {
        // 连接关闭
        if (c == ws_connection)
        {
            ws_connection = NULL;
            printf("WebSocket连接已关闭\n");
        }
    }
}

int main(int argc, char *argv[])
{
    (void)argc; // Suppress unused parameter warning
    (void)argv; // Suppress unused parameter warning
    
    const char *s_http_addr = "http://0.0.0.0:8080"; // 监听所有接口的8080端口

    mgr = (struct mg_mgr *)malloc(sizeof(struct mg_mgr));
    mg_mgr_init(mgr);

    printf("启动 Mongoose Web Server...\n");
    printf("访问地址: %s\n", s_http_addr);
    printf("按 Ctrl+C 停止服务器\n");

    // 创建HTTP监听器
    struct mg_connection *c = mg_http_listen(mgr, s_http_addr, fn, NULL);
    if (c == NULL)
    {
        printf("无法启动服务器，端口可能被占用\n");
        free(mgr);
        return 1;
    }

    // 创建输入处理线程
    pthread_t input_thread;
    if (pthread_create(&input_thread, NULL, input_thread_func, NULL) != 0)
    {
        printf("无法创建输入线程\n");
        mg_mgr_free(mgr);
        free(mgr);
        return 1;
    }
    // 事件循环
    while (true)
    {
        mg_mgr_poll(mgr, 1000); // 1秒超时
    }

    // 停止输入线程
    input_thread_running = 0;

    // 等待输入线程结束
    pthread_join(input_thread, NULL);

    mg_mgr_free(mgr);
    free(mgr);
    return 0;
}