#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include <math.h>


#include "GPS/GPS.h"


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

// Convert lat e lon to decimals (from deg)
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

