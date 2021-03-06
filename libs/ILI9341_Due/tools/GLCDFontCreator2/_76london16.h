

/*
 *
 * _76london16
 *
 * created with FontCreator
 * written by F. Maximilian Thiele
 *
 * http://www.apetech.de/fontCreator
 * me@apetech.de
 *
 * File Name           : _76london16.h
 * Date                : 26.02.2016
 * Font size in bytes  : 8847
 * Font width          : 10
 * Font height         : 15
 * Font first char     : 32
 * Font last char      : 128
 * Font used chars     : 96
 *
 * The font data are defined as
 *
 * struct _FONT_ {
 *     uint16_t   font_Size_in_Bytes_over_all_included_Size_it_self;
 *     uint8_t    font_Width_in_Pixel_for_fixed_drawing;
 *     uint8_t    font_Height_in_Pixel_for_all_characters;
 *     unit8_t    font_First_Char;
 *     uint8_t    font_Char_Count;
 *
 *     uint8_t    font_Char_Widths[font_Last_Char - font_First_Char +1];
 *                  // for each character the separate width in pixels,
 *                  // characters < 128 have an implicit virtual right empty row
 *
 *     uint8_t    font_data[];
 *                  // bit field of all characters
 */

#include <inttypes.h>
#include <avr/pgmspace.h>

#ifndef _76LONDON16_H
#define _76LONDON16_H

#define _76LONDON16_WIDTH 10
#define _76LONDON16_HEIGHT 15

