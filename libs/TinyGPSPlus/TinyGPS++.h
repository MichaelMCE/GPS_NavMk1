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

#ifndef __TinyGPSPlus_h
#define __TinyGPSPlus_h

#pragma GCC optimize ("-O3")


#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#include <limits.h>



#define POSBUFF_LENGTH				18



#define SAT_MAX						56
#define _GPS_VERSION				"MME_0.92" // software version of this library
#define _GPS_MPH_PER_KNOT			1.15077945
#define _GPS_MPS_PER_KNOT			0.51444444
#define _GPS_KMPH_PER_KNOT			1.852
#define _GPS_MILES_PER_METER		0.00062137112
#define _GPS_KM_PER_METER			0.001
#define _GPS_FEET_PER_METER			3.2808399
#define _GPS_MAX_FIELD_SIZE			21


enum {
	GPS_SENTENCE_GPRMC,
	GPS_SENTENCE_GPGGA,
	GPS_SENTENCE_GPGSV,
	GPS_SENTENCE_GPGSA,
	GPS_SENTENCE_GPGBS,
	GPS_SENTENCE_GPGST,
	GPS_SENTENCE_PUBX,
	GPS_SENTENCE_OTHER
};



struct RawDegrees
{
   uint16_t deg;
   uint32_t billionths;
   bool negative;
public:
   RawDegrees() : deg(0), billionths(0), negative(false)
   {}
};

typedef struct {
	double latitude;
	double longitude;
	float altitude;
}tinygps_pos_t;


struct TinyGPSLocation
{
   friend class TinyGPSPlus;
public:
   inline bool isValid() const			{ return valid; }
   inline void invalididate()			{ valid = false; }
   inline bool isUpdated() const		{ return updated; }
   //inline uint32_t age() const			{ return valid ? millis() - lastCommitTime : (uint32_t)ULONG_MAX; }
   inline const RawDegrees &rawLat()    { updated = false; return rawLatData; }
   inline const RawDegrees &rawLng()    { updated = false; return rawLngData; }

   double lat();
   double lng();
   
   bool valid;
   int cbuffSum (tinygps_pos_t *pos);
   void cbuffClear ();

   TinyGPSLocation() : valid(false), updated(false)
   {}

private:
   bool updated;
   RawDegrees rawLatData, rawLngData, rawNewLatData, rawNewLngData;
   //uint32_t lastCommitTime;
   void commit();
   void setLatitude (const char *term);
   void setLongitude (const char *term);

	struct {
		int sp;
		tinygps_pos_t pos[POSBUFF_LENGTH+1];
	}cbuff;
	
	void cbuffAdd (const double lat, const double lng, const float alt);
	void cbuffAdd (const tinygps_pos_t *pos);

};

struct TinyGPSDate
{
   friend class TinyGPSPlus;
public:
   bool isValid() const       { return valid; }
   bool isUpdated() const     { return updated; }
   //uint32_t age() const       { return valid ? millis() - lastCommitTime : (uint32_t)ULONG_MAX; }

   uint32_t value()           { updated = false; return date; }
   uint16_t year();
   uint8_t month();
   uint8_t day();

   TinyGPSDate() : valid(false), updated(false), date(0)
   {}

private:
   bool valid, updated;
   uint32_t date, newDate;
   //uint32_t lastCommitTime;
   void commit();
   void setDate(const char *term);
};

struct TinyGPSTime
{
   friend class TinyGPSPlus;
public:
   bool isValid() const       { return valid; }
   bool isUpdated() const     { return updated; }
   //uint32_t age() const       { return valid ? millis() - lastCommitTime : (uint32_t)ULONG_MAX; }

   uint32_t UTC()   	      { updated = false; return time; }
   uint32_t alt()			  { updated = false; return timeAlt; }
   
   uint8_t hour();
   uint8_t minute();
   uint8_t second();
   uint8_t centisecond();

   TinyGPSTime() : valid(false), updated(false), time(0)
   {}

private:
   bool valid, updated;
   uint32_t newTime = 0;
   uint32_t time = 0, timeAlt = 0;
   //uint32_t lastCommitTime = 0;

   void commit ();
   void setTime (const char *term);
};

struct TinyGPSDecimal
{
   friend class TinyGPSPlus;
public:
   bool isValid() const    { return valid; }
   bool isUpdated() const  { return updated; }
   //uint32_t age() const    { return valid ? millis() - lastCommitTime : (uint32_t)ULONG_MAX; }
   int32_t value()         { updated = false; return val; }

