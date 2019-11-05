/*
TinyGPS++ - a small GPS library for Arduino providing universal NMEA parsing
Based on work by and "distanceBetween" and "courseTo" courtesy of Maarten Lamers.
Suggestion to add satellites, courseTo(), and cardinal() by Matt Monson.
Location precision improvements suggested by Wayne Holder.
Copyright (C) 2008-2013 Mikal Hart
All rights reserved.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/
 
 
 

#pragma GCC optimize ("-O3")


#include "TinyGPS++.h"

#include <string.h>
#include <ctype.h>
#include <stdlib.h>


#define	_BST_1			1

 

#define _TERM_GPGBS		"GPGBS"			//	 	GNSS satellite fault detection 
#define _TERM_GPGST		"GPGST"			//		Position error statistics
#define _TERM_GPGSA		"GPGSA"			//  	GPS DOP and active satellites

#define _TERM_GPGSV		"GPGSV"			//	 	Number of SVs in view, PRN, elevation, azimuth, and SNR
#define _TERM_GPRMC		"GPRMC"			//  	Position, Velocity, and Time
#define _TERM_GPGGA		"GPGGA"			//	 	Time, position, and fix related data

#define _TERM_PUBX		"PUBX"




TinyGPSPlus::TinyGPSPlus()
  :  parity(0)
  ,  isChecksumTerm(false)
  ,  curSentenceType(GPS_SENTENCE_OTHER)
  ,  curTermNumber(0)
  ,  curTermOffset(0)
  ,  sentenceHasFix(false)
  ,  encodedCharCount(0)
  ,  sentencesWithFixCount(0)
  ,  failedChecksumCount(0)
  ,  passedChecksumCount(0)
{
  term[0] = '\0';
  location.cbuffClear();
}






//
// public methods
//

bool TinyGPSPlus::encode (const char c)
{
  ++encodedCharCount;

  switch(c){
  case ',': // term terminators
    parity ^= (uint8_t)c;
  case '\r':
  case '\n':
  case '*':
    {
      bool isValidSentence = false;
      if (curTermOffset < sizeof(term)){
        term[curTermOffset] = 0;
        isValidSentence = endOfTermHandler();
      }
      
      ++curTermNumber;
      curTermOffset = 0;
      isChecksumTerm = c == '*';		// fixme
      return true; //isValidSentence;
    }
    break;

  case '$': // sentence begin
    curTermNumber = curTermOffset = 0;
    parity = 0;
    curSentenceType = GPS_SENTENCE_OTHER;
    isChecksumTerm = false;
    sentenceHasFix = false;
	
    return false;

  default: // ordinary characters
    if (curTermOffset < sizeof(term)-1)
      term[curTermOffset++] = c;
    if (!isChecksumTerm)
      parity ^= c;
    return false;
  }

  return false;
}

int TinyGPSPlus::encode (const char *buffer, const int len)
{
	int ret = 0;
	
	for (int i = 0; i < len && buffer[i]; i++)
		ret += encode(buffer[i]);
	return ret;
}


//
// internal utilities
//
inline int TinyGPSPlus::fromHex (const uint8_t a)
{
  if (a >= 'A' && a <= 'F')
    return a - 'A' + 10;
  else if (a >= 'a' && a <= 'f')
    return a - 'a' + 10;
  else
    return a - '0';
}

inline double TinyGPSPlus::parseFloat (const char *term)
{
  bool negative = *term == '-';
  if (negative) ++term;
  double ret = (double)atol(term);
  
  while (isdigit(*term)) ++term;
  
  if (*term == '.' && isdigit(term[1])){
    ret += ((term[1] - '0') * 0.10);
    if (isdigit(term[2]))
      ret += ((term[2] - '0') * 0.01);
  }
  
  return negative ? -ret : ret;
}

// static
// Parse a (potentially negative) number with up to 2 decimal digits -xxxx.yy
inline int32_t TinyGPSPlus::parseDecimal (const char *term)
{
  bool negative = *term == '-';
  if (negative) ++term;
  int32_t ret = 100 * (int32_t)atol(term);
  
  while (isdigit(*term)) ++term;
  
  if (*term == '.' && isdigit(term[1])){
    ret += 10 * (term[1] - '0');
    if (isdigit(term[2]))
      ret += term[2] - '0';
  }
  return negative ? -ret : ret;

}

// static
// Parse degrees in that funny NMEA format DDMM.MMMM
void TinyGPSPlus::parseDegrees (const char *term, RawDegrees &deg)
{
#if 1
  uint32_t leftOfDecimal = (uint32_t)atol(term);
  uint16_t minutes = (uint16_t)(leftOfDecimal % 100);
  uint32_t multiplier = 10000000UL;
  uint32_t tenMillionthsOfMinutes = minutes * multiplier;

  deg.deg = (int16_t)(leftOfDecimal / 100);

  while (isdigit(*term))
    ++term;

  if (*term == '.'){
    while (isdigit(*++term)){
      multiplier /= 10;
      tenMillionthsOfMinutes += (*term - '0') * multiplier;
    }
  }
  
  deg.billionths = (5 * tenMillionthsOfMinutes + 1) / 3;
  deg.negative = false;
#endif
}

// Processes a just-completed term
// Returns true if new sentence has just passed checksum test and is validated
bool TinyGPSPlus::endOfTermHandler()
{
  // If it's the checksum term, and the checksum checks out, commit
  if (isChecksumTerm){
    byte checksum = 16 * fromHex(term[0]) + fromHex(term[1]);
    
    if (checksum == parity){
      passedChecksumCount++;
      if (sentenceHasFix) sentencesWithFixCount++;

      switch(curSentenceType){
      case GPS_SENTENCE_GPRMC:
        date.commit();
        time.commit();
        
        if (sentenceHasFix){
           location.commit();
           speed.commit();
           course.commit();
           //location.cbuffAdd(location.lat(), location.lng(), altitude.meters());
        }
        break;
      case GPS_SENTENCE_GPGGA:
        time.commit();
        
        if (sentenceHasFix){
          location.commit();
          altitude.commit();
          if (fix.type > 1)
		 	location.cbuffAdd(location.lat(), location.lng(), altitude.meters());
        }
        satellites.withFix.commit();
        //dop.hori.commit();
        break;
      case GPS_SENTENCE_PUBX:
      	time.commit();
      	if (sentenceHasFix){
          location.commit();
        }
      	break;
      /*case GPS_SENTENCE_GPGST:
      case GPS_SENTENCE_GPGSV:
      case GPS_SENTENCE_GPGSA:
      case GPS_SENTENCE_GPGBS:
      	break;*/
      default:
      	break;
      }

      return true;
    }else{
      ++failedChecksumCount;
    }

    return false;
  }

  // the first term determines the sentence type
  if (curTermNumber == 0){
    /*if (!strcmp(term, _TERM_GPGSA))
      curSentenceType = GPS_SENTENCE_GPGSA;
    else*/
    if (!strcmp(term, _TERM_GPGSV))
      curSentenceType = GPS_SENTENCE_GPGSV;
    else if (!strcmp(term, _TERM_GPRMC))
      curSentenceType = GPS_SENTENCE_GPRMC;
    else if (!strcmp(term, _TERM_GPGGA))
      curSentenceType = GPS_SENTENCE_GPGGA;
    else if (!strcmp(term, _TERM_PUBX))
      curSentenceType = GPS_SENTENCE_PUBX;
#if 0
    else if (!strcmp(term, _TERM_GPGST))
      curSentenceType = GPS_SENTENCE_GPGST;
    else if (!strcmp(term, _TERM_GPGBS))
      curSentenceType = GPS_SENTENCE_GPGBS;
#endif
    else
      curSentenceType = GPS_SENTENCE_OTHER;
      
	//++sentenceCount[curSentenceType] &= 0xFFFF;

#if 0
	SerialUSB.print("# ");
	SerialUSB.print(term);
	SerialUSB.print("  ");
	SerialUSB.println(curSentenceType);
#endif

	return false;
  }


	// SerialUSB.println(term);

  if (curSentenceType != GPS_SENTENCE_OTHER && term[0]){
	if (curSentenceType == GPS_SENTENCE_GPGGA){
	  switch (curTermNumber){
	  case 1: // Time in both sentences
	  	// SerialUSB.print("GPGGA 1: ");
		// SerialUSB.println(term);
	    time.setTime(term);
	    break;
	  case 2: // Latitude
	    location.setLatitude(term);
	    break;
	  case 3: // N/S
	    location.rawNewLatData.negative = (term[0] == 'S');
	    break;
	  case 4: // Longitude
	    location.setLongitude(term);
	    break;
	  case 5: // E/W
	    location.rawNewLngData.negative = (term[0] == 'W');
	    break;
	  case 6: // Fix data (GPGGA)
	    sentenceHasFix = (term[0] > '0');
	    break;
	  case 7: // Satellites used (GPGGA)
	    satellites.withFix.set(term);
	    break;
	  case 8: // HDOP
	    dop.hori = parseFloat(term);
	    break;
	  case 9: // Altitude (GPGGA)
	    altitude.set(term);
	    break;
	  }

	}else if (curSentenceType == GPS_SENTENCE_GPRMC){
	  switch (curTermNumber){
	  case 1: // Time in both sentences
	  	// SerialUSB.print("GPRMC 1: ");
		// SerialUSB.println(term);
	    time.setTime(term);
	    break;
	  case 2: // GPRMC validity
	    sentenceHasFix = (term[0] == 'A');
	    break;
	  case 3: // Latitude
	    location.setLatitude(term);
	    break;
	  case 4: // N/S
	    location.rawNewLatData.negative = (term[0] == 'S');
	    break;
	  case 5: // Longitude
	    location.setLongitude(term);
	    break;
	  case 6: // E/W
	    location.rawNewLngData.negative = (term[0] == 'W');
	    break;
	  case 7: // Speed (GPRMC)
	    speed.set(term);
	    
	    //SerialUSB.print("10hz spd: ");
		//SerialUSB.println(term);
	    
	    break;
	  case 8: // Course (GPRMC)
	    course.set(term);
	    break;
	  case 9: // Date (GPRMC)
	    date.setDate(term);
	    break;
	    
	  case 10:
	  	break;
	  }

  	}else if (curSentenceType == GPS_SENTENCE_GPGSV){
		switch (curTermNumber){
		case 1:		// number of GSV sentences
		  satellites.working.id = 0;
		  satellites.working.cnr = 0;
		  break;
		case 2:		// sentence number
		  //if (atol(term) == 1)		// its the beginning so reset all
			//memset(satellites.sats, 0, sizeof(satellites.sats));
		  break;
		case 3:		// number of satellites
		  satellites.visable = atol(term);
		  break;
		case 4:
		case 8:
		case 12:
		case 16:
		  if (satellites.working.id >= 1 && satellites.working.id <= SAT_MAX){
		  	//satellites.working.age = millis();
		  	satellites.sats[satellites.working.id-1] = satellites.working;
		  }
		  satellites.working.id = atol(term);
		  if (satellites.working.id > SAT_MAX)
		  	satellites.working.id = 0;
		  break;
		case 5:
		case 9:
		case 13:
		case 17:
		  satellites.working.elevation = atol(term);
		  if (satellites.working.elevation > 90)
		  	satellites.working.elevation = 90;
		  break;
		case 6:
		case 10:
		case 14:
		case 18:
		  satellites.working.azimuth = atol(term);
		  if (satellites.working.azimuth > 360)
		  	satellites.working.azimuth = 360;
		  break;
		case 7:
		case 11:
		case 15:
		case 19:
		  satellites.working.cnr = atol(term);
		  if (satellites.working.cnr <= 120){
			if (satellites.working.id >= 1 && satellites.working.id <= SAT_MAX){
				satellites.working.age = millis();
		  		satellites.sats[satellites.working.id-1] = satellites.working;
		  	}
		  }
		  satellites.working.id = 0;
		  satellites.working.cnr = 0;
		  break;
		}		
	}else if (curSentenceType == GPS_SENTENCE_PUBX){
		//SerialUSB.print("curTermNumber: ");
		//SerialUSB.println(curTermNumber);

  		switch (curTermNumber){
		case 1:
  		  pubx_00 = !strcmp("00", term);
  		   /*SerialUSB.print("PUBX message: ");
  		   SerialUSB.print(term);
  		   SerialUSB.print(" ");
  		   SerialUSB.println(pubx_00);*/
  		  break;
		case 2:
		if (pubx_00){
  			time.setTime(term);
  		}
  		break;
		case 3: // Latitude
		if (pubx_00){
	    	location.setLatitude(term);
	    }
	    break;
	  	case 4: // N/S
	  	if (pubx_00){
	    	location.rawNewLatData.negative = (term[0] == 'S');
	    }
	    break;
	  	case 5: // Longitude
	  	if (pubx_00){
	    	location.setLongitude(term);
	    }
	    break;
	  	case 6: // E/W
	  	if (pubx_00){
	    	location.rawNewLngData.negative = (term[0] == 'W');
	    }
		break;
		
		case 7:
		/*if (pubx_00){
			SerialUSB.print("alt: ");
			SerialUSB.println(parseFloat(term));
		}*/
		break;

		case 8: // fix type
		if (pubx_00){
	    	fix.mode = sentenceHasFix = (term[0] != 'N');		// NF = No Fix
	    	
	    	if (/*(term[0] == 'G' || term[0] == 'D')&&*/ term[1] == '2')
	    		fix.type = 2;
	    	else if (/*(term[0] == 'G' || term[0] == 'D') &&*/ term[1] == '3')
	    		fix.type = 3;
	    	else
	    		fix.type = 0;
	    }
	    break;
	    
		case 9:
		if (pubx_00){
		  	fix.hAcc = parseFloat(term);
		  	
		  	// SerialUSB.print("fix.hAcc: ");
		    // SerialUSB.println(fix.hAcc);
		}
		break;
		case 10:
		  if (pubx_00){
		  	fix.vAcc = parseFloat(term);
		  	
		  	// SerialUSB.print("fix.vAcc: ");
		    // SerialUSB.println(fix.vAcc);
		  }
		  break;
		case 11:
		  if (pubx_00){
		  	speed.km = parseFloat(term);
		  	
		  	//SerialUSB.print("1hz spd: ");
		    //SerialUSB.println(speed.km);
		  }
		  break;
		case 18:
		  if (pubx_00){
		  	fix.numSv = atol(term);
		  	
		  	// SerialUSB.print("fix.numSv: ");
		    // SerialUSB.println(fix.numSv);
		  }
		  break;

		case 19:	// Dead Reckon Status Flags 
		case 20:
		default:
			pubx_00 = false;
		  break;
		}
#if 0  		
  	}else if (curSentenceType == GPS_SENTENCE_GPGBS){
  		switch (curTermNumber){
		case 1:		//time
		  // SerialUSB.print("GPGBS 1: ");
		  // SerialUSB.println(term);
		  break;
		case 2:
		  estats.errLat = parseFloat(term);
		  break;
		case 3:
		  estats.errLong = parseFloat(term);
		  break;
		case 4:
		  estats.errAlt = parseFloat(term);
		  break;
		default:
		  break;
		}
  	}else if (curSentenceType == GPS_SENTENCE_GPGST){
  		switch (curTermNumber){
		case 1:		//time
			// SerialUSB.print("GPGST 1: ");
		  // SerialUSB.println(term);
		  break;
		case 2:
		  estats.rangeRMS = parseFloat(term);
		  break;
		case 3:
		  estats.stdMajor = parseFloat(term);
		  break;
		case 4:
		  estats.stdMinor = parseFloat(term);
		  break;
		case 5:
		  estats.orient = parseFloat(term);
		  break;
		case 6:
		  estats.stdLat = parseFloat(term);
		  break;
		case 7:
		  estats.stdLong = parseFloat(term);
		  break;
		case 8:
		  estats.stdAlt = parseFloat(term);
		  break;
		  
		default:
		  break;
		}
  	}else if (curSentenceType == GPS_SENTENCE_GPGSA){
  		switch (curTermNumber){
		case 1:
		  fix.mode = term[0];
		  break;
		case 2:
		  fix.type = (uint8_t)atol(term);
		  break;
		case 15:
		  dop.pos = parseFloat(term);
		  break;
		case 16:
		  dop.hori = parseFloat(term);
		  break;
		case 17:
		  dop.vert = parseFloat(term);
		  break;
		default:
		  break;
		}
#endif

	}
  }
  return false;
}

