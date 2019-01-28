#pragma once


#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/adc.h"
#include "esp_adc_cal.h"

extern const char* POTS;

#define DEFAULT_VREF    1100        //Use adc2_vref_to_gpio() to obtain a better estimate

#define NO_OF_SAMPLES   64          //Multisampling

void PotTask();