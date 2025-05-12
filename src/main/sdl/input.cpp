/***************************************************************************
    SDL Based Input Handling.

    Populates keys array with user input.
    If porting to a non-SDL platform, you would need to replace this class.

    Copyright Chris White.
    See license.txt for more details.
***************************************************************************/

#include <iostream>
#include <cstdlib> // abs
#include "sdl/input.hpp"

#ifdef PSP

#define PSP_CTRL_SELECT 0x000001
#define PSP_CTRL_START 0x000008
#define PSPS_CTRL_START 0x000008


#define PSP_CTRL_UP 0x000010 
#define PSP_CTRL_RIGHT 0x000020
#define PSP_CTRL_DOWN 0x000040 
#define PSP_CTRL_LEFT 0x000080

#define PSP_CTRL_LTRIGGER 0x000100 
#define PSP_CTRL_RTRIGGER 0x000200
#define PSP_CTRL_TRIANGLE 0x001000
#define PSP_CTRL_CIRCLE 0x002000
#define PSP_CTRL_CROSS  0x004000
#define PSP_CTRL_SQUARE 0x008000

#define PSP_CTRL_HOME 0x010000
#define PSP_CTRL_HOLD 0x020000

#endif

int input_buffer = 4;

Input input;

Input::Input(void)
{
}

Input::~Input(void)
{
}

void Input::init(int pad_id, int* key_config, int* pad_config, int analog, int* axis, int* analog_settings)
{
    this->key_config  = key_config;
    this->pad_config  = pad_config;
    this->analog      = analog;
    this->axis        = axis;
    this->wheel_zone  = analog_settings[0];
    this->wheel_dead  = analog_settings[1];
    this->pedals_dead = analog_settings[2];

    gamepad = SDL_NumJoysticks() > pad_id;

    if (gamepad)
    {
        stick = SDL_JoystickOpen(pad_id);
    }

    a_wheel = CENTRE;
}

void Input::close()
{
    if (gamepad && stick != NULL)
        SDL_JoystickClose(stick);
}

// Detect whether a key press change has occurred
bool Input::has_pressed(presses p)
{
    return keys[p] && !keys_old[p];
}

// Detect whether key is still pressed
bool Input::is_pressed(presses p)
{
    return keys[p];
}

// Detect whether pressed and clear the press
bool Input::is_pressed_clear(presses p)
{
    bool pressed = keys[p];
    keys[p] = false;
    return pressed;
}

// Denote that a frame has been done by copying key presses into previous array
void Input::frame_done()
{
    memcpy(&keys_old, &keys, sizeof(keys));
}

void Input::handle_key_down(SDL_keysym* keysym)
{
    key_press = keysym->sym;
    handle_key(key_press, true);
}

void Input::handle_key_up(SDL_keysym* keysym)
{
    handle_key(keysym->sym, false);
}

#ifdef PSP 
void Input::psp_handle_key(unsigned int flag)
{
    input_buffer++;

    switch (flag) {
        case 0:
            keys[BRAKE] = true;
            break;
        case 1:
            keys[DOWN] = true;
            break;
        case 2:
            keys[UP] = true;
            break;
        case 3:
            keys[RIGHT] = true;
            break;
        case 4:
            keys[LEFT] = true;
            break;
        case 5:
            if (input_buffer >= 4) {
                keys[START] = true;
                input_buffer = 4;
            }
            break;
        case 6:
            keys[ACCEL] = true;
            break;
        case 7:
            if (input_buffer >= 4) {
                keys[MENU] = true;
                input_buffer = 0;
            }
            break;
        case 8:
            if (input_buffer >= 4) {
                keys[COIN] = true;
                input_buffer = 0;
            }
            break;
        case 9:
            keys[GEAR1] = true;
            break;
        case 10:
            keys[GEAR2] = true;
            break;
        case 11:
            keys[VIEWPOINT] = true;
        default:
            break;
    }
}

void Input::psp_handle_release(unsigned int flag)
{
    switch (flag) {
        case 0:
            keys[BRAKE] = false;
            break;
        case 1:
            keys[DOWN] = false;
            break;
        case 2:
            keys[UP] = false;
            break;
        case 3:
            keys[RIGHT] = false;
            break;
        case 4:
            keys[LEFT] = false;
            break;
        case 5:
            keys[START] = false;
            break;
        case 6:
            keys[ACCEL] = false;
            break;
        case 7:
            keys[MENU] = false;
            break;
        case 8:
            keys[COIN] = false;
            break;
        case 9:
            keys[GEAR1] = false;
            break;
        case 10:
            keys[GEAR2] = false;
            break;
        case 11:
            keys[VIEWPOINT] = false;
            break;
        default:
            break;
    }
    /*
    if(flag !=  PSP_CTRL_DOWN)
        keys[DOWN] = false;

    if(flag !=  PSP_CTRL_UP)
        keys[UP] = false;

    if(flag != PSP_CTRL_RIGHT)
        keys[RIGHT] = false;
    
    if(flag != PSP_CTRL_LEFT)
        keys[LEFT] = false;
    
    if(flag !=  PSP_CTRL_START)
        keys[START] = false;

    if(flag != PSP_CTRL_CROSS)
        keys[ACCEL] = false;

    if(flag != PSP_CTRL_SQUARE)
        keys[BRAKE] = false;

    if(flag != PSP_CTRL_HOME)
        keys[MENU] = false;

    if(flag != PSP_CTRL_SELECT)
        keys[COIN] = false;

    if(flag != PSP_CTRL_LTRIGGER)
        keys[GEAR1] = false;
    
    if(flag != PSP_CTRL_RTRIGGER)
        keys[GEAR2] = false;

    if(flag != PSP_CTRL_TRIANGLE)
        keys[VIEWPOINT] = false;
        */
}


