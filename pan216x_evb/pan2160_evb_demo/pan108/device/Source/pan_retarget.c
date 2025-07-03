/**************************************************************************//**
 * @file     retarget.c
 * @version  V1.00
 * $Revision: 2 $
 * $Date: 15/02/06 7:41p $
 * @brief    Panchip Series retarget source file
 *
 * @note
 * Copyright (C) 2022 Panchip Technology Corp. All rights reserved.
*****************************************************************************/
#include <stdio.h>
#include "PanSeries.h"

#if defined( __CC_ARM)
#if (__ARMCC_VERSION < 400000)
#else
/* Insist on keeping widthprec, to avoid X propagation by benign code in C-lib */
#pragma import _printf_widthprec
#endif
#endif

#define DEBUG_ENABLE_UART1

#ifdef DEBUG_ENABLE_UART1
#define DEBUG_PORT         UART1
#else
#define DEBUG_PORT         UART0
#endif

enum { r0, r1, r2, r3, r12, lr, pc, psr};

void stackDump(uint32_t stack[])
{
    printf("r0  = 0x%x\r\n", stack[r0]);
    printf("r1  = 0x%x\r\n", stack[r1]);
    printf("r2  = 0x%x\r\n", stack[r2]);
    printf("r3  = 0x%x\r\n", stack[r3]);
    printf("r12 = 0x%x\r\n", stack[r12]);
    printf("lr  = 0x%x\r\n", stack[lr]);
    printf("pc  = 0x%x\r\n", stack[pc]);
    printf("psr = 0x%x\r\n", stack[psr]);
}

void Hard_Fault_Handler(uint32_t stack[])
{
    printf("In Hard Fault Handler\r\n");

    stackDump(stack);

    while(1);
}

#if defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6100100)
void HardFault_Handler(void)
{
    __asm("MOVS    r0, #4                        \n"
        "MOV     r1, LR                        \n"
        "TST     r0, r1                        \n"
        "BEQ     Stack_Use_MSP                 \n"
        "MRS     R0, PSP                       \n" //; stack use PSP
        "B       Get_LR_and_Branch             \n"
        "Stack_Use_MSP:                        \n"
        "MRS     R0, MSP                       \n" //; stack use MSP
        "Get_LR_and_Branch:                    \n"
        "MOV     R1, LR                        \n" //; LR current value
        "B Hard_Fault_Handler                  \n"
       );
}
#else
__asm int32_t HardFault_Handler(void)
{
    MOVS    r0, #4
    MOV     r1, LR
    TST     r0, r1
    BEQ     Stack_Use_MSP
    MRS     R0, PSP ;stack use PSP
    B       Get_LR_and_Branch
Stack_Use_MSP
    MRS     R0, MSP ; stack use MSP
Get_LR_and_Branch
    MOV     R1, LR ; LR current value
    LDR     R2,=__cpp(Hard_Fault_Handler)
    BX      R2
}
#endif


/**
  * @brief  Write a char to UART.
  * @param  ch The character sent to UART.
  * @return None
  */

void SendChar_ToUART(int ch)
{
    while(UART_IsTxFifoFull(DEBUG_PORT));
    DEBUG_PORT->RBR_THR_DLL = ch;
}


/**
  * @brief  Write a char to debug console.
  * @param  ch The character sent to debug console
  * @return None
  */
__WEAK void SendChar(int ch)
{
   SendChar_ToUART(ch);
}


/**
  * @brief  Read a char from debug console.
  * @param  None
  * @return Received character from debug console
  * @note   This API waits until UART debug port or semihost input a character
  */
__WEAK char GetChar(void)
{
    while (1)
    {
        if((DEBUG_PORT->USR & UART_USR_RFNE_Msk))
        {
            return (DEBUG_PORT->RBR_THR_DLL);
        }
    }
}


/*--------------------------------------------------*/
/*           C library retargetting                 */
/*--------------------------------------------------*/
void _ttywrch(int ch)
{
    SendChar(ch);
}

int fputc(int ch, FILE *f)
{
    SendChar(ch);
    return ch;
}

int fgetc(FILE *f) {
    return (GetChar());
}

int ferror(FILE *f) {
    return EOF;
}
