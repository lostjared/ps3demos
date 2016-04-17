/* 
 

 Codename Quadtris
 
 a mutli directional puzzle game
 
 written by Jared Bruni
 http://lostsidedead.com
 */

// comment out below if not in debug mode
//#define DEBUG_MODE

#include<stdlib.h>
#include<time.h>
#include<SDL/SDL.h>
#include<SDL/SDL_ttf.h>
#include<SDL/SDL_image.h>
#include<SDL/SDL_mixer.h>
#include<iostream>
#include<map>
#ifndef CURRENT_PLATFORM
#include <io/pad.h>
#include <sysutil/msg.h>
#include <sysutil/sysutil.h>
#endif
#include "console.hpp"
#include "quadtris.hpp"

unsigned int image_shift = 0;
bool connected = true;
bool image_fade=false;
int current_index = 0;
int screen_width = 1280, screen_height = 720;

quad::Game game;


enum KEY { K_DOWN=0, K_UP, K_LEFT, K_RIGHT, K_RETURN };
enum SCREEN { SCR_GAME, SCR_GAMEOVER, SCR_START, SCR_SUBMIT, SCR_INTRO };
SCREEN game_screen = SCR_INTRO;

SDL_Rect grid0 = {screen_width/4, 0, screen_width/2, screen_height/2};
SDL_Rect grid1 = {screen_width/4, screen_height/2, screen_width/2, screen_height/2};
SDL_Rect grid2 = {0, screen_height/4, screen_width/4, (screen_height/2)-32};
SDL_Rect grid3 = {screen_width-(screen_width/4), screen_height/4, screen_width/4, (screen_height/2)-32};
SDL_Rect rc0 = grid0;
SDL_Rect rc1 = grid1;
SDL_Rect rc2 = grid2;
SDL_Rect rc3 = grid3;


SDL_Surface *background_surface, *start_surface, *blocks[8], *logos[4], *border, *pointer, *jaredb_logo, *lost_logo;
SDL_Surface *front = 0;
SDL_TimerID timer1, timer2;

std::map<int, int> keys;

Mix_Chunk *intro_sound, *clear_sound;

void init();
void newGame();
void switchPiece(unsigned int &i);
void setActiveGrid(unsigned int i);
void changePiece();
void draw_game(SDL_Surface *surf);
void draw_gameover(SDL_Surface *surface);
void draw_intro(SDL_Surface *surface);
void drawGrid(quad::GameGrid &grid, SDL_Surface *surface, SDL_Rect &source_rect, int direction);
void drawBlock(quad::GamePiece &p,SDL_Rect source_rect,SDL_Surface *surface, int x, int y, int direction);
void loadGraphic(SDL_Surface **surf, std::string text);
unsigned int timerUpdateCallback(unsigned int old_value, void *param);
void joyDown(KEY key);


int main(int argc, char **argv) {
    
    SDL_Init(SDL_INIT_EVERYTHING);
    IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);
    SDL_Color green = {0,255,0};
#ifndef CURRENT_PLATFORM
    front =  SDL_SetVideoMode(1280, 720, 32 ,SDL_SWSURFACE|SDL_DOUBLEBUF|SDL_FULLSCREEN);
    padInfo padinfo;
    padData paddata;
     init_console(front, "/dev_hdd0/game/QUAD00LSD/font.ttf", &green);
#else
    front =  SDL_SetVideoMode(1280, 720, 32 ,0);
    init_console(front, "font.ttf", &green);
#endif
    std::cout << "Debug Info..\n";
    bool active = true;
    SDL_Event e;
    
    srand((int)time(0));
#ifndef CURRENT_PLATFORM
    Mix_OpenAudio( AUDIO_S16MSB, MIX_DEFAULT_FORMAT, 2, 1024 );
#else
    if(Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 4096) < 0) {
        std::cerr << Mix_GetError() << "\n";
        SDL_Quit();
        exit(0);
    }
    
