# IntCC
## · P4基础
### 1. 编译p4程序
```
拷贝program_name.p4到工作路径中（工作路径在~/bf-sde-8.9.1/下即可，p4_16示例程序在~/bf-sde-8.9.1/pkgsrc/p4-examples/p4_16_programs）
cd pkgsrc/p4-build
./autogen.sh
./configure --prefix=$SDE_INSTALL --with-tofino P4_NAME=portswitch P4_PATH=~/bf-sde-8.9.1/pkgsrc/p4-examples/p4_16_programs/portswitch/portswitch.p4 P4_VERSION=p4-16 P4C=p4c --enable-thrift P4_ARCHITECTURE=tna
make
make install
~/bf-sde-8.9.1/run_switchd.sh -p portswitch
```
### 2. 配置P4交换机
#### 1) 配置端口
```
pm port-add 1/0 10G NONE
pm port-enb 1/0 
```
#### 2) 下发流表
```
bfrt.portswitch.Ingress.set_out_port.add_with_set_egress_port(ingress_port=157, port=156)
bfrt.portswitch.Ingress.set_out_port.add_with_set_egress_port(ingress_port=156, port=157)
```
#### 3) 获取P4中的register数据
```
下发register流表： bfrt.portswitch.Egress.reg_match.add_with_register_action(156,   1)
同步register数据：bfrt.portswitch.Egress.test_reg.operation_register_sync()
查看register流表：bfrt.portswitch.Egress.test_reg.dump
```

#### 3) 关掉正在运行的P4程序
找到并kill掉正在运行的bf_switched：ps -ef | grep bf_switch

#### 4) p4核心代码
tna.p4和tofino.p4：install/share/p4c/p4include/tna.p4

## · 内核编译基础
### 1. 内核源码安装
#### 1） 搜索内核已有版本
```
sudo apt-cache search linux-source 
```
#### 2) 下载特定内核版本
```
sudo apt-get install linux-source-5.0.21
```
#### 3) 解压缩
```
sudo tar -axvf linux-source-5.0.0.tar.bz2
```
#### 4) 修改文件夹的权限为可读可写
```
sudo chmod -R 777 文件夹名
```

### 2. 内核编译
#### 1) 安装包
```
sudo apt-get install libncurses5-dev libssl-dev
sudo apt-get install build-essential openssl
sudo apt-get install zlibc minizip
sudo apt-get install libidn11-dev libidn11
sudo apt-get install libelf-dev libc6-dev
sudo apt-get install bison
sudo apt-get install flex
```

#### 2) 编译
```
sudo make mrproper
sudo make clean
sudo cp /boot/config-$(uname -r) .config
sudo make menuconfig
sudo make -j8
sudo make bzImage
sudo make modules
sudo make modules_install
sudo make install
sudo update-grub
sudo reboot
```
```
sudo make -j8 报错：No rule to make target 'debian/canonical-certs.pem', needed by 'certs/x509_certificate_list'
在.config中将CONFIG_SYSTEM_TRUSTED_KEYS="debian/canonical-certs.pem"修改为
CONFIG_SYSTEM_TRUSTED_KEYS=""
```
#### 3) 重启服务器
```
· 重启后按shift
· 根据系统提示操作，选择ubuntu开机
· 选择advanced options for ubuntu, 选择镜像，不操作则默认最新镜像
```

#### 4） 需要删除旧文件时
·  删除/lib/modules/下对应的内核版本 
·  删除/usr/src/对应的内核版本中已经解压后的文件夹
·  删除/boot/对应的内核版本

## · 其他有关配置和包安装
### 1. 商用交换机的配置
1. win + r 输入mstsc
2. 登录ip : 192.168.67.113, 账号和密码 : pcl
3. 进入配置模式 : con t
4. 进入端口: interface eth-0-2 (共有两个端口interface eth-0-1), show this 可以看到当前端口
5. 调整速率 : qos shape rate pir 10000000 (最高10G)
6. 若串口改变，telnet 192.168.109.160 账号: controller, 密码: pcl@os.com123

