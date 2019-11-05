
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



// serial 2 and 3 disabled within variant.cpp

#pragma GCC optimize ("-O2")


#include <DueTimer.h>
#include "bike.h"
#include "ublox.h"
#include "pages.h"




/*
degrees = arctan((percent grade)/100)
percent grade = tan(degrees) * 100

1% incline is a mere .57 degrees, 5% incline is 2.9 degrees, 10%
incline is 5.7 degrees, and 15% incline is 8.5 degrees. 
*/


static bikegps_t bikegps;


static float GPS_DATAPTS_MINSPEED = GPS_MINSPEED_CYCLING;



static inline void renderRequestClean (bikegps_t *bike)
{
	bike->render.clean = 1;
}

static inline void renderRequestUpdate (bikegps_t *bike)
{
	//bike->render.timePrevious = 0;
	bike->render.update = 1;
}

void displaySetPage (bikegps_t *bike, const int page, const int clean)
{
	pageSetActive(bike, &bike->ctrl, page);
	if (clean) renderRequestClean(bike);
}

int displayGetPage (bikegps_t *bike)
{
	return pageGetActive(&bike->ctrl);
}

int displayGetPagePre (bikegps_t *bike)
{
	return pageGetActivePrevious(&bike->ctrl);
}

static inline void buttonCancel (gps_button_t *button)
{
	button->state = 0;
	button->time = 0;
	button->timeHeld = 0;
}

static inline void buttonsCancel (gps_buttons_t *buttons)
{
	buttons->eventOccured = 0;
	buttons->event = 0;
	
	for (int i = 0; i < BUTTONS_TOTAL; i++)
		buttonCancel(&buttons->btn[i]);

}

static inline void buttonISR ()
{
	gps_buttons_t *buttons = &bikegps.buttons;
	
	buttons->event = 1;
	buttons->time0 = millis();
}

static inline int buttonAdd (gps_buttons_t *buttons, const int pin, gps_buttonCb_t cb, const int id, void *opaque)
{
	for (int i = 0; i < BUTTONS_TOTAL; i++){
		gps_button_t *btn = &buttons->btn[i];
		if (!btn->id){
			btn->id = id;
			btn->state = 0;
			btn->pin = pin;
			btn->cb = cb;
			btn->opaque = opaque;

			pinMode(pin, INPUT_PULLUP);
			attachInterrupt(pin, buttonISR, CHANGE);
			return 1;
		}
	}
	return 0;
}

static inline void lutSwap (route_table_t *rt)
{
	if (rt->lut.front == rt->lut.table[0]){
		rt->lut.front = rt->lut.table[1];
		rt->lut.back = rt->lut.table[0];
	}else{
		rt->lut.front = rt->lut.table[0];
		rt->lut.back = rt->lut.table[1];
	}
}

/*
static inline tft_pos_t *lutFront (const gps_route_t *rc)
{
	return rc->lut.front;
}

static inline tft_pos_t *lutBack (const gps_route_t *rc)
{
	return rc->lut.back;
}

static inline void lutSwap (gps_route_t *rc)
{
	if (rc->lut.front == rc->lut.table[0]){
		rc->lut.front = rc->lut.table[1];
		rc->lut.back = rc->lut.table[0];
	}else{
		rc->lut.front = rc->lut.table[0];
		rc->lut.back = rc->lut.table[1];
	}
}
*/


static inline void statsPositionReset (bikegps_t *bike)
{
	bike->stats.min.latitude = 999.0;
	bike->stats.min.longitude = 999.0;
	bike->stats.max.latitude = -999.0;
	bike->stats.max.longitude = -999.0;
	memset(&bike->stats.posPrevious, 0, sizeof(gps_pos_t));
}
	

static inline float track_time (gps_track_t *track)
{
	return ((track->recordTimeEnd - track->recordTimeStart) - track->timeIdleSum);
}

static inline int track_timeS (gps_track_t *track)
{
	return ((track->recordTimeEnd - track->recordTimeStart) - track->timeIdleSum) / 1000;
}

static inline int track_totalPoints (gps_track_t *track)
{
	return track->count;	// this works because count is increased once per second and only when moving
}

static inline float track_aveSpeedKm (bikegps_t *bike, gps_track_t *track)
{
	int totalSeconds = track_totalPoints(track);
	if (totalSeconds > 0){
		float aveSpeed = bike->stats.distance / (float)totalSeconds;
		if (aveSpeed > 0.00f)
			return aveSpeed * 3.6f;
	}
	return 0.0f;
}

static inline void tcx_calcScaleMultipliers (gps_route_t *rc)
{
	if (rc->aspect <= rc->scaleFactor){
		rc->aspectFactor = rc->aspect/rc->scaleFactor;
		rc->scaleLong = rc->aspectFactor / rc->dLong;
		rc->scaleLat = 1.0 / rc->dLat;
			
		//sl.println("long factor: %f\n", factor);
		
	}else{
		rc->aspectFactor = 1.0/(rc->aspect/rc->scaleFactor);
		rc->scaleLat = rc->aspectFactor / rc->dLat;
		rc->scaleLong = 1.0 / rc->dLong;
						
		//sl.println("lat factor: %f\n", factor);
	}	

	//sl.printf("calcNormalization %f %f %f\n", rc->aspectFactor, rc->scaleLat, rc->scaleLong);	
	//sl.print("rc->aspectFactor: ");
	//sl.println(rc->aspectFactor);
	//return rc->aspectFactor;
	return;
}
 
static inline double normalizeLong (gps_route_t *rc, const double longitude)
{
	return (longitude - rc->minLong) * rc->scaleLongZoom;
}

static inline double normalizeLat (gps_route_t *rc, const double latitude)
{
	return 1.0 - ((latitude - rc->minLat) * rc->scaleLatZoom);
}

static inline float calcScaleLong (gps_route_t *rc, const double longitude)
{
	return normalizeLong(rc, longitude) * rc->widthScale;
}

static inline float calcScaleLat (gps_route_t *rc, const double latitude)
{
	return normalizeLat(rc, latitude) * rc->heightScale;
}

static inline void tcx_setScale (gps_route_t *rc, const double widthMultiplier, const double heightMultiplier)
{
	rc->widthMultiplier = widthMultiplier;
	rc->widthScale = rc->width * widthMultiplier;
	
	rc->heightMultiplier = heightMultiplier;
	rc->heightScale = rc->height * heightMultiplier;
}

static inline void tcx_setZoom (gps_route_t *rc, const double factor)
{
	rc->zoomFactor = factor;
	rc->scaleLatZoom = rc->scaleLat * rc->zoomFactor;
	rc->scaleLongZoom = rc->scaleLong * rc->zoomFactor;
}

static inline void tcx_rescale (gps_route_t *rc, const double scale)
{
	//printf("garminTcxRescale %f %f %f\n", scale, offsetX, offsetY);
	
	// normalize() and scale() must be called after each call to zoom();
	tcx_setZoom(rc, scale);

	// normalize data points
	//tcx_normalize(rc, activities, 0, rc->minLong, rc->minLat);

	//calcWorldPositions(tcx, rc, WORLD_ZOOM, offsetX, offsetY);

	// scale for display 
	tcx_setScale(rc, 1.0, 1.0);

	double dPixelsW;
	double dPixelsH;
	if (rc->aspect <= rc->scaleFactor){
		dPixelsW = (rc->width * rc->aspectFactor) * scale;
		dPixelsH = (rc->height * rc->aspectFactor) * (1.0/rc->aspectFactor) * scale;
	}else{
		dPixelsW = (rc->width * rc->aspectFactor) * (1.0/rc->aspectFactor) * scale;
		dPixelsH = (rc->height * rc->aspectFactor) * scale;
	}

	double mlat = rc->minLat + (rc->dLat / 2.0);
	double mlong = rc->minLong + (rc->dLong / 2.0);
	rc->ruler.distanceLong = TinyGPSPlus::distanceBetween(mlat, rc->minLong, mlat, rc->maxLong);
	rc->ruler.distanceLat = TinyGPSPlus::distanceBetween(rc->minLat, mlong, rc->maxLat, mlong);
	rc->ruler.metersPerPixelH = rc->ruler.distanceLong / dPixelsW;
	rc->ruler.metersPerPixelV = rc->ruler.distanceLat / dPixelsH;

}

static inline void tcx_updateRenderContext (bikegps_t *bike, gps_route_t *rc)
{
	rc->daspect = (double)rc->width / (double)rc->height;
	rc->scaleFactor = (4.0/3.0) * rc->daspect * rc->scaleFix;
	
#if 0
	rc->minLong = bike->stats.pos.longitude - GPS_1KM_LONG;
	rc->maxLong = bike->stats.pos.longitude + GPS_1KM_LONG;
	rc->minLat = bike->stats.pos.latitude - (GPS_1KM_LONG * rc->scaleFactor);
	rc->maxLat = bike->stats.pos.latitude + (GPS_1KM_LONG * rc->scaleFactor);

#else
	rc->minLat = bike->stats.min.latitude;
	rc->minLong = bike->stats.min.longitude;
	rc->maxLat = bike->stats.max.latitude;
	rc->maxLong = bike->stats.max.longitude;
#endif

	rc->dLong = rc->maxLong - rc->minLong;
	rc->dLat = rc->maxLat - rc->minLat;
	rc->aspect = rc->dLong / rc->dLat;
	rc->zoomFactor = 1.0;
	
	//printf("rc->daspect %f %f, %f\n", rc->daspect, rc->aspect, rc->scaleFactor);
}

static inline void tcx_initRenderContext (bikegps_t *bike, gps_route_t *rc, const int width, const int height)
{
	rc->width = width;
	rc->height = height;
	
	//rc->altitude.width = width;
	//rc->altitude.height = height>>1;

	statsPositionReset(bike);
	tcx_updateRenderContext(bike, rc);
	tcx_calcScaleMultipliers(rc);
}

static inline void tcx_calcRouteCenter (gps_route_t *rc)
{
	double scale = ((rc->daspect * rc->aspect) / rc->scaleFactor);

	// center route
	if (rc->aspect <= rc->scaleFactor){
		scale *= rc->zoomFactor;
		rc->offsetX = (rc->width - (rc->height * scale)) / 2.0;
		rc->offsetY = (rc->height - (rc->height * rc->zoomFactor)) / 2.0;
	}else{
		scale /= rc->zoomFactor;
		rc->offsetY = (rc->height - (rc->width / scale)) / 2.0;
		rc->offsetX = (rc->width - (rc->width * rc->zoomFactor)) / 2.0;
	}
}

/*
static inline float compassGetHeading (HMC5883L *compass)
{
	compass->SetMeasurementMode(Measurement_SingleShot);
	MagnetometerScaled scaled = compass->ReadScaledAxis();

	// Calculate heading when the magnetometer is level, then correct for signs of axis.
	// Atan2() automatically check the correct formula taking care of the quadrant you are in
	float heading = atan2(scaled.YAxis, scaled.XAxis);

	// Once you have your heading, you must then add your 'Declination Angle',
	// which is the 'Error' of the magnetic field in your location. Mine is 0.0404 
	// Find yours here: http://www.magnetic-declination.com/
  
	// If you cannot find your Declination, comment out these two lines, your compass will be slightly off.
	float declinationAngle = 0.0523;
	heading += declinationAngle;

	// Correct for when signs are reversed.
	if(heading < 0.0) heading += 2.0*PI;

	// Check for wrap due to addition of declination.
	if(heading > 2.0*PI) heading -= 2.0*PI;

	// Convert radians to degrees for readability.
	float headingDegrees = heading * 180.0/M_PI;
	
	//sl.println(headingDegrees);
	//sl.println(heading);
	
	return headingDegrees;
}
*/


static inline void time32ToTime (const uint32_t time32, log_time_t *time)
{
	time->time = time32;
	time->ms = (time->time % 1000) / 10;
	time->time /= 1000;
	time->second = time->time % 60;
	time->minute = (time->time / 60) % 60;
	time->hour = (time->time / 60) / 60;
}

static inline char *timeToString (uint32_t time, char *str, const int len)
{
	int hr = time / 1000000;
	int min = (time / 10000) % 100;
	int sec = (time / 100) % 100;
	if (sec == 60){
		sec = 0;
		if (++min == 60){
			min = 0;
			if (++hr > 24)
				hr = 0;
		}
		
	}
	//int mil = time % 100;

	//snprintf(str, len-1, "%.2i:%.2i:%.2i.%.3iZ", hr, min, sec, mil);
	snprintf(str, len-1, "%.2i:%.2i:%.2iZ", hr, min, sec);
	return str;
}
   
