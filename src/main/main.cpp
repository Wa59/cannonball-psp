/***************************************************************************
    Cannonball Main Entry Point.
    
    Copyright Chris White.
    See license.txt for more details.
***************************************************************************/

// Error reporting
#include <iostream>
#if !defined(PSP)
#include <switch.h>
#endif
#if defined(PSP)

#include <pspkernel.h>
#include <pspdisplay.h>
#include <pspdebug.h>
#include <pspctrl.h>

#include <pspgu.h>
#include <pspgum.h>
#include <pspge.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>

PSP_MODULE_INFO("cannonball-psp", 0, 1, 1);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);

#include "callback.h"

#define printf pspDebugScreenPrintf
#endif

// SDL Library
#include <SDL/SDL.h>
#ifndef SDL2
#pragma comment(lib, "SDLmain.lib") // Replace main with SDL_main
#endif
#pragma comment(lib, "SDL.lib")
#pragma comment(lib, "glu32.lib")

// SDL Specific Code
#if defined SDL2
#include "sdl2/timer.hpp"
#include "sdl2/input.hpp"
#else
#include "sdl/timer.hpp"
#include "sdl/input.hpp"
#endif

#include "video.hpp"

#include "romloader.hpp"
#include "trackloader.hpp"
#include "stdint.hpp"
#include "main.hpp"
#include "setup.hpp"
#include "engine/outrun.hpp"
#include "frontend/config.hpp"
#include "frontend/menu.hpp"

#include "cannonboard/interface.hpp"
#include "engine/oinputs.hpp"
#include "engine/ooutputs.hpp"
#include "engine/omusic.hpp"

// Direct X Haptic Support.
// Fine to include on non-windows builds as dummy functions used.
#include "directx/ffeedback.hpp"

// Initialize Shared Variables
using namespace cannonball;

int    cannonball::state       = STATE_BOOT;
double cannonball::frame_ms    = 0;
int    cannonball::frame       = 0;
bool   cannonball::tick_frame  = true;
int    cannonball::fps_counter = 0;

#ifdef COMPILE_SOUND_CODE
Audio cannonball::audio;
#endif

Menu* menu;
Interface cannonboard;

static void quit_func(int code)
{
#ifdef COMPILE_SOUND_CODE
    audio.stop_audio();
#endif
    input.close();
    forcefeedback::close();
    delete menu;
    SDL_Quit();
    //exit(code);
    sceKernelExitGame();	
}

static void process_events(void)
{
    #ifndef PSP
    SDL_Event event;

    // Grab all events from the queue.
    while(SDL_PollEvent(&event))
    {
        switch(event.type)
        {
            case SDL_KEYDOWN:
                // Handle key presses.
                if (event.key.keysym.sym == SDLK_ESCAPE)
                    state = STATE_QUIT;
                else
                    input.handle_key_down(&event.key.keysym);
                break;

            case SDL_KEYUP:
                input.handle_key_up(&event.key.keysym);
                break;

            case SDL_JOYAXISMOTION:
                input.handle_joy_axis(&event.jaxis);
                break;

            case SDL_JOYBUTTONDOWN:
                input.handle_joy_down(&event.jbutton);
                break;

            case SDL_JOYBUTTONUP:
                input.handle_joy_up(&event.jbutton);
                break;

            case SDL_QUIT:
                // Handle quit requests (like Ctrl-c).
                state = STATE_QUIT;
                break;
        }
    }
    #endif
    
    SceCtrlData pad;

	sceCtrlSetSamplingCycle(0);
	sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);
    		
    sceCtrlReadBufferPositive(&pad, 1); 

	if (pad.Buttons & PSP_CTRL_TRIANGLE) {
        input.psp_handle_key(11);
	} else {
        input.psp_handle_release(11);
    } 

    if (pad.Buttons & PSP_CTRL_CROSS) {
        input.psp_handle_key(6);
    } else {
        input.psp_handle_release(6);
    }

    if (pad.Buttons & PSP_CTRL_SQUARE) {
        input.psp_handle_key(0);
    } else {
        input.psp_handle_release(0);
    }

	if (pad.Buttons & PSP_CTRL_UP) {
        input.psp_handle_key(2);
	} else {
        input.psp_handle_release(2);
    }

	if (pad.Buttons & PSP_CTRL_DOWN) {
        input.psp_handle_key(1);
	} else {
        input.psp_handle_release(1);
    }
     
	if (pad.Buttons & PSP_CTRL_LEFT) {
        input.psp_handle_key(4);
	} else {
        input.psp_handle_release(4);
    }
     
	if (pad.Buttons & PSP_CTRL_RIGHT) {
        input.psp_handle_key(3);
	} else {
        input.psp_handle_release(3);
    }
          
	if (pad.Buttons & PSP_CTRL_START){
        input.psp_handle_key(5);
	} else {
        input.psp_handle_release(5);
    }
    
	if (pad.Buttons & PSP_CTRL_SELECT){
        input.psp_handle_key(8);
	} else {
        input.psp_handle_release(8);
    }
    
	if (pad.Buttons & PSP_CTRL_RTRIGGER){
        input.psp_handle_key(9);
	} else {
        input.psp_handle_release(9);
    }
  
}


// Pause Engine
bool pause_engine;

