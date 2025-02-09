#include "ST7735.h"

#include "stdlib.h"

#define TFT_LED_H() LL_GPIO_SetOutputPin(LED_ST7735_GPIO_Port, LED_ST7735_Pin)
#define TFT_LED_L() LL_GPIO_ResetOutputPin(LED_ST7735_GPIO_Port, LED_ST7735_Pin)

#define TFT_CS_H() LL_GPIO_SetOutputPin(CS_ST7735_GPIO_Port, CS_ST7735_Pin)
#define TFT_CS_L() LL_GPIO_ResetOutputPin(CS_ST7735_GPIO_Port, CS_ST7735_Pin)

#define TFT_A0_H() LL_GPIO_SetOutputPin(A0_ST7735_GPIO_Port, A0_ST7735_Pin)
#define TFT_A0_L() LL_GPIO_ResetOutputPin(A0_ST7735_GPIO_Port, A0_ST7735_Pin)

#define TFT_RST_H() LL_GPIO_SetOutputPin(RST_ST7735_GPIO_Port, RST_ST7735_Pin)
#define TFT_RST_L() LL_GPIO_ResetOutputPin(RST_ST7735_GPIO_Port, RST_ST7735_Pin)


int16_t _width;       ///< Display width as modified by current rotation
int16_t _height;      ///< Display height as modified by current rotation
int16_t cursor_x;     ///< x location to start print()ing text
int16_t cursor_y;     ///< y location to start print()ing text
uint8_t rotation;     ///< Display rotation (0 thru 3)
uint8_t _colstart;   ///< Some displays need this changed to offset
uint8_t _rowstart;       ///< Some displays need this changed to offset
uint8_t _xstart;
uint8_t _ystart;

uint8_t buf_DMA[] = {0};
uint16_t framebuffer[ST7735_WIDTH * ST7735_HEIGHT];

  const uint8_t
  init_cmds1[] = {            // Init for 7735R, part 1 (red or green tab)
    15,                       // 15 commands in list:
    ST7735_SWRESET,   DELAY,  //  1: Software reset, 0 args, w/delay
      150,                    //     150 ms delay
    ST7735_SLPOUT ,   DELAY,  //  2: Out of sleep mode, 0 args, w/delay
      255,                    //     500 ms delay
    ST7735_FRMCTR1, 3      ,  //  3: Frame rate ctrl - normal mode, 3 args:
      0x01, 0x2C, 0x2D,       //     Rate = fosc/(1x2+40) * (LINE+2C+2D)
    ST7735_FRMCTR2, 3      ,  //  4: Frame rate control - idle mode, 3 args:
      0x01, 0x2C, 0x2D,       //     Rate = fosc/(1x2+40) * (LINE+2C+2D)
    ST7735_FRMCTR3, 6      ,  //  5: Frame rate ctrl - partial mode, 6 args:
      0x01, 0x2C, 0x2D,       //     Dot inversion mode
      0x01, 0x2C, 0x2D,       //     Line inversion mode
    ST7735_INVCTR , 1      ,  //  6: Display inversion ctrl, 1 arg, no delay:
      0x07,                   //     No inversion
    ST7735_PWCTR1 , 3      ,  //  7: Power control, 3 args, no delay:
      0xA2,
      0x02,                   //     -4.6V
      0x84,                   //     AUTO mode
    ST7735_PWCTR2 , 1      ,  //  8: Power control, 1 arg, no delay:
      0xC5,                   //     VGH25 = 2.4C VGSEL = -10 VGH = 3 * AVDD
    ST7735_PWCTR3 , 2      ,  //  9: Power control, 2 args, no delay:
      0x0A,                   //     Opamp current small
      0x00,                   //     Boost frequency
    ST7735_PWCTR4 , 2      ,  // 10: Power control, 2 args, no delay:
      0x8A,                   //     BCLK/2, Opamp current small & Medium low
      0x2A,
    ST7735_PWCTR5 , 2      ,  // 11: Power control, 2 args, no delay:
      0x8A, 0xEE,
    ST7735_VMCTR1 , 1      ,  // 12: Power control, 1 arg, no delay:
      0x0E,
    ST7735_INVOFF , 0      ,  // 13: Don't invert display, no args, no delay
    ST7735_COLMOD , 1      ,  // 15: set color mode, 1 arg, no delay:
      0x05 },                 //     16-bit color

