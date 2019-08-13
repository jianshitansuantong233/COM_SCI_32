#ifndef ACTOR_H_
#define ACTOR_H_
#include "GraphObject.h"
class StudentWorld;
//base class
class Actor :public GraphObject {
public:
	Actor(int imageID, double startX, double startY,
		int startDirection, int depth, StudentWorld* s);
	virtual bool canlive() = 0;
	virtual void setalive(bool a) = 0;
	virtual bool isalive() = 0;
	virtual void doSomething() = 0;
	virtual bool blockF() = 0;
	virtual bool blockM() = 0;
	virtual bool canexit() = 0;
	virtual bool isexit() = 0;
	virtual bool isLandMine() = 0;
	virtual bool setinfected(bool a) = 0;
	virtual bool isSmart() = 0;
	StudentWorld* getSW();
private:
	StudentWorld* s;
};
//1-sub class
class other :public Actor {
public:
	other(int imageID, double startX, double startY,
		int startDirection, int depth, StudentWorld* s);
	virtual bool setinfected(bool a) = 0;
	virtual bool isalive() {
		return alive;
	}
	virtual void setalive(bool a) {
		alive = a;
	}
	virtual bool canlive() {
		return true;
	}
	virtual void doSomething() = 0;
	virtual bool canexit() = 0;
	virtual bool isLandMine() = 0;
	virtual bool isexit() {
		return false;
	}
	virtual bool blockF() {
		return false;
	}
	virtual bool blockM() = 0;
	virtual bool isSmart() = 0;
private:
	bool alive;
};
class WaExPi :public Actor {
public:
	virtual bool isSmart() {
		return false;
	}
	virtual void setalive(bool a) {
		return;
	}
	virtual bool setinfected(bool a) {
		return false;
	}
	WaExPi(int imageID, double startX, double startY,
		int startDirection, int depth, StudentWorld* s);
	virtual bool canlive() {
		return false;
	}
	virtual bool isalive() {
		return false;
	}
	virtual void doSomething() = 0;
	virtual bool blockF() = 0;
	virtual bool blockM() = 0;
	virtual bool isexit() = 0;
	virtual bool isLandMine() {
		return false;
	}
	virtual bool canexit() {
		return false;
	}
};
//2-sub class
class Goodie :public other {
public:
	virtual bool isSmart() {
		return false;
	}
	Goodie(int imageID, double startX, double startY,
		int startDirection, int depth, StudentWorld* s);
	virtual bool setinfected(bool a) {
		return false;
	}
	virtual bool blockM() {
		return false;
	}
	virtual bool canexit() {
		return false;
	}
	virtual bool isLandMine() {
		return false;
	}
	virtual void doSomething() = 0;
};
class projectile :public other {
public:
	virtual bool isSmart() {
		return false;
	}
	projectile(int imageID, double startX, double startY,
		int startDirection, int depth, StudentWorld* s);
	virtual bool setinfected(bool a) {
		return false;
	}
	virtual bool blockM() {
		return false;
	}
	virtual void doSomething() = 0;
	virtual bool canexit() {
		return false;
	}
	virtual bool isLandMine() {
		return false;
	}
	bool dosome();
private:
	int tick;
};

