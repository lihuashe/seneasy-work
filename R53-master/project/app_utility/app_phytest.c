/******************************************************************************
 * @file     app_phytest.c
 *
 * @brief    for TLSR chips
 *
 * @author   public@telink-semi.com;
 * @date     Sep. 30, 2010
 *
 * @attention
 *
 *  Copyright (C) 2019-2020 Telink Semiconductor (Shanghai) Co., Ltd.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *****************************************************************************/

#include "tl_common.h"
#include "drivers.h"
#include "stack/ble/ble.h"
#include "../common/blt_led.h"

#if !CLOSE_CODE_RAM
#define UART_DATA_LEN    64      // data max 252
typedef struct{
    unsigned int len;        // data max 252
    unsigned char data[UART_DATA_LEN];
}uart_data_t;

//MYFIFO_INIT(hci_rx_fifo, 20, 2);
//MYFIFO_INIT(hci_tx_fifo, 20, 2);

#define HCI_RX_FIFO_SIZE             (20)
#define HCI_RX_FIFO_NUM              (2)

#define HCI_TX_FIFO_SIZE             (20)
#define HCI_TX_FIFO_NUM              (2)

extern u8 blt_buff_prepare_write[];
#define hci_rtxfifo_b                    (blt_buff_prepare_write + 4)    

my_fifo_t hci_rx_fifo = {
                                                    HCI_RX_FIFO_SIZE,
                                                    HCI_RX_FIFO_NUM,
                                                    0,
                                                    0,
                                                    hci_rtxfifo_b,};

my_fifo_t hci_tx_fifo = {
                                                    HCI_TX_FIFO_SIZE,
                                                    HCI_TX_FIFO_NUM,
                                                    0,
                                                    0,
                                                    hci_rtxfifo_b + (HCI_RX_FIFO_SIZE*HCI_RX_FIFO_NUM),};
#else
my_fifo_t hci_rx_fifo = {0};
my_fifo_t hci_tx_fifo = {0};

#endif

#if (BLE_PHYTEST_MODE != PHYTEST_MODE_DISABLE )

#define UART_DATA_LEN    64      // data max 252
typedef struct{
    unsigned int len;        // data max 252
    unsigned char data[UART_DATA_LEN];
}uart_data_t;

//MYFIFO_INIT(hci_rx_fifo, 20, 2);
//MYFIFO_INIT(hci_tx_fifo, 20, 2);

#define HCI_RX_FIFO_SIZE             (20)
#define HCI_RX_FIFO_NUM              (2)

#define HCI_TX_FIFO_SIZE             (20)
#define HCI_TX_FIFO_NUM              (2)

extern u8 blt_buff_prepare_write[];
#define hci_rtxfifo_b                    (blt_buff_prepare_write + 4)                                // 0x100


my_fifo_t hci_rx_fifo = {
                                                    HCI_RX_FIFO_SIZE,
                                                    HCI_RX_FIFO_NUM,
                                                    0,
                                                    0,
                                                    hci_rtxfifo_b,};

my_fifo_t hci_tx_fifo = {
                                                    HCI_TX_FIFO_SIZE,
                                                    HCI_TX_FIFO_NUM,
                                                    0,
                                                    0,
                                                    hci_rtxfifo_b + (HCI_RX_FIFO_SIZE*HCI_RX_FIFO_NUM),};


extern void blc_phy_preamble_length_set(unsigned char len);





#if (BLE_PHYTEST_MODE == PHYTEST_MODE_OVER_HCI_WITH_UART)
    int rx_from_uart_cb (void)
    {
        if(my_fifo_get(&hci_rx_fifo) == 0)
        {
            return 0;
        }

        u8* p = my_fifo_get(&hci_rx_fifo);
        u32 rx_len = p[0]; //usually <= 255 so 1 byte should be sufficient

        if (rx_len)
        {
            blc_hci_handler(&p[4], rx_len - 4);
            my_fifo_pop(&hci_rx_fifo);
        }

        return 0;




    }


    int tx_to_uart_cb (void)
    {
        uart_data_t T_txdata_buf;
        static u32 uart_tx_tick = 0;

        u8 *p = my_fifo_get (&hci_tx_fifo);

        #if (ADD_DELAY_FOR_UART_DATA)
            if (p && !uart_tx_is_busy () && clock_time_exceed(uart_tx_tick, 30000))
        #else
            if (p && !uart_tx_is_busy ())
        #endif
        {
            memcpy(&T_txdata_buf.data, p + 2, p[0]+p[1]*256);
            T_txdata_buf.len = p[0]+p[1]*256 ;

            uart_dma_send((unsigned char*)&T_txdata_buf);
            my_fifo_pop (&hci_tx_fifo);
            uart_tx_tick = clock_time();
        }
        return 0;
    }
