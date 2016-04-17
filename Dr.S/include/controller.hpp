// Jared Bruni 2015

#ifndef __CONTROLLER__H_
#define __CONTROLLER__H_
#include"SDL/SDL.h"
#ifndef CURRENT_PLATFORM
#include <io/pad.h>
#include <sysutil/msg.h>
#include <sysutil/sysutil.h>
#endif

namespace dr {
    
    enum BUTTON { BUTTON_CROSS, BUTTON_CIRCLE, BUTTON_TRIANGLE, BUTTON_SQUARE, BUTTON_L1, BUTTON_L2, BUTTON_R1, BUTTON_R2, BUTTON_START};
   
    enum HAT {  HAT_CENTERED, HAT_UP, HAT_RIGHT, HAT_DOWN, HAT_LEFT, HAT_RIGHTUP, HAT_RIGHTDOWN, HAT_LEFTUP, HAT_LEFTDOWN };
    
#ifdef CURRENT_PLATFORM
    unsigned int keys[256];
    unsigned int hat_k[256]; //= { 0, SDLK_UP, SDLK_RIGHT, SDLK_DOWN, SDLK_LEFT, 0, 0, 0, 0 };
#endif
    
    
    class Controller {
    public:
        // id is players controller 0-3
        Controller(int id) : cid(id), opened(false) {
#ifdef CURRENT_PLATFORM
            keys[BUTTON_CROSS] = 'a';
            keys[BUTTON_CIRCLE] = 's';
            keys[BUTTON_TRIANGLE] = 'd';
            keys[BUTTON_SQUARE] = 'f';
            keys[BUTTON_L1] = 'z';
            keys[BUTTON_L2] = 'x';
            keys[BUTTON_R1] = 'v';
            keys[BUTTON_R2] = 'b';
            keys[BUTTON_START] = SDLK_RETURN;
            hat_k[HAT_UP] = SDLK_UP;
            hat_k[HAT_LEFT] = SDLK_LEFT;
            hat_k[HAT_RIGHT] = SDLK_RIGHT;
            hat_k[HAT_DOWN] = SDLK_DOWN;
#endif
        }
        ~Controller() {
#ifdef CURRENT_PLATFORM
            if(opened == true) SDL_JoystickClose(joy);
#endif
        }
        bool openController() {
#ifdef CURRENT_PLATFORM
            if(SDL_NumJoysticks() == 0) {
                std::cerr << "Error could not detect Joystick..\n";
                opened = false;
                return false;
            }
            joy = SDL_JoystickOpen(cid);
            if(joy != 0) {
                std::cout << "Successfully opened joystick: " << SDL_JoystickName(cid) << "\n";
                opened = true;
                return true;
            } else {
                std::cerr << "Error could not open joystick #: " << cid << "\n";
                opened = false;
                return false;
            }

#else
            opened = true;
            return true;
#endif

            return false;
        }
        
        void closeController() {
#ifdef CURRENT_PLATFORM
            if(opened == true) {
                SDL_JoystickClose(joy);
                opened = false;
            }
#endif
        }
        
        bool reopenJoystick(int j) {
#ifdef CURRENT_PLATFORM
            closeController();
            cid = j;
            return openController();
#else
            cid = j;
            return true;
#endif
        }
        
        // use these when a joystick cannot be opened.
        
        bool pollKey(const BUTTON &b) {
#ifdef CURRENT_PLATFORM
            Uint8 *cur_keys = SDL_GetKeyState(0);
            for(unsigned i = 0; i < static_cast<int>(BUTTON_START); ++i) {
                if(cur_keys[keys[i]])
                    return true;
            
            }
#endif
            return false;
        }
        
        bool pollKeyHat(const HAT &h) {
#ifdef CURRENT_PLATFORM
            Uint8 *cur_keys = SDL_GetKeyState(0);
            for(unsigned i = 0; i < static_cast<int>(HAT_RIGHTUP); ++i) {
                if(cur_keys[hat_k[i]]) return true;
            }
#endif
            return false;
        }
        
        bool pollButton(const BUTTON &b) {
#ifdef CURRENT_PLATFORM
            if(opened == false) return false;
            return SDL_JoystickGetButton(joy, static_cast<int>(b));
#else
            ioPadGetInfo (&padinfo);
                if(padinfo.status[cid]) {
                    ioPadGetData (cid, &paddata);
                    if(paddata.BTN_CROSS && b == BUTTON_CROSS)  return true;
                    if(paddata.BTN_CIRCLE && b == BUTTON_CIRCLE) return true;
                    if(paddata.BTN_SQUARE && b == BUTTON_SQUARE) return true;
                    if(paddata.BTN_TRIANGLE && b == BUTTON_TRIANGLE) return true;
                    if(paddata.BTN_L1 && b == BUTTON_L1) return true;
                    if(paddata.BTN_L2 && b == BUTTON_L2) return true;
                    if(paddata.BTN_R1 && b == BUTTON_R1) return true;
                    if(paddata.BTN_R2 && b == BUTTON_R2) return true;
                    if(paddata.BTN_START && b == BUTTON_START) return true;
                }
            return false;
#endif
            return false;

}
        
        bool pollHat(const HAT &h) {
#ifdef CURRENT_PLATFORM
            if(opened == false) return false;
            return SDL_JoystickGetHat(joy, static_cast<int>(h));
#else
            ioPadGetInfo (&padinfo);
            if(padinfo.status[cid]) {
                ioPadGetData (cid, &paddata);
                
                if(paddata.BTN_LEFT && h == HAT_LEFT)  return true;
                if(paddata.BTN_RIGHT && h == HAT_RIGHT)  return true;
                if(paddata.BTN_UP && h == HAT_UP)  return true;
                if(paddata.BTN_DOWN && h == HAT_DOWN)  return true;
            }
#endif
            return false;
        }
        
        bool update() {
#ifdef CURRENT_PLATFORM
            if(opened == false) return false;
            SDL_JoystickUpdate();
            return true;
#endif
            return true;
        }
        
        bool connected() { return opened; }
    
    protected:
        int cid;
#ifdef CURRENT_PLATFORM
        SDL_Joystick *joy;
#else
        padInfo padinfo;
        padData paddata;
#endif
        bool opened;
    };
    
    
}






#endif