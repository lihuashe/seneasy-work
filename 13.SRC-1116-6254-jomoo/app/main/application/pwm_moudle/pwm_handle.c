#include "pwm_handle.h"
#include "driver.h"

volatile u32 led_pwm_freq[6];
_attribute_data_retention_ u32 led_pwm_time = 0;		   // 配对中led闪烁时间戳
_attribute_data_retention_ u32 led_pwm_all_blink_time = 0; // 所有led闪烁时间戳
_attribute_data_retention_ u8 led_pwm_blink_flag = 0;
volatile _attribute_data_retention_ u8 led_pwm_status = 0;
u32 gpio_pins[] = {GPIO_PD6, GPIO_PD7, GPIO_PD2, GPIO_PD1, GPIO_PD0};

void set_led_brightness()
{
	int voltage_level = 0;
	// 适配43R的保护电阻
	int target_duty_cycles[NUM_LEVELS][6] = {
		{1000, 1, 1, 1, 1, 1},			 //  电压等级0 1.7V
		{1000, 600, 450, 260, 150, 1},	 //  电压等级1
		{1000, 660, 490, 300, 170, 1},	 //  电压等级2
		{1000, 720, 560, 360, 200, 1},	 //  电压等级3
		{1000, 735, 570, 410, 220, 5},	 //  电压等级4
		{1000, 740, 580, 420, 230, 10},	 //  电压等级5
		{1000, 750, 590, 430, 270, 25},	 //  电压等级6
		{1000, 760, 620, 460, 280, 80},	 //  电压等级7
		{1000, 770, 630, 480, 330, 165}, //  电压等级8
		{1000, 780, 665, 520, 400, 280}, //  电压等级9
		{1000, 820, 710, 610, 460, 350}, //  电压等级10
		{1000, 840, 750, 640, 530, 420}, //  电压等级11
		{1000, 865, 790, 690, 590, 490}, //  电压等级12
		{1000, 870, 800, 720, 650, 580}, //  电压等级13
		{1000, 890, 840, 770, 710, 650}, //  电压等级14 3.2V
		{1000, 900, 850, 800, 730, 670}, //  电压等级15
		{1000, 910, 860, 810, 750, 690}, //  电压等级16
		{1000, 935, 895, 845, 790, 740}, //  电压等级17
										 // 0led, 1led,2led,3led,4led,5led
	};

	vbat_voltage = adc_sample_and_get_result();
	short voltage_range = (MAX_VOLTAGE - MIN_VOLTAGE) / NUM_LEVELS;

	//确保电压在合理范围内
	if (vbat_voltage < MIN_VOLTAGE)
		vbat_voltage = MIN_VOLTAGE;
	else if (vbat_voltage > MAX_VOLTAGE)
		vbat_voltage = MAX_VOLTAGE;
	//计算电压所在的档位
	voltage_level = (vbat_voltage - MIN_VOLTAGE) / voltage_range;

	for (int i = 0; i < 6; i++)
		led_pwm_freq[i] = target_duty_cycles[voltage_level][i];
	printf("--->level = %d\r\n", voltage_level);
	printf("--->voltage = %d\r\n", vbat_voltage);
	printf("--->voltage_range = %d\r\n", voltage_range);
}

void led_pwm_init_led(u8 i)
{
	if (i >= 1 && i <= 5)
	{
		gpio_set_func(gpio_pins[i - 1], PWM0 + i - 1);
		pwm_set_clk(CLOCK_SYS_CLOCK_HZ, CLOCK_SYS_CLOCK_HZ);
		pwm_set_mode(PWM0_ID + i - 1, PWM_NORMAL_MODE);
	}
}

void led_pwm_init_all_led()
{
	for (int i = 0; i < 5; i++)
		led_pwm_init_led(i + 1);
}

void led_pwm_all_open()
{
	led_pwm_init_all_led();
	for (u8 i = 0; i < 5; i++)
		pwm_set_cycle_and_duty(PWM0_ID + i, PWM_CLOCK, (unsigned short)(led_pwm_freq[5] * CLOCK_SYS_CLOCK_1US));
	for (u8 i = 0; i < 5; i++)
		pwm_start(PWM0_ID + i);
}

void led_pwm_all_off()
{
	led_pwm_init_all_led();
	for (u8 i = 0; i < 5; i++)
		pwm_set_cycle_and_duty(PWM0_ID + i, PWM_CLOCK, PWM_CLCOK_OFF);
	for (u8 i = 0; i < 5; i++)
		pwm_start(PWM0_ID + i);
}

