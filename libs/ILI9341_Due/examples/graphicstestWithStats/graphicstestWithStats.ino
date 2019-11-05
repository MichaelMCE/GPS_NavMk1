/***************************************************
This is our GFX example for the Adafruit ILI9341 Breakout and Shield
----> http://www.adafruit.com/products/1651

Check out the links above for our tutorials and wiring diagrams
These displays use SPI to communicate, 4 or 5 pins are required to
interface (RST is optional)
Adafruit invests time and resources providing this open source code,
please support Adafruit and open-source hardware by purchasing
products from Adafruit!

Written by Limor Fried/Ladyada for Adafruit Industries.
MIT license, all text above must be included in any redistribution
****************************************************/

#pragma GCC optimize ("-O2") 

#include <SPI.h>
#include <ILI9341_due_config.h>
#include <ILI9341_due.h>
#include "fonts\SystemFont5x7.h"
#include "fonts\Arial_bold_14.h"

#define TFT_RST 8
#define TFT_DC 9
#define TFT_CS 10

// Use hardware SPI (on Uno, #13, #12, #11) and the above for CS/DC
ILI9341_due tft = ILI9341_due(TFT_CS, TFT_DC, TFT_RST);

long Screenfill,
	Text,
	Lines,
	HorizVertLines,
	Rectanglesoutline,
	Rectanglesfilled,
	Circlesfilled,
	Circlesoutline,
	Trianglesoutline,
	Trianglesfilled,
	Roundedrectsoutline,
	Roundedrectsfilled;

void setup() {

	SerialUSB.begin(9600);
	while (!SerialUSB) ; // wait for Arduino Serial Monitor
	SerialUSB.println(F("ILI9341 Test!")); 

	tft.begin();
	tft.setRotation(iliRotation270);
	tft.fillScreen(ILI9341_BLUE);
	
	tft.setFont(Arial_bold_14);
	tft.setTextLetterSpacing(2);
	tft.setTextColor(ILI9341_WHITE, ILI9341_BLUE);

#if SPI_MODE_NORMAL
	char mode[] = "SPI_MODE_NORMAL";
#elif SPI_MODE_EXTENDED
	char mode[] = "SPI_MODE_EXTENDED";
#elif SPI_MODE_DMA
	char mode[] = "SPI_MODE_DMA";

#endif
	tft.printAligned(mode, gTextAlignMiddleCenter);
	delay(3000);
	tft.setRotation(iliRotation0);

	// read diagnostics (optional but can help debug problems)
	uint8_t x = tft.readcommand8(ILI9341_RDMODE);
	SerialUSB.print(F("Display Power Mode: 0x")); SerialUSB.println(x, HEX);
	x = tft.readcommand8(ILI9341_RDMADCTL);
	SerialUSB.print(F("MADCTL Mode: 0x")); SerialUSB.println(x, HEX);
	x = tft.readcommand8(ILI9341_RDPIXFMT);
	SerialUSB.print(F("Pixel Format: 0x")); SerialUSB.println(x, HEX);
	x = tft.readcommand8(ILI9341_RDIMGFMT);
	SerialUSB.print(F("Image Format: 0x")); SerialUSB.println(x, HEX);
	x = tft.readcommand8(ILI9341_RDSELFDIAG);
	SerialUSB.print(F("Self Diagnostic: 0x")); SerialUSB.println(x, HEX); 

	SerialUSB.println(F("Benchmark                Time (microseconds)"));

	SerialUSB.print(F("Screen fill              "));
	unsigned long start = micros();
	SerialUSB.println(Screenfill = testFillScreen());
	// delay(200);

	SerialUSB.print(F("Text                     "));
	SerialUSB.println(Text = testText());
	// delay(200);

	SerialUSB.print(F("Lines                    "));
	SerialUSB.println(Lines = testLines(ILI9341_CYAN));
	// delay(200);

	SerialUSB.print(F("Horiz/Vert Lines         "));
	SerialUSB.println(HorizVertLines = testFastLines(ILI9341_RED, ILI9341_BLUE));
	// delay(200);

	SerialUSB.print(F("Rectangles (outline)     "));
	SerialUSB.println(Rectanglesoutline = testRects(ILI9341_GREEN));
	// delay(200);

	SerialUSB.print(F("Rectangles (filled)      "));
	SerialUSB.println(Rectanglesfilled = testFilledRects(ILI9341_YELLOW, ILI9341_MAGENTA));
	// delay(200);

	SerialUSB.print(F("Circles (filled)         "));
	SerialUSB.println(Circlesfilled = testFilledCircles(10, ILI9341_MAGENTA));

	SerialUSB.print(F("Circles (outline)        "));
	SerialUSB.println(Circlesoutline = testCircles(10, ILI9341_WHITE));
	// delay(200);

	SerialUSB.print(F("Triangles (outline)      "));
	SerialUSB.println(Trianglesoutline = testTriangles());
	// delay(200);

	SerialUSB.print(F("Triangles (filled)       "));
	SerialUSB.println(Trianglesfilled = testFilledTriangles());
	// delay(200);

	SerialUSB.print(F("Rounded rects (outline)  "));
	SerialUSB.println(Roundedrectsoutline = testRoundRects());
	// delay(200);

	SerialUSB.print(F("Rounded rects (filled)   "));
	SerialUSB.println(Roundedrectsfilled = testFilledRoundRects());
	// delay(200);


	SerialUSB.println(F("Done!"));

	tft.fillScreen(ILI9341_BLUE);
	tft.setRotation(iliRotation270);
	tft.setFont(Arial_bold_14);
	tft.setTextColor(ILI9341_WHITE, ILI9341_BLUE);
	tft.setTextArea(30, 30, 26, 12, Arial_bold_14);
	tft.cursorToXY(45, 80);
	tft.print(F("Total time: "));
	tft.print((micros() - start)/1000);
	tft.print(F(" ms"));

	delay(2000);
	//printStats();

}

