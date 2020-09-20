# stm32_framework
Welcome to the stm32_framework !

一个计划实现STM32F1和F4工程模板的项目，包含高级封装的HAL库外设驱动、Menu、FreeRTOS、STemWin、LWIP、USB、FATFS、DSP等，以及包含加密、BPNN、最小二乘等诸多便利的组件和算法，并具有良好的易用性。

目前只有F1，当F1完成后，F4就快了...吧。

```
Author : Staok
Email : superxhy@qq.com
Repo : https://github.com/Staok/stm32_framework
System is starting...
...
System init over!
```



------

## 计划组件

-   大部分HAL库外设驱动的高级封装（ MCO \ RTC \ CRC \ TIM \ ADC \ DAC \ IWDG \ USART \ SPI \ WFI \ FLASH \ IAP \ IO \ SDIO \ LTDC LCD \ DCMI \ FSMC \ DMA \ DSP \ FPU \ USB \ CAN \ Ethernet）
-   FreeRTOS
-   LittlevGL或者STemWin
-   LWIP
-   FATFS
-   PID算法
-   ringbuf环形缓冲（类FIFO）
-   内存管理（malloc和free）
-   Menu框架
-   无依赖的string库和sprintf库
-   软件模拟I2C\SPI
-   线性回归
-   常用校验、加密算法（3DES、AES、RC6、TEA、RSA、MD5、SHA1、SHA256）
-   音频编解码（MP3转PCM，外接一个功放直接播放）
-   Modbus协议
-   MQTT协议（包含cjson）
-   JPEG、GIF解码和BMP编解码

## 组件说明

（标为“缺省”的为暂未实现的，以后会加上- -，工作量好大的！ ）

-   **所有基础外设均在sys.h里面通过宏定义配置，每一个宏定义旁边均有使用说明的注释**；所有用户配置外设均在PeriphConfig.c里面，有详细注释，不看不会用
-   MCU外设配置的统一函数为void sys_MCU_Init_Seq(void)，一般无需改动；外接器件初始化统一函数为void sys_Device_Init_Seq(void)，需要自行按需修改
-   所有头文件均放在sys.h里面，其他库文件想互相调用时只需调sys.h即可，不乱
-   本框架是正经的框架，有命名规范和应用规范的说明，在sys.h的上面

### 裸跑或者FreeRTOS

-   SYSTEM_SUPPORT_OS：是否开启FreeRTOS，不开为裸跑；具体应用逻辑编写和任务配置分别在TaskConfig.c和BareConfig.c里面

    FreeRTOS说明：所有API均罗列在TaskConfig.h里！关于FreeRTOS的信号量和事件标志组，均用任务通知替代！关于FreeRTOS的任务与任务、任务与中断间的通讯只用消息队列，功能最全！

### 框架常用组件

-   Menu框架：SYSTEM_SUPPORT_Menu：提供一个实现菜单功能的模板；推荐作为标准常用，应用逻辑围绕这个菜单框架展开编写；具体文件在sys_menu.c里面；按照里面的注释，void keyProcess(void)函数负责扫描输入，void menuProcess(void)函数负责执行菜单功能，这两个函数必须放到一个循环里一直执行
-   string、sprintf库：SYSTEM_SUPPORT_MyString：提供一个实现了string.h大部分字符操作函数的库；具体作用看MyString.c文件里的注释
-   string、sprintf库：SYSTEM_SUPPORT_sprintf：提供一个无依赖的独立实现的sprint库，github开源库from：mpaland/printf；经过修改，可以实现对指定串口发送printf格式化的字符串
-   PID算法：SYSTEM_SUPPORT_pid：提供一个pid算法实现库，集成了积分分离和变限积分，以及可选的不完全微分和微分先行，具体用法看pid.h里面
-   ringbuf：提供一个软件实现的唤醒缓冲区，依据FIFO规则；默认已经用在串口接收的地方，具体看串口宏定义处旁边的注释
-   内存管理（malloc和free）：提供一个自实现的内存分配和释放函数，可用于内部RAM和外部RAM，参考了正点原子的“内存管理”章节的源代码
-   LittlevGL或者STemWin：暂时缺省（优先LittlevGL）
-   LWIP：暂时缺省
-   FATFS：SYSTEM_FATFS_ENABLE：已经默认为SDIO写好底层驱动（需要打开SYSTEM_SDIO_SD_ENABLE），另还可以驱动SPI FLASH，内部FLASH等等，具体用法看宏定义旁边的注释**（TODO：添加SPI FLASH驱动并写入FATFS底层，添加内部FLASH驱动并写入FATFS底层）**
-   DSP\FPU：暂时缺省
-   线性回归：即将加上
-   常用校验、加密算法：即将加上
-   音频编解码：暂时缺省
-   Modbus：暂时缺省
-   MQTT：暂时缺省
-   JPEG、GIF解码和BMP编解码：即将加上

