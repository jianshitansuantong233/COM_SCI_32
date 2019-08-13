#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include"Actor.h"
#include <string>
#include<list>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
	~StudentWorld();
    virtual int init();
    virtual int move();
    virtual void cleanUp();
	bool tfire(double x,double y);
	void fire();
	void setlandmine();
	bool overlap(double x,double y,Actor* a);
	bool intesect(Actor* a, double x,double y);
	void exitt(double x,double y);
	void fall(Actor* a);
	void fff(Actor* a);
	void explode(Actor* a);
	void eexplode(Actor* a);
	void iiinfect(Actor* a);
	void getvac(Actor* a);
	void getgas(Actor* a);
	void getmine(Actor* a);
	void setvomit(double x, double y,Actor* a);
	bool setmove(Actor* a);
	void choosemove(Actor* a);
	void zombieborn(Actor* a);
	double getdistance(double x, double y,double x1,double y1);
	void decide(Actor* a);
	double getshort(double x,double y);
private:
	std::list<Actor*> ac;
	Penelope* p;
	bool advance;
};

#endif // STUDENTWORLD_H_