static inline char *dateToString (uint32_t date, char *str, const int len, const int format)
{
	int year = date % 100;
	int month = (date / 100) % 100;
	int day = date / 10000;
	
	snprintf(str, len-1, "%.4i-%.2i-%.2iT", year, month, day);
	return str;
}

static inline double calcDistance (const gps_pos_t *p1, const gps_pos_t *p2)
{
	return TinyGPSPlus::distanceBetween(p1->latitude, p1->longitude, p2->latitude, p2->longitude);
}

static inline void renderClear (bikegps_t *bike)
{
	bike->render.tft.fillScreen(COLOUR_MAIN_BACK);
	bike->stats.speed = -1.0f;	// fix me
}

void displayClear (bikegps_t *bike)
{
	renderClear(bike);
}

void displayInfoSpeed (bikegps_t *bike, ILI9341_due *tft, const int x, const int y)
{
	static int spd_x;

	const float spd = bike->stats.speedKm;
	if (spd != bike->stats.speed){
		bike->stats.speed = spd;

		//tft->setFont(FONT_SPEED);
		//tft->setTextLetterSpacing(FONT_SPEED_SPACING);
		//tft->setColour(COLOUR_MAIN_FRONT);
		tft->setCursor(x, y);
		tft->print(spd, (spd > 0.1f));
		
		int new_x = tft->getCursorX();
		tft->println("");
		if (new_x < spd_x)
			bike->render.tft.fillRect(new_x, y, (spd_x-new_x)+1, (tft->getCursorY()-y)-1, COLOUR_MAIN_BACK);
		spd_x = new_x;
	}
}

void displayInfoSpeedAlt (bikegps_t *bike, ILI9341_due *tft, const int x, const int y)
{
	static int spd_x;
	static float spd_old = -1.0f;

#if 1
	const float spd = bike->stats.speedKm;		// 1hz rate
#else
	float spd = bike->gps.speed.kmph();			// 10hz update rate
	if (spd < 1.0f)
		spd = 0.0f;
	else if (spd > 99.9f)
		spd = 99.9f;
#endif

	if (spd != spd_old || displayGetPagePre(bike) != displayGetPage(bike)){
		spd_old = spd;

		//tft->setFont(FONT_SPEED);
		//tft->setTextLetterSpacing(FONT_SPEED_SPACING);
		//tft->setColour(COLOUR_MAIN_FRONT);
		tft->setCursor(x, y);
		tft->print(spd, (spd > 0.1f));
		
		int new_x = tft->getCursorX();
		tft->println("");
		if (new_x < spd_x)
			bike->render.tft.fillRect(new_x, y, (spd_x-new_x)+1, (tft->getCursorY()-y)-1, COLOUR_MAIN_BACK);
		spd_x = new_x;
	}
}

static inline int getLatestPosition (bikegps_t *bike)
{
	if (!bike->gps.location.isValid())
		return 0;

	tinygps_pos_t pos = {0.0, 0.0, 0.0f};
	bike->gps.location.cbuffSum(&pos);
	bike->stats.pos.latitude = pos.latitude;
	bike->stats.pos.longitude = pos.longitude;
	bike->stats.altitude = pos.altitude;
	
	return 1;
}

void displayLocationInfo (bikegps_t *bike, ILI9341_due *tft, const int x, const int y)
{
	static int16_t lng_x, lat_x, alt_x, sat_x, tme_x;


	getLatestPosition(bike);
	tft->setCursor(x, y);
	
	int y1 = y;
	tprintf("Lng: %.6f", bike->stats.pos.longitude);
	int new_x = tft->getCursorX();
	tft->println("");
	int y2 = tft->getCursorY();
	if (new_x < lng_x)
		tft->fillRect(new_x, y1, (lng_x-new_x)+1, (y2-y1)-1, COLOUR_MAIN_BACK);
	lng_x = new_x;
	tft->setCursor(x, y2);
	
	
	y1 = y2;
	tprintf("Lat: %.6f", bike->stats.pos.latitude);
	new_x = tft->getCursorX();
	tft->println("");
	y2 = tft->getCursorY();
	if (new_x < lat_x)
		tft->fillRect(new_x, y1, (lat_x-new_x)+1, (y2-y1)-1, COLOUR_MAIN_BACK);
	lat_x = new_x;
	tft->setCursor(x, y2);


	y1 = y2;
	tprintf("Alt: %.1f", bike->stats.altitude);
	new_x = tft->getCursorX();
	tft->println("");
	y2 = tft->getCursorY();
	if (new_x < alt_x)
		tft->fillRect(new_x, y1, (alt_x-new_x)+1, (y2-y1)-1, COLOUR_MAIN_BACK);
	alt_x = new_x;
	tft->setCursor(x, y2);

	y1 = y2;
	tprintf("%02i:%.2i:%.2i", bike->stats.time.hour, bike->stats.time.minute, bike->stats.time.second);
  	new_x = tft->getCursorX();
	tft->println("");
	y2 = tft->getCursorY();
	if (new_x < tme_x)
		tft->fillRect(new_x, y1, (tme_x-new_x)+1, (y2-y1)-1, COLOUR_MAIN_BACK);
	tme_x = new_x;


	//if (logLastWriteStatus(bike) && logIsRecording(bike))	
	//	tft->setColour(ILI9341_LAWNGREEN);
	//else
	//	tft->setColour(ILI9341_RED);


	// satallites
	y1 = y2;
	tprintf("Sats: %i:%i", bike->stats.satellites, bike->gps.satellites.visable);
		
	new_x = tft->getCursorX();
	tft->println("");
	y2 = tft->getCursorY();
	if (new_x < sat_x)
		tft->fillRect(new_x, y1, (sat_x-new_x)+1, (y2-y1)-1, COLOUR_MAIN_BACK);
	sat_x = new_x;
	tft->setCursor(x, y2);


	// fix type
	if (bike->stats.hasFix == 2)
		tft->printAligned("2D", gTextAlignTopRight);
	else if (bike->stats.hasFix == 3)
		tft->printAligned("3D", gTextAlignTopRight);
	else
		tft->printAligned("##", gTextAlignTopRight);
	//tft->setColour(COLOUR_MAIN_FRONT);	

	// accuracy
	tft->setCursor(x, tft->getCursorY());
	tft->println("");
	
	float hAcc = bike->gps.fix.hAcc;
	if (hAcc > 50.0f) hAcc = 0.0f;
	tft->setCursor(DWIDTH-46, tft->getCursorY());
	tft->print(hAcc, bike->gps.fix.hAcc >= 10.0f ? 0 : 1);
	tft->printAt("", tft->getCursorX(), tft->getCursorY(), gTextEraseToEOL);
}

int displayInfoTime (bikegps_t *bike, ILI9341_due *tft, const int x, const int y)
{
	static int16_t tme_x;
	
	tft->setColour(COLOUR_MAIN_FRONT);
	tft->setCursor(x, y);
	
	tprintf("%02i:%.2i:%.2i", bike->stats.time.hour, bike->stats.time.minute, bike->stats.time.second);
  	int new_x = tft->getCursorX();
	tft->println("");
	int y2 = tft->getCursorY();
	if (new_x < tme_x)
		tft->fillRect(new_x, y, (tme_x-new_x)+1, (y2-y)-1, COLOUR_MAIN_BACK);
	tme_x = new_x;
	
	return y2;
}

int displayInfoAveSpeed (bikegps_t *bike, ILI9341_due *tft, const int x, const int y)
{
	static int16_t ave_x;
	
	tft->setCursor(x, y);
	
	tprintf("%.1f", bike->stats.aveSpeed);
	int new_x = tft->getCursorX();
	tft->println("");
	int y2 = tft->getCursorY();
	if (new_x < ave_x)
		tft->fillRect(new_x, y, (ave_x-new_x)+1, (y2-y)-1, COLOUR_MAIN_BACK);
	ave_x = new_x;
	
	return y2;
}

int displayInfoAltitude (bikegps_t *bike, ILI9341_due *tft, const int x, const int y, const int dPt)
{
	static int16_t alt_x;
	
	tft->setColour(COLOUR_ALTITUDE);
	tft->setCursor(x, y);
		
	if (dPt)
		tprintf("%.1f", bike->stats.altitude);
	else
		tprintf("%.0f", bike->stats.altitude);
	int new_x = tft->getCursorX();
	tft->println("");
	int y2 = tft->getCursorY();
	if (new_x < alt_x)
		tft->fillRect(new_x, y, (alt_x-new_x)+1, (y2-y)-1, COLOUR_MAIN_BACK);
	alt_x = new_x;
	
	return y2;	
}

int displayInfoDuration (bikegps_t *bike, ILI9341_due *tft, const int x, const int y)
{
	static int16_t dur_x;
	
	if (logLastWriteStatus(bike) && logIsRecording(bike))	
		tft->setColour(ILI9341_LAWNGREEN);
	else
		tft->setColour(ILI9341_RED);
	tft->setCursor(x, y);
	
	gps_track_t *track = &bike->map.track;
	tprintf("%i:%.2i:%.2i", track->duration.hour, track->duration.minute, track->duration.second);
	int new_x = tft->getCursorX();
	tft->println("");
	int y2 = tft->getCursorY();
	if (new_x < dur_x)
		tft->fillRect(new_x, y, (dur_x-new_x)+1, (y2-y)-1, COLOUR_MAIN_BACK);
	dur_x = new_x;

	tft->setColour(COLOUR_MAIN_FRONT);
	
	return y2;
}

int displayInfoDistance (bikegps_t *bike, ILI9341_due *tft, const int x, const int y)
{
	static int16_t dis_x;
	
	tft->setColour(COLOUR_DISTANCE);
	tft->setCursor(x, y);

	const float distance = bike->stats.distance/1000.0f;
	tprintf("%.1f", distance);
	int new_x = tft->getCursorX();
	tft->println("");
	int y2 = tft->getCursorY();
	if (new_x < dis_x){
		tft->fillRect(new_x, y, (dis_x-new_x)+1, (y2-y)-1, COLOUR_MAIN_BACK);
	}
	dis_x = new_x;
	
	return y2;
}

void displaySignalGraph (bikegps_t *bike, ILI9341_due *tft, const uint32_t t)
{
	const int satLimit = 17;
	
	uint16_t x = 0;
	const uint16_t y = DHEIGHT - 5;
	const uint16_t yMinus100 = y - 100;
	const uint16_t gap = 2;
	const uint16_t width = ((DWIDTH - (gap*(satLimit-1))) / satLimit);
	const uint16_t pitch = width + gap;
	
	TinyGPSSatalitesSignal *sats = bike->gps.getSatalites();
	TinyGPSSatalitesSignal *sat = sats;
	
	// draw bottom colours level of grap
	tft->drawFill_start(width, 100, COLOUR_SAT_SIGLVL);
	
	for (int i = 0; i < SAT_MAX; i++, sat++){
		if ((int32_t)(t - sat->age) > 4000) continue;

		if (sat->cnr)
			tft->drawRectFilled_write(x, y-sat->cnr, width, sat->cnr);
		x += pitch;
	}
	tft->draw_end();
	
	// clear top level of graph
	x = 0;
	sat = sats;
	tft->drawFill_start(width, 100, COLOUR_MAIN_BACK);
	
	for (int i = 0; i < SAT_MAX; i++, sat++){
		if ((int32_t)(t - sat->age) > 4000) continue;

		if (sat->cnr)
			tft->drawRectFilled_write(x, yMinus100, width, 100-sat->cnr-1);
		x += pitch;
	}
	tft->draw_end();
}

