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


#define	SSD1306_DISPLAYOFF			0xAE
#define SSD1306_SETDISPLAYCLOCKDIV	0xD5
#define SSD1306_SETMULTIPLEX		0xA8
#define SSD1306_SETDISPLAYOFFSET	0xD3
#define SSD1306_CHARGEPUMP			0x8D
#define SSD1306_MEMORYMODE			0x20
#define SSD1306_SEGREMAP			0xA0
#define SSD1306_COMSCANDEC			0xC8
#define SSD1306_SETCOMPINS			0xDA
#define SSD1306_SETCONTRAST			0x81
#define SSD1306_SETPRECHARGE		0xd9
#define SSD1306_SETVCOMDETECT		0xDB
#define SSD1306_DISPLAYALLON_RESUME	0xA4
#define SSD1306_NORMALDISPLAY		0xA6
#define SSD1306_DISPLAYON			0xAF

#define SSD1306_SETSTARTLINE		0x40

#define SSD1306_DEACTIVATE_SCROLL	0x2E



#define	SSD1306_COLUMNADDR			0x21
#define	SSD1306_PAGEADDR			0x22
#define	SSD1306_LCDWIDTH			128
#define	SSD1306_LCDHEIGHT			32




typedef enum
{
	COMMAND,
	DATA
} I2CMode; 




uint8_t display_buffer[1024];


