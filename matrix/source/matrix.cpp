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
std::map<const int, SDL_Surface *> letters_surf;
TTF_Font *matrix_font;


struct Letter {
	int x,y;
	int speed;
	char c[128];
	Uint32 color;
};

struct Letter letters[1280];

char randletter(int max) {
	char c;
	do {
		c = rand()%max;
	} while( c >= 'a' && c < 'z' );
	return c;
}

void init_letters(SDL_Surface *surf) {
	int i,z;
	for(i = 0; i < (1280/30); i++) {
		for(z = 0; z < (720/30); z++)
            letters[i].c[z] = randletter(128);

		letters[i].x = i*30;
		letters[i].y = -30*12;
		do
            letters[i].speed = rand()%10;
        while(letters[i].speed <= 0);
		letters[i].color = SDL_MapRGB(surf->format, 0, 255, 0);
	}
    
    for(int i = 33; i < 128; ++i) {
        static SDL_Color green = { 0, 255, 0 };
        std::string s;
        s += char(i);
        letters_surf[i] = TTF_RenderText_Solid(matrix_font, s.c_str(), green);
        if(!letters_surf[i]) {
            std::cerr << "Error creating surface..\n";
        }
    }
}


void draw(SDL_Surface *surf) {
	int i,z;
	for(i = 0; i < (1280/30); i++)
		for(z = 0; z < (720/30); z++) {
			if(letters[i].y+(z*12) > 0 && (letters[i].y+(z*12)) < surf->h) {
                SDL_Rect rc = { letters[i].x, letters[i].y+(z*12), 0, 0 };
                SDL_BlitSurface(letters_surf[letters[i].c[z]], 0, surf, &rc);
            }
        
		if(letters[i].y < surf->h)
			letters[i].y += letters[i].speed;
        
		if(letters[i].y >= surf->h) {
			letters[i].y = -30*12;
			do
                letters[i].speed = rand()%10;
			while (letters[i].speed <= 0);
		}
	}
    
}

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
    
    TTF_Init();
    
#ifdef CURRENT_PLATFORM
    matrix_font = TTF_OpenFont("matrix-code.ttf", 35);
#else
    matrix_font = TTF_OpenFont("/dev_hdd0/game/MTX00TEST/matrix-code.ttf", 35);
#endif
    
    if(!matrix_font) {
        std::cerr << "Error loading font..\n";
        exit(-1);
    }
    
    init_letters(front);
    
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
        draw(front);
        SDL_Flip(front);
        
        SDL_Delay(100);
        
#ifndef CURRENT_PLATFORM
        
        ioPadGetInfo (&padinfo);
        for(int i = 0; i < MAX_PADS; i++) {
            if(padinfo.status[i]) {
                ioPadGetData (i, &paddata);
            }
        }
#endif
      
    }
    
    for(int i = 33; i < 128; ++i) {
        SDL_FreeSurface(letters_surf[i]);
    }
    
    TTF_CloseFont(matrix_font);
    TTF_Quit();
    SDL_Quit();
    return 0;
}


