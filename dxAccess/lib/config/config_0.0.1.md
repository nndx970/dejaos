# 设备配置文档

## 版本说明

版本号采用三段式命名规则：
- **第一段**：分组变化 - 当配置项的分组结构发生变化时递增
- **第二段**：组员字段名变化 - 当某个分组内的字段名发生变化时递增  
- **第三段**：说明和值变化 - 当字段的说明描述或取值范围发生变化时递增

当前版本：0.0.1

注：配置项组员名称不能重复

## 设备基础配置

| 配置项 | 默认值 | 数据类型 | 数据范围 | 权限 | 说明 |
|--------|---------|----------|----------|------|------|
| `base.language` | "CN" | string | CN,EN | 读写 | 系统语言（CN/EN），不只是界面，还有语音 |
| `base.password` | "1" | string | 任意字符串，长度1-20 | 读写 | 管理员密码，用于进入后台设置菜单 |
| `base.firstLogin` | 0 | int | 0,1 | 只读 | 是否第一次登录后台 0未登录 1已登录 |
| `base.appMode` | 1 | int | 0,1 | 读写 | 应用模式 0:标准模式 1:简约模式 |
| `base.appVersion` | "" | string | 版本号格式，长度1-32 | 只读 | 固件版本号 |
| `base.releaseTime` | "" | string | 时间格式，长度1-32 | 只读 | 固件版本发布时间 |
| `base.devType` | 3 | int | 0,1,2,3 | 只读 | 设备类型 0:wifi版本 1:4G版本 2:以太网版本 3:未读取状态 |
| `base.restartCount` | 0 | int | 0-999999 | 只读 | 重启次数记录 |
| `base.volume` | 50 | int | 0-100 | 读写 | 音量(0-100) |
| `base.heartEn` | 0 | int | 0,1 | 读写 | 设备心跳开关 1开 0关 |
| `base.heartTime` | 30 | int | 10-300 | 读写 | 心跳时间(秒) |
| `base.heartChannel` | 0 | int | 0,1 | 读写 | 心跳上报通道，0：mqtt协议上报，1：http协议上报 |
| `base.userdata` | "" | string | json字符串数组 | 只读 | 用户资源列表 |

## 显示界面配置

| 配置项 | 默认值 | 数据类型 | 数据范围 | 权限 | 说明 |
|--------|---------|----------|----------|------|------|
| `ui.screenOff` | 0 | int | 0-86400 | 读写 | 熄屏时间（秒，0从不） |
| `ui.screensaver` | 0 | int | 0-86400 | 读写 | 屏幕保护（秒，0从不） |
| `ui.brightness` | 70 | int | 0-100 | 读写 | 屏幕亮度 |
| `ui.brightnessAuto` | 1 | int | 0,1 | 读写 | 自动调节亮度 1自动调节 |
| `ui.showIp` | 1 | int | 0,1 | 读写 | 显示IP地址 |
| `ui.showSn` | 1 | int | 0,1 | 读写 | SN显示 1显示 0隐藏 |
| `ui.showNir` | 1 | int | 0,1 | 读写 | 显示近红外图像 |
| `ui.showWXPro` | 1 | int | 0,1 | 读写 | 小程序码显示 1显示 0隐藏 |
| `ui.showIdCard` | 1 | int | 0,1 | 读写 | 显示身份证读卡功能菜单 1显示 0隐藏 |
| `ui.showLogo` | 0 | int | 0,1 | 读写 | 主页显示logo开关，配合showLogo使用 1显示 0隐藏 |
| `ui.logoImage` | "" | string | 资源名称，长度0-64 | 读写 | logo资源名称，可参考userdata配置内容 |
| `ui.showCRZ` | 0 | int | 0,1 | 读写 | 主页显示刷卡区域 1显示 0隐藏 |

## 网络通信配置

| 配置项 | 默认值 | 数据类型 | 数据范围 | 权限 | 说明 |
|--------|---------|----------|----------|------|------|
| `net.type` | 1 | int | 1,2,4 | 读写 | 网络类型 1:以太网，2：wifi，4：4g |
| `net.ssid` | "" | string | 任意字符串，长度1-32 | 读写 | WiFi SSID |
| `net.psk` | "" | string | 任意字符串，长度8-64 | 读写 | WiFi密码 |
| `net.ssidENC` | "" | string | 任意字符串，长度1-32 | 读写 | WiFi加密类型 |
| `net.dhcp` | 2 | int | 1,2 | 读写 | DHCP模式 1：静态，2：动态 |
| `net.ip` | "" | string | IPv4地址格式，长度7-15 | 读写 | 静态IP地址 |
| `net.gateway` | "" | string | IPv4地址格式，长度7-15 | 读写 | 网关地址 |
| `net.mask` | "" | string | IPv4地址格式，长度7-15 | 读写 | 子网掩码 |
| `net.dns` | "" | string | IPv4地址格式，长度7-15 | 读写 | DNS服务器 |
| `net.mac` | "" | string | MAC地址格式，长度17 | 读写 | MAC地址 |
| `net.ntp` | 1 | int | 0,1 | 读写 | NTP同步开关 0关闭，1定时同步 |
| `net.server` | "182.92.12.11" | string | IPv4地址格式，长度7-15 | 读写 | NTP服务器地址 |
| `net.hour` | 3 | int | 0-23 | 读写 | 定时同步时间（24小时制） |
| `net.gmt` | 8 | int | -12到+14 | 读写 | 时区 |

