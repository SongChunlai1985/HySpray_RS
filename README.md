HySpray_RS 喷雾控制上位机
基于 Qt 的喷雾设备串口/RS485 控制上位机，支持 RS232 原始串口通信与 RS485 自定义协议，用于喷头、主机、喷雾模式、定时任务、分组表、设备时间、波特率、ID 等配置与状态读取。

功能特性
Qt Widgets 图形界面，支持 Windows / Linux 桌面环境

RS232 通信：基于 QSerialPort，可配置端口、波特率、数据位、停止位、校验位、流控制

RS485 通信：基于 Net485 协议封装，支持 CRC16 校验、主从地址、读写请求

接收/发送支持 ASCII 与 Hex 显示和输入

支持手动发送与自动定时发送

自动发送间隔最小 10ms，默认 100ms

喷头控制：电磁阀、雾化器、水泵、暂停/继续、全部关闭、获取状态

主机配置：获取时间、设置时间、波特率写入等

设备配置：喷头选择、ID 写入/读取、序列号读写、CMD 发送

表数据读取：喷雾模式表、单日定时表、喷头分组表、日任务表、周任务表

实时状态解析：喷雾模式参数、单日定时、喷头分组、周任务、设备时间

固件升级界面预留

技术栈
Qt 5.x

C++11

Qt Serial Port

qmake

Net485 自定义 RS485 协议

建议使用 Qt 5.x 构建。若使用 Qt 6，需要适配 QTextCodec、QRegExpValidator 等已变化或移除的 API。

目录结构
text
HySpray_RS/
├── HySpray_RS.pro      # qmake 工程文件
├── main.cpp            # 程序入口
├── mainwindow.h        # 主窗口类声明
├── mainwindow.cpp      # 主窗口逻辑、串口收发、485 命令组装与解析
├── mainwindow.ui       # Qt Designer 界面文件
├── Net485.h            # RS485 协议类声明
├── Net485.cpp          # RS485 帧封装、CRC16、读写请求
├── logo.ico            # Windows 程序图标
└── logo.rc             # Windows 资源文件
构建
依赖安装
Ubuntu / Debian：

bash
sudo apt install qtbase5-dev qt5-qmake qtbase5-dev-tools libqt5serialport5-dev
Windows：

安装 Qt Online Installer

勾选 Qt 5.x 以及 Qt Serial Port 模块

可使用 MinGW 或 MSVC 工具链

编译
使用 Qt Creator 打开 HySpray_RS.pro，选择套件后构建运行。

或命令行：

bash
git clone <your-repo-url>
cd HySpray_RS
qmake HySpray_RS.pro
make -j
Windows + MSVC 可使用：

bash
qmake HySpray_RS.pro
nmake
或使用 jom。

运行
Windows：

text
release\HySpray_RS.exe
Linux：

bash
./HySpray_RS
Linux 下如果无法打开串口，请将当前用户加入 dialout 组：

bash
sudo usermod -aG dialout $USER
重新登录后生效。

使用说明
连接设备。

点击“检测”扫描可用串口。

选择端口名、波特率、数据位、停止位、校验位、流控制。

点击“打开RS232串口”或“打开RS485串口”。

在发送区选择 ASCII 或 Hex。

输入发送内容，点击“手动发送”，或勾选“自动发送”并设置间隔。

RS485 模式下，可通过“设备配置”“主机配置”“喷头控制”“实时状态”等区域进行设备操作。

接收区分别显示 RS232 与 RS485 数据。

Hex 发送示例：

text
AA 55 00 08 F0 F1 20 08
注意：Hex 输入只允许十六进制字符和空格，不需要写 0x。

RS485 / Net485 协议
Net485 负责将上层业务数据封装为 RS485 帧，帧格式如下：

