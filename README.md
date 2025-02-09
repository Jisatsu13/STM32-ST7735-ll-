# STM32-ST7735-ll-
Library for STM32 with ST7735 using Low Level Library.

Як користуватися?

1. Обрати SPI, Задефайнити ніжки RST_ST7735, CS_ST7735, LED_ST7735, A0_ST7735
![st2](https://github.com/user-attachments/assets/9350fd7a-7247-4d38-bd40-258e4d1a5d2e)

![53182727595_d2a7a6dcb9_o копія](https://github.com/user-attachments/assets/572f5c29-aebc-477c-800b-be6887522a90)

2. Змінити мікроконтроллер у бібліотеці spi_help_fun.h, якщо у вас він інший
#include "stm32f4xx_ll_spi.h"
#include "stm32f4xx_ll_utils.h"

3. Змінити ці дефайни, якщо маєте інші налаштування
//#define ST7735_IS_160X80 1
//#define ST7735_IS_128X128 1
#define ST7735_IS_160X128 1
#define ST7735_WIDTH  128
#define ST7735_HEIGHT 160

#define DMAx DMA1
#define Streem LL_DMA_STREAM_4


4. Додати у main.c або у main.h 
#include "GFX_FUNCTIONS.h"
#include "fonts.h"
#include "ST7735.h"

5. Зробіть тако от: init DMA, init ST7735, використайте функції з бібліотек

   LL_DMA_DisableStream(DMAx, Streem);
  LL_DMA_ClearFlag_TC4(DMAx);
  LL_DMA_ClearFlag_TE4(DMAx);
  LL_SPI_EnableDMAReq_TX(SPI2);
  LL_DMA_EnableIT_TC(DMAx, Streem);
  LL_DMA_EnableIT_TE(DMAx, Streem);
  LL_SPI_Enable(SPI2);

  LL_GPIO_SetOutputPin(LED_ST7735_GPIO_Port, LED_ST7735_Pin);
  ST7735_Init(SPI2, 3); //here i using SPI2 and rotate display
  
//----------------------------------------------------------------------------------------------------------------

How to use?

1. Choice SPI, define pins RST_ST7735, CS_ST7735, LED_ST7735, A0_ST7735
![st2](https://github.com/user-attachments/assets/9350fd7a-7247-4d38-bd40-258e4d1a5d2e)

![53182727595_d2a7a6dcb9_o копія](https://github.com/user-attachments/assets/572f5c29-aebc-477c-800b-be6887522a90)


2. Change microcontroller in library spi_help_fun.h if you using another microcontroller
#include "stm32f4xx_ll_spi.h"
#include "stm32f4xx_ll_utils.h"

3. Change this defines if you have another seting 
//#define ST7735_IS_160X80 1
//#define ST7735_IS_128X128 1
#define ST7735_IS_160X128 1
#define ST7735_WIDTH  128
#define ST7735_HEIGHT 160

#define DELAY 0x80

#define DMAx DMA1
#define Streem LL_DMA_STREAM_4


4. Add header to main.c or main.h 
#include "GFX_FUNCTIONS.h"
#include "fonts.h"
#include "ST7735.h"

5. Do this: init DMA, init ST7735, using functions

   LL_DMA_DisableStream(DMAx, Streem);
  LL_DMA_ClearFlag_TC4(DMAx);
  LL_DMA_ClearFlag_TE4(DMAx);
  LL_SPI_EnableDMAReq_TX(SPI2);
  LL_DMA_EnableIT_TC(DMAx, Streem);
  LL_DMA_EnableIT_TE(DMAx, Streem);
  LL_SPI_Enable(SPI2);

  LL_GPIO_SetOutputPin(LED_ST7735_GPIO_Port, LED_ST7735_Pin);
  ST7735_Init(SPI2, 3); //here i using SPI2 and rotate display

