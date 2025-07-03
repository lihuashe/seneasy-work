;/**
; *@file     utility_asm.s
; *@author   kang.chen
; *@version  V1.0
; *@date     2020/07/16
; * @history
; * @note
; * @attention
; *
; * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS WITH CODING INFORMATION REGARDING THEIR
; * PRODUCTS IN ORDER FOR THEM TO SAVE TIME. AS A RESULT, MAXSCEND SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
; * INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE
; * USE MADE BY CUSTOMERS OF THE CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
; *
; * Copyright (c) 2016~2021, Maxscend Microelectronics Company Limited.
; * All rights reserved.
; */

    PRESERVE8
    THUMB

    ; Cortex-M33
    IF  :DEF:__MAIN_PROCESSOR
    AREA |.text|, CODE, READONLY

    ; void delay_cycles(uint32_t cycles)
rom_delay_cycles  PROC
    EXPORT  rom_delay_cycles
    ; cycles /= 4
    LSR R0,R0,#2
    
    ; if 0 == cycles
    CMP R0,#0
    BEQ RET
    
LOOP    ;4 cycles
    MOV     R0,R0
    SUBS    R0,#1
    BNE     LOOP
    
RET
    BX      LR
    ENDP

    ; Cortex-M0P
    ELIF :DEF:__CO_PROCESSOR
    AREA |.text|, CODE, READONLY
 
    ; void delay_cycles(uint32_t cycles)
rom_delay_cycles  PROC
    EXPORT  rom_delay_cycles

    ; cycles /= 4 
    LSRS    R0,R0,#2

    ; if 0 == cycles
    CMP R0,#0
    BEQ RET

LOOP    ;4 cycles
    MOV     R0,R0
    SUBS    R0,#1
    BNE     LOOP

RET
    BX      LR
    ENDP

    ELSE

    ASSERT  (0>0)  ; unknown core

    ENDIF

    END