### 2. 安装收发包工具D-ITG
1. 官方文档及安装包下载: 
http://www.grid.unina.it/software/ITG/manual/index.html#par:instr
2. 安装: cd D-ITG/src, make
3. 发包：
   i. 接收端：sudo ./ITGRecv -l recv_log
   ii. 发送端: sudo ./ITGSend -T TCP -a 192.168.2.4 -rp 8000 -c 1500 -C 1000000 -t 10000 -z 1000000 -l send_log
   iii. 解析log: sudo ./ITGDec send_log
   iv. 将log写入到文件: sudo ./ITGDec send_log -f t -c 50 log.txt

### 3. 安装mahimahi
#### 直接安装
```
sudo apt-get install mahimahi
```
#### 从源代码安装
```
sudo apt install protobuf-compiler
sudo apt install libprotobuf-dev
sudo pip install protobuf
sudo apt install xcb
sudo apt install libxcb*
sudo apt install apache2
sudo apt-get install apache2-dev

git clone https://github.com/ravinet/mahimahi
cd mahimahi
bash autogen.sh
./configure
make
sudo make install
```

#### 使用mahimahi设置网络环境
```
mm-delay 20 mm-loss uplink 0.1 mm-link --meter-all trace.txt (20ms)
```

## · 修改端侧协议栈
IntCC需要在协议栈里添加新的TCP option, 其中新的option里面包括三个变量是txbytes, timestamp, qlen。其中server1为发送端, server3为接收端，在server1和server3我们都添加对新option的定义，但是只有在server1里面我们解析新的option

### 1. 添加新option的系统命令
```
diff --include/net/netns/ipv4.h  server1 and server3
struct netns_ipv4 {
 	struct local_ports ip_local_ports;
 
 	int sysctl_tcp_ecn;
+	int sysctl_tcp_qlen;
 	int sysctl_ip_no_pmtu_disc;
 	int sysctl_ip_fwd_use_pmtu;
 	int sysctl_ip_nonlocal_bind;
```

### 2. 新的系统命令的初始化
```
diff --net/ipv4/tcp_ipv4.c  server1 and server3
static int __net_init tcp_sk_init(struct net *net)
 		*per_cpu_ptr(net->ipv4.tcp_sk, cpu) = sk;
 	}
 	net->ipv4.sysctl_tcp_ecn = 2;
+	net->ipv4.sysctl_tcp_qlen = 0;
 	return 0;
```

### 3. 新的系统命令的实现
```
diff --net/ipv4/sysctl_net_ipv4.c  server1 and server3
static struct ctl_table ipv4_net_table[] = {
 		.proc_handler	= proc_dointvec
 	},
+ 	{
+		.procname	= "tcp_qlen",
+		.data		= &init_net.ipv4.sysctl_tcp_qlen,
+		.maxlen		= sizeof(int),
+		.mode		= 0644,
+		.proc_handler	= proc_dointvec
+	},
	{
 		.procname	= "ip_local_port_range",
 		.maxlen		= sizeof(init_net.ipv4.ip_local_ports.range),
 		.data		= &init_net.ipv4.ip_local_ports.range,
```
### 4. 定义新option的大小和类型
```
//添加新option server1 and server3
void tcp_time_wait(struct sock *sk, int state, int timeo);
 #define TCPOPT_WINDOW		3	/* Window scaling */
 #define TCPOPT_SACK_PERM        4       /* SACK Permitted */
 #define TCPOPT_SACK             5       /* SACK Block */
+#define TCPOPT_QLEN				10		
 #define TCPOPT_TIMESTAMP	8	/* Better RTT estimations/PAWS */
 #define TCPOPT_MD5SIG		19	/* MD5 Signature (RFC2385) */
 #define TCPOPT_EXP		254	/* Experimental */
@@ -189,6 +190,7 @@ void tcp_time_wait(struct sock *sk, int state, int timeo);
 #define TCPOLEN_TIMESTAMP      10
 #define TCPOLEN_MD5SIG         18
 #define TCPOLEN_EXP_FASTOPEN_BASE  4
+#define TCPOLEN_QLEN				    14//这个才是实际的选项的大小，下面那个大小是为了对齐

 
 /* But this is what stacks really send out. */  server1 and server3
 #define TCPOLEN_TSTAMP_ALIGNED		12
@@ -199,6 +201,7 @@ void tcp_time_wait(struct sock *sk, int state, int timeo);
 #define TCPOLEN_SACK_PERBLOCK		8
 #define TCPOLEN_MD5SIG_ALIGNED		20
 #define TCPOLEN_MSS_ALIGNED		4
+#define TCPOLEN_QLEN_ALIGNED			16//虽然我们设计的这个选项大小为6，但是这里并不是选项大小，而是对齐之后的大小
```

