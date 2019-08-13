#include "Actor.h"
#include "StudentWorld.h"
using namespace std;
Actor::Actor(int imageID, double startX, double startY,
	int startDirection, int depth, StudentWorld* d)
	:GraphObject(imageID, startX, startY,
		startDirection, depth)
{
	s = d;
}
StudentWorld* Actor::getSW() {
	return s;
}


other::other(int imageID, double startX, double startY,
	int startDirection, int depth, StudentWorld* s)
	:Actor(imageID, startX, startY,
		startDirection, depth,s)
{
	alive = true;
}


Landmine::Landmine(int imageID, double startX, double startY,
	int startDirection, int depth, StudentWorld* s)
	:other(imageID, startX, startY,
		startDirection, depth,s)
{
	safetick = 30;
	active = false;
}
void Landmine::doSomething() {
	if (!isalive()) {
		return;
	}
	if (!active) {
		safetick--;
		if (safetick == 0) {
			active = true;
			return;
		}
	}
	else {
		getSW()->explode(this);
	}
}


Penelope::Penelope(int imageID, double startX, double startY,
	int startDirection, int depth, StudentWorld* s)
	:other(imageID, startX, startY,
		startDirection, depth,s)
{
	Flame = 0;
	Mine = 0;
	Vac = 0;
	infect = 0;
	infected = false;
}
void Penelope::doSomething() {
	if (!isalive()) {
		return;
	}
	if (getinfected()) {
		infect++;
		if (infect == 500) {
			setalive(false);
			getSW()->playSound(SOUND_PLAYER_DIE);
			return;
		}
	}
	int key;
	double destx = getX();
	double desty = getY();
	if (getSW()->getKey(key)) {
		switch (key) {
		case KEY_PRESS_SPACE:
			if (Flame > 0) {
				Flame--;
				getSW()->playSound(SOUND_PLAYER_FIRE);
				getSW()->fire();
			}
			break;
		case KEY_PRESS_TAB:
			if (Mine > 0) {
				Mine--;
				getSW()->setlandmine();
			}
			break;
		case KEY_PRESS_ENTER:
			if (Vac > 0) {
				if (infected == true) {
					infect = 0;
					infected = false;
				}
				Vac--;
			}
			break;
		case KEY_PRESS_DOWN:
			setDirection(270);
			desty -= 4;
			if (!getSW()->intesect(this,destx, desty)) {
				moveTo(destx, desty);
			}
			break;
		case KEY_PRESS_LEFT:
			setDirection(180);
			destx -=4;
			if (!getSW()->intesect(this, destx, desty)) {
				moveTo(destx, desty);
			}
			break;
		case KEY_PRESS_RIGHT:
			setDirection(0);
			destx +=4;
			if (!getSW()->intesect(this, destx, desty)) {
				moveTo(destx, desty);
			}
			break;
		case KEY_PRESS_UP:
			setDirection(90);
			desty += 4;
			if (!getSW()->intesect(this, destx, desty)) {
				moveTo(destx, desty);
			}
			break;
		}
	}
}
Citizen::Citizen(int imageID, double startX, double startY,
	int startDirection, int depth, StudentWorld* s) 
	: other(imageID, startX, startY,
		startDirection, depth, s)
{
	infected = false;
	infect = 0;
	tick = 0;
}
void Citizen::doSomething() {
	if (!isalive()) {
		return;
	}
	if (infected) {
		infect++;
	}
	if (infect == 500) {
		setalive(false);
		getSW()->zombieborn(this);
		return;
	}
	tick++;
	if (tick % 2 == 0) {
		return;
	}
	getSW()->decide(this);
}

