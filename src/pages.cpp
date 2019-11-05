
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


#pragma GCC optimize ("-O2")


#include "bike.h"
#include "pages.h"
#include "ublox.h"

static gfx_stars_t g_stars;


static inline void starsInit (gfx_stars_t *stars)
{
 	randomSeed(analogRead(0));
 	
    for (int i = 0; i < stars->total; i++){
        stars->star[i].x = random(0, DWIDTH-1);
        stars->star[i].y = random(0, DHEIGHT-1);
        stars->star[i].plane = random(STARS_PLANES*STARS_PLANES) % STARS_PLANES;
    }
}

static inline void starsDraw (gfx_stars_t *stars, ILI9341_due *tft)
{
	tft->drawPixel_start();
	
	for (int i = 0; i < stars->total; i++){
		tft->drawPixel_write(stars->star[i].x, stars->star[i].y, COLOUR_BLACK);
		
		stars->star[i].x += stars->velocity[stars->star[i].plane];
		if (stars->star[i].x > DWIDTH-1){
			stars->star[i].x = 0;
			stars->star[i].y = random(0, DHEIGHT-1);
		}
		tft->drawPixel_write(stars->star[i].x, stars->star[i].y, stars->colour[stars->star[i].plane]);
	}

	tft->draw_end();
}

static inline void time_start (bikegps_t *bike, ILI9341_due *tft)
{
	starsInit(&g_stars);
}

static inline void time_enter (bikegps_t *bike, ILI9341_due *tft)
{
	//printfn("time_enter");

	//tft->fillScreen(COLOUR_BLACK);
	tft->setTextColor(COLOUR_WHITE/*ILI9341_PEACHPUFF*/, COLOUR_BLACK);
	tft->setFont(FONT_DATETIME_TIME);
	tft->setTextLetterSpacing(FONT_DATETIME_SPACING);
}

static inline void time_leave (bikegps_t *bike, ILI9341_due *tft)
{
	tft->setTextColor(COLOUR_MAIN_FRONT, COLOUR_MAIN_BACK);
}

//static int scrollOffset = DWIDTH-1;
//static int drawImg = 1;


static inline void time_render (bikegps_t *bike, ILI9341_due *tft, uint32_t counter)
{
	//printfn("time_render");

	//tft->setScroll(scrollOffset);
	//if (--scrollOffset < 0) scrollOffset = DWIDTH-1;


	/*if (drawImg){
		tft->drawImage(picture, 32, 33, 256, 174);
		return;
	}*/

	if (counter&0x01){
		tft->setCursor(9, (DHEIGHT>>1)-78);
		tprintf("%02i:%.2i:%.2i", bike->stats.time.hour, bike->stats.time.minute, bike->stats.time.second);
	}

	if (!bike->stats.time.second || !bike->stats.date.year || displayGetPagePre(bike) != displayGetPage(bike)){
		tft->setCursor(23, (DHEIGHT>>1)+48);
		tprintf("%02i/%i/%.2i", bike->stats.date.day, bike->stats.date.month, bike->stats.date.year);
	}
	
	if (bike->stats.date.year)	
		starsDraw(&g_stars, tft);
}

static inline int page_time (bikegps_t *bike, void *ctx, const uint32_t msg, int32_t v1, int32_t v2, void *opaque)
{
	switch (msg){
	case PAGE_MSG_RENDER:
		time_render(bike, (ILI9341_due*)ctx, (uint32_t)v1);
		break;
		
	case PAGE_MSG_INPUT:
		return PAGE_RET_INPUT_CONTINUE;
		
	case PAGE_MSG_ENTER:
		time_enter(bike, (ILI9341_due*)ctx);
		break;

	case PAGE_MSG_LEAVE:
		time_leave(bike, (ILI9341_due*)ctx);
		break;
	case PAGE_MSG_START:
		time_start(bike, (ILI9341_due*)ctx);
		break;
	case PAGE_MSG_SHUTDOWN:
		break;
	}
	return 0;
}

/*
##################################################################################################################
##################################################################################################################
##################################################################################################################
##################################################################################################################
*/

// idx 0 = start, 1 = stop
static uint16_t keyState[2];


static inline void overview_enter (bikegps_t *bike, ILI9341_due *tft)
{
	//tft->setColour(COLOUR_MAIN_FRONT);
	tft->setFont(FONT_DATETIME_TIME);
	tft->setTextLetterSpacing(FONT_DATETIME_SPACING);
}

static inline void overview_render (bikegps_t *bike, ILI9341_due *tft, const uint32_t counter, gps_track_t *track)
{
	//tft->setFont(FONT_KARISMA105);
	const int y = 2;
	// duration
	displayInfoDuration(bike, tft, 4, y);

	// speed	
	tft->setColour(COLOUR_SPEED);
	displayInfoSpeed(bike, tft, 4, y+65);
	
	// altitude
	tft->setColour(COLOUR_ALTITUDE);
	char str[8];
	snprintf(str, sizeof(str), "%.0f", bike->stats.altitude);
	tft->printAlignedOffseted(str, gTextAlignTopRight, -2, y+65);
	
	// distance
	tft->setCursor(4, y+129);
	tft->setColour(COLOUR_MAIN_FRONT);
	const float distance = bike->stats.distance/1000.0f;
	tprintf("%.2f", distance);
	
	// time of day
	//tft->setFont(FONT_DATETIME_TIME);
	tft->setCursor(4, y+193);
	tprintf("%02i:%.2i:%.2i", bike->stats.time.hour, bike->stats.time.minute, bike->stats.time.second);
}

