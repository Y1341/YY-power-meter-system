# Power Meter System - CETC41 AV2438B Control

基于C++和LabVIEW的双通道功率计控制系统

## 硬件信息
- **功率计型号**: CETC41 AV2438B
- **序列号**: ZKL00039
- **固件版本**: 1.0.23
- **通信方式**: USB-GPIB (NI VISA库)
- **操作系统**: Windows 11

## 项目结构

```
power-meter-system/
├── cpp/                          # C++后端驱动
│   ├── CMakeLists.txt
│   ├── include/
│   │   ├── PowerMeterSCPI.h     # SCPI命令定义
│   │   ├── DataLogger.h         # 数据记录模块
│   │   └── VisaDevice.h         # VISA设备通信
│   ├── src/
│   │   ├── main.cpp
│   │   ├── PowerMeterSCPI.cpp
│   │   ├── DataLogger.cpp
│   │   └── VisaDevice.cpp
│   └── bin/                      # 编译输出目录
���
├── labview/                      # LabVIEW前端
│   ├── PowerMeterControl.lvproj  # LabVIEW项目
│   ├── UI/
│   │   └── MainVI.vi             # 主控制界面VI
│   └── SubVIs/
│       ├── GPIB_Init.vi
│       ├── Read_Power.vi
│       ├── Set_Frequency.vi
│       └── Set_Attenuation.vi
│
├── docs/                         # 文档
│   ├── SCPI_Commands.md          # SCPI命令列表
│   ├── Setup_Guide.md            # 安装配置指南
│   └── AV2438B_Manual.txt        # 功率计手册参考
│
└── README.md                     # 本文件

```

## 功能说明

### C++驱动模块
- ✅ NI VISA库集成
- ✅ SCPI命令封装
- ✅ 1秒间隔自动采集
- ✅ 数据文件记录（格式：20260521232540.txt）
- ✅ 设备重启功能

### LabVIEW界面
- ✅ GPIB地址选择与连接
- ✅ 频率设定（MHz）
- ✅ 通道A/B衰减配置（dB）
- ✅ 实时值显示（原始值 + 衰减校正值）
- ✅ 数据单位：dBm
- ✅ 开始/停止/重启按钮

## 快速开始

### 环境要求
1. **Visual Studio 2019+** 或 GCC/CMake
2. **NI-VISA** (最新版本)
3. **LabVIEW 2019**
4. **CMake 3.10+** (可选，用于C++编译)

### 编译C++驱动

```bash
cd cpp
mkdir build
cd build
cmake ..
make  # 或在Visual Studio中打开解决方案
```

### 运行LabVIEW前端

1. 打开 `labview/PowerMeterControl.lvproj`
2. 打开 `labview/UI/MainVI.vi`
3. 选择GPIB地址（通常为0）
4. 点击"连接"按钮
5. 设置频率和衰减参数
6. 点击"开始"按钮开始采集

## 数据文件格式

**文件名**: `YYYYMMDDHHmmss.txt` (例如: `20260521232540.txt`)

**文件内容示例**:
```
=== Power Meter Data Log ===
Timestamp: 2026-05-21 23:25:40
Device: CETC41 AV2438B
GPIB Address: 0
Frequency: 1000 MHz
Channel A Attenuation: 10 dB
Channel B Attenuation: 0 dB

Time(s) | Channel A (dBm) | Channel B (dBm)
0       | -20.45         | -19.32
1       | -20.43         | -19.31
2       | -20.42         | -19.30
```

## SCPI命令参考

| 功能 | SCPI命令 | 示例 |
|------|---------|------|
| 设置频率 | `:CONF:FREQ <freq>` | `:CONF:FREQ 1000E6` |
| 读取功率 | `:READ:POW?` | 返回: `-20.45` |
| 设置衰减 | `:INP:ATT <att>` | `:INP:ATT 10` |
| 查询衰减 | `:INP:ATT?` | 返回: `10` |
| 设置通道 | `:CONF:CHANNEL <ch>` | `:CONF:CHANNEL 1` |
| 重置设备 | `*RST` | 重启功率计 |

详见 `docs/SCPI_Commands.md`

## 故障排除

### 无法连接GPIB设备
1. 检查USB驱动是否正确安装
2. 在NI MAX中验证设备是否可见
3. 确保GPIB地址正确

### 数据读取不稳定
1. 点击"重启"按钮重新初始化功率计
2. 检查功率计是否在预热状态
3. 增加采样间隔

### 文件保存失败
1. 检查硬盘空间
2. 检查文件夹权限
3. 确保路径不包含特殊字符

## 技术支持

- SCPI命令参考: `docs/SCPI_Commands.md`
- 安装配置指南: `docs/Setup_Guide.md`
- C++接口文档: `cpp/include/` 中的头文件

## 许可证

本项目用于CETC41 AV2438B功率计控制。

---
*Last Updated: 2026-05-23*
