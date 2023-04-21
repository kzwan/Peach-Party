#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Actor.h"
#include "Board.h"
#include <string>
#include <list>

class Actor;
// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    virtual ~StudentWorld();
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    bool validPos(int x, int y);
    Player* getPlayer(int pNum);
    void replace(int x, int y);
    double distance(int x1, int x2, int y1, int y2);
    bool overlap(int x, int y);
    void spawnVortex(int x, int y, int dir);
    bool peachWin();
  
    int getBankBalance() {
      return bankBalance;
    }
  
    void setBankBalance(int num) {
      bankBalance += num;
    }
  
    void clearBankBalance() {
      bankBalance = 0;
    }

private:
    int m_lives;
    int bankBalance = 0;
    int m_boardNumber;
    Player* peach;
    Player* yoshi;
    std::list<Actor*> m_actors;
};

#endif // STUDENTWORLD_H_