static inline int overview_input (bikegps_t *bike, page_input_t *ctx, gps_track_t *track)
{
	//if (ctx->state != INPUT_MSG_BUTTON_UP)
	//	return PAGE_RET_INPUT_CONTINUE;

	if (ctx->button == BUTTON_RECORD_START){
		if (ctx->state == INPUT_MSG_BUTTON_HELD){
			if (!keyState[0] && ctx->dt >= 1000){
				keyState[0] = 1;
				
				if (!logIsActive(bike)){
					logStartNew(bike);
					displayClean(bike);
				}
			}
		}else if (ctx->state == INPUT_MSG_BUTTON_RELEASED){
			keyState[0] = 0;
		}
		return PAGE_RET_INPUT_HALT;

	}else if (ctx->button == BUTTON_RECORD_STOP){
		if (ctx->state == INPUT_MSG_BUTTON_HELD){
			if (!keyState[1] && ctx->dt >= 1000){
				keyState[1] = 1;
				
				logEnd(bike);
				bike->map.log.updated = true;
			}
		}else if (ctx->state == INPUT_MSG_BUTTON_RELEASED){
			keyState[0] = 0;
		}
		return PAGE_RET_INPUT_HALT;
	}
	return PAGE_RET_INPUT_CONTINUE;	
}

static inline int page_overview (bikegps_t *bike, void *ctx, const uint32_t msg, int32_t v1, int32_t v2, void *opaque)
{
	switch (msg){
	case PAGE_MSG_RENDER:
		overview_render(bike, (ILI9341_due*)ctx, (uint32_t)v1, (gps_track_t*)opaque);
		break;
	case PAGE_MSG_INPUT:
		return overview_input(bike, (page_input_t*)ctx, (gps_track_t*)opaque);
		
	case PAGE_MSG_ENTER:
		overview_enter(bike, (ILI9341_due*)ctx);
		break;
	case PAGE_MSG_LEAVE:
		break;
	case PAGE_MSG_START:
		break;
	case PAGE_MSG_SHUTDOWN:
		break;
	}
	return 0;
}

/*
##################################################################################################################
##################################################################################################################
##################################################################################################################
##################################################################################################################
*/


static inline void speed_enter (bikegps_t *bike, ILI9341_due *tft)
{
	keyState[0] = 0;
	keyState[1] = 0;
		
	tft->setColour(COLOUR_MAIN_FRONT);
	//tft->setFont(FONT_DATETIME_TIME);
	//tft->setTextLetterSpacing(FONT_DATETIME_SPACING);
}

static inline void speed_render (bikegps_t *bike, ILI9341_due *tft, const uint32_t counter, gps_track_t *track)
{

	//tft->setColour(COLOUR_MAIN_FRONT);
	tft->setFont(FONT_SPEED_LARGE);
	tft->setTextLetterSpacing(8);
	displayInfoSpeedAlt(bike, tft, 2, 4);
	
	tft->setFont(FONT_KARISMA105);
	tft->setTextLetterSpacing(FONT_KARISMA105_SPACING);
	displayInfoAveSpeed(bike, tft, 0, DHEIGHT-65);
	
	//tft->setColour(COLOUR_DISTANCE);
	char str[8];
	const float distance = bike->stats.distance/1000.0f;
	snprintf(str, sizeof(str), "%.*f", (distance < 100.0f), distance);
	tft->printAlignedOffseted(str, gTextAlignBottomRight, 0, -1);
}

static inline int speed_input (bikegps_t *bike, page_input_t *ctx, gps_track_t *track)
{
	//if (ctx->state != INPUT_MSG_BUTTON_UP)
	//	return PAGE_RET_INPUT_CONTINUE;


	if (ctx->button == BUTTON_RECORD_START){
		if (ctx->state == INPUT_MSG_BUTTON_HELD){
			if (!keyState[0] && ctx->dt >= 1000){
				keyState[0] = 1;
				
				if (!logIsActive(bike)){
					logStartNew(bike);
					displayClean(bike);
				}
			}
		}else if (ctx->state == INPUT_MSG_BUTTON_RELEASED){
			keyState[0] = 0;
		}
		return PAGE_RET_INPUT_HALT;

	}else if (ctx->button == BUTTON_RECORD_STOP){
		if (ctx->state == INPUT_MSG_BUTTON_HELD){
			if (!keyState[1] && ctx->dt >= 1000){
				keyState[1] = 1;
				
				logEnd(bike);
				bike->map.log.updated = true;
			}
		}else if (ctx->state == INPUT_MSG_BUTTON_RELEASED){
			keyState[0] = 0;
		}
		return PAGE_RET_INPUT_HALT;

	}else if (ctx->button == BUTTON_PAGE_SWAP){
		if (ctx->state == INPUT_MSG_BUTTON_UP)
			displaySetPage(bike, PAGE_ACTIVITY, 1);
		return PAGE_RET_INPUT_HALT;
	}
	return PAGE_RET_INPUT_CONTINUE;	
}


