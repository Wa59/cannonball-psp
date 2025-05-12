/***************************************************************************
    Open GL Video Rendering.

    Useful References:
    http://www.sdltutorials.com/sdl-opengl-tutorial-basics
    http://www.opengl.org/wiki/Common_Mistakes
    http://open.gl/textures

    Copyright Chris White.
    See license.txt for more details.
***************************************************************************/

#include <iostream>

#include "rendergl.hpp"
#include "frontend/config.hpp"

const static uint32_t SCANLINE_TEXTURE[] = { 0x00000000, 0xff000000 }; // BGRA 8-8-8-8-REV

RenderGL::RenderGL()
{

}

bool RenderGL::init(int src_width, int src_height,
                    int scale,
                    int video_mode,
                    int scanlines)
{
    this->src_width  = src_width;
    this->src_height = src_height;
    this->video_mode = video_mode;
    this->scanlines  = scanlines;

    // Setup SDL Screen size
    if (!RenderBase::sdl_screen_size())
        return false;

    //int flags = SDL_OPENGL;
    int flags = 0;

    // Full Screen Mode
    if (video_mode == video_settings_t::MODE_FULL)
    {
        // Calculate how much to scale screen from its original resolution
        uint32_t w = (scn_width  << 16)  / src_width;
        uint32_t h = (scn_height << 16)  / src_height;
        dst_width  = (src_width  * std::min(w, h)) >> 16;
        dst_height = (src_height * std::min(w, h)) >> 16;
        flags |= SDL_FULLSCREEN; // Set SDL flag
        SDL_ShowCursor(false);   // Don't show mouse cursor in full-screen mode
    }
    // Stretch screen. Lose original proportions
    else if (video_mode == video_settings_t::MODE_STRETCH)
    {
        dst_width  = scn_width;
        dst_height = scn_height;
        flags |= SDL_FULLSCREEN; // Set SDL flag
        SDL_ShowCursor(false);   // Don't show mouse cursor in full-screen mode
    }
    // Window Mode
    else
    {
        scn_width  = dst_width  = src_width  * scale;
        scn_height = dst_height = src_height * scale;
        SDL_ShowCursor(true);
    }


    //int bpp = info->vfmt->BitsPerPixel;
    const int bpp = 32;
    const int available = SDL_VideoModeOK(scn_width, scn_height, bpp, flags);

    // Frees (Deletes) existing surface
    if (surface)
        SDL_FreeSurface(surface);

    // Set the video mode
    surface = SDL_SetVideoMode(scn_width, scn_height, bpp, flags);

    if (!surface || !available)
    {
        std::cerr << "Video mode set failed: " << SDL_GetError() << std::endl;
        return false;
    }

    if (screen_pixels)
        delete[] screen_pixels;
    screen_pixels = new uint32_t[src_width * src_height];

    // SDL Pixel Format Information
    Rshift = surface->format->Rshift;
    Gshift = surface->format->Gshift;
    Bshift = surface->format->Bshift;

    // --------------------------------------------------------------------------------------------
    // Initalize Open GL
    // --------------------------------------------------------------------------------------------

    // Disable dithering
    //glDisable(GL_DITHER);
    // Disable anti-aliasing
    //glDisable(GL_LINE_SMOOTH);
    //glDisable(GL_POINT_SMOOTH);
    // Disable depth buffer
    //glDisable(GL_DEPTH_TEST);

    // V-Sync
    SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 1);

    //glClearColor(0, 0, 0, 0); // Black background
    //glShadeModel(GL_FLAT);

    glViewport(0, 0, 512, 512);
    /*

    // Initalize Texture ID
    glGenTextures(1, textures);
    // Screen Texture Setup
    const GLint param = config.video.filtering ? GL_LINEAR : GL_NEAREST;
    glBindTexture(GL_TEXTURE_2D, textures[SCREEN]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, param);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, param);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,
                512, 512, 0,                // texture width, texture height
                GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV,    // Data format in pixel array
                NULL);


    // Initalize D-List
    dlist = glGenLists(1);
    glNewList(dlist, GL_COMPILE);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, scn_width, scn_height, 0, 0, 1);         // left, right, bottom, top, near, far
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear screen and depth buffer
    glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, textures[SCREEN]);     // Select Screen Texture
        glBegin(GL_QUADS);
            glTexCoord2f(0, 1);
            glVertex2f  (0,             0 + dst_height);  // lower left
            glTexCoord2f(0, 0);
            glVertex2f  (0,             0);               // upper left
            glTexCoord2f(1, 0);
            glVertex2f  (0 + dst_width, 0);               // upper right
            glTexCoord2f(1, 1);
            glVertex2f  (0 + dst_width, 0 + dst_height);  // lower right
        glEnd();

    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
    glEndList();
*/
    return true;
}

void RenderGL::disable()
{
    //glDeleteLists(dlist, 1);
    //glDeleteTextures(scanlines ? 2 : 1, textures);
}

bool RenderGL::start_frame()
{
    return !(SDL_MUSTLOCK(surface) && SDL_LockSurface(surface) < 0);
}

bool RenderGL::finalize_frame()
{
    if (SDL_MUSTLOCK(surface))
        SDL_UnlockSurface(surface);

    return true;
}

void RenderGL::draw_frame(uint16_t* pixels)
{
    uint32_t* spix = screen_pixels;

    // Lookup real RGB value from rgb array for backbuffer
    //for (int i = 0; i < (src_width * src_height); i++)
    //    *(spix++) = rgb[*(pixels++) & ((S16_PALETTE_ENTRIES * 3) - 1)];

/*
    glBindTexture(GL_TEXTURE_2D, textures[SCREEN]);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,            // target, LOD, xoff, yoff
            512, 512,                     // texture width, texture height
            GL_BGRA,                                   // format of pixel data
            GL_UNSIGNED_INT_8_8_8_8_REV,               // data type of pixel data
            screen_pixels);                            // pointer in image memory
*/
    //glCallList(dlist);
    //glFinish();
    //SDL_GL_SwapBuffers();
}