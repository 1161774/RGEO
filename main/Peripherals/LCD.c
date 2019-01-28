#include <stdio.h>
#include <string.h>

#include "driver/i2c.h"

#include "esp_log.h"

#include "Peripherals/LCD.h"






void I2C_Write(i2c_port_t i2c_num, uint8_t* data_wr, size_t size, I2CMode Mode)
{
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (ESP_SLAVE_ADDR << 1) | WRITE_BIT, ACK_CHECK_EN);
	i2c_master_write_byte(cmd, (Mode == COMMAND ? 0x00 : 0x40), ACK_CHECK_EN);
	i2c_master_write(cmd, data_wr, size, ACK_CHECK_EN);
	i2c_master_stop(cmd);
	i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
	i2c_cmd_link_delete(cmd);
}


void I2C_Init()
{
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
}




void LCD_Init()
{
	uint16_t i2cIndex;
	uint8_t *dat = malloc(100);

	i2cIndex = 0;
	*(dat + i2cIndex++) = SSD1306_DISPLAYOFF;       			// 0xAE
	*(dat + i2cIndex++) = SSD1306_SETDISPLAYCLOCKDIV;      	// 0xD5
	*(dat + i2cIndex++) = 0x80;      						// the suggested ratio 0x80
	*(dat + i2cIndex++) = SSD1306_SETMULTIPLEX;              // 0xA8
	*(dat + i2cIndex++) = SSD1306_LCDHEIGHT - 1;
	*(dat + i2cIndex++) = SSD1306_SETDISPLAYOFFSET;       	// 0xD3
	*(dat + i2cIndex++) = 0x0;      							// no offset
	*(dat + i2cIndex++) = SSD1306_SETSTARTLINE | 0x0;      	// line #0 0x40
	*(dat + i2cIndex++) = SSD1306_CHARGEPUMP;      			// 0x8D
	*(dat + i2cIndex++) = 0x14;       						// Could be 0x10, could be 0x14
	*(dat + i2cIndex++) = SSD1306_MEMORYMODE;        			// 0x20
	*(dat + i2cIndex++) = 0x00;      						// 0x0 act like ks0108
	*(dat + i2cIndex++) = SSD1306_SEGREMAP | 0x1;
	*(dat + i2cIndex++) = SSD1306_COMSCANDEC;
	*(dat + i2cIndex++) = SSD1306_SETCOMPINS;       			// 0xDA
	*(dat + i2cIndex++) = 0x02;
	*(dat + i2cIndex++) = SSD1306_SETCONTRAST;      			// 0x81
	*(dat + i2cIndex++) = 0x8F;
	*(dat + i2cIndex++) = SSD1306_SETPRECHARGE;       		// 0xd9
	*(dat + i2cIndex++) = 0xF1;       						// Could be 0x22, could be 0xF1
	*(dat + i2cIndex++) = SSD1306_SETVCOMDETECT;       		// 0xDB
	*(dat + i2cIndex++) = 0x40;
	*(dat + i2cIndex++) = SSD1306_DISPLAYALLON_RESUME;      	// 0xA4
	*(dat + i2cIndex++) = SSD1306_NORMALDISPLAY;      		// 0xA6
	*(dat + i2cIndex++) = SSD1306_DEACTIVATE_SCROLL;
	*(dat + i2cIndex++) = SSD1306_DISPLAYON;
	*(dat + i2cIndex++) = 0x22;     				// 0x22 COMMAND
	*(dat + i2cIndex++) = 0x00;    							// Start Page address
	*(dat + i2cIndex++) = 0xFF;     							// Start Page address
	*(dat + i2cIndex++) = 0x21; 							
	*(dat + i2cIndex++) = 0x00;
	*(dat + i2cIndex++) = 0x7F; 							

	I2C_Write(I2C_NUM_0, dat, i2cIndex, COMMAND);
}


void DisplayClear()
{
	memset(DisplayBuffer, 0x00, DISPLAY_BUFFER_SIZE);   
}

void DisplayRefresh()
{
	I2C_Write(I2C_NUM_0, DisplayBuffer, DISPLAY_BUFFER_SIZE, DATA);
}



void SetPixel(uint8_t x, uint8_t y)
{
	uint8_t xMajor = x / 8;
	uint8_t xMinor = x % 8;
	
	DisplayBuffer[xMajor * SSD1306_LCDWIDTH + y] |= 1 << xMinor;
	
}


void TogglePixel(uint8_t x, uint8_t y)
{
	uint8_t xMajor = x / 8;
	uint8_t xMinor = x % 8;
	
	DisplayBuffer[xMajor * SSD1306_LCDWIDTH + y] ^= 1 << xMinor;
	
}

bool GetBit(uint16_t Bit, uint8_t * buf)
{
	
	uint16_t bitMajor = Bit / 8;
	uint16_t bitMinor = Bit % 8;
	
	return *(buf + bitMajor) & (1 << bitMinor);
}


void DisplayWriteText(uint8_t* Text)
{
	
	uint8_t x = 0, xOff = 0;
	uint8_t y = 0, yOff = 0;
	uint16_t datOff = 0;
	GFXfont f = FreeMono12pt7b;

	
	
	uint8_t offset = *Text -(uint8_t)' ';
	
	GFXglyph g = f.glyph[offset];
	
	uint16_t numBits = g.width * g.height;
	
	for (uint16_t i = 0; i < numBits; i++)
	{
		xOff = i % g.width;
		yOff = i / g.width;
		
		datOff = 8*((xOff * g.width + yOff) / 8) + (7 - (xOff * g.width + yOff) % 8);
		
		if (GetBit(datOff, f.bitmap + g.bitmapOffset))
		{
			SetPixel( x + xOff, y + yOff);
		}
	}

	
	
	
	DisplayRefresh();
}




void LCDTask()
{
	ESP_LOGI(LCD, "Start I2C");
	I2C_Init();	
	

	ESP_LOGI(LCD, "Init LCD");
	LCD_Init();

	DisplayClear();
	DisplayRefresh();

	
	uint8_t text[] = "MyText";
	
	DisplayWriteText(text);

//	memset(DisplayBuffer, 0x50, DISPLAY_BUFFER_SIZE);   

	
//	for (uint8_t i = 0; i < 32; i++)
//	{
//		SetPixel(i, i);
//	}
	
	DisplayRefresh();


	ESP_LOGE(LCD, "LCD TASK ENDED");
	vTaskDelete(NULL);
}