class Zombie :public other {
public:
	virtual bool isSmart() = 0;
	Zombie(int imageID, double startX, double startY,
		int startDirection, int depth, StudentWorld* s);
	virtual bool setinfected(bool a) {
		return false;
	}
	virtual bool blockM() {
		return true;
	}
	bool doSome();
	virtual bool canexit() {
		return false;
	}
	virtual bool isLandMine() {
		return false;
	}
	int getmovement() {
		return movement;
	}
	void setmovement(int a) {
		movement = a;
	}
	virtual void doSomething() = 0;
private:
	int tick;
	int movement;
};
class Penelope :public other {
public:
	virtual bool isSmart() {
		return false;
	}
	Penelope(int imageID, double startX, double startY,
		int startDirection, int depth, StudentWorld* s);
	virtual void doSomething();
	int getVac() {
		return Vac;
	}
	void setVac() {
		Vac++;
	}
	int getFlame() {
		return Flame;
	}
	void setFlame() {
		Flame += 5;
	}
	int getMine() {
		return Mine;
	}
	void setMine() {
		Mine++;
	}
	int getinfect() {
		return infect;
	}
	bool getinfected() {
		return infected;
	}
	virtual bool blockM() {
		return true;
	}
	virtual bool canexit() {
		return true;
	}
	virtual bool isLandMine() {
		return false;
	}
	virtual bool setinfected(bool a) {
		infected = a;
		return true;
	}
private:
	int Vac;
	int Flame;
	int Mine;
	bool infected;
	int infect;
};
class Citizen :public other {
public:
	virtual bool isSmart() {
		return false;
	}
	virtual bool isLandMine() {
		return false;
	}
	virtual bool blockM() {
		return true;
	}
	virtual bool canexit() {
		return true;
	}
	virtual bool setinfected(bool a) {
		if (infected) {
			return false;
		}
		else {
			infected = a;
		}
		
	}
	virtual int getinfect() {
		return infect;
	}
	virtual void doSomething();
	Citizen(int imageID, double startX, double startY,
		int startDirection, int depth, StudentWorld* s);
private:
	bool infected;
	int infect;
	int tick;
};
class Landmine :public other {
public:
	virtual bool isSmart() {
		return false;
	}
	Landmine(int imageID, double startX, double startY,
		int startDirection, int depth, StudentWorld* s);
	virtual bool setinfected(bool a) {
		return false;
	}
	bool getactive() {
		return active;
	}
	virtual bool blockM() {
		return false;
	}
	virtual void doSomething();
	virtual bool canexit() {
		return false;
	}
	virtual bool isLandMine() {
		return true;
	}
private:
	int safetick;
	bool active;
};
//3-sub class
class DumZ :public Zombie {
public:
	virtual bool isSmart() {
		return false;
	}
	DumZ(int imageID, double startX, double startY,
		int startDirection, int depth, StudentWorld* s);
	virtual void doSomething();
};
class SmartZ :public Zombie {
public:
	SmartZ(int imageID, double startX, double startY,
		int startDirection, int depth, StudentWorld* s);
	virtual bool isSmart() {
		return true;
	}
	virtual void doSomething();
};

class Pit :public WaExPi {
public:
	Pit(int imageID, double startX, double startY,
		int startDirection, int depth, StudentWorld* s);
	virtual bool blockF() {
		return false;
	}
	virtual bool blockM() {
		return false;
	}
	virtual bool isexit() {
		return false;
	}
	virtual void doSomething();
};
class Wall :public WaExPi {
public:
	Wall(int imageID, double startX, double startY,
		int startDirection, int depth, StudentWorld* s);
	virtual bool blockF() {
		return true;
	}
	virtual bool blockM() {
		return true;
	}
	virtual void doSomething();
	virtual bool isexit() {
		return false;
	}
};
class Exit :public WaExPi {
public:
	Exit(int imageID, double startX, double startY,
		int startDirection, int depth, StudentWorld* s);
	virtual bool blockF() {
		return true;
	}
	virtual bool blockM() {
		return false;
	}
	virtual bool isexit() {
		return true;
	}
	virtual void doSomething();
};

class Flames :public projectile {
public:
	Flames(int imageID, double startX, double startY,
		int startDirection, int depth, StudentWorld* s);
	virtual void doSomething();
};
class Vomit :public projectile {
public:
	Vomit(int imageID, double startX, double startY,
		int startDirection, int depth, StudentWorld* s);
	virtual void doSomething();
};
class Vaccine :public Goodie {
public:
	Vaccine(int imageID, double startX, double startY,
		int startDirection, int depth, StudentWorld* s);
	virtual void doSomething();
};

class Gas :public Goodie {
public:
	Gas(int imageID, double startX, double startY,
		int startDirection, int depth, StudentWorld* s);
	virtual void doSomething();
};
class LandmineGoodie :public Goodie {
public:
	LandmineGoodie(int imageID, double startX, double startY,
		int startDirection, int depth, StudentWorld* s);
	virtual void doSomething();
};
#endif