static inline int page_speed (bikegps_t *bike, void *ctx, const uint32_t msg, int32_t v1, int32_t v2, void *opaque)
{
	switch (msg){
	case PAGE_MSG_RENDER:
		speed_render(bike, (ILI9341_due*)ctx, (uint32_t)v1, (gps_track_t*)opaque);
		break;
	case PAGE_MSG_INPUT:
		return speed_input(bike, (page_input_t*)ctx, (gps_track_t*)opaque);
		
	case PAGE_MSG_ENTER:
		speed_enter(bike, (ILI9341_due*)ctx);
		break;

	case PAGE_MSG_LEAVE:
		break;
	case PAGE_MSG_START:
		break;
	case PAGE_MSG_SHUTDOWN:
		break;
	}
	return 0;
}
/*
##################################################################################################################
##################################################################################################################
##################################################################################################################
##################################################################################################################
*/


static inline void activity_render (bikegps_t *bike, ILI9341_due *tft, const uint32_t time, gps_track_t *track)
{
	tft->setFont(FONT_DURATION);
	tft->setTextLetterSpacing(FONT_DURATION_SPACING);
	
	int x = 1, y = 0;
	y = displayInfoAltitude(bike, tft, x, y, 1);
	y = displayInfoDuration(bike, tft, x, y+8);
   	displayInfoDistance(bike, tft, x, y+8);

   	displayInfoTime(bike, tft, (DWIDTH>>1)-4, 1);
   	displayInfoAveSpeed(bike, tft, DWIDTH-108, DHEIGHT-32);
	//displayInfoTemp(bike, tft, DWIDTH-85, DHEIGHT-68);
	
	drawRoute4(bike, tft, lutFront(&bike->render.route), bike->render.route.lut.written, COLOUR_ROUTE);
	//drawRoute1(bike, tft, &bike->map.rc, lutFront(&bike->map.rc), COLOUR_ROUTE);
	//drawRoute5p(bike, tft, &bike->map.rc, lutFront(&bike->map.rc), COLOUR_ROUTE);
	
	tft->setFont(FONT_SPEED);
	tft->setTextLetterSpacing(FONT_SPEED_SPACING);
	displayInfoSpeed(bike, tft, 2, 143);
	
	displayCourse(bike, tft, DWIDTH-36, 60, 25, 50.0f, bike->stats.course);
	displayRoute(bike, tft, &bike->map.rc, COLOUR_ROUTE, 0);
	displayRecordingStatus(bike, tft);
}

static inline int activity_input (bikegps_t *bike, page_input_t *ctx, gps_track_t *track)
{
	//if (ctx->state != INPUT_MSG_BUTTON_UP)
	//	return PAGE_RET_INPUT_CONTINUE;


	if (ctx->button == BUTTON_RECORD_START){
		if (ctx->state == INPUT_MSG_BUTTON_HELD){
			if (!keyState[0] && ctx->dt >= 1000){
				keyState[0] = 1;
				
				if (!logIsActive(bike)){
					logStartNew(bike);
					displayClean(bike);
				}
			}
		}else if (ctx->state == INPUT_MSG_BUTTON_RELEASED){
			keyState[0] = 0;
		}
		return PAGE_RET_INPUT_HALT;

	}else if (ctx->button == BUTTON_RECORD_STOP){
		if (ctx->state == INPUT_MSG_BUTTON_HELD){
			if (!keyState[1] && ctx->dt >= 1000){
				keyState[1] = 1;
				
				logEnd(bike);
				bike->map.log.updated = true;
			}
		}else if (ctx->state == INPUT_MSG_BUTTON_RELEASED){
			keyState[0] = 0;
		}
		return PAGE_RET_INPUT_HALT;

	}else if (ctx->button == BUTTON_PAGE_SWAP){
		if (ctx->state == INPUT_MSG_BUTTON_UP)
			displaySetPage(bike, PAGE_SPEED, 1);

		return PAGE_RET_INPUT_HALT;
	}
	return PAGE_RET_INPUT_CONTINUE;	
}

static inline int page_activity (bikegps_t *bike, void *ctx, const uint32_t msg, int32_t v1, int32_t v2, void *opaque)
{
	switch (msg){
	case PAGE_MSG_RENDER:
		activity_render(bike, (ILI9341_due*)ctx, (uint32_t)v1, (gps_track_t*)opaque);
		break;
	case PAGE_MSG_INPUT:
		return activity_input(bike, (page_input_t*)ctx, (gps_track_t*)opaque);
		
	case PAGE_MSG_ENTER:
		keyState[0] = 0;
		keyState[1] = 0;
		break;
	case PAGE_MSG_LEAVE:
		break;
	case PAGE_MSG_START:
		break;
	case PAGE_MSG_SHUTDOWN:
		break;
	}
	
	return 0;
}

