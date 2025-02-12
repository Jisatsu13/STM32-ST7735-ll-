#ifndef INC_GFX_FUNCTIONS_H_
#define INC_GFX_FUNCTIONS_H_

#include "ST7735.h"
#include "stdint.h"
#include "stdlib.h"

void drawPixel(SPI_TypeDef *SPIx, int16_t x, int16_t y, uint16_t color);
void drawFastHLine(SPI_TypeDef *SPIx, int16_t x, int16_t y, int16_t h, uint16_t color);
void drawFastWLine(SPI_TypeDef *SPIx, int16_t x, int16_t y, int16_t w, uint16_t color);
void fillRect(SPI_TypeDef *SPIx, int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
void drawLine(SPI_TypeDef *SPIx, int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
void drawCircle(SPI_TypeDef *SPIx, int16_t x0, int16_t y0, int16_t r, uint16_t color);
void drawPartCircle(SPI_TypeDef *SPIx, int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t color);
void fillPartCircle(SPI_TypeDef *SPIx, int16_t x0, int16_t y0, int16_t r, uint8_t corners, int16_t delta, uint16_t color);
void fillCircle(SPI_TypeDef *SPIx, int16_t x0, int16_t y0, int16_t r, uint16_t color);
void drawRect(SPI_TypeDef *SPIx, int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
void drawRoundRect(SPI_TypeDef *SPIx, int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color);
void fillRoundRect(SPI_TypeDef *SPIx, int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color);
void drawTriangle(SPI_TypeDef *SPIx, int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
void fillTriangle(SPI_TypeDef *SPIx, int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
void fillScreen(SPI_TypeDef *SPIx, uint16_t color);


#endif /* INC_GFX_FUNCTIONS_H_ */
