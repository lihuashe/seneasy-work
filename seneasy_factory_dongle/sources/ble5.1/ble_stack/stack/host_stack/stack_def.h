


#ifndef _STACK_DEF_H_
#define _STACK_DEF_H_

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#if defined(__MAIN_PROCESSOR)
    #include "ARMCM33_DSP_FP.h"
#elif defined(__CO_PROCESSOR)
    #include "ARMCM0plus.h"
    #include "system_ARMCM0plus.h"
#else
    #error "CORE unset"
#endif

#include "rtx_os.h"
#include "cmsis_os2.h"

typedef   signed          char int8;
typedef   signed short     int int16;
typedef   signed           int int32;
//typedef   signed       __int64 int64;

typedef unsigned          char uint8;
typedef unsigned short     int uint16;
typedef unsigned           int uint32;
//typedef unsigned       __int64 uint64;


#define BOOT_RET_RAM_AREA       ((section(".boot.retention_ram")))
#define BOOT_RET_RAM_AREA_ZERO  ((section(".bss.boot.retention.ram")))

#define ST_RET_RAM              ((section(".stack.retention.ram")))
#define ST_RET_RAM_ZERO         ((section(".bss.stack.retention.ram")))

#define LLC_RET_RAM             ((section(".llc.retention.ram")))
#define LLC_RET_RAM_ZERO        ((section(".bss.llc.retention.ram")))

#define SYS_RET_RAM             ((section(".sys.retention.ram")))
#define SYS_RET_RAM_ZERO        ((section(".bss.sys.retention.ram")))

#define ENABLE_LE_SECURE_CONNECTIONS
#define DEFINE_CMAC_AND_SIGNED_WRITE

#ifndef false
#define false                   0
#endif

#ifndef true
#define true                    1
#endif

#endif