/*
##################################################################################################################
##################################################################################################################
##################################################################################################################
##################################################################################################################
*/

static int routeEnter = 0;

static inline void route_start (bikegps_t *bike, ILI9341_due *tft)
{
	altitudeReset(bike);
}

static inline void route_enter (bikegps_t *bike, ILI9341_due *tft)
{
	tft->setFont(FONT_SMALL);
	tft->setTextLetterSpacing(FONT_SMALL_SPACING);
	
	routeEnter = 1;	
}

static inline void route_render (bikegps_t *bike, ILI9341_due *tft, const uint32_t time, gps_route_t *rc)
{
	gps_altitude_t *altitude = &bike->altitude;	
	const int altThickness = 4;

	
	if (altitude->updated){
		altitude->updated = 0;
		
		// erase previous trace
		if (!routeEnter && altitude->ptIdx >= altitude->render.width-1){
			drawAltitude(altitude, tft, rc, altThickness, COLOUR_MAIN_BACK);
		}else{
			routeEnter = 0;
		}

		drawAltitudeCalcPixels(altitude, rc);
	}

	drawAltitude(altitude, tft, rc, altThickness, COLOUR_ALTITUDE);

	tft->setCursor(1, 1);
	tprintfn("dH: %.0f", rc->ruler.distanceLong);
	tprintfn("dV: %.0f", rc->ruler.distanceLat);
	tprintfn("mpH: %.1f", rc->ruler.metersPerPixelH);
	tprintfn("mpV: %.1f", rc->ruler.metersPerPixelV);

	if (altitude->max)
		tprintfae(-4, 1, gTextAlignTopRight, "%.1f / %.1f", altitude->min/10.0f, altitude->max/10.0f);
	

	displayRoute(bike, tft, rc, COLOUR_ROUTE, 0);

	if (bike->render.route.lut.written > 16){
		tft_pos_t *pos = lutFront(&bike->render.route);
		
		// start
		int16_t cx = pos[0].x;
		int16_t cy = pos[0].y;
		tft->fillRect(cx-4, cy-4, 9, 9, COLOUR_BLUE);

		// end
		cx = pos[bike->render.route.lut.written-1].x;
		cy = pos[bike->render.route.lut.written-1].y;
		tft->fillCircle(cx, cy, 4, COLOUR_RED);
	}
}

static inline int route_input (bikegps_t *bike, page_input_t *ctx, gps_route_t *route)
{
	if (ctx->state != INPUT_MSG_BUTTON_UP)
		return PAGE_RET_INPUT_CONTINUE;

	if (ctx->button == BUTTON_ID_2){
		logImportPrevious(bike);
		return PAGE_RET_INPUT_HALT;
		
	}else if (ctx->button == BUTTON_ID_3){
		logImportNext(bike);
		return PAGE_RET_INPUT_HALT;
	}

	return PAGE_RET_INPUT_CONTINUE;	
}

static inline int page_route (bikegps_t *bike, void *ctx, const uint32_t msg, int32_t v1, int32_t v2, void *opaque)
{
	switch (msg){
	case PAGE_MSG_RENDER:
		route_render(bike, (ILI9341_due*)ctx, (uint32_t)v1, (gps_route_t*)opaque);
		break;
	case PAGE_MSG_INPUT:
		return route_input(bike, (page_input_t*)ctx, (gps_route_t*)opaque);

	case PAGE_MSG_ENTER:
		route_enter(bike, (ILI9341_due*)ctx);
		break;
	case PAGE_MSG_LEAVE:
		break;
	case PAGE_MSG_START:
		route_start(bike, (ILI9341_due*)ctx);
		break;
	case PAGE_MSG_SHUTDOWN:
		break;
	}
	return 0;
}
/*
##################################################################################################################
##################################################################################################################
##################################################################################################################
##################################################################################################################
*/

static const gps_poi_t g_poi[] = {
	{{54.663276f, -5.982578f}, "Hightown Rd"},
	{{54.611116f, -6.017889f}, "Divis"},
	{{54.634313f, -6.023129f}, "Ballyhill Rd"},
	{{54.640108f, -6.009027f}, "Ballyutoag Rd"},
	//{{54.586646f, -5.995084f}, "Monagh Bypass"},
	{{54.581346f, -6.025069f}, "\nUpper Springfield Rd"},
	{{54.597504f, -5.976785f}, "Springfield Rd"},
	{{54.635126f, -6.147398f}, "\nNutts Corner"},
	{{54.621882f, -5.976810f}, "Ligoniel Rd"},
	//{{54.666661f, -5.942433f}, "O'Neill Rd"},
	{{54.664982f, -6.059990f}, "Lylehill Rd"},
	{{54.732774f, -6.122308f}, "Ballysavage"},
	{{54.715589f, -5.956529f}, "Irish Hill"},
	{{54.738252f, -5.896734f}, "Slievetrue"},
	{{54.724359f, -5.866116f}, "Knockagh Rd E."},
	{{54.702433f, -5.916975f}, "Knockagh Rd S."},
	//{{54.677458f, -5.954215f}, "Carnmoney Rd"},
	{{54.508808f, -5.970211f}, "Drumbo Rd"},		// Tullyard
	
	{{0.0f, 0.0f}, ""}
};

