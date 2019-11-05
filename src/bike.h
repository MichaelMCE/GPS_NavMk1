
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


#ifndef _BIKE_H_
#define _BIKE_H_

#pragma GCC optimize ("-O2")


#define ENABLE_TEMP				0


//#include <SerialFlash.h>
#include <SPI.h>
#include <TinyGPS++.h>
#include <ILI9341_due_config.h>
#include <ILI9341_due.h>
#include "SdFat/SdFat.h"
#include "SdFat/SdFatUtil.h"
#if (ENABLE_TEMP)
#include <OneWire.h>
#include <DallasTemperature.h>
#endif
//#include <Wire.h>
//#include <HMC5883L.h>
//#include <avr/dtostrf.h>
#ifndef INLINE
#define INLINE					__attribute__((always_inline))
#endif

#if defined(__SAM3X8E__)
//#undef __FlashStringHelper::F(string_literal)
#undef __FlashStringHelper
#undef F
#define F(string_literal) string_literal
#endif

#define	sl						SerialUSB				// serial log

#include "pins.h"
#include "fonts.h"
#include "printf.h"
#include "logformat.h"
#include "log.h"




//#define BIKE_SAMPLE_PERIOD		2000

#define DWIDTH					320
#define DHEIGHT					240

#define RENDER_SAT_LIMIT		24		// maximum sats we'll render

#define SERIAL_RATE				115200

#define wifi					Serial3
#define wifi_eventFunc			serialEvent3
#define WIFI_RATE				115200/*256000*/

#define gpsModule				Serial1
#define gpsModuleEventFunc		serialEvent1
#define GPS_MODULE_RATE			(115200)

#define GPS_RATE_IDLE			UBLOX_RATE_100
#define GPS_RATE_RECORDING		UBLOX_RATE_100


#define SD_SPI_SPEED			SPI_HALF_SPEED


#define FONT_SMALL				_76london20
#define FONT_SMALL_SPACING		2
#define FONT_MAIN				wintvpgothic24
#define FONT_MAIN_SPACING		4
#define FONT_SPEED				agbookroundedbq_116
#define FONT_SPEED_SPACING		9
#define FONT_SPEED_LARGE		blacktuesday191
#define FONT_DURATION			wintvpgothic36
#define FONT_DURATION_SPACING	5
#define FONT_DATETIME_DATE		sfsquarehead60
#define FONT_DATETIME_TIME		sfsquarehead60
#define FONT_DATETIME_SPACING	5
#define FONT_KARISMA105			karisma105
#define FONT_KARISMA105_SPACING 5
#define FONT_TINY				bauhausefdemibold17


#define TCX_ACTIVITY_SPORT		"Biking"
#define TCX_STRING_N(x)			sl.print(F(x"\n"))
#define TCX_STRING(x)			sl.print(F(x))




#define INPUT_MSG_BASE			1000
#define INPUT_MSG_BUTTON_UP		(INPUT_MSG_BASE+1)
#define INPUT_MSG_BUTTON_DOWN	(INPUT_MSG_BASE+2)
#define INPUT_MSG_BUTTON_HELD	(INPUT_MSG_BASE+3)
#define INPUT_MSG_BUTTON_RELEASED	INPUT_MSG_BUTTON_UP

#define INPUT_BUTTON_T_DT		((uint32_t)20)				// ms debounce period
#define INPUT_BUTTON_T_HELD		((uint32_t)300)				// ms held for period

#define BUTTON_HW_1				INPUT_PIN_BUTTON_1
#define BUTTON_HW_2				INPUT_PIN_BUTTON_2
#define BUTTON_HW_3				INPUT_PIN_BUTTON_3
#define BUTTON_HW_4				INPUT_PIN_BUTTON_4
#define BUTTON_HW_5				INPUT_PIN_BUTTON_5
#define BUTTON_HW_6				INPUT_PIN_BUTTON_6
#define BUTTON_HW_7				INPUT_PIN_BUTTON_7
#define BUTTON_HW_8				INPUT_PIN_BUTTON_8

#define BUTTON_ID_1				101				// callback Ids
#define BUTTON_ID_2				102
#define BUTTON_ID_3				103
#define BUTTON_ID_4				104
#define BUTTON_ID_5				105
#define BUTTON_ID_6				106
#define BUTTON_ID_7				107
#define BUTTON_ID_8				108

#define BUTTONS_TOTAL			8

