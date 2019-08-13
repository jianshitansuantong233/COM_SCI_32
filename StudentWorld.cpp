#include "StudentWorld.h"
#include"Level.h"
#include "GameConstants.h"
#include<iostream>
#include<sstream>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{
	advance = false;
}

int StudentWorld::init()
{
	advance = false;
	p = nullptr;
	Level lev(assetPath());
	ostringstream l;
	l << "level0" << getLevel()<<".txt";
	string levelFile = l.str();
	Level::LoadResult result = lev.loadLevel(levelFile);
	if (result == Level::load_fail_file_not_found)
		return GWSTATUS_PLAYER_WON;
	else if (result == Level::load_fail_bad_format)
		return GWSTATUS_LEVEL_ERROR;
	else if (result == Level::load_success)
	{
		cerr << "Successfully loaded level" << endl;
		Level::MazeEntry ge;
		for (int i = 0; i < 16; i++) {
			for (int j = 0; j < 16; j++) {
				ge = lev.getContentsOf(i, j); // x=5, y=10
				switch (ge)
				{
				case Level::smart_zombie:
					ac.push_front(new SmartZ(IID_ZOMBIE, SPRITE_WIDTH * i, SPRITE_HEIGHT * j, 0, 0,this));
					break;
				case Level::dumb_zombie:
					ac.push_front(new DumZ(IID_ZOMBIE, SPRITE_WIDTH * i, SPRITE_HEIGHT * j, 0, 0,this));
					break;
				case Level::player:
					p = new Penelope(IID_PLAYER, SPRITE_WIDTH * i, SPRITE_HEIGHT * j, 0, 0,this);
					break;
				case Level::exit:
					ac.push_front(new Exit(IID_EXIT, SPRITE_WIDTH * i, SPRITE_HEIGHT * j,0,1,this));
					break;
				case Level::wall:
					ac.push_front(new Wall(IID_WALL, SPRITE_WIDTH * i, SPRITE_HEIGHT * j,0,0,this));
					break;
				case Level::pit:
					ac.push_front(new Pit(IID_PIT, SPRITE_WIDTH * i, SPRITE_HEIGHT * j,0,0,this));
					break;
				case Level::citizen:
					ac.push_front(new Citizen(IID_CITIZEN, SPRITE_WIDTH * i, SPRITE_HEIGHT * j, 0, 0, this));
					break;
				case Level::gas_can_goodie:
					ac.push_front(new Gas(IID_GAS_CAN_GOODIE, SPRITE_WIDTH * i, SPRITE_HEIGHT *j, 0, 1,this));
					break;
				case Level::vaccine_goodie:
					ac.push_front(new Vaccine(IID_VACCINE_GOODIE,SPRITE_WIDTH * i, SPRITE_HEIGHT *j,0,1,this));
					break;
				case Level::landmine_goodie:
					ac.push_front(new LandmineGoodie(IID_LANDMINE_GOODIE,SPRITE_WIDTH * i, SPRITE_HEIGHT *j, 0, 1,this));
					break;
				default:
					break;
				}
			}
		}
		
	}
	return  GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
	list<Actor*>::iterator it;
	it = ac.begin();
	p->doSomething();
	if (!p->isalive()) {
		playSound(SOUND_PLAYER_DIE);
		decLives();
		return GWSTATUS_PLAYER_DIED;
	}
	while (it != ac.end()) {
		if ((*it)->canlive()&&!(*it)->isalive()) {
			it++;
			continue;
		}
		(*it)->doSomething();
		if (!p->isalive()) {
			playSound(SOUND_PLAYER_DIE);
			decLives();
			return GWSTATUS_PLAYER_DIED;
		}
		if (advance) {
			playSound(SOUND_LEVEL_FINISHED);
			return GWSTATUS_FINISHED_LEVEL;
		}
		it++;
	}
	it = ac.begin();
	while (it != ac.end()) {
		if ((*it)->canlive()&&!(*it)->isalive()) {
			delete *it;
			it = ac.erase(it);
		}
		else {
			it++;
		}
	}
	ostringstream stat;
	stat << "Score: " << to_string(getScore()) << "  ";
	stat << "Level: " << to_string(getLevel()) << "  ";
	stat << "Lives: " << to_string(getLives()) << "  ";
	stat << "Vacc: " << to_string(p->getVac()) << "  ";
	stat << "Flames: " << to_string(p->getFlame()) << "  ";
	stat << "Mines: " << to_string(p->getMine()) << "  ";
	stat << "Infected: " << to_string(p->getinfect());
	setGameStatText(stat.str());
	return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
	list<Actor*>::iterator it;
	it = ac.begin();
	delete p;
	while (it != ac.end()) {
		delete *it;
		it=ac.erase(it);
	}
}
StudentWorld::~StudentWorld() {
	if (getLives() > 0||p==nullptr) {
		cleanUp();
	}
}
bool StudentWorld::overlap(double x,double y,Actor* a) {
	if (getdistance(x,y,a->getX(),a->getY())<=10) {
		if (a->canlive()) {
			if (a->isalive()) {
				return true;
			}else {
				return false;
			}	
		}
		else {
			return true;
		}
	}
	return false;
}
bool StudentWorld::tfire(double x, double y) {
	list<Actor*>::iterator it;
	it = ac.begin();
	while (it != ac.end()) {
		if (!(*it)->canlive()&&(*it)->blockF()&&overlap(x, y, *it)) {
			return false;
		}
		it++;
	}
	ac.push_front(new Flames(IID_FLAME, x, y, p->getDirection(), 0, this));
	return true;
}
void StudentWorld::fire() {
	for (int i = 1; i < 4; i++) {
		if (p->getDirection() == 90) {
			double destx = p->getX();
			double desty = p->getY()+i*SPRITE_HEIGHT;
			if (!tfire(destx, desty)) {
				return;
			}
		}
		else if (p->getDirection() == 270) {
			double destx = p->getX();
			double desty = p->getY() - i * SPRITE_HEIGHT;
			if (!tfire(destx, desty)) {
				return;
			}
		}
		else if (p->getDirection() == 180) {
			double destx = p->getX() - i * SPRITE_HEIGHT;
			double desty = p->getY();
			if (!tfire(destx, desty)) {
				return;
			}
		}
		else if (p->getDirection() == 0) {
			double destx = p->getX() + i * SPRITE_HEIGHT;
			double desty = p->getY();
			if (!tfire(destx, desty)) {
				return;
			}
		}
	}
}
void StudentWorld::setlandmine() {
	ac.push_back(new Landmine(IID_LANDMINE, p->getX(), p->getY(), 0, 1, this));
}
bool StudentWorld::intesect(Actor* a, double x, double y) {
	list<Actor*>::iterator it;
	it = ac.begin();
	ac.push_back(p);
	while (it != ac.end()) {
		if (a==(*it)||((*it)->canlive()&&!(*it)->isalive())) {
			it++;
			continue;
		}
		if ((*it)->blockM()) {
			if (abs(x-(*it)->getX())<16&&abs(y-(*it)->getY()) < 16) {
				ac.pop_back();
				return true;
			}
		}
		it++;
	}
	ac.pop_back();
	return false;
}
void StudentWorld::exitt(double x,double y) {
	list<Actor*>::iterator it;
	it = ac.begin();
	while (it != ac.end()){
		if ((*it)->canexit()&&overlap(x, y, *it)) {
			increaseScore(500);
			playSound(SOUND_CITIZEN_SAVED);
			(*it)->setalive(false);
		}
		it++;
	}
	if (overlap(x, y, p)) {
		list<Actor*>::iterator it;
		it = ac.begin();
		while (it != ac.end()) {
			if ((*it)->canexit()) {
				advance= false;
				return;
			}
			it++;
		}
		advance= true;
	}
}
void StudentWorld::fall(Actor* a) {
	list<Actor*>::iterator it;
	it = ac.begin();
	if (overlap(p->getX(), p->getY(), a)) {
		p->setalive(false);
	}
	while (it != ac.end()) {
		if ((*it)->canlive() && (*it)->blockM()) {
			if (overlap((*it)->getX(), (*it)->getY(), a)) {
				(*it)->setalive(false);
				if ((*it)->canexit()) {
					increaseScore(-1000);
					playSound(SOUND_CITIZEN_DIE);
				}else {
					playSound(SOUND_ZOMBIE_DIE);
					if ((*it)->isSmart()) {
						increaseScore(2000);
					}
					else {
						int temp = randInt(1, 10);
						if (temp >= 1) {
							int te = randInt(1, 4);
							bool re = true;
							list<Actor*>::iterator i = ac.begin();
							switch (te) {
							case 1:
								while (i != ac.end()) {
									if (overlap((*it)->getX() + SPRITE_WIDTH, (*it)->getY(), (*i))) {
										re = false;
										break;
									}
									i++;
								}
								if (re)
									ac.push_back(new Vaccine(IID_VACCINE_GOODIE, (*it)->getX() + SPRITE_WIDTH, (*it)->getY(), 0, 1, this));
								break;
							case 2:
								while (i != ac.end()) {
									if (overlap((*it)->getX() - SPRITE_WIDTH, (*it)->getY(), (*i))) {
										re = false;
										break;
									}
									i++;
								}
								if (re)
									ac.push_back(new Vaccine(IID_VACCINE_GOODIE, (*it)->getX() - SPRITE_WIDTH, (*it)->getY(), 0, 1, this));
								break;
							case 3:
								while (i != ac.end()) {
									if (overlap((*it)->getX(), (*it)->getY() + SPRITE_HEIGHT, (*i))) {
										re = false;
										break;
									}
									i++;
								}
								if (re)
									ac.push_back(new Vaccine(IID_VACCINE_GOODIE, (*it)->getX(), (*it)->getY() + SPRITE_HEIGHT, 0, 1, this));
								break;
							case 4:
								while (i != ac.end()) {
									if (overlap((*it)->getX(), (*it)->getY() - SPRITE_HEIGHT, (*i))) {
										re = false;
										break;
									}
									i++;
								}
								if (re)
									ac.push_back(new Vaccine(IID_VACCINE_GOODIE, (*it)->getX(), (*it)->getY() - SPRITE_HEIGHT, 0, 1, this));
								break;
							}
						}
						increaseScore(1000);
					}
				}
			}
		}
		it++;
	}
}
void StudentWorld::fff(Actor* a) {
	list<Actor*>::iterator it;
	it = ac.begin();
	if (overlap(p->getX(), p->getY(), a)) {
		p->setalive(false);
	}
	while (it != ac.end()) {
		if ((*it)->canlive()&&overlap(a->getX(),a->getY(),*it)) {
			(*it)->setalive(false);
			if ((*it)->canexit()) {
				increaseScore(-1000);
				playSound(SOUND_CITIZEN_DIE);
			}else if ((*it)->blockM()) {
				if ((*it)->isSmart()) {
					increaseScore(2000);
				}
				else {
					increaseScore(1000);
					int temp=randInt(1, 10);
					if (temp == 5) {
						int te = randInt(1, 4);
						bool re = true;
						list<Actor*>::iterator i = ac.begin();
						switch (te) {
							case 1:
								while (i != ac.end()) {
									if (overlap((*it)->getX() + SPRITE_WIDTH, (*it)->getY(), (*i))) {
										re = false;
										break;
									}
									i++;
								}
								if(re)
									ac.push_back(new Vaccine(IID_VACCINE_GOODIE, (*it)->getX() + SPRITE_WIDTH, (*it)->getY(), 0, 1, this));
								break;
							case 2:
								while (i != ac.end()) {
									if (overlap((*it)->getX() - SPRITE_WIDTH, (*it)->getY(), (*i))) {
										re = false;
										break;
									}
									i++;
								}
								if(re)
									ac.push_back(new Vaccine(IID_VACCINE_GOODIE, (*it)->getX() - SPRITE_WIDTH, (*it)->getY(), 180, 1, this));
								break;
							case 3:
								while (i != ac.end()) {
									if (overlap((*it)->getX(), (*it)->getY() + SPRITE_HEIGHT, (*i))) {
										re = false;
										break;
									}
									i++;
								}
								if(re)
									ac.push_back(new Vaccine(IID_VACCINE_GOODIE, (*it)->getX(), (*it)->getY()+SPRITE_HEIGHT, 90, 1, this));
								break;
							case 4:
								while (i != ac.end()) {
									if (overlap((*it)->getX(), (*it)->getY() - SPRITE_HEIGHT, (*i))) {
										re = false;
										break;
									}
									i++;
								}
								if(re)
									ac.push_back(new Vaccine(IID_VACCINE_GOODIE, (*it)->getX(), (*it)->getY() - SPRITE_HEIGHT, 270, 1, this));
								break;
						}
					}
				}
				playSound(SOUND_ZOMBIE_DIE);
			}
			else if ((*it)->isLandMine()) {
				playSound(SOUND_LANDMINE_EXPLODE);
				eexplode(*it);
			}
		}
		it++;
	}
}
void StudentWorld::eexplode(Actor* a) {
	tfire(a->getX(), a->getY());
	ac.front()->setDirection(90);
	tfire(a->getX() + SPRITE_WIDTH, a->getY());
	ac.front()->setDirection(90);
	tfire(a->getX() - SPRITE_WIDTH, a->getY());
	ac.front()->setDirection(90);
	tfire(a->getX(), a->getY() - SPRITE_HEIGHT);
	ac.front()->setDirection(90);
	tfire(a->getX(), a->getY() + SPRITE_HEIGHT);
	ac.front()->setDirection(90);
	tfire(a->getX() - SPRITE_WIDTH, a->getY() - SPRITE_HEIGHT);
	ac.front()->setDirection(90);
	tfire(a->getX() + SPRITE_WIDTH, a->getY() + SPRITE_HEIGHT);
	ac.front()->setDirection(90);
	tfire(a->getX() - SPRITE_WIDTH, a->getY() + SPRITE_HEIGHT);
	ac.front()->setDirection(90);
	tfire(a->getX() + SPRITE_WIDTH, a->getY() - SPRITE_HEIGHT);
	ac.front()->setDirection(90);
	ac.push_back(new Pit(IID_PIT, a->getX(), a->getY(), 0, 0,this));
}
void StudentWorld::explode(Actor* a) {
	list<Actor*>::iterator it;
	it = ac.begin();
	bool temp = false;
	if (overlap(p->getX(), p->getY(), a)) {
		temp = true;
	}
	else {
		while (it != ac.end()) {
			if ((*it)->canexit() && overlap((*it)->getX(), (*it)->getY(), a)) {
				temp = true;
			}
			it++;
		}
	}
	if (temp) {
		playSound(SOUND_LANDMINE_EXPLODE);
		a->setalive(false);
		eexplode(a);
	}
}
void StudentWorld::iiinfect(Actor* a) {
	list<Actor*>::iterator it;
	it = ac.begin();
	if (overlap(p->getX(), p->getY(), a)) {
		p->setinfected(true);
	}
	while (it != ac.end()) {
		if ((*it)->canexit() && overlap((*it)->getX(), (*it)->getY(), a)) {
			if (!(*it)->setinfected(true)) {
				it++;
				continue;
			}
			else {
				playSound(SOUND_CITIZEN_INFECTED);
			}
			
		}
		it++;
	}
}
void StudentWorld::getvac(Actor* a) {
	if (overlap(p->getX(), p->getY(), a)) {
		increaseScore(50);
		playSound(SOUND_GOT_GOODIE);
		a->setalive(false);
		p->setVac();
	}
}
void StudentWorld::getgas(Actor* a) {
	if (overlap(p->getX(), p->getY(), a)) {
		increaseScore(50);
		playSound(SOUND_GOT_GOODIE);
		a->setalive(false);
		p->setFlame();
	}
}
void StudentWorld::getmine(Actor* a) {
	if (overlap(p->getX(), p->getY(), a)) {
		increaseScore(50);
		playSound(SOUND_GOT_GOODIE);
		a->setalive(false);
		p->setMine();
	}
}
void StudentWorld::setvomit(double x, double y, Actor* a) {
	if (overlap(x, y, p)) {
		int pr = randInt(1,3);
		if (pr == 1) {
			ac.push_back(new Vomit(IID_VOMIT, x, y, a->getDirection(), 0, this));
			return;
		}
	}
	else {
		list<Actor*>::iterator it;
		it = ac.begin();
		while (it != ac.end()) {
			if ((*it)->canexit() && overlap(x, y, (*it))) {
				int pr = randInt(1, 3);
				if (pr == 1) {
					ac.push_back(new Vomit(IID_VOMIT, x, y, a->getDirection(), 0, this));
					return;
				}
			}
			it++;
		}
	}
}
bool StudentWorld::setmove(Actor* a) {
	list<Actor*>::iterator it;
	it = ac.begin();
	switch (a->getDirection()) {
	case 0:
		if (intesect(a, a->getX() + 1, a->getY())) {
			return false;
		}
		a->moveTo(a->getX() + 1, a->getY());
		break;
	case 90:
		if (intesect(a, a->getX(), a->getY()+1)) {
			return false;
		}
		a->moveTo(a->getX(), a->getY()+1);
		break;
	case 180:
		if (intesect(a, a->getX() - 1, a->getY())) {
			return false;
		}
		a->moveTo(a->getX() - 1, a->getY());
		break;
	case 270:
		if (intesect(a, a->getX(), a->getY() - 1)) {
			return false;
		}
		a->moveTo(a->getX(), a->getY() - 1);
		break;
	}
	return true;
}
double StudentWorld::getdistance(double x, double y,double x1,double y1) {
	double tempx = x - x1;
	double tempy = y - y1;
	return sqrt(tempx*tempx + tempy * tempy);
}
void StudentWorld::choosemove(Actor* a) {
	list<Actor*>::iterator it;
	it = ac.begin();
	list<Actor*>::iterator t;
	double p_dis = getdistance(p->getX(), p->getY(), a->getX(),a->getY());
	double dis = p_dis;
	while (it != ac.end()) {
		if ((*it)->canexit()) {
			double temp = getdistance((*it)->getX(),(*it)->getY(),a->getX(),a->getY());
			if (dis > temp) {
				t = it;
				dis = temp;
			}
		}
		it++;
	}
	
	if (dis == p_dis) {
		ac.push_front(p);
		t = ac.begin();
	}
	if (dis > 80) {
		int nd = randInt(1, 4);
		switch (nd) {
		case 1:
			a->setDirection(0);
			break;
		case 2:
			a->setDirection(90);
			break;
		case 3:
			a->setDirection(180);
			break;
		case 4:
			a->setDirection(270);
			break;
		}
	}
	else {
		if (a->getX() == (*t)->getX()) {
			if (a->getY() - (*t)->getY() > 0) {
				a->setDirection(270);
			}else {
				a->setDirection(90);
			}
		}
		else if (a->getY() == (*t)->getY()) {
			if (a->getX() - (*t)->getX() > 0) {
				a->setDirection(180);
			}else {
				a->setDirection(0);
			}
		}
		else {
			int pr = randInt(1, 2);
			if (a->getY() - (*t)->getY() > 0) {
				if (a->getX() - (*t)->getX() > 0) {
					if (pr == 1) {
						a->setDirection(270);
					}else {
						a->setDirection(180);
					}
				}else {
					if (pr == 1) {
						a->setDirection(270);
					}else {
						a->setDirection(0);
					}
				}
			}else {
				if (a->getX() - (*t)->getX() > 0) {
					if (pr == 1) {
						a->setDirection(90);
					}
					else {
						a->setDirection(180);
					}
				}
				else {
					if (pr == 1) {
						a->setDirection(90);
					}
					else {
						a->setDirection(0);
					}
				}
			}
		}
	}
	if (ac.front() == p) {
		ac.pop_front();
	}
}
void StudentWorld::zombieborn(Actor* a) {
	int pr = randInt(1, 10);
	if (pr <= 7) {
		ac.push_back(new DumZ(IID_ZOMBIE, a->getX(), a->getY(), 0, 0, this));
	}
	else {
		ac.push_back(new SmartZ(IID_ZOMBIE, a->getX(), a->getY(), 0, 0, this));
	}
	playSound(SOUND_ZOMBIE_BORN);
	increaseScore(-1000);
}
void StudentWorld::decide(Actor* a) {
	double dist_p = getdistance(a->getX(), a->getY(), p->getX(), p->getY());
	double dist_z = getshort(a->getX(),a->getY());
	if (dist_p < dist_z || dist_z == 0) {
		if (a->getX() == p->getX()) {
			if (a->getY() - p->getY() > 0) {
				if (!intesect(a, a->getX(), a->getY() - 2)) {
					a->setDirection(270);
					a->moveTo(a->getX(), a->getY() - 2);
					return;
				}	
			}
			else {
				if (!intesect(a, a->getX(), a->getY() + 2)) {
					a->setDirection(90);
					a->moveTo(a->getX(), a->getY() + 2);
					return;
				}	
			}
		}
		else if (a->getY() == p->getY()) {
			if (a->getX() - p->getX() > 0) {
				if (!intesect(a, a->getX() - 2, a->getY())) {
					a->setDirection(180);
					a->moveTo(a->getX() - 2, a->getY());
					return;
				}
				
			}
			else {
				if (!intesect(a, a->getX() + 2, a->getY())) {
					a->setDirection(0);
					a->moveTo(a->getX() + 2, a->getY());
					return;
				}
				
			}
		}
		else {
			int pr = randInt(1, 2);
			if (a->getY() - p->getY() > 0) {
				if (a->getX() - p->getX() > 0) {
					if (pr == 1) {
						if (!intesect(a, a->getX(), a->getY() - 2)) {
							a->setDirection(270);
							a->moveTo(a->getX(), a->getY() - 2);
							return;
						}else {
							if (!intesect(a, a->getX() - 2, a->getY())) {
								a->setDirection(180);
								a->moveTo(a->getX() - 2, a->getY());
								return;
							}
							
						}
					}
					else {
						if (!intesect(a, a->getX() - 2, a->getY())) {
							a->setDirection(180);
							a->moveTo(a->getX() - 2, a->getY());
							return;
						}else {
							if (!intesect(a, a->getX(), a->getY()-2)) {
								a->setDirection(270);
								a->moveTo(a->getX(), a->getY()-2);
								return;
							}
							
						}
					}
				}
				else {
					if (pr == 1) {
						if (!intesect(a, a->getX(), a->getY() - 2)) {
							a->setDirection(270);
							a->moveTo(a->getX(), a->getY() - 2);
							return;
						}else {
							if (!intesect(a, a->getX() + 2, a->getY())) {
								a->setDirection(0);
								a->moveTo(a->getX() + 2, a->getY());
								return;
							}
							
						}
					}
					else {
						if (!intesect(a, a->getX() + 2, a->getY())) {
							a->setDirection(0);
							a->moveTo(a->getX() + 2, a->getY());
							return;
						}else {
							if (!intesect(a, a->getX(), a->getY()-2)) {
								a->setDirection(270);
								a->moveTo(a->getX(), a->getY()-2);
								return;
							}
							
						}
					}
				}
			}
			else {
				if (a->getX() - p->getX() > 0) {
					if (pr == 1) {
						if (!intesect(a, a->getX(), a->getY() + 2)) {
							a->setDirection(90);
							a->moveTo(a->getX(), a->getY() + 2);
							return;
						}else {
							if (!intesect(a, a->getX() - 2, a->getY())) {
								a->setDirection(180);
								a->moveTo(a->getX() - 2, a->getY());
								return;
							}
							
						}
					}
					else {
						if (!intesect(a, a->getX() - 2, a->getY())) {
							a->setDirection(180);
							a->moveTo(a->getX() - 2, a->getY());
							return;
						}else {
							if (!intesect(a, a->getX(), a->getY()+2)) {
								a->setDirection(90);
								a->moveTo(a->getX(), a->getY()+2);
								return;
							}
						}
					}
				}
				else {
					if (pr == 1) {
						if (!intesect(a, a->getX(), a->getY() + 2)) {
							a->setDirection(90);
							a->moveTo(a->getX(), a->getY() + 2);
							return;
						}else {
							if (!intesect(a, a->getX() + 2, a->getY())) {
								a->setDirection(0);
								a->moveTo(a->getX() + 2, a->getY());
								return;
							}
						}
					}
					else {
						if (!intesect(a, a->getX() + 2, a->getY())) {
							a->setDirection(0);
							a->moveTo(a->getX() + 2, a->getY());
							return;
						}else {
							if (!intesect(a, a->getX(), a->getY()+2)) {
								a->setDirection(90);
								a->moveTo(a->getX(), a->getY()+2);
								return;
							}
						}
					}
				}
			}
		}
	}
	if (dist_z <= 80 && dist_z!=0) {
		if (!intesect(a, a->getX() + 2, a->getY())&&
			getshort(a->getX() + 2, a->getY())>dist_z){
			a->setDirection(0);
			a->moveTo(a->getX()+2, a->getY());
			return;
		}
		else if (!intesect(a, a->getX(), a->getY() + 2) &&
			getshort(a->getX(), a->getY()+2) > dist_z) {
			a->setDirection(90);
			a->moveTo(a->getX(), a->getY()+2);
			return;
		}
		else if (!intesect(a, a->getX() - 2, a->getY()) &&
			getshort(a->getX()-2, a->getY()) > dist_z) {
			a->setDirection(180);
			a->moveTo(a->getX() - 2, a->getY());
			return;
		}
		else if (!intesect(a, a->getX(), a->getY() - 2) &&
			getshort(a->getX(), a->getY() - 2) > dist_z) {
			a->setDirection(270);
			a->moveTo(a->getX(), a->getY() - 2);
			return;
		}
	}
}
double StudentWorld::getshort(double x,double y) {
	list<Actor*>::iterator it;
	it = ac.begin();
	double dist_z = 0;
	while (it != ac.end()) {
		if ((*it)->canlive() && !(*it)->canexit() && (*it)->blockM()) {
			if (dist_z == 0) {
				dist_z = getdistance(x, y, (*it)->getX(), (*it)->getY());
			}
			else {
				double te = getdistance(x, y, (*it)->getX(), (*it)->getY());
				dist_z = dist_z < te ? dist_z : te;
			}
		}
		it++;
	}
	return dist_z;
}