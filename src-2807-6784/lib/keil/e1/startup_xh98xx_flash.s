;******************************************************************************
;
;  Copyright (C), 2016, Xihao Co., Ltd.
;
;******************************************************************************
; * @file     startup_XH98xx.s
; * @brief    CMSIS Cortex-M3 Core Device Startup File for
; *           Device XH98xx
; * @version  V1.00
; * @date     27. May 2016
; *
; * @note
; * Copyright (C) 2016 ARM Limited. All rights reserved.
; *
; * @par
; * ARM Limited (ARM) is supplying this software for use with Cortex-M
; * processor based microcontrollers.  This file can be freely distributed
; * within development tools that are supporting such ARM based processors.
; *
; * @par
; * THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
; * OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
; * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
; * ARM SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
; * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
; *
; ******************************************************************************/

ROM_ID          EQU     0xxh98xxE100
RUN2SF_FLAG     EQU     0x46533252

;/*
;//-------- <<< Use Configuration Wizard in Context Menu >>> ------------------
;*/

; <h> Stack Configuration
;   <o> Stack Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

Stack_Size      EQU     0x00000800

                AREA    STACK, NOINIT, READWRITE, ALIGN=3
Stack_Mem       SPACE   Stack_Size
__initial_sp


; <h> Heap Configuration
;   <o>  Heap Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

Heap_Size       EQU     0x00000000

                AREA    HEAP, NOINIT, READWRITE, ALIGN=3
__heap_base
Heap_Mem        SPACE   Heap_Size
__heap_limit

; For co_malloc
co_heap_base
co_Heap_Mem     SPACE   0x00000300
co_heap_limit
                EXPORT  co_heap_base
                EXPORT  co_heap_limit

                PRESERVE8
                THUMB


; Flash Vector Table Mapped to Address 0 at Reset (Only work for power on)

                AREA    RESET_FLASH, DATA, READONLY

__VectorsFLASH  DCD     __initial_sp              ; Top of Stack
                DCD     Reset_Handler             ; Reset Handler
                DCD     0xFFFFFFFF                ; NMI Handler
                DCD     HardFault_Handler         ; Hard Fault Handler
                DCD     0xFFFFFFFF                ; MPU Fault Handler
                DCD     0xFFFFFFFF                ; Bus Fault Handler
                DCD     0xFFFFFFFF                ; Usage Fault Handler
                DCD     SoftFault_Handler         ; Xihao assign
                DCD     0xFFFFFFFF                ; Reserved
                DCD     __VectorsFLASH            ; Base address in flash
                DCD     RUN2SF_FLAG               ; Run to flash flag
                DCD     0xFFFFFFFF                ; SVCall Handler
                DCD     0xFFFFFFFF                ; Debug Monitor Handler
                DCD     ROM_ID                    ; @0x34 XH98xx's rom_id
                DCD     0xFFFFFFFF                ; PendSV Handler
                DCD     0xFFFFFFFF                ; SysTick Handler


; RAM Vector Table Mapped to Address 0 at Reset (Real IRQ Vector Table)

                AREA    RESET_RAM, DATA, READONLY
                IMPORT  co_ram_reset_handler
                EXPORT  __Vectors
                EXPORT  __Vectors_End
                EXPORT  __Vectors_Size