### 框架基础外设

（再次说明，具体实用方法看相应宏定义旁的注释，均在sys.h里）

-   MCO：SYSTEM_MCO_PA8_OUT：设置PA8为MCO输出，默认时钟源为HSE
-   RTC：SYSTEM_RTC_ENABLE：配置使用RTC
-   CRC：SYSTEM_CRC_ENABLE：配置使用CRC循环冗余校验，外设初始化时进行一次硬件CRC32测试
-   TIM4：STSTEM_TIM4_ENABLE：模板基本功能，必须开启！使用通用定时器4，提供时基，默认10ms周期中断
-   TIM3：STSTEM_TIM3PWM_ENABLE：模板基本功能，必须开启！使用通用定时器3，默认不开启中断，提供四路PWM输出；引脚重映射、中断、频率、占空比均提供修改
-   TIM2：STSTEM_TIM2_ENABLE：通用定时器2，功能自定，默认分频系数为72，初始化函数在PeriphCconfig.c里面定义；提供定时器中断、PWM、输入捕获和正交解码功能，用户按需自定
-   ADC1：SYSTEM_ADC1_ENABLE：启否ADC1；提供添加规则组通道、是否连续扫描、是否使用DMA等可选功能；有详细注释
-   IWDG：SYSTEM_IWDG_ENABLE：开启独立看门狗，默认1S的喂狗周期，默认在TIM4定时中断里喂狗，用IWDG必开TIM4
-   USART：SYSTEM_UARTx_ENABLE：开启串口，x = 1、2和3；引脚重映射和波特率可选
-   SPI：SYSTEM_SPIx_ENABLE：开启SPI，x = 1和2；尽量只用其中一个，多个器件用多个SS使能端，不提供引脚重映射
-   WFI：SYSTEM_StdbyWKUP_ENABLE：使用待机-低功耗模式
-   FLASH：SYSTEM_FLASH_IAP_ENABLE：启用对内部FLASH储存空间编程
-   IO：GPIO相关的初始化、输入出和位带操作以及改变入出模式的调用口均在PeriphCconfig.c和PeriphCconfig.h里，有详细注释
-   DAC：SYSTEM_DAC_OUTx_ENABLE：大容量芯片系列外设，x=1~2
-   SDIO：SYSTEM_SDIO_SD_ENABLE：大容量芯片系列外设，底层API已经写入FATFS的底层，请用FATFS的API以文件方式操作
-   IAP：即将加上
-   DMA：默认用于ADC1的多通道扫描模式DMA传送（如果开启SYSTEM_ADC1_useScan的话），另提供驱动代码的模板以供参考
-   Ethernet：暂时缺省
-   FSMC：SYSTEM_FSMC_ENABLE：大容量芯片系列外设，可以用于外部SRAM或者LCD驱动，默认外部RAM用FSMC的块1区3，LCD用FSMC的块1区4，慎改（TODO：LCD的API暂时很乱，暂时不要用，会在近几天更新好~）
-   LTDC LCD：暂时缺省
-   DCMI：暂时缺省
-   USB：暂时缺省
-   CAN：暂时缺省

如果觉得好用，使用时还请别忘加上本仓库的地址哦：https://github.com/Staok/stm32_framework

