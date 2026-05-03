#ifndef WS2812B_H
#define WS2812B_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#ifndef PIXELS_AMOUNT
#define PIXELS_AMOUNT 128
#endif // PIXELS_AMOUNT

#ifndef RESET_LEDS
#define RESET_LEDS 12
#endif // RESET_LEDS

#ifndef COLOR_BITS
#define COLOR_BITS 24
#endif // COLOR_BITS

uint32_t* ws2812b_init();
void ws2812b_clear_pixel(uint16_t index);
void ws2812b_set_pixel(uint16_t index, uint32_t rgbColor);
void ws2812b_allOff(void);
void ws2812b_allOn(uint32_t rgbColor);
void ws2812b_dma_complete_callback(void);
void ws2812b_dma_half_callback(void);
void delay(uint32_t ms);

#ifdef __cplusplus
}
#endif

#endif // WS2812B_H