__Vectors       DCD     __initial_sp              ; Top of Stack
                DCD     co_ram_reset_handler      ; Reset Handler
                DCD     NMI_Handler               ; NMI Handler
                DCD     HardFault_Handler         ; Hard Fault Handler
                DCD     MemManage_Handler         ; MPU Fault Handler
                DCD     BusFault_Handler          ; Bus Fault Handler
                DCD     UsageFault_Handler        ; Usage Fault Handler
                DCD     SoftFault_Handler         ; Xihao assign
                DCD     0xFFFFFFFF                ; @0x20 SVN reversion
                DCD     0xFFFFFFFF                ; Reserved
                DCD     RUN2SF_FLAG               ; Reserved
                DCD     SVC_Handler               ; SVCall Handler
                DCD     DebugMon_Handler          ; Debug Monitor Handler
                DCD     ROM_ID                    ; @0x34 XH98xx's rid
                DCD     PendSV_Handler            ; PendSV Handler
                DCD     SysTick_Handler           ; SysTick Handler

                ; External Interrupts
                DCD     BT_IRQHandler             ; 0
                DCD     SLEEP_TIMER_IRQHandler    ; 1
                DCD     DMA_IRQHandler            ; 2
                DCD     GPIO_IRQHandler           ; 3
                DCD     TIM_IRQHandler            ; 4
                DCD     MAC98xx_RF_IRQHandler     ; 5
                DCD     MAC98xx_SPI_IRQHandler    ; 6
                DCD     PMU_TIMER_IRQHandler      ; 7
                DCD     IR_TX_IRQHandler	  ; 8
                DCD     UART1_IRQHandler          ; 9
                DCD     EFUSE_IRQHandler          ; 10
                DCD     PIN_WAKEUP_IRQHandler     ; 11
                DCD     ADC_IRQHandler            ; 12
                DCD     I2C_IRQHandler            ; 13
                DCD     SF_IRQHandler             ; 14
                DCD     SOFT0_IRQHandler          ; 15
                DCD     SOFT1_IRQHandler          ; 16
                DCD     SOFT2_IRQHandler          ; 17
                DCD     SOFT3_IRQHandler          ; 18
                DCD     SOFT4_IRQHandler          ; 19
                DCD     SOFT5_IRQHandler          ; 20
                DCD     SOFT6_IRQHandler          ; 21
                DCD     SOFT7_IRQHandler          ; 22
                DCD     VTRACK_IRQHandler         ; 23
                DCD     CRY32M_RDY_IRQHandler     ; 24
                DCD     UART0_IRQHandler          ; 25
                DCD     GPIO0_IRQHandler          ; 26
                DCD     CC_INTR_IRQHandler        ; 27
                DCD     TIM0_IRQHandler           ; 28
                DCD     TIM1_IRQHandler           ; 29
                DCD     TIM2_IRQHandler           ; 30
                DCD     CHARGER_IRQHandler        ; 31
                DCD     AUDIO_IRQHandler     	  ; 32
                DCD     I2S_RX_IRQnIRQHandler     ; 33
                DCD     I2S_TX_IRQnIRQHandler  	  ; 34 


__Vectors_End

__Vectors_Size  EQU     __Vectors_End - __Vectors

                AREA    |.text|, CODE, READONLY


; Reset Handler

Reset_Handler   PROC
                EXPORT  Reset_Handler             [WEAK]
                IMPORT  system_init
                IMPORT  __main
                LDR     R0, =system_init
                BLX     R0
                LDR     R0, =__main
                BX      R0
                ENDP

; Sub main (from __main)
|$Sub$$main|    PROC
                EXPORT  |$Sub$$main|
                IMPORT  system_init_submain
                IMPORT  |$Super$$main|
                ; Do system init
                LDR     R0, =system_init_submain
                BLX     R0
                ; Call main
                LDR     R0, =|$Super$$main|
                BX      R0
                ENDP

; Dummy Exception Handlers (infinite loops which can be modified)

NMI_Handler     PROC
                EXPORT  NMI_Handler               [WEAK]
                B       .
                ENDP
HardFault_Handler\
                PROC
                EXPORT  HardFault_Handler         [WEAK]
                B       .
                ENDP
MemManage_Handler\
                PROC
                EXPORT  MemManage_Handler         [WEAK]
                B       .
                ENDP
BusFault_Handler\
                PROC
                EXPORT  BusFault_Handler          [WEAK]
                B       .
                ENDP
UsageFault_Handler\
                PROC
                EXPORT  UsageFault_Handler        [WEAK]
                B       .
                ENDP
SoftFault_Handler \
                PROC
                EXPORT  SoftFault_Handler         [WEAK]
                B       .
                ENDP
SVC_Handler     PROC
                EXPORT  SVC_Handler               [WEAK]
                B       .
                ENDP
DebugMon_Handler\
                PROC
                EXPORT  DebugMon_Handler          [WEAK]
                B       .
                ENDP
PendSV_Handler\
                PROC
                EXPORT  PendSV_Handler            [WEAK]
                B       .
                ENDP
SysTick_Handler\
                PROC
                EXPORT  SysTick_Handler           [WEAK]
                B       .
                ENDP

