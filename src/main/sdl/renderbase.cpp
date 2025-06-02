#include "renderbase.hpp"
#include <iostream>

RenderBase::RenderBase()
{
    surface       = NULL;
    screen_pixels = NULL;

    orig_width  = 398;
    orig_height = 224;
}

// Setup screen size
bool RenderBase::sdl_screen_size()
{
    orig_width  = 398; 
    orig_height = 224;

    scn_width  = 398;
    scn_height = 224;

    return true;
}

// See: SDL_PixelFormat
#define CURRENT_RGB() (r << Rshift) | (g << Gshift) | (b << Bshift);

void RenderBase::convert_palette(uint32_t adr, uint32_t r, uint32_t g, uint32_t b)
{
    adr >>= 1;

    r = r * 255 / 31;
    g = g * 255 / 31;
    b = b * 255 / 31;

    rgb[adr] = CURRENT_RGB();
      
    // Create shadow / highlight colours at end of RGB array
    // The resultant values are the same as MAME
    r = r * 202 / 256;
    g = g * 202 / 256;
    b = b * 202 / 256;
        
    rgb[adr + S16_PALETTE_ENTRIES] =
    rgb[adr + (S16_PALETTE_ENTRIES * 2)] = CURRENT_RGB();
}