static inline void poi_enter (bikegps_t *bike, ILI9341_due *tft)
{
	tft->setFont(FONT_TINY);
	tft->setTextLetterSpacing(2);
	tft->setFontMode(gTextFontModeTransparent);
}

static inline void poi_leave (bikegps_t *bike, ILI9341_due *tft)
{
	tft->setFontMode(gTextFontModeSolid);
}

static inline void poi_render (bikegps_t *bike, ILI9341_due *tft, const uint32_t time, gps_route_t *rc)
{

	displayRoute(bike, tft, rc, COLOUR_ROUTE, 1);

	if (bike->render.route.lut.written > 16){
		tft_pos_t *pos = lutFront(&bike->render.route);
		
		// start
		int16_t cx = pos[0].x;
		int16_t cy = pos[0].y;
		tft->fillRect(cx-4, cy-4, 9, 9, COLOUR_BLUE);

		// end
		cx = pos[bike->render.route.lut.written-1].x;
		cy = pos[bike->render.route.lut.written-1].y;
		tft->fillCircle(cx, cy, 4, COLOUR_RED);
	}

	poiDraw(bike, tft, rc, g_poi);
	displayRecordingStatus(bike, tft);
	
	//double y1 = normalizeLat(rc, rc->minLat) * (double)rc->height * rc->heightMultiplier;
	//double y2 = normalizeLat(rc, rc->minLat) * (double)1.0 * rc->heightMultiplier;
	//printfn("%f %f, %f %f", rc->scaleLatZoom, rc->heightMultiplier, rc->scaleLatZoom*120.0, (rc->scaleLatZoom*120.0)*rc->heightMultiplier);
	
}

static inline int poi_input (bikegps_t *bike, page_input_t *ctx, gps_route_t *route)
{
	if (ctx->state != INPUT_MSG_BUTTON_UP)
		return PAGE_RET_INPUT_CONTINUE;

	if (ctx->button == BUTTON_ID_2){
		logImportPrevious(bike);
		return PAGE_RET_INPUT_HALT;
		
	}else if (ctx->button == BUTTON_ID_3){
		logImportNext(bike);
		return PAGE_RET_INPUT_HALT;
	}

	return PAGE_RET_INPUT_CONTINUE;	
}

static inline int page_poi (bikegps_t *bike, void *ctx, const uint32_t msg, int32_t v1, int32_t v2, void *opaque)
{
	switch (msg){
	case PAGE_MSG_RENDER:
		poi_render(bike, (ILI9341_due*)ctx, (uint32_t)v1, (gps_route_t*)opaque);
		break;
	case PAGE_MSG_INPUT:
		return poi_input(bike, (page_input_t*)ctx, (gps_route_t*)opaque);
		
	case PAGE_MSG_ENTER:
		poi_enter(bike, (ILI9341_due*)ctx);
		break;
	case PAGE_MSG_LEAVE:
		poi_leave(bike, (ILI9341_due*)ctx);
		break;
	case PAGE_MSG_START:
		break;
	case PAGE_MSG_SHUTDOWN:
		break;
	}
	return 0;
}

/*
##################################################################################################################
##################################################################################################################
##################################################################################################################
##################################################################################################################
*/

static inline void satellite_enter (bikegps_t *bike, ILI9341_due *tft)
{
	ublox_nema_enable("GSV");
	tft->setFont(FONT_MAIN);
	tft->setTextLetterSpacing(FONT_MAIN_SPACING);
}

static inline void satellite_leave (bikegps_t *bike, ILI9341_due *tft)
{
	ublox_nema_disable("GSV");
}

static inline void satellite_render (bikegps_t *bike, ILI9341_due *tft, const uint32_t time)
{
	
   	displayLocationInfo(bike, tft, 2, 0);
	displaySignalGraph(bike, tft, time);
	displayCourse(bike, tft, (DWIDTH>>1)+60, (DHEIGHT>>1)+21, 95, 13.33f, bike->stats.course);
	displaySignalWorld(bike, tft, (DWIDTH>>1)+60, (DHEIGHT>>1)+21);
	displayRecordingStatus(bike, tft);
}

static inline int satellite_input (bikegps_t *bike, page_input_t *ctx, gps_track_t *track)
{
	if (ctx->state != INPUT_MSG_BUTTON_UP)
		return PAGE_RET_INPUT_CONTINUE;

	if (ctx->button == BUTTON_ID_4){
		ILI9341_due *tft = &bike->render.tft;

		tft->reinit();
		displayClear(bike);
		return PAGE_RET_INPUT_HALT;
		
	}else if (ctx->button == BUTTON_ID_3){
		ILI9341_due *tft = &bike->render.tft;
		
		tft->setRotation(iliRotation90);
		displayClear(bike);
		return PAGE_RET_INPUT_HALT;
	}
	
	return PAGE_RET_INPUT_CONTINUE;	
}		

