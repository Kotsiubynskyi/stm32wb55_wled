#ifndef WS2812B_ANIMATIONS_H
#define WS2812B_ANIMATIONS_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "stdint.h"

    void stop_animation();

    /**
     * Rainbow
     */
    void rainbow();

    /**
     * Single color glowing - fade in and out
     * @param rgbColor RGB color of glowing
     * @param speed Movement speed of the beam
     */
    void glowing(uint32_t rgbColor, uint8_t speed);

    /**
     * Creates a moving beam effect
     * @param length Number of LEDs in the beam
     * @param rgbColor RGB color of the beam
     * @param speed Movement speed of the beam
     */
    void comet(uint8_t length, uint32_t rgbColor, uint8_t speed);

#ifdef __cplusplus
}
#endif

#endif // WS2812B_ANIMATIONS_H