#if 0
/* static */
double TinyGPSPlus::distanceBetween (double lat1, double long1, double lat2, double long2)
{
/*	Serial.print(lat1, 8); Serial.print(" ");
	Serial.println(long1, 8);
	
	Serial.print(lat2, 8); Serial.print(" ");
	Serial.println(long2, 8);
	Serial.println(" ");
*/	

  // returns distance in meters between two positions, both specified
  // as signed decimal-degrees latitude and longitude. Uses great-circle
  // distance computation for hypothetical sphere of radius 6372795 meters.
  // Because Earth is no exact sphere, rounding errors may be up to 0.5%.
  // Courtesy of Maarten Lamers
  double delta = radians(long1-long2);
  double sdlong = sin(delta);
  double cdlong = cos(delta);
  lat1 = radians(lat1);
  lat2 = radians(lat2);
  double slat1 = sin(lat1);
  double clat1 = cos(lat1);
  double slat2 = sin(lat2);
  double clat2 = cos(lat2);
  delta = (clat1 * slat2) - (slat1 * clat2 * cdlong);
  delta = sq(delta);
  delta += sq(clat2 * sdlong);
  delta = sqrt(delta);
  double denom = (slat1 * slat2) + (clat1 * clat2 * cdlong);
  delta = atan2(delta, denom);
  return delta * 6372795.0;
}

