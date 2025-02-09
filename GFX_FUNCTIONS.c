#include "GFX_FUNCTIONS.h"

extern int16_t _width;       ///< Display width as modified by current rotation
extern int16_t _height;      ///< Display height as modified by current rotation
extern int16_t cursor_x;     ///< x location to start print()ing text
extern int16_t cursor_y;     ///< y location to start print()ing text
extern uint8_t rotation;     ///< Display rotation (0 thru 3)
extern uint8_t _colstart;   ///< Some displays need this changed to offset
extern uint8_t _rowstart;       ///< Some displays need this changed to offset
extern uint8_t _xstart;
extern uint8_t _ystart;

/*----------------------------------------------------------------------*/
/*----------------------------Draw Pixel--------------------------------*/
/*----------------------------------------------------------------------*/
/* Draw One Pixel.
 * @param SPIx 	    Selected SPI
 * @param x  		x axis
 * @param y  		y axis
 * @param color  	color of pixel
 */
void drawPixel(SPI_TypeDef *SPIx,  int16_t x, int16_t y, uint16_t color)
{
	ST7735_DrawPixel(SPIx, x, y, color);
}
/*---------------------ST7735 Fill Rectangle----------------------------*/
/*----------------------------------------------------------------------*/
/* Draw Fill Rectangle.
 * @param SPIx 	    Selected SPI
 * @param x  		x axis
 * @param y  		y axis
 * @param w  		width
 * @param h  		hight
 * @param color  	Rectangle color
 */
void fillRect(SPI_TypeDef *SPIx,  int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
	ST7735_FillRectangle(SPIx, x, y, w, h, color);
}


/***********************************************************************************************************/


#define _swap_int16_t(a, b)                                                    \
  {                                                                            \
    int16_t t = a;                                                             \
    a = b;                                                                     \
    b = t;                                                                     \
  }

#define min(a, b) (((a) < (b)) ? (a) : (b))

/*----------------------------------------------------------------------*/
/*----------------------------Draw Line---------------------------------*/
/*----------------------------------------------------------------------*/
/* Draw Line.
 * @param SPIx 	    Selected SPI
 * @param SPIx 	    Selected SPI
 * @param x0 		first x
 * @param y0 		first y
 * @param x1 		Second x
 * @param y1 		Second y
 * @param color  	color
 */
void drawLine(SPI_TypeDef *SPIx,  int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color)
{
    int16_t steep = abs(y1 - y0) > abs(x1 - x0);
    if (steep) {
        _swap_int16_t(x0, y0);
        _swap_int16_t(x1, y1);
    }

    if (x0 > x1) {
        _swap_int16_t(x0, x1);
        _swap_int16_t(y0, y1);
    }

    int16_t dx, dy;
    dx = x1 - x0;
    dy = abs(y1 - y0);

    int16_t err = dx / 2;
    int16_t ystep;

    if (y0 < y1) {
        ystep = 1;
    } else {
        ystep = -1;
    }

    for (; x0<=x1; x0++) {
        if (steep) {
        	drawPixel(SPIx, y0, x0, color);
        } else {
        	drawPixel(SPIx, x0, y0, color);
        }
        err -= dy;
        if (err < 0) {
            y0 += ystep;
            err += dx;
        }
    }
}
/*----------------------------------------------------------------------*/
/*-------------------------Draw Fast H Line-----------------------------*/
/*----------------------------------------------------------------------*/
/* Draw Fast Hight Line(not faster than drawLine).
 * @param SPIx 	    Selected SPI
 * @param x  		x axis
 * @param y  		y axis
 * @param h  		Hight
 * @param color  	color
 */
void  drawFastHLine(SPI_TypeDef *SPIx,  int16_t x, int16_t y, int16_t h, uint16_t color)
{
	drawLine(SPIx, x, y, x, y + h - 1, color);
}
/*----------------------------------------------------------------------*/
/*-------------------------Draw Fast W Line-----------------------------*/
/*----------------------------------------------------------------------*/
/* Draw Fast Width Line(not faster than drawLine).
 * @param SPIx 	    Selected SPI
 * @param x  		x axis
 * @param y  		y axis
 * @param w  		Width
 * @param color  	color
 */
