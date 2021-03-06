#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"


#define _EMPTY 0x00
#define NMEA_GPRMC 0x01
#define NMEA_GPRMC_STR "$GPRMC"
#define NMEA_GPGGA 0x02
#define NMEA_GPGGA_STR "$GPGGA"
#define NMEA_UNKNOWN 0x00
#define _COMPLETED 0x03

#define NMEA_CHECKSUM_ERR 0x80
#define NMEA_MESSAGE_ERR 0xC0

#define BUF_SIZE (1024)

#define R 6371
#define TO_RAD (3.1415926536 / 180)


extern const char* GPS;

extern SemaphoreHandle_t Location_mux;

struct location {
	double latitude;
	double longitude;
	double speed;
	double altitude;
	double course;
};
typedef struct location loc_t;


struct gpgga{
	// Latitude eg: 4124.8963 (XXYY.ZZKK.. DEG, MIN, SEC.SS)
	double latitude;
	// Latitude eg: N
	char lat;
	// Longitude eg: 08151.6838 (XXXYY.ZZKK.. DEG, MIN, SEC.SS)
	double longitude;
	// Longitude eg: W
	char lon;
	// Quality 0, 1, 2
	uint8_t quality;
	// Number of satellites: 1,2,3,4,5...
	uint8_t satellites;
	// Altitude eg: 280.2 (Meters above mean sea level)
	double altitude;
};
typedef struct gpgga gpgga_t;

struct gprmc {
	double latitude;
	char lat;
	double longitude;
	char lon;
	double speed;
	double course;
};
typedef struct gprmc gprmc_t;

loc_t Location;


// -------------------------------------------------------------------------
// Internal functions
// -------------------------------------------------------------------------


void nmea_parse_gpgga(uint8_t *nmea, gpgga_t *loc);
void nmea_parse_gprmc(uint8_t *nmea, gprmc_t *loc);
double gps_deg_dec(double deg_point);
void gps_convert_deg_to_dec(double *latitude, char ns, double *longitude, char we);
uint8_t nmea_valid_checksum(const uint8_t *message);
uint8_t nmea_get_message_type(const uint8_t *message);

void GPSInit();

double GetDistance(loc_t Loc, loc_t Dest);

void GPSTask();