static void tick()
{
    frame++;

    // Get CannonBoard Packet Data
    Packet* packet = config.cannonboard.enabled ? cannonboard.get_packet() : NULL;

    process_events();

    if (tick_frame)
        oinputs.tick(packet); // Do Controls
    oinputs.do_gear();        // Digital Gear

    switch (state)
    {
        case STATE_GAME:
        {
            if (input.has_pressed(Input::TIMER))
                outrun.freeze_timer = !outrun.freeze_timer;

            if (input.has_pressed(Input::PAUSE))
                pause_engine = !pause_engine;

            if (input.has_pressed(Input::MENU))
                state = STATE_INIT_MENU;

            if (!pause_engine || input.has_pressed(Input::STEP))
            {
                outrun.tick(packet, tick_frame);
                input.frame_done(); // Denote keys read

                #ifdef COMPILE_SOUND_CODE
                // Tick audio program code
                osoundint.tick();
                // Tick SDL Audio
                audio.tick();
                #endif
            }
            else
            {                
                input.frame_done(); // Denote keys read
            }
        }
        break;

        case STATE_INIT_GAME:
            if (config.engine.jap && !roms.load_japanese_roms())
            {
                state = STATE_QUIT;
            }
            else
            {
                pause_engine = false;
                outrun.init();
                state = STATE_GAME;
            }
            break;

        case STATE_MENU:
        {
            menu->tick(packet);
            input.frame_done();
            #ifdef COMPILE_SOUND_CODE
            // Tick audio program code
            osoundint.tick();
            // Tick SDL Audio
            audio.tick();
            #endif
        }
        break;

        case STATE_INIT_MENU:
            oinputs.init();
            outrun.outputs->init();
            menu->init();
            state = STATE_MENU;
            break;
    }
    // Write CannonBoard Outputs
    if (config.cannonboard.enabled)
        cannonboard.write(outrun.outputs->dig_out, outrun.outputs->hw_motor_control);

    // Draw SDL Video
    video.draw_frame();  
}

static void main_loop()
{
    // FPS Counter (If Enabled)
    Timer fps_count;
    int frame = 0;
    fps_count.start();

    // General Frame Timing
    Timer frame_time;
    int t;
    double deltatime  = 0;
    int deltaintegral = 0;

    while (isRunning())//state != STATE_QUIT)
    {
        if(state == STATE_QUIT)
            quit_func(0);
        frame_time.start();
        tick();
        #ifdef COMPILE_SOUND_CODE
        deltatime += (frame_ms * audio.adjust_speed());
        #else
        deltatime += frame_ms;
        #endif
        deltaintegral  = (int) deltatime;
        t = frame_time.get_ticks();

        // Cap Frame Rate: Sleep Remaining Frame Time
        if (t < deltatime)
        {
            SDL_Delay((Uint32) (deltatime - t));
        }
        
        deltatime -= deltaintegral;
        frame++;
        // One second has elapsed
        if (fps_count.get_ticks() >= 1000)
        {
            fps_counter = frame;
            frame       = 0;
            fps_count.start();
        }
    }

    quit_func(0);
}

int main(int argc, char* argv[])
{
    setupExitCallback();

    //	 Initialize timer and video systems
    if( SDL_Init( SDL_INIT_TIMER | SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) == -1 ) 
    { 
        std::cerr << "SDL Initialization Failed: " << SDL_GetError() << std::endl;
        return 1; 
    }

    menu = new Menu(&cannonboard);

    bool loaded = false;

    // Load LayOut File
    if (argc == 3 && strcmp(argv[1], "-file") == 0)
    {
        if (trackloader.set_layout_track(argv[2]))
            loaded = roms.load_revb_roms(); 
    }
    // Load Roms Only
    else
    {  
        loaded = roms.load_revb_roms();
    }

    //trackloader.set_layout_track("d:/temp.bin");
    //loaded = roms.load_revb_roms();

    if (loaded)
    {
        // Load XML Config
		//config.init();
        config.load(FILENAME_CONFIG);

        // Load fixed PCM ROM based on config
        if (config.sound.fix_samples)
            roms.load_pcm_rom(true);

        // Load patched widescreen tilemaps
        if (!omusic.load_widescreen_map())
            std::cout << "Unable to load widescreen tilemaps" << std::endl;

#ifndef SDL2
        //Set the window caption 
        SDL_WM_SetCaption( "Cannonball", NULL ); 
#endif

        // Initialize SDL Video
        if (!video.init(&roms, &config.video))
            quit_func(1);

#ifdef COMPILE_SOUND_CODE
        //audio.init();
#endif
        state = config.menu.enabled ? STATE_INIT_MENU : STATE_INIT_GAME;

        // Initalize controls
        input.init(config.controls.pad_id,
                   config.controls.keyconfig, config.controls.padconfig, 
                   config.controls.analog,    config.controls.axis, config.controls.asettings);

        if (config.controls.haptic) 
            config.controls.haptic = forcefeedback::init(config.controls.max_force, config.controls.min_force, config.controls.force_duration);
        
        // Initalize CannonBoard (For use in original cabinets)
        if (config.cannonboard.enabled)
        {
            cannonboard.init(config.cannonboard.port, config.cannonboard.baud);
            cannonboard.start();
        }

        // Populate menus
        menu->populate();
        main_loop();  // Loop until we quit the app
    }
    else
    {
       //quit_func(1);

        while(isRunning())
        {
            pspDebugScreenInit();
            pspDebugScreenSetXY(0, 0);
            printf("Cannot load roms!\n");
            sceDisplayWaitVblankStart();
            sceKernelSleepThread();
        }		

    }

    // Never Reached
    return 0;
}