#define BUTTON_PAGE_NEXT		BUTTON_ID_1
#define BUTTON_RECORD_START		BUTTON_ID_2
#define BUTTON_RECORD_STOP		BUTTON_ID_3
#define BUTTON_PAGE_SWAP		BUTTON_ID_4
#define BUTTON_ROUTE_LOAD		BUTTON_ID_4

#define COLOUR_WHITE			0xFFFF
#define COLOUR_BLACK			0x0000
#define COLOUR_RED				ILI9341_RED
#define COLOUR_GREEN			0x07E0
#define COLOUR_BLUE				ILI9341_BLUE
#define COLOUR_ORANGE			ILI9341_ORANGE


#define COLOUR_MAIN_BACK		COLOUR_BLACK
#define COLOUR_MAIN_FRONT		COLOUR_WHITE
#define COLOUR_ROUTE			ILI9341_GREENYELLOW
#define COLOUR_ALTITUDE			COLOUR_GREEN
#define COLOUR_COURSE			ILI9341_CADETBLUE
#define COLOUR_DISTANCE			ILI9341_TEAL/*ILI9341_CYAN*/
#define COLOUR_SPEED			ILI9341_TEAL/*ILI9341_AQUA*/
#define COLOUR_SAT_RINGS		ILI9341_ORCHID
#define COLOUR_SAT_HASFIX		ILI9341_YELLOW
#define COLOUR_SAT_NOFIX		ILI9341_INDIANRED
#define COLOUR_SAT_SIGLVL		ILI9341_MEDIUMSEAGREEN
#define STARS_COLOUR			COLOUR_WHITE



#define GPS_MINSPEED_CYCLING	(1.50f)				// don't record when not moving
#define GPS_MINSPEED_RUNNING	(0.75f)				// don't record when not moving
#define GPS_MINSPEED_WALKING	(0.00f)				// when debugging
#define GPS_DATAPTS_LIMIT		(21)
#define ROUTE_POINT_LENGTH		(1022)
#define ALTITUDE_PTS			(24000/sizeof(uint16_t))	// enough for 6.5hrs at 1 datapoint per 2 seconds (0.5hz)


#define GPS_1KM_LONG			(0.01542075/2.0)


// display modes
enum _pages {
	PAGE_INVALID = 0,
	PAGE_SATELLITE = 1,
	PAGE_ACTIVITY,
	PAGE_ROUTE,					// route and altitude
	PAGE_POI,
	PAGE_OVERVIEW,
	PAGE_SPEED,					// large speed display @ 2hz refresh
	PAGE_FILES,					// file system
	PAGE_TIME,
	
	PAGE_TOTAL = PAGE_TIME,
	PAGE_FIRST = PAGE_SATELLITE,
	PAGE_DEFAULT = PAGE_FILES
};

enum _pageMsg {
	PAGE_MSG_INVALID = 0,
	
	PAGE_MSG_START,
	PAGE_MSG_SHUTDOWN,
	
	PAGE_MSG_ENTER,
	PAGE_MSG_RENDER,
	PAGE_MSG_LEAVE,
	
	PAGE_MSG_INPUT
};

#define PAGE_RET_INPUT_CONTINUE			1			// pass this input on to the next chain
#define PAGE_RET_INPUT_HALT				2			// don't process this input any further


typedef struct bikegps_t bikegps_t;
typedef int (*render_pageCb_t) (bikegps_t *bike, void *ctx, const uint32_t msg, int32_t arg1, int32_t arg2, void *opaque);


typedef struct {
	uint8_t id;
	render_pageCb_t cb;
	int32_t arg1;
	int32_t arg2;
	void *opaque;
}gps_page_t;

typedef struct {
	struct {
		gps_page_t list[PAGE_TOTAL+1];
		uint8_t total;		// number of pages
		uint8_t current;	// currently active/renderable page (id)
		uint8_t previous;	// previously rendered page
		uint8_t defaultId;
	}pages;
}gps_control_t;



typedef void (*gps_buttonCb_t) (const int id, void *opaque, const int state, const int dt);

typedef struct {
	int state;
	uint32_t time;		// time button pressed
	uint32_t timeHeld;		
	int pin;
	void *opaque;
	int id;
	gps_buttonCb_t cb;
}gps_button_t;

typedef struct {
	volatile int event;
	volatile uint32_t time0;
	int eventOccured;
	gps_button_t btn[BUTTONS_TOTAL];
}gps_buttons_t;

typedef struct {
	float latitude;
	float longitude;
}gps_loc_t;

typedef struct {
	gps_loc_t loc;
	char *text;
}gps_poi_t;