void displaySignalWorld (bikegps_t *bike, ILI9341_due *tft, const int cx, const int cy)
{

	struct posxy_t {
		uint16_t x;
		uint16_t y;
	};
	
	static posxy_t sats[RENDER_SAT_LIMIT];
	const int satSize = 5;


	// draw rings
	for (int16_t r = 2; r <= 105; r += 15)
		tft->drawCircle(cx, cy, r, COLOUR_SAT_RINGS);


#if 0	
	// draw central connecting lines
	for (int i = 0; i < SAT_MAX; i++){
		struct TinyGPSSatalitesSignal *sat = &bike->gps.satellites.sats[i];
		if (!sat->id) continue;

		uint16_t colour;
		if (sat->cnr)
			colour = ILI9341_LAVENDER;
		else
			colour = ILI9341_INDIANRED;
			
		const float elv = 90.0f - (float)sat->elevation;
		tft->drawLineByAngle(cx, cy, -sat->azimuth, elv, colour);
	}
#endif


	// clear previous Sats/circles
	if (displayGetPagePre(bike) == displayGetPage(bike)){
		for (int s = 0; sats[s].x && s < RENDER_SAT_LIMIT; s++){
			tft->fillCircle(sats[s].x, sats[s].y, satSize, COLOUR_MAIN_BACK);
			sats[s].x = 0;
		}
	}

	
	// draw Sats
	uint16_t colour;
	const uint32_t t = millis();
	TinyGPSSatalitesSignal *sat = bike->gps.getSatalites();
	int s = 0;
	
	for (int i = 0; i < SAT_MAX && s < RENDER_SAT_LIMIT; i++, sat++){
		if (!sat->id) continue;
		if ((int32_t)(t - sat->age) > 4000) continue;

		if (sat->cnr)
			colour = COLOUR_SAT_HASFIX;
		else
			colour = COLOUR_SAT_NOFIX;
		
		float elv = 90.0f - (float)sat->elevation;
		float az = (float)sat->azimuth - 90.0f;
		//az += heading;

		sats[s].x = cx + (elv * -tft->cosDegrees(az));
		sats[s].y = cy + (elv * tft->sinDegrees(az));
		tft->fillCircle(sats[s].x, sats[s].y, satSize, colour);
		s++;
	}
}

void displayCourse (bikegps_t *bike, ILI9341_due *tft, const int cx, const int cy, const uint16_t length, const float headSize, const float course)
{
	/*
	tft->setFont(FONT_MAIN);
	double heading = Compass.GetHeadingDegrees();
	char txt[12];
	sprintf(txt, "         %.1f ", heading);
	tft->printAligned(txt, gTextAlignBottomRight);
	//sl.println(heading);
	
	*/
	if (bike->stats.hasFix < 2 || bike->stats.speedKm < GPS_DATAPTS_MINSPEED)
		return;

	if (course != bike->stats.courseDisplayed)
		tft->fillArc(cx, cy, length, 6, bike->stats.courseDisplayed-headSize, bike->stats.courseDisplayed+headSize, COLOUR_MAIN_BACK);
	tft->fillArc(cx, cy, length, 6, course-headSize, course+headSize, COLOUR_COURSE);
	bike->stats.courseDisplayed = course;

}

static inline void drawRoute1 (bikegps_t *bike, ILI9341_due *tft, const tft_pos_t *const pos, const int lutTotal, const uint16_t colour)
{
	tft->drawPixel_start();
	
	for (int i = 0; i < lutTotal; i++)
		tft->drawPixel_write(pos[i].x+1, pos[i].y+1, colour);

	tft->draw_end();
}

static inline void drawRoute9 (bikegps_t *bike, ILI9341_due *tft, const tft_pos_t *const pos, const int lutTotal, const uint16_t colour)
{
	tft->drawPixel_start();
	
	for (int i = 0; i < lutTotal; i+=2)
		tft->drawPixel_write9(pos[i].x, pos[i].y, colour);

	tft->draw_end();
}

void drawRoute4 (bikegps_t *bike, ILI9341_due *tft, const tft_pos_t *const pos, const int lutTotal, const uint16_t colour)
{
	tft->drawPixel_start();
	
	for (int i = 0; i < lutTotal; i++)
		tft->drawPixel_write4(pos[i].x, pos[i].y, colour);

	tft->draw_end();
}
/*
static inline void drawRoute4x (bikegps_t *bike, ILI9341_due *tft, route_table_t *rt, const tft_pos_t *const pos, const uint16_t colour)
{
	tft->drawPixel_start();
	
	for (int i = 0; i < rt->lut.written; i++){
		tft->drawPixel_write(pos[i].x-1, pos[i].y-1, colour);
		tft->drawPixel_write(pos[i].x+1, pos[i].y-1, colour);
		tft->drawPixel_write(pos[i].x-1, pos[i].y+1, colour);
		tft->drawPixel_write(pos[i].x+1, pos[i].y+1, colour);
	}

	tft->draw_end();
}

static inline void drawRoute4p (bikegps_t *bike, ILI9341_due *tft, route_table_t *rt, const tft_pos_t *const pos, const uint16_t colour)
{
	tft->drawPixel_start();
	
	for (int i = 1; i < rt->lut.written; i++){
		tft->drawPixel_write(pos[i].x, pos[i].y-1, colour);
		tft->drawPixel_write(pos[i].x-1, pos[i].y, colour);
		
		tft->drawPixel_write(pos[i].x+1, pos[i].y, colour);
		tft->drawPixel_write(pos[i].x, pos[i].y+1, colour);

		tft->drawPixel_write(pos[i].x, pos[i].y, COLOUR_ROUTE);
		tft->drawPixel_write(pos[i-1].x, pos[i-1].y, COLOUR_ROUTE);
		//tft->drawPixel_write(pos[i+1].x, pos[i+1].y, COLOUR_ROUTE);
	}

	tft->draw_end();
}

static inline void drawRoute5 (bikegps_t *bike, ILI9341_due *tft, route_table_t *rt, const tft_pos_t *const pos, const uint16_t colour)
{
	tft->drawPixel_start();
	
	for (int i = 0; i < rt->lut.written; i++){
		tft->drawPixel_write(pos[i].x-1, pos[i].y-1, colour);
		tft->drawPixel_write(pos[i].x+1, pos[i].y-1, colour);
		tft->drawPixel_write(pos[i].x, pos[i].y, colour);
		tft->drawPixel_write(pos[i].x-1, pos[i].y+1, colour);
		tft->drawPixel_write(pos[i].x+1, pos[i].y+1, colour);
	}

	tft->draw_end();
}

static inline void drawRoute5p (bikegps_t *bike, ILI9341_due *tft, route_table_t *rt, const tft_pos_t *const pos, const uint16_t colour)
{
	tft->drawPixel_start();
	
	for (int i = 0; i < rt->lut.written; i++){
		tft->drawPixel_write(pos[i].x, pos[i].y-1, colour);
		tft->drawPixel_write(pos[i].x-1, pos[i].y, colour);
		tft->drawPixel_write(pos[i].x, pos[i].y, colour);
		tft->drawPixel_write(pos[i].x+1, pos[i].y, colour);
		tft->drawPixel_write(pos[i].x, pos[i].y+1, colour);
	}

	tft->draw_end();
}*/

static inline void locationToScreen (gps_route_t *const rc, const double longitude, const double latitude, int16_t *x, int16_t *y)
{
	*x = calcScaleLong(rc, longitude) + rc->offsetX;
	*y = calcScaleLat(rc, latitude)   - rc->offsetY;
}

static inline void screenToLocation (gps_route_t *rc, int x, int y, double *longitude, double *latitude)
{
	//x -= rc->offsetX;
	double pos = (1.0 / (double)rc->width) * x;
	pos /= rc->scaleLongZoom;
	*longitude = rc->dLong * pos;
	*longitude *= rc->scaleLongZoom;
	*longitude /= rc->scaleFactor;

	y = (rc->height - y);// - rc->offsetY;
	pos = (1.0 / (double)rc->height) * y;
	pos /= rc->scaleLatZoom;
	*latitude = rc->dLat * pos;
	*latitude *= rc->scaleLatZoom;
	*latitude /= rc->scaleFactor;

	if (rc->aspect <= rc->scaleFactor)
		*longitude /= rc->aspectFactor;
	else
		*latitude /= rc->aspectFactor;

	//*longitude += rc->minLong;
	//*latitude += rc->minLat;
}

static inline int drawRouteCalcPixels (bikegps_t *bike, gps_log_t *log, gps_route_t *rc, gps_track_t *track, tft_pos_t *pos, int *lutTotal)
{
	const int compareDepth = 16;
	
	if (track->count < compareDepth)
		return *lutTotal;// rc->lut.written;
		
	int skip = (track->count / (float)ROUTE_POINT_LENGTH);
	if (skip < 1) skip = 1;
	
	gps_datapt_t logPts[LOG_RDBUFF_LENGTH];
	int pix = 0;
	int exit = 0;
	int ptNum = 0;
	int16_t x, y;	
		

	while (pix < ROUTE_POINT_LENGTH-1 && !exit){
		const int tPts = log_read_pts(log, ptNum, logPts, LOG_RDBUFF_LENGTH);

		if (tPts <= 0)
			break;
		else if (tPts < LOG_RDBUFF_LENGTH)
			exit = 1;

		ptNum += tPts;
		gps_datapt_t *pt = logPts;

		for (int idx = 0; idx < tPts; idx+=skip, pt+=skip){
			locationToScreen(rc, pt->pos.longitude, pt->pos.latitude, &x, &y);
			//if (x < 0 || x > DWIDTH-1 || y < 0 || y > DHEIGHT-1) continue;

			int found = 0;
			if (pix > compareDepth){
				int start = pix - compareDepth;

				for (int j = start+1; !found && j < pix; j++)
					found += ((x == pos[j].x) && (y == pos[j].y));

				if (!found){
					pos[pix].x = x;
					pos[pix].y = y;
					if (++pix >= ROUTE_POINT_LENGTH)
						pix = ROUTE_POINT_LENGTH-120;
				}
			}else{
				pos[pix].x = x;
				pos[pix].y = y;
				if (++pix >= ROUTE_POINT_LENGTH)
					pix = ROUTE_POINT_LENGTH-120;
			}
		}
	}

	//rt->lut.written = pix;
	*lutTotal = pix;
	return pix;
}

static inline void displayCalcRoute (bikegps_t *bike, gps_route_t *rc, gps_track_t *track)
{
	// erase previous, but only if there is something new
	//if (bike->stats.speed >= 0.2 && bike->stats.hasFix > 1)

	tcx_updateRenderContext(bike, rc);	
	tcx_calcScaleMultipliers(rc);
	tcx_rescale(rc, 0.95);
	tcx_calcRouteCenter(rc);

	//sl.println("");	
	gps_log_t *log = &bike->map.log;
	//uint32_t t0 = micros();

	int tPts = 0;
	if (log_read_begin(log, LOG_HEADER_LENGTH)){
		tPts = drawRouteCalcPixels(bike, &bike->map.log, rc, track, lutBack(&bike->render.route), &bike->render.route.lut.written);
		log_read_end(log);

		if (tPts <= ROUTE_POINT_LENGTH)
			bike->render.route.swapSignalled = 1;
	}
	
	//uint32_t t1 = micros();
	//sl.print("tPts: "); sl.println(tPts);
	//sl.print("calc time: "); sl.println(t1-t0);
	//sl.println("");
		
}

void poiDraw (bikegps_t *bike, ILI9341_due *tft, gps_route_t *rc, const gps_poi_t *iPts)
{
	const float minLong = rc->minLong;
	const float minLat = rc->minLat;
	const float maxLong = rc->maxLong;
	const float maxLat = rc->maxLat;
	int16_t x, y;


	while (*iPts->text){
		if (iPts->loc.latitude > minLat && iPts->loc.latitude < maxLat){
			if (iPts->loc.longitude > minLong && iPts->loc.longitude < maxLong){
				locationToScreen(rc, iPts->loc.longitude, iPts->loc.latitude, &x, &y);
				tft->drawCircle(x, y, 3, COLOUR_WHITE);
				tft->drawCircle(x, y, 4, COLOUR_WHITE);
				
				if (*iPts->text != '\n')
					tft->printAt(iPts->text, x+7, y-7);
				else
					tft->printAt(iPts->text, x+7, y-14);
			}
		}
		iPts++;
	};
}


int loadMap (bikegps_t *bike, gps_map_t *map, gps_log_t *log, const char *filename)
{
	gps_track_t *track = &map->track;
	strcpy(log->filename, filename);
	
	if (log_read_begin(log, 0)){
		log_header_read(log, &track->header);
		
		track->count = log_getPositionCount(log);
		log_read_end(log);
		
		//printfn("loadMap: track->count %i", track->count);
	}else{
		//printfn("loadMap: could not open '%s'", filename);
		return 0;
	}
	
	
	gps_loc_t *loc = bike->staticmap.route.loc;
	const int locTotal = bike->staticmap.route.total;
	
	float skip = (((float)track->count / (float)locTotal));
	if (skip < 1.0f)
		skip = 1.0f;
	else if (skip > (LOG_RDBUFF_LENGTH/2.0f))
		skip = (LOG_RDBUFF_LENGTH/2.0f);

	//printfn("loadMap: skip %f", skip);

	gps_datapt_t logPt;
	//int ptNum = 0;
	int exit = 0;
	int ct = 0;
	
	if (log_read_begin(log, LOG_HEADER_LENGTH)){
		for (float i = 0.0f; i < track->count && !exit; i+=skip){
			int tPts = log_read_pts(log, (int)i, &logPt, 1);
			/*if (tPts <= 0)
				break;
			else if (tPts < 2)
				exit = 1;*/
		
			if (tPts > 0){
				loc->latitude = logPt.pos.latitude;
				loc->longitude = logPt.pos.longitude;
				loc++;
				if (++ct >= locTotal){
					ct--;
					exit = 1;
				}
			}
		}
		log_read_end(log);
	}
	
	//printfn("loadMap: ct %i", ct);
	
	bike->staticmap.route.written = ct;
	return ct;
}