#else

double TinyGPSPlus::distanceBetween (double lat1, double lon1, double lat2, double lon2)
{
	
	//printf("%f %f %f %f\n", lat1, lon1, lat2, lon2);
	
	const double R = 6378137.0;		// Earths radius
	const double pi80 = PI / 180.0;

	//double d = acos( sin(lat1*M_PI/180.0)*sin(lat2*M_PI/180.0) + cos(lat1*M_PI/180.0)*cos(lat2*M_PI/180.0)*cos(lon2*M_PI /180.0-lon1*M_PI/180.0) ) * R;
	//printf("dist b; %.0f\n", d);
		
	lat1 *= pi80;
	lon1 *= pi80;
	lat2 *= pi80;
	lon2 *= pi80;
	double dlat = fabs(lat2 - lat1);
	double dlon = fabs(lon2 - lon1);
	double a = sin(dlat / 2.0) * sin(dlat / 2.0) + cos(lat1) * cos(lat2) * sin(dlon /2.0) * sin(dlon / 2.0);
	double c = 2.0 * atan2(sqrt(a), sqrt(1.0 - a));
	double d = R * c;
	//printf("dist %.0f\n", d);

	return d;
}

#endif

double TinyGPSPlus::courseTo (double lat1, double long1, double lat2, double long2)
{
  // returns course in degrees (North=0, West=270) from position 1 to position 2,
  // both specified as signed decimal-degrees latitude and longitude.
  // Because Earth is no exact sphere, calculated course may be off by a tiny fraction.
  // Courtesy of Maarten Lamers
  double dlon = radians(long2-long1);
  lat1 = radians(lat1);
  lat2 = radians(lat2);
  double a1 = sin(dlon) * cos(lat2);
  double a2 = sin(lat1) * cos(lat2) * cos(dlon);
  a2 = cos(lat1) * sin(lat2) - a2;
  a2 = atan2(a1, a2);

  if (a2 < 0.0) a2 += TWO_PI;
  return degrees(a2);
}
/*
const char *TinyGPSPlus::cardinal (const double course)
{
  static const char* directions[] = {"N", "NNE", "NE", "ENE", "E", "ESE", "SE", "SSE", "S", "SSW", "SW", "WSW", "W", "WNW", "NW", "NNW"};
  int direction = (int)((course + 11.25) / 22.5);
  return directions[direction % 16];
}
*/
void TinyGPSLocation::cbuffClear ()
{
	cbuff.sp = -1;
	memset(cbuff.pos, 0, sizeof(cbuff.pos));
}

