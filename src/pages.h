
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


#ifndef _PAGES_H_
#define _PAGES_H_




typedef struct {
	uint16_t button;
	uint16_t state;
	uint32_t dt;
}page_input_t;


int pageRender (bikegps_t *bike, gps_control_t *ctrl, int32_t v1, int32_t v2);

void pagesInit (bikegps_t *bike, gps_control_t *ctrl, const uint8_t defaultId);
void pagesStart (bikegps_t *bike, gps_control_t *ctrl);

uint8_t pageGetNext (gps_control_t *ctrl);
uint8_t pageSetActive (bikegps_t *bike, gps_control_t *ctrl, const uint8_t id);
uint8_t pageGetActive (gps_control_t *ctrl);
uint8_t pageGetActivePrevious (gps_control_t *ctrl);

int pageInput (bikegps_t *bike, gps_control_t *ctrl, page_input_t *ctx);




/*

static inline void _render (bikegps_t *bike, ILI9341_due *tft)
{
}


static inline int _input (bikegps_t *bike, page_input_t *ctx, void *opaque)
{
	if (ctx->state != INPUT_MSG_BUTTON_UP)
		return PAGE_RET_INPUT_CONTINUE;


	if (ctx->button == BUTTON_ID_2){
		
		return PAGE_RET_INPUT_HALT;
		
	}else if (ctx->button == BUTTON_ID_3){
		
		return PAGE_RET_INPUT_HALT;
	}
	return PAGE_RET_INPUT_CONTINUE;	
}

static inline int page_ (bikegps_t *bike, void *ctx, const uint32_t msg, int32_t v1, int32_t v2, void *opaque)
{
	switch (msg){
	case PAGE_MSG_RENDER:
		_render(bike, (ILI9341_due*)ctx, (const uint32_t)v1);
		break;
		
	case PAGE_MSG_INPUT:
		return _input (bike, (page_input_t*)ctx, (gps_ _t*)opaque);

	case PAGE_MSG_ENTER:
		_enter(bike, (ILI9341_due*)ctx);
		break;
	case PAGE_MSG_LEAVE:
		break;
	case PAGE_MSG_START:
		break;
	case PAGE_MSG_SHUTDOWN:
		break;
	}
	
	return 0;
}*/


#endif