static inline void drawMap (bikegps_t *bike, ILI9341_due *tft, gps_route_t *rc, const uint16_t colour)
{
	double minLong, minLat;
	double maxLong, maxLat;
	screenToLocation(rc, 2, 2, &minLong, &minLat);
	screenToLocation(rc, DWIDTH-2, DHEIGHT-2, &maxLong, &maxLat);
	
	float _minLong = rc->minLong - minLat;
	float _minLat =  rc->minLat + minLong;
	float _maxLong = rc->maxLong - maxLat;
	float _maxLat =  rc->maxLat + maxLong;
	
	minLong = _minLong;
	maxLong = _maxLong;
	minLat = _minLat;
	maxLat = _maxLat;

	/*printfn("a min: %f %f", minLong, minLat);
	printfn("b min: %f %f", rc->minLong, rc->minLat);
	printfn("a max: %f %f", maxLong, maxLat);
	printfn("b max: %f %f", rc->maxLong, rc->maxLat);	*/
	/*minLong = rc->minLong;
	minLat = rc->minLat;
	maxLong = rc->maxLong;
	maxLat = rc->maxLat;*/

	int16_t x, y;
	gps_loc_t *loc = bike->staticmap.route.loc;
	const int total = bike->staticmap.route.written;

	if (!total) return;
	
	tft->drawPixel_start();
	
	for (int i = 0; i < total; i++){
		if (loc->latitude > minLat && loc->latitude < maxLat){
			if (loc->longitude > minLong && loc->longitude < maxLong){
				locationToScreen(rc, loc->longitude, loc->latitude, &x, &y);
				
				if ((x > 1 && x < DWIDTH-3) && (y > 1 && y < DHEIGHT-3)){
					tft->drawPixel_write(x-2, y-2, COLOUR_RED);
					tft->drawPixel_write(x+3, y-2, COLOUR_RED);
					tft->drawPixel_write(x-2, y+3, COLOUR_RED);
					tft->drawPixel_write(x+3, y+3, COLOUR_RED);
					//tft->drawPixel_write4(x, y, colour);

					tft->drawPixel_write(x, y-2, COLOUR_RED);
					tft->drawPixel_write(x-2, y, COLOUR_RED);
					tft->drawPixel_write(x+3, y, COLOUR_RED);
					tft->drawPixel_write(x, y+3, COLOUR_RED);
					
					/*tft->drawPixel_write(x-1, y-1, COLOUR_RED);
					tft->drawPixel_write(x-1, y, COLOUR_RED);
					tft->drawPixel_write(x+2, y, COLOUR_RED);
					tft->drawPixel_write(x, y+2, COLOUR_RED);*/
					
					/*tft->drawPixel_write(x-1, y-1, COLOUR_RED);
					tft->drawPixel_write(x+2, y-1, COLOUR_RED);
					tft->drawPixel_write(x-1, y+2, COLOUR_RED);
					tft->drawPixel_write(x+2, y+2, COLOUR_RED);*/
					tft->drawPixel_write4(x, y, colour);
				}
			}
		}
		loc++;
	}
	
	tft->draw_end();
}

void displayRoute (bikegps_t *bike, ILI9341_due *tft, gps_route_t *rc, const uint16_t colour, const int drawStaticMap)
{
	// erase previous, but only if there is something new
	//if (bike->stats.speed >= 0.2 && bike->stats.hasFix > 1)

	if (drawStaticMap)
		drawMap(bike, tft, rc, colour);

	if (bike->render.route.swapSignalled){
		bike->render.route.swapSignalled = 0;
		drawRoute9(bike, tft, lutFront(&bike->render.route), bike->render.route.lut.written, COLOUR_MAIN_BACK);
		//drawRoute5p(bike, tft, rc, lutFront(&bike->render.route), COLOUR_MAIN_BACK);
		lutSwap(&bike->render.route);
	}
	
	//int t0 = millis();
	drawRoute9(bike, tft, lutFront(&bike->render.route), bike->render.route.lut.written, colour);
	
	//drawRoute4p(bike, tft, rc, lutFront(rc), COLOUR_ROUTE);
	drawRoute1(bike, tft, lutFront(&bike->render.route), bike->render.route.lut.written, COLOUR_WHITE);

	//int t1 = millis();
	//sl.print("draw time: "); sl.println(t1-t0);

	//if (drawStaticMap)
	//	drawMap(bike, tft, rc, colour);

}

void displayRecordingStatus (bikegps_t *bike, ILI9341_due *tft)
{
	int colour;
	if (!logIsActive(bike))
		colour = ILI9341_BLUE;
	else if (!logIsRecording(bike))
		colour = ILI9341_RED;
	else
		colour = ILI9341_GREEN;
		
	int width = 16;
	tft->fillRect(DWIDTH-3-width, DHEIGHT-2-width, width, width, colour);

}

int displayInfoTemp (bikegps_t *bike, ILI9341_due *tft, const int x, const int y)
{
	static int16_t tmp_x;
	
	tft->setCursor(x, y);
	
	tprintf("%.1f", bike->stats.temp);
	int new_x = tft->getCursorX();
	tft->println("");
	int y2 = tft->getCursorY();
	if (new_x < tmp_x)
		tft->fillRect(new_x, y, (tmp_x-new_x)+1, (y2-y)-1, COLOUR_MAIN_BACK);
	tmp_x = new_x;
	
	return y2;
}

void drawAltitude (gps_altitude_t *altitude, ILI9341_due *tft, gps_route_t *rc, const int thickness, const uint16_t colour)
{
	const int width = altitude->render.width-1;
	uint8_t	*buffer = altitude->render.buffer;
	
	
	// draw background fill
	tft->drawFill_start(1, (DHEIGHT>>1)+6, ILI9341_DARKGREEN);
	for (int i = 0; i < width; i++){
		if (buffer[i] > thickness){
			int y = buffer[i] + thickness;
			tft->drawVLine_write(i, y, DHEIGHT-y);
		}
	}
	tft->draw_end();

	
	// draw upper outline ridge
	tft->drawFill_start(1, DHEIGHT>>1, colour);
	for (int i = 1; i < width; i++){
		int dt = buffer[i] - buffer[i+1];
		if (dt > 0){
			dt += thickness;
			tft->drawVLine_write(i, buffer[i+1], dt);
		}else{
			dt = abs(dt)+thickness;
			tft->drawVLine_write(i, buffer[i], dt);
		}
	}
	tft->draw_end();
}

void drawAltitudeCalcPixels (gps_altitude_t *altitude, gps_route_t *rc)
{

	const int height = 120;
	const int width = altitude->render.width;
	uint8_t	*buffer = altitude->render.buffer;
	uint16_t *alt16 = altitude->alt16;


	uint16_t vspread = altitude->max - altitude->min;
	if (vspread < 1) vspread = 1;
	float vscale = (float)height / (float)vspread;
	const uint8_t h = height - 10;
	
	float hscale = (float)altitude->ptIdx / (float)width;
	if (hscale < 1.0f) hscale = 1.0f;

	// calclate new trace
	for (int i = 0; i < width; i++){
		float idx = hscale * (float)i;
		buffer[i] = h + (height - (uint8_t)(vscale * (float)(alt16[(uint16_t)idx] - (float)altitude->min)));
	}
}

static inline void displayPage (bikegps_t *bike, const uint32_t time)
{
	pageRender(bike, &bike->ctrl, (const uint32_t)time, 0);
}

void display (bikegps_t *bike, const uint32_t time)
{
	//uint32_t t0 = micros();
	displayPage(bike, time);

	/*uint32_t t1 = micros();
	uint32_t t = (t1 - t0);
	printfn("render time: %i", t);*/

}

static inline int gpsEncode (bikegps_t *bike, const char *buffer, const int len)
{
	return bike->gps.encode(buffer, len);
}

static inline int gpsEncode (bikegps_t *bike, const char ch)
{
	return bike->gps.encode(ch) == true;
}

static inline void collectStats (bikegps_t *bike, gps_track_t *track)
{
	//printfn("collectStats");
	
#if 0		
	if (bike->gps.location.isValid()){
		bike->gps.location.invalididate();

		getLatestPosition(bike);

		/*if (logIsRecording(bike)){
			bike->stats.distance += calcDistance(&bike->stats.pos, &bike->stats.posPrevious);
			memcpy(&bike->stats.posPrevious, &bike->stats.pos, sizeof(gps_pos_t));
		}*/
	}
#endif
	
	if (bike->gps.satellites.withFix.isValid()){
		bike->stats.satellites = bike->gps.satellites.withFix.value();
		bike->stats.hdop = bike->gps.dop.hori;
		bike->stats.course = bike->gps.course.deg();
	}else{
		bike->stats.satellites = 0;
		bike->stats.hdop = 0.0f;
	}
	
	if (bike->gps.speed.isValid()){
		if (bike->gps.speed.km > 0.0f)
			bike->stats.speedKm = bike->gps.speed.km;
		else
			bike->stats.speedKm = bike->gps.speed.kmph();

		if (bike->stats.speedKm < 1.0f)
			bike->stats.speedKm = 0.0f;
		else if (bike->stats.speedKm > 99.0f)
			bike->stats.speedKm = 99.0f;
		
		if (bike->stats.speedKm > bike->stats.maxSpeed){
			bike->stats.maxSpeed = bike->stats.speedKm;
			track->flags.speedMax_updated = 1;
		}
			
		bike->stats.aveSpeed = track_aveSpeedKm(bike, &bike->map.track);
		if (bike->stats.aveSpeed > 99.0f) bike->stats.aveSpeed = 99.0f;
	}else{
		bike->stats.speedKm = 0.0f;
	}

#if 0
	const uint16_t nemaCount = bike->gps.getSentenceCount(GPS_SENTENCE_GPGST);
	if (nemaCount != bike->render.nemaUpdateEventId){
		bike->render.nemaUpdateEventId = nemaCount;
		bike->render.updateId++;
	}
#endif


	if (logIsRecording(bike)){
		gps_track_t *track = &bike->map.track;
		if (!track->recordTimeEnd) track->recordTimeEnd = track->recordTimeStart + (track->count*1000);
		//uint32_t t = (track->recordTimeEnd - track->recordTimeStart) - track->timeIdleSum;
		time32ToTime(track_time(track), &track->duration);
	}

	if (bike->gps.date.isValid()){
		bike->stats.date.day = bike->gps.date.day();
		bike->stats.date.month = bike->gps.date.month();
		bike->stats.date.year = (uint8_t)(bike->gps.date.year()-2000);
	}

	if (bike->gps.time.isValid()){
		bike->stats.time.hour = bike->gps.time.hour();
		bike->stats.time.minute = bike->gps.time.minute();
		bike->stats.time.second = bike->gps.time.second();
	}

	bike->stats.hasFix = bike->gps.fix.getFix();
	//printfn("fix: bike->stats.hasFix %i %i %i", bike->stats.hasFix,  bike->gps.fix.hasFix, bike->gps.fix.type);
}

//void gpsModuleEventFunc ()
/*
static uint32_t maxGpsAvail = 0;
static uint32_t totalGpsAvail = 0;
static uint32_t countGpsAvail = 0;
static uint32_t lastGpsAvail = 0;*/

/*
static inline void gpsService (bikegps_t *bike)
{
	return;
	
	countGpsAvail++;
	const int in = lastGpsAvail = module.available();
	if (in > maxGpsAvail) maxGpsAvail = in;
	totalGpsAvail += in;

	const int gpsBufferLen = sizeof(bike->map.rc.logPts);
	char *gpsBuffer = (char*)bike->map.rc.logPts;

	int ct = 0;
	for (ct = 0; ct < lastGpsAvail && ct < gpsBufferLen; )
		gpsBuffer[ct++] = ublox_read();

	while(ublox_hasdata() && ct < gpsBufferLen)
		gpsBuffer[ct++] = ublox_read();
	
	gpsEncode(bike, gpsBuffer, ct);
	
}*/



static int startUpMode = 0;
static int startUpModeEventEnable = 0;

