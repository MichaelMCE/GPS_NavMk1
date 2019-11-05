
/*

	Using printf on the Arduino.
	by Michael McElligott
	
	Usage:
	Set a buffer size with _PRINTF_BUFFER_LENGTH_, default is 64 bytes, or about a single line
	Set output stream with _Stream_Obj_. eg; SerialUSB
	
	printf(format string, argument(s) list).
	printfn(): As above but appends a new line on each print; aka serial.println()
	
	eg; printf("%.2f, 0x%X", 1234.5678f, 32768);
	
	For a detailed list on printf specifiers:
	http://www.cplusplus.com/reference/cstdio/printf/
	and
	http://man7.org/linux/man-pages/man3/printf.3.html


		
	Tested with the Arduino Due 1.6.6
	Jan 2016	

*/





#ifndef _printf_h_
#define _printf_h_

#define _PRINTF_BUFFER_LENGTH_		96

#define _Stream_Obj_				sl
//#define _wifi_Obj_					wifi
#define _tft_Obj_					bike->render.tft




#if 1
static char _pf_buffer_[_PRINTF_BUFFER_LENGTH_];
//static char _pf_wifi_buffer_[_PRINTF_BUFFER_LENGTH_];
static char _pf_tft_buffer_[_PRINTF_BUFFER_LENGTH_];
#else
extern char _pf_buffer_[_PRINTF_BUFFER_LENGTH_];
//extern char _pf_wifi_[_PRINTF_BUFFER_LENGTH_];
extern char _pf_tft_buffer_[_PRINTF_BUFFER_LENGTH_];
#endif


#if 0

#define printf(a,...)		sl.printf(a,...)
#define printfn(a,...)		sl.printf(a"\r\n",...)

#else

#define printf(a,...)														\
	do{																		\
	snprintf(_pf_buffer_, sizeof(_pf_buffer_), (a), ##__VA_ARGS__);			\
	_Stream_Obj_.print(_pf_buffer_);										\
	}while(0)

#define printfn(a,...)														\
	do{																		\
	snprintf(_pf_buffer_, sizeof(_pf_buffer_), a"\r\n", ##__VA_ARGS__);		\
	_Stream_Obj_.print(_pf_buffer_);										\
	}while(0)
#endif


#if 0
#define wprintf(a,...)														\
	do{																		\
	snprintf(_pf_wifi_buffer_, sizeof(_pf_wifi_buffer_), (a), ##__VA_ARGS__);			\
	_wifi_Obj_.print(_pf_wifi_buffer_);										\
	}while(0)

#define wprintfn(a,...)														\
	do{																		\
	snprintf(_pf_wifi_buffer_, sizeof(_pf_wifi_buffer_), a"\r\n", ##__VA_ARGS__);		\
	_wifi_Obj_.print(_pf_wifi_buffer_);										\
	}while(0)
#endif

#define tprintf(a,...)														\
	do{																		\
	snprintf(_pf_tft_buffer_, sizeof(_pf_tft_buffer_), (a), ##__VA_ARGS__);	\
	_tft_Obj_.print(_pf_tft_buffer_);										\
	}while(0)

#define tprintfn(str,...)														\
	do{																		\
	snprintf(_pf_tft_buffer_, sizeof(_pf_tft_buffer_), str"\n", ##__VA_ARGS__);		\
	_tft_Obj_.print(_pf_tft_buffer_);										\
	}while(0)


#define tprintfae(x,y,align,str,...)											\
	do{																		\
	snprintf(_pf_tft_buffer_, sizeof(_pf_tft_buffer_),(str), ##__VA_ARGS__);	\
	_tft_Obj_.printAlignedOffseted((const char*)_pf_tft_buffer_,(gTextAlign)(align),(int16_t)(x),(int16_t)(y),(gTextEraseLine)gTextEraseToEOL);	\
	}while(0)

#define tprintfa(x,y,align,str,...)											\
	do{																		\
	snprintf(_pf_tft_buffer_, sizeof(_pf_tft_buffer_),(str), ##__VA_ARGS__);	\
	_tft_Obj_.printAlignedOffseted((const char*)_pf_tft_buffer_,(gTextAlign)(align),(int16_t)(x),(int16_t)(y));	\
	}while(0)

#endif


