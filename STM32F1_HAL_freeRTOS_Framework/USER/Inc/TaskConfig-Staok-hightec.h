#ifndef __TASKCONFIG_H
#define __TASKCONFIG_H

#include "FreeRTOS.h"
#include "task.h"



struct TaskStructure{
    TaskFunction_t  Task_Function;
    const char*     Task_Name;
    uint16_t        Task_STK_Size;
    void*           Task_Para;
    UBaseType_t     Task_Prio;
    TaskHandle_t Task_Handle;
};

extern struct TaskStructure Define_Task[];
BaseType_t xTaskCreate_t(uint8_t taskIndex);

void Task_Begin(void);
void start_task(void *pvParameters);

/*********************************************************************************************/
/*                      以下为要修改的部分                                                   */
/********************************************************************************************/


//任务函数的声明
void led0_task(void *pvParameters);
void led1_task(void *pvParameters);




#endif


/*
FreeRTOS知识简析
	移植从原子的历程里面拿！
	使用：
		创建函数均使用动态申请内存的API，不使用静态创建API。
		阻塞时间参数均为节拍数，configTICK_RATE_HZ所设定的节拍。
		任务概念：
			状态：运行、就绪、阻塞和挂起(即暂停某任务)
			在优先级小于configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY的外设中断中可以用FreeRTOS的API！！
			注意在中断中恢复任务写法特殊，模板要注意，具体在原子429手册的99页！
			后面好几个从ISR调用的API，写法都需要判断一个值是否为pdTRUE，如果是就调度一次!
		延时：
			delay_us()——微秒延时
			delay_ms()——vTaskDelay()的封装，当前任务被延时阻塞住，如果调度器未启动，则调用微秒延时，
			在一个时间片内如果被此函数堵塞住，则会立马运行其他任务
			delay_xms()——delay_us()的封装，会延时直到一个时间片耗尽，即占满本次时间片
			
			vTaskDelay()		是相对延时，即延时所在任务运行的总时间
			vTaskDelayUntil()	是绝对延时，用于保证按照一定频率运行的任务，保证按照一定的周期取消阻塞！用法看194页，移过来
		
		taskENTER_CRITICAL()和taskEXIT_CRITICAL()用于临界区代码逻辑时序的保护；
		taskENTER_CRITICAL_FROM_ISR()和taskEXIT_CRITICAL_FROM_ISR()同上，只不过用在中断函数内，
		且只能在优先级低于configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY的中断函数内使用。
		以上四个函数是以下两个函数portDISABLE_INTERRUPTS()和portENABLE_INTERRUPTS()的封装，
		所以只用以上四个就好。

		注意：
		portDISABLE_INTERRUPTS()和portENABLE_INTERRUPTS()，STM32的外设的优先级低于configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY(≥此数，＜15)的外设中断归这俩函数管，(除了系统级中断如复位、硬件错误等)
		前者为关闭比configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY优先级低的STM32外设的所有普通中断，即5-15的，包含5。

		注意：
		STM32系统中断优先级为0-15，		0为最高优先级，是对于STM32的外设而言；
		FreeRTOS目前定义为0-15优先级，	0为最低优先级，任务的优先级最好都从1开始，15为最高优先级。
		
		队列：也称消息队列
			任务和任务、任务和中断之间传递消息。对于FreeRTOS，队列消息是全局的。
			通常FIFO机制：入队于队列尾部，出队从队列头部。或者LIFO机制
			小数据量的消息可以用拷贝方式把数据赋给队列，
			数据量大时可以把数据地址赋给队列，但注意防止局部变量的删除。
			一个队列中的数据建议页最好是一种数据，因为入队和出队的位置不能选择。
			
			创建队列：void()，参数：队列长度（队列项目数量），每个项目的大小（字节数，可以很多），
					返回队列句柄
			入队和出队函数分为任务级和中断级（注意中断级函数写法不一样，移植历程看227页开始）
			发送：
				分为发送到队列尾部（推荐）和头部。
				参数：队列句柄，发送的消息（拷贝方式），阻塞时间：
				如果队列满了，则等待，等于0不等，等于portMAX_DELAY死等，
				不建议阻塞的情况发生，否则采集的数据失去实时性，队列长度往大了整！                                                       
			接收：
				receive：数据出队后删除在队列中对应的数据，peek：不删除
				
			队列上锁和解锁函数：
			不建议用xQueueOverwrite()覆盖入队函数，队列长度整大一些呗。
			
		信号量：用于公共资源数量管理和任务同步
			分为：二值、计数、互斥和递归呼互斥，用于不同场合
			信号释放和提取函数分为任务级和中断级
			互斥和递归互斥信号量不能应用于中断中，中断中的信号量函数没有阻塞时间选项
			
			二值信号量创建：
				二值和数值信号量的信号释放和提取函数均一样：
				释放一次（Give）看作给二值信号量值赋一，提取一次（Take）赋零
				信号释放和提取在中断写法也特殊，看243页
				242页开始
				注意：避免两个任务公用一个二值信号量，例如当高优先级的任务等一个二值信号量
				但是当前只有一个低优先级的任务会释放这个二值信号量，
				再加上如果有一个中优先级任务打断了这个低优先级任务，会造成高优先级任务更加延误，
				则等同于低优先级任务打断了高优先级任务！此问题称为优先级翻转，互斥信号量解决之
			
			计数信号量的创建：参数：最大数量和初始值，返回计数信号量的句柄
				计数信号量的释放和提取，释放一次计数信号量值加一，提取一次减一，
				值是多少可以通过函数随时获取
				
			互斥信号量：
				创建：
				释放和提取与二值信号量的一样。
				一个低优先级任务正在使用互斥信号量时，有个高优先级任务也尝试提取这个时，这个高优先级
				的任务会被阻塞，但是这个高优先级任务会把低优先级任务的优先级提升至与自己相同，
				这个过程称为优先级继承，这样防止有一个中间优先级的任务打断原来低优先级任务造成更长延误，
				可以看出，互斥信号量不会消除优先级翻转所造成的延误，但会尽量减少延误！
				提取一次配套写释放一次。
				但我还是建议，一个信号量不要多个任务公用！这样互斥和递归互斥就不需要用了。
				
			
			递归互斥信号量：可以多次提取和释放的互斥信号量，
				即如果需要等总共提取三次才能执行下面的逻辑，就可以写三次提取，
				最后提取几次就释放几次。
			
			二值信号量更适合任务与任务、任务与中断的同步，本质为只有一个队列项的队列；
			计数信号量适合管理某公共资源数量，或者事件计数，本质为有多个队列项的队列；
			
		软件定时器：
			定时的周期性调用回调函数。
			在回调函数中不能用任何阻塞！例如延时、访问队列和信号量的非零阻塞等API。
			分为一次性和周期运行，均可由软件定时器复位函数复位。
			从复位的时刻开始重新运行指定的某软件定时器。
			注意config里面设置回调函数的堆栈大小！
			创建：
			复位：任务级：ISR级：
			开启：任务级：ISR级：
			停止：任务级：ISR级：
			
		事件标志组：
			某一个任务需要与多个事件或任务进行同步。
			事件标志组：当configUSE_16_BIT_TICKS为0时可以储存24个事件标志位。
			bit0存放事件位0，以此类推。
			创建：返回事件标志组的句柄。
			事件位的置一和清零：任务级：ISR级：
			获取事件标志组：任务级：ISR级：
			等待指定的事件位：参数：事件组句柄，事件位字，读完是否清零，等待事件位全部置位还是任意其一，阻塞时间
			依我看，事件位字、读完是否清零 和 等待事件位全部置位还是任意其一 可以都用define改个名！
			依我看，事件标志组可以替代二值信号量，信号量便只用计数型信号量。
			
		任务通知：
			可以使用任务通知来替代信号量、消息队列和事件标志组等这些东西，而且效率更高。
			每一个任务结构体中都有一个任务通知值。
			任务通知发送函数：
				xTaskNotify()，带有通知值，任务级和ISR级
				xTaskNotifyGive()，不带通知值，只将通知值加一，任务级和ISR级
				xTaskNotifyAndQuery()，比xTaskNotify()多一个保存上一次通知值的参数，任务级和ISR级
				三个函数的区别只是更新通知值的方式不同，用于模拟队列、信号量和事件标志组。
			获取任务通知：
				ulTaskNotifyTake()，可以设置退出函数之前将通知值清零或减一，返回值为改动之前的通知值！
				仅可模拟二值和计数信号量
				xTaskNotifyWait()，全功能的获取任务通知函数，为了与上面函数区分，
				可用于模拟队列和事件标志组
			
			模拟二值信号量：
				xTaskNotifyGive()给信号加一
				ulTaskNotifyTake()拿信号并清零
			模拟计数型信号量：
				xTaskNotifyGive()给信号加一，	停车场走了一辆车，空车位加一
				ulTaskNotifyTake()拿信号并减一，停车场来了一辆车，空车位减一，返回值为改动之前的值
			模拟消息队列：（不建议用此，老实用消息队列就好，功能最全）
				限制：相当于长度为1的队列，队列项大小为一个32bit
				xTaskNotify()，给通知值，覆盖写方式
				xTaskNotifyWait()，获取通知值，具体参数看原子手册337页
			模拟事件标志组：
				任务通知值的每一个bit用作事件位。
				xTaskNotify()，把事件位当作通知值给，eBitSet的写方式
				xTaskNotifyWait()，获取通知值，用通知值和事件位与运算判断哪些事件发生
				具体参数看原子手册340页，原子写麻烦了，移植想办法简化一下，
				另外原子写的费CPU，当有一个事件发生后阻塞就解除了，并不是等所有事件都发生再接触！
				注意中断写法不同，看参数详解。
				
				历程329页开始。要移植。
				
			任务通知总结：可以完全替代：二值 和 计数型信号量，事件标志组。
			所以，对于FreeRTOS，用任务通知和消息队列这两个即可。
			
		FreeRTOS的低功耗Tickless模式：要用的话再细看手册和例程。
			STM32F429提供三种低功耗模式：睡眠、停止和待机，相关描述从344页开始。
			FreeRTOS在空闲模式时进入和退出低功耗模式，通过WFI（等待中断）的方式进入低功耗模式。
			具体进出低功耗的配置和时间FreeRTOS均已帮我们做好了，哇哦。
			先开启该模式：
			设置宏configUSE_TICKLESS_IDLE为1
			写进入低功耗之前预处理函数和退出低功耗之后的恢复函数，用户按照需求自行编写：
			configPRE_SLEEP_PROCESSING()和configPOST_SLEEP_PROCESSING()，
			具体需要完成的内容细看351页！！例如降低主频，失能外部器件等待，这是高要求时候用的。
			看手册效果挺明显，板级电流降低了近50mA。
			要用的话再细看手册和例程。
			
		空闲任务：
			空闲任务中，会把已经删除的任务的内存释放掉，低功耗模式下会进出STM32的低功耗模式等
			空闲任务钩子函数：配置宏configUSE_IDLE_HOOK为1；编写钩子函数void vApplicationIdleHook(){}。
			钩子函数内杜绝使用阻塞，钩子函数内最好不实现低功耗，要用低功耗模式用FreeRTOS的更好。
			
		FreeRTOS的内存管理：
			标准C库里的malloc()和free()：在小MCU里效率不高；占空间；线程不安全等等问题
			可申请的总内存在：uint8_t ucHeap[configTOTAL_HEAP_SIZE];
			可以定义内存申请失败的钩子函数：
				配置configUSE_MALLOC_FAILED_HOOK为1；
				钩子函数：
			自己申请内存的一个好处是可以用外部SRAM
			函数：pvPortMalloc()和vPortFree()
			FreeRTOS提供五种内存分配方法，因为内存碎片等问题解决方法不同，需选择合适的：
			heap_1：适用于创建好的任务和队列等就不会再删除，因为没有释放函数；花费时间基本一致，不会产生内存碎片
			heap_2：有释放函数，但有内存碎片产生，适用于每个动态创建的任务或队列的堆栈大小一致的应用
			heap_3：对标准c的malloc()和free()进行简单封装并加了线程保护，还需要改启动文件，不好用还麻烦
			heap_4：提供内存碎片合并机制，不产生内存碎片，适合任务和队列的堆栈不同的应用，时间不确定
					在MCU内部RAM和外部SRAM之间只能二选一
			heap_5：内存碎片合并算法和heap_4一致，允许多段内存合用作为内存堆，
					内存堆初始化函数：393页开始，初始化内存堆之前不许搞任何内存申请活动
			获取剩余内存大小函数：
			内存申请和释放要成对出现，在一段内存没有释放之前绝不能再申请内存，否则造成内存泄漏！
			
				
	
	
	概念：（不需要编程但需要了解的）
		列表和列表项：本质很类似链表
			为FreeRTOS广泛使用的存东西的结构，例如任务调度就用到该结构来决定调度
			列表项就是结构体，分为迷你列表项和列表项，迷你列表项里面只有数据、上一个列表项的地址和下一个的
			列表项里面内容比迷你列表项多一点点
			列表里面包含：
				列表项个数（num）、当前指向的列表项地址（index）和一个迷你列表项（叫ListEnd）
			然后以列表里面的迷你列表项为头，像链表一样把这些列表项串起来形成一个环
			函数包括：
				初始化列表和列表项、往列表里面添加列表项和删除列表项，添加分为直接添加和尾部添加
			列表项的插入是按照value的值升序排列的！
			历程看原子的F429的116页
		
		FreeRTOS的任务切换最终都是在PendSV中断中完成的，切换图解：147页，
		里面解释了，这样安排有利于保证中断响应的实时性
		
		中断优先级：任务线程 < SVC、PendSV < ISR < systick
		
		不同优先级的任务按照优先级调度执行，一种软件计算，一种用硬件计算（前导零指令，默认用此）
		相同优先级的任务按照时间片轮询（选用时间片而非协程时），每一个时间片时间为一个时钟节拍（1ms），时间到了打断执行下一个任务，时间没到继续循环本任务
		
		
		
*/


