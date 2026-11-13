# Changelog

本分支 `refactor/code-standard` 在不改变功能的前提下，对任务七代码做规范化整理。
`main` 分支保留原始版本，本分支是重构版本，两者对比即为本次改动。

---

## cf297d7 修正头文件

- 删除 `motor.h` 顶部 CLion 自动生成、未包裹任何内容的空 include guard
- `__MOTOR_H` / `__PID_H` 改为 `MOTOR_H` / `PID_H`

原因：C99 §7.1.3 规定以双下划线开头、或下划线加大写字母开头的标识符保留给实现使用。
用户代码占用这类名字，万一将来与库里的同名宏撞车，报错位置会指向莫名其妙的文件。
`main.h` 里的 `__MAIN_H` 由 CubeMX 生成，重新 GENERATE 会覆盖，因此没有改动。

---

## c915f92 重命名规范函数与变量名称

- `Motor_Senddianya` → `Motor_SendVoltage`，参数 `dianya` → `voltage`
- `lasttime` → `last_rx_time`

原因：拼音不是英文单词，和项目里其余英文命名混在一起会打断阅读。
函数名用 `Send` 而不是 `Set`，是因为它对外发送一帧 CAN 报文，而不是设置某个内部状态。
`last_rx_time` 直接表达"最近一次收到反馈的时刻"，正好对应看门狗判断的语义；`lasttime` 少了分隔符，读起来是一个词。

---

## 4f428bd 提取魔法数字为命名常量

- 新增 `Core/Inc/config.h`，集中定义任务周期、串口命令、舵机行程、呼吸灯、PID 参数、CAN 滤波器等常量
- 常量名带单位后缀，如 `MOTOR_TASK_PERIOD_MS`、`SERVO_PULSE_SPAN_US`、`SERVO_RANGE_DEG`

原因：同一个物理量散落在多个文件里时，改一处要全局搜索，漏掉一处就是隐藏 bug。
单位写进名字里，看到 `SERVO_PULSE_SPAN_US` 就知道是微秒，不用回头翻计算过程。
这类改动本身零风险，但能让后续调参只需要改一个文件。

---

## d5c8f4a 统一代码格式

- 全部手写文件统一为 2 空格缩进、Allman 大括号（左大括号独占一行）
- 新增根目录 `.clang-format`

原因：`main.c`、`freertos.c`、`can.c`、`usart.c`、`tim.c` 等文件由 CubeMX 生成，
本身就是 2 空格 + Allman。手写代码跟随同一风格，可以让每次重新生成代码后 `git status` 保持干净，
也避免 diff 里混进纯格式噪音。

---

## b1419f7 补充注释

- `PID_Init` / `PID_Calc` / `PID_Reset` / `Motor_Init` / `Motor_SendVoltage` / `HAL_CAN_RxFifo0MsgPendingCallback` 补充 Doxygen 函数头
- `PID_Data`、`Motor_Data` 成员补充单位说明
- 移除 `pid.h` 中未使用的 `stdint.h`

原因：函数头写清参数含义和单位，读调用处就能知道该传什么，不必跳进函数体。
成员单位尤其重要——`motor.angle` 存的是编码值不是角度，`motor.current` 存的是指令值不是安培，
不写清楚很容易被当成物理量直接参与运算。
注释只写在函数头和数据结构上，不逐行解释语句，避免注释随代码改动一起过期。

---

## 2651647 清理头文件引用

- `freertos.c` 删除与上文重复的 `cmsis_os.h`，`<tim.h>` 改为 `"tim.h"`
- `motor.c` 删除未使用的 `usart.h` 与 `stdio.h`

原因：项目内头文件用双引号，系统和第三方库用尖括号，这是编译器查找路径顺序的区别，
写错虽然能编过，但依赖关系就不准确了。未使用的引用会让人误判该文件的真实依赖。

---

## 未做的改动

`freertos.c` 中 `rx_ready`、`key_flag` 的"检查后清零"不是原子操作，
理论上存在中断在检查与清零之间改写标志位的窗口。当前是单写单读，窗口极窄，
且最坏结果是丢一条命令，因此没有加临界区保护，在此记录为已知边界情况。

---

## 功能验证

重构前后在实机上对比运行，四条功能均正常：
motor+100 转速稳定 100rpm；串口每 2 秒打印一次转速；
每次按键舵机转 20°；断开 CAN 后红灯 2 秒周期呼吸，恢复后绿灯常亮。