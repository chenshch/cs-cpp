#include "Context.h"

Context::Context(CPlayer* p):mCurrentSate(nullptr)
,eState (PlayerState::eWait){
	player = p;
	initContext();
}

Context::~Context() {
	for (auto it = mContext.begin(); it != mContext.end(); ++it)
	{
		auto state = it->second;
		delete state;
	}
	mContext.clear();
}

void Context::initContext() {
	mContext.insert(std::make_pair(PlayerState::eAtk, new Attack()));
	mContext.insert(std::make_pair(PlayerState::eDeath, new Death()));
	mContext.insert(std::make_pair(PlayerState::eMove, new Move()));
	mContext.insert(std::make_pair(PlayerState::eWait, new StandBy()));
}

PlayerState Context::getState() const {
	return eState;
}

void Context::_setState(State* s) {
	if (mCurrentSate != nullptr) {
		mCurrentSate->level();
	}
	mCurrentSate = s;
	mCurrentSate->setContext(this);
	mCurrentSate->enter();
}

void Context::setState(PlayerState s) {
	auto it = mContext.find(s);
	if (it != mContext.end()) {
		State* state = it->second;
		eState = s;
		_setState(state);
	}
}