void  drawFastWLine(SPI_TypeDef *SPIx,  int16_t x, int16_t y, int16_t w, uint16_t color)
{
	drawLine(SPIx, x, y, x + w - 1, y, color);
}
/*----------------------------------------------------------------------*/
/*---------------------------Draw Circle--------------------------------*/
/*----------------------------------------------------------------------*/
/* Draw Circle.
 * @param SPIx 	    Selected SPI
 * @param x0  		x center
 * @param y0  		y center
 * @param r  		Radius
 * @param color  	color
 */
void drawCircle(SPI_TypeDef *SPIx,  int16_t x0, int16_t y0, int16_t r, uint16_t color)
{
    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;

    drawPixel(SPIx, x0  , y0+r, color);
    drawPixel(SPIx, x0  , y0-r, color);
    drawPixel(SPIx, x0+r, y0  , color);
    drawPixel(SPIx, x0-r, y0  , color);

    while (x<y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        drawPixel(SPIx, x0 + x, y0 + y, color);
        drawPixel(SPIx, x0 - x, y0 + y, color);
        drawPixel(SPIx, x0 + x, y0 - y, color);
        drawPixel(SPIx, x0 - x, y0 - y, color);
        drawPixel(SPIx, x0 + y, y0 + x, color);
        drawPixel(SPIx, x0 - y, y0 + x, color);
        drawPixel(SPIx, x0 + y, y0 - x, color);
        drawPixel(SPIx, x0 - y, y0 - x, color);
    }
}

/*----------------------------------------------------------------------*/
/*-------------------------Draw Part Circle ----------------------------*/
/*----------------------------------------------------------------------*/
/* Drawing parts of a circle.
 * @param SPIx 	     Selected SPI
 * @param x0  		 x center
 * @param y0  		 y center
 * @param r  		 Radius
 * @param cornername part of circle
 * @param color  	 color
 */
void drawPartCircle(SPI_TypeDef *SPIx, int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t color)
{
    int16_t f     = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x     = 0;
    int16_t y     = r;

    while (x<y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f     += ddF_y;
        }
        x++;
        ddF_x += 2;
        f     += ddF_x;
        if (cornername & 0x4) {
        	drawPixel(SPIx, x0 + x, y0 + y, color);
            drawPixel(SPIx, x0 + y, y0 + x, color);
        }
        if (cornername & 0x2) {
        	drawPixel(SPIx, x0 + x, y0 - y, color);
            drawPixel(SPIx, x0 + y, y0 - x, color);
        }
        if (cornername & 0x8) {
        	drawPixel(SPIx, x0 - y, y0 + x, color);
            drawPixel(SPIx, x0 - x, y0 + y, color);
        }
        if (cornername & 0x1) {
        	drawPixel(SPIx, x0 - y, y0 - x, color);
            drawPixel(SPIx, x0 - x, y0 - y, color);
        }
    }
}
/*----------------------------------------------------------------------*/
/*------------------------Draw Fill Part Circle-------------------------*/
/*----------------------------------------------------------------------*/
/* Draw Fill Part Circle.
 * @param SPIx 	     Selected SPI
 * @param x0  		 x center
 * @param y0  		 y center
 * @param r  		 Radius
 * @param corners 	 part of circle
 * @param delta 	 shift
 * @param color  	 color
 */
void fillPartCircle(SPI_TypeDef *SPIx, int16_t x0, int16_t y0, int16_t r, uint8_t corners, int16_t delta, uint16_t color)
{

    int16_t f     = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x     = 0;
    int16_t y     = r;
    int16_t px    = x;
    int16_t py    = y;

    delta++; // Avoid some +1's in the loop

    while(x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f     += ddF_y;
        }
        x++;
        ddF_x += 2;
        f     += ddF_x;
        // These checks avoid double-drawing certain lines, important
        // for the SSD1306 library which has an INVERT drawing mode.
        if(x < (y + 1)) {
            if(corners & 1) drawFastHLine(SPIx, x0+x, y0-y, 2*y+delta, color);
            if(corners & 2) drawFastHLine(SPIx, x0-x, y0-y, 2*y+delta, color);
        }
        if(y != py) {
            if(corners & 1) drawFastHLine(SPIx, x0+py, y0-px, 2*px+delta, color);
            if(corners & 2) drawFastHLine(SPIx, x0-py, y0-px, 2*px+delta, color);
            py = y;
        }
        px = x;
    }
}
/*----------------------------------------------------------------------*/
/*--------------------------Draw Fill Circle----------------------------*/
/*----------------------------------------------------------------------*/
/* Draw Fill Part Circle.
 * @param SPIx 	     Selected SPI
 * @param x0  		 x center
 * @param y0  		 y center
 * @param r  		 Radius
 * @param color  	 color
 */
