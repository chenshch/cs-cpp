#include "State.h"

void State::setContext(Context* ct) noexcept {
	_context = ct;
}

void State::enter() {
	_context->getPlayer()->updateAni();
}

void State::level() {

}

void StandBy::enter() {
	State::enter();
}

void StandBy::level() {

}

void Move::enter() {
	State::enter();
}

void Move::level() {

}

void Attack::enter() {
	State::enter();
}

void Attack::level() {

}

void Hit::enter() {
	State::enter();
}

void Hit::level() {

}

void Death::enter() {
	State::enter();
}

void Death::level() {

}