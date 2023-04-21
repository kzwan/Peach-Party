#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

class StudentWorld;

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

class Actor : public GraphObject {  //this is an abstract base class
public:
    //constructor
    Actor(StudentWorld* world, int imageID, int startX, int startY, int depth) : GraphObject(imageID, startX, startY, right, 0)
    {
      m_world = world;
    }
  
    virtual void doSomething() = 0;  //makes it pure virtual, so this is never called
  
    StudentWorld* getWorld() {  //gets world state
      return m_world;
    }
  
    virtual bool collided() {  //checks for collisions
      return false;
    }
  
    virtual void setIsSquare(bool val) {  //sets square type
      isSquare = val;
    }
  
    virtual void whenHit() {return;}
  
    bool isAlive() {
      return alive;
    }
  
    void kill() {
      alive = false;
    }
  
    bool getIsSquare() {
      return isSquare;
    }
  
private:
    StudentWorld* m_world;
    bool alive = true;
    bool isSquare = false;
    
};

/*---------------------------------------------------------------------------------------------------------------------
  MOVING ACTORS
---------------------------------------------------------------------------------------------------------------------*/

class MobileActor : public Actor {
public:
    //constructor
    MobileActor(StudentWorld* world, int imageID, int startX, int startY, int spriteDir): Actor(world, imageID, startX, startY, 0) {}
  
    virtual void doAction() = 0;
  
    virtual void doSomething();
  
    bool getState() {
      return state;
    }

    void setState(bool res) {  //sets state to either walking or waiting to roll
      state = res;
    }
  
    int getMoveDirection() {
      return moveDirection;
    }
  
    void setMoveDirection(int direction) {  //sets direction player will move in
      moveDirection = direction;
    }
  
    int getTicksToMove() {
      return ticks_to_move;
    }

    void setTicksToMove(int ticks) {  //sets the ticks to move
      ticks_to_move = ticks;
    }
  
    virtual void setPauseCounter() {return;}
  
    virtual void drop() {return;}
  
    virtual bool atFork() = 0;

    void cornerDirection();
  
    void spriteDirection();
  
    virtual void teleport();
  
    bool checkFork();
  
    int getOppDirection(int direction);
  
    void setForce(int num) {
      isForce = num;
    }
  

private:
    bool state = true; //waiting is true, walking is false
    int ticks_to_move = 0;
    int moveDirection = 0;
    int isForce = -1;
};

class Player : public MobileActor {
public:
    //constructor
    Player(StudentWorld* world, int imageID, int startX, int startY, int spriteDir, int numOfPlayer): MobileActor(world, imageID, startX, startY, 0)
    {
      playerNum = numOfPlayer;
    };
  
    virtual void doAction();
  
    virtual bool atFork();
  
    virtual void teleport();
    
    int getNumPlayer() {
      return playerNum;
    }
  
    void addCoins(int num) {
      m_coins += num;
    }
  
    int getCoins() {
      return m_coins;
    }
  
    void setCoins(int num) {
      m_coins = num;
    }
  
    void addStar(int n) {
      m_stars+= n;
    }
  
    int getStars() {
      return m_stars;
    }
  
    void setStars(int n) {
      m_stars = n;
    }
  
    int getRoll() {
      return m_roll;
    }
  
    void setRoll(int num) {
      m_roll = num;
    }
  
    bool keyHit();
  
    void giveVortex();
  
    bool hasVortx() {  //initially false
      return hasVortex;
    }
    
private:
    int playerNum;
    int m_coins = 0;
    int m_stars = 0;
    int m_roll = 0;
    bool hasVortex = false;  //try to initialize in constructor
};

class Peach : public Player {
public:
    //constructor
    Peach(StudentWorld* world, int startX, int startY) : Player(world, IID_PEACH, SPRITE_WIDTH*startX, SPRITE_HEIGHT*startY, right, 1) {};
  
private:
  //do not need privates
};

class Yoshi : public Player {
public:
    //constructor
    Yoshi(StudentWorld* world, int startX, int startY) : Player(world, IID_YOSHI, SPRITE_WIDTH*startX, SPRITE_HEIGHT*startY, right, 2) {};
  
private:
  //do not need privates
};

class Baddie : public MobileActor {
public:
    //constructor
    Baddie(StudentWorld* world, int imageID, int startX, int startY) : MobileActor(world, imageID, SPRITE_WIDTH*startX, SPRITE_HEIGHT*startY, right) {};
  
    virtual void doAction();
  
    virtual void baddieAction(int playerNum) = 0;
  
    virtual int squaresToMove() = 0;
  
    virtual void setPauseCounter() {
      pauseCounter = 180;
    }
  