#if (defined(ST7735_IS_128X128) || defined(ST7735_IS_160X128))
  init_cmds2[] = {            // Init for 7735R, part 2 (1.44" display)
    2,                        //  2 commands in list:
    ST7735_CASET  , 4      ,  //  1: Column addr set, 4 args, no delay:
      0x00, 0x00,             //     XSTART = 0
      0x00, 0x7F,             //     XEND = 127
    ST7735_RASET  , 4      ,  //  2: Row addr set, 4 args, no delay:
      0x00, 0x00,             //     XSTART = 0
      0x00, 0x7F },           //     XEND = 127
#endif // ST7735_IS_128X128

#ifdef ST7735_IS_160X80
  init_cmds2[] = {            // Init for 7735S, part 2 (160x80 display)
    3,                        //  3 commands in list:
    ST7735_CASET  , 4      ,  //  1: Column addr set, 4 args, no delay:
      0x00, 0x00,             //     XSTART = 0
      0x00, 0x4F,             //     XEND = 79
    ST7735_RASET  , 4      ,  //  2: Row addr set, 4 args, no delay:
      0x00, 0x00,             //     XSTART = 0
      0x00, 0x9F ,            //     XEND = 159
    ST7735_INVON, 0 },        //  3: Invert colors
#endif

  init_cmds3[] = {            // Init for 7735R, part 3 (red or green tab)
    4,                        //  4 commands in list:
    ST7735_GMCTRP1, 16      , //  1: Magical unicorn dust, 16 args, no delay:
      0x02, 0x1c, 0x07, 0x12,
      0x37, 0x32, 0x29, 0x2d,
      0x29, 0x25, 0x2B, 0x39,
      0x00, 0x01, 0x03, 0x10,
    ST7735_GMCTRN1, 16      , //  2: Sparkles and rainbows, 16 args, no delay:
      0x03, 0x1d, 0x07, 0x06,
      0x2E, 0x2C, 0x29, 0x2D,
      0x2E, 0x2E, 0x37, 0x3F,
      0x00, 0x00, 0x02, 0x10,
    ST7735_NORON  ,    DELAY, //  3: Normal display on, no args, w/delay
      10,                     //     10 ms delay
    ST7735_DISPON ,    DELAY, //  4: Main screen turn on, no args w/delay
      100 };                  //     100 ms delay

/*----------------------------------------------------------------------*/
/*----------------------------ST7735 Reset------------------------------*/
/*----------------------------------------------------------------------*/
void ST7735_Reset()
{
    TFT_RST_L();
    LL_mDelay(5);
    TFT_RST_H();
}

/*----------------------------------------------------------------------*/
/*----------------------ST7735 DMA Write Data---------------------------*/
/*----------------------------------------------------------------------*/
/* Write Data with DMA.
 * @param SPIx 	    Selected SPI
 * @param buff_size Buffer size
 */
void ST7735_WriteDataDMA(SPI_TypeDef *SPIx, uint16_t buff_size)
{
	TFT_A0_H();
    LL_DMA_DisableStream(DMAx, Streem);
    LL_DMA_SetDataLength(DMAx, Streem, buff_size);
    LL_DMA_ConfigAddresses(DMAx, Streem,
    		(uint32_t)&buf_DMA,
    		LL_SPI_DMA_GetRegAddr(SPIx),
    		LL_DMA_GetDataTransferDirection(DMAx, Streem));
    LL_DMA_EnableStream(DMAx, Streem);
    while(!dma_spi_f){}
    dma_spi_f = 0;
}

/*----------------------------------------------------------------------*/
/*----------------------ST7735 Write Command----------------------------*/
/*----------------------------------------------------------------------*/
/* Write Command.
 * @param SPIx 	    Selected SPI
 * @param cmd		Hex command
 */
