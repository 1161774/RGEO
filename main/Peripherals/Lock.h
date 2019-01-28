#pragma once

#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


#include "driver/gpio.h"


extern const char* LOCK;

#define GPIO_LOCK_SLEEP		25
#define GPIO_LOCK_LOCK		26
#define GPIO_LOCK_UNLOCK    27
#define GPIO_OUTPUT_PIN_SEL  ( (1 << GPIO_LOCK_SLEEP) | (1 << GPIO_LOCK_LOCK) | (1 << GPIO_LOCK_UNLOCK) )

void LockTask();
