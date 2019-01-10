/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "driver/i2c.h"
#include "driver/uart.h"


#include "esp_spi_flash.h"
#include "esp_system.h"
#include "esp_event_loop.h"
#include "esp_log.h"



#include "nvs.h"
#include "nvs_flash.h"
//#include "LibGPS/gps.h"
#include "LibGPS/nmea.h"


static const char* GPS = "GPS";
static const char* LCD= "LCD";


#define BUF_SIZE (1024)


void znmea_parse_gpgga(uint8_t *nmea, gpgga_t *loc)
{
	char *p = (uint8_t)nmea;

	p = strchr(p, ',') + 1;  //skip time

	p = strchr(p, ',') + 1;
	loc->latitude = atof(p);

	p = strchr(p, ',') + 1;
	switch (p[0]) {
	case 'N':
		loc->lat = 'N';
		break;
	case 'S':
		loc->lat = 'S';
		break;
	case ',':
		loc->lat = '\0';
		break;
	}

	p = strchr(p, ',') + 1;
	loc->longitude = atof(p);

	p = strchr(p, ',') + 1;
	switch (p[0]) {
	case 'W':
		loc->lon = 'W';
		break;
	case 'E':
		loc->lon = 'E';
		break;
	case ',':
		loc->lon = '\0';
		break;
	}

	p = strchr(p, ',') + 1;
	loc->quality = (uint8_t)atoi(p);

	p = strchr(p, ',') + 1;
	loc->satellites = (uint8_t)atoi(p);

	p = strchr(p, ',') + 1;

	p = strchr(p, ',') + 1;
	loc->altitude = atof(p);
}

void znmea_parse_gprmc(uint8_t *nmea, gprmc_t *loc)
{
	char *p = (uint8_t)nmea;

	p = strchr(p, ',') + 1;  //skip time
	p = strchr(p, ',') + 1;  //skip status

	p = strchr(p, ',') + 1;
	loc->latitude = atof(p);

	p = strchr(p, ',') + 1;
	switch (p[0]) {
	case 'N':
		loc->lat = 'N';
		break;
	case 'S':
		loc->lat = 'S';
		break;
	case ',':
		loc->lat = '\0';
		break;
	}

	p = strchr(p, ',') + 1;
	loc->longitude = atof(p);

	p = strchr(p, ',') + 1;
	switch (p[0]) {
	case 'W':
		loc->lon = 'W';
		break;
	case 'E':
		loc->lon = 'E';
		break;
	case ',':
		loc->lon = '\0';
		break;
	}

	p = strchr(p, ',') + 1;
	loc->speed = atof(p);

	p = strchr(p, ',') + 1;
	loc->course = atof(p);
}



double zgps_deg_dec(double deg_point)
{
	double ddeg;
	double sec = modf(deg_point, &ddeg) * 60;
	int deg = (int)(ddeg / 100);
	int min = (int)(deg_point - (deg * 100));

	double absdlat = round(deg * 1000000.);
	double absmlat = round(min * 1000000.);
	double absslat = round(sec * 1000000.);

	return round(absdlat + (absmlat / 60) + (absslat / 3600)) / 1000000;
}

// Convert lat e lon to decimals (from deg)
void zgps_convert_deg_to_dec(double *latitude, char ns, double *longitude, char we)
{
	double lat = (ns == 'N') ? *latitude : -1 * (*latitude);
	double lon = (we == 'E') ? *longitude : -1 * (*longitude);

	*latitude = zgps_deg_dec(lat);
	*longitude = zgps_deg_dec(lon);
}




uint8_t znmea_valid_checksum(const uint8_t *message) {
	uint8_t checksum = (uint8_t)strtol(strchr((const char *)message, '*') + 1, NULL, 16);

	char p;
	uint8_t sum = 0;
	++message;
	while ((p = *message++) != '*') {
		sum ^= p;
	}

	if (sum != checksum) {
		return NMEA_CHECKSUM_ERR;
	}

	return _EMPTY;
}




