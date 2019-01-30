
#include "Peripherals/Buzzer.h"

void BuzzerInit()
{

	mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0A, 4);      //Set GPIO 18 as PWM0A, to which servo is connected

	mcpwm_config_t pwm_config;

	pwm_config.frequency = 50;      //frequency = 50Hz, i.e. for every servo motor time period should be 20ms

	pwm_config.cmpr_a = 0;      //duty cycle of PWMxA = 0

	pwm_config.cmpr_b = 0;      //duty cycle of PWMxb = 0

	pwm_config.counter_mode = MCPWM_UP_COUNTER;

	pwm_config.duty_mode = MCPWM_DUTY_MODE_0;

	mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &pwm_config);      //Configure PWM0A & PWM0B with above settings

	mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, 50.0);


	ESP_LOGD(BUZZER, "Buzzer Initialised");
}


void Song()
{
	
	ESP_LOGI(BUZZER, "C4");
	mcpwm_set_frequency(MCPWM_UNIT_0, MCPWM_TIMER_0, 262);
	vTaskDelay(pdMS_TO_TICKS(400));
	mcpwm_set_frequency(MCPWM_UNIT_0, MCPWM_TIMER_0, 1);
	vTaskDelay(pdMS_TO_TICKS(100));

	ESP_LOGI(BUZZER, "C4");
	mcpwm_set_frequency(MCPWM_UNIT_0, MCPWM_TIMER_0, 262);
	vTaskDelay(pdMS_TO_TICKS(400));
	mcpwm_set_frequency(MCPWM_UNIT_0, MCPWM_TIMER_0, 1);
	vTaskDelay(pdMS_TO_TICKS(100));

	ESP_LOGI(BUZZER, "D4");
	mcpwm_set_frequency(MCPWM_UNIT_0, MCPWM_TIMER_0, 294);
	vTaskDelay(pdMS_TO_TICKS(400));
	mcpwm_set_frequency(MCPWM_UNIT_0, MCPWM_TIMER_0, 1);
	vTaskDelay(pdMS_TO_TICKS(100));

	ESP_LOGI(BUZZER, "E4");
	mcpwm_set_frequency(MCPWM_UNIT_0, MCPWM_TIMER_0, 330);
	vTaskDelay(pdMS_TO_TICKS(400));
	mcpwm_set_frequency(MCPWM_UNIT_0, MCPWM_TIMER_0, 1);
	vTaskDelay(pdMS_TO_TICKS(100));

	ESP_LOGI(BUZZER, "E4");
	mcpwm_set_frequency(MCPWM_UNIT_0, MCPWM_TIMER_0, 330);
	vTaskDelay(pdMS_TO_TICKS(400));
	mcpwm_set_frequency(MCPWM_UNIT_0, MCPWM_TIMER_0, 1);
	vTaskDelay(pdMS_TO_TICKS(100));

	ESP_LOGI(BUZZER, "D4");
	mcpwm_set_frequency(MCPWM_UNIT_0, MCPWM_TIMER_0, 294);
	vTaskDelay(pdMS_TO_TICKS(400));
	mcpwm_set_frequency(MCPWM_UNIT_0, MCPWM_TIMER_0, 1);
	vTaskDelay(pdMS_TO_TICKS(100));

	ESP_LOGI(BUZZER, "C4");
	mcpwm_set_frequency(MCPWM_UNIT_0, MCPWM_TIMER_0, 262);
	vTaskDelay(pdMS_TO_TICKS(400));
	mcpwm_set_frequency(MCPWM_UNIT_0, MCPWM_TIMER_0, 1);
	vTaskDelay(pdMS_TO_TICKS(100));

	ESP_LOGI(BUZZER, "B4");
	mcpwm_set_frequency(MCPWM_UNIT_0, MCPWM_TIMER_0, 247);
	vTaskDelay(pdMS_TO_TICKS(400));
	mcpwm_set_frequency(MCPWM_UNIT_0, MCPWM_TIMER_0, 1);
	vTaskDelay(pdMS_TO_TICKS(100));

	ESP_LOGI(BUZZER, "A4");
	mcpwm_set_frequency(MCPWM_UNIT_0, MCPWM_TIMER_0, 220);
	vTaskDelay(pdMS_TO_TICKS(400));
	mcpwm_set_frequency(MCPWM_UNIT_0, MCPWM_TIMER_0, 1);
	vTaskDelay(pdMS_TO_TICKS(100));

	ESP_LOGI(BUZZER, "A4");
	mcpwm_set_frequency(MCPWM_UNIT_0, MCPWM_TIMER_0, 220);
	vTaskDelay(pdMS_TO_TICKS(400));
	mcpwm_set_frequency(MCPWM_UNIT_0, MCPWM_TIMER_0, 1);
	vTaskDelay(pdMS_TO_TICKS(100));

	ESP_LOGI(BUZZER, "B4");
	mcpwm_set_frequency(MCPWM_UNIT_0, MCPWM_TIMER_0, 247);
	vTaskDelay(pdMS_TO_TICKS(400));
	mcpwm_set_frequency(MCPWM_UNIT_0, MCPWM_TIMER_0, 1);
	vTaskDelay(pdMS_TO_TICKS(100));

	ESP_LOGI(BUZZER, "C4");
	mcpwm_set_frequency(MCPWM_UNIT_0, MCPWM_TIMER_0, 262);
	vTaskDelay(pdMS_TO_TICKS(400));
	mcpwm_set_frequency(MCPWM_UNIT_0, MCPWM_TIMER_0, 1);
	vTaskDelay(pdMS_TO_TICKS(100));

	ESP_LOGI(BUZZER, "C4");
	mcpwm_set_frequency(MCPWM_UNIT_0, MCPWM_TIMER_0, 262);
	vTaskDelay(pdMS_TO_TICKS(400));
	mcpwm_set_frequency(MCPWM_UNIT_0, MCPWM_TIMER_0, 1);
	vTaskDelay(pdMS_TO_TICKS(100));

	ESP_LOGI(BUZZER, "B4");
	mcpwm_set_frequency(MCPWM_UNIT_0, MCPWM_TIMER_0, 247);
	vTaskDelay(pdMS_TO_TICKS(200));
	mcpwm_set_frequency(MCPWM_UNIT_0, MCPWM_TIMER_0, 1);
	vTaskDelay(pdMS_TO_TICKS(50));

	ESP_LOGI(BUZZER, "B4");
	mcpwm_set_frequency(MCPWM_UNIT_0, MCPWM_TIMER_0, 247);
	vTaskDelay(pdMS_TO_TICKS(200));
	mcpwm_set_frequency(MCPWM_UNIT_0, MCPWM_TIMER_0, 1);
	vTaskDelay(pdMS_TO_TICKS(50));


}