typedef struct {
	uint16_t x;
 	uint16_t y;	// could make this uint8_t packed
}tft_pos_t;

typedef struct {
	int written;
	tft_pos_t table[2][ROUTE_POINT_LENGTH+2];
	tft_pos_t *back;
	tft_pos_t *front;
}tft_lut_t;

typedef struct {
	tft_lut_t lut;
	int swapSignalled;
}route_table_t;
	
typedef struct {
	int update;
	int update2;
	//uint32_t timePrevious;		// time last rendered
	ILI9341_due tft = ILI9341_due(TFT_PIN_CS, TFT_PIN_DC, TFT_PIN_RST);
	uint16_t displayReinit;
	//unsigned int updateId;
	//uint16_t nemaUpdateEventId;
	uint16_t clean;
	//uint16_t displayPage;
	//uint16_t displayPagePrevious = 999;
	
	route_table_t route;
}bike_render_t;

/*
typedef struct {
	//volatile uint8_t gpsEn;
	volatile unsigned int gpsPPS;
}bike_isr_t;
*/

typedef struct {
	gps_pos_t pos;			// current location
	gps_pos_t posPrevious;
	gps_pos_t min;
	gps_pos_t max;

	double distance;		//
	float altitude;
	float hdop;
	float speedKm;			// speed as supplied/decoded from module
	float speed;			// speed value as rendered
	float maxSpeed;
	float aveSpeed;
	float course;
	float courseDisplayed;	// course as last display
	float temp;				// temperature in C

	struct {
		uint8_t hour;
		uint8_t minute;
		uint8_t second;
	}time;
	
	struct {
		uint8_t day;
		uint8_t month;
		uint8_t year;
	}date;
	
	uint8_t hasFix;
	uint8_t satellites;
	uint8_t update;
}bike_stats_t;

typedef struct {
	double daspect;
	double aspect;
	double aspectFactor;
	
	double scaleFactor;
	double scaleLong;
	double scaleLat;
	double scaleLatZoom;
	double scaleLongZoom;

	double zoomFactor;
			
	double dLong;
	double dLat;
	double minLong;
	double minLat;
	double maxLong;
	double maxLat;
	
	//double startX;
	//double startY;

	double scaleFix = 1.10;		// aspect fixup,  shouldn't really need this

	uint16_t width;
	uint16_t height;
	
	// could probably use float for these
	double widthScale;
	double heightScale;
	double widthMultiplier;
	double heightMultiplier;
	
	struct {
		double distanceLong;
		double distanceLat;
		double metersPerPixelH;
		double metersPerPixelV;
	}ruler;

	int offsetX;
	int offsetY;
}gps_route_t;


typedef struct {
	uint32_t time;			// time in seconds
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
	uint8_t ms;		// centi
}log_time_t;

typedef struct {
	uint32_t count;
	uint32_t sp;
	uint32_t startTime;
		
	//int32_t recordInterval;
	//int32_t recordTime;

	int32_t recordTimeStart;
	int32_t recordTimeEnd;
	
	uint32_t timeIdleStart;
	uint32_t timeIdleEnd;
	uint32_t timeIdleSum;
	
	log_time_t duration;
	
	uint16_t year;
	uint8_t month;
	uint8_t day;
	
	int8_t isRecording;
	uint8_t lastWriteSuccess;

	log_header_t header;
	gps_datapt_t log[GPS_DATAPTS_LIMIT];
	
	struct {
		uint8_t recordTimeStart_updated;
		uint8_t recordTimeEnd_updated;
		uint8_t totalSeconds_updated;
		uint8_t speedMax_updated;
		uint8_t writeRemaining;
	}flags;
	
	int update;
}gps_track_t;


typedef struct {
#if (ENABLE_TEMP)
	OneWire oneWire = OneWire(DS18B20_PIN);
	DallasTemperature sensor = DallasTemperature();
#endif
	int tempReadReady;
}bike_temp_t;


typedef struct {
	gps_track_t track;
	gps_route_t rc;
	gps_log_t log;
}gps_map_t;

typedef struct {
	uint16_t ptIdx;		// add most recent data point here
	uint16_t updated = 1;
	uint16_t min;
	uint16_t max;
	
	struct {
		uint16_t width = DWIDTH;
		uint8_t	buffer[DWIDTH];
	}render;
	uint16_t alt16[ALTITUDE_PTS];
}gps_altitude_t;

struct bikegps_t {
	volatile int counter;
	volatile int counter1;
	