void TinyGPSLocation::cbuffAdd (const double lat, const double lng, const float alt)
{
	tinygps_pos_t pos = {lat, lng, alt};
	if (lat != lng && lat > 0.0 && lng)
		cbuffAdd(&pos);
	else
		cbuffClear();
}

void TinyGPSLocation::cbuffAdd (const tinygps_pos_t *pos)
{
	if (++cbuff.sp >= POSBUFF_LENGTH) cbuff.sp = POSBUFF_LENGTH-1;
	
	if (cbuff.sp == POSBUFF_LENGTH-1)
		memcpy(&cbuff.pos[0], &cbuff.pos[1], (POSBUFF_LENGTH-1)*sizeof(tinygps_pos_t)/*sizeof(cbuff.pos)-sizeof(tinygps_pos_t)*/);
	memcpy(&cbuff.pos[cbuff.sp], pos, sizeof(tinygps_pos_t));
}

int TinyGPSLocation::cbuffSum (tinygps_pos_t *pos)
{
	if (cbuff.sp < 0) return 0;
	
	int len = cbuff.sp + 1;
	if (len > POSBUFF_LENGTH) len = POSBUFF_LENGTH;
	//int ct = 0;
	memset(pos, 0, sizeof(tinygps_pos_t));
	
	struct outliner {
		int idx;
		double val;
	};
	struct outliners {
		outliner min;
		outliner max;
		union {
			double dbl;
			float flt;
		}sum;
		int ct;
	};
	
	outliners lng = {{999, 999.0},{999, -999.0}, 0.0, 0};
	outliners lat = {{999, 999.0},{999, -999.0}, 0.0, 0};
	outliners alt = {{999, 65535.0},{999, -65535.0}, 0.0, 0};
	
	for (int i = 0; i < len; i++){
		if (lng.min.val < cbuff.pos[i].longitude){
			lng.min.val = cbuff.pos[i].longitude;
			lng.min.idx = i;
		}
		if (lng.max.val > cbuff.pos[i].longitude){
			lng.max.val = cbuff.pos[i].longitude;
			lng.max.idx = i;
		}

		if (lat.min.val < cbuff.pos[i].latitude){
			lat.min.val = cbuff.pos[i].latitude;
			lat.min.idx = i;
		}
		if (lat.max.val > cbuff.pos[i].latitude){
			lat.max.val = cbuff.pos[i].latitude;
			lat.max.idx = i;
		}

		if (alt.min.val < cbuff.pos[i].altitude){
			alt.min.val = cbuff.pos[i].altitude;
			alt.min.idx = i;
		}
		if (alt.max.val > cbuff.pos[i].altitude){
			alt.max.val = cbuff.pos[i].altitude;
			alt.max.idx = i;
		}
	}


	for (int i = 0; i < len; i++){
		if (i != lng.min.idx && i != lng.max.idx){
			lng.sum.dbl += cbuff.pos[i].longitude;
			lng.ct++;
		}
	}
	if (lng.ct)
		pos->longitude = lng.sum.dbl / (double)lng.ct;
	else
		pos->longitude = cbuff.pos[0].longitude;


	for (int i = 0; i < len; i++){
		if (i != lat.min.idx && i != lat.max.idx){
			lat.sum.dbl += cbuff.pos[i].latitude;
			lat.ct++;
		}
	}
	if (lat.ct)
		pos->latitude = lat.sum.dbl / (double)lat.ct;
	else
		pos->latitude = cbuff.pos[0].latitude;


	for (int i = 0; i < len; i++){
		if (i != alt.min.idx && i != alt.max.idx){
			alt.sum.flt += cbuff.pos[i].altitude;
			alt.ct++;
		}
	}
	if (alt.ct)
		pos->altitude = alt.sum.flt / (float)alt.ct;
	else
		pos->altitude = cbuff.pos[0].altitude;
	return len;
}

