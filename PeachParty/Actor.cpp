#include "Actor.h"
#include "StudentWorld.h"
#include "GameConstants.h"

/*---------------------------------------------------------------------------------------------------------------------
  Mobile Actor Implementations
---------------------------------------------------------------------------------------------------------------------*/

void MobileActor::doSomething() {
    if (state) { //if the actor is in the waiting to roll/not moving state
        doAction();  //calls an action to do for the actor
    } else { //if the actor is in the walking state
        if (checkFork() && isForce == -1){ //fork
            if (!atFork()) //if player is not at a fork (no need to get input), return from tick do something
                return;
        }
        isForce = -1;  //sets to negative 1
        cornerDirection(); //sets direction at corners
        spriteDirection(); //sets sprite direction
        switch (getMoveDirection()) { //sets move diection of actor
            case (right):
                moveAtAngle(right, 2);
                break;
            case (up):
                moveAtAngle(up, 2);
                break;
            case (left):
                moveAtAngle(left, 2);
                break;
            case (down):
                moveAtAngle(down, 2);
                break;
            default:
                break;
        }
        ticks_to_move--;
        if (ticks_to_move == 0) {
            state = true;  //is in walking state
            setPauseCounter();  //pause counter for baddies (initially at 180)
            drop();  //random chance to drop square
        }
    }
}

void MobileActor::cornerDirection() {
    int direction = getMoveDirection();
    if (ticks_to_move % 8 == 0)
    {
        int newX = 0;
        int newY = 0;
        switch(direction) {
            case (right):
                getPositionInThisDirection(right, 16, newX, newY);
                if (!getWorld()->validPos(newX, newY)) {
                    getPositionInThisDirection(up, 16, newX, newY);
                    if (getWorld()->validPos(newX, newY)){
                        setMoveDirection(up);
                    }
                    getPositionInThisDirection(down, 16, newX, newY);
                    if (getWorld()->validPos(newX, newY))
                        setMoveDirection(down);
                }
                break;
            case(up):
                getPositionInThisDirection(up, 16, newX, newY);
                if (!getWorld()->validPos(newX, newY)) {
                    getPositionInThisDirection(right, 16, newX, newY);
                    if (getWorld()->validPos(newX, newY))
                        setMoveDirection(right);
                    else
                        setMoveDirection(left);
                }
                break;
            case(left):
                getPositionInThisDirection(left, 16, newX, newY);
                if (!getWorld()->validPos(newX, newY)) {
                    getPositionInThisDirection(up, 16, newX, newY);
                    if (getWorld()->validPos(newX, newY))
                        setMoveDirection(up);
                    else
                        setMoveDirection(down);
                }
                break;
            case(down):
                getPositionInThisDirection(down, 16, newX, newY);
                if (!getWorld()->validPos(newX, newY)) {
                    getPositionInThisDirection(right, 16, newX, newY);
                    if (getWorld()->validPos(newX, newY))
                        setMoveDirection(right);
                    else
                        setMoveDirection(left);
                }
                break;
            default:
                break;
        }
    }
    spriteDirection();
}

void MobileActor::spriteDirection() {
    if (getMoveDirection() == left)
        setDirection(left);
    else
        setDirection(right);
}

bool MobileActor::checkFork() {
    int newX;
    int newY;
    int count = 0;
    for (int i = 0; i < 4; i++) {
        getPositionInThisDirection(i * 90, 16, newX, newY);
        if (getWorld()->validPos(newX, newY))
            count ++;
    }
    if (count >= 3)
        return true;
    return false;
}

void MobileActor::teleport() {
    int randX;
    int randY;
    do {
        randX = randInt(0, SPRITE_WIDTH*BOARD_WIDTH-1);
        randY = randInt(0, SPRITE_HEIGHT*BOARD_HEIGHT-1);
    } while(!getWorld()->validPos(randX, randY));
    moveTo(randX, randY);
}



/*---------------------------------------------------------------------------------------------------------------------
  Player Implementations
---------------------------------------------------------------------------------------------------------------------*/

void Player::doAction() {
    int action = getWorld()->getAction(this->getNumPlayer()); //gets input from user
    switch (action) {
        case ACTION_NONE: {
            return;
            break;
        }
        case ACTION_ROLL: {
            int die_roll = randInt(1, 10);
            setRoll(die_roll);
            setTicksToMove(die_roll*8);
            setState(false);  //changes to walking
            break;
        }
        case ACTION_FIRE: {
            if (hasVortex) {
                getWorld()->spawnVortex(getX(), getY(), getMoveDirection());
                getWorld()->playSound(SOUND_PLAYER_FIRE);
                hasVortex = false;  //no more vortex
            }
        }
        default:
            return;
    }
}

