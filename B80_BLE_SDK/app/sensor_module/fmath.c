
#include "fmath.h"

/**
 * @brief 查表法快速反正切,精度=1°
 * 
 * @param x : 正弦值
 * @param y : 余弦值
 * @return s16: 角度(-127 ~ 127)
 */
s16 fast_arctan2(s16 x,s16 y)
{
    // 正切值-角度对照表
    static const unsigned char arctan_table[] = {
        /* 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64 */
        0, 1, 2, 3, 4, 4, 5, 6, 7, 8, 9, 10,11,11,12,13,14,15,16,17,
        17,18,19,20,21,21,22,23,24,24,25,26,27,27,28,29,29,30,31,31,
        32,33,33,34,35,35,36,36,37,37,38,39,39,40,40,41,41,42,42,43,
        43,44,44,45,45
    };
    // 判断方向
    bool nx = (x < 0),ny = (y < 0);
    if(x < 0)
        x = -x;
    if(y < 0)
        y = -y;
    if(x < y)
        x = 90 - arctan_table[x * 64 / y];//余切值*64
    else
        x = arctan_table[y * 64 / x];//正切值*64
    if(nx) {
        if(180 - x > 127)
            x = 127;
        else
            x = 180 - x;
    }
    if(ny)
        x = -x;
    return x;
}

s16 fast_arccos2(s16 x, u16 y) 
{
    static const u8 arccos_table[] = {
        90,90,90,89,89,89,89,88,88,88,88,88,87,87,87,87,86,86,86,86,
        86,85,85,85,85,84,84,84,84,83,83,83,83,83,82,82,82,82,81,81,
        81,81,81,80,80,80,80,79,79,79,79,78,78,78,78,78,77,77,77,77,
        76,76,76,76,75,75,75,75,75,74,74,74,74,73,73,73,73,72,72,72,
        72,71,71,71,71,71,70,70,70,70,69,69,69,69,68,68,68,68,67,67,
        67,67,66,66,66,66,65,65,65,65,64,64,64,64,63,63,63,63,62,62,
        62,62,61,61,61,61,60,60,60,60,59,59,59,59,58,58,58,58,57,57,
        57,56,56,56,56,55,55,55,55,54,54,54,53,53,53,53,52,52,52,51,
        51,51,51,50,50,50,49,49,49,48,48,48,48,47,47,47,46,46,46,45,
        45,45,44,44,44,43,43,43,43,42,42,41,41,41,40,40,40,39,39,39,
        38,38,38,37,37,36,36,36,35,35,35,34,34,33,33,33,32,32,31,31,
        30,30,29,29,29,28,28,27,27,26,26,25,25,24,23,23,22,22,21,20,
        20,19,18,18,17,16,15,14,13,12,11,10,9,7,5,0
    };

    if (x > y) {
        // errror
        return 0;
    }
    bool nx = (x < 0);
    x = arccos_table[x * 255 / y];

    if (nx) {
        return -x;
    } else {
        return x;
    }
}

// /**
//  * @brief Fast inverse square root
//  * 
//  * @param float:x 
//  * @return float: y =1/√x
//  */
// float fast_inv_sqrt(float x)
// {
// 	union {
// 		float    f;
// 		uint32_t i;
// 	} conv = { .f = x };
// 	conv.i  = 0x5f3759df - (conv.i >> 1);
// 	conv.f *= 1.5F - (x * 0.5F * conv.f * conv.f);
// 	return conv.f;
// }

int fast_int_sqrt(int x)
{
    int v_bit = 15;
    int n = 0; // 结果
    int b = 0x8000; // 对应的位

    if(x <= 1) return x;

    while(b)
    {
        int temp = ((n << 1) + b) << v_bit--;
        if(x >= temp)
        {
            n += b;
            x -= temp;
        }
        b >>= 1;
    }
    return n;
}