#endif
    init();
    Mix_PlayChannel(-1, intro_sound, 0);

    
    
    while(active) {
        while(SDL_PollEvent(&e)) {
            switch(e.type) {
#ifdef CURRENT_PLATFORM
                case SDL_KEYDOWN:
                    if(e.key.keysym.sym == SDLK_ESCAPE) active = false;
                    keys[e.key.keysym.sym] = 1;
                    break;
                case SDL_KEYUP:
                    keys[e.key.keysym.sym] = 0;
                    break;
#endif
                case SDL_QUIT:
                    active = false;
                    break;
            }
        }
        
        
        SDL_FillRect(front, 0, 0);
        switch(game_screen) {
            case SCR_GAME:
                draw_game(front);
                break;
            case SCR_GAMEOVER:
                draw_gameover(front);
                break;
            case SCR_INTRO:
                draw_intro(front);
                break;
            default:
                break;
        }
//        draw_console();
        SDL_Flip(front);
        
        
#ifndef CURRENT_PLATFORM
        
        ioPadGetInfo (&padinfo);
            for(int i = 0; i < MAX_PADS; i++) {
                if(padinfo.status[i]) {
                    ioPadGetData (i, &paddata);
                    if(game_screen == SCR_GAME) {
                        
                        if(paddata.BTN_LEFT) {
                            joyDown(K_LEFT);
                        }
                        
                        if(paddata.BTN_RIGHT) {
                            joyDown(K_RIGHT);
                        }
                        
                        if(paddata.BTN_UP) {
                            joyDown(K_UP);
                        }
                        
                        if(paddata.BTN_DOWN) {
                            joyDown(K_DOWN);
                        }
                        
                        if(paddata.BTN_CROSS) {
                            joyDown(K_RETURN);
                        }
                    
                    } else if(game_screen == SCR_GAMEOVER) {
                        if(paddata.BTN_START) {
                            newGame();
                            game_screen = SCR_GAME;
                        }
                    }
            }
        }
#else
        if(game_screen == SCR_GAME) {
            if(keys[SDLK_LEFT]) {
                joyDown(K_LEFT);
            }
            
            if(keys[SDLK_RIGHT]) {
                joyDown(K_RIGHT);
            }
            
            if(keys[SDLK_UP]) {
                joyDown(K_UP);
            }
            
            if(keys[SDLK_DOWN]) {
                joyDown(K_DOWN);
            }
            if(keys[SDLK_RETURN]) {
                joyDown(K_RETURN);
            }
        } else if(game_screen == SCR_GAMEOVER) {
            if(keys[SDLK_RETURN]) {
                newGame();
                game_screen = SCR_GAME;
            }
        }
        
#endif
    
    }
    Mix_FreeChunk(intro_sound);
    Mix_FreeChunk(clear_sound);
    Mix_CloseAudio();
    IMG_Quit();
    SDL_Quit();
    return 0;
}

unsigned int timerUpdateCallback(unsigned int old_value, void *param) {
	game.timer_Update();
	return quad::Game::score.game_speed;
}

unsigned int blockProc(unsigned int old_value, void *param) {
	game.update();
	return old_value;
}

void newGame() {
    game.newGame((screen_width/2)/16, (screen_height/2)/16, (grid3.w/16), (grid3.h/16));
    game.setCallback(changePiece);
    timer1 = SDL_AddTimer(quad::Game::score.game_speed, timerUpdateCallback, 0);
    timer2 = SDL_AddTimer(25, blockProc, 0);
    game.setActiveGrid(1);
}

void init() {
    
    loadGraphic(&background_surface, "img/background.jpg");
	loadGraphic(&start_surface, "img/start.jpg");
	for (unsigned int i = 1; i <= 8; ++i) {
		std::ostringstream stream;
		stream << "img/" << "block" << i << ".jpg";
		loadGraphic(&blocks[i-1], stream.str());
	}
	loadGraphic(&logos[0], "img/newgame.jpg");
	loadGraphic(&logos[1], "img/aboutgame.jpg");
	loadGraphic(&logos[2], "img/exit.jpg");
	loadGraphic(&border, "img/boxborder.jpg");
	loadGraphic(&pointer, "img/cd.png");
	loadGraphic(&jaredb_logo, "img/jaredblogo.jpg");
	loadGraphic(&lost_logo, "img/lostlogo.jpg");
    
    rc0.y += 24;
    rc1.h -= 8;
    rc2.x += 32;
    rc2.h -= 8;
    rc3.w -= 32;
    rc3.h -= 8;
    
#ifdef CURRENT_PLATFORM
    clear_sound = Mix_LoadWAV("line.wav");
    intro_sound = Mix_LoadWAV("open.wav");
    
#else
    clear_sound = Mix_LoadWAV("/dev_hdd0/game/QUAD00LSD/line.wav");
    intro_sound = Mix_LoadWAV("/dev_hdd0/game/QUAD00LSD/open.wav");
    
#endif
    
    if(clear_sound == 0 || intro_sound == 0) {
        std::cerr << "Error could not load sound.\n";
    }

}