Default_Handler PROC
                EXPORT BT_IRQHandler              [WEAK]
                EXPORT SLEEP_TIMER_IRQHandler     [WEAK]
                EXPORT DMA_IRQHandler             [WEAK]
                EXPORT GPIO_IRQHandler            [WEAK]
                EXPORT TIM_IRQHandler             [WEAK]
                EXPORT MAC98xx_RF_IRQHandler      [WEAK]
                EXPORT MAC98xx_SPI_IRQHandler     [WEAK]
                EXPORT PMU_TIMER_IRQHandler       [WEAK]
                EXPORT IR_TX_IRQHandler           [WEAK]
                EXPORT UART1_IRQHandler           [WEAK]
                EXPORT EFUSE_IRQHandler           [WEAK]
                EXPORT PIN_WAKEUP_IRQHandler      [WEAK]
                EXPORT ADC_IRQHandler             [WEAK]
                EXPORT I2C_IRQHandler             [WEAK]
                EXPORT SF_IRQHandler              [WEAK]
                EXPORT SOFT0_IRQHandler           [WEAK]
                EXPORT SOFT1_IRQHandler           [WEAK]
                EXPORT SOFT2_IRQHandler           [WEAK]
                EXPORT SOFT3_IRQHandler           [WEAK]
                EXPORT SOFT4_IRQHandler           [WEAK]
                EXPORT SOFT5_IRQHandler           [WEAK]
                EXPORT SOFT6_IRQHandler           [WEAK]
                EXPORT SOFT7_IRQHandler           [WEAK]
                EXPORT VTRACK_IRQHandler          [WEAK]
                EXPORT CRY32M_RDY_IRQHandler      [WEAK]
                EXPORT UART0_IRQHandler		  [WEAK]
                EXPORT GPIO0_IRQHandler           [WEAK]
                EXPORT CC_INTR_IRQHandler         [WEAK]
                EXPORT TIM0_IRQHandler            [WEAK]
                EXPORT TIM1_IRQHandler            [WEAK]
                EXPORT TIM2_IRQHandler            [WEAK]
                EXPORT CHARGER_IRQHandler         [WEAK]
                EXPORT AUDIO_IRQHandler		  [WEAK]
                EXPORT I2S_RX_IRQnIRQHandler      [WEAK]
                EXPORT I2S_TX_IRQnIRQHandler	  [WEAK]
                
BT_IRQHandler
SLEEP_TIMER_IRQHandler
DMA_IRQHandler
GPIO_IRQHandler
TIM_IRQHandler
MAC98xx_RF_IRQHandler
MAC98xx_SPI_IRQHandler
PMU_TIMER_IRQHandler
IR_TX_IRQHandler 
UART1_IRQHandler
EFUSE_IRQHandler 
PIN_WAKEUP_IRQHandler
ADC_IRQHandler
I2C_IRQHandler  
SF_IRQHandler
SOFT0_IRQHandler
SOFT1_IRQHandler
SOFT2_IRQHandler
SOFT3_IRQHandler
SOFT4_IRQHandler
SOFT5_IRQHandler          
SOFT6_IRQHandler      
SOFT7_IRQHandler 
VTRACK_IRQHandler
CRY32M_RDY_IRQHandler
UART0_IRQHandler 
GPIO0_IRQHandler
CC_INTR_IRQHandler
TIM0_IRQHandler
TIM1_IRQHandler
TIM2_IRQHandler
CHARGER_IRQHandler
AUDIO_IRQHandler
I2S_RX_IRQnIRQHandler
I2S_TX_IRQnIRQHandler
                B       .
                ENDP


                ALIGN


; User Initial Stack & Heap

                IF      :DEF:__MICROLIB

                EXPORT  Stack_Mem
                EXPORT  __initial_sp
                EXPORT  __heap_base
                EXPORT  __heap_limit

                ELSE

                IMPORT  __use_two_region_memory
                EXPORT  __user_initial_stackheap

__user_initial_stackheap PROC
                LDR     R0, =  Heap_Mem
                LDR     R1, = (Stack_Mem + Stack_Size)
                LDR     R2, = (Heap_Mem  + 0)
                LDR     R3, = Stack_Mem
                BX      LR
                ENDP

                ALIGN

                ENDIF


                END