字段	长度	说明
SYNC	1	帧同步，0x55
SOH	1	头开始，0x01
Method	1	方法，如 0x01 只写
AddrSource	1	源地址
AddrDestination	1	目标地址
Event	1	事件/命令
Length	2	数据长度，小端
STX	1	可选，0x02，仅当 Length > 0
Data	N	负载数据
ETX	1	可选，0x03，仅当 Length > 0
CRC16	2	CRC 校验，小端
EOT	1	帧结束，0x04
CRC16 参数：

多项式：0x1021

位宽：16

初始值：0x0000

查表实现：CrcTable

地址定义：

名称	值
AddrMaster	0x00
AddrSlave01	0x01
...	...
AddrSlave31	0x1F
AddrBroadcast	0xFF
在 MainWindow::write485() 中，上层业务数据会通过 n485.writeOnly() 发送。例如：

cpp
n485.writeOnly(address, 0x00, data, ushort(data.size()));
因此，界面中输入的 AA 55 ... 属于业务数据，实际串口上发送的是经过 Net485 封装后的完整帧。

业务命令示例
以下为代码中出现的部分业务数据示例，RS485 模式下会由 Net485 进一步封装：

功能	业务数据示例
获取时间	AA 55 00 08 F0 F1 20 08
设置时间	AA 55 00 0F F0 F1 1F 14 0C 11 0E 17 15 04 7D
读取喷头分组表	AA 55 00 08 F0 F1 19 01
读取周任务表	AA 55 00 08 F0 F1 1D 05
写入波特率	AA 55 00 0C FE F1 12 <BaudRate> B1
读取喷头 ID	AA 55 00 08 <SprayHeadId> F1 0C 08
读取设备信息时，根据表类型组装不同命令：

喷雾模式表：AA 55 00 09 F0 F1 15 <ID> <ID-2>

单日定时表：AA 55 00 09 F0 F1 17 <ID> <ID>

喷头分组表：AA 55 00 08 F0 F1 19 01

周任务表：AA 55 00 08 F0 F1 1D 05

响应解析
MainWindow::read485() 会对返回的 Hex 字符串进行前缀匹配并解析到界面。当前已处理：

响应前缀	含义
FAF50017	实时状态 / 喷雾模式参数
FAF5000E	单日定时表
FAF50030	喷头分组表
FAF50010	周任务表
FAF5000F	设备时间
解析结果会显示在“实时状态”区域的 lineEdit11 至 lineEdit73 等控件中，并通过 ToolTip 提示字段含义。

主要类说明
MainWindow
负责：

串口扫描与参数配置

RS232 与 RS485 打开/关闭

数据发送与接收显示

ASCII / Hex 转换

自动发送定时器

RS485 业务命令组装

RS485 响应解析

喷头、主机、设备配置等界面交互

Net485
负责：

RS485 帧封装

CRC16 计算

写请求 writeOnly()

原始读取 readRequest()

串口错误状态维护

从站模式下响应处理预留

已知限制 / TODO
部分按钮目前只有界面，尚未绑定完整业务逻辑，例如序列号读写、CMD 发送、固件升级、部分喷头控制等。

固件升级界面为预留，未实现实际升级流程。

日任务表 的读取命令在 on_pushButton_13_released() 中未处理。

自定义串口名和自定义波特率在不同平台下需注意组合框可编辑状态。

当前代码使用 QTextCodec、QRegExpValidator，迁移到 Qt 6 时需要额外适配。

建议增加日志保存、协议校验提示、错误码显示、配置文件保存等功能。

贡献
欢迎提交 Issue 和 Pull Request。提交前建议：

说明测试环境与 Qt 版本。

提供复现步骤或串口日志。

如果涉及协议变更，请同时更新 README 中的协议说明。

许可
Net485.h / Net485.cpp 头部包含 SimCORE 版权信息，并声明了 GNU LGPLv3、GPLv2 或商业许可的使用方式。使用、修改和分发该模块时请遵守相应许可条款。

项目其他部分如无单独 LICENSE 文件，请根据仓库实际需要补充开源许可或商业授权说明。

捐赠
BTC: 13SongiriQuWoFhoimsVS21CyaTxozKBVA