void playSound(int index) {
    switch(index) {
        case 0:
            Mix_PlayChannel(-1, clear_sound, 0);
            break;
    }
}

void loadGraphic(SDL_Surface **surf, std::string text) {
    std::string file_path;
#ifndef CURRENT_PLATFORM
    file_path = "/dev_hdd0/game/QUAD00LSD/"+text;
#else
    file_path = text;
#endif
	SDL_Surface *img = 0;
    if((img = IMG_Load(file_path.c_str())) == 0) {
		std::cerr << "Error could not load image: " << text << "\n";
		SDL_Quit();
		exit(0);
	} else {
		std::cout << "Image: " << text << " loaded.\n";
        *surf = SDL_DisplayFormat(img);
        SDL_FreeSurface(img);
	}
}


void switchPiece(unsigned int &i) {
	switch(i) {
        case quad::MOVE_UP:
            i = quad::MOVE_RIGHT;
            break;
        case quad::MOVE_DOWN:
            i = quad::MOVE_LEFT;
            break;
        case quad::MOVE_LEFT:
            i = quad::MOVE_UP;
            break;
        case quad::MOVE_RIGHT:
            i = quad::MOVE_DOWN;
            break;
	}
}

void setActiveGrid(unsigned int i) {
	switchPiece(i);
	if(game[0].isGameOver() == true && game[1].isGameOver() == true && game[2].isGameOver() == true && game[3].isGameOver() == true) {
		game_screen = SCR_GAMEOVER;
        SDL_RemoveTimer(timer1);
        SDL_RemoveTimer(timer2);
		return;
	}
	if(game[i].isGameOver()==true) {
		switchPiece(i);
	}
	game.setActiveGrid(i);
}

void changePiece() {
	unsigned int i = game.getActiveGridIndex();
	setActiveGrid(i);
}




void draw_game(SDL_Surface *surf) {
    
    SDL_BlitSurface(background_surface, 0, surf, 0);
    std::ostringstream score;
    score << "Level: " << quad::Game::score.level << " Score: " << quad::Game::score.score << " Cleared: " << quad::Game::score.num_clr;
    static SDL_Color color = {255,255,255};
    SDL_Surface *font_tex = TTF_RenderText_Blended(std::console.font, score.str().c_str(), color);
    SDL_Rect rc = { 20,45, font_tex->w, font_tex->h};
    SDL_FillRect(front, &rc, 0x0);
    SDL_BlitSurface(font_tex, 0, front, &rc);
    SDL_FreeSurface(font_tex);
    static unsigned int backcolor = 0x0;
    SDL_FillRect(surf, &rc0, backcolor);
    SDL_FillRect(surf, &rc1, backcolor);
    SDL_FillRect(surf, &rc2, backcolor);
    SDL_FillRect(surf, &rc3, backcolor);
    drawGrid(game[0],front,grid0, 0);// 0 - Up
    drawGrid(game[1],front,grid1, 1);// 1 - down
    drawGrid(game[2],front,grid2, 2);// 2 - left
    drawGrid(game[3],front,grid3, 3);// 3 - right
#ifdef DEBUG_MODE
    static int frames = 0, cur_frame = 0;
    static Uint32 ticks = SDL_GetTicks();
    std::ostringstream ts;
    Uint32 cur_tick = SDL_GetTicks();
    
    ts << "Frame Rate: " << cur_frame;
    
    SDL_Surface *fr_s = TTF_RenderText_Blended(std::console.font, ts.str().c_str(), color);
    
    SDL_Rect rc_s = { 100, 100, fr_s->w, fr_s->h };
    
    SDL_FillRect(front, &rc_s, 0x0);
    SDL_BlitSurface(fr_s, 0, front, &rc_s);
    SDL_FreeSurface(fr_s);
    
    if(cur_tick > ticks+1000) {
        ticks = SDL_GetTicks();
        cur_frame = frames;
        frames = 0;
    }
    
    frames++;
#endif
    
}

