/* SDL Console written by Jared Bruni
   http://lostsidedead.com
*/


#ifndef CONSOLE_HPP_X
#define CONSOLE_HPP_X

#include<iostream>
#include<sstream>
#include<string>
#include<SDL/SDL.h>
#include<SDL/SDL_ttf.h>



class Console {
public:
    std::ostringstream sout;
    TTF_Font *font;

    Console() {}
    
    Console(std::string font_path, int x, int y, int width, int height, SDL_Surface *surface, SDL_Color *ccolor) {
        init_console(font_path, x, y, width, height, surface, ccolor);
    }
    
    void init_console(std::string font_path, int x, int y, int width, int height, SDL_Surface *surface, SDL_Color *ccolor) {
        
        console_x = x;
        console_y = y;
        console_w = width;
        console_h = height;
        draw_surface = surface;
        color = ccolor;
        
        
        if(!TTF_WasInit()) TTF_Init();
        
        font = TTF_OpenFont(font_path.c_str(), 14);
        
        if(!font) {
            std::cerr << "Error initalizling font...\n";
            TTF_Quit();
            SDL_Quit();
            exit(-1);
        }
        
        for(int i = 1; i < 255; ++i) {
            std::string s;
            s += char(i);
            letters[i] = TTF_RenderText_Blended(font, s.c_str(), *color);
        }
        
        
        
    }
    
    ~Console() {
        clean_letters();
        TTF_CloseFont(font);
        TTF_Quit();
    }
    
    
    void clean_letters() {
        for(int c = 1; c < 255; ++c) SDL_FreeSurface(letters[c]);
    }
    
    void drawScreen() {
        
        std::string text = sout.str();
        
        int cx = console_x;
        int cy = console_y;
        
        for(unsigned int i = 0; i < text.length(); ++i) {
            
            int letter = text[i];
            
            if(letter == ' ') {
                cx += 5;
                continue;
            }
            
            if(letter == '\n') {
                cx = console_x;
                cy += TTF_FontLineSkip(font);;
                continue;
            }
            
            if(cy+16 > console_h) return;
            
            SDL_Rect rc;
            rc.x = cx;
            rc.y = cy;
            rc.w = letters[letter]->w;
            rc.h = letters[letter]->h;
            
            SDL_BlitSurface(letters[letter], 0, draw_surface, &rc);
            
            cx += letters[letter]->w + 1;
            if(cx+letters[letter]->w > console_w) {
                cx = console_x;
                cy += TTF_FontLineSkip(font);
            }
        }
    }
    
    
protected:
    int console_x, console_y, console_w, console_h;
    SDL_Surface *draw_surface;
    
    SDL_Surface *letters[256];
    SDL_Color *color;
};


namespace std {
    Console console;
}

void init_console(SDL_Surface *surface, std::string path, SDL_Color *color) {
    std::console.init_console(path, 5, 5, surface->w, surface->h, surface, color);
}

void draw_console() {
    std::console.drawScreen();
}

void clear_console() {
    std::console.sout.str("");
}

#define cout console.sout
#define cerr console.sout


#endif