void printStats()
{
	tft.fillScreen(ILI9341_BLUE);
	
	tft.cursorTo(0, 0);
	tft.print(F("Screen fill              ")); tft.cursorTo(18); tft.print(Screenfill);
	tft.cursorTo(0, 1);
	tft.print(F("Text                     ")); tft.cursorTo(18); tft.print(Text);
	tft.cursorTo(0, 2);
	tft.print(F("Lines                    ")); tft.cursorTo(18); tft.print(Lines);
	tft.cursorTo(0, 3);
	tft.print(F("Horiz/Vert Lines         ")); tft.cursorTo(18); tft.print(HorizVertLines);
	tft.cursorTo(0, 4);
	tft.print(F("Rectangles (outline)     ")); tft.cursorTo(18); tft.print(Rectanglesoutline);
	tft.cursorTo(0, 5);
	tft.print(F("Rectangles (filled)      ")); tft.cursorTo(18); tft.print(Rectanglesfilled);
	tft.cursorTo(0, 6);
	tft.print(F("Circles (filled)         ")); tft.cursorTo(18); tft.print(Circlesfilled);
	tft.cursorTo(0, 7);
	tft.print(F("Circles (outline)        ")); tft.cursorTo(18); tft.print(Circlesoutline);
	tft.cursorTo(0, 8);
	tft.print(F("Triangles (outline)      ")); tft.cursorTo(18); tft.print(Trianglesoutline);
	tft.cursorTo(0, 9);
	tft.print(F("Triangles (filled)       ")); tft.cursorTo(18); tft.print(Trianglesfilled);
	tft.cursorTo(0, 10);
	tft.print(F("Rounded rects (outline)  ")); tft.cursorTo(18); tft.print(Roundedrectsoutline);
	tft.cursorTo(0, 11);
	tft.print(F("Rounded rects (filled)   ")); tft.cursorTo(18); tft.print(Roundedrectsfilled);

}

void loop(void) {
	/*for(uint8_t rotation=0; rotation<4; rotation++) {
	tft.setRotation(rotation);
	testText();
	delay(1000);
	}*/
}