#endif


void Input::handle_key(const int key, const bool is_pressed)
{
    // Redefinable Key Input
    if (key == key_config[0])
        keys[UP] = is_pressed;

    else if (key == key_config[1])
        keys[DOWN] = is_pressed;

    else if (key == key_config[2])
        keys[LEFT] = is_pressed;

    else if (key == key_config[3])
        keys[RIGHT] = is_pressed;

    if (key == key_config[4])
        keys[ACCEL] = is_pressed;

    if (key == key_config[5])
        keys[BRAKE] = is_pressed;

    if (key == key_config[6])
        keys[GEAR1] = is_pressed;

    if (key == key_config[7])
        keys[GEAR2] = is_pressed;

    if (key == key_config[8])
        keys[START] = is_pressed;

    if (key == key_config[9])
        keys[COIN] = is_pressed;

    if (key == key_config[10])
        keys[MENU] = is_pressed;

    if (key == key_config[11])
        keys[VIEWPOINT] = is_pressed;

    // Function keys are not redefinable
    switch (key)
    {
        case SDLK_F1:
            keys[PAUSE] = is_pressed;
            break;

        case SDLK_F2:
            keys[STEP] = is_pressed;
            break;

        case SDLK_F3:
            keys[TIMER] = is_pressed;
            break;

        case SDLK_F5:
            keys[MENU] = is_pressed;
            break;
    }
}

void Input::handle_joy_axis(SDL_JoyAxisEvent* evt)
{
    int16_t value = evt->value;

    // Digital Controls
    if (!analog)
    {
        // X-Axis
        if (evt->axis == 0)
        {
            // Neural
            if ( (value > -DIGITAL_DEAD ) && (value < DIGITAL_DEAD ) )
            {
                keys[LEFT]  = false;
                keys[RIGHT] = false;
            }
            else if (value < 0)
            {
                keys[LEFT] = true;
            }
            else if (value > 0)
            {
                keys[RIGHT] = true;
            }
        }
        // Y-Axis
        else if (evt->axis == 1)
        {
            // Neural
            if ( (value > -DIGITAL_DEAD ) && (value < DIGITAL_DEAD ) )
            {
                keys[UP]  = false;
                keys[DOWN] = false;
            }
            else if (value < 0)
            {
                keys[UP] = true;
            }
            else if (value > 0)
            {
                keys[DOWN] = true;
            }
        }
    }
    // Analog Controls
    else
    {
        //std::cout << "Axis: " << (int) evt->axis << " Value: " << (int) evt->value << std::endl;

        // Steering
        // OutRun requires values between 0x48 and 0xb8.
        if (evt->axis == axis[0])
        {
            int percentage_adjust = ((wheel_zone) << 8) / 100;         
            int adjusted = value + ((value * percentage_adjust) >> 8);
            
            // Make 0 hard left, and 0x80 centre value.
            adjusted = ((adjusted + (1 << 15)) >> 9);
            adjusted += 0x40; // Centre

            if (adjusted < 0x40)
                adjusted = 0x40;
            else if (adjusted > 0xC0)
                adjusted = 0xC0;

            // Remove Dead Zone
            if (wheel_dead)
            {
                if (std::abs(CENTRE - adjusted) <= wheel_dead)
                    adjusted = CENTRE;
            }

            //std::cout << "wheel zone : " << wheel_zone << " : " << std::hex << " : " << (int) adjusted << std::endl;
            a_wheel = adjusted;
        }
        // Accelerator and Brake [Combined Axis]
        else if (axis[1] == axis[2] && (evt->axis == axis[1] || evt->axis == axis[2]))
        {
            // Accelerator
            if (value < -pedals_dead)
            {
                // Scale input to be in the range of 0 to 0x7F
                value = -value;
                int adjusted = value / 258;
                adjusted += (adjusted >> 2);
                a_accel = adjusted;
            }
            // Brake
            else if (value > pedals_dead)
            {
                // Scale input to be in the range of 0 to 0x7F
                int adjusted = value / 258;
                adjusted += (adjusted >> 2);
                a_brake = adjusted;
            }
            else
            {
                a_accel = 0;
                a_brake = 0;
            }
        }
        // Accelerator [Single Axis]
        else if (evt->axis == axis[1])
        {
            // Scale input to be in the range of 0 to 0x7F
            int adjusted = 0x7F - ((value + (1 << 15)) >> 9);           
            adjusted += (adjusted >> 2);
            a_accel = adjusted;
        }
        // Brake [Single Axis]
        else if (evt->axis == axis[2])
        {
            // Scale input to be in the range of 0 to 0x7F
            int adjusted = 0x7F - ((value + (1 << 15)) >> 9);
            adjusted += (adjusted >> 2);
            a_brake = adjusted;
        }
    }
}

void Input::handle_joy_down(SDL_JoyButtonEvent* evt)
{
    // Latch joystick button presses for redefines
    joy_button = evt->button;
    handle_joy(evt->button, true);
}

void Input::handle_joy_up(SDL_JoyButtonEvent* evt)
{
    handle_joy(evt->button, false);
}

void Input::handle_joy(const uint8_t button, const bool is_pressed)
{
    if (button == pad_config[0])
        keys[ACCEL] = is_pressed;

    if (button == pad_config[1])
        keys[BRAKE] = is_pressed;

    if (button == pad_config[2])
        keys[GEAR1] = is_pressed;

    if (button == pad_config[3])
        keys[GEAR2] = is_pressed;

    if (button == pad_config[4])
        keys[START] = is_pressed;

    if (button == pad_config[5])
        keys[COIN] = is_pressed;

    if (button == pad_config[6])
        keys[MENU] = is_pressed;

    if (button == pad_config[7])
        keys[VIEWPOINT] = is_pressed;
}