//void ISR_gps_event ()
void gpsModuleEventFunc ()
{
	if (!startUpModeEventEnable) return;
  	bikegps_t *bike = &bikegps;

	/*countGpsAvail++;
	int in = lastGpsAvail = module.available();
	if (in > maxGpsAvail) maxGpsAvail = in;
	
	totalGpsAvail += in;*/
	//bike->checkGPS++;//ublox_hasdata();
	

	do{
		gpsEncode(bike, ublox_read());
	}while(ublox_hasdata());

}

void initGPS (bikegps_t *bike, const int cfg)
{
	ublox_start(GPS_MODULE_RATE); 

	// allow module to start, as per datasheet
	delay(250);


	// set input protocols to NEMA+UBX, output to NEMA only and baud rate to 115200, no auto-baud
	//ublox_msg_send("PUBX,41,1,0007,0002,115200,0");
	//delay(10);
 	ublox_set_msg_profile(cfg);


	ublox_sbas_disable();
	//ublox_set_gnss_gps_sbas();

	// disable what we don't use
	ublox_nema_disable("DTM");
	ublox_nema_disable("GNS");
	ublox_nema_disable("GRS");
	ublox_nema_disable("ZDA");
	ublox_nema_disable("GLL");
	ublox_nema_disable("VTG");
	ublox_nema_disable("GBS");
	ublox_nema_disable("GST");
	ublox_nema_disable("GSA");
	ublox_nema_disable("GSV");
	//ublox_nema_disable("RMC");
	//ublox_nema_disable("GGA");

	// enable what we want to see
	//ublox_nema_enable("GSA");
	//ublox_nema_enable("GSV");
	ublox_nema_enable("RMC");
	ublox_nema_enable("GGA");
	ublox_pubx_00_disable();


	ublox_set_rate(GPS_RATE_IDLE);	// 1 to 10hz
	//ublox_request_hotstart();


	ublox_profile_portable();
	//ublox_profile_pedestrian();
	//ublox_profile_stationary();
	delay(5);
	
}

#if 0
static inline void initCompass (bikegps_t *bike)
{   
	Wire1.begin();  

	/*bike->compass = HMC5883L();  
	bike->compass.SetScale(1.3);
	bike->compass.SetMeasurementMode(Measurement_SingleShot);*/
	
	Compass.SetDeclination(3, 26, 'W');  
	Compass.SetSamplingMode(COMPASS_CONTINUOUS);
  	Compass.SetScale(COMPASS_SCALE_088);
	Compass.SetOrientation(COMPASS_HORIZONTAL_X_NORTH);
}
#endif

static inline void dateTime (uint16_t *date, uint16_t *time)
{
	bikegps_t *bike = &bikegps;	

	*date = FAT_DATE(bike->gps.date.year(), bike->gps.date.month(), bike->gps.date.day());
	*time = FAT_TIME(bike->gps.time.hour(), bike->gps.time.minute(), bike->gps.time.second());
}


static inline int initSD (bikegps_t *bike)
{
	SdFat *sd = &bike->map.log.sd;
	
	tft_disable();
	sd_enable();
	
	int ret = 1;

	if (!sd->begin(SD_PIN_CS, SD_SPI_SPEED)){
		//sl.println("initSD failed");
		ret = 0;
	}else{
		/*bike->map.log.cardSize = sd->card()->cardSize() * 0.512;
		if (!bike->map.log.cardSize){
			sl.println("SD cardSize failed");
			ret = 0;
		}else{
			sl.print("SD CardSize (KBytes): ");
			sl.println(bike->map.log.cardSize);
		}*/
		//sl.println("initSD done");
		
		SdFile::dateTimeCallback(dateTime);
	}
	
	sd_disable();
	tft_enable();
	return ret;
}
/*
void initSPIFlash (bikegps_t *bike)
{
	tft_disable();
	sd_disable();
	sf_enable();
 		
	//flash.begin();
	flash.initialize();
	
	
	sf_disable();
	tft_enable();
}
*/

void initTFT (bikegps_t *bike)
{
	ILI9341_due *tft = &bike->render.tft;
	
	tft->begin();
	//tft->setPowerLevel(pwrLevelIdle);
	tft->setRotation(iliRotation90);
	tft->setTextColor(COLOUR_MAIN_FRONT, COLOUR_MAIN_BACK);
	tft->setTextArea(0, 0, DWIDTH-1, DHEIGHT-1);
	tft->setCursor(0, 0);
	tft->setFont(FONT_MAIN);
	tft->setFontMode(gTextFontModeSolid);
	tft->setTextLineSpacing(1);
	
	renderClear(bike);
}

/*
void ISR_gps_pps ()
{
	bikegps.isr.gpsPPS++;
}
*/
/*void ISR_gps_en ()
{
	bikegps.isr.gpsEn = digitalRead(ISR_PIN_GPS_EN);
}*/
/*
void ISR_render_clean ()
{
	bikegps_t *bike = &bikegps;
	
	if (bike->render.displayPage != PAGE_TIME)
		renderRequestClean(bike);
}
*/

void ISR_service ()
{
	static int serviceCount;
	
	bikegps_t *bike = &bikegps;

#if (ENABLE_TEMP)
	bike_temp_t *temp = &bike->temp;
	temp->tempReadReady = 1;
#endif
	
	if (++serviceCount > 8){			// 15s * 8 = 120s
		serviceCount = 0;
		bike->render.displayReinit = 1;	
		
		int pageId = pageGetActive(&bike->ctrl);
		if (pageId != PAGE_TIME && pageId != PAGE_SPEED)
			renderRequestClean(bike);
	}
}

void ISR_render_sig ()
{
	static unsigned int callCounter;
	bikegps_t *bike = &bikegps;

	// standard 1hz update
	if (++callCounter&0x01){
		bike->counter1 = bike->counter;
		bike->counter = 0;
		bike->stats.update = 1;
		bike->render.update = 1;
		
		if (logIsRecording(bike))
			bike->map.track.update = 1;
		return;
	}

	// 2hz refresh for 'large speed' view
	if (displayGetPage(bike) == PAGE_SPEED)
		bike->render.update = 1;
}

void ISR_render_timesig ()
{
	bikegps_t *bike = &bikegps;
	
	// 25hz refresh for the time/stars page
	if (displayGetPage(bike) == PAGE_TIME)
		bike->render.update2 = 1;
}

void initISR (bikegps_t *bike)
{
	//pinMode(ISR_PIN_GPS_EN, INPUT);
	//attachInterrupt(ISR_PIN_GPS_EN, ISR_gps_en, CHANGE);
	
	//pinMode(ISR_PIN_GPS_PPS, INPUT);
	//attachInterrupt(ISR_PIN_GPS_PPS, ISR_gps_pps, RISING);

	Timer.attachInterrupt(ISR_render_timesig).start(40*1001);	
	Timer1.attachInterrupt(ISR_render_sig).start(1*500*1000);
	//Timer2.attachInterrupt(ISR_render_clean).start(120*1001*1001);
	Timer2.attachInterrupt(ISR_service).start(15*1001*1000);
	
	//Timer2.attachInterrupt(ISR_gps_event).start(5*1000);
}

uint32_t dataRecordWriteRemaining (bikegps_t *bike, gps_log_t *log, gps_track_t *track)
{
	uint32_t tWritten = 0;
	
	if (!log_write_begin(log))
		return tWritten;
	
	if (track->sp >= 1){
		tWritten = log_write_pts(log, track->log, track->sp);
		log->updated = (tWritten == track->sp);

		//sl.print("log_write tWritten: "); sl.println(tWritten);
		//sl.print("log_write updated: "); sl.println(log->updated);
	}

	int headerUpdated = 0;
	log_header_t *header = &track->header;
		
	if (track->flags.recordTimeStart_updated){
		if (track->recordTimeStart > 0){
			sprintf(header->timeStart, "%u", (unsigned int)track->recordTimeStart);
			track->flags.recordTimeStart_updated = 0;
			headerUpdated++;
		}
	}

	if (track->flags.recordTimeEnd_updated){
		if (track->recordTimeEnd > 0){
			sprintf(header->timeEnd, "%u", (unsigned int)track->recordTimeEnd);
			track->flags.recordTimeEnd_updated = 0;
			headerUpdated++;
		}
	}

	if (track->flags.totalSeconds_updated/* && track->recordTimeEnd*/){
		int32_t totalSeconds = track_timeS(track);
		if (totalSeconds > 0){
			sprintf(header->totalSeconds, "%u", (unsigned int)totalSeconds);
			//sprintf(header->timeIdle, "%i", track->timeIdleSum);
			track->flags.totalSeconds_updated = 0;
			headerUpdated++;
		}
	}

	if (track->flags.speedMax_updated){
		if (bike->stats.maxSpeed > 0){
			float speed = bike->stats.maxSpeed / 3.6f;
			if (speed > 99.9f) speed = 99.9f;
			sprintf(header->speedMax, "%.2f", speed);
			track->flags.speedMax_updated = 0;
			headerUpdated++;
		}
	}

	if (headerUpdated)
		log_header_write(log, header);

	log_write_end(log);
	track->sp = 0;
	return tWritten;
}

static inline gps_datapt_t *log_nextDataSlot (gps_track_t *track)
{
	return &track->log[track->sp++];
}

static inline void dataStoreAltitude (bikegps_t *bike, gps_altitude_t *altitude, float altFlt32)
{
	if (altFlt32 < 0.00f) altFlt32 = 0.00f;
	altFlt32 *= 10.0f;
	if (altFlt32 > 0xFFFF-1) altFlt32 = 0xFFFF-1;
	uint16_t alt16 = (uint16_t)altFlt32&0xFFFF;
	
	if (alt16 < altitude->min) altitude->min = alt16;
	if (alt16 > altitude->max) altitude->max = alt16;
	
	if (altitude->ptIdx < ALTITUDE_PTS){
		altitude->alt16[++altitude->ptIdx] = alt16;
	}else{
		memcpy(&altitude->alt16[0], &altitude->alt16[1], ((ALTITUDE_PTS-1) * sizeof(uint16_t)));
		altitude->alt16[ALTITUDE_PTS-1] = alt16;
	}
}

int dataRecordFunc (bikegps_t *bike, gps_log_t *log, gps_track_t *track)
{
	if (bike->stats.speedKm < GPS_DATAPTS_MINSPEED)
		return 0;


#if 0
	if (!getLatestPosition(bike))
		return 0;
#else
	tinygps_pos_t pos = {0.0, 0.0, 0.0f};
	bike->gps.location.cbuffSum(&pos);
	bike->stats.altitude = pos.altitude;
	bike->stats.pos.longitude = pos.longitude;
	bike->stats.pos.latitude = pos.latitude;
#endif
	
	if (bike->stats.pos.latitude != 0.0000 && bike->stats.pos.longitude != 0.0000){
		gps_datapt_t *pt = log_nextDataSlot(track);
		pt->time = bike->gps.time.UTC();
		pt->altitude = bike->stats.altitude;
		pt->pos.latitude = bike->stats.pos.latitude;
		pt->pos.longitude = bike->stats.pos.longitude;

		const double distance = calcDistance(&pt->pos, &bike->stats.posPrevious);
		if (distance > 5000.0 && track->count){	// try to remove invalid pts
			if (track->sp) track->sp--;	// undo the INC from log_nextDataSlot()
			return 1;
		}

		if (pt->pos.latitude < bike->stats.min.latitude)
			bike->stats.min.latitude = pt->pos.latitude;
		if (pt->pos.latitude > bike->stats.max.latitude)
			bike->stats.max.latitude = pt->pos.latitude;
	
		if (pt->pos.longitude < bike->stats.min.longitude)
			bike->stats.min.longitude = pt->pos.longitude;
		if (pt->pos.longitude > bike->stats.max.longitude)
			bike->stats.max.longitude = pt->pos.longitude;

		track->count++;
		if (track->count > 1){
			bike->stats.distance += distance;
			
			if (track->count&0x01){	// store every other second
				dataStoreAltitude(bike, &bike->altitude, pt->altitude);
				bike->altitude.updated = 1;
			}
		}
		
		//memcpy(&bike->stats.posPrevious, &pt->pos, sizeof(pt->pos));
		bike->stats.posPrevious = pt->pos;

		if (track->sp >= GPS_DATAPTS_LIMIT)
			dataRecordWriteRemaining(bike, log, track);

		return 1;
	}

	return 0;
}

static inline void distanceReset (bikegps_t *bike)
{
	bike->stats.distance = 0.0;
	statsPositionReset(bike);
}