static inline int page_satellite (bikegps_t *bike, void *ctx, const uint32_t msg, int32_t v1, int32_t v2, void *opaque)
{
	switch (msg){
	case PAGE_MSG_RENDER:
		satellite_render(bike, (ILI9341_due*)ctx, (const uint32_t)v1);
		break;
	case PAGE_MSG_INPUT:
		return satellite_input(bike, (page_input_t*)ctx, (gps_track_t*)opaque);
		
	case PAGE_MSG_ENTER:
		satellite_enter(bike, (ILI9341_due*)ctx);
		break;
	case PAGE_MSG_LEAVE:
		satellite_leave(bike, (ILI9341_due*)ctx);
		break;
	case PAGE_MSG_START:
		break;
	case PAGE_MSG_SHUTDOWN:
		break;
	}
	return 0;
}


/*
##################################################################################################################
##################################################################################################################
##################################################################################################################
##################################################################################################################
*/


static int fileIdx = 0;
static int lineChange = 0;
static int dirTotalFiles = 0;
static int cursorIdx = 5;
static int fileLoadSuccess = 0;
static char filename[32];
#define FILES_DISPLAYABLE		(10)


static inline void setFileCursorPrev ()
{
	if (--cursorIdx < 0)
		cursorIdx = 0;

	if (cursorIdx > dirTotalFiles-1)
		cursorIdx = dirTotalFiles-1;
		
	if (cursorIdx > fileIdx+(FILES_DISPLAYABLE-1))
		fileIdx = cursorIdx+(FILES_DISPLAYABLE-1);
	if (cursorIdx < fileIdx)
		fileIdx = cursorIdx;
	if (fileIdx > dirTotalFiles-(FILES_DISPLAYABLE-1))
		fileIdx = dirTotalFiles-(FILES_DISPLAYABLE-1);
	if (fileIdx < 0) fileIdx = 0;
}

static inline void setFileCursorNext ()
{
	if (++cursorIdx > dirTotalFiles-1)
		cursorIdx = dirTotalFiles-1;

	if (cursorIdx > fileIdx+(FILES_DISPLAYABLE-1))
		fileIdx = cursorIdx-(FILES_DISPLAYABLE-1);
	if (cursorIdx < fileIdx)
		fileIdx = cursorIdx;
	if (fileIdx > dirTotalFiles-(FILES_DISPLAYABLE-1))
		fileIdx = dirTotalFiles-(FILES_DISPLAYABLE-1);
	if (fileIdx < 0) fileIdx = 0;
}

static inline void files_enter (bikegps_t *bike, ILI9341_due *tft, gps_log_t *log)
{
	tft->setFont(dialog20);
	tft->setColour(COLOUR_MAIN_FRONT);
	tft->setTextLetterSpacing(2);
	
	dirTotalFiles = log_getTotalFiles(log);
	//cursorIdx = setFileCursor(cursorIdx);
	lineChange = 1;
	fileLoadSuccess = 0;
	*filename = 0;
}

static inline void files_leave (bikegps_t *bike, ILI9341_due *tft, gps_log_t *log)
{
	//printfn("files_leave %i, %i", bike->ctrl.pages.current, log->logsync);
	
	tft->setFont(FONT_MAIN);
	tft->setColour(COLOUR_MAIN_FRONT);
	tft->setTextLetterSpacing(FONT_MAIN_SPACING);
	
	if (log->logsync && fileLoadSuccess){
		log->logsync = 0;

		if (logIsActive(bike)){
			displayClear(bike);
			tft->printAt("Processing:", 2, 2);
			tft->printAt(filename, 46, 32);
			
			logResync(bike, log);
		}
	}
}

static inline void files_render (bikegps_t *bike, ILI9341_due *tft, gps_log_t *log)
{
	SdFat *sd = &log->sd;
	SdFile *file = &log->file;

	char buffer[64];
	int16_t x, y;
	int idx = 0;
	const int fontHeight = tft->getFontHeight();
	int listMax = FILES_DISPLAYABLE + fileIdx;
	log_header_t header = {0};
		
	tft->setCursor(0, 1);
	if (file->isOpen()) file->close();
	sd->chdir("/");
	sd->vwd()->seekSet(0);



	while (idx < fileIdx){
		idx++;
		if (file->openNext(sd->vwd(), O_READ))
			file->close();
	}

	while (idx < listMax && file->openNext(sd->vwd(), O_READ)){
		if (!file->isDir()){
			if (cursorIdx == idx)
				tft->setColour(ILI9341_LIGHTCORAL);
			else if (idx == log->fileIndex)
				tft->setColour(ILI9341_GOLD);
			else
				tft->setColour(COLOUR_MAIN_FRONT);

			if (idx == log->fileIndex)
				tft->print(">");

			tft->getCursor(&x, &y);
			file->getName(buffer, sizeof(buffer));
			if (!lineChange){
				tft->print(buffer);
			}else{
				tft->printAt(buffer, x, y);
				tft->getCursor(&x, &y);	
				tft->fillRect(x, y+1, ((DWIDTH>>1)-16)-x, fontHeight-2, COLOUR_MAIN_BACK);
			}
			
			if (log_header_read(log, &header)){
				if (!lineChange)
					tft->printAt(header.date, (DWIDTH>>1)-16, y);
				else
					tft->printAt(header.date, (DWIDTH>>1)-16, y, gTextEraseToEOL);
			}

			size_t fsize = file->fileSize();
			if (fsize >= 10*1024)
				sprintf(buffer, "%.0fk", fsize/1024.0f);		// over 10k
			else if (fsize >= 1024)
				sprintf(buffer, "%.1fk", fsize/10.0f/1024.0f);	// 1.0k to 9.9k
			else
				sprintf(buffer, "%i", fsize);					// less than 1024 bytes

			tft->printAlignedOffseted(buffer, gTextAlignTopRight, 0, y);
			tft->println("");

			idx++;
		}
		file->close();
	}
	
	if (lineChange) lineChange = 0;
	tft->setColour(COLOUR_MAIN_FRONT);

	if (fileLoadSuccess){
		if (fileLoadSuccess == 1)
			snprintf(buffer, sizeof(buffer), "Activity: %s", filename);
		else if (fileLoadSuccess == 2)
			snprintf(buffer, sizeof(buffer), "Map: %s", filename);
		tft->printAligned(buffer, gTextAlignBottomLeft, gTextEraseToEOL);
	}
	sprintf(buffer, "%i", dirTotalFiles);
	tft->printAligned(buffer, gTextAlignBottomRight);	
}

