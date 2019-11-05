



#include <shlwapi.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <time.h>
#include <math.h>
#include "fileal.h"




#include "logformat.h"



#define DEVICE_NAME					"Edge 810"
#define DEVICE_PID					"1721"
#define DEVICE_UID					"7825166143"

#define TCX_ACTIVITY_SPORT			"Biking"


#define TCX_STRING(x)				printf(x)
#define TCX_STRING_N(x)				printf(x"\n")
#define TCX_STRING_I(x)				printf("%i",x)
#define TCX_STRING_F(x)				printf("%.5f",x)
#define TCX_STRING_D(x)				printf("%.8lf",x)
#define TCX_STRING_64(x)			printf("%I64d",x)

#define LOG_HEADER_LENGTH			(sizeof(log_header_t))
#define LOG_FILENAME_MAXLENGTH		(16)





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

static inline double calcDistance (const gps_pos_t *p1, const gps_pos_t *p2)
{
	return distanceBetween(p1->latitude, p1->longitude, p2->latitude, p2->longitude);
}

static inline int parseEle (char *line, float *ele)
{
	char *alt = strstr(line, "<ele>");
	if (alt){
		alt += 5;
		*ele = atof((const char*)alt);
		//printf("%f\n", *ele);
		return 1;
	}
	return 0;
}

static inline int parseAlt (char *line, float *ele)
{
	char *alt = strstr(line, "<AltitudeMeters>");
	if (alt){
		alt += 16;
		*ele = atof((const char*)alt);
		return 1;
	}
	return 0;
}

static inline int parseTcxLine (char **line, double *latitude, double *longitude)
{
	char *lat = strstr(line[0], "<LatitudeDegrees>");
	if (lat){
		lat += 17;
		*latitude = atof((const char*)lat);
		
		char *lon = strstr(line[1],  "<LongitudeDegrees>");
		if (lon){
			lon += 18;
			*longitude = atof((const char*)lon);
			
			//printf("%f %f\n", *latitude, *longitude);
			return 1;
		}
	}
	return 0;
}

static inline int parseGpxLine (char **line, double *latitude, double *longitude)
{
	char *lat = strstr(line[0], "lat=\"");
	if (lat){
		lat += 5;
		*latitude = atof((const char*)lat);

		char *lon = strstr(line[0], "lon=\"");
		if (lon){
			lon += 5;
			*longitude = atof((const char*)lon);
			
			//printf("%f %f\n", *latitude, *longitude);
			return 1;
		}
	}
	return 0;
}


void gpx2tcx (TASCIILINE *al)
{
	//gps_datapt_t *pt = track->log;

	char time[24];
	char date[24];
	snprintf(date, sizeof(date)-1, "2016-03-03T");
	snprintf(time, sizeof(time)-1, "12:34:58Z");
	
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
	TCX_STRING("12:34:58Z");
	TCX_STRING_N("</Id>");
	
	TCX_STRING("    <Lap StartTime=\"");
	TCX_STRING(date); TCX_STRING("12:34:58Z");
	TCX_STRING_N("\">");

	//int32_t totalSeconds = (track->recordTimeEnd - track->recordTimeStart) / 1000;
	TCX_STRING("      <TotalTimeSeconds>");
	TCX_STRING_I(al->tlines);
	TCX_STRING_N("</TotalTimeSeconds>");
	
	TCX_STRING("      <MaximumSpeed>");
	TCX_STRING("5.5");
	//TCX_STRING("11.8861113");
	TCX_STRING_N("</MaximumSpeed>");
	
	TCX_STRING_N("      <Intensity>Active</Intensity>");
	TCX_STRING_N("      <TriggerMethod>Manual</TriggerMethod>");

	TCX_STRING_N("      <Track>");
	
	double distance = 0.0000000;
	//uint64_t dis64 = 0;
	int skip = 0;
	float altitude = 0.0f;
	int foundPos = 0;

	gps_datapt_t pt;
	memset(&pt, 0, sizeof(pt));
	gps_datapt_t ptPre;
	memset(&ptPre, 0, sizeof(ptPre));

	
	for (int i = 0; i < al->tlines; i++){
		if (!parseGpxLine((char**)&al->line[i], &pt.pos.latitude, &pt.pos.longitude))
			continue;
	
		if (!foundPos) skip = 1;
		foundPos = i;

		TCX_STRING_N("		<Trackpoint>");
		
		TCX_STRING("		  <Time>");
		TCX_STRING(date);
		TCX_STRING(time);
		TCX_STRING_N("</Time>");

		TCX_STRING_N("		  <Position>");
		TCX_STRING  ("			<LatitudeDegrees>"); TCX_STRING_D(pt.pos.latitude); TCX_STRING_N("</LatitudeDegrees>");
		TCX_STRING  ("			<LongitudeDegrees>"); TCX_STRING_D(pt.pos.longitude); TCX_STRING_N("</LongitudeDegrees>");
		TCX_STRING_N("		  </Position>");

		TCX_STRING("		  <AltitudeMeters>");
		if (!parseEle((char*)al->line[++i], &altitude))
			i--;
		TCX_STRING_F(altitude);
		TCX_STRING_N("</AltitudeMeters>");
		
		TCX_STRING("		  <DistanceMeters>");
		if (!foundPos || skip || !i || (pt.pos.latitude == 0.0) || (pt.pos.longitude == 0.0)){
			skip = 0;
			TCX_STRING("0.0000000");
		}else{
			distance += calcDistance(&ptPre.pos, &pt.pos);
			//dis64 += calcDistance64(&ptPre.pos, &pt.pos);
			TCX_STRING_D(distance);
			//TCX_STRING_64(dis64/10000000);TCX_STRING("."); printf("%i", (int)(dis64%10000000));
		}	
		TCX_STRING_N("</DistanceMeters>");
		
		//TCX_STRING_N("		  <Cadence>0</Cadence>");
        TCX_STRING_N("		  <SensorState>Absent</SensorState>");
		
		TCX_STRING_N("		</Trackpoint>");
		
		ptPre = pt;
	}
	
	TCX_STRING_N("      </Track>");
	
	TCX_STRING("      <DistanceMeters>");
	TCX_STRING_D(distance);
	TCX_STRING_N("</DistanceMeters>");

	//TCX_STRING("      <DistanceMeters64>");
	//TCX_STRING_64(dis64/10000000);TCX_STRING("."); printf("%i", (int)(dis64%10000000));
	//TCX_STRING_N("</DistanceMeters64>");

	
	TCX_STRING_N("    </Lap>");
	
	TCX_STRING_N("    <Creator xsi:type=\"Device_t\">");
	TCX_STRING_N("      <Name>"DEVICE_NAME"</Name>");
	TCX_STRING_N("      <UnitId>"DEVICE_UID"</UnitId>");
	TCX_STRING_N("      <ProductID>"DEVICE_PID"</ProductID>");
	/*TCX_STRING_N("      <Version>");
	TCX_STRING_N("        <VersionMajor>2</VersionMajor>");
	TCX_STRING_N("        <VersionMinor>40</VersionMinor>");
	TCX_STRING_N("        <BuildMajor>0</BuildMajor>");
	TCX_STRING_N("        <BuildMinor>0</BuildMinor>");
	TCX_STRING_N("      </Version>");*/
	TCX_STRING_N("    </Creator>");
	
	
	
	TCX_STRING_N("  </Activity>");
	TCX_STRING_N("</Activities>");
	TCX_STRING_N("</TrainingCenterDatabase>");

}

