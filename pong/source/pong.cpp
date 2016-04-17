/* written by Jared Bruni
 http://lostsidedead.com
 */


#include<SDL/SDL.h>
#include<SDL/SDL_ttf.h>
#include<iostream>
#include<sstream>
#include<map>
#ifndef CURRENT_PLATFORM
#include <io/pad.h>
#include <sysutil/msg.h>
#include <sysutil/sysutil.h>
#endif


const int width=1280, height=720;

class Paddle {
public:
    Paddle() : x(0), y(0), width(0), height(0), scr(0) {}
    int x, y, width, height, scr;
    
    
    void moveDown() {
        if(y < ::height-100) y += 10;
    }
    void moveUp() {
        if(y > 0) y -= 10;
    }
    
    void score() {
        scr ++;
    }
};

class Ball {
public:
    Ball()  { }
    int x,y,w,h,dir;
};

class GameBoard {
public:
    GameBoard() {
        player1.x = 40;
        player1.y = (1280-100)/2;
        player2.x = 1280-40;
        player2.y = (1280-100)/2;
        player1.width = 10;
        player1.height = 100;
        player2.width = 10;
        player2.height = 100;
        ball.x = (1280/2)-(10/2);
        ball.y = (720/2)-(10/2);
        ball.w = 10;
        ball.h = 10;
        ball.dir = 4;
    }
    Paddle player1, player2;
    Ball ball;
};

GameBoard game;

void drawFrame(SDL_Surface *, SDL_Surface *, SDL_Surface *);
void procGame(Paddle &one, Paddle &two, Ball &ball, int speed);

int main(int argc, char **argv) {
    if(SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        std::cout << SDL_GetError() << "\n";
    }

    TTF_Init();
    
#ifndef CURRENT_PLATFORM
    SDL_Surface *front =  SDL_SetVideoMode(1280, 720, 32 ,SDL_SWSURFACE|SDL_DOUBLEBUF|SDL_FULLSCREEN);
    
    SDL_Surface *paddle = SDL_LoadBMP("/dev_hdd0/game/PONG00LSD/paddle.bmp");
    SDL_Surface *ball = SDL_LoadBMP("/dev_hdd0/game/PONG00LSD/ball.bmp");
    TTF_Font *font = TTF_OpenFont("/dev_hdd0/game/PONG00LSD/font.ttf", 16);
    padInfo padinfo;
#else
    SDL_Surface *front =  SDL_SetVideoMode(1280, 720, 32 ,0);
    SDL_Surface *paddle = SDL_LoadBMP("paddle.bmp");
    SDL_Surface *ball = SDL_LoadBMP("ball.bmp");
    TTF_Font *font = TTF_OpenFont("font.ttf", 16);
#endif
    

    if(!paddle || !ball) {
        std::cerr << "Error could not load bitmap\n";
        SDL_Quit();
        return -1;
    }
    
    SDL_SetColorKey(ball, SDL_TRUE, 0);
    
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
        
        
        if(keys[SDLK_UP] == 1) game.player1.moveUp();
        if(keys[SDLK_DOWN] == 1) game.player1.moveDown();
        if(keys[SDLK_a] == 1) game.player2.moveUp();
        if(keys[SDLK_s] == 1) game.player2.moveDown();
        
        SDL_FillRect(front, 0, 0);
        drawFrame(front,paddle, ball);
        procGame(game.player1, game.player2, game.ball, 5);
        
        static SDL_Color white = { 255,255,255 };
        
        std::ostringstream s;
        s << "Player 1 Score: " << game.player1.scr;
        
        SDL_Surface *font_surf = TTF_RenderText_Blended(font, s.str().c_str(),white);
        
        s.str("");
        s << "Player 2 Score: " << game.player2.scr;
        
        SDL_Surface *font_surf2 = TTF_RenderText_Blended(font,s.str().c_str(), white);
        
        SDL_Rect pos = { 100, 50, 0, 0 };
        SDL_BlitSurface(font_surf, 0, front, &pos);
        SDL_FreeSurface(font_surf);
        
        SDL_Rect pos2 = { 1280-250, 50, 0, 0 };
        SDL_BlitSurface(font_surf2, 0, front, &pos2);
        SDL_FreeSurface(font_surf2);
        
        SDL_Flip(front);
        
        
        
#ifndef CURRENT_PLATFORM
        ioPadGetInfo (&padinfo);
        
        
        if(padinfo.status[0]) {
            padData paddata;
            ioPadGetData (0, &paddata);
            if(paddata.BTN_UP) {
                game.player1.moveUp();
            }
            if(paddata.BTN_DOWN) {
                game.player1.moveDown();
            }
        }
        
        if(padinfo.status[1]) {
            padData paddata;
            ioPadGetData (1, &paddata);
            if(paddata.BTN_UP) {
                game.player2.moveUp();
            }
            if(paddata.BTN_DOWN) {
                game.player2.moveDown();
            }
        }
#endif
        
        SDL_Delay(5);
    }

    SDL_FreeSurface(ball);
    SDL_FreeSurface(paddle);
    SDL_Quit();
    TTF_Quit();
    return 0;
}

void drawFrame(SDL_Surface *front, SDL_Surface *paddle_tex, SDL_Surface *ball_tex)
{
    SDL_Rect rc1 = { game.player1.x, game.player1.y, game.player1.width, game.player1.height };
    SDL_Rect rc2 = { game.player2.x, game.player2.y, game.player2.width, game.player2.height };
    SDL_Rect ballrc = { game.ball.x, game.ball.y, game.ball.w, game.ball.h };
    

    SDL_BlitSurface(paddle_tex, 0, front, &rc1);
    SDL_BlitSurface(ball_tex, 0, front, &ballrc);
    SDL_FillRect(front, &rc2, SDL_MapRGB(front->format, 255, 255, 255));
    
}

void procGame(Paddle &one, Paddle &two, Ball &ball, int speed) {
    if(ball.dir == 1 && ball.x > 5 && ball.y > 17) {
        
        if(ball.x == one.x + 10 && ball.y >= one.y && ball.y <= one.y + 100) {
            ball.dir = rand()%2 + 3;
        }
        else {
            ball.x -= speed;
            ball.y -= speed;
        }
    }
    else if(ball.dir == 2 && ball.x > 5 && ball.y < 720) {
        
        if(ball.x == one.x + 10 && ball.y >= one.y && ball.y <= one.y + 100) {
            ball.dir = rand()% 2 + 3;
        }
        else {
            ball.x -= speed;
            ball.y += speed;
        }
    }
    else if(ball.dir == 3 && ball.x < 1280 && ball.y > 17) {
        
        if(ball.x == two.x - 10 && ball.y >= two.y && ball.y <= two.y + 100) {
            ball.dir = rand()%2 + 1;
            
        }
        else {
            ball.x += speed;
            ball.y -= speed;
        }
    }
    else if(ball.dir == 4 && ball.x < 1280 && ball.y < 720) {
        
        if(ball.x == two.x - 10 && ball.y >= two.y && ball.y <= two.y + 100) {
            ball.dir = rand()% 2 + 1;
        }
        else {
            ball.x += speed;
            ball.y += speed;
        }
    }
    else {
        if(ball.dir == 1 || ball.dir == 3) ball.dir++;
        else if(ball.dir == 2 || ball.dir == 4) ball.dir--;
    }
    
    if(ball.x < 6) {
        ball.x = 315;
        ball.y = 240;
        ball.dir = 1+(rand()%4);
        two.score();
    } if(ball.x > 1280-30) {
        ball.x = 315;
        ball.y = 240;
        ball.dir = 1+(rand()%4);
        one.score();
    }
    
}