bool Player::keyHit() {
    int action = getWorld()->getAction(this->getNumPlayer());
    int newX;
    int newY;
    switch (action) {
        case ACTION_NONE: {
            return false;
            break;
        }
        case ACTION_UP: {
            getPositionInThisDirection(up, 16, newX, newY);
            if (getWorld()->validPos(newX, newY) && getMoveDirection() != down) {
                setMoveDirection(up);
                return true;
            }
            return false;
        }
        case ACTION_DOWN: {
            getPositionInThisDirection(down, 16, newX, newY);
            if (getWorld()->validPos(newX, newY) && getMoveDirection() != up) {
                setMoveDirection(down);
                return true;
            }
            return false;
        }
        case ACTION_RIGHT: {
            getPositionInThisDirection(right, 16, newX, newY);
            if (getWorld()->validPos(newX, newY) && getMoveDirection() != left) {
                setMoveDirection(right);
                return true;
            }
            return false;
        }
        case ACTION_LEFT: {
            getPositionInThisDirection(left, 16, newX, newY);
            if (getWorld()->validPos(newX, newY) && getMoveDirection() != right) {
                setMoveDirection(left);
                return true;
            }
            return false;
        }
        default:
            return false;
    }
}

bool Player::atFork() {
    if (keyHit())
        return true;
    return false;
}

void Player::teleport() {
    MobileActor::teleport();
    
    int newX;
    int newY;
    int randDir;
    do {
        randDir = randInt(0, 3)*90;
        getPositionInThisDirection(randDir, 16, newX, newY);
    } while(!getWorld()->validPos(newX, newY));
    setMoveDirection(randDir);
    spriteDirection();
}

void Player::giveVortex() {
    hasVortex = true;
}

/*---------------------------------------------------------------------------------------------------------------------
  Baddie Implementations
---------------------------------------------------------------------------------------------------------------------*/

void Baddie::doAction() {
    for (int i = 1; i < 3; i++) {  //loops through player numbers
        if (getWorld()->getPlayer(i)->getX() == this->getX() && getWorld()->getPlayer(i)->getY() == this->getY()) {
            if (getWorld()->getPlayer(i)->getState()) {
                if ((i == 1 && newPeach) || (i == 2 && newYoshi)) {
                    if (i == 1)
                        newPeach = false;
                    else
                        newYoshi = false;
                    baddieAction(i);
                }
            }
        }
        else {
            if (i == 1)
                newPeach = true;
            else
                newYoshi = true;
        }
    }
    pauseCounter--;
    if (pauseCounter == 0) {
        squares_to_move = squaresToMove();  //gets number of squares to move
        setTicksToMove(squares_to_move * 8);
        setMoveDirection(randInt(0, 3) * 90);
        spriteDirection();
        setState(false);
    }
}

bool Baddie::atFork() {
    setMoveDirection(randInt(0, 3) * 90);
    return true;
}

void Baddie::whenHit() {
    teleport();
    setMoveDirection(right);
    spriteDirection();
    setState(true);
    setPauseCounter();
}

void Bowser::baddieAction(int playerNum) {
    int chance = randInt(1, 2);
    if (chance == 1) {
        getWorld()->getPlayer(playerNum)->setCoins(0);
        getWorld()->playSound(SOUND_BOWSER_ACTIVATE);
    }
}

void Bowser::drop() {
    int randChance = randInt(1, 4);
    if (randChance == 3) {
        getWorld()->replace(getX(), getY());
        getWorld()->playSound(SOUND_DROPPING_SQUARE_CREATED);
    }
}

void Boo::baddieAction(int playerNum) {
    int chance = randInt(1, 2);
    if (chance == 1) {
        int tempCoins = getWorld()->getPlayer(1)->getCoins();
        getWorld()->getPlayer(1)->setCoins(getWorld()->getPlayer(2)->getCoins());
        getWorld()->getPlayer(2)->setCoins(tempCoins);
    } else {
        int tempStars = getWorld()->getPlayer(1)->getStars();
        getWorld()->getPlayer(1)->setStars(getWorld()->getPlayer(2)->getStars());
        getWorld()->getPlayer(2)->setStars(tempStars);
    }
    getWorld()->playSound(SOUND_BOO_ACTIVATE);
}

/*---------------------------------------------------------------------------------------------------------------------
  Square Implementations
---------------------------------------------------------------------------------------------------------------------*/

void Square::hasLandedOrPassed(int playerNum) {
    if (getWorld()->getPlayer(playerNum)->getX() == this->getX() && getWorld()->getPlayer(playerNum)->getY() == this->getY()) {
        if (!getWorld()->getPlayer(playerNum)->getState()) {
            if (playerNum == 1 && passPeach) {
                passPeach = false;  //sets pass to false
                passAction(playerNum);
                return;
            }
            if (playerNum == 2 && passYoshi) {
                passYoshi = false;
                passAction(playerNum);
                return;
            }
        }
        else {
            if (playerNum == 1 && newPeach) {
                newPeach = false;
                passPeach = false;
                squareAct(playerNum);
                return;
            }
            if (playerNum == 2 && newYoshi) {
                newYoshi = false;
                passYoshi = false;
                squareAct(playerNum);
                return;
            }
        }
    }
    else {
        if (playerNum == 1) {
            passPeach = true;
            newPeach = true;
            newSwap = true;
        }
        if (playerNum == 2){
            passYoshi = true;
            newYoshi = true;
            newSwap = true;
        }
    }
}

void Square::doSomething() {
    hasLandedOrPassed(1);
    hasLandedOrPassed(2);
}
    