void led_pwm_status_open(u8 led_port_chose)
{
	switch (led_port_chose)
	{
	case 0:
	case 6:
		led_pwm_function(0, 0, 0, 0, 0);
		break;
	case 1:
	case 7:
		led_pwm_function(1, 0, 0, 0, 0);
		break;
	case 2:
	case 8:
	case 10:
		led_pwm_function(1, 1, 0, 0, 0);
		break;
	case 3:
	case 9:
	case 15:
	case 17:
		led_pwm_function(1, 1, 1, 0, 0);
		break;
	case 4:
		led_pwm_function(1, 1, 1, 1, 0);
		break;
	case 5:
		led_pwm_function(1, 1, 1, 1, 1);
		break;
	case 11:
	case 12:
	case 16:
		led_pwm_function(0, 0, 1, 0, 0);
		break;
	case 13:
		led_pwm_function(0, 0, 1, 0, 1);
		break;
	case 14:
		led_pwm_function(1, 0, 1, 0, 1);
		break;
	case 18:
		led_pwm_function(g_function.auto_flush, g_function.foot_sensation, 1, g_function.auto_flip_cover, g_function.power_save);
		break;
	default:
		break;
	}
}

void led_pwm_function(u8 led_1, u8 led_2, u8 led_3, u8 led_4, u8 led_5)
{
	u8 led_count = led_1 + led_2 + led_3 + led_4 + led_5;
	u32 pwm_cycles = led_count > 0 ? led_pwm_freq[led_count] : 0;
	for (u8 i = 0; i < 5; i++)
	{
		if ((i == 0 && led_1) || (i == 1 && led_2) || (i == 2 && led_3) || (i == 3 && led_4) || (i == 4 && led_5))
		{
			led_pwm_init_led(i + 1);
			pwm_set_cycle_and_duty(PWM0_ID + i, PWM_CLOCK, (unsigned short)(pwm_cycles * CLOCK_SYS_CLOCK_1US));
		}
		else
			pwm_set_cycle_and_duty(PWM0_ID + i, PWM_CLOCK, PWM_CLCOK_OFF);
	}
	for (u8 i = 0; i < 5; i++)
		pwm_start(PWM0_ID + i);
}

void led_pwm_factory_control(u8 led_port_chose, bool turn_on)
{
	static u8 led_open_num = 0;
	static bool led_open[6] = {0};

	if (led_port_chose >= 1 && led_port_chose <= 5)
	{
		if (turn_on && !led_open[led_port_chose])
		{
			led_open[led_port_chose] = true;
			led_open_num++;
		}
		else if (!turn_on && led_open[led_port_chose])
		{
			led_open[led_port_chose] = false;
			led_open_num--;
		}
	}

	for (u8 i = 0; i < 5; i++)
	{
		if ((i == 0 && led_open[i+1]) || (i == 1 &&  led_open[i+1]) || (i == 2 &&  led_open[i+1]) || (i == 3 &&  led_open[i+1]) || (i == 4 &&  led_open[i+1]))
		{
			led_pwm_init_led(i + 1);
			pwm_set_cycle_and_duty(PWM0_ID + i, PWM_CLOCK, (unsigned short)(led_pwm_freq[led_open_num] * CLOCK_SYS_CLOCK_1US));
		}
		else
			pwm_set_cycle_and_duty(PWM0_ID + i, PWM_CLOCK, PWM_CLCOK_OFF);
	}
	for (u8 i = 0; i < 5; i++)
		pwm_start(PWM0_ID + i);
}

void led_pwm_control(u8 led_port_chose)
{
	if (led_port_chose == 0x01)
		led_pwm_function(1, 0, 0, 0, 0);
	else if (led_port_chose == 0x02)
		led_pwm_function(0, 1, 0, 0, 0);
	else if (led_port_chose == 0x03)
		led_pwm_function(0, 0, 1, 0, 0);
	else if (led_port_chose == 0x04)
		led_pwm_function(0, 0, 0, 1, 0);
	else if (led_port_chose == 0x05)
		led_pwm_function(0, 0, 0, 0, 1);
}

void led_pwm_135_blink()
{
	if (led_pwm_time && clock_time_exceed(led_pwm_time, 5 * 100 * 1000) && led_pwm_blink_flag == 0)
	{
		led_pwm_function(1, 0, 1, 0, 1);
		led_pwm_blink_flag = 1;
		led_pwm_time = clock_time();
	}
	else if (led_pwm_time && clock_time_exceed(led_pwm_time, 5 * 100 * 1000) && led_pwm_blink_flag == 1)
	{
		led_pwm_all_off();
		led_pwm_blink_flag = 0;
		led_pwm_time = clock_time();
	}
}

void led_pwm_power_on()
{
	u8 channels[] = {5, 14, 17};
	u8 num_channels = sizeof(channels) / sizeof(channels[0]);
	for (int i = 0; i < num_channels; i++)
	{
		led_pwm_status_open(channels[i]);
		WaitMs(1000);
	}
	led_pwm_all_off();
}
