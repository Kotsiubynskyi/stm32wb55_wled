#include "ws2812b.h"

#define CLEAR_RG_MASK ~(0x00ff0000 | 0x0000ff00)

static uint32_t pixels_data[PIXELS_AMOUNT];
static uint8_t pixels_pwm_data[2 * COLOR_BITS];
static void (*waitMs)(uint32_t ms);

uint32_t *ws2812b_init(void (*wait)(uint32_t ms))
{
    waitMs = wait;
    for (uint16_t i = 0; i < 2 * COLOR_BITS; i++)
    {
        pixels_pwm_data[i] = 1;
    }

    return (uint32_t *)pixels_pwm_data;
}

static uint32_t convertRgbToGrb(uint32_t rgb)
{
    uint8_t green = (rgb >> 8) & 0xFF;
    uint8_t red = (rgb >> 16) & 0xFF;

    return (rgb & CLEAR_RG_MASK) | (red << 8) | (green << 16);
}

void ws2812b_set_pixel(uint16_t index, uint32_t rgbColor)
{
    if (index >= PIXELS_AMOUNT)
    {
        return;
    }

    uint32_t grb = convertRgbToGrb(rgbColor);
    pixels_data[index] = grb;
}

void ws2812b_allOff()
{
    for (uint16_t i = 0; i < PIXELS_AMOUNT; i++)
    {
        ws2812b_clear_pixel(i);
    }
}

void ws2812b_clear_pixel(uint16_t index)
{
    ws2812b_set_pixel(index, 0);
}

void ws2812b_allOn(uint32_t rgbColor)
{
    for (uint16_t i = 0; i < PIXELS_AMOUNT; i++)
    {
        ws2812b_set_pixel(i, rgbColor);
    }
}

const uint32_t mask = 0xfffffe;
static uint32_t counter = 0;
void ws2812b_dma_complete_callback()
{
    if (counter++ >= PIXELS_AMOUNT)
    {
        for (uint16_t bit = 0; bit < COLOR_BITS; bit++)
        {
            pixels_pwm_data[COLOR_BITS + bit] = 0;
        }
        if (counter > (RESET_LEDS + PIXELS_AMOUNT))
        {
            counter = 0;
        }
    }
    else
    {
        uint32_t grb = pixels_data[(counter - 1)];
        for (uint16_t bit = 0; bit < COLOR_BITS; bit++)
        {
            uint8_t bitOn = (grb >> (COLOR_BITS - 1 - bit) & ~(mask));
            uint32_t ind = COLOR_BITS + bit;
            pixels_pwm_data[ind] = (bitOn == 1) ? 3 : 1;
        }
    }
}

void ws2812b_dma_half_callback()
{
    if (counter++ >= PIXELS_AMOUNT)
    {
        for (uint16_t bit = 0; bit < COLOR_BITS; bit++)
        {
            pixels_pwm_data[bit] = 0;
        }
    }
    else
    {
        uint32_t grb = pixels_data[(counter - 1)];
        for (uint16_t bit = 0; bit < COLOR_BITS; bit++)
        {
            uint8_t bitOn = (grb >> (COLOR_BITS - 1 - bit) & ~(mask));
            pixels_pwm_data[bit] = (bitOn == 1) ? 3 : 1;
        }
    }
}

void delay(uint32_t ms)
{
    waitMs(ms);
}