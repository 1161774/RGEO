#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include <math.h>

#include "esp_log.h"

#include "driver/uart.h"

#include "Peripherals/GPS.h"


void nmea_parse_gpgga(uint8_t *nmea, gpgga_t *loc)
{
	char *p = (char *)nmea;

	p = strchr(p, ',') + 1;   //skip time

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

void nmea_parse_gprmc(uint8_t *nmea, gprmc_t *loc)
{
	char *p = (char *)nmea;

	p = strchr(p, ',') + 1;   //skip time
	p = strchr(p, ',') + 1;   //skip status

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

double gps_deg_dec(double deg_point)
{
	double ddeg;
	double sec = modf(deg_point, &ddeg) * 60.0;
	int deg = (int)(ddeg / 100.0);
	int min = (int)(deg_point - (deg * 100.0));

	double absdlat = round(deg * 1000000.0);
	double absmlat = round(min * 1000000.0);
	double absslat = round(sec * 1000000.0);

	return round(absdlat + (absmlat / 60.0) + (absslat / 3600.0)) / 1000000.0;
}

void gps_convert_deg_to_dec(double *latitude, char ns, double *longitude, char we)
{
	double lat = (ns == 'N') ? *latitude : -1 * (*latitude);
	double lon = (we == 'E') ? *longitude : -1 * (*longitude);

	*latitude = gps_deg_dec(lat);
	*longitude = gps_deg_dec(lon);
}

uint8_t nmea_valid_checksum(const uint8_t *message) {
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

uint8_t nmea_get_message_type(const uint8_t *message)
{
	uint8_t checksum = 0;
	if ((checksum = nmea_valid_checksum(message)) != _EMPTY) {
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

void GPSInit()
{
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

}

double GetDistance(loc_t Loc, loc_t Dest)
{
	double dx, dy, dz;
	Loc.longitude -= Dest.longitude;
	Loc.longitude *= TO_RAD;
	Loc.latitude *= TO_RAD;
	Dest.latitude *= TO_RAD;
 
	dz = sin(Loc.latitude) - sin(Dest.latitude);
	dx = cos(Loc.longitude) * cos(Loc.latitude) - cos(Dest.latitude);
	dy = sin(Loc.longitude) * cos(Loc.latitude);
	
	return asin(sqrt(dx * dx + dy * dy + dz * dz) / 2) * 2 * R;
}

void GPSTask()
{
	ESP_LOGI(GPS, "Start UART");

	// Configure buffers for the incoming data
	uint8_t *data = (uint8_t *) malloc(BUF_SIZE);
	uint8_t *message = (uint8_t *) malloc(BUF_SIZE);

	memset(message, 0x00, BUF_SIZE);
	
	gpgga_t gpgga;
	gprmc_t gprmc;
	
	int16_t i, start = -1, msgLen;

	ESP_LOGI(GPS, "UART Configured");

	
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

					switch (nmea_get_message_type((const uint8_t *)message)) {
					case NMEA_GPGGA:
						nmea_parse_gpgga(message, &gpgga);
						
						gps_convert_deg_to_dec(&(gpgga.latitude), gpgga.lat, &(gpgga.longitude), gpgga.lon);

						xSemaphoreTake(Location_mux, portMAX_DELAY);

						Location.latitude = gpgga.latitude;
						Location.longitude = gpgga.longitude;
						Location.altitude = gpgga.altitude;

						ESP_LOGI(GPS, "Latitude:\t%f", Location.latitude);
						ESP_LOGI(GPS, "Longitude:\t%f", Location.longitude);
						ESP_LOGI(GPS, "Altitude:\t%f", Location.altitude);

						xSemaphoreGive(Location_mux);

						break;
					case NMEA_GPRMC:
						nmea_parse_gprmc(message, &gprmc);

						xSemaphoreTake(Location_mux, portMAX_DELAY);

						Location.speed = gprmc.speed;
						Location.course = gprmc.course;

						ESP_LOGI(GPS, "Speed\t\t%f", Location.speed);
						ESP_LOGI(GPS, "Course\t\t%f", Location.course);

						xSemaphoreGive(Location_mux);

						break;
					}
				}
			}
		
			if (start >= 0)
			{
				memcpy(message, data + start, len - start);
				message[len - start] = 0;
			}
		}
	}
		
	ESP_LOGE(GPS, "GPS TASK ENDED");
	vTaskDelete(NULL);
}