void fillCircle(SPI_TypeDef *SPIx, int16_t x0, int16_t y0, int16_t r, uint16_t color)
{
    drawFastHLine(SPIx, x0, y0-r, 2*r+1, color);
    fillPartCircle(SPIx, x0, y0, r, 3, 0, color);
}


/*----------------------------------------------------------------------*/
/*------------------------Draw Rectangle--------------------------------*/
/*----------------------------------------------------------------------*/
/* Draw Rectangle.
 * @param SPIx 	    Selected SPI
 * @param x  		x axis
 * @param y  		y axis
 * @param w  		width
 * @param h  		hight
 * @param color  	Rectangle color
 */
void drawRect(SPI_TypeDef *SPIx, int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
    drawFastWLine(SPIx, x, y, w, color);
    drawFastWLine(SPIx, x, y+h-1, w, color);
    drawFastHLine(SPIx, x, y, h, color);
    drawFastHLine(SPIx, x+w-1, y, h, color);
}
/*----------------------------------------------------------------------*/
/*------------------------Draw Round Rectangle--------------------------*/
/*----------------------------------------------------------------------*/
/* Draw Rectangle with round corner.
 * @param SPIx 	    Selected SPI
 * @param x  		x axis
 * @param y  		y axis
 * @param w  		width
 * @param h  		hight
 * @param r  		Radius
 * @param color  	Rectangle color
 */
void drawRoundRect(SPI_TypeDef *SPIx, int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color)
{
    int16_t max_radius = ((w < h) ? w : h) / 2; // 1/2 minor axis
    if(r > max_radius) r = max_radius;
    // smarter version
    drawFastWLine(SPIx, x+r  , y    , w-2*r, color); // Top
    drawFastWLine(SPIx, x+r  , y+h-1, w-2*r, color); // Bottom
    drawFastHLine(SPIx, x    , y+r  , h-2*r, color); // Left
    drawFastHLine(SPIx, x+w-1, y+r  , h-2*r, color); // Right
    // draw four corners
    drawPartCircle(SPIx, x+r    , y+r    , r, 1, color);
    drawPartCircle(SPIx, x+w-r-1, y+r    , r, 2, color);
    drawPartCircle(SPIx, x+w-r-1, y+h-r-1, r, 4, color);
    drawPartCircle(SPIx, x+r    , y+h-r-1, r, 8, color);
}

/*----------------------------------------------------------------------*/
/*----------------------Draw Fill Round Rectangle-----------------------*/
/*----------------------------------------------------------------------*/
/* Draw Fill Rectangle with round corner.
 * @param SPIx 	    Selected SPI
 * @param x  		x axis
 * @param y  		y axis
 * @param w  		width
 * @param h  		hight
 * @param r  		Radius
 * @param color  	Rectangle color
 */
void fillRoundRect(SPI_TypeDef *SPIx, int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color)
{
    int16_t max_radius = ((w < h) ? w : h) / 2; // 1/2 minor axis
    if(r > max_radius) r = max_radius;
    // smarter version
    fillRect(SPIx, x+r, y, w-2*r, h, color);
    // draw four corners
    fillPartCircle(SPIx, x+w-r-1, y+r, r, 1, h-2*r-1, color);
    fillPartCircle(SPIx, x+r    , y+r, r, 2, h-2*r-1, color);
}

