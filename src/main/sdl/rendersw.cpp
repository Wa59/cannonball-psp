/***************************************************************************
    SDL Software Video Rendering.  
    
    Known Bugs:
    - Scanlines don't work when Endian changed?

    Copyright Chris White.
    See license.txt for more details.
***************************************************************************/

#include <iostream>

#include "rendersw.hpp"
#include "frontend/config.hpp"

RenderSW::RenderSW()
{
    scan_pixels = NULL;
    pix         = NULL;
}

RenderSW::~RenderSW()
{
    if (scan_pixels) 
        delete[] scan_pixels;

    if (pix)
        delete[] pix;
}

bool RenderSW::init(int src_width, int src_height, 
                    int scale,
                    int video_mode,
                    int scanlines)
{
    this->src_width  = src_width;
    this->src_height = src_height;
    this->video_mode = video_mode;

    // Setup SDL Screen size
    if (!RenderBase::sdl_screen_size())
        return false;

    SDL_ShowCursor(0);

    //this->video_mode = video_settings_t::MODE_WINDOW;
   
    scn_width  = src_width = 320;
    scn_height = src_height = 224;

    // As we're windowed this is just the same
    //int bpp = info->vfmt->BitsPerPixel;
    const int bpp = 32;

    // Frees (Deletes) existing surface
    if (surface)
        SDL_FreeSurface(surface);

    // Set the video mode
    surface = SDL_SetVideoMode(404, 224, bpp, SDL_SWSURFACE | SDL_HWPALETTE | SDL_FULLSCREEN);


    // Convert the SDL pixel surface to 32 bit.
    // This is potentially a larger surface area than the internal pixel array.
    screen_pixels = (uint32_t*)surface->pixels;
    
    // SDL Pixel Format Information
    Rshift = surface->format->Rshift;
    Gshift = surface->format->Gshift;
    Bshift = surface->format->Bshift;
    Rmask  = surface->format->Rmask;
    Gmask  = surface->format->Gmask;
    Bmask  = surface->format->Bmask;

    if (pix)
        delete[] pix;
    pix = new uint32_t[320 * 224];

    return true;
}

void RenderSW::disable()
{

}

bool RenderSW::start_frame()
{
    return !(SDL_MUSTLOCK(surface) && SDL_LockSurface(surface) < 0);
}

bool RenderSW::finalize_frame()
{
    if (SDL_MUSTLOCK(surface))
        SDL_UnlockSurface(surface);

    SDL_UpdateRect(surface, 0, 0, 0, 0);

    return true;
}

void RenderSW::draw_frame(uint16_t* pixels)
{
    uint32_t* spix = screen_pixels;

    // Lookup real RGB value from rgb array for backbuffer
    for (int i = 1; i <= (380 * 224); i++) {
        if (i==1) {
            for (int j = 0; j < (40); j++) {
                *(spix++) = 0x000000;
            }
        }
        if (i % 321 == 0) {
            for (int j = 0; j < (192); j++) {
                *(spix++) = 0x000000;
            }
        } else {
            *(spix++) = rgb[*(pixels++) & ((S16_PALETTE_ENTRIES * 3) - 1)];
        }
    }

    // Example: Set the pixel at 10,10 to red
    //pixels[( 10 * surface->w ) + 10] = 0xFF0000;
    // ------------------------------------------------------------------------      
}