void TinyGPSLocation::commit ()
{
   rawLatData = rawNewLatData;
   rawLngData = rawNewLngData;
   //lastCommitTime = millis();
   valid = updated = true;
}

void TinyGPSLocation::setLatitude (const char *term)
{
	TinyGPSPlus::parseDegrees(term, rawNewLatData);
}

void TinyGPSLocation::setLongitude (const char *term)
{
	TinyGPSPlus::parseDegrees(term, rawNewLngData);
}

double TinyGPSLocation::lat ()
{
   updated = false;
   double ret = rawLatData.deg + rawLatData.billionths / 1000000000.0;
   return rawLatData.negative ? -ret : ret;
}

double TinyGPSLocation::lng ()
{
   updated = false;
   double ret = rawLngData.deg + rawLngData.billionths / 1000000000.0;
   return rawLngData.negative ? -ret : ret;
}

void TinyGPSDate::commit ()
{
   date = newDate;
   //lastCommitTime = millis();
   valid = updated = true;
}

void TinyGPSDate::setDate (const char *term)
{
   newDate = atol(term);
}

uint16_t TinyGPSDate::year()
{
   updated = false;
   uint16_t year = date % 100;
   return year + 2000;
}

uint8_t TinyGPSDate::month()
{
   updated = false;
   return (date / 100) % 100;
}

