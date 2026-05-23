# CETC41 AV2438B 功率计系统安装指南

## 目录

1. [硬件准备](#硬件准备)
2. [驱动安装](#驱动安装)
3. [C++编译](#c编译)
4. [LabVIEW配置](#labview配置)
5. [测试](#测试)
6. [故障排除](#故障排除)

## 硬件准备

### 所需设备

- ✅ CETC41 AV2438B 功率计（序列号: ZKL00039）
- ✅ USB转GPIB适配器
- ✅ USB数据线（通常随适配器提供）
- ✅ 功率计专用电源线
- ✅ 射频连接线（SMA或其他接头）
- ✅ Windows 11 计算机

### 硬件连接

```
功率计
  |
  |-- USB-GPIB适配器
        |
        |-- USB
            |
          Windows11
         笔记本电脑
```

**步骤：**

1. 打开功率计电源（预热5-10分钟）
2. 将USB-GPIB适配器连接到功率计GPIB接口
3. 将USB线连接到计算机USB端口
4. 等待设备被识别

## 驱动安装

### 步骤1: 安装NI-VISA

1. 访问 [NI官方网站](https://www.ni.com/zh-cn/downloads/drivers/download.visa.html)
2. 下载最新版本的 NI-VISA（或驱动光盘）
3. 运行安装程序
4. 按照向导完成安装
5. 重启计算机

### 步骤2: 验证GPIB驱动

1. 打开 "NI Measurement & Automation Explorer" (NI MAX)
2. 展开 "Devices and Interfaces"
3. 查看是否显示 "GPIB0::0::INSTR" 或类似的设备
4. 如果未显示，重新安装驱动或检查USB连接

### 步骤3: 通信测试

```bash
# 在NI MAX中
1. 右键点击GPIB设备
2. 选择"Test"
3. 发送命令: *IDN?\n
4. 应返回: CETC41,AV2438B,ZKL00039,1.0.23
```

## C++编译

### 方法1: 使用CMake (推荐)

#### 前置要求

- Visual Studio 2019+ 或 MinGW
- CMake 3.10+
- NI-VISA SDK

#### 编译步骤

```bash
# 进入项目目录
cd power-meter-system/cpp

# 创建构建目录
mkdir build
cd build

# 运行CMake
cmake ..

# 编译
cmake --build . --config Release
```

#### 输出

```
./bin/PowerMeterApp.exe
```

### 方法2: 使用Visual Studio

1. 打开 Visual Studio 2019
2. 选择 "Open CMake"
3. 浏览到 `power-meter-system/cpp` 文件夹
4. CMake会自动配置
5. 点击 "Build" → "Build All"
6. 可执行文件生成在 `cpp/cmake-build-release/bin/`

### 配置NI-VISA库路径

如果CMake无法找到VISA库，手动添加路径：

**CMakeLists.txt**:

```cmake
# 添加这一行
set(CMAKE_PREFIX_PATH "C:/Program Files/National Instruments/VISA/" ${CMAKE_PREFIX_PATH})
```

## LabVIEW配置

### 步骤1: 安装LabVIEW 2019

- 确保已安装LabVIEW 2019
- 安装GPIB库（通常与NI-VISA一起安装）

### 步骤2: 打开项目

```
1. 启动LabVIEW 2019
2. 打开 File → Open
3. 浏览到 power-meter-system/labview/PowerMeterControl.lvproj
4. 点击打开
```

### 步骤3: 配置VI

1. 在项目浏览器中打开 `UI/MainVI.vi`
2. 检查所有子VI是否正确加载
3. 如有缺失的VI，从 `SubVIs` 文件夹补充

### 步骤4: 验证GPIB连接

在LabVIEW中测试GPIB通信：

1. 创建临时测试VI
2. 使用"VISA Configure Serial Port"配置GPIB资源
3. 使用"VISA Write"发送 `*IDN?\n`
4. 使用"VISA Read"读取响应
5. 应返回设备ID

## 测试

### C++程序测试

```bash
# 进入bin目录
cd power-meter-system/cpp/cmake-build-release/bin

# 运行程序
./PowerMeterApp.exe

# 预期输出
=== CETC41 AV2438B Power Meter Control System ===
Initializing power meter at GPIB address 0...
Power meter initialized successfully.
Device ID: CETC41,AV2438B,ZKL00039,1.0.23
...
```

### LabVIEW界面测试

1. 打开 `MainVI.vi`
2. 点击 "Run" 按钮
3. 输入GPIB地址（通常为0）
4. 点击 "Connect"
5. 设置频率（例如1000MHz）
6. 点击 "Start"
7. 观察实时功率值
8. 点击 "Stop"

### 数据文件验证

检查生成的数据文件：

```
查找文件: 20260521232540.txt (根据实际时间)

内容示例：
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
```

## 故障排除

### 问题1: 无法找到GPIB设备

**症状**: "Failed to open GPIB instrument"

**解决方案**:

1. 检查USB连接
2. 在NI MAX中验证设备是否可见
3. 尝试更改GPIB地址（0-30）
4. 重新安装VISA驱动

### 问题2: VISA库链接错误

**症状**: "Cannot find visa.h" 或链接错误

**解决方案**:

1. 确保NI-VISA已安装
2. 检查CMakeLists.txt中的库路径
3. 在Visual Studio中手动添加：
   - 包含目录: `C:/Program Files/National Instruments/VISA/Include`
   - 库目录: `C:/Program Files/National Instruments/VISA/Lib`
   - 库文件: `visa.lib`

### 问题3: 功率计无响应

**症状**: "Device not responding" 或超时

**解决方案**:

1. 检查功率计电源（指示灯是否亮）
2. 等待功率计预热（5-10分钟）
3. 重启功率计
4. 尝试在NI MAX中测试 `*IDN?` 命令

### 问题4: 数据读取不稳定

**症状**: 功率值波动较大或间断

**解决方案**:

1. 增加采样间隔（设置>1秒）
2. 检查射频连接线是否松动
3. 确保功率计已完全预热
4. 点击"Restart"按钮重新初始化

### 问题5: LabVIEW编译错误

**症状**: "SubVI not found" 或路径错误

**解决方案**:

1. 重新加载项目（File → Close → Open）
2. 检查所有VI文件是否完整
3. 验证VI路径（不应包含中文字符）
4. 重建VI依赖关系

## 环境变量配置（可选）

### Windows 10/11

```
系统属性 → 环境变量

新建系统变量：
VISA_HOME = C:\Program Files\National Instruments\VISA

编辑Path，添加：
C:\Program Files\National Instruments\VISA\Bin
```

## 快速诊断脚本

创建 `test_visa.py` (使用Python+PyVISA)：

```python
import pyvisa

rm = pyvisa.ResourceManager()
print("Available resources:")
print(rm.list_resources())

# 测试连接
try:
    inst = rm.open_resource('GPIB0::0::INSTR')
    print(inst.query('*IDN?'))
    inst.close()
except Exception as e:
    print(f"Error: {e}")
```

运行方式：
```bash
python test_visa.py
```

## 更新日志

- **2026-05-23**: 初始版本
  - 驱动安装说明
  - CMake编译配置
  - LabVIEW项目配置
  - 故障排除指南

---

有问题？请参考 `README.md` 和 `SCPI_Commands.md`
