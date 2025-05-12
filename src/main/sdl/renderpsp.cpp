#include <iostream>

#include "renderpsp.hpp"
#include "frontend/config.hpp"

#include <pspdisplay.h>
#include <pspgu.h>
#include <pspgum.h>

static unsigned int __attribute__((aligned(16))) list[262144];

// Get Memory Size
static unsigned int getMemorySize(unsigned int width, unsigned int height, unsigned int psm)
{
	switch (psm)
	{
		case GU_PSM_T4:
			return (width * height) >> 1;

		case GU_PSM_T8:
			return width * height;

		case GU_PSM_5650:
		case GU_PSM_5551:
		case GU_PSM_4444:
		case GU_PSM_T16:
			return 2 * width * height;

		case GU_PSM_8888:
		case GU_PSM_T32:
			return 4 * width * height;

		default:
			return 0;
	}
}

// Vram Buffer Request
void* getStaticVramBuffer(unsigned int width, unsigned int height, unsigned int psm)
{
	static unsigned int staticOffset = 0;
	unsigned int memSize = getMemorySize(width,height,psm);
	void* result = (void*)staticOffset;
	staticOffset += memSize;

	return result;
}

RenderPSP::RenderPSP()
{

}

bool RenderPSP::init(int src_width, int src_height,
                    int scale,
                    int video_mode,
                    int scanlines)
{
    this->src_width  = src_width;
    this->src_height = src_height;
    this->video_mode = video_mode;
    this->scanlines  = scanlines;

    scn_width  = dst_width  = src_width;
    scn_height = dst_height = src_height;

    void* fbp0 = getStaticVramBuffer(512,480,GU_PSM_8888);
	void* fbp1 = getStaticVramBuffer(512,480,GU_PSM_8888);
	void* zbp = getStaticVramBuffer(512,480,GU_PSM_4444);

	sceGuInit();

	sceGuStart(GU_DIRECT,list);
	sceGuDrawBuffer(GU_PSM_8888,fbp0,512);
	sceGuDispBuffer(src_width,src_height,fbp1,512);
	sceGuDepthBuffer(zbp,512);


	sceGuOffset(2048 - (src_width/2),2048 - (src_height/2));
	sceGuViewport(2048,2048,src_width,src_height);
	sceGuDepthRange(65535,0);
	sceGuScissor(0,0,src_width,src_height);
	sceGuEnable(GU_SCISSOR_TEST);
	sceGuDepthFunc(GU_GEQUAL);
	sceGuEnable(GU_DEPTH_TEST);
	sceGuFrontFace(GU_CW);
	sceGuShadeModel(GU_SMOOTH);
	sceGuEnable(GU_CULL_FACE);
	sceGuEnable(GU_TEXTURE_2D);
	sceGuEnable(GU_CLIP_PLANES);
	sceGuFinish();
	sceGuSync(0,0);

	sceDisplayWaitVblankStart();
	sceGuDisplay(GU_TRUE);


    return true;
}

void RenderPSP::disable()
{
	sceGuTerm();
}

bool RenderPSP::start_frame()
{
  	sceGuStart(GU_DIRECT, list);	
    return true;
}

bool RenderPSP::finalize_frame()
{
	// End the scene
	sceGuFinish();
	sceGuSync(0,0);
	sceDisplayWaitVblankStart();
	sceGuSwapBuffers();
	return true;   
}

void RenderPSP::draw_frame(uint16_t* pixels)
{
	uint32_t* pixx = screen_pixels;

    // Lookup real RGB value from rgb array for backbuffer
    for (int i = 0; i < (src_width * src_height); i++)
    	//*(pixx++) = rgb[*(pixels++) & ((S16_PALETTE_ENTRIES * 3) - 1)];


    //sceGuScissor(0, 0, 420, 272);
    //sceGuEnable(GU_SCISSOR_TEST);

    //sceGuStart(GU_SEND,list);
	//sceGuEnable(GU_TEXTURE_2D);
	//sceGuTexMode(GU_PSM_8888,0,0,0);
	//sceGuTexFunc(GU_TFX_REPLACE, GU_TCC_RGB);
	//sceGuTexFilter(GU_LINEAR, GU_LINEAR);
	//sceGuTexImage(0, 512, 512, GU_PSM_8888, screen_pixels);
	

	//sceGuTexSync();

        sceGuClearColor(rand());
        sceGuClear(GU_COLOR_BUFFER_BIT);


/*
    glBindTexture(GL_TEXTURE_2D, textures[SCREEN]);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,            // target, LOD, xoff, yoff
            src_width, src_height,                     // texture width, texture height
            GL_BGRA,                                   // format of pixel data
            GL_UNSIGNED_INT_8_8_8_8_REV,               // data type of pixel data
            screen_pixels);                            // pointer in image memory

    glCallList(dlist);





	
	sceGuStart(GU_SEND,list);
	sceGuEnable(GU_TEXTURE_2D);
	sceGuTexMode(tpsm,0,0,0);
	sceGuTexFunc(GU_TFX_REPLACE, GU_TCC_RGB);
	sceGuTexFilter(GU_LINEAR, GU_LINEAR);
	sceGuTexImage(0, width, height, tbw, pixels);
	sceGuTexSync();
	sceGuFinish();

	sceGuStart(GU_DIRECT,pallist);
	sceGuClutLoad(render_int.src.bpp*4, render_int.pal);
	sceGuFinish();

	sceGuStart(GU_SEND,list);
	sceGuEnable(GU_TEXTURE_2D);
	sceGuTexMode(tpsm,0,0,0);
	sceGuTexFunc(GU_TFX_REPLACE, GU_TCC_RGB);
	sceGuTexFilter(GU_LINEAR, GU_LINEAR);
	sceGuTexImage(0, width, height, tbw, pixels);
	sceGuTexSync();

    uint32_t* spix = screen_pixels;

    // Lookup real RGB value from rgb array for backbuffer
    for (int i = 0; i < (src_width * src_height); i++)
        *(spix++) = rgb[*(pixels++) & ((S16_PALETTE_ENTRIES * 3) - 1)];

    glBindTexture(GL_TEXTURE_2D, textures[SCREEN]);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,            // target, LOD, xoff, yoff
            src_width, src_height,                     // texture width, texture height
            GL_BGRA,                                   // format of pixel data
            GL_UNSIGNED_INT_8_8_8_8_REV,               // data type of pixel data
            screen_pixels);                            // pointer in image memory

    glCallList(dlist);
    //glFinish();
    SDL_GL_SwapBuffers();
    */
}