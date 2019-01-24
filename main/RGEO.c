/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/



/*
 *
 *	TODO: GPS Location logging
 *
 *
 *
 **/


#include <stdio.h>
#include <string.h>
#include <math.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"



#include "esp_spi_flash.h"
#include "esp_system.h"
#include "esp_event_loop.h"
#include "esp_log.h"



#include "nvs.h"
#include "nvs_flash.h"
#include "GPS/GPS.h"
#include "LCD/LCD.h"


extern void GPSTask();
extern void LCDTask();

const char* GPS = "GPS";
const char* LCD= "LCD";





void app_main()
{

	esp_log_level_set(GPS, ESP_LOG_DEBUG);
	
	
	xTaskCreate(&GPSTask, "GPS", 2048, NULL, 3, NULL);
	xTaskCreate(&LCDTask, "LCD", 2048, NULL, 8, NULL);	
}