### 5. 初始化新的option
```
diff --net/ipv4/tcp_output.c  server1 and server3
static inline bool tcp_urg_mode(const struct tcp_sock *tp)
 #define OPTION_TS		(1 << 1)
 #define OPTION_MD5		(1 << 2)
 #define OPTION_WSCALE		(1 << 3)
//这个标志是为了做实验用的，目的是让所有的TCP包把当前的
//发送窗口通过TCP options发送出去，这样中间网络设备可以得知
//源点的发送窗口，从而在发生拥塞的时候方便抑制源点
+#define OPTION_QLEN			(1<<4)
 #define OPTION_FAST_OPEN_COOKIE	(1 << 8)
 
struct tcp_out_options {
 	u8 ws;			/* window scale, 0 to disable */
 	u8 num_sack_blocks;	/* number of SACK blocks to include */
 	u8 hash_size;		/* bytes in hash_location */
+	u32 qlen;
+	u32 timedelta;
+	u32 txbytes;
 	__u8 *hash_location;	/* temporary pointer, overloaded */
 	__u32 tsval, tsecr;	/* need to include OPTION_TS */
 	struct tcp_fastopen_cookie *fastopen_cookie;	/* Fast open cookie */

static void tcp_options_write(__be32 *ptr, struct tcp_sock *tp,
 			       struct tcp_out_options *opts){
+if(likely(OPTION_QLEN & options)){
+		*ptr++ = htonl((TCPOPT_NOP << 24) |
+				       (TCPOPT_NOP << 16) |
+				       (TCPOPT_QLEN << 8) |
+				       (TCPOLEN_QLEN));
+		*ptr++ = htonl(opts->qlen);
+		*ptr++ = htonl(opts->timedelta);
+		*ptr++ = htonl(opts->txbytes);
+	}


 	if (unlikely(opts->num_sack_blocks)) {
 		struct tcp_sack_block *sp = tp->rx_opt.dsack ?
 			tp->duplicate_sack : tp->selective_acks;
@@ -533,6 +547,7 @@ static void tcp_options_write(__be32 *ptr, struct tcp_sock *tp,
 		}
 		ptr += (foc->len + 3) >> 2;
 	}
 }
 
static unsigned int tcp_established_options(struct sock *sk, struct sk_buff *skb
 			opts->num_sack_blocks * TCPOLEN_SACK_PERBLOCK）{
	//注意这个函数是给已经建立起来连接的包设置options的
+	if(sock_net(sk)->ipv4.sysctl_tcp_qlen==1){
+			opts->options|=OPTION_QLEN;
+			opts->qlen=tp->snd_cwnd;
+			opts->timedelta=tp->snd_cwnd;
+			opts->txbytes=tp->snd_cwnd;
+			size += TCPOLEN_QLEN_ALIGNED;
+		}

 	return size;
 }
```