static inline int resyncLog (bikegps_t *bike, gps_log_t *log)
{

	//sl.println("resynclog");

	if (!log_read_begin(log, LOG_HEADER_LENGTH))
		return 0;

	statsPositionReset(bike);
	distanceReset(bike);
	altitudeReset(bike);

	gps_datapt_t logPts[32];
	gps_datapt_t *pts = logPts;
	int ct = 0;
		
	do{
		int tPts = log_read_pts(log, ct, pts, 32);
		if (tPts < 1) break;

		//printfn("%i", tPts);
		//if (ct < 50){
			//printfn("%i: %f %f", ct, pt->pos.latitude, pt->pos.longitude);
		//}
		
		gps_datapt_t *pt = pts;

		for (int i = 0; i < tPts; i++, pt++){
			if (pt->pos.latitude != 0.0 && pt->pos.longitude != 0.0){
				if (ct > 1){
					double distance = calcDistance(&pt->pos, &bike->stats.posPrevious);
					if (distance < 5000.0)	// attempt to remove invalid points
						bike->stats.distance += distance;
					else
						continue;
				}
					
				if (pt->pos.latitude < bike->stats.min.latitude)
					bike->stats.min.latitude = pt->pos.latitude;
				if (pt->pos.latitude > bike->stats.max.latitude)
					bike->stats.max.latitude = pt->pos.latitude;
	
				if (pt->pos.longitude < bike->stats.min.longitude)
					bike->stats.min.longitude = pt->pos.longitude;
				if (pt->pos.longitude > bike->stats.max.longitude)
					bike->stats.max.longitude = pt->pos.longitude;

				//memcpy(&bike->stats.posPrevious, &pt->pos, sizeof(pt->pos));
				bike->stats.posPrevious = pt->pos;
				
				//if (i&0x01)
					dataStoreAltitude(bike, &bike->altitude, pt->altitude);
			}
		}
		
		ct += tPts;
	}while(1);

	//map->track.count = ct;

	/*printfn("ct: %i", ct);
	printfn("dis: %f", bike->stats.distance);
	printfn("min: %.6f %.6f", ct, bike->stats.min.latitude, bike->stats.min.longitude);
	printfn("max: %.6f %.6f", ct, bike->stats.max.latitude, bike->stats.max.longitude);*/
	
	log->updated = true;
	log_read_end(log);
	return ct;
}

void logResync (bikegps_t *bike, gps_log_t *log)
{
	bike->map.track.count = resyncLog(bike, log);
}

/*
static inline void dumpLog (bikegps_t *bike, gps_log_t *log, gps_route_t *rc, gps_track_t *track)
{
 
	if (!log_read_begin(log, LOG_HEADER_LENGTH))
		return;

	log_header_t header;
	if (log_header_read(log, &header))
		sl.println((char*)&header);
	
	gps_datapt_t *pt = rc->logPts;

	int ct = 0;
	do{
		int tPts = log_read_pts(log, ct, pt, 1);
		if (tPts != 1) break;
		printfn("%i: %i,%.7f,%.7f,%.2f", ct, pt->time/100, pt->pos.latitude, pt->pos.longitude, pt->altitude);
		ct++;
	}while(1);

	log_read_end(log);
	sl.flush();
}
*/

void dumpStats (bikegps_t *bike, gps_track_t *track)
{
	/*sl.println(sizeof(bikegps_t));
	sl.println(sizeof(gps_datapt_t));
	sl.println(sizeof(log_header_t));
	sl.println(sizeof(tft_pos_t));
	sl.println(sizeof(tft_lut_t));
	sl.println(sizeof(gps_track_t));	
	sl.println(sizeof(gps_route_t));
	sl.println(sizeof(gps_log_t));
	sl.println(sizeof(gps_map_t));
	sl.println(GPS_DATAPTS_LIMIT);
	gps_buttons_t *buttons = &bikegps.buttons;
	sl.println(sizeof(*buttons));*/


	printfn("counter1: %i", bike->counter1);
	
	/*gps_route_t *rc = &bike->map.rc;
	printfn("distanceLong: %.7f", rc->ruler.distanceLong);
	printfn("distanceLat: %.7f", rc->ruler.distanceLat);
	printfn("metersPerPixelH: %.7f", rc->ruler.metersPerPixelH);
	printfn("metersPerPixelV: %.7f", rc->ruler.metersPerPixelV);*/
	
	printfn("log: '%s'", bike->map.log.filename);	
	printfn("logIsActive: %i", logIsActive(bike));
	printfn("logIsRecording: %i", logIsRecording(bike));
	printfn("totalSeconds: %i", track_timeS(track));

	/*printfn("lastGpsAvail %i", lastGpsAvail);
	printfn("maxGpsAvail %i", maxGpsAvail);
	printfn("totalGpsAvail %i", totalGpsAvail);
	printfn("countGpsAvail %i", countGpsAvail);
	printfn("aveGpsAvail %i", totalGpsAvail/countGpsAvail);
	printfn("aveBpsGpsAvail %.0f", totalGpsAvail/(float)(millis()/1000.0f));*/
	
	/*printfn("charsProcessed %i", bike->gps.charsProcessed());
	printfn("sentencesWithFix %i", bike->gps.sentencesWithFix());*/
	printfn("failedChecksum %i", (int)bike->gps.failedChecksum());
	printfn("passedChecksum %i", (int)bike->gps.passedChecksum());

	
	/*tft_disable();
	sd_enable();
	sl.print("log size: "); sl.println(log_size(&bike->map.log));
	sl.print("freeSpace: "); sl.println(log_freeSpace(&bike->map.log));
	sd_disable();
	tft_enable();
	*/
	sl.println("**");
}

static inline int serialInIsReady (bikegps_t *bike)
{
	if (GPS_DATAPTS_MINSPEED > 0.0f && bike->stats.speedKm >= 5.0f)		// serial can't be available when moving
		return 0;

	return sl.available();
}

void dumpRaw (bikegps_t *bike, gps_log_t *log, gps_route_t *rc, gps_track_t *track, Serial_ *serial)
{
	//ublox_set_sleep();

	serial->flush();
	delay(10);		// ensure PC is ready to read
	while(!sl);
	
	if (logIsRecording(bike)){
		track->flags.recordTimeEnd_updated = 1;
		track->flags.totalSeconds_updated = 1;
		track->flags.speedMax_updated = 1;
		dataRecordWriteRemaining(bike, log, track);
	}
	
	
	if (log_read_begin(log, 0)){
		log_header_t header;
		if (log_header_read(log, &header)){
			serial->write((uint8_t*)&header, LOG_HEADER_LENGTH);
			serial->write("<start>\r\n");
		
			int exit = 0;
			const int batchSize = 64;
			gps_datapt_t logPts[batchSize];
			
			for (int i = 0; !exit; i+=batchSize){
				int tPts = log_read_pts(log, i, logPts, batchSize);
				if (tPts <= 0)	
					break;
				else if (tPts < batchSize)
					exit = 1;
			
				serial->write((char*)logPts, tPts*sizeof(gps_datapt_t));
			}
			serial->write("\r\n<end>\r\n");
		}
		log_read_end(log);
	}

	serial->flush();
	//sl.end();
	//ublox_request_hotstart();
	//delay(10);
}

void resetState (bikegps_t *bike, gps_track_t *track)
{
	//sl.println("resetState");
	track->update = 0;
	track->sp = 0;
	track->count = 0;
	//track->startTime = 0;
	track->recordTimeStart = 0;
	track->recordTimeEnd = 0;
	
	//inform the logger than we want this data written to datafile when first available/valid
	track->flags.recordTimeStart_updated = 0;
	track->flags.recordTimeEnd_updated = 0;
	track->flags.totalSeconds_updated = 0;
	
	//track->isRecording = 0;
	track->lastWriteSuccess = 0;
	//track->recordTime = (int32_t)millis() + (int32_t)4000;
	
	tcx_initRenderContext(bike, &bike->map.rc, bike->map.rc.width, bike->map.rc.height);
	bike->render.update = 1;
	
	memset(&bike->stats.posPrevious, 0, sizeof(gps_pos_t));
	memset(&bike->stats.pos, 0, sizeof(gps_pos_t));
	//bike->stats.distance = 0.0;
	bike->gps.location.cbuffClear();

}

void serialInDoCommand (bikegps_t *bike, const char *cmd)
{
	//printfn("serialInDoCommand #%s#", cmd);

	/*if (!strncmp(cmd, F("wifi_init"), 9)){
		wifi.println("ATE0");
		delay(10);
		wifi.println("AT+CWMODE=1");
		delay(10);
		wifi.println("AT+CIPMUX=0");
		delay(10);
		wifi.println("AT+CIPSTART=\"TCP\",\"192.168.1.2\",23");
		//delay(10);
		//wifi.println("AT+CIPMODE=1");
		
	}else if (!strncmp(cmd, F("wifi_on"), 7)){
		wifi_on();
		
	}else if (!strncmp(cmd, F("wifi_off"), 8)){
		wifi_off();

	}else if (!strncmp(cmd, F("wifi_close"), 11)){
		wifi.println("AT+CIPCLOSE");

	}else if (!strncmp(cmd, F("wifi_echoon"), 11)){
		wifi.println("ATE1");

	}else if (!strncmp(cmd, F("wifi_echooff"), 12)){
		wifi.println("ATE0");
		
	}else if (!strncmp(cmd, F("wifi_ip"), 7)){
		wifi.println("AT+CIFSR");
		
	}else if (!strncmp(cmd, "at ", 3)){
		//printfn("::#%s#", cmd);

		char buffer[32];
		sprintf(buffer, "AT+%s\r\n", &cmd[3]);
		//sl.println(buffer);
		wifi.print(buffer);

	}else if (!strncmp(cmd, F("newlog"), 6)){
		createLogNext(bike, &bike->map.log);
		resyncLog(bike, &bike->map.log, &bike->map.rc, &bike->map.track);
		bike->render.clean = 2;
		
	}else*/
	
	
	if (!strncmp(cmd, F("removelog"), 9)){
		if (logIsRecording(bike)) logEnd(bike);
		log_deleteLog(&bike->map.log);
		
	}else if (!strncmp(cmd, F("listlogs"), 8)){
		log_listLogs(&bike->map.log);

	/*}else if (!strncmp(cmd, F("resynclog"), 9)){
		resyncLog(bike, &bike->map.log, &bike->map.rc, &bike->map.track);
		bike->render.clean = 2;*/

	//}else if (!strncmp(cmd, F("dumplog"), 7)){
	//	dumpLog(bike, &bike->map.log, &bike->map.rc, &bike->map.track);
		
	}else if (!strncmp(cmd, F("freemem"), 7)){
		sl.println(SdFatUtil::FreeRam());
	
	}else if (!strncmp(cmd, F("dumpraw"), 7)){
		dumpRaw(bike, &bike->map.log, &bike->map.rc, &bike->map.track, &sl);

	/*}else if (!strncmp(cmd, F("rate1000"), 8)){
		ublox_set_rate(UBLOX_RATE_1000);
		printfn("Rate set to 1hz");*/
	
	/*}else if (!strncmp(cmd, F("rate500"), 7)){
		ublox_set_rate(UBLOX_RATE_500);
		sl.println("Rate set to 2hz");
		
	}else if (!strncmp(cmd, F("rate200"), 7)){
		ublox_set_rate(UBLOX_RATE_200);
		sl.println("Rate set to 5hz");*/
			
	/*}else if (!strncmp(cmd, F("rate100"), 7)){
		ublox_set_rate(UBLOX_RATE_100);
		printfn("Rate set to 10hz");*/
			
	/*}else if (!strncmp(cmd, F("hotstart"), 8)){
		renderRequestClean(bike);
		ublox_request_hotstart();
		sl.println("Hotstart sent");
		delay(100);*/
		
	/*}else if (!strncmp(cmd, F("sleep"), 5)){
		ublox_set_sleep();
		sl.println("GPS entered sleep mode");
	*/
	}else if (!strncmp(cmd, F("stats"), 5)){
		gps_track_t *track = &bike->map.track;

		printfn("\n**********");
		dumpStats(bike, track);
		printfn("data points: %i", (int)track->count);
		
		printfn("lut.written %i", bike->render.route.lut.written);

		printfn("log startTime: %i", (int)track->startTime);
		printfn("log recordTimeStart: %i", (int)track->recordTimeStart);
		printfn("log recordTimeEnd: %i", (int)track->recordTimeEnd);
		printfn("log timeIdleStart: %i", (int)track->timeIdleStart);
		printfn("log timeIdleEnd: %i", (int)track->timeIdleEnd);
		printfn("log timeIdleSum: %i", (int)track->timeIdleSum);
		printfn("log duration: %i:%.2i:%.2i", track->duration.hour, track->duration.minute, track->duration.second);
		printfn("log distance: %f", bike->stats.distance);
		printfn("log ave speed: %.2f", track_aveSpeedKm(bike, track));
		
		//char str[16];
		//printfn("%s", timeToString(track->startTime, str, sizeof(str)));
		printfn("**********\n");
	
	/*}else if (!strncmp(cmd, F("clear"), 5)){
		renderRequestClean(bike);
		
	}else if (!strncmp(cmd, F("reset"), 5)){
		gps_track_t *track = &bike->map.track;
		
		resetState(bike, track);
		renderRequestClean(bike);*/
	}
}