void ST7735_WriteCommand(SPI_TypeDef *SPIx, uint8_t cmd)
{
	TFT_A0_L();
	SPI_Send8Bits(SPIx, cmd);
}
/*----------------------------------------------------------------------*/
/*----------------------ST7735 Write Data-------------------------------*/
/*----------------------------------------------------------------------*/
/* Write Data to register.
 * @param SPIx 	    Selected SPI
 * @param buff		Pointer to buffer
 * @param buff_size Buffer size
 */
void ST7735_WriteData(SPI_TypeDef *SPIx, uint8_t* buff, uint16_t buff_size)
{
    TFT_A0_H();
    for(uint16_t i = 0; i < buff_size; i++)
    {
    	SPI_Send8Bits(SPIx, buff[i]);
    }
}

/*----------------------------------------------------------------------*/
/*----------------------ST7735 Display Init-----------------------------*/
/*----------------------------------------------------------------------*/
/* Display Init.
 * @param SPIx 	    Selected SPI
 * @param addr 		Pointer to init_cmds
 */
void DisplayInit(SPI_TypeDef *SPIx, const uint8_t *addr)
{
    uint8_t numCommands, numArgs;
    uint16_t ms;

    numCommands = *addr++;
    while(numCommands--) {
        uint8_t cmd = *addr++;
        ST7735_WriteCommand(SPIx, cmd);

        numArgs = *addr++;
        // If high bit set, delay follows args
        ms = numArgs & DELAY;
        numArgs &= ~DELAY;
        if(numArgs) {
            ST7735_WriteData(SPIx, (uint8_t*)addr, numArgs);
            addr += numArgs;
        }

        if(ms) {
            ms = *addr++;
            if(ms == 255) ms = 500;
            LL_mDelay(ms);
        }
    }
}
/*----------------------------------------------------------------------*/
/*-------------------ST7735 Set Address Window--------------------------*/
/*----------------------------------------------------------------------*/
/* Size of window.
 * @param SPIx 	    Selected SPI
 * @param x0 		first x
 * @param y0 		first y
 * @param x1 		Second x
 * @param y1 		Second y
 */
void ST7735_SetAddressWindow(SPI_TypeDef *SPIx, uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1)
{
    // column address set
    ST7735_WriteCommand(SPIx, ST7735_CASET);
//    uint8_t data[] = { 0x00, x0 + _xstart, 0x00, x1 + _xstart };
    buf_DMA[0] = 0x00;
    buf_DMA[1] = x0 + _xstart;
    buf_DMA[2] = 0x00;
    buf_DMA[3] =x1 + _xstart;
    ST7735_WriteDataDMA(SPIx, 4);
//    ST7735_WriteData(SPIx, data, 4);

    // row address set
    ST7735_WriteCommand(SPIx, ST7735_RASET);
    buf_DMA[1] = y0 + _ystart;
    buf_DMA[3] = y1 + _ystart;
    ST7735_WriteDataDMA(SPIx, 4);

    // write to RAM
    ST7735_WriteCommand(SPIx, ST7735_RAMWR);
}
/*----------------------------------------------------------------------*/
/*------------------------------ST7735 Init-----------------------------*/
/*----------------------------------------------------------------------*/
/* Init.
 * @param SPIx 	    Selected SPI
 * @param rotation  Side
 */
void ST7735_Init(SPI_TypeDef *SPIx, uint8_t rotation)
{
    TFT_CS_L();
    ST7735_Reset();
    DisplayInit(SPIx, init_cmds1);
    DisplayInit(SPIx, init_cmds2);
    DisplayInit(SPIx, init_cmds3);
#if ST7735_IS_160X80
    _colstart = 24;
    _rowstart = 0;
 /*****  IF Doesn't work, remove the code below (before #elif) *****/
    uint8_t data = 0xC0;
    ST7735_Select();
    ST7735_WriteCommand(ST7735_MADCTL);
    ST7735_WriteData(&data,1);
    ST7735_Unselect();

#elif ST7735_IS_128X128
    _colstart = 2;
    _rowstart = 3;
#else
    _colstart = 0;
    _rowstart = 0;
#endif
    ST7735_SetRotation(SPIx, rotation);
    TFT_CS_H();
}
/*----------------------------------------------------------------------*/
/*----------------------ST7735 Set Rotation-----------------------------*/
/*----------------------------------------------------------------------*/
/* Set Rotation.
 * @param SPIx 	    Selected SPI
 * @param m 		Side
 */