static inline int files_loadAsRoute (bikegps_t *bike, gps_log_t *log)
{
	if (log_getFilenameByIndex(log, cursorIdx, filename, sizeof(filename))){
		if (logLoad(bike, log, &bike->map.track, -1, filename)){
			log->fileIndex = cursorIdx;
			return 1;
		}
	}
	return 0;
}

static inline int files_loadAsMap (bikegps_t *bike, gps_log_t *log)
{
	if (log_getFilenameByIndex(log, cursorIdx, filename, sizeof(filename))){
		if (loadMap(bike, &bike->staticmap.map, &bike->staticmap.map.log, filename)){
			log->fileIndex = cursorIdx;
			return 1;
		}
	}
	return 0;
}

int files_input (bikegps_t *bike, page_input_t *ctx, void *opaque)
{
	//printfn("files_input %i %i", ctx->state, ctx->dt);
	//if (ctx->state != INPUT_MSG_BUTTON_UP)
	//	return PAGE_RET_INPUT_CONTINUE;


	if (ctx->button == BUTTON_ID_2){			// up
		if (ctx->state == INPUT_MSG_BUTTON_RELEASED){
			setFileCursorPrev();
			lineChange = 1;
			return PAGE_RET_INPUT_HALT;
		}else if (ctx->state == INPUT_MSG_BUTTON_HELD){
			for (int i = 8; i; i--)
				setFileCursorPrev();
			lineChange = 1;
			return PAGE_RET_INPUT_HALT;
		}
	}else if (ctx->button == BUTTON_ID_3){		// down
		if (ctx->state == INPUT_MSG_BUTTON_RELEASED){
			setFileCursorNext();
			lineChange = 1;
			return PAGE_RET_INPUT_HALT;
		}else if (ctx->state == INPUT_MSG_BUTTON_HELD){
			for (int i = 8; i; i--)
				setFileCursorNext();
			lineChange = 1;
			return PAGE_RET_INPUT_HALT;
		}
	}else if (ctx->button == BUTTON_ROUTE_LOAD){
		if (ctx->state == INPUT_MSG_BUTTON_RELEASED){
			if (ctx->dt < 250){
				//printfn("activity");
				fileLoadSuccess = files_loadAsRoute(bike, (gps_log_t*)opaque);
			}else if (ctx->dt >= 1500){
				//printfn("map");
				fileLoadSuccess = files_loadAsMap(bike, (gps_log_t*)opaque)+1;
			}
			lineChange = 1;
		}else if (ctx->state == INPUT_MSG_BUTTON_HELD){
			if (ctx->dt >= 1500){
				//printfn("held");
				fileLoadSuccess = 2;
				log_getFilenameByIndex((gps_log_t*)opaque, cursorIdx, filename, sizeof(filename));
				displayUpdate(bike);
			//}else if (ctx->dt >= 3000){
				
			}
		}
		//printfn("fileLoadSuccess %i", fileLoadSuccess);
		return PAGE_RET_INPUT_HALT;
	}
	
	return PAGE_RET_INPUT_CONTINUE;	
}

int page_files (bikegps_t *bike, void *ctx, const uint32_t msg, int32_t v1, int32_t v2, void *opaque)
{
	switch (msg){
	case PAGE_MSG_RENDER:
		files_render(bike, (ILI9341_due*)ctx, (gps_log_t*)opaque);
		break;
		
	case PAGE_MSG_INPUT:
		return files_input(bike, (page_input_t*)ctx, opaque);

	case PAGE_MSG_ENTER:
		files_enter(bike, (ILI9341_due*)ctx, (gps_log_t*)opaque);
		break;
		
	case PAGE_MSG_LEAVE:
		files_leave(bike, (ILI9341_due*)ctx, (gps_log_t*)opaque);
		break;
		
	case PAGE_MSG_START:
		break;
	case PAGE_MSG_SHUTDOWN:
		break;
	}
	
	return 0;
}

