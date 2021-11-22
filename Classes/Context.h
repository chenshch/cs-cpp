#pragma once
#include "State.h"
#include "CPlayer.h"
#include "Constant.h"

class CPlayer;
class State;
class Context
{
public:
	Context(CPlayer* p);
	~Context();
	PlayerState getState() const;
	void setState(PlayerState s);
	CPlayer* getPlayer() const { return player; }
private:
	void initContext();
	inline void _setState(State* s);
	PlayerState eState;
	State* mCurrentSate;
	CPlayer* player;
	std::map<PlayerState, State*>mContext;
};

