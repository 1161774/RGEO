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

#define BOX_LOCK_TIME_MS	1000
#define BOX_UNLOCK_TIME_MS	1000

typedef enum
{
	BOX_IDLE,
	BOX_LOCK,
	BOX_UNLOCK
} lockCmd;


typedef enum
{
	BOX_UNDEF,
	BOX_LOCKED,
	BOX_UNLOCKED,
	BOX_LOCKING,
	BOX_UNLOCKING
} lockState;


extern lockState BoxState;

void LockBoxTask(void *arg);
void LockInit();