static uint8_t _76london16[] PROGMEM = {
    0x22, 0x8F, // size
    0x0A, // width
    0x0F, // height
    0x20, // first char
    0x60, // char count
    
    // char widths
    0x00, 0x02, 0x05, 0x09, 0x07, 0x09, 0x08, 0x02, 0x04, 0x04, 
    0x05, 0x08, 0x02, 0x04, 0x02, 0x04, 0x07, 0x03, 0x07, 0x06, 
    0x07, 0x07, 0x07, 0x06, 0x07, 0x07, 0x02, 0x02, 0x03, 0x08, 
    0x03, 0x06, 0x0B, 0x07, 0x07, 0x06, 0x07, 0x06, 0x06, 0x07, 
    0x07, 0x02, 0x06, 0x07, 0x06, 0x0C, 0x07, 0x07, 0x07, 0x07, 
    0x07, 0x07, 0x06, 0x07, 0x08, 0x0D, 0x07, 0x07, 0x07, 0x03, 
    0x04, 0x03, 0x05, 0x09, 0x03, 0x07, 0x07, 0x06, 0x07, 0x06, 
    0x06, 0x07, 0x07, 0x02, 0x06, 0x07, 0x06, 0x0C, 0x07, 0x07, 
    0x07, 0x07, 0x07, 0x07, 0x06, 0x07, 0x08, 0x0D, 0x07, 0x07, 
    0x07, 0x04, 0x02, 0x04, 0x06, 0x00, 
    
    // font data
    0xFE, 0xFE, 0x3E, 0x3E, // 33
    0x0E, 0x0E, 0x00, 0x0E, 0x0E, 0x00, 0x00, 0x00, 0x00, 0x00, // 34
    0x00, 0x30, 0xB0, 0xFC, 0x36, 0xF0, 0xFC, 0x36, 0x30, 0x06, 0x36, 0x3E, 0x06, 0x36, 0x1E, 0x06, 0x06, 0x00, // 35
    0x3C, 0x7C, 0x64, 0x66, 0x66, 0xE4, 0xC0, 0x00, 0x18, 0x18, 0x38, 0x38, 0x1E, 0x0E, // 36
    0x3E, 0x22, 0x22, 0x3E, 0xC0, 0xB0, 0x8C, 0x86, 0x80, 0x00, 0x30, 0x08, 0x06, 0x00, 0x3E, 0x20, 0x20, 0x3E, // 37
    0x7C, 0xFE, 0xC6, 0xC6, 0xC6, 0xC6, 0xC0, 0xC0, 0x1E, 0x3E, 0x30, 0x30, 0x30, 0x3E, 0x1E, 0x00, // 38
    0x0E, 0x0E, 0x00, 0x00, // 39
    0xF8, 0xFC, 0x02, 0x02, 0x0E, 0x1E, 0x20, 0x20, // 40
    0x02, 0x02, 0xFC, 0xF8, 0x20, 0x20, 0x1E, 0x0E, // 41
    0x4C, 0x68, 0x1E, 0x78, 0x4C, 0x00, 0x00, 0x00, 0x00, 0x00, // 42
    0x00, 0x00, 0x00, 0xE0, 0xE0, 0x00, 0x00, 0x00, 0x06, 0x06, 0x06, 0x3E, 0x3E, 0x06, 0x06, 0x06, // 43
    0x00, 0x00, 0x70, 0x70, // 44
    0x00, 0x00, 0x00, 0x00, 0x06, 0x06, 0x06, 0x06, // 45
    0x00, 0x00, 0x30, 0x30, // 46
    0x00, 0x00, 0xF0, 0x0E, 0x30, 0x1E, 0x00, 0x00, // 47
    0xFC, 0xFE, 0x06, 0x06, 0x06, 0xFE, 0xFC, 0x1E, 0x3E, 0x30, 0x30, 0x30, 0x3E, 0x1E, // 48
    0x06, 0xFE, 0xFC, 0x00, 0x3E, 0x3E, // 49
    0x80, 0xC6, 0xC6, 0xC6, 0xC6, 0xFE, 0x7C, 0x3E, 0x3E, 0x30, 0x30, 0x30, 0x30, 0x30, // 50
    0x06, 0xC6, 0xC6, 0xC6, 0xFE, 0xFC, 0x30, 0x30, 0x30, 0x30, 0x3E, 0x1E, // 51
    0x7E, 0xFE, 0xC0, 0xC0, 0xC0, 0xFE, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3E, 0x3E, // 52
    0xFC, 0xFE, 0xC6, 0xC6, 0xC6, 0xC6, 0x80, 0x30, 0x30, 0x30, 0x30, 0x30, 0x3E, 0x1E, // 53
    0xFC, 0xFE, 0x46, 0x66, 0x66, 0xE6, 0xC0, 0x1E, 0x3E, 0x30, 0x30, 0x30, 0x3E, 0x1E, // 54
    0x06, 0x06, 0x06, 0x06, 0xFE, 0xFC, 0x00, 0x00, 0x00, 0x00, 0x3E, 0x3E, // 55
    0xFC, 0xFE, 0xC6, 0xC6, 0xC6, 0xFE, 0xFC, 0x1E, 0x3E, 0x30, 0x30, 0x30, 0x3E, 0x1E, // 56
    0xFC, 0xFE, 0x86, 0x86, 0x86, 0xFE, 0xFC, 0x00, 0x32, 0x32, 0x32, 0x30, 0x3E, 0x1E, // 57
    0x30, 0x30, 0x30, 0x30, // 58
    0x30, 0x30, 0x70, 0x70, // 59
    0x00, 0x80, 0x40, 0x04, 0x0E, 0x10, // 60
    0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0x80, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x10, // 61
    0x40, 0xC0, 0x00, 0x10, 0x18, 0x06, // 62
    0x86, 0xC6, 0xC6, 0xC6, 0x7E, 0x7C, 0x36, 0x36, 0x00, 0x00, 0x00, 0x00, // 63
    0xF8, 0x04, 0x02, 0xE2, 0xF2, 0x92, 0x92, 0xF2, 0xE2, 0x84, 0xF8, 0x0E, 0x10, 0x20, 0x26, 0x26, 0x20, 0x20, 0x26, 0x26, 0x00, 0x00, // 64
    0xFC, 0xFE, 0xC6, 0xC6, 0xC6, 0xFE, 0xFC, 0x3E, 0x3E, 0x00, 0x00, 0x00, 0x3E, 0x3E, // 65
    0xFC, 0xFE, 0xC6, 0xC6, 0xC6, 0xFE, 0x38, 0x3E, 0x3E, 0x30, 0x30, 0x30, 0x3E, 0x1E, // 66
    0xFC, 0xFE, 0x06, 0x06, 0x06, 0x06, 0x1E, 0x3E, 0x30, 0x30, 0x30, 0x30, // 67
    0xFE, 0xFE, 0x06, 0x06, 0x06, 0xFE, 0xFC, 0x3E, 0x3E, 0x30, 0x30, 0x30, 0x3E, 0x1E, // 68
    0xFC, 0xFE, 0xC6, 0xC6, 0xC6, 0x06, 0x1E, 0x3E, 0x30, 0x30, 0x30, 0x30, // 69
    0xFC, 0xFE, 0xC6, 0xC6, 0xC6, 0x06, 0x3E, 0x3E, 0x00, 0x00, 0x00, 0x00, // 70
    0xFC, 0xFE, 0x06, 0xC6, 0xC6, 0xC6, 0xC0, 0x1E, 0x3E, 0x30, 0x30, 0x30, 0x1E, 0x3E, // 71
    0xFE, 0xFE, 0xC0, 0xC0, 0xC0, 0xFE, 0xFE, 0x3E, 0x3E, 0x00, 0x00, 0x00, 0x3E, 0x3E, // 72
    0xFE, 0xFE, 0x3E, 0x3E, // 73
    0x00, 0x00, 0x00, 0x00, 0xFE, 0xFE, 0x30, 0x30, 0x30, 0x30, 0x3E, 0x1E, // 74
    0xFE, 0xFE, 0xE0, 0xF0, 0xFC, 0xCE, 0x82, 0x3E, 0x3E, 0x00, 0x00, 0x00, 0x3E, 0x3E, // 75
    0xFE, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x1E, 0x3E, 0x30, 0x30, 0x30, 0x30, // 76
    0xFC, 0xFE, 0x06, 0x06, 0x06, 0xFE, 0xFE, 0x06, 0x06, 0x06, 0xFE, 0xFC, 0x3E, 0x3E, 0x00, 0x00, 0x00, 0x3E, 0x3E, 0x00, 0x00, 0x00, 0x3E, 0x3E, // 77
    0xFC, 0xFE, 0x06, 0x06, 0x06, 0xFE, 0xFC, 0x3E, 0x3E, 0x00, 0x00, 0x00, 0x3E, 0x3E, // 78
    0xFC, 0xFE, 0x06, 0x06, 0x06, 0xFE, 0xFC, 0x1E, 0x3E, 0x30, 0x30, 0x30, 0x3E, 0x1E, // 79
    0xFC, 0xFE, 0x86, 0x86, 0x86, 0xFC, 0xFC, 0x3E, 0x3E, 0x02, 0x02, 0x02, 0x02, 0x00, // 80
    0xFC, 0xFE, 0x06, 0x06, 0x0E, 0xFE, 0xFC, 0x3E, 0x3E, 0xE0, 0xE0, 0x70, 0x3E, 0x3E, // 81
    0xFC, 0xFE, 0xC6, 0xC6, 0xC6, 0xFE, 0x3C, 0x3E, 0x3E, 0x00, 0x00, 0x00, 0x3E, 0x3E, // 82
    0x7C, 0xFE, 0xC6, 0xC6, 0xC6, 0xC6, 0x80, 0x00, 0x30, 0x30, 0x30, 0x30, 0x3E, 0x1E, // 83
    0x06, 0x06, 0xFE, 0xFE, 0x06, 0x06, 0x00, 0x00, 0x3E, 0x3E, 0x00, 0x00, // 84
    0xFE, 0xFE, 0x00, 0x00, 0x00, 0xFE, 0xFE, 0x1E, 0x3E, 0x30, 0x30, 0x30, 0x3E, 0x1E, // 85
    0x02, 0x3E, 0xFE, 0xC0, 0xC0, 0xFE, 0x3E, 0x02, 0x00, 0x00, 0x0E, 0x3E, 0x3E, 0x0E, 0x00, 0x00, // 86
    0x02, 0x3E, 0xFE, 0xC0, 0x80, 0xFE, 0x3E, 0xFE, 0xC0, 0x80, 0xFE, 0x3E, 0x02, 0x00, 0x00, 0x0E, 0x3E, 0x3E, 0x0E, 0x00, 0x0E, 0x3E, 0x3E, 0x0E, 0x00, 0x00, // 87
    0x06, 0x1E, 0xF8, 0xF0, 0xFC, 0x0E, 0x02, 0x30, 0x3C, 0x0E, 0x06, 0x1E, 0x38, 0x20, // 88
    0x7E, 0xFE, 0xC0, 0xC0, 0xC0, 0xFE, 0x7E, 0x00, 0x00, 0x3E, 0x3E, 0x00, 0x00, 0x00, // 89
    0x00, 0x06, 0x86, 0xE6, 0x7E, 0x1E, 0x06, 0x30, 0x3C, 0x3E, 0x32, 0x30, 0x30, 0x30, // 90
    0xFE, 0xFE, 0x02, 0x3E, 0x3E, 0x20, // 91
    0x06, 0x7C, 0xC0, 0x00, 0x00, 0x00, 0x06, 0x38, // 92
    0x02, 0xFE, 0xFE, 0x20, 0x3E, 0x3E, // 93
    0x08, 0x0C, 0x06, 0x0E, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, // 94
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, // 95
    0x02, 0x06, 0x04, 0x00, 0x00, 0x00, // 96
    0xE0, 0xF0, 0x30, 0x30, 0x30, 0xF0, 0xE0, 0x3E, 0x3E, 0x06, 0x06, 0x06, 0x3E, 0x3E, // 97
    0xE0, 0xF0, 0xB0, 0xB0, 0xB0, 0xF0, 0x60, 0x3E, 0x3E, 0x32, 0x32, 0x32, 0x3E, 0x1C, // 98
    0xE0, 0xF0, 0x30, 0x30, 0x30, 0x30, 0x1E, 0x3E, 0x30, 0x30, 0x30, 0x30, // 99
    0xF0, 0xF0, 0x30, 0x30, 0x30, 0xF0, 0xE0, 0x3E, 0x3E, 0x30, 0x30, 0x30, 0x3E, 0x1E, // 100
    0xE0, 0xF0, 0xB0, 0xB0, 0xB0, 0x30, 0x1E, 0x3E, 0x32, 0x32, 0x32, 0x30, // 101
    0xE0, 0xF0, 0xB0, 0xB0, 0xB0, 0x30, 0x3E, 0x3E, 0x02, 0x02, 0x02, 0x00, // 102
    0xE0, 0xF0, 0x30, 0x30, 0x30, 0x30, 0x00, 0x1E, 0x3E, 0x30, 0x36, 0x36, 0x1E, 0x3E, // 103
    0xF0, 0xF0, 0x00, 0x00, 0x00, 0xF0, 0xF0, 0x3E, 0x3E, 0x06, 0x06, 0x06, 0x3E, 0x3E, // 104
    0xF0, 0xF0, 0x3E, 0x3E, // 105
    0x00, 0x00, 0x00, 0x00, 0xF0, 0xF0, 0x30, 0x30, 0x30, 0x30, 0x3E, 0x1E, // 106
    0xF0, 0xF0, 0x80, 0xC0, 0xE0, 0x30, 0x10, 0x7E, 0x7E, 0x06, 0x06, 0x06, 0x7E, 0x7C, // 107
    0xF0, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x1E, 0x3E, 0x30, 0x30, 0x30, 0x30, // 108
    0xE0, 0xF0, 0x30, 0x30, 0x30, 0xF0, 0xF0, 0x30, 0x30, 0x30, 0xF0, 0xE0, 0x3E, 0x3E, 0x00, 0x00, 0x00, 0x3E, 0x3E, 0x00, 0x00, 0x00, 0x3E, 0x3E, // 109
    0xE0, 0xF0, 0x30, 0x30, 0x30, 0xF0, 0xE0, 0x3E, 0x3E, 0x00, 0x00, 0x00, 0x3E, 0x3E, // 110
    0xE0, 0xF0, 0x30, 0x30, 0x30, 0xF0, 0xE0, 0x3E, 0x7E, 0x60, 0x60, 0x60, 0x7E, 0x3E, // 111
    0xE0, 0xF0, 0x30, 0x30, 0x30, 0xF0, 0xE0, 0x3E, 0x3E, 0x06, 0x06, 0x06, 0x06, 0x02, // 112
    0xE0, 0xF0, 0x30, 0x30, 0x30, 0xF0, 0xE0, 0x3E, 0x7E, 0xE0, 0xE0, 0x60, 0x7E, 0x3E, // 113
    0xE0, 0xF0, 0x30, 0x30, 0x30, 0xF0, 0xE0, 0x3E, 0x3E, 0x06, 0x06, 0x06, 0x3E, 0x3E, // 114
    0xE0, 0xF0, 0xB0, 0xB0, 0xB0, 0xB0, 0x00, 0x00, 0x32, 0x32, 0x32, 0x32, 0x3E, 0x1E, // 115
    0x30, 0x30, 0xF0, 0xF0, 0x30, 0x30, 0x00, 0x00, 0x3E, 0x3E, 0x00, 0x00, // 116
    0xF0, 0xF0, 0x00, 0x00, 0x00, 0xF0, 0xF0, 0x3E, 0x7E, 0x60, 0x60, 0x60, 0x7E, 0x3E, // 117
    0x10, 0xF0, 0xF0, 0x80, 0x00, 0xF0, 0xF0, 0x10, 0x00, 0x00, 0x1E, 0x3E, 0x3E, 0x0E, 0x00, 0x00, // 118
    0x10, 0xF0, 0xF0, 0x80, 0x00, 0xF0, 0xF0, 0xF0, 0x80, 0x00, 0xF0, 0xF0, 0x10, 0x00, 0x00, 0x0E, 0x3E, 0x3E, 0x1E, 0x00, 0x0E, 0x3E, 0x3E, 0x1E, 0x00, 0x00, // 119
    0x10, 0x70, 0xE0, 0xC0, 0xF0, 0x30, 0x10, 0x30, 0x3C, 0x1E, 0x06, 0x3E, 0x38, 0x20, // 120
    0xF0, 0xF0, 0x00, 0x00, 0x00, 0xF0, 0xF0, 0x02, 0x06, 0x3E, 0x3E, 0x06, 0x06, 0x02, // 121
    0x00, 0x30, 0x30, 0xB0, 0xF0, 0x70, 0x30, 0x30, 0x38, 0x3C, 0x36, 0x32, 0x30, 0x30, // 122
    0x40, 0xFE, 0xFE, 0x02, 0x00, 0x3E, 0x3E, 0x20, // 123
    0xFF, 0xFF, 0xFE, 0xFE, // 124
    0x02, 0xFE, 0xFE, 0x40, 0x20, 0x3E, 0x3E, 0x00, // 125
    0x04, 0x06, 0x06, 0x04, 0x06, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 126
    
};

#endif