    virtual bool collided() {
      return true;
    }
  
    virtual void whenHit();
  
    virtual bool atFork();
  
private:
    bool newPeach = true;
    bool newYoshi = true;
    int pauseCounter = 180;
    int squares_to_move;
};

class Boo : public Baddie {
public:
    //constructor
    Boo(StudentWorld* world, int startX, int startY) : Baddie(world, IID_BOO, startX, startY) {};
  
    virtual void baddieAction(int playerNum);
  
    virtual int squaresToMove() {
      return randInt(1, 3);
    }
  
private:

};

class Bowser : public Baddie {
public:
    //constructor
    Bowser(StudentWorld* world, int startX, int startY) : Baddie(world, IID_BOWSER, startX, startY) {};
  
    virtual void baddieAction(int playerNum);
  
    virtual int squaresToMove() {
      return randInt(1,10);
    }
  
    virtual void drop();
  
private:
  
};

/*---------------------------------------------------------------------------------------------------------------------
  SQUARE (IMMOBILE) ACTORS
---------------------------------------------------------------------------------------------------------------------*/

class Square : public Actor {
public:
    //constructor
    Square(StudentWorld* world, int imageID, int startX, int startY, int spriteDir): Actor(world, imageID, SPRITE_WIDTH*startX, SPRITE_HEIGHT*startY, 1)
    {
      setIsSquare(true);
    };
  
    virtual void doSomething();
  
    virtual void hasLandedOrPassed(int playerNum);
  
    virtual void squareAct(int playerNum) = 0;
  
    virtual void passAction(int playerNum) {return;}
  
    virtual void setNewSwap(bool s) {
      newSwap = s;
    }
  
    virtual bool getNewSwap() {
      return newSwap;
    }
  
private:
    bool isAlive;
    bool newPeach = true;
    bool newYoshi = true;
    bool passPeach = true;
    bool passYoshi = true;
    bool newSwap = true;
};

class CoinSquare : public Square {
public:
    //constructor
    CoinSquare(StudentWorld* world, int imageID, int startX, int startY) : Square(world, imageID, startX, startY, 0){};
  
private:
  
};

class BlueCoinSquare : public CoinSquare {
public:
    //constructor
    BlueCoinSquare(StudentWorld* world, int startX, int startY) : CoinSquare(world, IID_BLUE_COIN_SQUARE, startX, startY){};
  
    virtual void squareAct(int playerNum);
  
private:
  
};

class RedCoinSquare : public CoinSquare {
public:
    //constructor
    RedCoinSquare(StudentWorld* world, int startX, int startY) : CoinSquare(world, IID_RED_COIN_SQUARE, startX, startY){};
  
    virtual void squareAct(int playerNum);
  
private:
};

class StarSquare : public Square {
public:
    //constructor
    StarSquare(StudentWorld* world, int startX, int startY) : Square(world, IID_STAR_SQUARE, startX, startY, 0){};
  
    virtual void squareAct(int playerNum);
  
    virtual void passAction(int playerNum);
  
private:
  
};

class DirectionalSquare : public Square {
public:
    //constructor
    DirectionalSquare(StudentWorld* world, int startX, int startY, int squareDir) : Square(world, IID_DIR_SQUARE, startX, startY, squareDir)
    {
      setDirection(squareDir);
      forcingDirection = squareDir;
    }
  
    virtual void squareAct(int playerNum);
  
    virtual void passAction(int playerNum);
  
private:
    int forcingDirection;
};

class BankSquare : public Square {
public:
    //constructor
    BankSquare(StudentWorld* world, int startX, int startY) : Square(world, IID_BANK_SQUARE, startX, startY, 0){};
  
    virtual void squareAct(int playerNum);
  
    virtual void passAction(int playerNum);
  
private:
  
};

class EventSquare : public Square {
public:
    //constructor
    EventSquare(StudentWorld* world, int startX, int startY) : Square(world, IID_EVENT_SQUARE, startX, startY, 0){};
  
    virtual void squareAct(int playerNum);
  
    void swapPlayers();
  
private:
  
};

class DroppingSquare : public Square {
public:
    //constructor
    DroppingSquare(StudentWorld* world, int startX, int startY) : Square(world, IID_DROPPING_SQUARE, startX, startY, 0){};
  
    virtual void squareAct(int playerNum);
  
private:
  
};

class Vortex : public Actor {
public:
    //constructor
    Vortex(StudentWorld* world, int imageID, int startX, int startY, int spriteDir, int dir): Actor(world, IID_VORTEX, startX, startY, 0)
    {
        setDirection(dir);
      
    }
  
    virtual void doSomething();
  
private:
};


#endif // ACTOR_H_