void draw_gameover(SDL_Surface *surface) {
    SDL_BlitSurface(start_surface, 0, surface, 0);
    SDL_Rect source = {75, 75, 650, 170};
    SDL_FillRect(surface, &source, 0);
    std::ostringstream str_stream, str_stream2;
    str_stream << "Game Over Your score: " << game.score.score << " Lines cleared: " << game.score.num_clr;
    if(game.score.num_clr < 6) {
        str_stream2 << "Try again, youll figure it out.";
    }
    else if(game.score.num_clr > 6) {
        str_stream2 << " Good job";
    } else if(game.score.num_clr > 6 && game.score.num_clr < 9) {
        str_stream2 << " Excellent ";
    } else if(game.score.num_clr > 9 && game.score.num_clr < 20) {
        str_stream2 << " Cool you did good.";
    } else if(game.score.num_clr > 20) {
        str_stream2 << " You got this figured out.";
    }
    
    str_stream2 << ", Press Start to play again.";
    
    SDL_Color color = {255,255,255};
    SDL_Surface *text = TTF_RenderText_Blended(std::console.font, str_stream.str().c_str(), color);
    SDL_Rect rc = {100,100,text->w, text->h};
    SDL_BlitSurface(text, 0, surface, &rc);
    SDL_FreeSurface(text);
    SDL_Surface *text2 = TTF_RenderText_Blended(std::console.font, str_stream2.str().c_str(), color);
    SDL_Rect rc2 = {100,125,text2->w, text2->h};
    SDL_BlitSurface(text2, 0, surface, &rc2);
    SDL_FreeSurface(text2);
}

void draw_intro(SDL_Surface *surface) {
    
    static int index = 0;
    
    switch(index) {
        case 0:
            SDL_BlitSurface(lost_logo, 0, surface, 0);
        break;
        case 1:
            SDL_BlitSurface(jaredb_logo, 0, surface, 0);
            break;
        case 2:
            newGame();
            game_screen = SCR_GAME;
            index = 0;
            break;
    }
    
    static Uint32 time_pos = SDL_GetTicks();
    Uint32 cur_time = SDL_GetTicks();
    
    if(cur_time > time_pos + 3000) {
        time_pos = cur_time;
        ++index;
    }
}

