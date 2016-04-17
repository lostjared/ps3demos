/* written by Jared Bruni
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


const int width=1280, height=720;

int main(int argc, char **argv) {
    
    SDL_Init(SDL_INIT_EVERYTHING);
    
#ifndef CURRENT_PLATFORM
    SDL_Surface *front =  SDL_SetVideoMode(1280, 720, 32 ,SDL_SWSURFACE|SDL_DOUBLEBUF|SDL_FULLSCREEN);
    padInfo padinfo;
    padData paddata;
#else
    SDL_Surface *front =  SDL_SetVideoMode(1280, 720, 32 ,0);
#endif
    
    bool active = true;
    SDL_Event e;
    
    std::map<int, int> keys;
    
    while(active) {
        while(SDL_PollEvent(&e)) {
            switch(e.type) {
                case SDL_KEYDOWN:
                    if(e.key.keysym.sym == SDLK_ESCAPE) active = false;
                    keys[e.key.keysym.sym] = 1;
                    break;
                case SDL_KEYUP:
                    keys[e.key.keysym.sym] = 0;
                    break;
                case SDL_QUIT:
                    active = false;
                    break;
            }
        }
        
        
        SDL_FillRect(front, 0, 0);
        SDL_Flip(front);
        
#ifndef CURRENT_PLATFORM
        
        ioPadGetInfo (&padinfo);
        for(int i = 0; i < MAX_PADS; i++) {
            if(padinfo.status[i]) {
                ioPadGetData (i, &paddata);
            }
        }
#endif
        
    }
    
    SDL_Quit();
    return 0;
}