void ST7735_SetRotation(SPI_TypeDef *SPIx, uint8_t m)
{

  uint8_t madctl = 0;

  rotation = m % 4; // can't be higher than 3

  switch (rotation)
  {
  case 0:
#if ST7735_IS_160X80
	  madctl = ST7735_MADCTL_MX | ST7735_MADCTL_MY | ST7735_MADCTL_BGR;
#else
      madctl = ST7735_MADCTL_MX | ST7735_MADCTL_MY | ST7735_MADCTL_RGB;
      _height = ST7735_HEIGHT;
      _width = ST7735_WIDTH;
      _xstart = _colstart;
      _ystart = _rowstart;
#endif
    break;
  case 1:
#if ST7735_IS_160X80
	  madctl = ST7735_MADCTL_MY | ST7735_MADCTL_MV | ST7735_MADCTL_BGR;
#else
      madctl = ST7735_MADCTL_MY | ST7735_MADCTL_MV | ST7735_MADCTL_RGB;
      _width = ST7735_HEIGHT;
      _height = ST7735_WIDTH;
    _ystart = _colstart;
    _xstart = _rowstart;
#endif
    break;
  case 2:
#if ST7735_IS_160X80
	  madctl = ST7735_MADCTL_BGR;
#else
      madctl = ST7735_MADCTL_RGB;
      _height = ST7735_HEIGHT;
      _width = ST7735_WIDTH;
    _xstart = _colstart;
    _ystart = _rowstart;
#endif
    break;
  case 3:
#if ST7735_IS_160X80
	  madctl = ST7735_MADCTL_MX | ST7735_MADCTL_MV | ST7735_MADCTL_BGR;
#else
      madctl = ST7735_MADCTL_MX | ST7735_MADCTL_MV | ST7735_MADCTL_RGB;
      _width = ST7735_HEIGHT;
      _height = ST7735_WIDTH;
    _ystart = _colstart;
    _xstart = _rowstart;
#endif
    break;
  }
  TFT_CS_L();
  ST7735_WriteCommand(SPIx, ST7735_MADCTL);
  ST7735_WriteData(SPIx, &madctl,1);
  TFT_CS_H();
}
/*----------------------------------------------------------------------*/
/*-------------------------ST7735 Draw Pixel----------------------------*/
/*----------------------------------------------------------------------*/
/* Draw One Pixel.
 * @param SPIx 	    Selected SPI
 * @param x  		x axis
 * @param y  		y axis
 * @param color  	color of pixel
 */
void ST7735_DrawPixel(SPI_TypeDef *SPIx, uint16_t x, uint16_t y, uint16_t color) {
    if((x >= _width) || (y >= _height))
        return;

    TFT_CS_L();

    ST7735_SetAddressWindow(SPIx, x, y, x+1, y+1);

    TFT_A0_H();

    buf_DMA[0] = color >> 8;
    buf_DMA[1] = color & 0xFF;
    ST7735_WriteDataDMA(SPIx, 2);

    TFT_CS_H();
}
/*----------------------------------------------------------------------*/
/*-------------------------ST7735 Write Char----------------------------*/
/*----------------------------------------------------------------------*/
/* Write One Symbol.
 * @param SPIx 	    Selected SPI
 * @param x  		x axis
 * @param y  		y axis
 * @param ch  		Symbol
 * @param font  	Symbol font
 * @param color  	Symbol color
 * @param bgcolor  	background color
 */
void ST7735_WriteChar(SPI_TypeDef *SPIx, uint16_t x, uint16_t y, char ch, FontDef font, uint16_t color, uint16_t bgcolor) {
    uint32_t i, b, j;
    ST7735_SetAddressWindow(SPIx, x, y, x+font.width-1, y+font.height-1);

    for(i = 0; i < font.height; i++) {
        b = font.data[(ch - 32) * font.height + i];
        for(j = 0; j < font.width; j++) {
            if((b << j) & 0x8000)  {
                buf_DMA[0] = color >> 8;
                buf_DMA[1] = color & 0xFF;
            } else {
            	buf_DMA[0] = bgcolor >> 8;
            	buf_DMA[1] = bgcolor & 0xFF;
            }
            ST7735_WriteDataDMA(SPIx, 2);
        }
    }
}
/*----------------------------------------------------------------------*/
/*-----------------------ST7735 Write String----------------------------*/
/*----------------------------------------------------------------------*/
/* Write String.
 * @param SPIx 	    Selected SPI
 * @param x  		x axis
 * @param y  		y axis
 * @param str  		pointer to char array
 * @param font  	Symbol font
 * @param color  	Symbol color
 * @param bgcolor  	background color
 */