unsigned long testFillScreen() {
	unsigned long start = micros();
	tft.fillScreen(ILI9341_BLACK);
	tft.fillScreen(ILI9341_RED);
	tft.fillScreen(ILI9341_GREEN);
	tft.fillScreen(ILI9341_BLUE);
	tft.fillScreen(ILI9341_BLACK);
	return micros() - start;
}

unsigned long testText() {
	tft.fillScreen(ILI9341_BLACK);
	tft.setFont(SystemFont5x7);
	unsigned long start = micros();
	tft.cursorTo(0, 0);
	tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);  tft.setTextScale(1);
	tft.println(F("Hello World!"));
	tft.setTextColor(ILI9341_YELLOW); tft.setTextScale(2);
	tft.println(1234.56);
	tft.setTextColor(ILI9341_RED);    tft.setTextScale(3);
	tft.println(0xDEADBEEF, HEX);
	tft.println();
	tft.setTextColor(ILI9341_GREEN);
	tft.setTextScale(5);
	tft.println(F("Groop"));
	tft.setTextScale(2);
	tft.println(F("I implore thee,"));
	tft.setTextScale(1);
	tft.println(F("my foonting turlingdromes."));
	tft.println(F("And hooptiously drangle me"));
	tft.println(F("with crinkly bindlewurdles,"));
	tft.println(F("Or I will rend thee"));
	tft.println(F("in the gobberwarts"));
	tft.println(F("with my blurglecruncheon,"));
	tft.println(F("see if I don't!"));
	return micros() - start;
}

unsigned long testLines(uint16_t color) {
	unsigned long start, t;
	int           x1, y1, x2, y2,
		w = tft.width(),
		h = tft.height();

	tft.fillScreen(ILI9341_BLACK);

	x1 = y1 = 0;
	y2    = h - 1;
	start = micros();
	for(x2=0; x2<w; x2+=6) tft.drawLine(x1, y1, x2, y2, color);
	x2    = w - 1;
	for(y2=0; y2<h; y2+=6) tft.drawLine(x1, y1, x2, y2, color);
	t     = micros() - start; // fillScreen doesn't count against timing

	tft.fillScreen(ILI9341_BLACK);

	x1    = w - 1;
	y1    = 0;
	y2    = h - 1;
	start = micros();
	for(x2=0; x2<w; x2+=6) tft.drawLine(x1, y1, x2, y2, color);
	x2    = 0;
	for(y2=0; y2<h; y2+=6) tft.drawLine(x1, y1, x2, y2, color);
	t    += micros() - start;

	tft.fillScreen(ILI9341_BLACK);

	x1    = 0;
	y1    = h - 1;
	y2    = 0;
	start = micros();
	for(x2=0; x2<w; x2+=6) tft.drawLine(x1, y1, x2, y2, color);
	x2    = w - 1;
	for(y2=0; y2<h; y2+=6) tft.drawLine(x1, y1, x2, y2, color);
	t    += micros() - start;

	tft.fillScreen(ILI9341_BLACK);

	x1    = w - 1;
	y1    = h - 1;
	y2    = 0;
	start = micros();
	for(x2=0; x2<w; x2+=6) tft.drawLine(x1, y1, x2, y2, color);
	x2    = 0;
	for(y2=0; y2<h; y2+=6) tft.drawLine(x1, y1, x2, y2, color);
	t    += micros() - start;
	
	return t
}

unsigned long testFastLines(uint16_t color1, uint16_t color2) {
	unsigned long start;
	int           x, y, w = tft.width(), h = tft.height();

	tft.fillScreen(ILI9341_BLACK);
	start = micros();
	for(y=0; y<h; y+=5) tft.drawFastHLine(0, y, w, color1);
	for(x=0; x<w; x+=5) tft.drawFastVLine(x, 0, h, color2);

	return micros() - start;
}

unsigned long testRects(uint16_t color) {
	unsigned long start;
	int           n, i, i2,
		cx = tft.width()  / 2,
		cy = tft.height() / 2;

	tft.fillScreen(ILI9341_BLACK);
	n     = min(tft.width(), tft.height());
	start = micros();
	for(i=2; i<n; i+=6) {
		i2 = i / 2;
		tft.drawRect(cx-i2, cy-i2, i, i, color);
	}

	return micros() - start;
}