   TinyGPSDecimal() : valid(false), updated(false), val(0)
   {}

private:
   bool valid, updated;
   //uint32_t lastCommitTime;
   int32_t val, newval;
   void commit();
   void set (const char *term);
};

struct TinyGPSInteger
{
   friend class TinyGPSPlus;
public:
   bool isValid() const    { return valid; }
   bool isUpdated() const  { return updated; }
   //uint32_t age() const    { return valid ? millis() - lastCommitTime : (uint32_t)ULONG_MAX; }
   uint32_t value()        { updated = false; return val; }

   TinyGPSInteger() : valid(false), updated(false), val(0)
   {}

private:
   bool valid, updated;
   //uint32_t lastCommitTime;
   uint32_t val, newval;
   void commit();
   void set(const char *term);
};

struct TinyGPSSpeed : TinyGPSDecimal
{
	double km;
	
	double knots()    { return value() / 100.0; }
	double mph()      { return _GPS_MPH_PER_KNOT * value() / 100.0; }
	double mps()      { return _GPS_MPS_PER_KNOT * value() / 100.0; }
	double kmph()     { return _GPS_KMPH_PER_KNOT * value() / 100.0; }
};

struct TinyGPSCourse : public TinyGPSDecimal
{
   double deg()      { return value() / 100.0; }
};

struct TinyGPSAltitude : TinyGPSDecimal
{
   double meters()       { return value() / 100.0; }
   double miles()        { return _GPS_MILES_PER_METER * value() / 100.0; }
   double kilometers()   { return _GPS_KM_PER_METER * value() / 100.0; }
   double feet()         { return _GPS_FEET_PER_METER * value() / 100.0; }
};

struct TinyGPSSatalitesSignal
{
	uint16_t id;
	uint16_t elevation;		// maximum is 90
	uint16_t azimuth;		// Range:0-359
	uint16_t cnr;			// Range: 0-99; Null means there is no positioning
	uint32_t age;			// when last visable
};

struct TinyGPSSatalites
{
	uint8_t visable;		// number of sats visable
	TinyGPSInteger withFix;	// number of sats with a lock
	TinyGPSSatalitesSignal working;
	TinyGPSSatalitesSignal sats[SAT_MAX];
};

struct TinyGPSFix
{
	uint8_t mode;		// 'A' or 'M'
	uint8_t type;		// 1:Fix not available, 2:2D fix, 3:3D fix  (navMode)
	//uint8_t navStat;
	uint8_t numSv;

	double hAcc;
	double vAcc;
	
	inline uint8_t getFix() const { return type; }
	inline uint8_t getMode() const { return mode; }
};

struct TinyGPSDop
{
	double pos;
	double hori;
	double vert;
	double alt;
};

struct TinyGPSRangeErrorStatistics
{
	double rangeRMS;
	double stdMajor;
	double stdMinor;
	double orient;
	double stdLat;
	double stdLong;
	double stdAlt;
	
	double errLat;
	double errLong;
	double errAlt;
	
};

class TinyGPSPlus
{
public:
  TinyGPSPlus();

  bool encode (const char c); // process one character received from GPS
  int encode (const char *buffer, const int len);
  
  TinyGPSPlus &operator << (char c) {encode(c); return *this;}

  TinyGPSLocation location;
  TinyGPSDate date;
  TinyGPSTime time;
  TinyGPSSpeed speed;
  TinyGPSCourse course;
  TinyGPSAltitude altitude;
  TinyGPSSatalites satellites;
  TinyGPSFix fix;
  TinyGPSDop dop;
  TinyGPSRangeErrorStatistics estats;


  static const char *libraryVersion() { return _GPS_VERSION; }

  static double distanceBetween (double lat1, double long1, double lat2, double long2);
  static double courseTo (double lat1, double long1, double lat2, double long2);
  static const char *cardinal (const double course);

  static double parseFloat (const char *term);
  static double parseFloatb (const char *term);
  static int32_t parseDecimal(const char *term);
  static void parseDegrees(const char *term, RawDegrees &deg);
  
  uint32_t charsProcessed()   const { return encodedCharCount; }
  uint32_t sentencesWithFix() const { return sentencesWithFixCount; }
  uint32_t failedChecksum()   const { return failedChecksumCount; }
  uint32_t passedChecksum()   const { return passedChecksumCount; }
  
