#include "CRole.h"
#include "GameUtils.h"
#include "PlayerManager.h"

const int CRole::queue_max_frame_length = 30;
const float CRole::follow_near = 200.0f;
CRole* CRole::createRole(bool bmain) {
	CRole* role = new(std::nothrow) CRole();
	if (role && role->init(bmain))
	{
		role->autorelease();
		return role;
	}
	else
	{
		delete role;
		role = nullptr;
		return nullptr;
	}
}

CRole::CRole() :skillId(0), bMain(true)
, followTarget(nullptr)
, drawNode(nullptr) {
	//drawNode = cocos2d::DrawNode::create();
	//addChild(drawNode, 10);
	//drawNode->setLineWidth(2.0f);
}

CRole::~CRole() {

}

bool CRole::init() {
	if (!CPlayer::init())
		return false;
	createName();
	if (isMain()) {
		skill_effect = cocos2d::Sprite::create();
		addChild(skill_effect);
		setSpeed(400.0f);
	}

	rapidjson::Document doc;
	auto config = GameUtils::getConfig(doc, "configs/drealm.json", "id", PlayerManager::getInstance()->getLevel());
	if (config) {
		auto cfg_hp = (*config)["hp"].GetInt();
		setHpMax(cfg_hp);
		setHp(cfg_hp);
	}
	setDir(MoveDir::eRight);
	std::string str;
	if (isMain()) {
		str = GameUtils::gbk_to_utf8(PlayerManager::getInstance()->getName());
	}
	else {
		str = GameUtils::gbk_to_utf8("Ëæ´Ó");
	}
	setPlayerName(str, cocos2d::Color3B::WHITE);
	updateAni();
	//scheduleUpdate();
	return true;
}

bool CRole::init(bool main)
{
	setMain(main);
	return init();
}

void CRole::setHp(int _hp)
{
	CPlayer::setHp(_hp);
	if (isMain()) {
		PlayerManager::getInstance()->setHp(getHp());
	}
}

void CRole::setHpMax(int _hp_max)
{
	CPlayer::setHpMax(_hp_max);
	if (isMain()) {
		PlayerManager::getInstance()->setHpMax(_hp_max);
	}
}

void CRole::addHp(int add)
{
	auto max = getHpMax();
	auto hp = getHp();
	if (hp + add > max) {
		hp = max;
	}
	else {
		hp = hp + add;
	}
	setHp(hp);
}

void CRole::updateFollow(const cocos2d::Vec2& map_pos) {
	if (drawNode) {
		drawNode->clear();
		auto sz = getContentSize();
		drawNode->drawRect(cocos2d::Vec2(0, 0), cocos2d::Vec2(sz.width, sz.height), cocos2d::Color4F(1.0f, 1.0f, 0.0f, 1.0f));
	}

	if (followTarget) {
		auto fd_r = followTarget->getNextPathRecord();
		if (fd_r.dir != MoveDir::eUnkown) {
			MoveDir dir_old = getDir();
			setDir(fd_r.dir);
			setPlayerState(fd_r.state);
			auto s_pos = fd_r.pos - fd_r.map_pos + map_pos;
			setPosition(s_pos);
		}
		else {

		}
	}
	else {
		while (captainPathQueue.size() >= 100)
		{
			captainPathQueue.pop_front();
		}

		if (captainPathQueue.size() > 0) {
			auto last_data = captainPathQueue.back();
			auto last_role_pos = last_data.pos - last_data.map_pos;
			auto role_pos = getPosition() - map_pos;
			if (last_data.dir != getDir() || last_role_pos != role_pos ||
				((getPlayerState() == PlayerState::eMove || getPlayerState() == PlayerState::eWait) && last_data.state != getPlayerState())) {
				FollowData fd;
				fd.pos = getPosition();
				fd.dir = getDir();
				fd.state = getPlayerState();
				fd.map_pos = map_pos;
				captainPathQueue.push_back(fd);
				CCLOG("captainPathQueue.size == %d", captainPathQueue.size());
			}
		}
		else {
			FollowData fd;
			fd.pos = getPosition();
			fd.dir = getDir();
			fd.state = getPlayerState();
			fd.map_pos = map_pos;
			captainPathQueue.push_back(fd);
		}
	}


}