void drawGrid(quad::GameGrid &grid, SDL_Surface *surface, SDL_Rect &source_rect, int direction) {
	int x = 0, y = 0;
	int source_x = 0, source_y = 0;
	int grid_index = game.getActiveGridIndex();
	switch(direction) {
        case 0: // UP
            
        {
            int pos_x = source_rect.x;
            int pos_y = source_rect.y+source_rect.h;
            for(x = 0; x <  (int)grid.grid_width(); ++x) {
                for(y = 0; y < (int)grid.grid_height(); ++y) {
                    SDL_Rect pos_rect = {pos_x, pos_y, 16, 16};
                    unsigned int color = grid.game_grid[x][y].getColor();
                    if(color != 0 && color != 0xFE && grid.isGameOver() == false)

                        SDL_BlitSurface(blocks[color-1], 0, front, &pos_rect);
                        else if(color == 0xFE || (color != 0 && grid.isGameOver() == true))  {
                                SDL_FillRect(surface, &pos_rect, SDL_MapRGB(surface->format, rand()%255, rand()%255, rand()%255));
                        }
                    pos_y -= 16;
                }
                pos_y = source_rect.y+source_rect.h;
                pos_x += 16;
            }
        }
            break;
        case 1: // Down
            
        {
            int pos_x = source_rect.x;
            int pos_y = source_rect.y;
            for(x = 0; x < (int)grid.grid_width(); ++x) {
                for(y = 0; y < (int)grid.grid_height(); ++y) {
                    // draw
                    SDL_Rect pos_rect = {pos_x, pos_y, 16, 16};
                    unsigned int color = grid.game_grid[x][y].getColor();
                    if(color != 0 && color != 0xFE && grid.isGameOver() == false)
                        SDL_BlitSurface(blocks[color-1], 0, front, &pos_rect);
                    
                    else if(color == 0xFE || (color != 0 && grid.isGameOver() == true))
                        SDL_FillRect(surface, &pos_rect, SDL_MapRGB(surface->format, rand()%255, rand()%255, rand()%255));

                    pos_y += 16;
                }
                pos_y = source_rect.y;
                pos_x += 16;
            }
        }
            break;
        case 2: // Left
            
        {
            int pos_x = source_rect.x+source_rect.w+16;
            for(x = 0; x < (int)grid.grid_width(); ++x) {
                for(y = 0; y < (int)grid.grid_height(); ++y) {
                    SDL_Rect posRect = {pos_x-(y*16), source_rect.y+(x*16), 16, 16};
                    unsigned int color = grid.game_grid[x][y].getColor();
                    if(color != 0 && color != 0xFE && grid.isGameOver() == false)
                        SDL_BlitSurface(blocks[color-1], 0, front, &posRect);
                    
                    else if(color == 0xFE || (color != 0 && grid.isGameOver() == true))
                        SDL_FillRect(surface, &posRect, SDL_MapRGB(surface->format, rand()%255, rand()%255, rand()%255));
                }
            }
        }
            break;
        case 3: // Right
            
        {
            int pos_x = source_rect.x-32;
            for(x = 0; x < (int)grid.grid_width(); ++x) {
                for(y = 0; y < (int)grid.grid_height(); ++y) {
                    SDL_Rect posRect = {pos_x+(y*16), source_rect.y+(x*16), 16, 16};
                    unsigned int color = grid.game_grid[x][y].getColor();
                    if(color != 0 && color != 0xFE && grid.isGameOver() == false)
                        SDL_BlitSurface(blocks[color-1], 0, front, &posRect);
                    else if(color == 0xFE || (color != 0 && grid.isGameOver() == true)) SDL_FillRect(surface, &posRect, SDL_MapRGB(surface->format, rand()%255, rand()%255, rand()%255));
                }
            }
            
        }
            break;
	}
	if(direction == grid_index) {
		source_x += source_rect.x;
		source_y += source_rect.y;
		drawBlock(grid.piece,source_rect,surface,source_x, source_y,direction);
	}
}

void drawBlock(quad::GamePiece &p,SDL_Rect source_rect,SDL_Surface *surface, int x, int y, int direction) {
	switch(direction) {
        case 0: {
            for(unsigned int i = 0; i < 4; ++i) {
                int block_x =(int) p.x+p.blocks[i].x;
                int block_y =(int) p.y+p.blocks[i].y;
                SDL_Rect rct = {x+(block_x*16), source_rect.h-(y+(block_y*16)), 16, 16};
                if(p.blocks[i].color>0)
                    SDL_BlitSurface(blocks[p.blocks[i].color-1], 0, front, &rct);
            }
        }
            break;
        case 1: {
            for(unsigned int i = 0; i < 4; ++i) {
                int block_x = (int)p.x+p.blocks[i].x;
                int block_y = (int)p.y+p.blocks[i].y;
                SDL_Rect rct = {x+(block_x*16), (y+(block_y*16)), 16, 16};
                if(p.blocks[i].color>0)
                    SDL_BlitSurface(blocks[p.blocks[i].color-1], 0, front, &rct);
            }
        }
            break;
        case 2: {
            for(unsigned int i = 0; i < 4; ++i) {
                int pos_x = (int)source_rect.x+source_rect.w+16;
                int block_x = (int) p.x+p.blocks[i].x, block_y = p.y+p.blocks[i].y;
                SDL_Rect rct = {pos_x-(block_y*16), source_rect.y+(block_x*16), 16, 16};
                if(p.blocks[i].color>0)
                    SDL_BlitSurface(blocks[p.blocks[i].color-1], 0, front, &rct);
            }
        }
            break;
        case 3: {
            for(unsigned int i = 0; i < 4; ++i) {
                int pos_x = (int)source_rect.x-32;
                int block_x = (int)p.x+p.blocks[i].x, block_y = p.y+p.blocks[i].y;
                SDL_Rect rct = {pos_x+(block_y*16), source_rect.y+(block_x*16), 16, 16};
                if(p.blocks[i].color>0)
                    SDL_BlitSurface(blocks[p.blocks[i].color-1], 0, front, &rct);
            }
            
        }
            break;
	}
}


