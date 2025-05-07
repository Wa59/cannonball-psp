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

    int flags = SDL_FLAGS;

    //this->video_mode = video_settings_t::MODE_WINDOW;
   
    scale_factor  = scale;

    scn_width  = src_width  * scale_factor;
    scn_height = src_height * scale_factor;

    // As we're windowed this is just the same
    dst_width  = 480;
    dst_height = 272;
    
    screen_xoff = 0;
    screen_yoff = 0;

    //int bpp = info->vfmt->BitsPerPixel;
    const int bpp = 32;
    const int available = SDL_VideoModeOK(scn_width, scn_height, bpp, flags);

    // Frees (Deletes) existing surface
    if (surface)
        SDL_FreeSurface(surface);

    // Set the video mode
    surface = SDL_SetVideoMode(scn_width, scn_height, bpp, SDL_HWPALETTE);


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
    pix = new uint32_t[src_width * src_height];

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
    // Do Scaling
    if (scale_factor != 1)
    {
        uint32_t* pixx = pix;

        // Lookup real RGB value from rgb array for backbuffer
        for (int i = 0; i < (src_width * src_height); i++)
            *(pixx++) = rgb[*(pixels++) & ((S16_PALETTE_ENTRIES * 3) - 1)];

        scalex(pix, src_width, src_height, screen_pixels, 2); 
       
    }
    // No Scaling
    else
    {
        uint32_t* spix = screen_pixels;
    
        // Lookup real RGB value from rgb array for backbuffer
        for (int i = 0; i < (src_width * src_height); i++)
            *(spix++) = rgb[*(pixels++) & ((S16_PALETTE_ENTRIES * 3) - 1)];
    }

    // Example: Set the pixel at 10,10 to red
    //pixels[( 10 * surface->w ) + 10] = 0xFF0000;
    // ------------------------------------------------------------------------      
}

// Fastest scaling algorithm. Scales proportionally.
void RenderSW::scalex(uint32_t* src, const int srcwid, const int srchgt, uint32_t* dest, const int scale)
{
    const int destwid = srcwid * scale;

    for (int y = 0; y < srchgt; y++)
    {
        int src_inc = 0;
    
        // First Row
        for (int x = 0; x < destwid; x++)
        {
            *dest++ = *src;
            if (++src_inc == scale)
            {
                src_inc = 0;
                src++;
            }
        }
        // Make additional copies of this row
        for (int i = 0; i < scale-1; i++)
        {
            memcpy(dest, dest - destwid, destwid * sizeof(uint32_t)); 
            dest += destwid;
        }
    }
}