void CRole::updateAni() {
	auto state = getPlayerState();
	auto str = CPlayer::getStateStr(state);
	auto dir = getDir();
	auto str_dir = CPlayer::getDirStr(dir);
	std::string str_main = "merc";
	if (isMain()) {
		str_main = "role";
	}
	auto str_file_pre = cocos2d::StringUtils::format("frames/%s/%s/%s/", str_main.c_str(), str, str_dir.c_str());
	auto plist_file_name = cocos2d::StringUtils::format("frames/%s/%s.plist", str_main.c_str(), str_main.c_str());
	int count = GameUtils::getFramesCountInPlist(plist_file_name, str_file_pre);
	int loops = -1;
	if (state == PlayerState::eDeath || state == PlayerState::eAtk)
		loops = 1;
	auto ani_cache = cocos2d::StringUtils::format("%s_%d_%s_%s", str_main.c_str(), bMain ? 1 : 0, str, str_dir.c_str());

	stFramesAniData data;
	data.node = getHeroSprite();
	data.pre_name = str_file_pre;
	data.count = count;
	if (state == PlayerState::eDeath)
		data.delay = 1.0f;
	else
		data.delay = 0.1 * count;
	data.loops = loops;
	data.cache_name = ani_cache;
	data.bPlist = true;
	data.func_end = [=] {
		if (state == PlayerState::eAtk) {
			if (skillId > 0) {
				auto str_skill_pre = cocos2d::StringUtils::format("frames/effect/%d/", skillId);
				int skill_count = GameUtils::getFramesCountInPlist("frames/effect/effect.plist", str_skill_pre);
				stFramesAniData skill;
				skill.node = getSkillSprite();
				auto pos = getSkillPos();
				skill.node->setPosition(pos);
				skill.pre_name = str_skill_pre;
				skill.count = skill_count;
				skill.delay = 0.25f;
				skill.loops = 1;
				skill.cache_name = cocos2d::StringUtils::format("role_skill_%d", skillId);
				skill.bPlist = true;
				getSkillSprite()->setVisible(true);
				skill.func_end = [=] {
					if (isMain()) {
						GameUtils::sendEvent(event_role_atk_end, (void*)skillId);
					}
					setSkillId(0);
					getSkillSprite()->setVisible(false);
					setPlayerState(PlayerState::eWait);
				};
				GameUtils::runFrameAction(skill);
			}
			else {
				if (isMain()) {
					setPlayerState(PlayerState::eWait);
				}
				GameUtils::sendEvent(event_role_atk_end);
			}
		}
		else if (state == PlayerState::eDeath) {
			runAction(cocos2d::Sequence::create(cocos2d::FadeTo::create(0.5f, 80), cocos2d::DelayTime::create(0.2f), cocos2d::CallFunc::create([] {
				GameUtils::sendEvent(event_battle_role_death);
			}), nullptr));

		}
	};
	GameUtils::runFrameAction(data);

	CPlayer::updateAni();
}

CRole::FollowData CRole::getNextPathRecord()
{
	if (captainPathQueue.size() < CRole::queue_max_frame_length) {
		FollowData fd;
		return fd;
	}
	else
	{
		auto first = captainPathQueue.front();
		captainPathQueue.pop_front();
		return first;
	}
}

void CRole::setFollowTarget(CRole* role)
{
	followTarget = role;
	auto pos = role->getPosition();
	auto back_pos = role->getFollowBackPos() + pos;
	setPosition(back_pos);
}

cocos2d::Vec2 CRole::getSkillPos()
{
	auto dir = getDir();
	auto skew_dis = sqrtf(CPlayer::dis_atk * CPlayer::dis_atk / 2);
	switch (dir)
	{
	case MoveDir::eLeft:
		return cocos2d::Vec2(-CPlayer::dis_atk, 0.0f);
		break;
	case MoveDir::eRight:
		return cocos2d::Vec2(CPlayer::dis_atk, 0.0f);
		break;
	case MoveDir::eLeft_Up:
		return cocos2d::Vec2(-skew_dis, skew_dis);
		break;
	case MoveDir::eLeft_Down:
		return cocos2d::Vec2(-skew_dis, -skew_dis);
		break;
	case MoveDir::eUp:
		return cocos2d::Vec2(0.0f, CPlayer::dis_atk);
		break;
	case MoveDir::eDown:
		return cocos2d::Vec2(0.0f, -CPlayer::dis_atk);
		break;
	case MoveDir::eRight_Up:
		return cocos2d::Vec2(skew_dis, skew_dis);
		break;
	case MoveDir::eRight_Down:
		return cocos2d::Vec2(skew_dis, -skew_dis);
		break;
	default:
		break;
	}
	return cocos2d::Vec2::ZERO;
}

cocos2d::Vec2 CRole::getFollowBackPos() {
	Vec2 vpos;
	auto skew_follow = sqrtf(CRole::follow_near * CRole::follow_near / 2);
	switch (getDir())
	{
	case MoveDir::eUnkown:
		break;
	case MoveDir::eUp:
		vpos = Vec2(0, -CRole::follow_near);
		break;
	case MoveDir::eRight_Up:
		vpos = Vec2(-skew_follow, -skew_follow);
		break;
	case MoveDir::eRight:
		vpos = Vec2(-CRole::follow_near, 0);
		break;
	case MoveDir::eRight_Down:
		vpos = Vec2(-skew_follow, skew_follow);
		break;
	case MoveDir::eDown:
		vpos = Vec2(0, CRole::follow_near);
		break;
	case MoveDir::eLeft_Down:
		vpos = Vec2(skew_follow, skew_follow);
		break;
	case MoveDir::eLeft:
		vpos = Vec2(skew_follow, 0);
		break;
	case MoveDir::eLeft_Up:
		vpos = Vec2(skew_follow, -skew_follow);
		break;
	default:
		break;
	}
	return vpos;
}