
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


#ifndef _LOG_H_
#define _LOG_H_

#pragma GCC optimize ("-O2")


#include "Arduino.h"
#include "logformat.h"




#define LOG_HEADER_LENGTH			(sizeof(log_header_t))
#define LOG_RDBUFF_LENGTH			(128+64)


typedef struct {
	SdFat sd;
	SdFile file;			// currently open file, there can only be one
	//uint32_t cardSize;		// size in KBytes
	int openTryCt;
	int isOpen;
	bool updated;
	int logsync;
	int16_t fileIndex;
	char filename[LOG_FILENAME_MAXLENGTH];
}gps_log_t;


#include "bike.h"



inline void log_exit ();
inline void log_enter ();

inline int log_createFilename (gps_log_t *log, char *filename, const int bufflen);
inline int log_createFilenameNext (gps_log_t *log, char *filename, const int bufflen, int idx);
inline int log_createFilenamePrev (gps_log_t *log, char *filename, const int bufflen, int idx);
inline int log_createFilenameLast (gps_log_t *log, char *filename, const int bufflen);

inline int log_exists (gps_log_t *log, const char *filename);

inline int log_read (gps_log_t *log, void *bin, const uint32_t len);
inline int log_read (gps_log_t *log, char *str, const uint32_t buffLen);
inline int log_read_open (gps_log_t *log, const char *filename);	// open for read only, do not create, can not write

inline int log_write (gps_log_t *log, const char *str);
inline int log_write (gps_log_t *log, void *bin, const uint32_t len);
inline int log_write_pts (gps_log_t *log, gps_datapt_t *pts, const uint32_t total);
inline void log_write_end (gps_log_t *log);
inline int log_write_begin (gps_log_t *log);
inline int log_write_begin (gps_log_t *log, const size_t position);

inline int log_header_write (gps_log_t *log, log_header_t *header);
inline int log_header_read (gps_log_t *log, log_header_t *header);

inline void log_read_end (gps_log_t *log);
inline int log_read_begin (gps_log_t *log, const uint32_t offset);
inline int log_read_pts (gps_log_t *log, const uint32_t pt_readFrom, gps_datapt_t *pts, const int pt_readTotal);
inline void log_rewind (gps_log_t *log);
inline uint32_t log_seteof (gps_log_t *log);

INLINE void log_listFiles (gps_log_t *log);
inline int log_listLogs (gps_log_t *log);
inline int log_deleteLog (gps_log_t *log, const int logIdx);
inline int log_deleteLog (gps_log_t *log);

inline int32_t log_getPositionCount (gps_log_t *log);
inline int log_getTotalFiles (gps_log_t *log);
inline int log_getFilenameByIndex (gps_log_t *log, int idx, char *filename, const int len);

#include "log.cpp"



#endif