static inline int my_strlen (const uint8_t *str)
{
	int len = 0;
	while (*str++) len++;
	return len;
}

static inline void serialInProcess (bikegps_t *bike)
{
	char str[16] = {0};
	
	if (!sl.readBytesUntil('\n', str, 16))
		return;
	
	int len = my_strlen((uint8_t*)str);
	if (len > 1 && len < 16)		// check it fits the requirements of current command set
		serialInDoCommand(bike,  str);
}

int buttonUpdate (gps_buttons_t *buttons)
{
	int eventOccured = 0;
	int eventContinue = 0;
	
	if (buttons->event){
		const uint32_t t = (uint32_t)millis();
		const uint32_t dt = t - buttons->time0;

		if (dt > INPUT_BUTTON_T_DT){
			for (int i = 0; i < BUTTONS_TOTAL; i++){
				gps_button_t *btn = &buttons->btn[i];
				if (btn->id){
					int buttonReadState = digitalRead(btn->pin);

					if (buttonReadState == HIGH && btn->state == 1){		// check if released
						btn->state = 0;
						btn->time = t - btn->time;
						eventOccured = buttons->eventOccured = INPUT_MSG_BUTTON_UP;
						btn->cb(btn->id, btn->opaque, buttons->eventOccured, btn->time);
						
					}else if (buttonReadState == LOW && btn->state == 0){	// check if pressed
						btn->state = 1;
						btn->time = btn->timeHeld = t;
						eventOccured = buttons->eventOccured = INPUT_MSG_BUTTON_DOWN;
						btn->cb(btn->id, btn->opaque, buttons->eventOccured, dt);
						eventContinue = 1;
						
					}else if (buttonReadState == LOW && btn->state == 1){	// check if held down
						if (t - btn->timeHeld >= INPUT_BUTTON_T_HELD){
							btn->timeHeld = t;
							eventOccured = buttons->eventOccured = INPUT_MSG_BUTTON_HELD;
							btn->cb(btn->id, btn->opaque, buttons->eventOccured, dt);
						}
						eventContinue = 1;
					}
				}else{
					break;
				}
			}
			buttons->event = eventContinue;
		}
	}			
				
	return eventOccured;
}

void displayUpdate (bikegps_t *bike)
{
	renderRequestUpdate(bike);
}

void displayClean (bikegps_t *bike)
{
	renderRequestClean(bike);
}

static inline void tempRequest (bike_temp_t *temp)
{
#if (ENABLE_TEMP)
	temp->tempReadReady = 0;
	temp->sensor.requestTemperaturesByIndex(0);
#endif
}

static inline int tempIsReady (bike_temp_t *temp)
{
#if (ENABLE_TEMP)
	//temp->tempReadReady = 0;
	return temp->sensor.isConversionAvailable(0);
#endif
}

static inline float tempGet (bike_temp_t *temp)
{
#if (ENABLE_TEMP)
	//temp->tempReadReady = 0;
	return temp->sensor.getTempCByIndex(0);
#endif
}

static inline void tempService (bikegps_t *bike, bike_temp_t *temp)
{
#if (ENABLE_TEMP)
	//temp->tempReadReady = 0;
	//if (tempIsReady(temp)){
		bike->stats.temp = tempGet(temp);
		tempRequest(temp);
	//}
#endif
}

static inline void initTemp (bikegps_t *bike, bike_temp_t *temp)
{
#if (ENABLE_TEMP)
	temp->sensor.begin(&temp->oneWire);
	temp->sensor.setResolution(TEMP_11_BIT);
	temp->sensor.setWaitForConversion(FALSE);
	tempRequest(temp);
#endif
}


void main_loop ()
{
	bikegps_t *bike = &bikegps;
	bike->counter++;
	gps_track_t *track = &bike->map.track;


	if (track->update){		// time to log something
		//printfn("track->update");
		
		track->update = 0;
		track->lastWriteSuccess = dataRecordFunc(bike, &bike->map.log, &bike->map.track);
		if (track->lastWriteSuccess)
			track->recordTimeEnd = /*track->recordTime =*/ (uint32_t)millis();
	}

	if (bike->render.update2){
		bike->render.update2 = 0;
		//bike->counter1 = bike->counter;
		//bike->counter = 0;

  		//printfn("render.update2");

		if (bike->stats.update){
			bike->stats.update = 0;
  			collectStats(bike, track);
  			//ublox_msg_send("PUBX,00");	// request a POSITION message
  		}

		display(bike, bike->counter);
	
	}else if (bike->render.update){
		bike->render.update = 0;

		//printfn("render.update");

		if (bike->stats.update){
			bike->stats.update = 0;
   			collectStats(bike, track);
  		}
		if (bike->render.clean){
			if (--bike->render.clean < 0) bike->render.clean = 0;
			renderClear(bike);
		}

		if (displayGetPage(bike) != PAGE_TIME){
			//bike->counter1 = bike->counter;
			//bike->counter = 0;
  			display(bike, millis());
  		}
  		ublox_msg_send("PUBX,00");	// request a POSITION message
		
	}
	
	if (bike->map.log.updated){
		//printfn("log.update");
		if (displayGetPage(bike) != PAGE_TIME && displayGetPage(bike) != PAGE_FILES){
			bike->map.log.updated = false;
			displayCalcRoute(bike, &bike->map.rc, &bike->map.track);
		}
	}
	
	if (bike->map.log.logsync){
		//printfn("log.logsync");
		if (displayGetPage(bike) != PAGE_FILES){
			bike->map.log.logsync = 0;
			if (logIsActive(bike))
				logResync(bike, &bike->map.log); //&bike->map.log, &bike->map.rc, &bike->map.track);
		}
		
	}
	
	if (track->flags.writeRemaining){	
		//printfn("flags.writeRemaining");
		track->flags.writeRemaining = 0;
		dataRecordWriteRemaining(bike, &bike->map.log, &bike->map.track);
		
	}
	
	if (buttonUpdate(&bike->buttons)){
		//printfn("buttonUpdate");
		
	}else if (bike->counter&0x400){
		if (serialInIsReady(bike))
			serialInProcess(bike);
#if (ENABLE_TEMP)
		else if (bike->temp.tempReadReady)
			tempService(bike, &bike->temp);
#endif		
		
		if (bike->render.displayReinit){
			bike->render.displayReinit = 0;
			ILI9341_due *tft = &bike->render.tft;
			//tft->reinit();
			tft->setRotation(iliRotation90);
			renderClear(bike);
		}
	}
}

static inline void initPins (bikegps_t *bike) 
{
	//pinMode(WIFI_PIN_EN, OUTPUT);
	//digitalWrite(WIFI_PIN_EN, LOW);

	pinMode(BUTTON_HW_1, INPUT_PULLUP);
	pinMode(BUTTON_HW_2, INPUT_PULLUP);
	pinMode(BUTTON_HW_3, INPUT_PULLUP);
	pinMode(BUTTON_HW_4, INPUT_PULLUP);
	pinMode(BUTTON_HW_5, INPUT_PULLUP);
	pinMode(BUTTON_HW_6, INPUT_PULLUP);
	pinMode(BUTTON_HW_7, INPUT_PULLUP);
	pinMode(BUTTON_HW_8, INPUT_PULLUP);
	
		
	pinMode(FLASH_PIN_CS, OUTPUT);
	digitalWrite(FLASH_PIN_CS, HIGH);

	pinMode(SD_PIN_CS, OUTPUT);
	digitalWrite(SD_PIN_CS, HIGH);

	pinMode(TFT_PIN_CS, OUTPUT);
	digitalWrite(TFT_PIN_CS, HIGH);

	pinMode(LED_BUILTIN, OUTPUT);
	digitalWrite(LED_BUILTIN, LOW);
	
	pinMode(PIN_LED_RXL, OUTPUT);
	digitalWrite(PIN_LED_RXL, HIGH);

	pinMode(PIN_LED_TXL, OUTPUT);
	digitalWrite(PIN_LED_TXL, HIGH);

	//pinMode(DS18B20_PIN, OUTPUT);
	//digitalWrite(DS18B20_PIN, HIGH);
	
	delay(5);
}

void altitudeReset (bikegps_t *bike)
{
	bike->altitude.ptIdx = 0;
	bike->altitude.min = 65535;
	bike->altitude.max = 0;
	bike->altitude.updated = 1;
}

static inline void resetTrack (bikegps_t *bike, gps_track_t *track)
{
	memset(track, 0, sizeof(*track));
	memset(&bike->stats, 0, sizeof(bike->stats));
	//memset(&bike->stats.posPrevious, 0, sizeof(gps_pos_t));
	//memset(&bike->stats.pos, 0, sizeof(gps_pos_t));
	
	distanceReset(bike);
	altitudeReset(bike);
	resetState(bike, track);
}

static inline int createLog (bikegps_t *bike, gps_log_t *log, gps_track_t *track)
{
	log->fileIndex = log_createFilename(log, log->filename, LOG_FILENAME_MAXLENGTH);
	
	log->isOpen = log_write_begin(log);
	if (log->isOpen){
		log_header_t *header = &track->header;
		memset(header, ' ', LOG_HEADER_LENGTH);
				
		strcpy(header->filename, log->filename);
		sprintf(header->date, "%.4i-%.2i-%.2i", track->year, track->month, track->day);
		timeToString(track->startTime, header->time, sizeof(header->time));	// should be 'file creation time'
		sprintf(header->timeStart, "%u", ((unsigned int)track->recordTimeStart));
		sprintf(header->timeEnd, "0");
		sprintf(header->totalSeconds, "0");

		/*sl.println("log created:");
		sl.println("date:" + String(header->date));
		sl.println("startTime:" + String(track->startTime) + String(" / ") + String(header->time));
		sl.println("recordTimeStart:" + String(header->timeStart));*/
		
		log_header_write(log, header);
		log_write_end(log);
		
		//sl.println("log file created");
		return 1;
	}else{
		//sl.println("could not create log file");
		return 0;
	}
}

static inline int createLogNext (bikegps_t *bike, gps_log_t *log)
{
	resetState(bike, &bike->map.track);
	return createLog(bike, log, &bike->map.track);
}

static inline void initLogger (bikegps_t *bike) 
{
	
	bike->render.route.lut.front = bike->render.route.lut.table[0];
	bike->render.route.lut.back = bike->render.route.lut.table[1];

	resetState(bike, &bike->map.track);	
	//createLog(bike, &bike->map.log);
}

int logIsActive (bikegps_t *bike)
{
	gps_track_t *track = &bike->map.track;
	
	return track->isRecording != -1;
}

int logIsRecording (bikegps_t *bike)
{
	gps_track_t *track = &bike->map.track;
	
	return track->isRecording == 1;
}

void logStop (bikegps_t *bike)
{
	//sl.println("logStop");
	
	gps_track_t *track = &bike->map.track;
	
	track->isRecording = 0;
	track->recordTimeEnd =/* track->recordTime =*/ millis();
	track->timeIdleStart = track->recordTimeEnd;
	track->flags.recordTimeEnd_updated = 1;
	track->flags.totalSeconds_updated = 1;
	track->flags.writeRemaining = 1;
	
	ublox_set_rate(GPS_RATE_IDLE);

}

void logContinue (bikegps_t *bike)
{
	//sl.println("logContinue");
	gps_track_t *track = &bike->map.track;
		
	track->isRecording = 1;
	if (track->timeIdleStart){
		track->timeIdleEnd = track->recordTimeEnd = millis();
		track->timeIdleSum += (track->timeIdleEnd - track->timeIdleStart);
		track->timeIdleStart = 0;
	}
	
	ublox_set_rate(GPS_RATE_RECORDING);
}

