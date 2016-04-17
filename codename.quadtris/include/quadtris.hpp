/*
 *
 *
 * http://lostsidedead.com
 * (c) 2012 Jared Bruni GPL
 */
/* This source code is Licensed under GPL. ( http://www.gnu.org/licenses/gpl.html ) */

#ifndef QUADTRIS_H_
#define QUADTRIS_H_
#include<iostream>
#include<iomanip>
#include<string>
#include<cstdlib>

    extern void playSound(int index);

	namespace quad {

		class GameGrid;
		class Game;
		class GamePiece;

		class Block {
		public:
			unsigned int x,y,color,timeout;

			Block();
			Block(const Block &b);
			void operator=(const Block &b);
			bool operator==(const Block &b);
			friend std::ostream &operator<<(std::ostream &out, const Block &b);
			void clear();
			inline void setColor(const unsigned int color) { this->color = color; }
			unsigned int getColor() { return color; }
			inline void setPosition( const unsigned int x, const unsigned int y) { this->x = x; this->y = y; }
			inline void getPosition(unsigned int &x, unsigned int &y) { x = this->x; y = this->y; }
			void Swap(Block &b);
		};

		enum block_t { BLOCK_NULL=0, BLOCK_VERT, BLOCK_HORIZ, BLOCK_SQUARE };

		class GamePiece {
		public:
			Block blocks[4];
			unsigned int x,y;
			enum BlockIndex { BLOCK_0=0, BLOCK_1, BLOCK_2, BLOCK_3 };
			GamePiece();
			GamePiece(const GamePiece &p);
			void randBlock();
			void copyBlock(const GamePiece *p);
			void operator=(const GamePiece &p);
			friend std::ostream &operator<<(std::ostream &out, GamePiece &p);
			void moveLeft();
			void moveRight();
			void moveDown();
			void showPiece(bool t);
			inline bool isPieceHidden() const { return visible; }
			// assumes color index is correct

			inline Block &colorAt(const BlockIndex i) { return blocks[static_cast<unsigned int>(i)]; }
			void swapColorsUp();
			void swapColorsDown();
			void setBlockType(block_t type);

			bool block_lock;
		protected:
			friend class GameGrid;
			bool visible;
			block_t type;
		};

		enum Direction { MOVE_LEFT, MOVE_RIGHT, MOVE_UP, MOVE_DOWN, MOVE_BLOCKSWITCH, MOVE_NULL };

		class GameGrid {
			void (*callback_x)();
		public:
			GamePiece piece;
			GameGrid();
			GameGrid(const unsigned int width, const unsigned int height);
			~GameGrid();
			void resetGrid(const unsigned int width, const unsigned int height);
			void eraseGrid();
			void fillGrid(const unsigned int color);
			void setDirection(const enum Direction d);
			inline Direction getDirection() { return direction; }
			void mergeBlocks();
			void mergeDown();
			void procBlocks();
			void update();
			void update_moveDown();
			void erase();
			void releaseBlock();
			bool testBlockPos(unsigned int x, unsigned int y);
			void setCallback(void (*callback)()) { callback_x = callback; }
			void setBlock(Block &b, unsigned int x, unsigned int y);
			void movePos(Direction dir);
			unsigned int grid_width() const { return width; }
			unsigned int grid_height() const { return height; }
			friend std::ostream &operator<<(std::ostream &out, const GameGrid &g);
			friend class Game;
			bool isGameOver() const { return gameOver; }

			Block **game_grid;
		protected:
			unsigned int width, height, position;
			enum Direction direction;
			bool gameOver;
			bool switch_block;
		};

		enum { DEFAULT_WIDTH=25, DEFAULT_HEIGHT=25 };

		struct Score {
			unsigned int score;
			unsigned int num_clr;
			unsigned int total_moves;
			unsigned int game_speed;
			unsigned int level;
			unsigned int BLOCK_COUNT;
			Score() { BLOCK_COUNT=5; score = num_clr = total_moves = level = game_speed = 0; }
			inline void clear() { score = num_clr = total_moves = 0; level = 1; setTimerInterval(1000); }
			void addClear(int total) { score += total; num_clr += 1; ++total_moves; if((num_clr%6)==0) increaseSpeed();  }
			int getTimerInterval() const { return game_speed; }
			void setTimerInterval(const int t) {
				game_speed = t;
				// set the timer
			}
			void increaseSpeed() {
				++level;
				if(game_speed > 100) game_speed -= 100;
				setTimerInterval(game_speed);
				if((level%2==0) && BLOCK_COUNT<8) ++BLOCK_COUNT;
				std::cout << "Speed increase.\n";
			}
		};

		std::ostream &operator<<(std::ostream &out, const Score &s);

		class Game {
		public:
			enum { GRID_0=0, GRID_1, GRID_2, GRID_3 };
			Game();
			void newGame(unsigned int width_, unsigned int height_, unsigned int sw, unsigned int sh);
			inline GameGrid &operator[](unsigned int grid) { return grids[grid]; }
			inline GameGrid &getActiveGrid() { return grids[current]; }
			inline GamePiece &getActiveBlock() { return grids[current].piece; }
			void update();
			void setActiveGrid(const unsigned int cur);
			unsigned int getActiveGridIndex() const;
			bool changeActiveGrid() const;
			void procBlocks();
			friend std::ostream &operator<<(std::ostream &out, const Game &g);
			void timer_Update();
			void setCallback( void (*callback)()) {
				for(unsigned int i = 0; i < 4; ++i) {
					grids[i].setCallback(callback);
				}
			}
		protected:
			GameGrid grids[4];
			unsigned int current;
		public:
			static Score score;
		};
        
    
    Block::Block() {
        clear();
    }
    
    Block::Block(const Block &b) {
        operator=(b);
    }
    
    void Block::operator=(const Block &b) {
        x = b.x;
        y = b.y;
        color = b.color;
        timeout = b.timeout;
    }
    
    bool Block::operator==(const Block &b) {
        if(b.color == color) return true;
        return false;
    }
    
    void Block::clear() {
        x = y = color = 0;
        timeout = 0;
    }
    
    void Block::Swap(Block &b) {
        Block temp(b);
        b = *this;
        *this = temp;
    }
    
    std::ostream &operator<<(std::ostream &out, const Block &b) {
        out << "Block [" << b.x << "," << b.y << "] = " << b.color << "\n";
        return out;
    }
    
    GamePiece::GamePiece() {
        x = y = 0;
        visible = false;
        type = BLOCK_NULL;
        block_lock = false;
    }
    
    GamePiece::GamePiece(const GamePiece &p) {
        copyBlock(&p);
    }
    
    void GamePiece::operator=(const GamePiece &p) {
        copyBlock(&p);
    }
    
    void GamePiece::showPiece(bool s) {
        visible = s;
    }
    
    std::ostream &operator<<(std::ostream &out, GamePiece &p) {
        out << "Game Block {" << p.x << "," << p.y << "} = {\n";
        for(int i = 0; i < 4; ++i)
            out << p.blocks[i];
        
        out << "\n};\n";
        return out;
    }
    
    void GamePiece::setBlockType(block_t type) {
        if(block_lock == true) return;
        switch (type) {
			case BLOCK_HORIZ:
				blocks[0].x = 0;
				blocks[0].y = 0;
				blocks[1].x = 0;
				blocks[1].y = 1;
				blocks[2].x = 0;
				blocks[2].y = 2;
				blocks[3].x = 0;
				blocks[3].y = 3;
				break;
			case BLOCK_VERT:
				blocks[0].x = 0;
				blocks[0].y = 0;
				blocks[1].x = 1;
				blocks[1].y = 0;
				blocks[2].x = 2;
				blocks[2].y = 0;
				blocks[3].x = 3;
				blocks[3].y = 0;
				break;
			case BLOCK_SQUARE:
				blocks[0].x = 0;
				blocks[0].y = 0;
				blocks[1].x = 0;
				blocks[1].y = 1;
				blocks[3].x = 1;
				blocks[3].y = 0;
				blocks[2].x = 1;
				blocks[2].y = 1;
				break;
			default:
				std::cerr << "Error setBlockType failed..\n";
				break;
        }
        this->type = type;
    }
    
    void GamePiece::randBlock() {
        type = block_t(1 + rand()%3);
        setBlockType(type);
        do {
            for(unsigned int i = 0; i < 4; ++i) {
                blocks[i].color = 1+(rand()%Game::score.BLOCK_COUNT);
            }
        } while(blocks[0] == blocks[1] && blocks[0] == blocks[2] && blocks[0] == blocks[3]);
    }
    
    void GamePiece::swapColorsUp() {
        Block tempBlocks[4];
        tempBlocks[0] = blocks[3];
        tempBlocks[1] = blocks[0];
        tempBlocks[2] = blocks[1];
        tempBlocks[3] = blocks[2];
        for(unsigned int i = 0; i < 4; ++i)
            blocks[i] = tempBlocks[i];
    }
    void GamePiece::swapColorsDown() {
        int temp_colors[4];
        for(unsigned int i = 0; i < 4; ++i) temp_colors[i] = blocks[i].color;
        blocks[0].color = temp_colors[1];
        blocks[1].color = temp_colors[2];
        blocks[2].color = temp_colors[3];
        blocks[3].color = temp_colors[0];
    }
    
    void GamePiece::copyBlock(const GamePiece *p) {
        for(unsigned int i = 0; i < 4; ++i) {
            blocks[i] = p->blocks[i];
        }
        x = p->x;
        y = p->y;
    }
    
    void GamePiece::moveLeft() {
        if(x > 0) --x;
    }
    
    void GamePiece::moveRight() {
        x++;
    }
    
    void GamePiece::moveDown() {
        y++;
    }
    
    GameGrid::GameGrid() {
        width = height = 0;
        game_grid = 0;
        direction = MOVE_NULL;
        position = 0;
        gameOver = false;
        switch_block = false;
        callback_x = 0;
    }
    
    GameGrid::GameGrid(const unsigned int w, const unsigned int h) {
        game_grid = 0;
        direction = MOVE_NULL;
        position = 0;
        gameOver = false;
        switch_block = false;
        callback_x = 0;
        resetGrid(w, h);
    }
    
    GameGrid::~GameGrid() {
        eraseGrid();
    }
    
    void GameGrid::eraseGrid() {
        if(game_grid == 0) return;
        for(unsigned int i = 0; i < width; ++i) {
            delete [] game_grid[i];
        }
        delete [] game_grid;
        game_grid = 0;
    }
    
    void GameGrid::resetGrid(const unsigned int w, const unsigned int h) {
        if(game_grid != 0) {
            eraseGrid();
        }
        game_grid = new Block*[w];
        for(unsigned int i = 0; i < w; ++i) {
            game_grid[i] = new Block[h];
            for(unsigned int hx = 0; hx < h; ++hx) {
                game_grid[i][hx].clear();
            }
        }
        width = w;
        height = h;
        direction = MOVE_NULL;
        gameOver = false;
    }
    
    void GameGrid::fillGrid(const unsigned int color) {
        for(unsigned int i = 0; i < width; ++i) {
            for(unsigned int z = 0; z < height; ++z) {
                game_grid[i][z].setColor(color);
            }
        }
    }
    
    void GameGrid::setDirection(const enum Direction d) {
        direction = d;
    }
    
    bool GameGrid::testBlockPos(unsigned int x, unsigned int y) {
        for(unsigned int i = 0; i < 4; ++i) {
            unsigned int tx=x+piece.x+piece.blocks[i].x;
            unsigned int ty=y+piece.y+piece.blocks[i].y;
            if(tx > 0 && tx < width && ty > 0 && ty < height && game_grid[tx][ty].getColor() != 0) return false;
        }
        return true;
    }
    void GameGrid::movePos(Direction dir) {
        ++quad::Game::score.total_moves;
        switch(dir) {
			case MOVE_LEFT: {
				if(piece.x > 0 && piece.y < height && testBlockPos(-1,0) == true)
					piece.moveLeft();
			}
				break;
			case MOVE_RIGHT:
			{
				if(piece.x < width-2 && piece.y < height-1) {
					for(unsigned int q = 0; q < 4; ++q) {
						unsigned int tx = piece.x+piece.blocks[q].x;
						unsigned int ty = piece.y+piece.blocks[q].y;
						if(tx > width-2 || ty > height-2)
							return;
					}
					if(testBlockPos(1, 0)==true) piece.moveRight();
				}
			}
				break;
			case MOVE_DOWN:
				update_moveDown();
				break;
			case MOVE_UP:
				piece.swapColorsDown();
				break;
			case MOVE_NULL:
				return;
				break;
			case MOVE_BLOCKSWITCH: {
                
				unsigned int type = piece.type;
				if(++type > 3) type = 1;
				for(unsigned int q = 0; q < 4; ++q) {
					if((piece.x+piece.blocks[q].x < width-3) && (piece.y+piece.blocks[q].y < height-3) && testBlockPos(1,1) == true && testBlockPos(0, 1) == true)
					{
						piece.setBlockType(block_t(type));
						return;
					}
					else
						return;
				}
			}
				break;
        }
    }
    // piece must be in a valid location
    
    void GameGrid::mergeBlocks() {
        for(unsigned int i = 0; i < 4; ++i) {
            unsigned int px=piece.x+piece.blocks[i].x;
            unsigned int py=piece.y+piece.blocks[i].y;
            game_grid[px][py] = piece.blocks[i];
            if(py <= 1) { gameOver = true;  }
        }
    }
    
    void GameGrid::setBlock(Block &b, unsigned int x, unsigned int y) {
        game_grid[x][y] = b;
        
    }
    
    void GameGrid::mergeDown() {
        for(unsigned int i = 0; i < width; ++i) {
            for(unsigned int z = 0; z < height-1; ++z) {
                if(game_grid[i][z].getColor() != 0 && game_grid[i][z+1].getColor() == 0) {
                    game_grid[i][z+1] = game_grid[i][z];
                    game_grid[i][z].setColor(0);
                    break;
                }
            }
        }
    }
    
    void GameGrid::procBlocks() {
        unsigned int i=0,z=0;
        for(i = 0; i < width; ++i) {
            for(z = 0; z < height; ++z) {
                if(game_grid[i][z].getColor() == 0xFE) {
                    ++game_grid[i][z].timeout;
                    if(game_grid[i][z].timeout > 7) {
                        game_grid[i][z].timeout = 0;
                        game_grid[i][z].color = 0;
                    }
                }
            }
        }
        for(i = 0; i < width-3; ++i) {
            for(z = 0; z < height; ++z) {
                Block cur_block = game_grid[i][z];
                if(cur_block.getColor() == 0 || cur_block.getColor() == 0xFE) continue;
                if(cur_block == game_grid[i+1][z] && cur_block == game_grid[i+2][z] && cur_block == game_grid[i+3][z]) {
                    
                    playSound(0);
                    
                    if(i+4 < width && game_grid[i+4][z] == cur_block) {
                        Game::score.addClear(1);
                        game_grid[i+4][z].setColor(0xFE);
                    }
                    if(i+5 < width && game_grid[i+5][z] == cur_block) {
                        Game::score.addClear(1);
                        game_grid[i+5][z].setColor(0xFE);
                    }
                    
                    if(i+6 < width && game_grid[i+6][z] == cur_block) {
                        Game::score.addClear(1);
                        game_grid[i+6][z].setColor(0xFE);
                    }
                    
                    Game::score.addClear(4);
                    for(unsigned int q = 0; q < 4; ++q)
                        game_grid[i+q][z].setColor(0xFE);
                    continue;
                }
            }
        }
        
        for(i = 0; i < width; ++i) {
            for(z = 0; z < height-3; ++z) {
                Block cur_block = game_grid[i][z];
                if(cur_block.getColor() == 0 || cur_block.getColor() == 0xFE) continue;
                if(cur_block == game_grid[i][z+1] && cur_block == game_grid[i][z+2] && cur_block == game_grid[i][z+3]) {
                    
                    playSound(0);
                    
                    if(z+4 < height && game_grid[i][z+4] == cur_block) {
                        Game::score.addClear(1);
                        game_grid[i][z+4].setColor(0xFE);
                    }
                    if(z+5 < height && game_grid[i][z+5] == cur_block) {
                        Game::score.addClear(1);
                        game_grid[i][z+5].setColor(0xFE);
                    }
                    
                    if(z+6 < height && game_grid[i][z+6] == cur_block) {
                        Game::score.addClear(1);
                        game_grid[i][z+6].setColor(0xFE);
                    }
                    
                    Game::score.addClear(4);
                    for(unsigned int q = 0; q < 4; ++q) {
                        game_grid[i][z+q].setColor(0xFE);
                    }
                    continue;
                }
            }
        }
        for(i = 0; i < width-1; ++i) {
            for(z = 0; z < height-1; ++z) {
                Block cur_block = game_grid[i][z];
                if(cur_block.getColor() == 0 || cur_block.getColor() == 0xFE) continue;
                if(cur_block == game_grid[i][z+1] && cur_block == game_grid[i+1][z] && cur_block == game_grid[i+1][z+1]) {
                    if(i+2 < width && z+2 < height && cur_block == game_grid[i+2][z+1] && cur_block == game_grid[i+2][z+2]) {
                        Game::score.addClear(2);
                        game_grid[i+2][z+1].setColor(0xFE);
                        game_grid[i+2][z+2].setColor(0xFE);
                    }
                    Game::score.addClear(4);
                    playSound(0);
                    game_grid[i][z].setColor(0xFE);
                    game_grid[i+1][z].setColor(0xFE);
                    game_grid[i+1][z+1].setColor(0xFE);
                    game_grid[i][z+1].setColor(0xFE);
                    continue;
                }
            }
        }
    }
    
    void GameGrid::update() {
    }
    
    void GameGrid::update_moveDown() {
        
        for(unsigned int q = 0; q < 4; ++q) {
            if(piece.y+piece.blocks[q].y > height-2) {
                mergeBlocks();
                releaseBlock();
                return;
            }
            else if(game_grid[piece.x+piece.colorAt(GamePiece::BlockIndex(q)).x][piece.y+piece.colorAt(GamePiece::BlockIndex(q)).y+1].getColor() != 0) {
                mergeBlocks();
                releaseBlock();
                return;
            }
        }
        piece.y++;
    }
    
    void GameGrid::erase() {
        for(unsigned int i = 0; i < width; ++i) {
            for(unsigned int z = 0; z < height; ++z) {
                game_grid[i][z].clear();
            }
        }
    }
    
    void GameGrid::releaseBlock() {
        if(gameOver == false) {
            piece.x = width/2;
            piece.y = 0;
            piece.randBlock();
        }
        if(callback_x != 0) callback_x();
    }
    
    Game::Game() {
        current = 0;
    }
    
    void Game::newGame(unsigned int width_, unsigned int height_, unsigned int sw, unsigned int sh) {
        grids[0].resetGrid(width_, height_);
        grids[0].setDirection(MOVE_UP);
        grids[1].resetGrid(width_, height_);
        grids[1].setDirection(MOVE_DOWN);
        grids[2].resetGrid(sw,sh);
        grids[2].setDirection(MOVE_LEFT);
        grids[3].resetGrid(sw, sh);
        grids[3].setDirection(MOVE_RIGHT);
        current = 0;
        for(unsigned int i = 0; i < 4; ++i) {
            grids[i].erase();
            grids[i].releaseBlock();
            grids[i].switch_block = false;
        }
        score.clear();
        setActiveGrid(GRID_1); // first grid is move down
        
    }
    
    void Game::setActiveGrid(const unsigned int cur) {
        current = cur;
        for(int i = 0; i < 4; ++i) {
            grids[i].piece.showPiece(false);
        }
        grids[current].piece.showPiece(true);
    }
    
    unsigned int Game::getActiveGridIndex() const {
        return current;
    }
    
    void Game::update() {
        
        procBlocks();
        for(unsigned int i = 0; i < 4; ++i) {
            grids[i].mergeDown();
        }
    }
    
    void Game::procBlocks() {
        for(unsigned int i = 0; i < 4; ++i) {
            grids[i].procBlocks();
        }
    }
    
    void Game::timer_Update() {
        grids[current].update_moveDown();
    }
    
    quad::Score Game::score;
    
    std::ostream &operator<<(std::ostream &out, const GameGrid &g) {
        for(unsigned int i=0; i < g.height; ++i) {
            for(unsigned int z = 0; z < g.width; ++z) {
                out << g.game_grid[z][i].getColor() << " ";
            }
            out << std::endl;
        }
        return out;
    }
    
    std::ostream &operator<<(std::ostream &out, const Game &g) {
        for(unsigned int i = 0; i < 4; ++i) {
            out << "Grid: " << i << "\n" << g.grids[i] << "\n";
        }
        return out;
    }
    
    std::ostream &operator<<(std::ostream &out, const Score &s) {
        out << "Score of: " << s.score << " : " << s.num_clr << std::endl;
        return out;
    }
}


#endif


