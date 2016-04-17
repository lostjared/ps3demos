/*
 
 Random Pixels
 
 written by Jared Bruni
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

const int width=1280, height=720;

void drawScreen(SDL_Surface *surface);


int main(int argc, char **argv) {
    
    if(SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        std::cout << SDL_GetError() << "\n";
    }
    
    SDL_Color color = {255,0,0};
    
#ifndef CURRENT_PLATFORM
    SDL_Surface *front =  SDL_SetVideoMode(1280, 720, 32 ,SDL_SWSURFACE|SDL_DOUBLEBUF|SDL_FULLSCREEN);
    padInfo padinfo;
    padData paddata;
    init_console(front, "/dev_hdd0/game/RND00TEST/font.ttf", &color);
#else
    SDL_Surface *front =  SDL_SetVideoMode(1280, 720, 32 ,0);
    init_console(front, "font.ttf", &color);
#endif
    
    std::cout << "Hello World with the Console press start to scroll. Press X to quit.\n";
    
    bool active = true;
    SDL_Event e;
    
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
        drawScreen(front);
        draw_console();
        SDL_Flip(front);
        
        
#ifndef CURRENT_PLATFORM
        
        ioPadGetInfo (&padinfo);
        for(int i = 0; i < MAX_PADS; i++) {
            if(padinfo.status[i]) {
                ioPadGetData (i, &paddata);
                
                if(paddata.BTN_START) {
                    std::cout << "Hello World\n";
                }
                
                if(paddata.BTN_CROSS) {
                    active = false;
                }
                
            }
        }
#endif
    }
    
    SDL_Quit();
    
    return 0;
}


void drawScreen(SDL_Surface *surface) {
    if(SDL_MUSTLOCK(surface)) {
        SDL_LockSurface(surface);
    }
   
    for(int y = 0; y < surface->h; ++y) {
        for(int x = 0; x < surface->w; ++x) {
            Uint8 *buffer = (Uint8*)surface->pixels;
            buffer += (y*surface->pitch)+(x*4);
            Uint32 *color = (Uint32*)buffer;
            *color = SDL_MapRGB(surface->format, rand()%255, rand()%255, rand()%255);
        }
    }
    
    if(SDL_MUSTLOCK(surface)) {
        SDL_UnlockSurface(surface);
    }
}