int logStartNew (bikegps_t *bike)
{
	//sl.println("logStartNew");
	
	gps_track_t *track = &bike->map.track;
	
	if (logIsRecording(bike))
		logStop(bike);

	resetTrack(bike, track);
	//resetState(bike, track);

	track->startTime = bike->gps.time.UTC();
	track->year = bike->gps.date.year();
	track->month = bike->gps.date.month();
	track->day = bike->gps.date.day();
	time32ToTime(0, &track->duration);

	track->timeIdleStart = 0;
	track->timeIdleEnd = 0;
	track->timeIdleSum = 0;
	
	if (createLogNext(bike, &bike->map.log)){
		track->recordTimeStart = millis();
		track->flags.recordTimeStart_updated = 1;
		logContinue(bike);
		return 1;
	}
	
	return 0;
}

void logEnd (bikegps_t *bike)
{
	gps_track_t *track = &bike->map.track;

	if (logIsRecording(bike))
		logStop(bike);
	
	track->isRecording = -1;
	//resetState(bike, track);
}

int logLastWriteStatus (bikegps_t *bike)
{
	gps_track_t *track = &bike->map.track;

	return track->lastWriteSuccess;
}

int logLoad (bikegps_t *bike, gps_log_t *log, gps_track_t *track, const int index, const char *filename)
{
	int ret = 0;
	
	if (index == -1){	// load by filename, any filename
		if (log_read_open(log, filename)){
			resetTrack(bike, track);
			log_header_read(log, &track->header);
			
			int count = log_getPositionCount(log);
			log_read_end(log);

			if (count > 0)
				ret = track->count = count;
			else
				return 0;

			track->startTime = atoi(track->header.time);
			track->recordTimeStart = atoi(track->header.timeStart);
			track->recordTimeEnd = atoi(track->header.timeEnd);
		
			if (!track->recordTimeEnd) track->recordTimeEnd = track->recordTimeStart + (track->count * 1000);
			uint32_t t = (track->recordTimeEnd - track->recordTimeStart);// - track->timeIdleSum;
			time32ToTime(t, &track->duration);
		
			int sum = (int)(atoi(track->header.totalSeconds)*1000) - (int)t;
			if (sum < 0) sum = 0;
			track->timeIdleSum = sum;
			
			memset(&bike->render.route.lut, 0, sizeof(tft_lut_t));
			lutSwap(&bike->render.route);
			log->logsync = 1;
		}
	}else{				// load file by index, as found on SD. index of 0 = 000000.dat, 1 = 000001.dat, etc..
		
	}
	
	return ret;
}

void logImportPrevious (bikegps_t *bike)
{
	gps_log_t *log = &bike->map.log;
	char filename[LOG_FILENAME_MAXLENGTH];
	int idx = 0;

	if (log->fileIndex < 0)
		log->fileIndex = log_createFilenameLast(log, filename, LOG_FILENAME_MAXLENGTH);

	if ((idx=log_createFilenamePrev(log, filename, LOG_FILENAME_MAXLENGTH, log->fileIndex)) != 0){
		//printfn("%s", filename);
			
	 	log->fileIndex = idx;
		logLoad(bike, log, &bike->map.track, -1, filename);
		renderRequestClean(bike);
		renderRequestUpdate(bike);
			
	 }else if ((idx=log_createFilenameLast(log, filename, LOG_FILENAME_MAXLENGTH)) != 0){
		//printfn("%s", filename);

	 	log->fileIndex = idx;
		logLoad(bike, log, &bike->map.track, -1, filename);
		renderRequestClean(bike);
		renderRequestUpdate(bike);
	 }
}

void logImportNext (bikegps_t *bike)
{
	gps_log_t *log = &bike->map.log;
	char filename[LOG_FILENAME_MAXLENGTH];
	int idx = 0;
		
	if (log->fileIndex < 0)
		log->fileIndex = 0;

	if ((idx=log_createFilenameNext(log, filename, LOG_FILENAME_MAXLENGTH, log->fileIndex)) != 0){
		//printfn("%s", filename);
			
	 	log->fileIndex = idx;
		logLoad(bike, log, &bike->map.track, -1, filename);
		renderRequestClean(bike);
		renderRequestUpdate(bike);
	 }
}

void buttonCb_PAGE_NEXT (const int id, void *opaque, const int state, const int dt)
{
	bikegps_t *bike = (bikegps_t*)opaque;
	
	//printfn("buttonCb_PAGE_NEXT %i %i %i", id, state, dt);
	
	if (state != INPUT_MSG_BUTTON_UP)
		return;
		
	buttonsCancel(&bikegps.buttons);
	page_input_t ctx;
	ctx.button = id;
	ctx.state = state;
	ctx.dt = dt;

	if (pageInput(bike, &bike->ctrl, &ctx) == PAGE_RET_INPUT_CONTINUE)
		displaySetPage(bike, pageGetNext(&bike->ctrl), 1);

	renderRequestUpdate(bike);
}

void buttonCb_234 (const int id, void *opaque, const int state, const int dt)
{
	bikegps_t *bike = (bikegps_t*)opaque;
	
	//printfn("buttonCb_234 %i %i %i", id, state, dt);
	
	//if (state == INPUT_MSG_BUTTON_DOWN)
	//	return;

	page_input_t ctx;
	ctx.button = id;
	ctx.state = state;
	ctx.dt = dt;
	
	if (pageInput(bike, &bike->ctrl, &ctx) == PAGE_RET_INPUT_HALT)
		renderRequestUpdate(bike);
}

void buttonCb_unused (const int id, void *opaque, const int state, const int dt)
{
	//printfn("buttonCb_unused %i %i %i", id, state, dt);
}

static inline void initButtons (bikegps_t *bike, gps_buttons_t *buttons)
{
	buttonAdd(buttons, BUTTON_HW_6, buttonCb_PAGE_NEXT, BUTTON_ID_1, (void*)bike);	
	buttonAdd(buttons, BUTTON_HW_8, buttonCb_234, BUTTON_ID_2, (void*)bike);
	buttonAdd(buttons, BUTTON_HW_2, buttonCb_234, BUTTON_ID_3, (void*)bike);
	buttonAdd(buttons, BUTTON_HW_4, buttonCb_234, BUTTON_ID_4, (void*)bike);
	
	buttonAdd(buttons, BUTTON_HW_5, buttonCb_unused, BUTTON_ID_5, (void*)bike);
	buttonAdd(buttons, BUTTON_HW_7, buttonCb_unused, BUTTON_ID_6, (void*)bike);
	buttonAdd(buttons, BUTTON_HW_1, buttonCb_unused, BUTTON_ID_7, (void*)bike);
	buttonAdd(buttons, BUTTON_HW_3, buttonCb_unused, BUTTON_ID_8, (void*)bike);
}

static inline int buttonIsPressed (const int button)
{
	return !digitalRead(button);
}

static inline int buttonIsReleased (const int button)
{
	return digitalRead(button);
}

// startup in a debug mode if requested
// button 4 exits
static inline int doStartModeSelect (bikegps_t *bike)
{

	// connect SerialUSB directly to GPS, disable application, with default application GPS config
	// GPS config: IN:UBX+NEMA, OUT:NEMA only, 115200
	// intended for use with U-Center
	if (buttonIsPressed(BUTTON_HW_1)){
		sl.begin(SERIAL_RATE);
		initGPS(bike, 1);
		ublox_nema_enable("GSV");
		
		while (buttonIsReleased(BUTTON_HW_4)){
			if (gpsModule.available())
				sl.write(gpsModule.read());
		
			if (sl.available())
				gpsModule.write(sl.read());
		}
		ublox_set_msg_profile(1);
		ublox_nema_enable("RMC");
		ublox_nema_enable("GGA");
		ublox_pubx_00_disable();
		return 1;
		
	// connect SerialUSB directly to GPS, disable application, with 'everything enabled' GPS config
	// GPS config: IN:UBX+NEMA, OUT:UBX+NEMA, 115200
	}else if (buttonIsPressed(BUTTON_HW_2)){
		sl.begin(SERIAL_RATE);
		initGPS(bike, 2);
		/*ublox_nema_enable("DTM");
		ublox_nema_enable("GNS");
		ublox_nema_enable("GRS");
		ublox_nema_enable("ZDA");
		ublox_nema_enable("GLL");
		ublox_nema_enable("VTG");
		ublox_nema_enable("GBS");
		ublox_nema_enable("GST");
		ublox_nema_enable("GSA");
		ublox_nema_enable("GSV");*/

		ublox_set_nav_status();
		ublox_set_nav_dop();
		ublox_set_nav_pvt();
		ublox_set_nav_svinfo();

		while (buttonIsReleased(BUTTON_HW_4)){
			if (gpsModule.available())
				sl.write(gpsModule.read());
		
			if (sl.available())
				gpsModule.write(sl.read());
		}
		
		ublox_set_msg_profile(1);
		ublox_nema_enable("RMC");
		ublox_nema_enable("GGA");
		ublox_pubx_00_disable();
		return 1;
		
	}else if (buttonIsPressed(BUTTON_HW_3)){
		GPS_DATAPTS_MINSPEED = GPS_MINSPEED_WALKING;
		return 0;	//allow normal startup
	
	// used when recovering from a 'Revert to default configuration' before applying ublox GNSS config
	}else if (buttonIsPressed(BUTTON_HW_4)){
		ublox_start(9600);
		delay(250);
		ublox_set_baud(UBLOX_BAUD_115200);
		ublox_set_msg_profile(2);
		ublox_stop();
		return 0;	//allow normal startup
	}
	return 0;
}

void main_setup ()
{

#if 0
	//pmc_mck_set_prescaler(48); 

	pmc_disable_periph_clk(2);   // unused clocks
	pmc_disable_periph_clk(4);   // watchdog timer
	
	pmc_disable_periph_clk(9);   // SMC_SDRAMC
	pmc_disable_periph_clk(10);  // SDRAMC
	
	pmc_disable_periph_clk(15);  // PIO E 
	pmc_disable_periph_clk(16);  // PIO F 
	
	pmc_disable_periph_clk(18);  // USART1
	pmc_disable_periph_clk(19);  // USART2
	
	pmc_disable_periph_clk(21);  // HSMCI (SD/MMC ctrl, N/C)
	pmc_disable_periph_clk(22);  // TWI/I2C bus 0 (i.MX6 controlling)
	pmc_disable_periph_clk(23);  // TWI/I2C bus 1
	//pmc_disable_periph_clk(24);  // SPI0
	pmc_disable_periph_clk(25);  // SPI1
	pmc_disable_periph_clk(26);  // SSC (I2S digital audio, N/C)

	pmc_disable_periph_clk(38);  // DAC ctrl
	//pmc_disable_periph_clk(40);  // USB OTG high-speed ctrl
	//pmc_disable_periph_clk(41);  // random number generator
	pmc_disable_periph_clk(42);  // ethernet MAC - N/C
	pmc_disable_periph_clk(43);  // CAN controller 0
	pmc_disable_periph_clk(44);  // CAN controller 1
  	pmc_disable_periph_clk(31);  // timer/counter 4
	pmc_disable_periph_clk(32);  // timer/counter 5
	pmc_disable_periph_clk(33);  // timer/counter 6
	pmc_disable_periph_clk(34);  // timer/counter 7
	pmc_disable_periph_clk(35);  // timer/counter 8
	pmc_disable_periph_clk(36);  // PWM
	//SystemCoreClockUpdate();
#endif


	bikegps_t *bike = &bikegps;
	
	initPins(bike);

	startUpModeEventEnable = 0;
	startUpMode = doStartModeSelect(bike);
	startUpModeEventEnable = 1;
	
	//SerialFlash.begin(FlashChipSelect);
	//SerialFlash.sleep();
	initTemp(bike, &bike->temp);
		
	initTFT(bike);
	//initCompass(bike);
	initSD(bike);
	if (!startUpMode) initGPS(bike, 1);
	//initSPIFlash(bike);
	initLogger(bike);
	initButtons(bike, &bike->buttons);

	//wifi.begin(WIFI_RATE);
	pagesInit(bike, &bike->ctrl, PAGE_DEFAULT);
	pagesStart(bike, &bike->ctrl);
	
	statsPositionReset(bike);
	distanceReset(bike);
	altitudeReset(bike);

	gps_track_t *track = &bike->map.track;
	track->startTime = 0;
	//track->recordInterval = BIKE_SAMPLE_PERIOD;
	//track->recordTime = 4000;
	track->isRecording = -1;
	track->sp = 0;

	bike->map.log.fileIndex = -1;
	bike->stats.update = 1;
		
	tcx_initRenderContext(bike, &bike->map.rc, DWIDTH, DHEIGHT);
	
	initISR(bike);

	if (!startUpMode) sl.begin(SERIAL_RATE);
	
	displaySetPage(bike, PAGE_DEFAULT, 0);
}


