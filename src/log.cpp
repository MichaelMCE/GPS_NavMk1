
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

#ifndef _LOG_CPP_
#define _LOG_CPP_

#include "SdFat/SdFat.h"
#include "SdFat/SdFatUtil.h"
#include "Arduino.h"
#include "log.h"
//#include "printf.h"



inline int log_exists (gps_log_t *log, const char *filename)
{
	return log->sd.exists(filename);
}

inline int log_open (gps_log_t *log, const char *filename)
{
	if (!log->file.open(filename, O_CREAT | O_READ | O_WRITE /*| O_EXCL*/)){
		sl.println("log open failed");
		return 0;
	}
	
	//sl.println("log ready");
	return 1;
}

inline int log_open_owrite (gps_log_t *log, const char *filename)
{
	if (!log->file.open(filename, O_WRITE)){
		sl.println("log open failed");
		return 0;
	}
	
	//sl.println("log ready");
	return 1;
}

inline int log_open_oread (gps_log_t *log, const char *filename)
{
	if (!log->file.open(filename, O_READ)){
		sl.println("log open failed");
		return 0;
	}
	
	//sl.println("log ready");
	return 1;
}

inline void log_close (gps_log_t *log)
{
	log->file.close();
}

inline uint32_t log_freeSpace (gps_log_t *log)
{
	uint32_t clusterCount = log->sd.vol()->freeClusterCount();
	uint32_t fs = 0.512 * clusterCount * log->sd.vol()->blocksPerCluster();
	return fs ;
}

inline uint32_t log_size (gps_log_t *log)
{
	return log->file.fileSize();
}

inline int log_remove (gps_log_t *log)
{
	return log->file.remove();
}

inline void log_seek (gps_log_t *log, const uint32_t position)
{
	log->file.seekSet(position);
}

inline void log_setpos (gps_log_t *log, uint32_t position)
{
	log_seek(log, position);
}

inline uint32_t log_seteof (gps_log_t *log)
{
	return log->file.seekEnd();
}

inline uint32_t log_eof (gps_log_t *log)
{
	return log->file.fileSize();
}

inline uint32_t log_getpos (gps_log_t *log)
{
	return log->file.curPosition();
}

inline void log_rewind (gps_log_t *log)
{
	log_setpos(log, 0);
}

inline void log_flush (gps_log_t *log)
{
	log->file.sync();
}

inline int log_write (gps_log_t *log, void *bin, const uint32_t len)
{
	//return (log->file.write(bin, len) == len);
	return log->file.write(bin, len);
}

inline int log_write (gps_log_t *log, const char *str)
{
	return log->file.println(str);
}

inline int log_header_write (gps_log_t *log, log_header_t *header)
{
	log_rewind(log);
	return log_write(log, (void*)header, LOG_HEADER_LENGTH);
}

inline int log_header_read (gps_log_t *log, log_header_t *header)
{
	log_rewind(log);
	int len = log_read(log, (void*)header, LOG_HEADER_LENGTH);

	//if (len > 0)
	//	header[LOG_HEADER_LENGTH-1] = 0;

	return len;
}

inline int log_read (gps_log_t *log, char *str, const uint32_t buffLen)
{
	return log->file.fgets(str, buffLen, "\n");
}

inline int log_read (gps_log_t *log, void *bin, const uint32_t len)
{
	return log->file.read(bin, len);
}

inline int log_getFilenameByIndex (gps_log_t *log, int idx, char *filename, const int len)
{
	if (idx++ < 0) return 0;
	
	SdFat *sd = &log->sd;
	SdFile *file = &log->file;
	
	if (file->isOpen()) file->close();
	sd->chdir("/");
	sd->vwd()->seekSet(0);
	
	*filename = 0;
	
	while (idx--){
		if (file->openNext(sd->vwd(), O_READ)){
			if (!idx) file->getName(filename, len);
			file->close();
		}else{
			return 0;
		}
	}

	return *filename;
}

inline int log_getTotalFiles (gps_log_t *log)
{
	tft_disable();
	
	SdFat *sd = &log->sd;
	SdFile *file = &log->file;

	if (file->isOpen()) file->close();
	sd->chdir("/");
	sd->vwd()->seekSet(0);

	int dirTotalFiles = 0;
	
	while (file->openNext(sd->vwd(), O_READ)){
		dirTotalFiles++;
		file->close();
	}
	
	tft_enable();
	return dirTotalFiles;
}

#if 0
INLINE void log_listFiles (gps_log_t *log)
{

	tft_disable();
	// sd_enable();

	log->file.ls(&sl, LS_SIZE /*|LS_DATE*/);

	// sd_disable();
	tft_enable();
}
#endif

inline int log_deleteFile (gps_log_t *log, const int dbg, const char *filename)
{
	tft_disable();
	int ret = 0;
	
	if (dbg)
		printfn("searching for log '%s'", filename);

	if (log_exists(log, filename)){
		if (log_open(log, filename)){
			ret = log_remove(log);
			log_close(log);
		}
			
		if (dbg){
			if (ret)
				printfn("log removed");
			else
				printfn("log delete failed");
		}
	}else if (dbg){
		printfn("file '%s' not found", filename);
	}
	
	tft_enable();
	return ret;
}

inline int log_deleteLog (gps_log_t *log, const int logIdx)
{
	
	if (logIdx > 0){
		char filename[16];
		snprintf(filename, sizeof(filename), "%.6i.dat", logIdx);
		return log_deleteFile(log, 0, filename);
	}
	return 0;
}

inline int log_deleteLog (gps_log_t *log)
{
	int ret = log_deleteLog(log, log->fileIndex);
	log->fileIndex = -1;
	
	return ret;
}

