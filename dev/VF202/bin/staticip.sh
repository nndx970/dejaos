# 网络配置参数
INTERFACE="eth0"
IP_ADDRESS="192.168.10.100"
NETMASK="/24"
GATEWAY="192.168.10.1"
DNS1="8.8.8.8"
DNS2="223.5.5.5"

echo "开始配置网络..."

# 网络诊断
echo "=== 网络诊断 ==="
echo "检查网络接口..."
ip link show

echo "检查当前网络配置..."
ip address show
ip route show

echo "检查网络服务状态..."
systemctl status networking 2>/dev/null || echo "networking服务未运行"
systemctl status NetworkManager 2>/dev/null || echo "NetworkManager服务未运行"

echo "=== 开始网络配置 ==="

# 0. 清空现有网络配置
echo "清空现有网络配置..."

# 检查接口是否存在
if ip link show $INTERFACE > /dev/null 2>&1; then
    echo "清空 $INTERFACE 接口的所有IP地址"
    ip address flush dev $INTERFACE
    
    echo "清空 $INTERFACE 接口的所有路由"
    ip route flush dev $INTERFACE
else
    echo "警告: 接口 $INTERFACE 不存在"
fi

echo "清空所有默认路由"
ip route flush default

# 清空ARP表
echo "清空ARP表"
ip neighbor flush all

echo "网络配置清理完成"

# 1. 启用网络接口
echo "启用网络接口 $INTERFACE"

# 先关闭接口，然后重新启用
ip link set $INTERFACE down
sleep 2
ip link set $INTERFACE up
sleep 2

# 检查接口状态
echo "检查接口状态..."
ip link show $INTERFACE

# 2. 配置IP地址
echo "配置IP地址 $IP_ADDRESS$NETMASK"
ip address add $IP_ADDRESS$NETMASK dev $INTERFACE

# 等待接口稳定
sleep 2

# 3. 设置默认网关
echo "设置默认网关 $GATEWAY"
ip route add default via $GATEWAY

# 4. 配置DNS
echo "配置DNS服务器"
echo "nameserver $DNS1" > /etc/resolv.conf
echo "nameserver $DNS2" >> /etc/resolv.conf

# 5. 验证配置
echo "验证网络配置..."
echo "IP地址配置："
ip address show $INTERFACE

echo "路由表："
ip route show

echo "测试网络连接..."

# 检查接口物理连接状态
echo "检查物理连接状态..."
if ip link show $INTERFACE | grep -q "NO-CARRIER"; then
    echo "⚠ 警告: 检测到NO-CARRIER，请检查网线连接"
    echo "建议检查："
    echo "1. 网线是否插好"
    echo "2. 交换机/路由器是否正常工作"
    echo "3. 网卡驱动是否正常"
fi

# 检查接口状态
if ip link show $INTERFACE | grep -q "state UP"; then
    echo "✓ 接口状态正常"
else
    echo "✗ 接口状态异常，尝试重新启用..."
    ip link set $INTERFACE down
    sleep 1
    ip link set $INTERFACE up
    sleep 2
fi

# 测试网关连接
echo "测试网关连接 $GATEWAY..."
if ping -c 3 $GATEWAY > /dev/null 2>&1; then
    echo "✓ 网关连接正常"
else
    echo "✗ 网关连接失败"
    echo "尝试诊断网关问题..."
    
    # 检查ARP表
    echo "检查ARP表..."
    ip neighbor show dev $INTERFACE
    
    # 尝试ping网关并显示详细信息
    echo "详细ping测试..."
    ping -c 2 $GATEWAY
fi

# 测试DNS连接
echo "测试DNS服务器连接 $DNS1..."
if ping -c 3 $DNS1 > /dev/null 2>&1; then
    echo "✓ DNS服务器连接正常"
else
    echo "✗ DNS服务器连接失败"
fi

# 测试DNS解析
echo "测试DNS解析..."
if nslookup google.com > /dev/null 2>&1; then
    echo "✓ DNS解析正常"
else
    echo "✗ DNS解析失败"
fi

echo "网络配置完成！"