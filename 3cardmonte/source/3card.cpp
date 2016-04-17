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
#include "3card.hpp"

const int Width = 1280, Height = 720;
void updateScreen();
void initTextures();
void freeTextures();

bool moving = false;
bool shuffled = true;

SDL_Surface *display_textures[6];
SDL_Rect display_cords[6];
void initFontTextures(TTF_Font *font);

int main(int argc, char **argv) {

    if(SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        std::cout << SDL_GetError() << "\n";
    }
    
#ifndef CURRENT_PLATFORM
    SDL_Surface *front =  SDL_SetVideoMode(1280, 720, 32 ,SDL_SWSURFACE|SDL_DOUBLEBUF|SDL_FULLSCREEN);
    padInfo padinfo;
    padData paddata;
#else
    SDL_Surface *front =  SDL_SetVideoMode(1280, 720, 32 ,0);
#endif
    front_surface = front;
    TTF_Init();
    
    Shuffle shuffle;
    
#ifdef CURRENT_PLATFORM
    TTF_Font *font = TTF_OpenFont("font.ttf", 24);
#else
    TTF_Font *font = TTF_OpenFont("/dev_hdd0/game/JCD00TEST/font.ttf", 24);
#endif
    
    initFontTextures(font);
    initTextures();
    
    bool active = true;
    SDL_Event e;
    int index = 0;
    bool show_win = false;
    
    while(active) {
        while(SDL_PollEvent(&e)) {
            switch(e.type) {
                case SDL_KEYDOWN:
                    if(e.key.keysym.sym == SDLK_ESCAPE) active = false;
                    switch(e.key.keysym.sym) {
                        case SDLK_RETURN:
                            if(shuffled == false) continue;
                            
                            if(moving == false) {
                                shuffle.startShuffle();
                                moving = true;
                                index = 1;
                            }
                            else {
                                moving = false;
                                shuffle.set();
                                index = 3;
                                
                            }
                            break;
                        case SDLK_SPACE: {
                            if(moving == false && shuffled == false) {
                                shuffle.shuffle();
                                shuffled = true;
                                index = 0;
                                show_win = false;
                            }
                        }
                            break;
                    }
                    
                    if(shuffled == true && moving == false) {
                        switch(e.key.keysym.sym) {
                            case SDLK_1:
                                shuffle.show(0);
                                shuffled = false;
                                show_win = true;
                                index = 2;
                                break;
                            case SDLK_2:
                                shuffle.show(1);
                                shuffled = false;
                                show_win = true;
                                index = 2;
                                break;
                            case SDLK_3:
                                shuffle.show(2);
                                shuffled = false;
                                show_win = true;
                                index = 2;
                                break;
                        }
                    }
                    break;
                case SDL_QUIT:
                    active = false;
                    break;
            }
        }
        
        if(moving == true)
            shuffle.move();
        
        SDL_FillRect(front, 0, 0);
        
        
        SDL_Rect rc1 = { 25, 25, display_cords[index].w, display_cords[index].h};
        SDL_BlitSurface(display_textures[index], 0, front, &rc1);
   
        if(show_win) {
            int index;
            
            if(shuffle.won()==true)
                index = 4;
            else
                index = 5;
            
            SDL_Rect rc1 = { 25, 70, display_cords[index].w, display_cords[index].h };
            SDL_BlitSurface(display_textures[index], 0, front, &rc1);
    
          
        }
        
        shuffle.draw();
        
        SDL_Flip(front);
        
        
#ifndef CURRENT_PLATFORM
        
        ioPadGetInfo (&padinfo);
        for(int i = 0; i < MAX_PADS; i++) {
            if(padinfo.status[i]) {
                ioPadGetData (i, &paddata);
                
                if(paddata.BTN_START) {
                    if(shuffled == false) continue;
                    
                    if(moving == false) {
                        shuffle.startShuffle();
                        moving = true;
                        index = 1;
                    }
                }
                
                if(paddata.BTN_TRIANGLE) {
                    if(moving == true) {
                        moving = false;
                        shuffle.set();
                        index = 3;
                        
                    }
                }
                
                if(paddata.BTN_SELECT) {
                    if(moving == false && shuffled == false) {
                        shuffle.shuffle();
                        shuffled = true;
                        index = 0;
                        show_win = false;
                    }
                }
                
                if(shuffled == true && moving == false) {
                    if(paddata.BTN_SQUARE) {
                        shuffle.show(0);
                        shuffled = false;
                        show_win = true;
                        index = 2;
                    }
                    
                    if(paddata.BTN_CROSS) {
                        shuffle.show(1);
                        shuffled = false;
                        show_win = true;
                        index = 2;
                    }
                    if(paddata.BTN_CIRCLE) {
                        shuffle.show(2);
                        shuffled = false;
                        show_win = true;
                        index = 2;
                    }
                }
            }
        }
#endif
        
        SDL_Delay(10);
    }

    TTF_CloseFont(font);
    freeTextures();
    TTF_Quit();
    SDL_Quit();
    
    return 0;
}

void setRect(SDL_Rect *rc, SDL_Surface *surface) {
    rc->w = surface->w;
    rc->h = surface->h;
    rc->x = rc->y = 0;
}

SDL_Surface *createTextureFromString(TTF_Font *font, std::string s, SDL_Rect *rc) {
    SDL_Surface *surface;
    SDL_Color color = {255,255,255};
    surface = TTF_RenderText_Solid(font, s.c_str(), color);
    if(!surface) {
        std::cerr << "Error creating texture: " << TTF_GetError() << "\n";
        return 0;
    }
    setRect(rc, surface);
    return surface;
}



void initFontTextures(TTF_Font *font) {
    display_textures[0] = createTextureFromString(font, "Press Start to Shuffle", &display_cords[0]);
    display_textures[1] = createTextureFromString(font, "Press /\\ to Stop Shuffle", &display_cords[1]);
    display_textures[2] = createTextureFromString(font, "Press Select to Reset", &display_cords[2]);
    display_textures[3] = createTextureFromString(font, "Press [],X or O to Select Card", &display_cords[3]);
    display_textures[4] = createTextureFromString(font, "You Win!", &display_cords[4]);
    display_textures[5] = createTextureFromString(font, "You Lose!", &display_cords[5]);
}

void initTextures() {
#ifdef CURRENT_PLATFORM
    cards[0] = SDL_LoadBMP("cardback.bmp");
    cards[1] = SDL_LoadBMP("cardace.bmp"),
    cards[2] = SDL_LoadBMP("cardking.bmp");
#else
    cards[0] = SDL_LoadBMP("/dev_hdd0/game/JCD00TEST/cardback.bmp");
    cards[1] = SDL_LoadBMP("/dev_hdd0/game/JCD00TEST/cardace.bmp"),
    cards[2] = SDL_LoadBMP("/dev_hdd0/game/JCD00TEST/cardking.bmp");
#endif
    
}

void freeTextures() {
    int i;
    for(i = 0; i < 3; ++i) SDL_FreeSurface(cards[i]);
    for(i = 0; i < 6; ++i) SDL_FreeSurface(display_textures[i]);
}



