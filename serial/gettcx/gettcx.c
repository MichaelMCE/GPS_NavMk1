




/*

a) connects to Due on serial port n then downloads whatever the active route is (nnnnnn.dat file), exports as .tcx/.gpx
b) convert .dat to .tcx/.gpx by passing .dat through the commandline

gcc s.c -Wall -O2 -std=gnu11 -s

*/


#define WINVER 0x0700

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <wchar.h>
#include <inttypes.h>
#include <math.h>
#include <windows.h>

#include "../../src/logformat.h"


#define BUILD_OUT					(1)			/* 1==output .tcx, 0==output .gpx */


#define DEVICE_NAME					"Edge 510"
#define DEVICE_PID					"1720"
#define DEVICE_UID					"7825166144"

#define TCX_ACTIVITY_SPORT			"Biking"
#define TCX_LOG_CMD					"dumpraw"

#define TCX_STRING(x)				printf(x)
#define TCX_STRING_N(x)				printf(x"\n")
#define TCX_STRING_I(x)				printf("%i",x)
#define TCX_STRING_F(x)				printf("%.5f",x)
#define TCX_STRING_D(x)				printf("%.8lf",x)
#define TCX_STRING_64(x)			printf("%I64d",x)

#define LOG_HEADER_LENGTH			(sizeof(log_header_t))
#define LOG_FILENAME_MAXLENGTH		(16)

#define INITIALBUFFERLENGTH			(1*1024*1024)		// ~100k per hour, 1 meg starting length should do fine




static HANDLE hSerial = INVALID_HANDLE_VALUE;
static char dev_name[MAX_PATH] = "";
static int deviceNumber = -1;
static char *filename = NULL;
static int baudRate = 115200;
static int scanMax = 32;
static int scanMin = 2;



static inline char *memstr (const char *block, const int bsize, const char *pattern)
{
    char *where;
    char *start = (char*)block;
    int found = 0;
    
    while (!found) {
        where = (char*)memchr(start, (int)pattern[0], (size_t)bsize - (size_t)(start - block));
        if (where==NULL){
            found++;
        }else{
			if (!memcmp(where, pattern, strlen(pattern)))
                found++;
        }
        start = where+1;
    }
    return where;
}

static inline void serialClose ()
{
    if (hSerial != INVALID_HANDLE_VALUE){
        CloseHandle(hSerial);
        hSerial = INVALID_HANDLE_VALUE;
    }
}
 
static inline void exit_message (const char* error_message, int error)
{
    fprintf(stderr, error_message);
    fprintf(stderr, "\n");
 
    exit(error);
}

