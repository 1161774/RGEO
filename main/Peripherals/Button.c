#include "Peripherals/Button.h"



/**

 * Brief:

 * This test code shows how to configure gpio and how to use gpio interrupt.

 *

 * GPIO status:

 * GPIO18: output

 * GPIO19: output

 * GPIO4:  input, pulled up, interrupt from rising edge and falling edge

 * GPIO5:  input, pulled up, interrupt from rising edge.

 *

 * Test:

 * Connect GPIO18 with GPIO4

 * Connect GPIO19 with GPIO5

 * Generate pulses on GPIO18/19, that triggers interrupt on GPIO4/5

 *

 */


#define GPIO_OUTPUT_IO_0    25
#define GPIO_OUTPUT_IO_1    26
#define GPIO_OUTPUT_IO_2    27
#define GPIO_OUTPUT_PIN_SEL  ( (1 << GPIO_OUTPUT_IO_0) | (1 << GPIO_OUTPUT_IO_1) | (1 << GPIO_OUTPUT_IO_2) )


#define GPIO_INPUT_BUTTON     32
#define GPIO_INPUT_PIN_SEL  (1ULL<<GPIO_INPUT_BUTTON)



#define ESP_INTR_FLAG_DEFAULT 0



static xQueueHandle gpio_evt_queue = NULL;



static void IRAM_ATTR gpio_isr_handler(void* arg)

{

	uint32_t gpio_num = (uint32_t) arg;

//	ESP_LOGI(BUTTON, "ISR Received");
	
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

	

	//interrupt of rising edge
	io_conf.intr_type = GPIO_PIN_INTR_POSEDGE;
	//bit mask of the pins, use GPIO4/5 here
	io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL;
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

	
	int cnt = 0;
	while (1) 
	{
		ESP_LOGI(BUTTON, "cnt: %d", cnt++);
		vTaskDelay(1000 / portTICK_RATE_MS);
		gpio_set_level(GPIO_OUTPUT_IO_0, cnt % 2);
		gpio_set_level(GPIO_OUTPUT_IO_1, cnt % 2);
		gpio_set_level(GPIO_OUTPUT_IO_2, cnt % 2);
	}

	
	
	while (1)
	{
/*
		if (gpio_get_level(GPIO_INPUT_BUTTON))
		{
			ESP_LOGI(BUTTON, "Button Pressed");	
		}
*/
	}

	vTaskDelete(NULL);
}