uint8_t TinyGPSDate::day()
{
   updated = false;
   return date / 10000;
}

void TinyGPSTime::setTime (const char *term)
{
	newTime = (uint32_t)TinyGPSPlus::parseDecimal(term);
}

void TinyGPSTime::commit ()
{
	timeAlt = time = newTime;

#if _BST_1
	timeAlt += 1000000;	// add one hour for BST
	if (timeAlt >= 24000000) timeAlt -= 24000000;
#endif

	//lastCommitTime = millis();
	valid = updated = true;
}

uint8_t TinyGPSTime::hour()
{
   updated = false;
   int hour = (timeAlt / 1000000);
   return hour;
}

uint8_t TinyGPSTime::minute()
{
   updated = false;
   return (timeAlt / 10000) % 100;
}

uint8_t TinyGPSTime::second()
{
   updated = false;
   return (timeAlt / 100) % 100;
}

uint8_t TinyGPSTime::centisecond()
{
   updated = false;
   return timeAlt % 100;
}

void TinyGPSDecimal::commit()
{
   val = newval;
   //lastCommitTime = millis();
   valid = updated = true;
}

void TinyGPSDecimal::set (const char *term)
{
   newval = TinyGPSPlus::parseDecimal(term);
}

void TinyGPSInteger::commit ()
{
   val = newval;
   //lastCommitTime = millis();
   valid = updated = true;
}

void TinyGPSInteger::set(const char *term)
{
   newval = atol(term);
}


