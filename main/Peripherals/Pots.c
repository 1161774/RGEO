

#include "Peripherals/Pots.h"

static esp_adc_cal_characteristics_t *adc_chars;

static const adc_channel_t channel_1 = ADC_CHANNEL_6;       //GPIO34 if ADC1, GPIO14 if ADC2
static const adc_channel_t channel_2 = ADC_CHANNEL_7;        //GPIO34 if ADC1, GPIO14 if ADC2

static const adc_atten_t atten = ADC_ATTEN_DB_0;

static const adc_unit_t unit = ADC_UNIT_1;





void PotTask()
{
	ESP_LOGI(POTS, "Starting Pots");
	

	//Configure ADC
	adc1_config_width(ADC_WIDTH_BIT_12);
	adc1_config_channel_atten(channel_1, atten);
	adc1_config_channel_atten(channel_2, atten);



	adc_chars = calloc(1, sizeof(esp_adc_cal_characteristics_t));
	
	esp_adc_cal_characterize(unit, atten, ADC_WIDTH_BIT_12, DEFAULT_VREF, adc_chars);



	while (1) {

		uint32_t adc_reading_1 = 0;
		uint32_t adc_reading_2 = 0;

		//Multisampling

		for(int i = 0 ; i < NO_OF_SAMPLES ; i++) {
			adc_reading_1 += adc1_get_raw((adc1_channel_t)channel_1);
			adc_reading_2 += adc1_get_raw((adc1_channel_t)channel_2);
		}

		adc_reading_1 /= NO_OF_SAMPLES;
		adc_reading_2 /= NO_OF_SAMPLES;

		
		
		uint16_t duty = adc_reading_1 * 100 / 4096;
		uint16_t freq = adc_reading_2 * 1200 / 4096;
		
		
		
		ESP_LOGI(POTS, "Duty: %d\tFreq: %d", duty, freq);
		
		
//		mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, (float)duty);
//		mcpwm_set_frequency(MCPWM_UNIT_0, MCPWM_TIMER_0, freq);

		

		vTaskDelay(pdMS_TO_TICKS(100));

	}
}
