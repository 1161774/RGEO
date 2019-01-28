#include "Peripherals/Lock.h"


void LockTask()
{
	gpio_config_t io_conf;

	
	//disable interrupt
	io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
	//set as output mode
	io_conf.mode = GPIO_MODE_OUTPUT;
	//bit mask of the pins that you want to set,e.g.GPIO18/19
	io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
	//disable pull-down mode
	io_conf.pull_down_en = 0;
	//disable pull-up mode
	io_conf.pull_up_en = 0;
	//configure GPIO with the given settings
	gpio_config(&io_conf);
	
	uint8_t cnt = 0;
	while (1) 
	{
		ESP_LOGI(LOCK, "cnt: %d", cnt++);
		vTaskDelay(1000 / portTICK_RATE_MS);
		gpio_set_level(GPIO_LOCK_SLEEP, cnt % 2);
		gpio_set_level(GPIO_LOCK_LOCK, cnt % 2);
		gpio_set_level(GPIO_LOCK_UNLOCK, cnt % 2);
	}


}