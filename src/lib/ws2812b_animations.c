#include "ws2812b.h"
#include "ws2812b_animations.h"

static volatile uint8_t animation_running = 0;

static uint32_t wheel(uint8_t pos)
{
    uint8_t red, green, blue;
    pos = 255 - pos;

    if (pos < 85)
    {
        red = 255 - pos * 3;
        green = 0;
        blue = pos * 3;
    }
    else if (pos < 170)
    {
        pos -= 85;
        red = 0;
        green = pos * 3;
        blue = 255 - pos * 3;
    }
    else
    {
        pos -= 170;
        red = pos * 3;
        green = 255 - pos * 3;
        blue = 0;
    }

    return (uint32_t)red << 16 | (uint32_t)green << 8 | blue;
}

void stop_animation(void)
{
    animation_running = 0;
}

void rainbow(void)
{
    animation_running = 1;
    uint8_t position = 0;
    while (animation_running)
    {
        for (uint32_t pixel = 0; pixel < PIXELS_AMOUNT; pixel++)
        {
            uint8_t color_index = ((pixel * 256 / PIXELS_AMOUNT) + position) & 255;
            ws2812b_set_pixel(pixel, wheel(color_index));
        }
        delay(15);
        position = (position + 1) & 255;
    }
}

static uint32_t fadeOutCometSegmentColor(uint32_t color, uint8_t segment, uint8_t length)
{
    if (segment >= length - 1)
    {
        return color;
    }

    uint8_t red = (color >> 16) & 0xFF;
    uint8_t green = (color >> 8) & 0xFF;
    uint8_t blue = color & 0xFF;

    red = (red * (segment + 1)) / length;
    green = (green * (segment + 1)) / length;
    blue = (blue * (segment + 1)) / length;

    return ((uint32_t)red << 16) | ((uint32_t)green << 8) | blue;
}

void comet(uint8_t length, uint32_t color, uint8_t speed)
{
    if (length > PIXELS_AMOUNT)
    {
        length = PIXELS_AMOUNT;
    }

    if (speed > 10)
    {
        speed = 10;
    }
    uint8_t delayMs = 10 + (100 - 10 * speed);

    animation_running = 1;
    while (animation_running)
    {
        for (uint32_t pixel = 0; pixel < PIXELS_AMOUNT; pixel++)
        {
            for (uint8_t segment = 0; segment < length; segment++)
            {
                uint32_t segment_color = fadeOutCometSegmentColor(color, segment, length);
                ws2812b_set_pixel(pixel + segment, segment_color);
            }

            if (pixel == 0)
            {
                ws2812b_clear_pixel(PIXELS_AMOUNT - 1);
            }
            ws2812b_clear_pixel(pixel - 1);

            delay(delayMs);
        }
    }
}

void glowing(uint32_t rgbColor, uint8_t speed)
{
    animation_running = 1;
    uint8_t brightness = 0;
    int8_t direction = 1;

    if (speed > 10)
    {
        speed = 10;
    }
    uint8_t delayMs = 1 + (20 - speed * 2);

    while (animation_running)
    {
        uint8_t red = ((rgbColor >> 16) & 0xff) * brightness / 255;
        uint8_t green = ((rgbColor >> 8) & 0xff) * brightness / 255;
        uint8_t blue = (rgbColor & 0xff) * brightness / 255;
        uint32_t adjustedColor = ((uint32_t)red << 16) | ((uint32_t)green << 8) | blue;

        for (uint32_t i = 0; i < PIXELS_AMOUNT; i++)
        {
            ws2812b_set_pixel(i, adjustedColor);
        }

        brightness += direction * 2;
        if (brightness >= 255 || brightness <= 0)
        {
            direction *= -1;
            brightness = ~brightness;
        }

        delay(delayMs);
    }
}