### 6. 解析新的option : 定义存储新option的变量
```
diff --include/linux/tcp.h  server1
struct tcp_options_received{
+u32 qlen;
+u32 timedelta;
+u32 txbytes;
}
```

### 7. 解析新的option : 获取新option的值
```
diff --net/ipv4/tcp_input.c  server1
tcp_parse_options(const struct net *net,
		       const struct sk_buff *skb,
		       struct tcp_options_received *opt_rx, int estab,
		       struct tcp_fastopen_cookie *foc)
+case TCPOPT_QLEN:
+				if ((opsize == TCPOLEN_QLEN) &&
+					(net->ipv4.sysctl_tcp_qlen)) {
+						opt_rx->qlen = get_unaligned_be32(ptr);
+						opt_rx->timedelta = get_unaligned_be32(ptr+4);
+						opt_rx->txbytes = get_unaligned_be32(ptr+8);
+						pri_qlen = opt_rx->qlen;
+						pri_timedelta = opt_rx->timedelta;
+						pri_txbytes = opt_rx->txbytes;
+						cur_qlen = opt_rx->qlen;
+						cur_timedelta = opt_rx->timedelta;
+						cur_txbytes = opt_rx->txbytes;
+						u32 now = tcp_jiffies32;
+						cur_timedelta = jiffies_to_usecs(now);
+				}
+				break;
```

## · 修改P4交换机
IntCC需要在P4中收集数据，并把数据写入到TCP option

### 1. P4解析包含新的TCP option的数据包
```
/*tcp option header*/
header opts_h {
    bit<8> nop1;
    bit<8> nop2;
    bit<8> kind;
    bit<8> opt_length;
    bit<32> qlen;
    bit<32> timestamp;
    bit<32> pkg_count;
}
/*use metadata to save the unmodified tcp checksum*/
struct metadata_t {
    bit<16> checksum_tcp_tmp;
}
/*parse tcp option*/
state parse_option{
    pkt.extract(hdr.opts);
    transition accept;
}
/*write the queue length, the timestamp, the number of packets 
processed in uplink*/
typedef bit<32> qdepth_t;
typedef bit<32> timestamp_t;
```

### 2. P4中包转发 ： 端口转发
```P4
action set_egress_port(egressSpec_t port) {                                  
        ig_tm_md.ucast_egress_port = port; //设置出端口
    }                                                        
    table set_out_port { 
         key = {
            ig_intr_md.ingress_port: exact;
        }	
        actions = { set_egress_port; 
		}                                        
    }
```

### 3. P4中包转发 ： ip转发
```P4
action set_dst(egressSpec_t port) {                                  
        ig_tm_md.ucast_egress_port = port; //设置出端口
    }                                                        
    table set_dst_port { 
         key = {
            hdr.ipv4.dst_addr: exact;
        }	
        actions = { set_dst; 
		}                                        
    }
```

### 4. P4中用register获取txbytes信息，即上行链路处理的数据包的信息
```
/*use metadata to save the data of register counts*/
struct metadata_t {
    bit<32> pkgcount_overall;
    bit<32> pkgcount_downlink;
}
//use register to get the  number of packets both in uplink and downlink
Register<bit<32>, bit<32>>(3) reg_overall;
RegisterAction<bit<32>, bit<32>, bit<32>>(reg_overall) reg_overall_action = {
    void apply(inout bit<32> value, out bit<32> read_value){
        value = value + 1;
        read_value = value;
    }
};
action register_overall_action(bit<32> idx) {
    meta.pkgcount_overall = reg_overall_action.execute(idx);
}
table reg_overall_match {
    key = {
        eg_intr_md.egress_port : exact;
    }
    actions = {
        register_overall_action;
    }
}
//use register to get the  number of packets both in downlink
Register<bit<32>, bit<32>>(3) reg_downlink;
RegisterAction<bit<32>, bit<32>, bit<32>>(reg_downlink) reg_downlink_action = {
    void apply(inout bit<32> value, out bit<32> read_value){
        value = value + 1;
        read_value = value;
    }
};
action register_downlink_action(bit<32> idx) {
    meta.pkgcount_downlink = reg_downlink_action.execute(idx);
}
table reg_downlink_match {
    key = {
        eg_intr_md.egress_port : exact;
    }
    actions = {
        register_downlink_action;
    }
}
```

