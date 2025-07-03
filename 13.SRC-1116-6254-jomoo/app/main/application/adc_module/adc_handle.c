#include "adc_handle.h"
short vbat_voltage = 0;
void power_supply_voltage_check_init()
{
   adc_init();
   adc_vbat_channel_init();
   adc_power_on_sar_adc(1);		//After setting the ADC parameters, turn on the ADC power supply control bit
   vbat_voltage = adc_sample_and_get_result();
   // printf("voltage = %d\r\n", vbat_voltage);	
}
  