int writeBin (void *data, const size_t len, const char *path)
{
	int ret = 0;

	FILE *fp = fopen(path, "w+b");
	if (fp){
		ret = fwrite(data, len, 1, fp);
		fclose(fp);
	}

	return ret;
}


int tcx2dat (TASCIILINE *al)
{
	int count = 0;
	log_header_t header;
	memset(&header, 32, sizeof(header));
	gps_datapt_t pt;
	memset(&pt, 0, sizeof(pt));

	
	FILE *fp = fopen("out.dat", "w+b");
	if (fp){
		fseek(fp, 0, SEEK_SET);
		sprintf(header.filename, "unnamed.dat");
		sprintf(header.date, "2016-03-04");
		sprintf(header.time, "08:30:00");
		sprintf(header.timeStart, "0000");
		sprintf(header.timeEnd, "1000");
		sprintf(header.totalSeconds, "--");
		sprintf(header.speedMax, "5.5");
		fwrite(&header, sizeof(header), 1, fp);


		for (int i = 0; i < al->tlines; i++){
			if (parseTcxLine((char**)&al->line[i], &pt.pos.latitude, &pt.pos.longitude)){
				//fwrite(&pt, sizeof(pt), 1, fp);
				//count++;
				//i++;	// for tcx only
			//}else if (parseEle((char*)al->line[i], &pt.altitude)){
			}else if (parseAlt((char*)al->line[i], &pt.altitude)){
				fwrite(&pt, sizeof(pt), 1, fp);
				count++;
				i++;	// for tcx only
			}
		}

		sprintf(header.totalSeconds, "%i", count);
		fseek(fp, 0, SEEK_SET);
		fwrite(&header, sizeof(header), 1, fp);
		fclose(fp);
	}

	return count;
}

int gpx2dat (TASCIILINE *al)
{
	int count = 0;
	log_header_t header;
	memset(&header, 32, sizeof(header));
	gps_datapt_t pt;
	memset(&pt, 0, sizeof(pt));

	FILE *fp = fopen("out.dat", "w+b");
	if (fp){
		fseek(fp, 0, SEEK_SET);
		sprintf(header.filename, "unnamed.dat");
		sprintf(header.date, "2016-03-04");
		sprintf(header.time, "08:30:00");
		sprintf(header.timeStart, "0000");
		sprintf(header.timeEnd, "1000");
		sprintf(header.totalSeconds, "--");
		sprintf(header.speedMax, "5.5");
		fwrite(&header, sizeof(header), 1, fp);


		for (int i = 0; i < al->tlines; i++){
			int writePt = parseGpxLine((char**)&al->line[i], &pt.pos.latitude, &pt.pos.longitude);
			writePt += parseEle((char*)al->line[i], &pt.altitude);

			if (writePt){
				fwrite(&pt, sizeof(pt), 1, fp);
				count++;
			}
		}

		sprintf(header.totalSeconds, "%i", count);
		fseek(fp, 0, SEEK_SET);
		fwrite(&header, sizeof(header), 1, fp);
		fclose(fp);
	}

	return count;
}

int main (const int argc, const char *argv[])
{
	
	if (argc < 2) return 0;
	
	TASCIILINE *al = readFile(argv[1]);
	if (!al) return 0;

	
	/*for (int i = 0; i < al->tlines; i++){
		if (strstr(al->line[i], "<Time>"))
			continue;
		else if (strstr(al->line[i], "<DistanceMeters>"))
			continue;
		printf("%s\n", al->line[i]);
	}*/
	
	
#if 0
	gpx2tcx(al);
#elif 1
	gpx2dat(al);
#else
	int count = tcx2dat(al);
	printf("total lines %i\n", count);	
#endif
	
	freeASCIILINE(al);
	
	return 1;
}