## MQTT通信配置

| 配置项 | 默认值 | 数据类型 | 数据范围 | 权限 | 说明 |
|--------|---------|----------|----------|------|------|
| `mqtt.addr` | "mqtt://192.168.10.166:1883" | string | tcp://, ssl://, mqtt://, mqtts://，长度1-128 | 读写 | MQTT服务器地址 |
| `mqtt.clientId` | "" | string | 任意字符串，长度1-23 | 读写 | MQTT客户端ID，MQTT协议要求1-23字符 |
| `mqtt.username` | "" | string | 任意字符串，长度0-12 | 读写 | MQTT用户名，建议不超过12字符 |
| `mqtt.password` | "" | string | 任意字符串，长度0-12 | 读写 | MQTT密码，建议不超过12字符 |
| `mqtt.qos` | 1 | int | 0,1,2 | 读写 | MQTT服务质量 |
| `mqtt.prefix` | "" | string | 任意字符串，长度0-64 | 读写 | MQTT主题前缀 |
| `mqtt.willTopic` | "access_device/v2/event/offline" | string | 任意字符串，长度1-64 | 读写 | 遗嘱主题 |
| `mqtt.cleanSession` | 0 | int | 0,1 | 读写 | 清理会话 |

## 人脸识别配置

| 配置项 | 默认值 | 数据类型 | 数据范围 | 权限 | 说明 |
|--------|---------|----------|----------|------|------|
| `face.similarity` | 0.6 | float | 0.0-1.0 | 读写 | 人脸识别相似度 |
| `face.livenessOff` | 1 | int | 0,1 | 读写 | 活体检测开关 |
| `face.livenessVal` | 10 | int | 0-100 | 读写 | 活体检测阈值 |
| `face.detectMask` | 0 | int | 0,1 | 读写 | 口罩检测 |
| `face.stranger` | 1 | int | 0,1,2,3 | 读写 | 陌生人语音模式 （0无 1请先注册 2通行失败 3自定义） |
| `face.voiceMode` | 1 | int | 0,1,2,3 | 读写 | 认证成功语音模式 （0无 1姓名 2自定义 3请通行），当为2时，使用voiceSucCum配置内容播报 |
| `face.voiceSucCum` | "欢迎光临" | string | 任意字符串，长度1-64 | 读写 | 语音自定义内容 |

## 门禁控制配置

| 配置项 | 默认值 | 数据类型 | 数据范围 | 权限 | 说明 |
|--------|---------|----------|----------|------|------|
| `access.offlineAcsNum` | 2000 | int | 100-10000 | 读写 | 通行记录存储上限（条） |
| `access.reportTime` | 5 | int | 2-60 | 读写 | 通行记录上报间隔（秒） |
| `access.tamperAlarm` | 0 | int | 0,1 | 读写 | 报警开关 1开 0关 |
| `access.relayTime` | 2 | int | 2-30 | 读写 | 继电器吸合时长(s) |
| `access.onlinecheck` | 0 | int | 0,1 | 读写 | 在线验证开关 1 先走离线失败走在线验证 0 只走离线 ，默认0 |
| `access.timeout` | 5000 | int | 1000-60000 | 读写 | 在线验证超时时间(ms) |
| `access.showPwd` | 1 | int | 0,1 | 读写 | 密码开门开关 1开 0关 |
| `access.nfc` | 1 | int | 0,1 | 读写 | NFC刷卡开关 1开 0关 |
| `access.Idcard` | 1 | int | 1,2,3 | 读写 | 身份证模式(1物理卡号 2云证获取 3身份证模块) |
| `access.strangerImg` | 1 | int | 0,1 | 读写 | 陌生人通行记录图片保存开关（0不保存 1保存） |
| `access.accessImageType` | 1 | int | 0,1 | 读写 | 通行图片类型 1人脸 0全景 |

## 系统标识配置

| 配置项 | 默认值 | 数据类型 | 数据范围 | 权限 | 说明 |
|--------|---------|----------|----------|------|------|
| `sys.devmac` | "" | string | MAC地址格式 | 只读 | 设备主MAC地址 |
| `sys.uuid` | "" | string | UUID格式，长度6-32 | 只读 | UUID |