static inline int serialOpen (int port, const int baud)
{
 
 	if (port < 1){
        scanMax = port;
        scanMin = port;
 
    	for (int n = scanMax; n >= scanMin; --n){
        	sprintf(dev_name, "\\\\.\\COM%d", n);
        	hSerial = CreateFile(dev_name, GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
        	if (hSerial != INVALID_HANDLE_VALUE){
        		port = n;
            	break;
            }
    	}
 	}else{
		sprintf(dev_name, "\\\\.\\COM%d", port);
		hSerial = CreateFile(dev_name, GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
		if (hSerial == INVALID_HANDLE_VALUE)
			return 0;
	}

	if (port < 1) return 0;
	   
	DCB dcb;
 	GetCommState(hSerial, &dcb);
	dcb.fRtsControl = RTS_CONTROL_ENABLE;
	dcb.fDtrControl = DTR_CONTROL_ENABLE;
	dcb.fOutX = dcb.fInX = FALSE;
	dcb.fOutxCtsFlow = FALSE;
	dcb.fOutxDsrFlow = 0;
	dcb.BaudRate = baud;
	dcb.ByteSize = 8;
	dcb.Parity = NOPARITY;
	dcb.StopBits = ONESTOPBIT;
	SetCommState(hSerial, &dcb);
 
	COMMTIMEOUTS CommTimeouts;
 	GetCommTimeouts(hSerial, &CommTimeouts);
	CommTimeouts.WriteTotalTimeoutConstant = 5;
	CommTimeouts.WriteTotalTimeoutMultiplier = 1;
	CommTimeouts.ReadTotalTimeoutConstant = 100;
	CommTimeouts.ReadTotalTimeoutMultiplier = 0;
	CommTimeouts.ReadIntervalTimeout = -1;
	SetCommTimeouts(hSerial, &CommTimeouts);

	unsigned long comError = 0;
	COMSTAT comstat;
	PurgeComm(hSerial, PURGE_RXCLEAR | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_TXABORT);
 	ClearCommError(hSerial, &comError, &comstat);
 	
 	return 1;
}

static inline int serialSendString (const char *str)
{
	size_t len = strlen(str);
	size_t bytesRead = 0;
		
	WriteFile(hSerial, str, (DWORD)len, (DWORD*)&bytesRead, NULL);
	
	return (bytesRead == len);
}

static inline int serialSendCmd (const char *cmd)
{
	char str[strlen(cmd)+4];
	snprintf(str, sizeof(str), "%s\r\n", cmd);
	
	return serialSendString(str);
}

static inline char *serialReadResponse (size_t bufferSize, const int allocDelta, size_t *len)
{
	*len = 0;
	size_t bytesRead = 0;
	size_t bytesReadTotal = 0;
	
	char *buffer = malloc(bufferSize);
	if (!buffer) return 0;
		

	do{
		bytesRead = 0;
		if (!ReadFile(hSerial, &buffer[bytesReadTotal], bufferSize-bytesReadTotal, (LPDWORD)&bytesRead, NULL))
			break;
			
		bytesReadTotal += bytesRead;
		//printf("%i %i %i '%c'\n", bytesRead, bytesReadTotal, bufferSize, *buffer);
		if (!bytesRead && bytesReadTotal){
			*len = bytesReadTotal;
			return buffer;
		}
		if (bytesReadTotal >= bufferSize-32){
			bufferSize += allocDelta;
			buffer = realloc(buffer, bufferSize);
			if (!buffer) return 0;
		}
	}while(1);

	free(buffer);
	return NULL;
}

static inline char *timeToString (uint32_t time, char *str, const int len)
{
	int hr = time / 1000000;
	
	
	//hr++;
	
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

	int centi = time % 100;

	snprintf(str, len-1, "%.2i:%.2i:%.2i.%.2i0Z", hr, min, sec, centi);
	//snprintf(str, len-1, "%.2i:%.2i:%.2iZ", hr, min, sec);
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

#if 0


#define sq(x) ((x)*(x))
#define radians(x) ((x)*0.01745329251994329)

static inline double distanceBetween (double lat1, double long1, double lat2, double long2)
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
  
  return (double)delta * (double)6372795.0;
  
 /*   double delta = radians(long1-long2);
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
  
  return delta * 6372795.0;*/
}

#else

static inline double distanceBetween (double lat1, double lon1, double lat2, double lon2)
{
	
	//printf("%f %f %f %f\n", lat1, lon1, lat2, lon2);
	
	const double R = 6378137.0;		// Earths radius
	const double pi80 = M_PI / 180.0;

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

static inline uint64_t distanceBetween64 (double lat1, double lon1, double lat2, double lon2)
{
	
	//printf("%f %f %f %f\n", lat1, lon1, lat2, lon2);
	
	const uint64_t R = (uint64_t)63781370000000;		// Earths radius
	const double pi80 = M_PI / 180.0;

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
	uint64_t d = (R * c);
	//printf("dist %.0f\n", d);

	return d;
}


#endif

static inline double calcDistance (const gps_pos_t *p1, const gps_pos_t *p2)
{
	return distanceBetween(p1->latitude, p1->longitude, p2->latitude, p2->longitude) * 1.0193;
}
/*
static inline uint64_t calcDistance64 (const gps_pos_t *p1, const gps_pos_t *p2)
{
	return distanceBetween64(p1->latitude, p1->longitude, p2->latitude, p2->longitude);
}
*/
static inline void dumpGPX (log_header_t *header, gps_datapt_t *pts, const int tPts)
{
	//gps_datapt_t *pt = track->log;

	char time[24];
	char date[24];
	
	snprintf(date, sizeof(date)-1, "%sT", header->date);
	
	
	TCX_STRING_N("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
	TCX_STRING_N("<gpx creator=\""DEVICE_NAME"\" version=\"1.1\" xmlns=\""
					"http://www.topografix.com/GPX/1/1\" xmlns:xsi=\""
					"http://www.w3.org/2001/XMLSchema-instance\" xsi:schemaLocation=\""
					"http://www.topografix.com/GPX/1/1 "
					"http://www.topografix.com/GPX/1/1/gpx.xsd "
					"http://www.garmin.com/xmlschemas/GpxExtensions/v3 "
					"http://www.garmin.com/xmlschemas/GpxExtensionsv3.xsd "
					"http://www.garmin.com/xmlschemas/TrackPointExtension/v1 "
					"http://www.garmin.com/xmlschemas/TrackPointExtensionv1.xsd\" xmlns:gpxtpx=\""
					"http://www.garmin.com/xmlschemas/TrackPointExtension/v1\" xmlns:gpxx=\""
					"http://www.garmin.com/xmlschemas/GpxExtensions/v3\">");
	
	TCX_STRING_N(" <metadata>");
	TCX_STRING  ("  <time>");
	TCX_STRING(date);
	TCX_STRING(header->time);
	TCX_STRING_N("</time>");
	TCX_STRING_N(" </metadata>");
	
	TCX_STRING_N(" <trk>");
	//TCX_STRING_N("  <number>1</number>");

	TCX_STRING  ("  <name>");
	TCX_STRING(date);
	TCX_STRING(header->time);
	TCX_STRING_N("</name>");
	
	TCX_STRING_N("  <trkseg>");



	gps_datapt_t *pt = pts;
	
	for (int i = 0; i < tPts; i++, pt++){
		if (!(int)pt->pos.latitude && !(int)pt->pos.longitude){
			continue;
		}

		TCX_STRING  ("   <trkpt lat=\"");
		TCX_STRING_D(pt->pos.latitude);
		TCX_STRING  ("\" lon=\"");
		TCX_STRING_D(pt->pos.longitude);
		TCX_STRING_N("\">");

		TCX_STRING("    <ele>");
		TCX_STRING_F(pt->altitude);
		TCX_STRING_N("</ele>");
		
		TCX_STRING("    <time>");
		TCX_STRING(date);
		TCX_STRING(timeToString(pt->time, time, sizeof(time)));
		TCX_STRING_N("</time>");

#if 0
		TCX_STRING_N("    <extensions>");
		TCX_STRING_N("     <gpxtpx:TrackPointExtension>");
		//TCX_STRING_N("      <gpxtpx:atemp>12</gpxtpx:atemp>");
		TCX_STRING("      <gpxtpx:hr>");
		TCX_STRING_F((float)(i%170));
		TCX_STRING_N("</gpxtpx:hr>");
		//TCX_STRING_N("      <gpxtpx:cad>100</gpxtpx:cad>");
		TCX_STRING_N("     </gpxtpx:TrackPointExtension>");
		TCX_STRING_N("    </extensions>");
#endif		
		TCX_STRING_N("   </trkpt>");
	}


	TCX_STRING_N("  </trkseg>");
	TCX_STRING_N(" </trk>");
	TCX_STRING_N("</gpx>");
	
}

static inline void dumpTCX (log_header_t *header, gps_datapt_t *pts, const int tPts)
{
	//gps_datapt_t *pt = track->log;

	char time[24];
	char date[24];
	snprintf(date, sizeof(date)-1, "%sT", header->date);
	
	TCX_STRING_N("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>");
	TCX_STRING_N("<TrainingCenterDatabase xmlns=\"http://www.garmin.com/xmlschemas/"
				 "TrainingCenterDatabase/v2\" xmlns:xsi=\"http://www.w3.org/2001/XM"
				 "LSchema-instance\" xsi:schemaLocation=\"http://www.garmin.com/xml"
				 "schemas/TrainingCenterDatabase/v2 http://www.garmin.com/xmlschema"
				 "s/TrainingCenterDatabasev2.xsd\">");
	
	TCX_STRING_N("<Activities>");
	TCX_STRING_N("  <Activity Sport=\"" TCX_ACTIVITY_SPORT "\">");
 
	TCX_STRING("    <Id>");
	TCX_STRING(date);
	TCX_STRING(header->time);
	TCX_STRING_N("</Id>");
	
	TCX_STRING("    <Lap StartTime=\"");
	TCX_STRING(date); TCX_STRING(header->time);
	TCX_STRING_N("\">");

	//int32_t totalSeconds = (track->recordTimeEnd - track->recordTimeStart) / 1000;
	//TCX_STRING("      <TotalTimeSeconds>");
	//TCX_STRING(header->totalSeconds);
	//TCX_STRING_N("</TotalTimeSeconds>");
	
	TCX_STRING("      <MaximumSpeed>");
	TCX_STRING(header->speedMax);
	//TCX_STRING("11.8861113");
	TCX_STRING_N("</MaximumSpeed>");
	
	TCX_STRING_N("      <Intensity>Active</Intensity>");
	TCX_STRING_N("      <TriggerMethod>Manual</TriggerMethod>");

	TCX_STRING_N("      <Track>");
	
	int totalSeconds = 0;
	double distance = 0.0000000;
	//uint64_t dis64 = 0;
	int skip = 0;


	gps_datapt_t *pt = pts;
	gps_datapt_t *ptPre = pt;
	
	for (int i = 0; i < tPts; i+=2, pt+=2){

		// try to remove invalid points
		if (!(int)pt->pos.latitude && !(int)pt->pos.longitude){
			skip = 1;
			continue;
		}
		if (i && !skip){
			if (calcDistance(&ptPre->pos, &pt->pos) > 1500.0){
				continue;
			}
			
			
			// try to remove invalid timestamps
			int32_t dt = pt->time - ptPre->time;
			if (dt < -10000){
				skip = 1;
				continue;
			}
			//printf("%i: %i %i\n", i, pt->time, dt);
			
		}


		TCX_STRING_N("		<Trackpoint>");
		
		TCX_STRING("		  <Time>");
		TCX_STRING(date);
		TCX_STRING(timeToString(pt->time, time, sizeof(time)));
		TCX_STRING_N("</Time>");

		TCX_STRING_N("		  <Position>");
		TCX_STRING  ("			<LatitudeDegrees>"); TCX_STRING_D(pt->pos.latitude); TCX_STRING_N("</LatitudeDegrees>");
		TCX_STRING  ("			<LongitudeDegrees>"); TCX_STRING_D(pt->pos.longitude); TCX_STRING_N("</LongitudeDegrees>");
		TCX_STRING_N("		  </Position>");

		TCX_STRING("		  <AltitudeMeters>");
		TCX_STRING_F(pt->altitude);
		TCX_STRING_N("</AltitudeMeters>");
		
		TCX_STRING("		  <DistanceMeters>");
		if (!i || skip){
			skip = 0;
			TCX_STRING("0.0000000");
		}else{
			distance += calcDistance(&ptPre->pos, &pt->pos);
			//dis64 += calcDistance64(&ptPre->pos, &pt->pos);
			TCX_STRING_D(distance);
			//TCX_STRING_64(dis64/10000000);TCX_STRING("."); printf("%i", (int)(dis64%10000000));
		}	
		TCX_STRING_N("</DistanceMeters>");
		
		//TCX_STRING_N("		  <Cadence>0</Cadence>");
        TCX_STRING_N("		  <SensorState>Absent</SensorState>");
		
		TCX_STRING_N("		</Trackpoint>");
		
		ptPre = pt;
		totalSeconds++;
	}
	
	TCX_STRING_N("      </Track>");

	TCX_STRING("      <TotalTimeSeconds>");
	TCX_STRING_I(totalSeconds*2);
	TCX_STRING_N("</TotalTimeSeconds>");
	
	TCX_STRING("      <DistanceMeters>");
	TCX_STRING_D(distance);
	TCX_STRING_N("</DistanceMeters>");

	/*TCX_STRING("      <DistanceMeters64>");
	TCX_STRING_64(dis64/10000000);TCX_STRING("."); printf("%i", (int)(dis64%10000000));
	TCX_STRING_N("</DistanceMeters64>");*/

	
	TCX_STRING_N("    </Lap>");
	
	TCX_STRING_N("    <Creator xsi:type=\"Device_t\">");
	TCX_STRING_N("      <Name>"DEVICE_NAME"</Name>");
	TCX_STRING_N("      <UnitId>"DEVICE_UID"</UnitId>");
	TCX_STRING_N("      <ProductID>"DEVICE_PID"</ProductID>");
	TCX_STRING_N("      <Version>");
	TCX_STRING_N("        <VersionMajor>2</VersionMajor>");
	TCX_STRING_N("        <VersionMinor>40</VersionMinor>");
	TCX_STRING_N("        <BuildMajor>0</BuildMajor>");
	TCX_STRING_N("        <BuildMinor>0</BuildMinor>");
	TCX_STRING_N("      </Version>");
	TCX_STRING_N("    </Creator>");
	
	
	
	TCX_STRING_N("  </Activity>");
	TCX_STRING_N("</Activities>");
	TCX_STRING_N("</TrainingCenterDatabase>");

}

static inline void process (char *buffer, const size_t len/*, const int mode*/)
{
	char *start = memstr(buffer, len, "<start>\r\n");
	if (!start){
		printf("stream start marker not found\n");
		return;
	}

	*start = 0;		// up until this point header is plain asci
	char *end = memstr(start, len-(start-buffer), "\r\n<end>");
	if (!end){
		printf("stream end marker not found\n");
		return;
	}
	
	log_header_t *header = (log_header_t*)(start - LOG_HEADER_LENGTH);
	if ((void*)header < (void*)buffer){
		printf("header not found within stream %p %p\n", header, buffer);
		return;
	}
	
	/*printf("\n");
	printf("'%s'\n", header->filename);
	printf("'%s'\n", header->date);
	printf("'%s'\n", header->time);
	printf("'%s'\n", header->timeStart);
	printf("'%s'\n", header->timeEnd);
	printf("'%s'\n", header->totalSeconds);
	
	
	int tstart = atoi(header->timeStart);
	int tend = atoi(header->timeEnd);
	
	int duration = (tend - tstart) / 1000;
	int second = duration % 60;
	int minute = (duration / 60) % 60;
	int hour = (duration / 60) / 60;
	
	printf("%i %i %i %i\n", duration, hour, minute, second);*/
	
	start += strlen("<start>\r\n");
	int tPts = (end - start) / sizeof(gps_datapt_t);
	gps_datapt_t *pt = (gps_datapt_t*)start;
	
	/*int timeStart = pt->time;
	for (int i = 0; i < 10 && i < tPts; i++, pt++){
		int t = (pt->time - timeStart) / 100;
		printf("%i: %.6f %.6f %.2f %u\n", t, pt->pos.longitude, pt->pos.latitude, pt->altitude, pt->time);
	}*/
	
#if (BUILD_OUT)	
	dumpTCX(header, (gps_datapt_t*)pt, tPts);
#else
	dumpGPX(header, (gps_datapt_t*)pt, tPts);
#endif
}


int processFromSerial (const int deviceNumber, const int baudRate)
{
    atexit(serialClose);
 
	if (!serialOpen(deviceNumber, baudRate))
		return 0;

	if (serialSendCmd(TCX_LOG_CMD)){
		size_t len;
		char *buffer = serialReadResponse(INITIALBUFFERLENGTH, 8192, &len);
		serialClose();
		if (buffer){
			if (len) process(buffer, len);
			free(buffer);
		}
	}

	return 0;
}

static uint64_t lof (FILE *stream)
{
	fpos_t pos;
	fgetpos(stream, &pos);
	fseek(stream,0, SEEK_END);
	uint64_t fl = ftell(stream);
	fsetpos(stream, &pos);
	return fl;
}

int processFromFile (const char *filename)
{

	uint64_t len = 0;
	
	FILE *file = fopen(filename, "r+b");
	if (file){
		len = lof(file);
		if (len > 90){
			char *buffer = calloc(len+90, sizeof(char));
			if (buffer){
				int dataRead = (int)fread(buffer, 1, len, file);
				if (dataRead == len){
					const int tPts = (len - LOG_HEADER_LENGTH) / sizeof(gps_datapt_t);
					dumpTCX((log_header_t*)buffer, (gps_datapt_t*)(buffer+LOG_HEADER_LENGTH), tPts);
				}
				free(buffer);
			}
		}
		fflush(stdout);
		fclose(file);
	}	
	return len > 0;
}

int main (int argc, char *argv[])
{
    int n = 1;
    while (n < argc){
        if (strcmp(argv[n], "-file") == 0){
        	if (++n >= argc)	
            	exit_message("filename not specified", 1);
            filename = (char*)argv[n];
            if (!filename)
            	exit_message("invalid filename", 1);

        }else if (strcmp(argv[n], "-port") == 0){
            if (++n >= argc)	
            	exit_message("no device number specified", 1);
            deviceNumber = atoi(argv[n]);
            
        	if (strcmp(argv[n], "-baud") == 0){
            	if (++n >= argc)
	            	exit_message("baud rate not specified", 1);
            	baudRate = atoi(argv[n]);
            }
		}else{ 
		//	printf("unrecognised option: %s\n", argv[n]);
        }
        n++;
    }
    
    if (filename)
    	processFromFile(filename);
    else if (deviceNumber >= 0 && baudRate)
    	processFromSerial(deviceNumber, baudRate);
    else
		exit_message("nothing to do\n", 1);
		
	return 0;
}