	gps_buttons_t buttons;
	TinyGPSPlus gps;
	//HMC5883L compass;
	bike_render_t render;
	bike_stats_t stats;
	//volatile bike_isr_t isr;
	gps_map_t map;
	bike_temp_t temp;
	
	struct {
		gps_map_t map;
		
		struct {	
			uint16_t total = 1024;		// number of points in [below] table
			uint16_t written;
			gps_loc_t loc[1024];
		}route;
	}staticmap;
	
	gps_control_t ctrl;
	gps_altitude_t altitude;		// should really be part of route_t
};

#define STARS_TOTAL			(128+16)
#define STARS_PLANES		(4)

typedef struct {
	uint16_t x;
	uint8_t y;
	uint8_t plane;
}gfx_star_t;


typedef struct {
	uint16_t colour[STARS_PLANES] = {STARS_COLOUR/4, STARS_COLOUR/3, STARS_COLOUR/2, STARS_COLOUR};
	uint8_t velocity[STARS_PLANES] = {1, 2, 3, 4};
	gfx_star_t star[STARS_TOTAL];
	uint8_t total = STARS_TOTAL;
}gfx_stars_t;

#define lutFront(rt)	((rt)->lut.front)
#define lutBack(rt)		((rt)->lut.back)







void displayClean (bikegps_t *bike);
void displayUpdate (bikegps_t *bike);

void displaySetPage (bikegps_t *bike, const int page, const int clean);
int displayGetPage (bikegps_t *bike);
//void displaySetPagePre (bikegps_t *bike, const int page);
int displayGetPagePre (bikegps_t *bike);

int displayInfoDuration (bikegps_t *bike, ILI9341_due *tft, const int x, const int y);
void displayInfoSpeed (bikegps_t *bike, ILI9341_due *tft, const int x, const int y);

void displayInfoSpeedAlt (bikegps_t *bike, ILI9341_due *tft, const int x, const int y);
int displayInfoAveSpeed (bikegps_t *bike, ILI9341_due *tft, const int x, const int y);

int displayInfoAltitude (bikegps_t *bike, ILI9341_due *tft, const int x, const int y, const int dPt);
int displayInfoDistance (bikegps_t *bike, ILI9341_due *tft, const int x, const int y);
int displayInfoTime (bikegps_t *bike, ILI9341_due *tft, const int x, const int y);
int displayInfoTemp (bikegps_t *bike, ILI9341_due *tft, const int x, const int y);
void drawRoute4 (bikegps_t *bike, ILI9341_due *tft, const tft_pos_t *const pos, const int lutTotal, const uint16_t colour);
void displayCourse (bikegps_t *bike, ILI9341_due *tft, const int cx, const int cy, const uint16_t length, const float headSize, const float course);
void displayRoute (bikegps_t *bike, ILI9341_due *tft, gps_route_t *rc, const uint16_t colour, const int drawStaticMap);
void displayRecordingStatus (bikegps_t *bike, ILI9341_due *tft);

void displayClear (bikegps_t *bike);

void drawAltitude (gps_altitude_t *altitude, ILI9341_due *tft, gps_route_t *rc, const int thickness, const uint16_t colour);
void drawAltitudeCalcPixels (gps_altitude_t *altitude, gps_route_t *rc);
void altitudeReset (bikegps_t *bike);

void poiDraw (bikegps_t *bike, ILI9341_due *tft, gps_route_t *rc, const gps_poi_t *iPts);
const gps_poi_t *poiTable ();

void displayLocationInfo (bikegps_t *bike, ILI9341_due *tft, const int x, const int y);
void displaySignalGraph (bikegps_t *bike, ILI9341_due *tft, const uint32_t t);
void displaySignalWorld (bikegps_t *bike, ILI9341_due *tft, const int cx, const int cy);


void logEnd (bikegps_t *bike);
int logStartNew (bikegps_t *bike);
void logContinue (bikegps_t *bike);
void logStop (bikegps_t *bike);
int logIsActive (bikegps_t *bike);
int logIsRecording (bikegps_t *bike);
int logLastWriteStatus (bikegps_t *bike);
void logImportPrevious (bikegps_t *bike);
void logImportNext (bikegps_t *bike);
void logResync (bikegps_t *bike, gps_log_t *log);

int logLoad (bikegps_t *bike, gps_log_t *log, gps_track_t *track, const int index, const char *filename);
int loadMap (bikegps_t *bike, gps_map_t *map, gps_log_t *log, const char *filename);


#endif


