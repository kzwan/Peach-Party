#include "StudentWorld.h"
#include "Actor.h"
#include "GameConstants.h"
#include <string>
#include <list>

using namespace std;

GameWorld* createStudentWorld(string assetPath) {
    return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h, and Actor.cpp

StudentWorld::StudentWorld(string assetPath) : GameWorld(assetPath) {
}

//destructor
StudentWorld::~StudentWorld() {
    cleanUp();
}

int StudentWorld::init()
{
    m_boardNumber = getBoardNumber();
    Board board;
    string boardNum = "0" + to_string(m_boardNumber);
    string boardFile = assetPath() + "board" + boardNum + ".txt";
    Board::LoadResult result = board.loadBoard(boardFile);
    if (result == Board::load_fail_file_not_found || result == Board::load_fail_bad_format)
        return GWSTATUS_BOARD_ERROR;
    else if (result == Board::load_success) {
        for (int i = 0; i < SPRITE_WIDTH; i ++) {
            for (int j = 0; j < SPRITE_HEIGHT; j++) {
                Board::GridEntry ge = board.getContentsOf(i, j);
                switch (ge) {
                    case Board::empty:
                        break;
                    case Board::boo:
                        m_actors.push_back(new Boo(this, i, j));
                        m_actors.push_back(new BlueCoinSquare(this, i, j));
                        break;
                    case Board::bowser:
                        m_actors.push_back(new Bowser(this, i, j));
                        m_actors.push_back(new BlueCoinSquare(this, i, j));
                        break;
                    case Board::player:
                        peach = new Peach(this, i, j);
                        yoshi = new Yoshi(this, i,j);
                        m_actors.push_back(new BlueCoinSquare(this, i, j));
                        break;
                    case Board::red_coin_square:
                        m_actors.push_back(new RedCoinSquare(this, i, j));
                        break;
                    case Board::blue_coin_square:
                        m_actors.push_back(new BlueCoinSquare(this, i, j));
                        break;
                    case Board::up_dir_square:
                        m_actors.push_back(new DirectionalSquare(this, i, j, 90));
                        break;
                    case Board::down_dir_square:
                        m_actors.push_back(new DirectionalSquare(this, i, j, 270));
                        break;
                    case Board::left_dir_square:
                        m_actors.push_back(new DirectionalSquare(this, i, j, 180));
                        break;
                    case Board::right_dir_square:
                        m_actors.push_back(new DirectionalSquare(this, i, j, 0));
                        break;
                    case Board::event_square:
                        m_actors.push_back(new EventSquare(this, i, j));
                        break;
                    case Board::bank_square:
                        m_actors.push_back(new BankSquare(this, i, j));
                        break;
                    case Board::star_square:
                        m_actors.push_back(new StarSquare(this, i, j));
                        break;
                }
            }
        }
    }
        startCountdownTimer(99);
        return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    if (peach->isAlive())
        peach->doSomething();
    if (yoshi->isAlive())
        yoshi->doSomething();
    for (list<Actor*>::iterator itr = m_actors.begin(); itr != m_actors.end(); itr++) {
        if ((*itr)->isAlive()) {
            (*itr)->doSomething();
        } else {
            delete *itr;
            itr = m_actors.erase(itr);
        }
    }
    
    string peachVortex = "";
    string yoshiVortex = "";
    if (getPlayer(1)->hasVortx()) {
        peachVortex = " VOR";
    }
    if (getPlayer(2)->hasVortx()) {
        yoshiVortex = " VOR";
    }
    string statLine = "P1 Roll: " + to_string(this->getPlayer(1)->getTicksToMove()/8) + " Stars: " + to_string(this->getPlayer(1)->getStars()) + " $$: " + to_string(this->getPlayer(1)->getCoins()) + peachVortex + " | Time: " + to_string(timeRemaining()) + " | Bank: " + to_string(this->getBankBalance()) + " | P2 Roll: " + to_string(this->getPlayer(2)->getTicksToMove()/8) + " Stars: " + to_string(this->getPlayer(2)->getStars()) + " $$: " +  to_string(this->getPlayer(2)->getCoins()) + yoshiVortex;
    setGameStatText(statLine);
    
    if (timeRemaining() <= 0) {
        playSound(SOUND_GAME_FINISHED);
        if (peachWin()) {
            setFinalScore(getPlayer(1)->getStars(), getPlayer(1)->getCoins());
            return GWSTATUS_PEACH_WON;
        } else {
            setFinalScore(getPlayer(2)->getStars(), getPlayer(2)->getCoins());
            return GWSTATUS_YOSHI_WON;
        }
    }
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    list<Actor*>::iterator it;
    it = m_actors.begin();
    
    while (it != m_actors.end()) {
        delete *it;
        it = m_actors.erase(it);
    }
    
    delete peach;
    peach = nullptr;
    delete yoshi;
    yoshi = nullptr;
}

bool StudentWorld::validPos(int x, int y) { //change later
    m_boardNumber = getBoardNumber();
    Board board;
    string boardNum = "0" + to_string(m_boardNumber);
    string boardFile = assetPath() + "board" + boardNum + ".txt";
    Board::LoadResult result = board.loadBoard(boardFile);
    Board::GridEntry ge = board.getContentsOf(x/SPRITE_WIDTH, y/SPRITE_HEIGHT);
    if (ge != Board::empty && x%16 == 0 && y%16 == 0) {
        return true;
    }
    return false;
}

Player* StudentWorld::getPlayer(int playerNum) {
    if (playerNum == 1)
        return peach;
    else
        return yoshi;
}

bool StudentWorld::overlap(int x, int y) {
    for (list<Actor*>::iterator itr = m_actors.begin(); itr != m_actors.end(); itr++) {
        if ((*itr)->collided()) {
            int actX = (*itr)->getX();
            int actY = (*itr)->getY();
            if (distance(actX, x, actY, y) <= 16) {//idk bro?
                (*itr)->whenHit(); //will only kill first one in list
                return true;
            }
        }
    }
    return false;
}

void StudentWorld::spawnVortex(int x, int y, int dir) {
    m_actors.push_back(new Vortex(this, IID_VORTEX, x, y, 0, dir));
}

void StudentWorld::replace(int x, int y) {
    for (list<Actor*>::iterator it = m_actors.begin(); it != m_actors.end(); it++) {
        if ((*it)->getX() == x && (*it)->getY() == y && (*it)->getIsSquare()) {
            delete *it;
            it = m_actors.erase(it);
        }
    }
    m_actors.push_back(new DroppingSquare(this, x/SPRITE_WIDTH, y/SPRITE_HEIGHT));
}

double StudentWorld::distance(int x1, int x2, int y1, int y2){
    return pow(pow(x1 - x2, 2.0) + pow(y1 - y2, 2.0), 0.5);
}

bool StudentWorld::peachWin() {
    if (getPlayer(1)->getStars() > getPlayer(2)->getStars())
        return true;
    else if (getPlayer(1)->getStars() < getPlayer(2)->getStars())
        return false;
    else {
        if (getPlayer(1)->getCoins() > getPlayer(2)->getCoins())
            return true;
        else if (getPlayer(1)->getCoins() < getPlayer(2)->getCoins())
            return false;
    }
    
    int chance = randInt(1, 2);
    if (chance == 1)
        return false;
    else
        return true;
}