uint8_t znmea_get_message_type(const uint8_t *message)
{
	uint8_t checksum = 0;
	if ((checksum = znmea_valid_checksum(message)) != _EMPTY) {
		return checksum;
	}

	if (strstr((const char *)message, NMEA_GPGGA_STR) != NULL) {
		return NMEA_GPGGA;
	}

	if (strstr((const char *)message, NMEA_GPRMC_STR) != NULL) {
		return NMEA_GPRMC;
	}

	return NMEA_UNKNOWN;
}







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



	// Configure buffers for the incoming data
	uint8_t *data = (uint8_t *) malloc(BUF_SIZE);
	uint8_t *message = (uint8_t *) malloc(BUF_SIZE);


	ESP_LOGI(GPS, "UART Configured");

	loc_t coord;
	
	int16_t i, start = -1, msgLen;
	
	while (1) {



		// Read data from the UART
		int len = uart_read_bytes(UART_NUM_2, data, BUF_SIZE, 20 / portTICK_RATE_MS);

		data[len] = 0;
		
		
		if (len > 0)
		{
			
			ESP_LOGV(GPS, "Raw: %s", data);

		
					
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
						msgLen = strlen((const char *)message);
						memcpy(message + msgLen, data, i);
						message[msgLen + i] = 0;
					}
					else if (start >= 0)
					{
						memcpy(message, data + start, i - start);
						message[i - start] = 0;
					}
					start = -1;
					ESP_LOGD(GPS, "%s", message);
					
					
					gpgga_t gpgga;
					gprmc_t gprmc;

//					znmea_get_message_type(message);
					
					switch (znmea_get_message_type((const uint8_t *)message)){
//					switch (999) {
					case NMEA_GPGGA:
						znmea_parse_gpgga(message, &gpgga);
						
						zgps_convert_deg_to_dec(&(gpgga.latitude), gpgga.lat, &(gpgga.longitude), gpgga.lon);

						coord.latitude = gpgga.latitude;
						coord.longitude = gpgga.longitude;
						coord.altitude = gpgga.altitude;

						break;
					case NMEA_GPRMC:
						znmea_parse_gprmc(message, &gprmc);

						coord.speed = gprmc.speed;
						coord.course = gprmc.course;

						break;
					}

				}
			}
		
			if (start >= 0)
			{
				memcpy(message, data + start, len - start);
				message[len - start] = 0;
			}
						
			//		gps_location(&gpsData);
		}
	}
		
	ESP_LOGW(GPS, "GPS TASK ENDED");
	vTaskDelete(NULL);
}


//static void LCDTask()
//{
//	ESP_LOGI(LCD, "Start LCD");
//	
//	int i2c_master_port = I2C_NUM_0;
//	i2c_config_t conf;
//	conf.mode = I2C_MODE_MASTER;
//	conf.sda_io_num = GPIO_NUM_31;
//	conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
//	conf.scl_io_num = GPIO_NUM_17;
//	conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
//	conf.master.clk_speed = I2C_EXAMPLE_MASTER_FREQ_HZ;
//	i2c_param_config(i2c_master_port, &conf);
//	i2c_driver_install(i2c_master_port,
//		conf.mode,
//		I2C_EXAMPLE_MASTER_RX_BUF_DISABLE,
//		I2C_EXAMPLE_MASTER_TX_BUF_DISABLE,
//		0);
//	
//
//	ESP_LOGW(LCD, "LCD TASK ENDED");
//	vTaskDelete(NULL);
//
//}



void app_main()
{
	
	esp_log_level_set(GPS, ESP_LOG_DEBUG);
	
	
	xTaskCreate(&GPSTask, "GPS", 2048, NULL, 3, NULL);
//	xTaskCreate(&LCDTask, "LCD", 2048, NULL, 8, NULL);	
}