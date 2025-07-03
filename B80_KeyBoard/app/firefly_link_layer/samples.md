## 1. 设备端任意时刻单次发送,并确认发送结果

````c

bool fyll_sample_send_one_packet()
{   
    static unsigned long tick_for_tx = 0;

    uint8_t fyll_test_data[32] = {0,1,2,3,4,5,6,7};

    // 1. 待发送数据写入发送缓冲区,并启动发送
    if (fyll_tx_data(fyll_test_data, 32) != true)
    {
        // 无法启动发送
        //  1. 参数错误
        //  2. 发送缓冲区已满

        return false;   
    }

    // 2. 等待发送完成,或超时退出.FYLL会自动在多个频道尝试发送多次,直到发送成功或者超过次数限制,进入空闲状态
    while (fyll_get_state() != FYLL_IDLE)
    {
        ;
    }

    // 3. 检查发送结果
    if (fyll_tx_success()) 
    {
        printf("fyll device tx 32 bytes success!\r\n");
    }
    else
    {
        printf("fyll device tx failed!\r\n");
    }
}
````

## 2. 设备端任意时刻批量发送,并确认发送结果

````c

bool fyll_sample_send_bulk()
{   
    static unsigned long tick_for_tx = 0;

    uint8_t fyll_test_data[1000] = {0,1,2,3,4,5,6,7};
    uint16_t fyll_test_data_len = 1000;

    bool res;
    // 1. 待发送数据写入发送缓冲区,并启动发送
    uint16_t tx_bytes;
    do 
    {   
        tx_bytes = fyll_test_data_len > 32 ? 32 : fyll_test_data_len

        if (fyll_tx_data(fyll_test_data + index, tx_bytes))
        {
            fyll_test_data_len -= tx_bytes;
        }
        else
        {

        }
    }while(fyll_text_data_len != 0);


    // 2. 等待发送完成,或超时退出.FYLL会自动在多个频道尝试发送多次,直到发送成功或者超过次数限制,进入空闲状态
    while (fyll_get_state() != FYLL_IDLE)
    {
        ;
    }

    // 3. 检查发送结果
    if (fyll_tx_success()) 
    {
        printf("fyll device tx bytes success!\r\n");
    }
    else
    {
        printf("fyll device tx failed!\r\n");
    }
}
````