inline int log_listLogs (gps_log_t *log)
{

	tft_disable();
	// sd_enable();
		
	char filename[16];
	int ct = 1;
	size_t totalSize = 0;

	do {
		snprintf(filename, sizeof(filename), "%.6i.dat", ct);
		printf("%i: %s", ct, filename);
		
		
		if (log_exists(log, filename)){
			//sl.print("opening.. ");
			
			if (log_open(log, filename)){
				//sl.println("done");
				size_t fsize = log_size(log);
				totalSize += fsize;
				
				sl.print("  ");
				log->file.printCreateDateTime(&sl);
				log_close(log);

				printfn("  %i", fsize);
			//}else{
				//sl.println("failed");
			}
			
		}else{
			sl.println("  could not open");
			break;
		}
		
		ct++;
	}while(1);
	
	sl.print("total size:  "); sl.println(totalSize);
	
	// sd_disable();
	tft_enable();
	
	return ct;
}

inline int log_createFilenamePrev (gps_log_t *log, char *filename, const int bufflen, int idx)
{
	if (bufflen <= 10) return 0;
	if (idx < 2) return 0;
	
	//do {
		snprintf(filename, bufflen, "%.6i.dat", --idx);
		if (log_exists(log, filename))
			return idx;
		else
			return 0;
	//}while(!log_exists(log, filename));

	//sl.print("log_createFilename: ");
	//sl.println(filename);
	
	//return ++idx;
}

inline int log_createFilenameNext (gps_log_t *log, char *filename, const int bufflen, int idx)
{
	if (bufflen <= 10) return 0;

	//do {
		snprintf(filename, bufflen, "%.6i.dat", ++idx);
		if (log_exists(log, filename))
			return idx;
		else
			return 0;
	//}while(1);

	//sl.print("log_createFilename: ");
	//sl.println(filename);
	
	//return --idx;
}

inline int log_createFilename (gps_log_t *log, char *filename, const int bufflen)
{
	if (bufflen <= 10) return 0;

	int idx = 1;
	do {
		snprintf(filename, bufflen, "%.6i.dat", idx++);
	}while(log_exists(log, filename));

	//sl.print("log_createFilename: ");
	//sl.println(filename);
	
	return --idx;
}

inline int log_createFilenameLast (gps_log_t *log, char *filename, const int bufflen)
{
	if (bufflen <= 10) return 0;

	int idx = 1;
	do {
		snprintf(filename, bufflen, "%.6i.dat", idx++);
	}while(log_exists(log, filename));

	//sl.print("log_createFilename: ");
	//sl.println(filename);
	
	idx -= 1;
	if (idx < 1) idx = 1;
	return idx;
}

inline void log_finish (gps_log_t *log)
{
	log_flush(log);
	log_close(log);
	log->isOpen = 0;
}


/* #################################################################################*/
/* #################################################################################*/
/* #################################################################################*/
/* #################################################################################*/


inline void log_enter ()
{
	sf_disable();
	tft_disable();
	sd_enable();
}

inline void log_exit ()
{
	sf_disable();
	sd_disable();
	tft_enable();
}

inline int log_read_open (gps_log_t *log, const char *filename)
{
	log_enter();
			
	if (!log_exists(log, filename)){
		log_exit();
		return 0;
	}
		
	log->isOpen = log_open_oread(log, filename);	
	if (!log->isOpen)
		log_exit();
	else
		strcpy(log->filename, filename);

	return log->isOpen;
}

inline int log_read_begin (gps_log_t *log, const uint32_t offset)
{
	log_enter();
			
	log->isOpen = log_open(log, log->filename);	
	if (!log->isOpen)
		log_exit();
	else
		log_setpos(log, offset);
	
	return log->isOpen;
}

inline int log_read_pts (gps_log_t *log, const uint32_t pt_readFrom, gps_datapt_t *pts, const int pt_readTotal)
{
	uint32_t offset = LOG_HEADER_LENGTH + (pt_readFrom * sizeof(gps_datapt_t));	
	log_setpos(log, offset);

	int readLength = pt_readTotal * sizeof(gps_datapt_t);	
	
	int ptsRead = log_read(log, pts, readLength) / sizeof(gps_datapt_t);
	if (ptsRead > readLength)	// we have a problem
		return 0;
	else
		return ptsRead;
}

inline void log_read_end (gps_log_t *log)
{
	if (log->isOpen){
		log->isOpen = 0;
		log_close(log);
	}
		
	log_exit();
}

inline int log_write_begin (gps_log_t *log)
{
	tft_disable();
	// sd_enable();
			
	log->isOpen = log_open(log, log->filename);	
	if (!log->isOpen){
		// sd_disable();
		tft_enable();
	}else{
		//log_setpos(log, log_eof(log));
		log_seteof(log);
	}

	return log->isOpen;
}

inline int log_write_begin (gps_log_t *log, const size_t position)
{
	tft_disable();
	// sd_enable();
			
	log->isOpen = log_open(log, log->filename);	
	if (!log->isOpen){
		// sd_disable();
		tft_enable();
	}else{
		log_setpos(log, position);
	}

	return log->isOpen;
}

inline int log_write_pts (gps_log_t *log, gps_datapt_t *pts, const uint32_t total)
{
	return log_write(log, (void*)pts, total*sizeof(gps_datapt_t)) / sizeof(gps_datapt_t);
}

inline void log_write_end (gps_log_t *log)
{
	if (log->isOpen)
		log_finish(log);
		
	// sd_disable();
	tft_enable();
}

inline int32_t log_getPositionCount (gps_log_t *log)
{
	return (log_size(log) - LOG_HEADER_LENGTH) / sizeof(gps_datapt_t);
}

#endif

