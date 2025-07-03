#include "uart.h";

uint8_t rx_data_len = 4; //�������ݳ��ȣ���ʼ��Ϊ4(ȥ��֡ͷ���������͡����ݳ���)
uint8_t tx_data_len = 0;    //ʵ�ʷ������ݳ���
uint8_t tx_buf_len = 1;

uart_state_t uart_rx_state;
uart_data_type_t uart_data_type;

uint8_t rx_data_num = 0;
uint8_t rx_data[RECMAXLEN] = {0};
uint8_t tx_data[SENDMAXLEN] = {0};

 /*-------------------------------------------------
 * ��������uart_initial
 * ���ܣ�  ��ʼ������
 * ���룺  ��
 * �����  ��
 --------------------------------------------------*/
void uart_init(void)
 {
 
    PCKEN|=0B00100000;			//ʹ��UART1ģ��ʱ��
    UR1IER=0B00100001;			//ʹ�ܷ�������жϣ�ʹ�ܽ��������ж�
    UR1LCR=0B00000001;			//8λ���ݳ��ȣ�1λֹͣλ������żУ��λ
    UR1MCR=0B00011000;			//ʹ�ܷ��ͺͽ��սӿ�
       
    UR1DLL=18;					//������=Fmaster/(16*{URxDLH,URxDLL})=56000, Fmaster:16MHZ
    UR1DLH=0;  
    UR1TCF=1;                   //���ݷ�����ɱ�־(дһ����)
 }

 /*-------------------------------------------------
 * ��������check_rx_data_type
 * ���ܣ�  �ж����������Ƿ�Ϊ��֪����
 * ���룺  ��
 * �����  true:��֪��false��δ֪
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
 * ��������uart_send_packet
 * ���ܣ�  ��ȡ��ת��Ҫ���ص�����
 * ���룺  ��
 * �����  ��
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
 * ��������uart_read_len
 * ���ܣ�  �ݴ���յ�������
 * ���룺  ��
 * �����  true:�ɹ���false��ʧ��
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
 * ��������check_rx_data
 * ���ܣ�  ͨ��У��λ�ж����ݴ����Ƿ��쳣
 * ���룺  ��
 * �����  true:���쳣��false���쳣
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
 * ��������uart_rx_isr
 * ���ܣ�  ���ݽ����¼�
 * ���룺  ��
 * �����  ��
 --------------------------------------------------*/
void uart_rx_isr(void)
{
    if ((UR1DATAL == 0xAA) && (uart_rx_state == UART_STATE_IDLE)) //������ݽ����жϣ�������յ��쳣���ݵ������ݷ����쳣
    {
        uart_rx_state = UART_STATE_HEAD;
        rx_data_len = 4;   //�������ݳ���
        rx_data[0] = UR1DATAL;   //����֡ͷ����1
        system_tick_exceed(50);     //50ms��δ���յ��������жϣ�����
    }
    else if (uart_rx_state == UART_STATE_HEAD)
    {
        if (UR1DATAL == 0xBB)
        {
            uart_rx_state = UART_STATE_TYPE;
            rx_data[1] = UR1DATAL;   //����֡ͷ����2
        }
        else
        {
            uart_rx_state = UART_STATE_IDLE;
        }
        system_tick_exceed(50);
    }
    else if (uart_rx_state == UART_STATE_TYPE)
    {
        rx_data[2] = UR1DATAL;   //������������
        if (check_rx_data_type())   //�ж����������Ƿ��Ǽȶ�����
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
        rx_data[3] = UR1DATAL;    //�������ݳ���
        system_tick_exceed(50);
    }
    else if (uart_rx_state == UART_STATE_DATA)
    {
        system_tick_exceed(50);
        if (uart_data_type == DATA_TYPE_WRITE)
        {
            rx_data_num ++;
            rx_data[rx_data_len] = UR1DATAL;
            if ((rx_data_num - 1) == rx_data[3])  //�ж������Ƿ������
            {
                if (check_rx_data())
                {
                    uart_rx_state = UART_STATE_SUCCESS;
                }
                else
                {
                    rx_data_len = 4;   //�������ݳ���
                    uart_rx_state = UART_STATE_IDLE;
                }
                rx_data_num = 0;
                exit_system_timer();
            }
            else if ((rx_data_len + 1) > RECMAXLEN)    //���ݳ���Ԥֵ������
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
            rx_data[3] = 0;    //У��λ��0
            for (uint8_t i = 0; i < 3; i++)   //ͨ��У��λ�ж����ݴ����Ƿ��쳣
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
 * ��������uart_tx_isr
 * ���ܣ�  ���ݷ����¼�
 * ���룺  ��
 * �����  ��
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
 * ��������send_nec_finish
 * ���ܣ�  ������nec��ֵ���ݺ󷵻ط������
 * ���룺  ��
 * �����  ��
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
 * ��������send_unicersal_finish
 * ���ܣ�  ������������ܼ�ֵ���ݺ󷵻ط������
 * ���룺  ��
 * �����  ��
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