/*----------------------------------------------------------------------*/
/*---------------------------Draw Triangle------------------------------*/
/*----------------------------------------------------------------------*/
/* Draw Triangle.
 * @param SPIx 	    Selected SPI
 * @param x0  		First x
 * @param y0  		First y
 * @param x1  		Second x
 * @param y1  		Second y
 * @param x2  		Third x
 * @param y2  		Third y
 * @param color  	Triangle color
 */
void drawTriangle(SPI_TypeDef *SPIx, int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color)
{
    drawLine(SPIx, x0, y0, x1, y1, color);
    drawLine(SPIx, x1, y1, x2, y2, color);
    drawLine(SPIx, x2, y2, x0, y0, color);
}

/*----------------------------------------------------------------------*/
/*-------------------------Draw Fill Triangle---------------------------*/
/*----------------------------------------------------------------------*/
/* Draw Fill Triangle.
 * @param SPIx 	    Selected SPI
 * @param x0  		First x
 * @param y0  		First y
 * @param x1  		Second x
 * @param y1  		Second y
 * @param x2  		Third x
 * @param y2  		Third y
 * @param color  	Triangle color
 */
void fillTriangle(SPI_TypeDef *SPIx, int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color)
{

    int16_t a, b, y, last;

    // Sort coordinates by Y order (y2 >= y1 >= y0)
    if (y0 > y1) {
        _swap_int16_t(y0, y1); _swap_int16_t(x0, x1);
    }
    if (y1 > y2) {
        _swap_int16_t(y2, y1); _swap_int16_t(x2, x1);
    }
    if (y0 > y1) {
        _swap_int16_t(y0, y1); _swap_int16_t(x0, x1);
    }

    if(y0 == y2) { // Handle awkward all-on-same-line case as its own thing
        a = b = x0;
        if(x1 < a)      a = x1;
        else if(x1 > b) b = x1;
        if(x2 < a)      a = x2;
        else if(x2 > b) b = x2;
        drawFastWLine(SPIx, a, y0, b-a+1, color);
        return;
    }

    int16_t
    dx01 = x1 - x0,
    dy01 = y1 - y0,
    dx02 = x2 - x0,
    dy02 = y2 - y0,
    dx12 = x2 - x1,
    dy12 = y2 - y1;
    int32_t
    sa   = 0,
    sb   = 0;

    // For upper part of triangle, find scanline crossings for segments
    // 0-1 and 0-2.  If y1=y2 (flat-bottomed triangle), the scanline y1
    // is included here (and second loop will be skipped, avoiding a /0
    // error there), otherwise scanline y1 is skipped here and handled
    // in the second loop...which also avoids a /0 error here if y0=y1
    // (flat-topped triangle).
    if(y1 == y2) last = y1;   // Include y1 scanline
    else         last = y1-1; // Skip it

    for(y=y0; y<=last; y++) {
        a   = x0 + sa / dy01;
        b   = x0 + sb / dy02;
        sa += dx01;
        sb += dx02;
        /* longhand:
        a = x0 + (x1 - x0) * (y - y0) / (y1 - y0);
        b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
        */
        if(a > b) _swap_int16_t(a,b);
        drawFastWLine(SPIx, a, y, b-a+1, color);
    }

    // For lower part of triangle, find scanline crossings for segments
    // 0-2 and 1-2.  This loop is skipped if y1=y2.
    sa = (int32_t)dx12 * (y - y1);
    sb = (int32_t)dx02 * (y - y0);
    for(; y<=y2; y++) {
        a   = x1 + sa / dy12;
        b   = x0 + sb / dy02;
        sa += dx12;
        sb += dx02;
        /* longhand:
        a = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
        b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
        */
        if(a > b) _swap_int16_t(a,b);
        drawFastWLine(SPIx, a, y, b-a+1, color);
    }
}
/*----------------------------------------------------------------------*/
/*--------------------------Fill Screen---------------------------------*/
/*----------------------------------------------------------------------*/
/* Fill Screen.
 * @param SPIx 	    Selected SPI
 * @param color  	Screen Color
 */
void fillScreen(SPI_TypeDef *SPIx, uint16_t color) {
    fillRect(SPIx, 0, 0, _width, _height, color);
}

