#include "Peripherals/Lock.h"



void LockBoxTask(void *arg)
{
	volatile lockCmd c = (lockCmd)arg;
	
	switch (c)
	{		
	case BOX_LOCK:
		ESP_LOGI(LOCK, "Received lock command");	
		break;
		
	case BOX_UNLOCK:
		ESP_LOGI(LOCK, "Received unlock command");	
		break;
	
	case BOX_IDLE:
	default:
		ESP_LOGE(LOCK, "Unknown Command %d", (int8_t)c);	
	}
	
	
	if (c == BOX_UNLOCK && BoxState == BOX_LOCKED)
	{
		ESP_LOGD(LOCK, "Unlocking...");
		BoxState = BOX_UNLOCKING;

		gpio_set_level(GPIO_LOCK_LOCK, 0);
		gpio_set_level(GPIO_LOCK_UNLOCK, 1);

		vTaskDelay(BOX_UNLOCK_TIME_MS / portTICK_RATE_MS);
		
		gpio_set_level(GPIO_LOCK_LOCK, 0);
		gpio_set_level(GPIO_LOCK_UNLOCK, 0);
		
		BoxState = BOX_UNLOCKED;
		ESP_LOGD(LOCK, "Unlocked");
	}
	else if (c ==  BOX_LOCK && BoxState == BOX_UNLOCKED)
	{
		ESP_LOGD(LOCK, "Locking...");
		BoxState = BOX_LOCKING;

		gpio_set_level(GPIO_LOCK_LOCK, 1);
		gpio_set_level(GPIO_LOCK_UNLOCK, 0);

		vTaskDelay(BOX_LOCK_TIME_MS / portTICK_RATE_MS);
		
		gpio_set_level(GPIO_LOCK_LOCK, 0);
		gpio_set_level(GPIO_LOCK_UNLOCK, 0);
		
		BoxState = BOX_LOCKED;
		ESP_LOGD(LOCK, "Locked");
	}
	else
	{
		ESP_LOGE(LOCK, "Unable to action command");
	}

	vTaskDelete(NULL);
}


void LockInit()
{
	ESP_LOGD(LOCK, "Initialising Lock");
	
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
	

	BoxState = BOX_UNLOCKED;
	
	ESP_LOGD(LOCK, "Lock Initialised");
}