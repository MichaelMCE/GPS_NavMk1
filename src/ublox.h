
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


#ifndef _UBLOX_H_
#define _UBLOX_H_

#pragma GCC optimize ("-O2")

#include "Arduino.h"



#if defined(__SAM3X8E__)
//#undef __FlashStringHelper::F(string_literal)
#undef __FlashStringHelper
#undef F
#define F(string_literal) string_literal
#endif


#ifdef INLINE
#undef INLINE
#endif
#define INLINE						inline __attribute__((always_inline))
#define module						Serial1


#define UBLOX_BAUD_4800				10
#define UBLOX_BAUD_9600				20
#define UBLOX_BAUD_38400			30
#define UBLOX_BAUD_115200			40
#define UBLOX_BAUD_230400			50

#define UBLOX_RATE_20000			20000		// once per 20 seconds
#define UBLOX_RATE_3000				3000		// once per 3 seconds
#define UBLOX_RATE_1000				1000		// once per second
#define UBLOX_RATE_500				500			// 1hz
#define UBLOX_RATE_200				200			// 5hz
#define UBLOX_RATE_100				100			// 10hz



inline int ublox_msg_send (const char *msg);
inline void ublox_nema_enable (const char *nema);
inline void ublox_nema_disable (const char *nema);

inline int ublox_set_baud (const int baud);
inline int ublox_set_rate (const int rate);
inline void ublox_request_coldstart ();		// similar to a power-cycle: clears internal memory losing all Satellite tracking metadata
inline void ublox_request_hotstart ();		// as with coldstart but don't lose the satellite meta and tracking info
inline void ublox_set_sleep ();				// enter low poer mode. there is no output in sleeping mode, but the module still tracks position. send a hotstart request to revert to normal functionality
inline void ublox_sbas_enable ();
inline void ublox_sbas_disable ();
inline void ublox_set_gnss_gps_sbas ();
inline void ublox_set_gnss_gps ();
inline void ublox_profile_portable ();
inline void ublox_profile_pedestrian ();
inline void ublox_profile_airborne ();		// < 1g
inline void ublox_profile_stationary ();
inline void ublox_request_saveconfig ();	// save current configuration to flash (GPs module's flash)
inline void ublox_request_reset ();			// reset to manufacture defaults
inline void ublox_request_softreset ();		// restart/reinitialize device
inline void ublox_set_msg_profile (const int cfg);
inline void ublox_set_nav_status ();
inline void ublox_set_nav_dop ();
inline void ublox_set_nav_pvt ();
inline void ublox_set_nav_svinfo ();
inline void ublox_pubx_00_enable ();
inline void ublox_pubx_00_disable ();
inline void ublox_start (const int rate);
inline void ublox_stop ();
inline char ublox_read ();
inline int ublox_hasdata ();



#include "ublox.cpp"



#endif


