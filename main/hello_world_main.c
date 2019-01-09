/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "driver/uart.h"

#include "esp_spi_flash.h"
#include "esp_system.h"
#include "esp_event_loop.h"
#include "esp_log.h"



#include "nvs.h"
#include "nvs_flash.h"
//#include "LibGPS/gps.h"


static const char* MainTask = "Main Task";
static const char* GPS = "GPS";


void HelloWorld_Task(void *pvParameter)
{
	ESP_LOGI(MainTask, "Hello world!");

    /* Print chip information */
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
	ESP_LOGI(MainTask,
		"This is ESP32 chip with %d CPU cores, WiFi%s%s, ",
            chip_info.cores,
            (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
            (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");

	ESP_LOGI(MainTask, "silicon revision %d, ", chip_info.revision);

	ESP_LOGI(MainTask,
		"%dMB %s flash",
		spi_flash_get_chip_size() / (1024 * 1024),
            (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

    for (int i = 10; i > 0; i--) {
	    ESP_LOGI(MainTask, "Restarting in %d seconds...", i);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
	ESP_LOGI(MainTask, "Restarting now.");
    fflush(stdout);
	vTaskDelete(NULL);
}




#define BUF_SIZE (1024)



static void GPSTask()

{
	ESP_LOGI(GPS, "Start UART");

	/* Configure parameters of an UART driver,

	 * communication pins and install the driver */

	uart_config_t uart_config = {

		.baud_rate = 9600,

		.data_bits = UART_DATA_8_BITS,

		.parity = UART_PARITY_DISABLE,

		.stop_bits = UART_STOP_BITS_1,

		.flow_ctrl = UART_HW_FLOWCTRL_DISABLE

	};

	uart_param_config(UART_NUM_2, &uart_config);

	uart_set_pin(UART_NUM_2, GPIO_NUM_17, GPIO_NUM_16, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

	uart_driver_install(UART_NUM_2, BUF_SIZE * 2, 0, 0, NULL, 0);



	// Configure a temporary buffer for the incoming data

	uint8_t *data = (uint8_t *) malloc(BUF_SIZE);
	uint8_t *message = (uint8_t *) malloc(BUF_SIZE);


	ESP_LOGI(GPS, "UART Configured");

//	loc_t gpsData;

	int16_t i, start = -1;
	
	while (1) {



		// Read data from the UART
		int len = uart_read_bytes(UART_NUM_2, data, BUF_SIZE, 20 / portTICK_RATE_MS);

		
		
		for (i = 0; i < len; i++)
		{
			if (data[i] == '$')
			{
				start = i;				
			}
			else if (data[i] == '\r')
			{
				if (i < start)
				{
					memcpy(message + strlen((const char *)message), data, i);
					message[i] = 0;
				}
				if (start >= 0)
				{
					
					memcpy(message, data + start, i - start);
					message[i - start] = 0;
				}
				
				ESP_LOGI(GPS, "%s", message);
				
				start = -1;
			}
		}
		
		if (start >= 0)
		{
			memcpy(message, data + start, len - start);
			message[len - start] = 0;
		}

		
		
						
//		gps_location(&gpsData);

		
	}
	
	ESP_LOGW(GPS, "GPS TASK ENDED");
	vTaskDelete(NULL);

}




void app_main()
{
	
//	xTaskCreate(&HelloWorld_Task, "Hello_World", 2048, NULL, 5, NULL);
//	xTaskCreate(&UART_Task, "UART", 2048, NULL, 6, NULL);
	xTaskCreate(&GPSTask, "GPS", 2048, NULL, 7, NULL);
	
}