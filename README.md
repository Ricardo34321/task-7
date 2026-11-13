# 任务七：FreeRTOS 多任务控制

RoboMaster 电控培训任务七。在任务六的 PID 速度环基础上，
将单一主循环程序改造为 FreeRTOS 多任务结构。

## 硬件环境

- 主控：STM32F407IGHx（RoboMaster 开发板）
- 电机：DJI GM6020，CAN 总线 1 Mbps
- 上位机通信：USART1，115200 8N1

## 开发环境

- IDE：CLion
- 构建：CMake + arm-none-eabi-gcc
- 配置工具：STM32CubeMX
- 实时系统：FreeRTOS（CMSIS-RTOS V2）

## 代码结构

| 路径 | 作用 |
| --- | --- |
| `Core/Src/main.c` | 外设初始化、PID 参数与任务参数配置 |
| `Core/Src/freertos.c` | 任务函数实现与任务创建 |
| `Core/Src/motor.c` | GM6020 指令下发与反馈帧解析 |
| `Core/Src/pid.c` | PID 控制器 |
| `Core/Inc/` | 各模块头文件 |

## 任务划分

| 任务 | 优先级 | 栈（word） | 职责 |
| --- | --- | --- | --- |
| `MotorTask` | 32 | 128 | PID 计算与电机指令下发，周期 10 ms |
| `CommTask` | 25 | 256 | 串口命令解析与状态打印，周期 10 ms |
| `LedTask` | 25 | 128 | 按键响应与指示灯状态，周期 10 ms |

## 分支说明

- `main`：任务七原始代码，未做修改
- `refactor/code-standard`：按代码规范重构后的版本

改动内容与原因见 [changelog.md](changelog.md)。