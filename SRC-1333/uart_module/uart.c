#include "uart.h";

uint8_t rx_data_len = 4; //接收数据长度，初始化为4(去掉帧头及数据类型、数据长度)
uint8_t tx_data_len = 0;    //实际发送数据长度
uint8_t tx_buf_len = 1;

uart_state_t uart_rx_state;
uart_data_type_t uart_data_type;

uint8_t rx_data_num = 0;
uint8_t rx_data[RECMAXLEN] = {0};
uint8_t tx_data[SENDMAXLEN] = {0};

 /*-------------------------------------------------
 * 函数名：uart_initial
 * 功能：  初始化串口
 * 输入：  无
 * 输出：  无
 --------------------------------------------------*/
void uart_init(void)
 {
 
    PCKEN|=0B00100000;			//使能UART1模块时钟
    UR1IER=0B00100001;			//使能发送完成中断，使能接收数据中断
    UR1LCR=0B00000001;			//8位数据长度，1位停止位，无奇偶校验位
    UR1MCR=0B00011000;			//使能发送和接收接口
       
    UR1DLL=18;					//波特率=Fmaster/(16*{URxDLH,URxDLL})=56000, Fmaster:16MHZ
    UR1DLH=0;  
    UR1TCF=1;                   //数据发送完成标志(写一清零)
 }

 /*-------------------------------------------------
 * 函数名：check_rx_data_type
 * 功能：  判断数据类型是否为已知类型
 * 输入：  无
 * 输出：  true:已知，false：未知
 --------------------------------------------------*/
static uint8_t check_rx_data_type(void)
{
    if ((rx_data[2] == 0x81) || (rx_data[2] == 0x82) || (rx_data[2] == 0x83)
        || (rx_data[2] == 0x84) || (rx_data[2] == 0x90) || (rx_data[2] == 0x91))
    {
        uart_data_type = DATA_TYPE_WRITE;
        return true;
    }
    else if ((rx_data[2] == 0x00) || (rx_data[2] == 0x02))
    {
        uart_data_type = DATA_TYPE_READ;
        return true;
    }
    else
    {
        return false;
    }
    
}

 /*-------------------------------------------------
 * 函数名：uart_send_packet
 * 功能：  读取并转译要返回的数据
 * 输入：  无
 * 输出：  无
 --------------------------------------------------*/
void uart_send_packet(uint8_t * packet, uint8_t len)
{
    uint8_t checksum = 0x00;
    uint8_t z = 0;
    uint8_t j = 2;

    tx_data[0] = 0xBB;
    tx_data[1] = 0xAA;
    for (uint8_t i = 0; i < len; i++)
    {
        tx_data[i + 2] = packet[i];
    }

    for (uint8_t p = 0; p < (len + 2); p++)
    {
        checksum = checksum + tx_data[p];
    }
    
    for (z = 0; z < len; z++)
    {
        if (packet[z] == 0xA5 || 
            packet[z] == 0xAA || 
            packet[z] == 0xBB)
        {
            tx_data[j] = 0xA5;
            j++;
        }
        tx_data[j] = packet[z];
        j++;
    }

    if (checksum == 0x5A || 
        checksum == 0xAA || 
        checksum == 0xBB) 
    {
        tx_data[j] = 0xA5;
        j++;
        tx_data[j] = checksum;
        j++;
    }
    else
    {
        tx_data[j] = checksum;
    }

    tx_data_len = j;
    
    UR1DATAL = tx_data[0];
}

 /*-------------------------------------------------
 * 函数名：uart_read_len
 * 功能：  暂存接收到的数据
 * 输入：  无
 * 输出：  true:成功，false：失败
 --------------------------------------------------*/
uint8_t uart_read_len()
{
    if (uart_rx_state == UART_STATE_SUCCESS)
    {
        uart_rx_state = UART_STATE_IDLE;
        return rx_data_len;
    }
    else
    {
        return 0;
    }
}

 /*-------------------------------------------------
 * 函数名：check_rx_data
 * 功能：  通过校验位判断数据传输是否异常
 * 输入：  无
 * 输出：  true:无异常，false：异常
 --------------------------------------------------*/
static uint8_t check_rx_data(void)
{
    uint8_t check_data = 0;
    for (uint8_t i = 0; i < rx_data_len; i++)
    {
        check_data = check_data + rx_data[i];
    }
    
    if (rx_data[rx_data_len] == check_data)
    {
        return true;
    }
    else
    {
        return false;
    }
    
}

 /*-------------------------------------------------
 * 函数名：uart_rx_isr
 * 功能：  数据接收事件
 * 输入：  无
 * 输出：  无
 --------------------------------------------------*/