### 5. P4写入信息到TCP option中
```
/*write the queue length, the timestamp, the number of packets 
processed in uplink*/
typedef bit<32> qdepth_t;
typedef bit<32> timestamp_t;
action write_option_action() {
    hdr.opts.qlen = (qdepth_t)eg_intr_md.enq_qdepth;
    hdr.opts.timestamp = (timestamp_t)(eg_intr_md.enq_tstamp);
    hdr.opts.pkg_count = meta.pkgcount_overall - meta.pkgcount_downlink;
    }
table write_option {
    key = {
        hdr.opts.kind : exact;
    }
    actions = {
        write_option_action;
    }
}
/*deparse tcp option*/
Checksum<bit<16>>(HashAlgorithm_t.CSUM16) tcp_checksum;
apply {
    hdr.tcp.checksum = tcp_checksum.update({
            meta.checksum_tcp_tmp,
            hdr.opts
        });
        pkt.emit(hdr);
}
```

## · P4和端侧的协同
在原型实现中，拓扑从简单的变得更复杂一些： 1) 最开始是一对一发包，中间是两个P4交换机, 使用简单的端口转发; 2) 然后是两个服务器与两个服务器通信, 是哑铃型拓扑, 此时端口转发已无法满足转发要求, 加上了根据ip转发的规则; 3) 与云服务器通信, 需要将P4连接到外网

### 1. 一对一的简单拓扑
server1(enp102s0f1 192.168.2.3) - switch 32
server3(enp179s0f0 192.168.2.4) - switch 35

初始化服务器连接P4的端口ip，并关掉同一网段下的其他端口ip以防止干扰
```
server1 : 
sudo ifconfig enp102s0f1 192.168.2.3 netmask 255.255.255.0
sudo ifconfig enp102s0f0 down

server3 : 
sudo ifconfig enp179s0f0 up
```

下发流表规则
```
switch 32
bfrt.portswitch.Ingress.set_out_port.add_with_set_egress_port(ingress_port=156, port=157)
bfrt.portswitch.Ingress.set_out_port.add_with_set_egress_port(ingress_port=157, port=156)
bfrt.portswitch.Egress.reg_match.add_with_register_action(156,   1)
bfrt.portswitch.Egress.reg_match.add_with_register_action(157,   1)
bfrt.portswitch.Egress.reg_match2.add_with_register_action2(156,   1)
bfrt.portswitch.Egress.options_table.add_with_set_options(14)

switch 35
bfrt.portswitch.Ingress.set_out_port.add_with_set_egress_port(ingress_port=156, port=157)
bfrt.portswitch.Ingress.set_out_port.add_with_set_egress_port(ingress_port=157, port=156)
```

### 2. 哑铃型拓扑：端口转发+arp支持的ip转发
server1  - switch 32 156 - switch 32 157 - 商用交换机 - switch 35 157 - switch 35 156 - server3
server61 - switch 32 158 - switch 32 157 - 商用交换机 - switch 35 157 - switch 35 158 - server62 

设置服务器端口ip
```
server3:
sudo ifconfig enp179s0f0 up

server 62:
sudo ifconfig ens1f0 192.168.2.6/24

server1: 
sudo ifconfig enp102s0f1 192.168.2.3 netmask 255.255.255.0
sudo ifconfig enp102s0f0 down
```

