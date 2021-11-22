#pragma once
#include "Context.h"
class Context;
class State
{
public:
	State(){}
	virtual ~State() {}
	void setContext(Context* ct) noexcept;
	virtual void enter();
	virtual void level();
protected:
	Context* _context;
};

class StandBy : public State {
protected:
	~StandBy() {}
public:
	StandBy() {}
	void enter();
	void level();
};

class Move : public State {
protected:
	~Move() {}
public:
	Move(){}
	void enter();
	void level();
};

class Attack : public State {
protected:
	~Attack() {}
public:
	Attack(){}
	void enter();
	void level();
};

class Hit : public State {
protected:
	~Hit() {}
public:
	Hit(){}
	void enter();
	void level();
};

class Death : public State {
protected:
	~Death() {}
public:
	Death(){}
	void enter();
	void level();
};

