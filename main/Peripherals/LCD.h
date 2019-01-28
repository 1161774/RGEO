#pragma once

#include <inttypes.h>

typedef struct
{
	uint16_t bitmapOffset;         ///< Pointer into GFXfont->bitmap
	uint8_t  width;                ///< Bitmap dimensions in pixels
	uint8_t  height;               ///< Bitmap dimensions in pixels
	uint8_t  xAdvance;             ///< Distance to advance cursor (x axis)
	int8_t   xOffset;              ///< X dist from cursor pos to UL corner
	int8_t   yOffset;              ///< Y dist from cursor pos to UL corner
}	GFXglyph;




// Data stored for FONT AS A WHOLE
typedef struct 
{ 
	uint8_t  *bitmap;           ///< Glyph bitmaps, concatenated
	GFXglyph *glyph;           ///< Glyph array
	uint8_t   first;           ///< ASCII extents (first char)
	uint8_t   last;            ///< ASCII extents (last char)
	uint8_t   yAdvance;        ///< Newline distance (y axis)
}	GFXfont;


extern const uint8_t FreeMono12pt7bBitmaps[];
extern const GFXglyph FreeMono12pt7bGlyphs[];
extern const GFXfont MyFont;



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




#define ESP_SLAVE_ADDR                     0x3C             /*!< ESP32 slave address, you can set any 7bit value */
#define WRITE_BIT                          I2C_MASTER_WRITE /*!< I2C master write */
#define READ_BIT                           I2C_MASTER_READ  /*!< I2C master read */
#define ACK_CHECK_EN                       0x1              /*!< I2C master will check ack from slave*/



#define DISPLAY_BUFFER_SIZE			512

extern const char* LCD;


extern uint8_t CursorX;
extern uint8_t CursorY;

typedef enum
{
	COMMAND,
	DATA
} I2CMode; 





static uint8_t DisplayBuffer[DISPLAY_BUFFER_SIZE];


void I2C_Write();

void I2C_Init();

void LCDTask();