void ST7735_WriteString(SPI_TypeDef *SPIx, uint16_t x, uint16_t y, const char* str, FontDef font, uint16_t color, uint16_t bgcolor) {
	TFT_CS_L();

    while(*str) {
        if(x + font.width >= _width) {
            x = 0;
            y += font.height;
            if(y + font.height >= _height) {
                break;
            }

            if(*str == ' ') {
                // skip spaces in the beginning of the new line
                str++;
                continue;
            }
        }

        ST7735_WriteChar(SPIx, x, y, *str, font, color, bgcolor);
        x += font.width;
        str++;
    }

    TFT_CS_H();
}
/*----------------------------------------------------------------------*/
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
void ST7735_FillRectangle(SPI_TypeDef *SPIx, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
    if((x >= _width) || (y >= _height)) return;
    if((x + w - 1) >= _width) w = _width - x;
    if((y + h - 1) >= _height) h = _height - y;

    TFT_CS_L();
    ST7735_SetAddressWindow(SPIx, x, y, x+w-1, y+h-1);

    for(uint16_t i = 0; i < w * h; i++)
    {
    	buf_DMA[i * 2] = color >> 8;
    	buf_DMA[i * 2 + 1] = color & 0xFF;
    }
    ST7735_WriteDataDMA(SPIx, w * h * 2);
    TFT_CS_H();
}
/*----------------------------------------------------------------------*/
/*------------------------ST7735 Draw Image-----------------------------*/
/*----------------------------------------------------------------------*/
/* Draw Image.
 * @param SPIx 	    Selected SPI
 * @param x  		x axis
 * @param y  		y axis
 * @param w  		width
 * @param h  		hight
 * @param data  	Array of image bits
 */
void ST7735_DrawImage(SPI_TypeDef *SPIx, uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t* data) {
    if((x >= _width) || (y >= _height)) return;
    if((x + w - 1) >= _width) return;
    if((y + h - 1) >= _height) return;

    TFT_CS_L();
    ST7735_SetAddressWindow(SPIx, x, y, x+w-1, y+h-1);

    for(uint16_t i = 0; i < w * h; i++)
    {
    	buf_DMA[i * 2] = data[i] >> 8;
    	buf_DMA[i * 2 + 1] = data[i] & 0xFF;
    }
    ST7735_WriteDataDMA(SPIx, w * h * 2);
    TFT_CS_H();
}
/*----------------------------------------------------------------------*/
/*------------------------ST7735 Invert Colors--------------------------*/
/*----------------------------------------------------------------------*/
/* Invert Colors.
 * @param SPIx 	    Selected SPI
 * @param invert 	State of invert
 */
void ST7735_InvertColors(SPI_TypeDef *SPIx, uint8_t invert) {
	TFT_CS_L();
    ST7735_WriteCommand(SPIx, invert ? ST7735_INVON : ST7735_INVOFF);
    TFT_CS_H();
}
/*----------------------------------------------------------------------*/
/*------------------------ST7735 Update Screen--------------------------*/
/*----------------------------------------------------------------------*/
/* Update Screen (for future->).
 * @param SPIx 	    Selected SPI
 */
void ST7735_UpdateScreen(SPI_TypeDef *SPIx)
{
	TFT_CS_L();
    ST7735_SetAddressWindow(SPIx, 0, 0, ST7735_WIDTH - 1, ST7735_HEIGHT - 1);
    ST7735_WriteData(SPIx, (uint8_t*)framebuffer, ST7735_WIDTH * ST7735_HEIGHT * 2);
    TFT_CS_H();
}
