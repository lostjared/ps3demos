/*
 written by Jared Bruni
 http://lostsidedead.com
 */

#ifndef __3CARD__H__
#define __3CARD__H__

#include"SDL/SDL.h"
#include"SDL/SDL_ttf.h"
#include<iostream>
#include<string>
#include<cstdlib>
#include<ctime>


SDL_Surface *cards[3];
SDL_Surface *front_surface;

class Card {
    
public:
    
    enum CardFace { CardBack = 0, CardAce, CardKing } face;
    
    Card() : face(CardBack), x(0), y(0), shown(false) {
        
    }
    Card(int xx, int yy) : face(CardBack), x(xx), y(yy), shown(false) {
        
    }
    void draw() {
        
        SDL_Rect rc1 = { x, y, 320, 451 };
        
        if(shown == false)
            SDL_BlitSurface(cards[0], 0, front_surface, &rc1);
        else {
            if(is_ace()==true)
            SDL_BlitSurface(cards[1], 0, front_surface, &rc1);
            else
                SDL_BlitSurface(cards[2], 0, front_surface, &rc1);
        }
        
    }
    
    void setface(CardFace f) {
        face = f;
    }
    
    void setdir(int dir) {
        direction = dir;
    }
    
    void move(int dir, int xx, int yy) {
        x = xx;
        y = yy;
        direction = dir;
    }
    
    void setshown(bool s) {
        shown = s;
    }
    
    bool is_ace() {
        if(face == CardAce) return true;
        return false;
    }
    
    int getX() const { return x; }
    int getY() const { return y; }
    int dir() const { return direction; }
    
protected:
    int x,y;
    int direction;
    bool shown;
};


class Shuffle {
public:
    Shuffle() {
        prog_cards[0] = Card(320/2, 175);
        prog_cards[1] = Card(800, 175);
        prog_cards[2] = Card(320/2*3, 175);
        shuffle();
    }
    
    bool won() const {
        return win;
    }
    
    void show(int s) {
        switch(s) {
            case 0:
                for(int i = 0; i < 3; ++i) {
                    if(prog_cards[i].getX() == 320/2) {
                        prog_cards[i].setshown(true);
                        if(prog_cards[i].is_ace())
                            win = true;
                        else
                            win = false;
                        
                        return;
                    }
                    
                }
                break;
            case 1:
                for(int i = 0; i < 3; ++i) {
                    if(prog_cards[i].getX() == 480) {
                        prog_cards[i].setshown(true);
                        if(prog_cards[i].is_ace())
                            win = true;
                        else
                            win = false;
                        
                        return;
                    }
                    
                }
                break;
            case 2:
                for(int i = 0; i < 3; ++i) {
                    if(prog_cards[i].getX() == 800) {
                        prog_cards[i].setshown(true);
                        if(prog_cards[i].is_ace())
                            win = true;
                        else
                            win = false;
                        
                        return;
                    }
                    
                }
                break;
        }
        
    }
    
    
    void startShuffle() {
        for(unsigned int i = 0; i < 3; ++i) {
            prog_cards[i].setshown(false);
        }
    }
    
    void draw() {
        for(int i = 0; i < 3; ++i) prog_cards[i].draw();
    }
    
    void shuffle() {
        int r = rand()%3;
        switch(r) {
            case 0:
                prog_cards[0].setface(Card::CardAce);
                prog_cards[1].setface(Card::CardKing);
                prog_cards[2].setface(Card::CardKing);
                prog_cards[0].setshown(true);
                prog_cards[1].setshown(false);
                prog_cards[2].setshown(false);
                break;
            case 1:
                prog_cards[1].setface(Card::CardAce);
                prog_cards[0].setface(Card::CardKing);
                prog_cards[2].setface(Card::CardKing);
                prog_cards[1].setshown(true);
                prog_cards[0].setshown(false);
                prog_cards[2].setshown(false);
                break;
            case 2:
                prog_cards[2].setface(Card::CardAce);
                prog_cards[1].setface(Card::CardKing);
                prog_cards[0].setface(Card::CardKing);
                prog_cards[2].setshown(true);
                prog_cards[1].setshown(false);
                prog_cards[0].setshown(false);
                break;
        }
        
        prog_cards[0].setdir(1);
        prog_cards[1].setdir(0);
        prog_cards[2].setdir(1);
    }
    
    void move() {
        for(unsigned int i = 0; i < 3; ++i) {
            if(prog_cards[i].dir() == 0 && prog_cards[i].getX() >= 320/2) {
                prog_cards[i].move(prog_cards[i].dir(), prog_cards[i].getX()-25, prog_cards[i].getY());
            } else if(prog_cards[i].dir() == 0 && prog_cards[i].getX() <= 320/2) {
                prog_cards[i].move(1, prog_cards[i].getX(), prog_cards[i].getY());
            }
            
            if(prog_cards[i].dir() == 1 && prog_cards[i].getX() <= 802) {
                prog_cards[i].move(prog_cards[i].dir(), prog_cards[i].getX()+25, prog_cards[i].getY());
            } else if(prog_cards[i].dir() == 1 && prog_cards[i].getX() >= 802) {
                prog_cards[i].move(0, prog_cards[i].getX(), prog_cards[i].getY());
            }
        }
    }
    
    void set() {
        
        if(prog_cards[0].getX() < prog_cards[1].getX() && prog_cards[0].getX() < prog_cards[2].getX())
            prog_cards[0].move(prog_cards[0].dir(), 320/2, prog_cards[0].getY());
        else if(prog_cards[0].getX() > prog_cards[1].getX() && prog_cards[0].getX() > prog_cards[2].getX())
            prog_cards[0].move(prog_cards[0].dir(), 800, prog_cards[0].getY());
        else
            prog_cards[0].move(prog_cards[0].dir(), 480, prog_cards[0].getY());
        
        if(prog_cards[1].getX() < prog_cards[0].getX() && prog_cards[1].getX() < prog_cards[2].getX())
            prog_cards[1].move(prog_cards[1].dir(), 320/2, prog_cards[1].getY());
        else if(prog_cards[1].getX() > prog_cards[0].getX() && prog_cards[1].getX() > prog_cards[2].getX())
            prog_cards[1].move(prog_cards[1].dir(), 800, prog_cards[1].getY());
        else
            prog_cards[1].move(prog_cards[1].dir(), 480, prog_cards[1].getY());
        
        if(prog_cards[2].getX() < prog_cards[1].getX() && prog_cards[2].getX() < prog_cards[0].getX())
            prog_cards[2].move(prog_cards[2].dir(), 320/2, prog_cards[2].getY());
        else if(prog_cards[2].getX() > prog_cards[1].getX() && prog_cards[2].getX() > prog_cards[0].getX())
            prog_cards[2].move(prog_cards[2].dir(), 800, prog_cards[2].getY());
        else
            prog_cards[2].move(prog_cards[2].dir(), 480, prog_cards[2].getY());
        
    }
    
    
protected:
    Card prog_cards[3];
    bool win;
};


#endif