Zombie::Zombie(int imageID, double startX, double startY,
	int startDirection, int depth, StudentWorld* s) 
	:other(imageID, startX, startY,
		startDirection, depth, s)
{
	tick = 1;
	movement = 0;
}
bool Zombie::doSome() {
	tick++;
	if (tick % 2 == 0) {
		return false;
	}
	switch (this->getDirection()) {
	case 0:
		getSW()->setvomit(getX() + SPRITE_WIDTH, getY(), this);
		break;
	case 90:
		getSW()->setvomit(getX(), getY() + SPRITE_HEIGHT, this);
		break;
	case 180:
		getSW()->setvomit(getX() - SPRITE_WIDTH, getY(), this);
		break;
	case 270:
		getSW()->setvomit(getX(), getY() - SPRITE_HEIGHT, this);
		break;
	default:
		break;
	}
	return true;
}
DumZ::DumZ(int imageID, double startX, double startY,
	int startDirection, int depth, StudentWorld* s) 
	:Zombie(imageID, startX, startY,
		startDirection, depth, s)
{

}
void DumZ::doSomething() {
	if (!doSome()) {
		return;
	}
	else {
		if (getmovement() == 0) {
			int nm = randInt(3, 10);
			setmovement(nm);
			int nd = randInt(1, 4);
			switch (nd) {
			case 1:
				setDirection(0);
				break;
			case 2:
				setDirection(90);
				break;
			case 3:
				setDirection(180);
				break;
			case 4:
				setDirection(270);
				break;
			}
		}
		if (getSW()->setmove(this)) {
			setmovement(getmovement()-1);
		}
		else {
			setmovement(0);
		}
	}
}
SmartZ::SmartZ(int imageID, double startX, double startY,
	int startDirection, int depth, StudentWorld* s) 
	:Zombie(imageID, startX, startY,
		startDirection, depth, s)
{

}
void SmartZ::doSomething() {
	if (!doSome()) {
		return;
	}
	else {
		if (getmovement() == 0) {
			int nm = randInt(3, 10);
			setmovement(nm);
			getSW()->choosemove(this);
		}
		if (getSW()->setmove(this)) {
			setmovement(getmovement() - 1);
		}
		else {
			setmovement(0);
		}
	}
}


projectile::projectile(int imageID, double startX, double startY,
	int startDirection, int depth, StudentWorld* s):
	other(imageID, startX, startY,
		startDirection, depth,s)
{
	tick = 3;
}
bool projectile::dosome() {
	if (!isalive()) {
		return false;
	}
	tick--;
	if (tick == 0) {
		setalive(false);
		return false;
	}
	return true;
}
Flames::Flames(int imageID, double startX, double startY,
	int startDirection, int depth, StudentWorld* s)
	:projectile(imageID, startX, startY,
		startDirection, depth,s)
{
	
}
void Flames::doSomething() {
	if (!dosome()) {
		return;
	}
	else {
		getSW()->fff(this);
	}
	
}
Vomit::Vomit(int imageID, double startX, double startY,
	int startDirection, int depth, StudentWorld* s)
	:projectile(imageID, startX, startY,
		startDirection, depth,s)
{
	getSW()->playSound(SOUND_ZOMBIE_VOMIT);
}
void Vomit::doSomething() {
	if (!dosome()) {
		return;
	}
	else {
		getSW()->iiinfect(this);
	}	
}


WaExPi::WaExPi(int imageID, double startX, double startY,
	int startDirection, int depth, StudentWorld* s)
	:Actor(imageID, startX, startY,
		startDirection, depth,s)
{

}
Pit::Pit(int imageID, double startX, double startY,
	int startDirection, int depth, StudentWorld* s)
	:WaExPi(imageID, startX, startY,
		startDirection, depth,s)
{

}
void Pit::doSomething() {
	getSW()->fall(this);
}
Wall::Wall(int imageID, double startX, double startY,
	int startDirection, int depth, StudentWorld* s)
	:WaExPi(imageID, startX, startY,
		startDirection, depth,s)
{

}
void Wall::doSomething() {
	return;
}

Exit::Exit(int imageID, double startX, double startY,
	int startDirection, int depth, StudentWorld* s)
	:WaExPi(imageID, startX, startY,
		startDirection, depth,s)
{

}
void Exit::doSomething() {
	getSW()->exitt(getX(), getY());
}


Goodie::Goodie(int imageID, double startX, double startY,
	int startDirection, int depth, StudentWorld* s)
	:other(imageID, startX, startY,
		startDirection, depth,s)
{

}
Vaccine::Vaccine(int imageID, double startX, double startY,
	int startDirection, int depth, StudentWorld* s)
	:Goodie(imageID, startX, startY,
		startDirection, depth,s)
{

}
void Vaccine::doSomething() {
	if (!isalive()) {
		return;
	}
	getSW()->getvac(this);
}
Gas::Gas(int imageID, double startX, double startY,
	int startDirection, int depth, StudentWorld* s)
	:Goodie(imageID, startX, startY,
		startDirection, depth,s)
{

}
void Gas::doSomething() {
	if (!isalive()) {
		return;
	}
	getSW()->getgas(this);
}
LandmineGoodie::LandmineGoodie(int imageID, double startX, double startY,
	int startDirection, int depth, StudentWorld* s)
	:Goodie(imageID, startX, startY,
		startDirection, depth,s)
{

}
void LandmineGoodie::doSomething() {
	if (!isalive()) {
		return;
	}
	getSW()->getmine(this);
}