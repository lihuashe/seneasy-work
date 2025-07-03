
#ifndef SRC_RF_TEST_H_
#define SRC_RF_TEST_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "main.h"
#include "key_evt_handle.h"
#include "led_evt.h"
#include "app.h"
#include "my_log.h"
#include "time_utils.h"
#include "led_bar.h"
#include "2_4g_io.h"
#include "led.h"
#include "pm.h"
#include "app.h"
#include "flash_io.h"
    static uint8_t a7535_TxBuf[] = {0xAA, 0xAA, 0xAA, 0xBB, 0xCC, 0xDD, 0x0d, 0x6c, 0x4a, 0x79, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x04, 0xff, 0xff, 0x04, 0xff, 0x3a};
    u8 xn297l_TxBuf[] = {0x18, 0xAA, 0xAA, 0xAA, 0xBB, 0xCC, 0xDD, 0x7E, 0x00, 0x00, 0x00, 0x04, 0x00, 0x03, 0x01, 0x06, 0x00, 0x1E, 0xFF, 0xFF, 0xFF, 0x1B, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xBB};

    static void rf_test()
    {
        static u32 clock_t;
        static u32 i = 0;
        if (!clock_time_exceed(clock_t, 1000))
            return;

        // if(i)
        // {
        //     PAN216_SetUpConfig((RFConfig_t *)&a7535_gRfConfig);
        //     delay_us(120);
        //     rf_2_4g_sendto(a7535_TxBuf, sizeof(a7535_TxBuf));
        // }
        // else
        // {
        //     delay_us(120);
        //     PAN216_SetUpConfig((RFConfig_t *)&xn297l_gRfConfig);
        //     rf_2_4g_sendto(xn297l_TxBuf, sizeof(xn297l_TxBuf));
        // }
        // i++;
        // i %= 2;
        // PAN216_SetUpConfig((RFConfig_t *)&xn297l_gRfConfig);
        // rf_2_4g_sendto(xn297l_TxBuf, sizeof(xn297l_TxBuf));
        clock_t = clock_time();
    }

    static void pm_vcc_test()
    {
        static u32 clock_t;
        static u32 i = 0;
        if (!clock_time_exceed(clock_t, 1000))
            return;
#if __DEBUG__ >= __DEBUG_DEV__
        my_log(">D%d[%s] VCC : %d mV\n", __LINE__, __FUNCTION__, pm_VCC());
#endif
        clock_t = clock_time();
    }

    static void flash_test()
    {
        static u32 clock_t;
        static u32 i = 0;
        if (!clock_time_exceed(clock_t, 1000))
            return;
        flash_save();
        flash_load();
        clock_t = clock_time();
    }

    static void test_main()
    {
        // rf_test();
        // pm_vcc_test();
        // flash_test(); 
    }

#ifdef __cplusplus
}
#endif

#endif // SRC_RF_TEST_H_
