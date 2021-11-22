#include "CPlayer.h"

USING_NS_FGUI;
const float CPlayer::dis_atk = 200.0f;
const float CPlayer::dis_patrol = 400.0f;
int CPlayer::gId = 0;

CPlayer::CPlayer() :mDir(MoveDir::eUnkown)
, walk_line_speed(0.0f)
, hp(0)
, hp_max(0)
, mMonsterId(0)
, uId(0)
, npc_Id(0)
, labName(nullptr)
, barHp(nullptr)
, hp_bar(nullptr){

}

CPlayer::~CPlayer() {
	CC_SAFE_RELEASE(hp_bar);
}

PlayerState CPlayer::getPlayerState() {
	if (mContext) {
		return mContext->getState();
	}
	return PlayerState::eWait;
}

void CPlayer::setDir(MoveDir dir) {
	if (mDir == dir)
		return;
	if (dir > MoveDir::eDown) {
		spRole->setFlippedX(true);
	}
	else {
		spRole->setFlippedX(false);
	}
	mDir = dir;
	spRole->stopAllActions();
	updateAni();
	updateVelocityByDir();
}

void CPlayer::setVelocity(const cocos2d::Vec2& _velocity) {
	velocity.x = _velocity.x;
	velocity.y = _velocity.y;
}

bool CPlayer::init() {
	if (!Node::init())
		return false;
	setCascadeOpacityEnabled(true);
	mContext = new Context(this);
	spRole = Sprite::create();
	addChild(spRole);
	return true;
}

void CPlayer::createName() {
	if (getMonsterId() > 0)
		hp_bar = UIPackage::createObject("Main", "bar_mp")->as<GProgressBar>();
	else
		hp_bar = UIPackage::createObject("Main", "bar_hp")->as<GProgressBar>();
	hp_bar->retain();
	addChild(hp_bar->displayObject());

	//auto sp = cocos2d::Sprite::createWithSpriteFrameName("Main/bar_bg.png");

	labName = cocos2d::Label::create();
	labName->setAnchorPoint(cocos2d::Vec2(0.5f, 0.5f));
	labName->setHorizontalAlignment(cocos2d::TextHAlignment::CENTER);
	labName->setSystemFontSize(22);
	addChild(labName);
}

void CPlayer::setPlayerName(const std::string& name, const cocos2d::Color3B color) {
	if (labName) {
		labName->setString(name);
		labName->setColor(color);
	}
}

void CPlayer::updateAni() {
	labName->setPositionY(spRole->getSpriteFrame()->getRect().size.height / 2);
	auto pos = labName->getPosition();
	pos.y = pos.y + 20;
	hp_bar->setPosition(pos.x, -pos.y);
}

const char* CPlayer::getStateStr(PlayerState s) {
	char* strs[] = { "standby","run","atk","dead" };
	int index = (int)s;
	return strs[index];
}

std::string CPlayer::getDirStr(MoveDir dir) {
	std::string str;
	MoveDir temp = dir;
	if (dir == MoveDir::eLeft) {
		temp = MoveDir::eRight;
	}
	else if (dir == MoveDir::eLeft_Down) {
		temp = MoveDir::eRight_Down;
	}
	else if (dir == MoveDir::eLeft_Up) {
		temp = MoveDir::eRight_Up;
	}
	str = cocos2d::StringUtils::format("%d", temp);
	return str;
}

void CPlayer::setPlayerState(PlayerState s) {
	if (mContext) {
		if (mContext->getState() == s)
			return;
		spRole->stopAllActions();
		mContext->setState(s);
	}
}

float CPlayer::getSkewSpeed() {
	float v = sqrtf(walk_line_speed * walk_line_speed / 2);
	return v;
}

void CPlayer::setHp(int _hp) {
	hp = _hp;
	float p = (float)hp / (float)hp_max * 100.0f;
	hp_bar->tweenValue(p, 0.1f);
}

void CPlayer::showHit(int _hp) { 
	if (getMonsterId() == 0) {
		spRole->setColor(cocos2d::Color3B(255, 0, 0));
		unschedule(schedule_selector(CPlayer::hitEnd));
		scheduleOnce(schedule_selector(CPlayer::hitEnd), 0.1f);
	}
	else {
		spRole->setColor(cocos2d::Color3B(0, 255, 0));
		unschedule(schedule_selector(CPlayer::hitEnd));
		scheduleOnce(schedule_selector(CPlayer::hitEnd), 0.1f);
	}
	auto hp_nw = hp - _hp;
	setHp(hp_nw);
	if (hp_nw <= 0) {
		if (getPlayerState() != PlayerState::eDeath) {
			setPlayerState(PlayerState::eDeath);
		}
	}
}

void CPlayer::hitEnd(float dt) {
	spRole->setColor(cocos2d::Color3B(255, 255, 255));
}

void CPlayer::updateVelocityByDir() {
	auto dir = getDir();
	auto speed_line = getLineSpeed();
	switch (dir)
	{
	case MoveDir::eLeft:
		setVelocity(cocos2d::Vec2(-speed_line, 0.0f));
		break;
	case MoveDir::eRight:
		setVelocity(cocos2d::Vec2(speed_line, 0.0f));
		break;
	case MoveDir::eLeft_Up:
		setVelocity(cocos2d::Vec2(-getSkewSpeed(), getSkewSpeed()));
		break;
	case MoveDir::eLeft_Down:
		setVelocity(cocos2d::Vec2(-getSkewSpeed(), -getSkewSpeed()));
		break;
	case MoveDir::eUp:
		setVelocity(cocos2d::Vec2(0.0f, speed_line));
		break;
	case MoveDir::eDown:
		setVelocity(cocos2d::Vec2(0.0f, -speed_line));
		break;
	case MoveDir::eRight_Up:
		setVelocity(cocos2d::Vec2(getSkewSpeed(), getSkewSpeed()));
		break;
	case MoveDir::eRight_Down:
		setVelocity(cocos2d::Vec2(getSkewSpeed(), -getSkewSpeed()));
		break;
	default:
		break;
	}
}