unsigned long testFilledRects(uint16_t color1, uint16_t color2) {
	unsigned long start, t = 0;
	int           n, i, i2,
		cx = tft.width()  / 2 - 1,
		cy = tft.height() / 2 - 1;

	tft.fillScreen(ILI9341_BLACK);
	n = min(tft.width(), tft.height());
	for(i=n; i>0; i-=6) {
		i2    = i / 2;
		start = micros();
		tft.fillRect(cx-i2, cy-i2, i, i, color1);
		t    += micros() - start;
		// Outlines are not included in timing results
		tft.drawRect(cx-i2, cy-i2, i, i, color2);
	}

	return t;
}

unsigned long testFilledCircles(uint8_t radius, uint16_t color) {
	unsigned long start;
	int x, y, w = tft.width(), h = tft.height(), r2 = radius * 2;

	tft.fillScreen(ILI9341_BLACK);
	start = micros();
	for(x=radius; x<w; x+=r2) {
		for(y=radius; y<h; y+=r2) {
			tft.fillCircle(x, y, radius, color);
		}
	}

	return micros() - start;
}

unsigned long testCircles(uint8_t radius, uint16_t color) {
	unsigned long start;
	int           x, y, r2 = radius * 2,
		w = tft.width()  + radius,
		h = tft.height() + radius;

	// Screen is not cleared for this one -- this is
	// intentional and does not affect the reported time.
	start = micros();
	for(x=0; x<w; x+=r2) {
		for(y=0; y<h; y+=r2) {
			tft.drawCircle(x, y, radius, color);
		}
	}

	return micros() - start;
}

unsigned long testTriangles() {
	unsigned long start;
	int           n, i, cx = tft.width()  / 2 - 1,
		cy = tft.height() / 2 - 1;

	tft.fillScreen(ILI9341_BLACK);
	n     = min(cx, cy);
	start = micros();
	for(i=0; i<n; i+=5) {
		tft.drawTriangle(
			cx    , cy - i, // peak
			cx - i, cy + i, // bottom left
			cx + i, cy + i, // bottom right
			tft.color565(0, 0, i));
	}

	return micros() - start;
}

unsigned long testFilledTriangles() {
	unsigned long start, t = 0;
	int           i, cx = tft.width()  / 2 - 1,
		cy = tft.height() / 2 - 1;

	tft.fillScreen(ILI9341_BLACK);
	start = micros();
	for(i=min(cx,cy); i>10; i-=5) {
		start = micros();
		tft.fillTriangle(cx, cy - i, cx - i, cy + i, cx + i, cy + i,
			tft.color565(0, i, i));
		t += micros() - start;
		tft.drawTriangle(cx, cy - i, cx - i, cy + i, cx + i, cy + i,
			tft.color565(i, i, 0));
	}

	return t;
}

unsigned long testRoundRects() {
	unsigned long start;
	int           w, i, i2,
		cx = tft.width()  / 2 - 1,
		cy = tft.height() / 2 - 1;

	tft.fillScreen(ILI9341_BLACK);
	w     = min(tft.width(), tft.height());
	start = micros();
	for(i=0; i<w; i+=6) {
		i2 = i / 2;
		tft.drawRoundRect(cx-i2, cy-i2, i, i, i/8, tft.color565(i, 0, 0));
	}

	return micros() - start;
}

unsigned long testFilledRoundRects() {
	unsigned long start;
	int           i, i2,
		cx = tft.width()  / 2 - 1,
		cy = tft.height() / 2 - 1;

	tft.fillScreen(ILI9341_BLACK);
	start = micros();

	for(i=min(tft.width(), tft.height()); i>20; i-=6) {
		i2 = i / 2;
		tft.fillRoundRect(cx-i2, cy-i2, i, i, i/8, tft.color565(0, i, 0));
	}

	return micros() - start;
}