void joyDown(KEY key) {
    
    static Uint32 time_pos = SDL_GetTicks();
    
    Uint32 cur_time = SDL_GetTicks();
    
    if(cur_time > time_pos + 125) {
        time_pos = cur_time;
        quad::GameGrid &grid = game.getActiveGrid();
        switch (key) {
            case K_DOWN:
                switch (grid.getDirection()) {
                    case quad::MOVE_UP: {
                        static Uint32 tp = SDL_GetTicks();
                        Uint32 t = SDL_GetTicks();
                        
                        if(t > tp + 200) {
                            tp = t;
                            grid.movePos(quad::MOVE_UP);
                        }
                    }
                        break;
                    case quad::MOVE_DOWN:
                        grid.movePos(quad::MOVE_DOWN);
                        break;
                    case quad::MOVE_LEFT:
                        grid.movePos(quad::MOVE_RIGHT);
                        break;
                    case quad::MOVE_RIGHT:
                        grid.movePos(quad::MOVE_RIGHT);
                        break;
                    default:
                        break;
                }
                
                break;
            case K_UP:
                switch (grid.getDirection()) {
                    case quad::MOVE_UP:
                        grid.movePos(quad::MOVE_DOWN);
                        break;
                    case quad::MOVE_DOWN: {
                        static Uint32 tp = SDL_GetTicks();
                        Uint32 t = SDL_GetTicks();
                        
                        if(t > tp + 200) {
                            tp = t;
                            grid.movePos(quad::MOVE_UP);
                        }
                    }
                        break;
                    case quad::MOVE_LEFT:
                        grid.movePos(quad::MOVE_LEFT);
                        break;
                    case quad::MOVE_RIGHT:
                        grid.movePos(quad::MOVE_LEFT);
                        break;
                    default:
                        break;
                }
                break;
            case K_LEFT:
                switch (grid.getDirection()) {
                    case quad::MOVE_UP:
                        grid.movePos(quad::MOVE_LEFT);
                        break;
                    case quad::MOVE_DOWN:
                        grid.movePos(quad::MOVE_LEFT);
                        break;
                    case quad::MOVE_LEFT:
                        grid.movePos(quad::MOVE_DOWN);
                        break;
                    case quad::MOVE_RIGHT: {
                        static Uint32 tp = SDL_GetTicks();
                        Uint32 t = SDL_GetTicks();
                        
                        if(t > tp + 200) {
                            tp = t;
                            grid.movePos(quad::MOVE_UP);
                        }
                        
                    }
                        break;
                    default:
                        break;
                }
                break;
            case K_RIGHT:
                switch (grid.getDirection()) {
                    case quad::MOVE_UP:
                        grid.movePos(quad::MOVE_RIGHT);
                        break;
                    case quad::MOVE_DOWN:
                        grid.movePos(quad::MOVE_RIGHT);
                        break;
                    case quad::MOVE_LEFT: {
                        static Uint32 tp = SDL_GetTicks();
                        Uint32 t = SDL_GetTicks();
                        
                        if(t > tp + 200) {
                            tp = t;
                            grid.movePos(quad::MOVE_UP);
                        }
                        
                    }
                        break;
                    case quad::MOVE_RIGHT:
                        grid.movePos(quad::MOVE_DOWN);
                        break;
                    default:
                        break;
                }
                break;
            case K_RETURN: {
                
                    static Uint32 tp = SDL_GetTicks();
                    Uint32 t = SDL_GetTicks();
                
                    if(t > tp + 200) {
                        grid.movePos(quad::MOVE_BLOCKSWITCH);
                        tp = t;
                    }
                }
                break;
        }
        
    }
}