#endif




_attribute_ram_code_ void irq_phyTest_handler(void)
{
#if(FEATURE_TEST_MODE == TEST_BLE_PHY)
    unsigned char uart_dma_irqsrc;
    //1. UART irq
    uart_dma_irqsrc = dma_chn_irq_status_get();///in function,interrupt flag have already been cleared,so need not to clear DMA interrupt flag here
    if(uart_dma_irqsrc & FLD_DMA_CHN_UART_RX)
    {
        dma_chn_irq_status_clr(FLD_DMA_CHN_UART_RX);
        u8* w = hci_rx_fifo.p + (hci_rx_fifo.wptr & (hci_rx_fifo.num-1)) * hci_rx_fifo.size;
        if(w[0]!=0)
        {
            my_fifo_next(&hci_rx_fifo);
            u8* p = hci_rx_fifo.p + (hci_rx_fifo.wptr & (hci_rx_fifo.num-1)) * hci_rx_fifo.size;
            reg_dma0_addr = (u16)((u32)p);
        }
    }
    if(uart_dma_irqsrc & FLD_DMA_CHN_UART_TX){
        dma_chn_irq_status_clr(FLD_DMA_CHN_UART_TX);
    }
#endif
}



extern const led_cfg_t led_cfg[];

void app_trigger_phytest_mode(void)
{
    static u8 phyTestFlag = 0;
    if(!phyTestFlag && blc_ll_getCurrentState() != BLS_LINK_STATE_CONN){  //can not enter phytest in connection state
        phyTestFlag = 1;
        device_led_setup(led_cfg[4]);  //LED_SHINE_FAST: 4


        //adjust some rf parameters here if needed
        write_reg8(0x402, 0x2b);   //must: adjust rf packet preamble for BQB
        blc_phy_setPhyTestEnable( BLC_PHYTEST_ENABLE );
    }

}






void app_phytest_init(void)
{
    blc_phy_initPhyTest_module();
    blc_phy_preamble_length_set(11);

    #if(BLE_PHYTEST_MODE == PHYTEST_MODE_THROUGH_2_WIRE_UART || BLE_PHYTEST_MODE == PHYTEST_MODE_OVER_HCI_WITH_UART)  //uart
        uart_gpio_set(UART_TX_PB1, UART_RX_PB0);
        uart_reset();
    #endif

//    uart_recbuff_init((unsigned char*)hci_rx_fifo_b, hci_rx_fifo.size);
    uart_recbuff_init((unsigned char*)hci_rtxfifo_b, hci_rx_fifo.size);


    #if (CLOCK_SYS_CLOCK_HZ == 16000000)
        uart_init(9,13,PARITY_NONE, STOP_BIT_ONE); //baud rate: 115200
    #elif (CLOCK_SYS_CLOCK_HZ == 24000000)
        uart_init(12,15,PARITY_NONE, STOP_BIT_ONE); //baud rate: 115200
    #endif


    uart_dma_enable(1,1);

    irq_set_mask(FLD_IRQ_DMA_EN);
    dma_chn_irq_enable(FLD_DMA_CHN_UART_RX | FLD_DMA_CHN_UART_TX, 1);       //uart Rx/Tx dma irq enable
    uart_irq_enable(1,0);

    #if    (BLE_PHYTEST_MODE == PHYTEST_MODE_THROUGH_2_WIRE_UART)
        blc_register_hci_handler (phy_test_2_wire_rx_from_uart, phy_test_2_wire_tx_to_uart);
    #elif(BLE_PHYTEST_MODE == PHYTEST_MODE_OVER_HCI_WITH_UART)
        blc_register_hci_handler (rx_from_uart_cb, tx_to_uart_cb);        //default handler
    #endif

}








#endif  //end of  BLE_PHYTEST_MODE != PHYTEST_MODE_DISABLE