void BlueCoinSquare::squareAct(int playerNum) {
    getWorld()->getPlayer(playerNum)->addCoins(3);
    getWorld()->playSound(SOUND_GIVE_COIN);
}

void RedCoinSquare::squareAct(int playerNum) {
    getWorld()->getPlayer(playerNum)->addCoins(-3);
    getWorld()->playSound(SOUND_TAKE_COIN);
}

void StarSquare::squareAct(int playerNum) {
    if (getWorld()->getPlayer(playerNum)->getCoins() < 20)
        return;
    else {
        getWorld()->getPlayer(playerNum)->addCoins(-20);
        getWorld()->getPlayer(playerNum)->addStar(1);
        getWorld()->playSound(SOUND_GIVE_STAR);
    }
}

void StarSquare::passAction(int playerNum) {
    squareAct(playerNum);
}

void DirectionalSquare::squareAct(int playerNum) {
    getWorld()->getPlayer(playerNum)->setMoveDirection(forcingDirection);
    getWorld()->getPlayer(playerNum)->setForce(forcingDirection);
}

void DirectionalSquare::passAction(int playerNum) {
    squareAct(playerNum);
}

void BankSquare::squareAct(int playerNum) {
    getWorld()->getPlayer(playerNum)->addCoins(getWorld()->getBankBalance());
    getWorld()->clearBankBalance();
    getWorld()->playSound(SOUND_WITHDRAW_BANK);
}
    
void BankSquare::passAction(int playerNum) {
    int playerCoins = getWorld()->getPlayer(playerNum)->getCoins();
    if (playerCoins < 5) {
        getWorld()->getPlayer(playerNum)->addCoins(-playerCoins);
        getWorld()->setBankBalance(playerCoins);
    }
    else {
        getWorld()->getPlayer(playerNum)->addCoins(-5);
        getWorld()->setBankBalance(5);
    }
    getWorld()->playSound(SOUND_DEPOSIT_BANK);
}

void EventSquare::squareAct(int playerNum) {
    if (getNewSwap()){
        int num = randInt(1, 3);
        if (num == 1) {
            getWorld()->getPlayer(playerNum)->teleport();
            getWorld()->playSound(SOUND_PLAYER_TELEPORT);
        } else if (num == 2) {
            swapPlayers();
            setNewSwap(false);
            getWorld()->playSound(SOUND_PLAYER_TELEPORT);
        } else {  //vortex
            getWorld()->getPlayer(playerNum)->giveVortex();
            getWorld()->playSound(SOUND_GIVE_VORTEX);
        }
    }
}

void EventSquare::swapPlayers() {
    int tempX = getWorld()->getPlayer(1)->getX();
    int tempY = getWorld()->getPlayer(1)->getY();
    getWorld()->getPlayer(1)->moveTo(getWorld()->getPlayer(2)->getX(), getWorld()->getPlayer(2)->getY());
    getWorld()->getPlayer(2)->moveTo(tempX, tempY);
    
    int tempPlayer = getWorld()->getPlayer(1)->getTicksToMove();
    getWorld()->getPlayer(1)->setTicksToMove(getWorld()->getPlayer(2)->getTicksToMove());
    getWorld()->getPlayer(2)->setTicksToMove(tempPlayer);
    
    bool tempState = getWorld()->getPlayer(1)->getState();
    getWorld()->getPlayer(1)->setState(getWorld()->getPlayer(2)->getState());
    getWorld()->getPlayer(2)->setState(tempState);
    
    int tempDirection = getWorld()->getPlayer(1)->getMoveDirection();
    getWorld()->getPlayer(1)->setMoveDirection(getWorld()->getPlayer(2)->getMoveDirection());
    getWorld()->getPlayer(2)->setMoveDirection(tempDirection);
    
    getWorld()->getPlayer(1)->spriteDirection();
    getWorld()->getPlayer(2)->spriteDirection();
}

void DroppingSquare::squareAct(int playerNum) {
    int num = randInt(1, 2);
    if (num == 1) {
        int pCoins = getWorld()->getPlayer(playerNum)->getCoins();
        if (pCoins <= 10) {
            getWorld()->getPlayer(playerNum)->setCoins(0);
        }
        else {
            getWorld()->getPlayer(playerNum)->addCoins(-10);
        }
    } else {
        if (getWorld()->getPlayer(playerNum)->getStars() > 0)
            getWorld()->getPlayer(playerNum)->addStar(-1);
    }
    getWorld()->playSound(SOUND_DROPPING_SQUARE_ACTIVATE);
}

/*---------------------------------------------------------------------------------------------------------------------
  Vortex Implementations
---------------------------------------------------------------------------------------------------------------------*/

void Vortex::doSomething() {
    if (!isAlive())  //if player is alive
        return;
    moveAtAngle(getDirection(), 2);
    
    if (getX() < 0 || getX() >= VIEW_WIDTH || getY() < 0 || getY() >= VIEW_HEIGHT)
        kill();  //sets alive to false
    
    if (getWorld()->overlap(getX(), getY())) {
        kill();
        getWorld()->playSound(SOUND_HIT_BY_VORTEX);
    }
}