/*
##################################################################################################################
##################################################################################################################
##################################################################################################################
##################################################################################################################
*/


static inline gps_page_t *pageGetStruct (gps_control_t *ctrl, const uint8_t id)
{
	return &ctrl->pages.list[id];
}

static inline void pageRegister (gps_control_t *ctrl, const uint8_t id, render_pageCb_t cb, int32_t v1, int32_t v2, void *opaque)
{
	gps_page_t *page = pageGetStruct(ctrl, id);
	
	page->id = id;
	page->cb = cb;
	page->arg1 = v1;
	page->arg2 = v2;
	page->opaque = opaque;
}

static inline int pageSendMessage (bikegps_t *bike, gps_control_t *ctrl, const uint8_t id, const uint32_t msg, void *ctx, int32_t v1, int32_t v2)
{
	gps_page_t *page = pageGetStruct(ctrl, id);

	return page->cb(bike, ctx, msg, v1, v2, page->opaque);
}

static inline int pageStart (bikegps_t *bike, gps_control_t *ctrl, const uint8_t id, int32_t v1, int32_t v2)
{
	//printfn("pageStart %i", id);
	
	return pageSendMessage(bike, ctrl, id, PAGE_MSG_START, (void*)&bike->render.tft, v1, v2);
}

static inline int pageEnter (bikegps_t *bike, gps_control_t *ctrl, const uint8_t id, int32_t v1, int32_t v2)
{
	//printfn("pageEnter %i", id);
	
	return pageSendMessage(bike, ctrl, id, PAGE_MSG_ENTER, (void*)&bike->render.tft, v1, v2);
}

static inline int pageLeave (bikegps_t *bike, gps_control_t *ctrl, const uint8_t id, int32_t v1, int32_t v2)
{
	//printfn("pageLeave %i", id);
	
	return pageSendMessage(bike, ctrl, id, PAGE_MSG_LEAVE, (void*)&bike->render.tft, v1, v2);
}

/*
##############
##############
##############
*/

int pageRender (bikegps_t *bike, gps_control_t *ctrl, int32_t v1, int32_t v2)
{
	
	if (ctrl->pages.current != ctrl->pages.previous)
		pageEnter(bike, ctrl, ctrl->pages.current, 0, 0);


	//printfn("pageRender %i", ctrl->pages.current);
		
	int ret = pageSendMessage(bike, ctrl, ctrl->pages.current, PAGE_MSG_RENDER, (void*)&bike->render.tft, v1, v2);
	ctrl->pages.previous = ctrl->pages.current;
	return ret;
}

uint8_t pageGetNext (gps_control_t *ctrl)
{
	int id = ctrl->pages.current;
	if (++id > PAGE_TOTAL)
		id = PAGE_FIRST;
	return id;
}

uint8_t pageGetActive (gps_control_t *ctrl)
{
	return ctrl->pages.current;
}

uint8_t pageGetActivePrevious (gps_control_t *ctrl)
{
	return ctrl->pages.previous;
}

uint8_t pageSetActive (bikegps_t *bike, gps_control_t *ctrl, const uint8_t id)
{
	//printfn("pageSetActive %i", id);
	
	if (ctrl->pages.current != PAGE_INVALID)
		pageLeave(bike, ctrl, ctrl->pages.current, 0, 0);

	ctrl->pages.previous = ctrl->pages.current;
	ctrl->pages.current	= id;
	
	return ctrl->pages.previous;
}

void pagesStart (bikegps_t *bike, gps_control_t *ctrl)
{
	for (int i = PAGE_FIRST; i <= PAGE_TOTAL; i++)
		pageStart(bike, ctrl, i, 0, 0);
}

int pageInput (bikegps_t *bike, gps_control_t *ctrl, page_input_t *ctx)
{
	return pageSendMessage(bike, ctrl, ctrl->pages.current, PAGE_MSG_INPUT, ctx, 0, 0);
}

void pagesInit (bikegps_t *bike, gps_control_t *ctrl, const uint8_t defaultId)
{
	ctrl->pages.total = PAGE_TOTAL;
	ctrl->pages.current = PAGE_INVALID;
	ctrl->pages.previous = PAGE_INVALID;
	ctrl->pages.defaultId = defaultId;


	pageRegister(ctrl, PAGE_TIME,	  page_time,	 0, 0, &bike->map.rc);
	pageRegister(ctrl, PAGE_OVERVIEW, page_overview, 0, 0, &bike->map.track);
	pageRegister(ctrl, PAGE_SPEED,	  page_speed,	 0, 0, &bike->map.track);
	pageRegister(ctrl, PAGE_ACTIVITY, page_activity, 0, 0, &bike->map.track);
	pageRegister(ctrl, PAGE_ROUTE,	  page_route,	 0, 0, &bike->map.rc);
	pageRegister(ctrl, PAGE_POI,	  page_poi,		 0, 0, &bike->map.rc);
	pageRegister(ctrl, PAGE_SATELLITE,page_satellite,0, 0, NULL);
	pageRegister(ctrl, PAGE_FILES,	  page_files,	 0, 0, &bike->map.log);
}

