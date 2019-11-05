
// A bike computer
// 
// Michael McElligott
// okio@users.sourceforge.net

//  Copyright (c) 2005-2015  Michael McElligott
// 
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU LIBRARY GENERAL PUBLIC LICENSE
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU LIBRARY GENERAL PUBLIC LICENSE for more details.
//
//	You should have received a copy of the GNU Library General Public
//	License along with this library; if not, write to the Free
//	Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.


#ifndef _PINS_H_
#define _PINS_H_


#define ISR_PIN_GPS_PPS			11
#define ISR_PIN_GPS_EN			12


#define TFT_PIN_RST				8
#define TFT_PIN_DC				9
#define TFT_PIN_CS				10
#define SD_PIN_CS				4
#define WIFI_PIN_EN				2
#define FLASH_PIN_CS			52
#define DS18B20_PIN				53

#define INPUT_PIN_BUTTON_1		22
#define INPUT_PIN_BUTTON_2		24
#define INPUT_PIN_BUTTON_3		26
#define INPUT_PIN_BUTTON_4		28
#define INPUT_PIN_BUTTON_5		30
#define INPUT_PIN_BUTTON_6		32
#define INPUT_PIN_BUTTON_7		34
#define INPUT_PIN_BUTTON_8		36

#define sd_enable()					\
	do{								\
	digitalWrite(SD_PIN_CS, LOW);	\
	delay(1);						\
	}while(0)						\

INLINE static void sd_disable ()
{
	digitalWrite(SD_PIN_CS, HIGH);
	//delay(1);
}

INLINE static void tft_enable ()
{
	//digitalWrite(TFT_PIN_CS, LOW);
	//delay(1);
}

INLINE static void tft_disable ()
{
	//digitalWrite(TFT_PIN_CS, HIGH);
	//delay(1);
}

INLINE static void sf_enable ()
{
	digitalWrite(FLASH_PIN_CS, LOW);
	//delay(1);
}

INLINE static void sf_disable ()
{
	digitalWrite(FLASH_PIN_CS, HIGH);
	//delay(1);
}


#endif