  //uint16_t getSentenceCount (unsigned char nemaMsgType)   const { return sentenceCount[nemaMsgType]; }
  
  uint32_t encodedCharCount;
  
  
  inline TinyGPSSatalitesSignal *getSatalites ()
  {
  	return satellites.sats;
  }

  inline int getSatalite (TinyGPSSatalitesSignal *sat, const int idx)
  {
  	if (idx < SAT_MAX){
  		//memcpy(sat, &satellites.sats[idx], sizeof(TinyGPSSatalitesSignal));
  		*sat = satellites.sats[idx];
  		return 1;
  	}
  	return 0;
  }


  
private:

  // parsing state variables
  uint8_t parity;
  bool isChecksumTerm;
  char term[_GPS_MAX_FIELD_SIZE + 1];
  uint8_t curSentenceType;
  uint8_t curTermNumber;
  uint8_t curTermOffset;
  bool sentenceHasFix;
  bool pubx_00;

  // statistics
  //uint16_t sentenceCount[8];		// GPS_SENTENCE_GPGGA, etc..
  //uint32_t encodedCharCount;
  uint32_t sentencesWithFixCount;
  uint32_t failedChecksumCount;
  uint32_t passedChecksumCount;

  // internal utilities
  inline int fromHex (const uint8_t a);
  bool endOfTermHandler();
};

#endif // def(__TinyGPSPlus_h)



/*
public class KalmanLatLong {
    private final float MinAccuracy = 1;

    private float Q_metres_per_second;    
    private long TimeStamp_milliseconds;
    private double lat;
    private double lng;
    private float variance; // P matrix.  Negative means object uninitialised.  NB: units irrelevant, as long as same units used throughout

    public KalmanLatLong(float Q_metres_per_second) { this.Q_metres_per_second = Q_metres_per_second; variance = -1; }

    public long get_TimeStamp() { return TimeStamp_milliseconds; }
    public double get_lat() { return lat; }
    public double get_lng() { return lng; }
    public float get_accuracy() { return (float)Math.sqrt(variance); }

    public void SetState(double lat, double lng, float accuracy, long TimeStamp_milliseconds) {
        this.lat=lat; this.lng=lng; variance = accuracy * accuracy; this.TimeStamp_milliseconds=TimeStamp_milliseconds;
    }

    /// <summary>
    /// Kalman filter processing for lattitude and longitude
    /// </summary>
    /// <param name="lat_measurement_degrees">new measurement of lattidude</param>
    /// <param name="lng_measurement">new measurement of longitude</param>
    /// <param name="accuracy">measurement of 1 standard deviation error in metres</param>
    /// <param name="TimeStamp_milliseconds">time of measurement</param>
    /// <returns>new state</returns>
    public void Process(double lat_measurement, double lng_measurement, float accuracy, long TimeStamp_milliseconds) {
        if (accuracy < MinAccuracy) accuracy = MinAccuracy;
        if (variance < 0) {
            // if variance < 0, object is unitialised, so initialise with current values
            this.TimeStamp_milliseconds = TimeStamp_milliseconds;
            lat=lat_measurement; lng = lng_measurement; variance = accuracy*accuracy; 
        } else {
            // else apply Kalman filter methodology

            long TimeInc_milliseconds = TimeStamp_milliseconds - this.TimeStamp_milliseconds;
            if (TimeInc_milliseconds > 0) {
                // time has moved on, so the uncertainty in the current position increases
                variance += TimeInc_milliseconds * Q_metres_per_second * Q_metres_per_second / 1000;
                this.TimeStamp_milliseconds = TimeStamp_milliseconds;
                // TO DO: USE VELOCITY INFORMATION HERE TO GET A BETTER ESTIMATE OF CURRENT POSITION
            }

            // Kalman gain matrix K = Covarariance * Inverse(Covariance + MeasurementVariance)
            // NB: because K is dimensionless, it doesn't matter that variance has different units to lat and lng
            float K = variance / (variance + accuracy * accuracy);
            // apply K
            lat += K * (lat_measurement - lat);
            lng += K * (lng_measurement - lng);
            // new Covarariance  matrix is (IdentityMatrix - K) * Covarariance 
            variance = (1 - K) * variance;
        }
    }
}

*/
