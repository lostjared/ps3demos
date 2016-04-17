/* 
 

 2015 Jared Bruni
 http://lostsidedead.com
 

 */


#include<SDL/SDL.h>
#include<SDL/SDL_ttf.h>
#include<iostream>
#include<map>
#ifndef CURRENT_PLATFORM
#include <io/pad.h>
#include <sysutil/msg.h>
#include <sysutil/sysutil.h>
#endif
#include "console.hpp"
#include "controller.hpp"
#include "grid.hpp"

int main(int argc, char **argv) {
    
    if(SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        std::cout << SDL_GetError() << "\n";
    }
    
    SDL_Color green = {0,255,0};
    
#ifndef CURRENT_PLATFORM
    SDL_Surface *front =  SDL_SetVideoMode(1280, 720, 32 ,SDL_SWSURFACE|SDL_DOUBLEBUF|SDL_FULLSCREEN);
//    padInfo padinfo;
//    padData paddata;
     init_console(front, "/dev_hdd0/game/DR00TEST/font.ttf", &green);
#else
    SDL_Surface *front =  SDL_SetVideoMode(1280, 720, 32 ,0);
    init_console(front, "font.ttf", &green);
#endif
    
    std::cout << "Hello World with the Console.\nPress start to scroll.\n";
    bool active = true;
    SDL_Event e;
    
#ifdef CURRENT_PLATFORM
    bool use_keyboard = false;
#endif
    
    dr::Controller controller1(0);
    if(!controller1.openController()) {
#ifdef CURRENT_PLATFORM
        std::cout << "Falling back to Keyboard.\n";
        use_keyboard = true;
#endif
    } 
    while(active) {
        while(SDL_PollEvent(&e)) {
            switch(e.type) {
                case SDL_KEYDOWN:
                    if(e.key.keysym.sym == SDLK_ESCAPE) active = false;
        
                    break;
                case SDL_QUIT:
                    active = false;
                    break;
            }
        }
        
        
        SDL_FillRect(front, 0, 0);
        draw_console();
        SDL_Flip(front);
        
        
        if(controller1.connected()) {
            if(controller1.pollButton(dr::BUTTON_CROSS)) {
                std::cout << "Cross Pressed\n";
            }
        }
#ifdef CURRENT_PLATFORM
        else {
            
            if(use_keyboard == true) {
                if(controller1.pollKey(dr::BUTTON_CROSS)) {
                    std::cout << "Cross Keypressed\n";
                }
            }
        }
#endif
        
        SDL_Delay(10);
    }
    
    SDL_Quit();
    
    return 0;
}