void uart_rx_isr(void)
{
    if ((UR1DATAL == 0xAA) && (uart_rx_state == UART_STATE_IDLE)) //添加数据接收判断，避免接收到异常数据导致数据发送异常
    {
        uart_rx_state = UART_STATE_HEAD;
        rx_data_len = 4;   //重置数据长度
        rx_data[0] = UR1DATAL;   //保存帧头数据1
        system_tick_exceed(50);     //50ms内未接收到新数据中断，则丢弃
    }
    else if (uart_rx_state == UART_STATE_HEAD)
    {
        if (UR1DATAL == 0xBB)
        {
            uart_rx_state = UART_STATE_TYPE;
            rx_data[1] = UR1DATAL;   //保存帧头数据2
        }
        else
        {
            uart_rx_state = UART_STATE_IDLE;
        }
        system_tick_exceed(50);
    }
    else if (uart_rx_state == UART_STATE_TYPE)
    {
        rx_data[2] = UR1DATAL;   //保存数据类型
        if (check_rx_data_type())   //判断数据类型是否是既定类型
        {
            if (uart_data_type == DATA_TYPE_WRITE)
            {
                uart_rx_state = UART_STATE_LEN;
            }
            else
            {
                uart_rx_state = UART_STATE_DATA;
            }
        }
        else
        {
            uart_rx_state = UART_STATE_IDLE;
        }
        system_tick_exceed(50);
    }
    else if (uart_rx_state == UART_STATE_LEN)
    {
        uart_rx_state = UART_STATE_DATA;
        rx_data[3] = UR1DATAL;    //保存数据长度
        system_tick_exceed(50);
    }
    else if (uart_rx_state == UART_STATE_DATA)
    {
        system_tick_exceed(50);
        if (uart_data_type == DATA_TYPE_WRITE)
        {
            rx_data_num ++;
            rx_data[rx_data_len] = UR1DATAL;
            if ((rx_data_num - 1) == rx_data[3])  //判断数据是否传输结束
            {
                if (check_rx_data())
                {
                    uart_rx_state = UART_STATE_SUCCESS;
                }
                else
                {
                    rx_data_len = 4;   //重置数据长度
                    uart_rx_state = UART_STATE_IDLE;
                }
                rx_data_num = 0;
                exit_system_timer();
            }
            else if ((rx_data_len + 1) > RECMAXLEN)    //数据超出预值，则丢弃
            {
                rx_data_len = 4;
                rx_data_num = 0;
                uart_rx_state = UART_STATE_IDLE;
                exit_system_timer();
            }
            if (uart_rx_state == UART_STATE_DATA)
            {
                rx_data_len ++;
            }
        }
        else
        {
            rx_data[3] = 0;    //校验位清0
            for (uint8_t i = 0; i < 3; i++)   //通过校验位判断数据传输是否异常
            {
                rx_data[3] = rx_data[3] + rx_data[i];
            }
            if (rx_data[3] == UR1DATAL)
            {
                uart_rx_state = UART_STATE_SUCCESS;
            }
            else
            {
                uart_rx_state = UART_STATE_IDLE;
            }
            exit_system_timer();
        }
    }
}

 /*-------------------------------------------------
 * 函数名：uart_tx_isr
 * 功能：  数据发送事件
 * 输入：  无
 * 输出：  无
 --------------------------------------------------*/
void uart_tx_isr(void)
{
    if (tx_buf_len < (tx_data_len + 1))
    {
        UR1DATAL = tx_data[tx_buf_len];
        tx_buf_len ++;
    }
    else
    {
        tx_buf_len = 1;
    }
}

 /*-------------------------------------------------
 * 函数名：send_nec_finish
 * 功能：  发送完nec键值数据后返回发送完成
 * 输入：  无
 * 输出：  无
 --------------------------------------------------*/
void send_nec_finish(void)
{
    tx_data[0] = 0xBB;
    tx_data[1] = 0xAA;
    tx_data[2] = 0x00;
    tx_data[3] = 0x01;
    tx_data[4] = 0x00;
    tx_data[5] = 0x66;

    tx_data_len = 5;
    UR1DATAL = tx_data[0];
}

 /*-------------------------------------------------
 * 函数名：send_unicersal_finish
 * 功能：  发送完红外万能键值数据后返回发送完成
 * 输入：  无
 * 输出：  无
 --------------------------------------------------*/
void send_unicersal_finish(void)
{
    tx_data[0] = 0xBB;
    tx_data[1] = 0xAA;
    tx_data[2] = 0x90;
    tx_data[3] = 0x01;
    tx_data[4] = 0x02;
    tx_data[5] = 0xF8;

    tx_data_len = 5;
    UR1DATAL = tx_data[0];
}
