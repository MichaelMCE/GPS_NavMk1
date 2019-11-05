

/*
 *
 * wintvpgothic24
 *
 * created with FontCreator
 * written by F. Maximilian Thiele
 *
 * http://www.apetech.de/fontCreator
 * me@apetech.de
 *
 * File Name           : wintvpgothic24
 * Date                : 01.12.2015
 * Font size in bytes  : 27902
 * Font width          : 10
 * Font height         : 25
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

#ifndef WINTVPGOTHIC24_H
#define WINTVPGOTHIC24_H

#define WINTVPGOTHIC24_WIDTH 10
#define WINTVPGOTHIC24_HEIGHT 25

static const uint8_t wintvpgothic24[] PROGMEM = {
    0x6D, 0x06, // size
    0x0A, // width
    0x19, // height
    0x20, // first char
    0x60, // char count
    
    // char widths
    0x02, 0x03, 0x08, 0x0E, 0x0C, 0x14, 0x11, 0x04, 0x08, 0x08, 
    0x0A, 0x0A, 0x04, 0x0A, 0x04, 0x0B, 0x0E, 0x08, 0x0D, 0x0D, 
    0x0F, 0x0D, 0x0D, 0x0D, 0x0E, 0x0E, 0x04, 0x04, 0x0A, 0x0A, 
    0x0A, 0x0C, 0x12, 0x11, 0x0D, 0x10, 0x10, 0x0C, 0x0C, 0x10, 
    0x0F, 0x03, 0x08, 0x0F, 0x0B, 0x13, 0x0F, 0x12, 0x0D, 0x12, 
    0x0F, 0x0D, 0x0F, 0x0F, 0x10, 0x17, 0x10, 0x0F, 0x0F, 0x08, 
    0x0F, 0x08, 0x08, 0x08, 0x05, 0x0C, 0x0D, 0x0C, 0x0D, 0x0C, 
    0x0A, 0x0D, 0x0C, 0x03, 0x06, 0x0B, 0x03, 0x12, 0x0C, 0x0D, 
    0x0D, 0x0D, 0x08, 0x0A, 0x09, 0x0C, 0x0D, 0x14, 0x0D, 0x0D, 
    0x0A, 0x06, 0x02, 0x06, 0x09, 0x13, 
    
    // font data
    0x00, 0x00, 0x00, 0x00,	0x00, 0x00, 0x00, 0x00,// 32
    0xF0, 0xF0, 0xF0, 0x03, 0xFF, 0xFF, 0x38, 0x39, 0x39, 0x00, 0x00, 0x00, // 33
    0x80, 0xF8, 0xF8, 0x18, 0x00, 0xF8, 0xF8, 0x38, 0x03, 0x03, 0x01, 0x00, 0x00, 0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 34
    0x00, 0x00, 0x00, 0x00, 0x80, 0xF8, 0x38, 0x00, 0x00, 0x00, 0xF8, 0x38, 0x00, 0x00, 0x40, 0x41, 0x41, 0xE1, 0xFF, 0x47, 0x41, 0x41, 0xC1, 0xFF, 0x4F, 0x41, 0x41, 0x41, 0x00, 0x00, 0x38, 0x3F, 0x01, 0x00, 0x00, 0x30, 0x3F, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 35
    0x80, 0xC0, 0xE0, 0x70, 0x30, 0xFC, 0xFC, 0x70, 0x70, 0xE0, 0xE0, 0x40, 0x07, 0x0F, 0x1F, 0x1C, 0x3C, 0xFF, 0xFF, 0x70, 0xF0, 0xE0, 0xC0, 0x80, 0x0C, 0x1E, 0x1C, 0x38, 0x38, 0xFF, 0xFF, 0x30, 0x38, 0x1F, 0x0F, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 36
    0xC0, 0xF0, 0x38, 0x18, 0x18, 0x38, 0xF0, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xE0, 0x30, 0x18, 0x00, 0x00, 0x00, 0x07, 0x1F, 0x38, 0x30, 0x30, 0x38, 0x1F, 0x87, 0xC0, 0x70, 0x18, 0x0E, 0xC3, 0xE1, 0x70, 0x30, 0x30, 0x70, 0xE0, 0xC0, 0x00, 0x00, 0x00, 0x30, 0x18, 0x0C, 0x07, 0x01, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x1F, 0x38, 0x30, 0x30, 0x38, 0x1F, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 37
    0x00, 0x00, 0xE0, 0xF0, 0xF0, 0x38, 0x18, 0x18, 0x38, 0xF8, 0xF0, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xE0, 0xF1, 0x77, 0x3F, 0x3E, 0x7C, 0xFC, 0xEE, 0xC7, 0x87, 0x01, 0xC0, 0xF0, 0xF0, 0x00, 0x00, 0x07, 0x0F, 0x1F, 0x38, 0x30, 0x30, 0x30, 0x30, 0x39, 0x1B, 0x1F, 0x0F, 0x0F, 0x1F, 0x3C, 0x38, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 38
    0x80, 0xF8, 0xF8, 0x18, 0x03, 0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 39
    0x00, 0xC0, 0xE0, 0xF8, 0x3C, 0x0E, 0x06, 0x02, 0xFE, 0xFF, 0xFF, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x0F, 0x3F, 0x78, 0xE0, 0xC0, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 40
    0x02, 0x06, 0x0E, 0x3C, 0xF8, 0xE0, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xFF, 0xFF, 0xFE, 0x80, 0xC0, 0xE0, 0x78, 0x3F, 0x0F, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 41
    0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0xC6, 0xEE, 0x6C, 0x28, 0xFF, 0xFF, 0x28, 0x6C, 0xEE, 0xC6, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 42
    0x00, 0x00, 0x00, 0x00, 0xE0, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x18, 0x18, 0xFF, 0xFF, 0x18, 0x18, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 0x07, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 43
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1E, 0xDE, 0xFE, 0x3E, 0x80, 0x80, 0x00, 0x00, // 44
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 45
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3C, 0x3C, 0x3C, 0x3C, 0x00, 0x00, 0x00, 0x00, // 46
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x78, 0x1E, 0x04, 0x00, 0x00, 0x00, 0x80, 0xF0, 0x3C, 0x0F, 0x01, 0x00, 0x00, 0x00, 0x60, 0x78, 0x0E, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 47
    0x00, 0xC0, 0xE0, 0xF0, 0x38, 0x18, 0x18, 0x18, 0x18, 0x38, 0xF0, 0xE0, 0xC0, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x01, 0x07, 0x0F, 0x1E, 0x38, 0x30, 0x30, 0x30, 0x30, 0x38, 0x1E, 0x0F, 0x07, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 48
    0x60, 0x60, 0x60, 0x70, 0x70, 0xF0, 0xF8, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3F, 0x3F, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 49
    0x80, 0xE0, 0xF0, 0x70, 0x38, 0x18, 0x18, 0x18, 0x18, 0x38, 0xF0, 0xE0, 0xC0, 0x00, 0x00, 0x00, 0x80, 0xC0, 0xE0, 0x70, 0x70, 0x38, 0x1C, 0x1F, 0x0F, 0x07, 0x3C, 0x3E, 0x3F, 0x3B, 0x39, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 50
    0x00, 0x60, 0x70, 0x30, 0x38, 0x18, 0x18, 0x18, 0x18, 0x38, 0xF0, 0xE0, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x18, 0x18, 0x3C, 0x7E, 0xEF, 0xE7, 0x83, 0x0C, 0x1C, 0x1C, 0x38, 0x30, 0x30, 0x30, 0x30, 0x38, 0x38, 0x1F, 0x0F, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 51
    0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xC0, 0xE0, 0x78, 0xF8, 0xF8, 0xF8, 0x00, 0x00, 0x00, 0xC0, 0xE0, 0xF0, 0x7C, 0x1E, 0x0F, 0x03, 0x01, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x3F, 0x3F, 0x3F, 0x03, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 52
    0x00, 0xF0, 0xF0, 0x70, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x00, 0x38, 0x3F, 0x3F, 0x18, 0x08, 0x0C, 0x0C, 0x0C, 0x1C, 0x3C, 0xF8, 0xF0, 0xE0, 0x08, 0x1C, 0x1C, 0x38, 0x30, 0x30, 0x30, 0x30, 0x38, 0x1C, 0x1F, 0x0F, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 53
    0x00, 0xC0, 0xE0, 0xF0, 0x30, 0x38, 0x18, 0x18, 0x18, 0x38, 0x70, 0x30, 0x20, 0xFE, 0xFF, 0xFF, 0x30, 0x18, 0x0C, 0x0C, 0x0C, 0x1C, 0x3C, 0xF8, 0xF0, 0xE0, 0x01, 0x07, 0x0F, 0x1C, 0x38, 0x30, 0x30, 0x30, 0x38, 0x1C, 0x1F, 0x0F, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 54
    0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0xF0, 0xF0, 0xF0, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0xF0, 0xFC, 0x3F, 0x0F, 0x03, 0x00, 0x00, 0x00, 0x00, 0x40, 0x78, 0x7F, 0x7F, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 55
    0x00, 0xC0, 0xE0, 0xF0, 0x38, 0x18, 0x18, 0x18, 0x18, 0x38, 0xF0, 0xF0, 0xC0, 0x00, 0x80, 0xC3, 0xE7, 0xEF, 0x7E, 0x3C, 0x38, 0x38, 0x3C, 0x7E, 0xEF, 0xE7, 0xC3, 0x80, 0x07, 0x0F, 0x1F, 0x18, 0x38, 0x30, 0x30, 0x30, 0x30, 0x38, 0x18, 0x1F, 0x0F, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 56
    0x80, 0xE0, 0xF0, 0x70, 0x38, 0x18, 0x18, 0x18, 0x18, 0x30, 0x70, 0xE0, 0xC0, 0x00, 0x0F, 0x1F, 0x3F, 0x38, 0x70, 0x60, 0x60, 0x60, 0x60, 0x30, 0x18, 0xFF, 0xFF, 0xFF, 0x08, 0x18, 0x1C, 0x38, 0x30, 0x30, 0x30, 0x30, 0x38, 0x1C, 0x1F, 0x0F, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 57
    0x00, 0x00, 0x00, 0x00, 0x1E, 0x1E, 0x1E, 0x1E, 0x3C, 0x3C, 0x3C, 0x3C, 0x00, 0x00, 0x00, 0x00, // 58
    0x00, 0x00, 0x00, 0x00, 0x1E, 0x1E, 0x1E, 0x1E, 0x3C, 0xBC, 0xFC, 0x3C, 0x80, 0x80, 0x00, 0x00, // 59
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x10, 0x38, 0x28, 0x6C, 0xC6, 0xC6, 0x83, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x03, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 60
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 61
    0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x83, 0x82, 0xC6, 0x44, 0x6C, 0x38, 0x38, 0x02, 0x03, 0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 62
    0xC0, 0xE0, 0xF0, 0x38, 0x18, 0x18, 0x18, 0x18, 0x38, 0xF0, 0xF0, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0xF8, 0x3C, 0x1E, 0x0F, 0x07, 0x03, 0x00, 0x00, 0x00, 0x00, 0x38, 0x38, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 63
    0x00, 0x80, 0xC0, 0xE0, 0x70, 0x30, 0x38, 0x18, 0x98, 0x98, 0x98, 0x98, 0x38, 0x30, 0x70, 0xE0, 0xC0, 0x00, 0xFC, 0xFF, 0x03, 0x00, 0xF8, 0xFE, 0x87, 0x03, 0x01, 0x81, 0xE3, 0xFF, 0xFF, 0x1F, 0x01, 0xC1, 0xFF, 0x3F, 0x00, 0x03, 0x07, 0x0E, 0x1C, 0x19, 0x3B, 0x33, 0x33, 0x31, 0x31, 0x31, 0x33, 0x1B, 0x1B, 0x19, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 64
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0xF0, 0x70, 0xF0, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0xF8, 0xFF, 0x9F, 0x83, 0x80, 0x83, 0x9F, 0xFF, 0xFC, 0xE0, 0x80, 0x00, 0x00, 0x20, 0x3C, 0x3F, 0x1F, 0x03, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x03, 0x1F, 0x3F, 0x3C, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 65
    0xF0, 0xF0, 0xF0, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x70, 0xE0, 0xE0, 0x80, 0xFF, 0xFF, 0xFF, 0x18, 0x18, 0x18, 0x18, 0x18, 0x38, 0x7C, 0xEF, 0xCF, 0x83, 0x3F, 0x3F, 0x3F, 0x30, 0x30, 0x30, 0x30, 0x30, 0x38, 0x3C, 0x1F, 0x0F, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 66
    0x00, 0x80, 0xC0, 0xE0, 0x70, 0x30, 0x18, 0x18, 0x18, 0x18, 0x18, 0x38, 0x70, 0xF0, 0xE0, 0x40, 0xFE, 0xFF, 0xFF, 0x81, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x07, 0x0F, 0x1E, 0x1C, 0x3C, 0x38, 0x38, 0x38, 0x38, 0x3C, 0x1C, 0x0F, 0x0F, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 67
    0xF0, 0xF0, 0xF0, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x70, 0x60, 0xE0, 0xC0, 0xC0, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xFF, 0xFF, 0xFC, 0x3F, 0x3F, 0x3F, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x38, 0x18, 0x1C, 0x0F, 0x07, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 68
    0xF0, 0xF0, 0xF0, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xFF, 0xFF, 0xFF, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00, 0x3F, 0x3F, 0x3F, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 69
    0xF0, 0xF0, 0xF0, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xFF, 0xFF, 0xFF, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x00, 0x3F, 0x3F, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 70
    0x00, 0x80, 0xC0, 0xE0, 0x70, 0x30, 0x18, 0x18, 0x18, 0x18, 0x18, 0x38, 0x70, 0xF0, 0xE0, 0x40, 0xFE, 0xFF, 0xFF, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x60, 0x60, 0x60, 0x60, 0xE0, 0xE0, 0x00, 0x03, 0x07, 0x0F, 0x1C, 0x38, 0x30, 0x30, 0x30, 0x30, 0x38, 0x18, 0x1C, 0x0F, 0x3F, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 71
    0xF8, 0xF8, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0xF8, 0xF8, 0xFF, 0xFF, 0xFF, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0xFF, 0xFF, 0xFF, 0x1F, 0x1F, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x1F, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 72
    0xF0, 0xF0, 0xF0, 0xFF, 0xFF, 0xFF, 0x3F, 0x3F, 0x3F, 0x00, 0x00, 0x00, // 73
    0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0xF0, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x38, 0x30, 0x30, 0x30, 0x38, 0x3F, 0x1F, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 74
    0xF0, 0xF0, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x80, 0xC0, 0xE0, 0xF0, 0x30, 0x10, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x78, 0x3C, 0x3E, 0x7F, 0xF7, 0xC3, 0x81, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3F, 0x3F, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x0F, 0x1F, 0x3E, 0x38, 0x30, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 75
    0xF8, 0xF8, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3F, 0x3F, 0x3F, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 76
    0xF0, 0xF0, 0xF0, 0x70, 0xF0, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xF0, 0x70, 0xF0, 0xF0, 0xF0, 0xFF, 0xFF, 0xFF, 0x00, 0x03, 0x1F, 0xFE, 0xF0, 0xC0, 0x00, 0xC0, 0xF0, 0xFE, 0x1F, 0x03, 0x00, 0xFF, 0xFF, 0xFF, 0x3F, 0x3F, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x03, 0x1F, 0x1E, 0x1F, 0x03, 0x00, 0x00, 0x00, 0x00, 0x3F, 0x3F, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 77
    0xF0, 0xF0, 0xF0, 0xF0, 0xE0, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0xF0, 0xF0, 0xFF, 0xFF, 0xFF, 0x00, 0x03, 0x07, 0x1E, 0x7C, 0xF0, 0xE0, 0x80, 0x00, 0xFF, 0xFF, 0xFF, 0x3F, 0x3F, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x07, 0x1E, 0x3F, 0x3F, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 78
    0x00, 0x80, 0xC0, 0xE0, 0x70, 0x30, 0x38, 0x18, 0x18, 0x18, 0x18, 0x38, 0x30, 0x70, 0xE0, 0xC0, 0x80, 0x00, 0xFE, 0xFF, 0xFF, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xFF, 0xFF, 0xFE, 0x00, 0x03, 0x07, 0x0F, 0x1C, 0x18, 0x38, 0x30, 0x30, 0x30, 0x30, 0x38, 0x18, 0x1C, 0x0F, 0x07, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 79
    0xF0, 0xF0, 0xF0, 0x30, 0x30, 0x30, 0x30, 0x30, 0x70, 0xE0, 0xE0, 0xC0, 0x80, 0xFF, 0xFF, 0xFF, 0x60, 0x60, 0x60, 0x60, 0x60, 0x70, 0x38, 0x3F, 0x1F, 0x0F, 0x3F, 0x3F, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 80
    0x00, 0x80, 0xC0, 0xE0, 0x70, 0x30, 0x38, 0x18, 0x18, 0x18, 0x18, 0x38, 0x30, 0x70, 0xE0, 0xC0, 0x80, 0x00, 0xFE, 0xFF, 0xFF, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x01, 0xFF, 0xFF, 0xFE, 0x00, 0x03, 0x07, 0x0F, 0x1C, 0x18, 0x38, 0x30, 0x31, 0x33, 0x37, 0x3F, 0x1E, 0x1C, 0x1F, 0x3F, 0x73, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 81
    0xF0, 0xF0, 0xF0, 0x30, 0x30, 0x30, 0x30, 0x30, 0x70, 0x70, 0xE0, 0xC0, 0x80, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x30, 0x30, 0x30, 0x30, 0x70, 0xF8, 0xD8, 0x8F, 0x0F, 0x07, 0x00, 0x00, 0x3F, 0x3F, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x0F, 0x3F, 0x3C, 0x30, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 82
    0x00, 0xC0, 0xE0, 0xF0, 0x38, 0x18, 0x18, 0x18, 0x18, 0x38, 0x70, 0x70, 0x20, 0x00, 0x07, 0x0F, 0x0F, 0x1E, 0x1C, 0x3C, 0x38, 0x78, 0xF0, 0xF0, 0xE0, 0xC0, 0x08, 0x1C, 0x1C, 0x18, 0x30, 0x30, 0x30, 0x30, 0x30, 0x38, 0x1F, 0x0F, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 83
    0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xF0, 0xF0, 0xF0, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3F, 0x3F, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 84
    0xF0, 0xF0, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0xF0, 0xF0, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x03, 0x0F, 0x1F, 0x1C, 0x38, 0x30, 0x30, 0x30, 0x30, 0x30, 0x38, 0x1C, 0x1F, 0x0F, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 85
    0x10, 0x70, 0xF0, 0xF0, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0xF0, 0xF0, 0x70, 0x00, 0x00, 0x03, 0x1F, 0x7F, 0xFC, 0xF0, 0x80, 0x00, 0x80, 0xF0, 0xFE, 0x3F, 0x0F, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x0F, 0x3F, 0x3C, 0x3F, 0x07, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 86
    0x10, 0xF0, 0xF0, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0xF0, 0xF0, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xF0, 0xF0, 0x70, 0x00, 0x01, 0x0F, 0x7F, 0xFE, 0xF0, 0x00, 0x80, 0xF8, 0xFF, 0x1F, 0x01, 0x07, 0x7F, 0xFE, 0xE0, 0x00, 0x80, 0xF8, 0xFF, 0x3F, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x3F, 0x3F, 0x3F, 0x0F, 0x01, 0x00, 0x00, 0x00, 0x00, 0x03, 0x3F, 0x3F, 0x3F, 0x1F, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 87
    0x10, 0x30, 0xF0, 0xF0, 0xE0, 0x80, 0x00, 0x00, 0x00, 0x00, 0x80, 0xE0, 0xF0, 0x70, 0x30, 0x10, 0x00, 0x00, 0x00, 0x01, 0x83, 0xEF, 0xFF, 0x7C, 0xFC, 0xFF, 0xCF, 0x83, 0x01, 0x00, 0x00, 0x00, 0x30, 0x38, 0x3E, 0x1F, 0x0F, 0x03, 0x01, 0x00, 0x00, 0x01, 0x07, 0x0F, 0x3F, 0x3C, 0x38, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 88
    0x10, 0x70, 0xF0, 0xE0, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0xE0, 0xF0, 0x70, 0x30, 0x00, 0x00, 0x00, 0x03, 0x07, 0x1F, 0xFE, 0xF8, 0xFC, 0x1F, 0x07, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3F, 0x3F, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 89
    0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xB0, 0xF0, 0xF0, 0xF0, 0x30, 0x00, 0x00, 0x00, 0x00, 0x80, 0xC0, 0xF0, 0x78, 0x3C, 0x1F, 0x0F, 0x03, 0x01, 0x00, 0x00, 0x00, 0x38, 0x3C, 0x3F, 0x3F, 0x3B, 0x39, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 90
    0xFE, 0xFE, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 91
    0x10, 0x70, 0xF0, 0xE0, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xE0, 0xF0, 0x70, 0x10, 0x00, 0xB0, 0xB0, 0xB3, 0xB7, 0xBF, 0xFE, 0xF8, 0xFC, 0xBF, 0xB7, 0xB3, 0xB0, 0xB0, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x1F, 0x1F, 0x1F, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 92
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0xFE, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 93
    0x40, 0x60, 0x30, 0x18, 0x18, 0x38, 0x70, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 94
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, // 95
    0x08, 0x18, 0x78, 0x70, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 96
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x9C, 0x9C, 0xCE, 0xC6, 0xC6, 0xC6, 0xCE, 0xFC, 0xFC, 0xF8, 0x00, 0x0F, 0x1F, 0x3F, 0x39, 0x30, 0x30, 0x30, 0x18, 0x0F, 0x3F, 0x3F, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 97
    0xF8, 0xF8, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x18, 0x0C, 0x06, 0x06, 0x06, 0x0E, 0x1E, 0xFC, 0xF8, 0xE0, 0x3F, 0x3F, 0x1F, 0x1C, 0x18, 0x30, 0x30, 0x30, 0x38, 0x3C, 0x1F, 0x0F, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 98
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0xF8, 0xFC, 0x1C, 0x0E, 0x06, 0x06, 0x06, 0x0E, 0x1C, 0x1C, 0x08, 0x03, 0x0F, 0x1F, 0x1C, 0x38, 0x30, 0x30, 0x30, 0x30, 0x38, 0x1C, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 99
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0xF8, 0xF8, 0xE0, 0xF8, 0xFC, 0x1E, 0x0E, 0x06, 0x06, 0x06, 0x0C, 0x1C, 0xFF, 0xFF, 0xFF, 0x03, 0x0F, 0x1F, 0x3C, 0x38, 0x30, 0x30, 0x30, 0x18, 0x0C, 0x3F, 0x3F, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 100
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0xF8, 0xFC, 0xCC, 0xC6, 0xC6, 0xC6, 0xC6, 0xCE, 0xFC, 0xF8, 0xE0, 0x03, 0x0F, 0x1F, 0x1C, 0x38, 0x30, 0x30, 0x30, 0x30, 0x38, 0x1C, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 101
    0x00, 0x00, 0xE0, 0xF0, 0xF8, 0x38, 0x18, 0x18, 0x18, 0x10, 0x06, 0x06, 0xFF, 0xFF, 0xFF, 0x06, 0x06, 0x06, 0x00, 0x00, 0x00, 0x00, 0x3F, 0x3F, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 102
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0xF8, 0xFC, 0x1C, 0x0E, 0x06, 0x06, 0x06, 0x0E, 0x1C, 0xFC, 0xFE, 0xFE, 0x41, 0xC3, 0xE7, 0xCF, 0x8E, 0x8C, 0x8C, 0x8C, 0xC6, 0xE7, 0xFF, 0x7F, 0x3F, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, // 103
    0xF8, 0xF8, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x18, 0x0C, 0x06, 0x06, 0x06, 0x0E, 0xFE, 0xFC, 0xF8, 0x3F, 0x3F, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3F, 0x3F, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 104
    0x70, 0x70, 0x70, 0xFE, 0xFE, 0xFE, 0x3F, 0x3F, 0x3F, 0x00, 0x00, 0x00, // 105
    0x00, 0x00, 0x00, 0x70, 0x70, 0x70, 0x00, 0x00, 0x00, 0xFE, 0xFE, 0xFE, 0xC0, 0xC0, 0xE0, 0xFF, 0x7F, 0x3F, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, // 106
    0xF8, 0xF8, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xC0, 0xE0, 0xF0, 0xB8, 0x1E, 0x0E, 0x06, 0x02, 0x3F, 0x3F, 0x3F, 0x01, 0x00, 0x03, 0x07, 0x1F, 0x3E, 0x38, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 107
    0xF8, 0xF8, 0xF8, 0xFF, 0xFF, 0xFF, 0x3F, 0x3F, 0x3F, 0x00, 0x00, 0x00, // 108
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFE, 0xFE, 0xFE, 0x1C, 0x0C, 0x06, 0x06, 0x0E, 0xFE, 0xFC, 0xF8, 0x0C, 0x06, 0x06, 0x0E, 0xFE, 0xFC, 0xF8, 0x3F, 0x3F, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3F, 0x3F, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x3F, 0x3F, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 109
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFE, 0xFE, 0xFE, 0x18, 0x0C, 0x06, 0x06, 0x06, 0x0E, 0xFE, 0xFC, 0xF8, 0x3F, 0x3F, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3F, 0x3F, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 110
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0xF8, 0xFC, 0x1C, 0x0E, 0x06, 0x06, 0x06, 0x0E, 0x1C, 0xFC, 0xF8, 0xE0, 0x03, 0x0F, 0x1F, 0x1C, 0x38, 0x30, 0x30, 0x30, 0x38, 0x1C, 0x1F, 0x0F, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 111
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFE, 0xFE, 0xFE, 0x18, 0x0C, 0x06, 0x06, 0x06, 0x0E, 0x1E, 0xFC, 0xF8, 0xF0, 0xFF, 0xFF, 0xFF, 0x0E, 0x0C, 0x18, 0x18, 0x18, 0x1C, 0x1E, 0x0F, 0x07, 0x03, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 112
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0xF8, 0xFC, 0x1E, 0x0E, 0x06, 0x06, 0x06, 0x0C, 0x1C, 0xFC, 0xFE, 0xFE, 0x03, 0x07, 0x0F, 0x1E, 0x1C, 0x18, 0x18, 0x18, 0x0C, 0x0E, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80, // 113
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFE, 0xFE, 0xFE, 0x70, 0x1C, 0x0E, 0x0E, 0x0E, 0x3F, 0x3F, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 114
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x38, 0x7C, 0xFE, 0xE6, 0xC6, 0xC6, 0x86, 0x8E, 0x8C, 0x04, 0x18, 0x18, 0x38, 0x30, 0x30, 0x31, 0x39, 0x3F, 0x1F, 0x0E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 115
    0x00, 0x00, 0xC0, 0xC0, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x06, 0x06, 0xFF, 0xFF, 0xFF, 0x06, 0x06, 0x06, 0x06, 0x00, 0x00, 0x0F, 0x1F, 0x3F, 0x30, 0x30, 0x30, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 116
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFE, 0xFE, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFE, 0xFE, 0xFE, 0x0F, 0x1F, 0x3F, 0x38, 0x30, 0x30, 0x30, 0x18, 0x0C, 0x3F, 0x3F, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 117
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x0E, 0x7E, 0xFE, 0xF0, 0x80, 0x00, 0x80, 0xF0, 0xFC, 0x7E, 0x0E, 0x02, 0x00, 0x00, 0x00, 0x01, 0x0F, 0x3F, 0x3C, 0x3F, 0x0F, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 118
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x1E, 0xFE, 0xFC, 0xC0, 0x00, 0x00, 0xF0, 0xFE, 0x1E, 0x3E, 0xFC, 0xE0, 0x00, 0x00, 0xC0, 0xFC, 0xFE, 0x1E, 0x02, 0x00, 0x00, 0x00, 0x07, 0x3F, 0x3E, 0x3F, 0x0F, 0x01, 0x00, 0x00, 0x01, 0x0F, 0x3F, 0x3E, 0x3F, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 119
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x0E, 0x1E, 0x7C, 0xF0, 0xE0, 0xF0, 0x78, 0x1E, 0x0E, 0x06, 0x00, 0x20, 0x30, 0x3C, 0x3E, 0x0F, 0x07, 0x01, 0x07, 0x0F, 0x3E, 0x3C, 0x30, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 120
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x1C, 0x7C, 0xFC, 0xE0, 0x80, 0x00, 0x00, 0xC0, 0xF8, 0x7C, 0x1C, 0x04, 0x00, 0x80, 0x80, 0x81, 0xC7, 0xFF, 0x7E, 0x3F, 0x0F, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 121
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x06, 0x06, 0x86, 0xC6, 0xF6, 0xFE, 0x3E, 0x1E, 0x06, 0x38, 0x3C, 0x3F, 0x37, 0x33, 0x31, 0x30, 0x30, 0x30, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 122
    0x00, 0x00, 0xF8, 0xFC, 0x0E, 0x06, 0x30, 0x78, 0xEF, 0xC7, 0x00, 0x00, 0x00, 0x00, 0x3F, 0x7F, 0xE0, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 123
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x80, 0x80, // 124
    0x06, 0x0E, 0xFC, 0xF8, 0x00, 0x00, 0x00, 0x00, 0xC7, 0xEF, 0x78, 0x30, 0xC0, 0xE0, 0x7F, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 125
    0x30, 0x18, 0x18, 0x18, 0x38, 0x30, 0x30, 0x38, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 126
    0xF8, 0xF8, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0xF8, 0xF8, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x3F, 0x3F, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x3F, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 // 127
    
};

#endif