下发流表规则
```
switch 32
bfrt.portswitch.Ingress.set_out_port.add_with_set_egress_port(ingress_port=156, port=157)
bfrt.portswitch.Ingress.set_out_port.add_with_set_egress_port(ingress_port=158, port=157)
bfrt.portswitch.Ingress.set_dst_port.add_with_set_dst(dst_addr="192.168.2.3", port=156)
bfrt.portswitch.Ingress.set_dst_port.add_with_set_dst(dst_addr="192.168.2.5", port=158)
bfrt.portswitch.Egress.reg_match.add_with_register_action(156,   1)
bfrt.portswitch.Egress.reg_match.add_with_register_action(157,   1)
bfrt.portswitch.Egress.reg_match2.add_with_register_action2(156,   1)
bfrt.portswitch.Egress.options_table.add_with_set_options(14)

switch 35
bfrt.portswitch.Ingress.set_out_port.add_with_set_egress_port(ingress_port=156, port=157)
bfrt.portswitch.Ingress.set_out_port.add_with_set_egress_port(ingress_port=158, port=157)
bfrt.portswitch.Ingress.set_dst_port.add_with_set_dst(dst_addr="192.168.2.4", port=156)
bfrt.portswitch.Ingress.set_dst_port.add_with_set_dst(dst_addr="192.168.2.6", port=158)
```

设置服务器的arp
```
server1 :  sudo arp -s 192.168.2.4  6c:92:bf:69:66:be
server3 :  sudo arp -s 192.168.2.3 6c:92:bf:69:6e:39
server61 : sudo arp -s 192.168.2.6 a4:fa:76:01:49:e0
server62 : sudo arp -s 192.168.2.5 a4:fa:76:01:59:78

删除arp：
sudo arp -d 192.168.2.4
```

### 3. 鹏城服务器通过P4交换机访问外网
需要把内部服务器连接p4的端口改成外网可连的ip，而原来连接外网的端口的ip要改成 192.168.109.x/24，方便鹏城内部可以访问。内部服务器ping云服务器首先用端口转发实现单个服务器可以访问外网，从而让云服务器可以学到鹏城服务器的mac地址对应的ip。p4通过连接到外网从而可以访问云服务器，此处只用了一个端口，方便直接用端口转发。
server1 - switch 32 156 - switch 32 157 - 商用交换机 - switch 35 157 - switch 35 140 - 8.141.78.63
server61 - switch 32 158 - switch 32 157- 商用交换机 - switch 35 157 - switch 35 140 - 101.132.178.136

设置服务器ip
```
server1 : sudo ifconfig enp102s0f1 192.168.67.82/24, sudo ifconfig enp102s0f1 192.168.67.82/24
server61 : sudo ifconfig ens1f0 192.168.67.61/24, sudo ifconfig enp61s0f0 192.168.109.x/24
```

下发流表
```
switch 32
bfrt.portswitch.Ingress.set_out_port.add_with_set_egress_port(ingress_port=156, port=157)
bfrt.portswitch.Ingress.set_out_port.add_with_set_egress_port(ingress_port=158, port=157)
bfrt.portswitch.Ingress.set_dst_port.add_with_set_dst(dst_addr="192.168.67.82", port=156)
bfrt.portswitch.Ingress.set_dst_port.add_with_set_dst(dst_addr="192.168.67.61", port=158)
bfrt.portswitch.Egress.reg_match.add_with_register_action(156,   1)
bfrt.portswitch.Egress.reg_match.add_with_register_action(157,   1)
bfrt.portswitch.Egress.reg_match2.add_with_register_action2(156,   1)
bfrt.portswitch.Egress.options_table.add_with_set_options(14)

switch 35
bfrt.portswitch.Ingress.set_out_port.add_with_set_egress_port(ingress_port=140, port=157)
bfrt.portswitch.Ingress.set_out_port.add_with_set_egress_port(ingress_port=157, port=140)
```

更新云端服务器的arp
```
8.141.78.63 : sudo arp -s 192.168.67.82 6c:92:bf:69:6e:39
101.132.178.136 : sudo arp -s 192.168.67.61 a4:fa:76:01:59:78 
```

## · 工程问题总结
