void znmea_parse_gpgga(uint8_t *nmea, gpgga_t *loc)
{
	char *p = (char *)nmea;

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
	char *p = (char *)nmea;

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
	double sec = modf(deg_point, &ddeg) * 60.0;
	int deg = (int)(ddeg / 100.0);
	int min = (int)(deg_point - (deg * 100.0));

	double absdlat = round(deg * 1000000.0);
	double absmlat = round(min * 1000000.0);
	double absslat = round(sec * 1000000.0);

	return round(absdlat + (absmlat / 60.0) + (absslat / 3600.0)) / 1000000.0;
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
	gpgga_t gpgga;
	gprmc_t gprmc;

	
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

					switch (znmea_get_message_type((const uint8_t *)message)) {
					case NMEA_GPGGA:
						znmea_parse_gpgga(message, &gpgga);
						
						zgps_convert_deg_to_dec(&(gpgga.latitude), gpgga.lat, &(gpgga.longitude), gpgga.lon);

						coord.latitude = gpgga.latitude;
						coord.longitude = gpgga.longitude;
						coord.altitude = gpgga.altitude;
						
						ESP_LOGI(GPS, "Latitude:\t%f", coord.latitude);
						ESP_LOGI(GPS, "Longitude:\t%f", coord.longitude);
						ESP_LOGI(GPS, "Altitude:\t%f", coord.altitude);

						break;
					case NMEA_GPRMC:
						znmea_parse_gprmc(message, &gprmc);

						coord.speed = gprmc.speed;
						coord.course = gprmc.course;
						
						ESP_LOGI(GPS, "Speed\t\t%f", coord.speed);
						ESP_LOGI(GPS, "Course\t\t%f", coord.course);

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


#define ESP_SLAVE_ADDR                     0x3C             /*!< ESP32 slave address, you can set any 7bit value */
#define WRITE_BIT                          I2C_MASTER_WRITE /*!< I2C master write */
#define READ_BIT                           I2C_MASTER_READ  /*!< I2C master read */
#define ACK_CHECK_EN                       0x1              /*!< I2C master will check ack from slave*/


//static void i2c_write(i2c_port_t i2c_num, uint8_t* data_wr, size_t size, I2CMode Mode)
//{
//	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
//	i2c_master_start(cmd);
//	i2c_master_write_byte(cmd, (ESP_SLAVE_ADDR << 1) | WRITE_BIT, ACK_CHECK_EN);
//	i2c_master_write_byte(cmd, (Mode == COMMAND ? 0x00 : 0x40), ACK_CHECK_EN);
//	uint8_t byteCount = 1;
//
//	while (size--)
//	{
//		if (byteCount >= 32)
//		{
//			i2c_master_stop(cmd);
//			i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
//			i2c_master_start(cmd);
//			i2c_master_write_byte(cmd, (ESP_SLAVE_ADDR << 1) | WRITE_BIT, ACK_CHECK_EN);
//			i2c_master_write_byte(cmd, (Mode == COMMAND ? 0x00 : 0x40), ACK_CHECK_EN);
//			byteCount = 1;
//		}
//		i2c_master_write(cmd, data_wr, size, ACK_CHECK_EN);
//		i2c_master_stop(cmd);
//		i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
//	}
//	
//	i2c_cmd_link_delete(cmd);
//}


static esp_err_t i2c_write(i2c_port_t i2c_num, uint8_t* data_wr, size_t size, I2CMode Mode)
{
	volatile i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (ESP_SLAVE_ADDR << 1) | WRITE_BIT, ACK_CHECK_EN);
	i2c_master_write_byte(cmd, (Mode == COMMAND ? 0x00 : 0x40), ACK_CHECK_EN);
	i2c_master_write(cmd, data_wr, size, ACK_CHECK_EN);
	i2c_master_stop(cmd);
	esp_err_t ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
	i2c_cmd_link_delete(cmd);
	return ret;
}


// Issue list of commands to SSD1306, same rules as above re: transactions.
// This is a private function, not exposed.
//void Adafruit_SSD1306::ssd1306_commandList(const uint8_t *c, uint8_t n) {
//		 // I2C
//	  wire->beginTransmission(i2caddr);
//		WIRE_WRITE((uint8_t)0x00);  // Co = 0, D/C = 0
//		uint8_t bytesOut = 1;
//		while (n--) {
//			if (bytesOut >= WIRE_MAX) {
//				wire->endTransmission();
//				wire->beginTransmission(i2caddr);
//				WIRE_WRITE((uint8_t)0x00);  // Co = 0, D/C = 0
//				bytesOut = 1;
//			}
//			WIRE_WRITE(pgm_read_byte(c++));
//			bytesOut++;
//		}
//		wire->endTransmission();
//}




static void LCDTask()
{
	ESP_LOGI(LCD, "Start LCD");
	
	int i2c_master_port = I2C_NUM_0;
	i2c_config_t conf;
	conf.mode = I2C_MODE_MASTER;
	conf.sda_io_num = GPIO_NUM_21;
	//	conf.sda_io_num = GPIO_NUM_18;
	conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
	conf.scl_io_num = GPIO_NUM_22;
	//	conf.scl_io_num = GPIO_NUM_19;
	conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
	conf.master.clk_speed = 100000;
	i2c_param_config(i2c_master_port, &conf);
	i2c_driver_install(i2c_master_port,
		conf.mode,
		0,
		0,
		0);
	
	
	uint16_t i2cIndex;
	uint8_t *dat = malloc(1000);

	memset(dat, 0, 1000);

	ESP_LOGI(LCD, "Init LCD");

	
	#define HEIGHT 32
	
	i2cIndex = 0;
//	*(dat + i2cIndex++) = 0;     							// 0x00
	*(dat + i2cIndex++) = SSD1306_DISPLAYOFF;     			// 0xAE
	*(dat + i2cIndex++) = SSD1306_SETDISPLAYCLOCKDIV;    	// 0xD5
	*(dat + i2cIndex++) = 0x80;    						// the suggested ratio 0x80
	*(dat + i2cIndex++) = SSD1306_SETMULTIPLEX;            // 0xA8

	i2c_write(I2C_NUM_0, dat, i2cIndex, COMMAND);

	i2cIndex = 0;
//	*(dat + i2cIndex++) = 0;      							// 0x00
	*(dat + i2cIndex++) = HEIGHT - 1;

	i2c_write(I2C_NUM_0, dat, i2cIndex, COMMAND);

	i2cIndex = 0;
//	*(dat + i2cIndex++) = 0;       							// 0x00
	*(dat + i2cIndex++) = SSD1306_SETDISPLAYOFFSET;     	// 0xD3
	*(dat + i2cIndex++) = 0x0;    							// no offset
	*(dat + i2cIndex++) = SSD1306_SETSTARTLINE | 0x0;    	// line #0 0x40
	*(dat + i2cIndex++) = SSD1306_CHARGEPUMP;    			// 0x8D

	i2c_write(I2C_NUM_0, dat, i2cIndex, COMMAND);

	i2cIndex = 0;
//	*(dat + i2cIndex++) = 0;       							// 0x00
	*(dat + i2cIndex++) = 0x14;     						// Could be 0x10, could be 0x14

	i2c_write(I2C_NUM_0, dat, i2cIndex, COMMAND);

	i2cIndex = 0;
//	*(dat + i2cIndex++) = 0;       							// 0x00
	*(dat + i2cIndex++) = SSD1306_MEMORYMODE;      			// 0x20
	*(dat + i2cIndex++) = 0x00;    						// 0x0 act like ks0108
	*(dat + i2cIndex++) = SSD1306_SEGREMAP | 0x1;
	*(dat + i2cIndex++) = SSD1306_COMSCANDEC;

	i2c_write(I2C_NUM_0, dat, i2cIndex, COMMAND);

	i2cIndex = 0;
//	*(dat + i2cIndex++) = 0;       							// 0x00
	*(dat + i2cIndex++) = SSD1306_SETCOMPINS;     			// 0xDA
	*(dat + i2cIndex++) = 0x02;
	*(dat + i2cIndex++) = SSD1306_SETCONTRAST;    			// 0x81
	*(dat + i2cIndex++) = 0x8F;

	i2c_write(I2C_NUM_0, dat, i2cIndex, COMMAND);

	i2cIndex = 0;
//	*(dat + i2cIndex++) = 0;       							// 0x00
	*(dat + i2cIndex++) = SSD1306_SETPRECHARGE;     		// 0xd9

	i2c_write(I2C_NUM_0, dat, i2cIndex, COMMAND);

	i2cIndex = 0;
//	*(dat + i2cIndex++) = 0;       							// 0x00
	*(dat + i2cIndex++) = 0xF1;     						// Could be 0x22, could be 0xF1

	i2c_write(I2C_NUM_0, dat, i2cIndex, COMMAND);

	i2cIndex = 0;
//	*(dat + i2cIndex++) = 0;       							// 0x00
	*(dat + i2cIndex++) = SSD1306_SETVCOMDETECT;     		// 0xDB
	*(dat + i2cIndex++) = 0x40;
	*(dat + i2cIndex++) = SSD1306_DISPLAYALLON_RESUME;    	// 0xA4
	*(dat + i2cIndex++) = SSD1306_NORMALDISPLAY;    		// 0xA6
	*(dat + i2cIndex++) = SSD1306_DEACTIVATE_SCROLL;
	*(dat + i2cIndex++) = SSD1306_DISPLAYON;

	i2c_write(I2C_NUM_0, dat, i2cIndex, COMMAND);
	
	
	
	
	i2cIndex = 0;
//	*(dat + i2cIndex++) = 0x00;  							// 
	*(dat + i2cIndex++) = 0x22;   				// 0x22 COMMAND
	*(dat + i2cIndex++) = 0x00;  							// Start Page address
	*(dat + i2cIndex++) = 0xFF;   							// Start Page address
	*(dat + i2cIndex++) = 0x21; 							
	*(dat + i2cIndex++) = 0x00;
	i2c_write(I2C_NUM_0, dat, i2cIndex, COMMAND);

	i2cIndex = 0;
//	*(dat + i2cIndex++) = 0;       							// 0x00
	*(dat + i2cIndex++) = 0x7F; 							

	i2c_write(I2C_NUM_0, dat, i2cIndex, COMMAND);
	
	
	memset(display_buffer, 0X55, 33);   

//	display_buffer[0] = 0x40;
	

	for (uint16_t j = 0; j < 16; j++)
	{
		i2c_write(I2C_NUM_0, display_buffer, 32, DATA);
	}

//	i2c_write(I2C_NUM_0, display_buffer, 17, DATA);

	
	ESP_LOGW(LCD, "Well that worked at least");
	
	ESP_LOGE(LCD, "LCD TASK ENDED");
	vTaskDelete(NULL);
}



void app_main()
{
	
	esp_log_level_set(GPS, ESP_LOG_DEBUG);
	
	
	
//	xTaskCreate(&GPSTask, "GPS", 2048, NULL, 3, NULL);
	xTaskCreate(&LCDTask, "LCD", 2048, NULL, 8, NULL);	
}