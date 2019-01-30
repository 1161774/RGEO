#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "esp_log.h"

#include "driver/gpio.h"


xQueueHandle gpio_evt_queue;


extern const char* BUTTON;

extern void ButtonTask(void* arg);



void ButtonInit();