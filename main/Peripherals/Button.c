#include "Peripherals/Button.h"



#define GPIO_INPUT_BUTTON     23
#define GPIO_INPUT_PIN_SEL  (1 << GPIO_INPUT_BUTTON)

#define ESP_INTR_FLAG_DEFAULT 0



static xQueueHandle gpio_evt_queue = NULL;



static void IRAM_ATTR gpio_isr_handler(void* arg)

{

	uint32_t gpio_num = (uint32_t) arg;
	xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);

}



static void gpio_task_example(void* arg)

{

	uint32_t io_num;

	while(1) {

		if (xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY)) {
			ESP_LOGI(BUTTON, "GPIO[%d] intr, val: %d\n", io_num, gpio_get_level(io_num));
		}
	}
}



void ButtonTask()
{

	gpio_config_t io_conf;

	//interrupt of rising edge
	io_conf.intr_type = GPIO_PIN_INTR_POSEDGE;
	//bit mask of the pins, use GPIO4/5 here
	io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL;
	//disable pull-down mode
	io_conf.pull_down_en = 1;
	//disable pull-up mode
	io_conf.pull_up_en = 0;
	//set as input mode    
	io_conf.mode = GPIO_MODE_INPUT;

	gpio_config(&io_conf);


	//create a queue to handle gpio event from isr
	gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));
	//start gpio task
	xTaskCreate(gpio_task_example, "gpio_task_example", 2048, NULL, 10, NULL);

	//install gpio isr service
	gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);

	//hook isr handler for specific gpio pin
	gpio_isr_handler_add(GPIO_INPUT_BUTTON, gpio_isr_handler, (void*) GPIO_INPUT_BUTTON);

	
	vTaskDelete(NULL);
}