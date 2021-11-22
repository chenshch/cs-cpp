#include "CMonster.h"
#include "GameUtils.h"


CMonster* CMonster::createWith(int monsterId) {
	auto monster = new (std::nothrow) CMonster(monsterId);
	if (monster && monster->init()) {
		monster->autorelease();
	}
	else {
		delete monster;
		monster = nullptr;
		return nullptr;
	}
	return monster;
}

CMonster::CMonster(int monsterId) {
	setMonsterId(monsterId);
	atk_dis = cocos2d::RandomHelper::random_real(CPlayer::dis_atk, CPlayer::dis_atk + 30.0f);
}

CMonster::~CMonster() {

}

bool CMonster::init() {
	if (!CPlayer::init())
		return false;
	createName();
	setDir(MoveDir::eDown);
	rapidjson::Document doc;
	auto config = GameUtils::getConfig(doc,"configs/monster.json", "id", getMonsterId());
	if (config != nullptr) {
		auto name = (*config)["name"].GetString();
		setPlayerName(name, cocos2d::Color3B::RED);

		auto hp = (*config)["hp"].GetInt();
		setHpMax(hp);
		setHp(hp);
	}
	else {
		setPlayerName("monster", cocos2d::Color3B::RED);
	}
	updateAni();
	return true;
}

void CMonster::updateAni() {
	auto state = getPlayerState();
	auto str = CPlayer::getStateStr(state);
	auto dir = getDir();
	auto dir_str = CPlayer::getDirStr(dir);
	rapidjson::Document doc;
	auto config = GameUtils::getConfig(doc,"configs/monster.json", "id", getMonsterId());
	if (config == nullptr)
		return;
	auto frames_path = (*config)["icon"].GetString();
	auto str_file_pre = cocos2d::StringUtils::format("frames/%s/%s/%s/", frames_path, str, dir_str.c_str());
	auto str_plist_file = cocos2d::StringUtils::format("frames/%s/%s.plist", frames_path, frames_path);
	int count = GameUtils::getFramesCountInPlist(str_plist_file, str_file_pre);
	int loops = -1;
	if (state == PlayerState::eDeath || state == PlayerState::eAtk)
		loops = 1;
	auto ani_cache = cocos2d::StringUtils::format("monster_%d_%s_%s", getMonsterId(), str, dir_str.c_str());

	stFramesAniData data;
	data.node = getHeroSprite();
	data.pre_name = str_file_pre;
	data.count = count;
	if (state == PlayerState::eDeath)
		data.delay = 1.0f;
	else
		data.delay = 0.1f * count;
	data.loops = loops;
	data.cache_name = ani_cache;
	data.bPlist = true;
	data.func_end = [=] {
		if (state == PlayerState::eAtk) {
			setPlayerState(PlayerState::eWait);
		}
		else if (state == PlayerState::eDeath) {
			GameUtils::sendEvent(event_monster_remove, (void*)getUid());
			runAction(cocos2d::Sequence::create(cocos2d::DelayTime::create(0.1f), cocos2d::FadeOut::create(0.3f), cocos2d::CallFunc::create([=] {
				auto data = new DropData();
				data->pos.x = getPosition().x;
				data->pos.y = getPosition().y;
				data->is_boss = isBoss();
				GameUtils::sendEvent(event_boss_drop_item, (void*)data);
				removeFromParent();
				setOpacity(255);
			}), nullptr));
		}
	};
	if (state == PlayerState::eAtk) {

	}
	GameUtils::runFrameAction(data);

	CPlayer::updateAni();
}

void CMonster::updatePos(const cocos2d::Vec2& pos_hero, const cocos2d::Vec2& pos_next) {
	if (getPlayerState() == PlayerState::eDeath)
	{
		return;
	}
	auto dir = GameUtils::getCocosDir(getPosition(), pos_next);
	if (!isBoss()) {
		setDir(dir);
	}
	else {
		if (dir == MoveDir::eDown || dir == MoveDir::eLeft_Down || dir == MoveDir::eRight_Down) {
			setDir(dir);
		}
	}

	float dis = pos_hero.distance(getPosition());
	if (dis < atk_dis) {
		if (getPlayerState() != PlayerState::eAtk) {
			setPlayerState(PlayerState::eAtk);
			rapidjson::Document doc;
			auto config = GameUtils::getConfig(doc,"configs/monster.json", "id", getMonsterId());
			if (config == nullptr)
				return;
			auto atk = (*config)["atk"].GetInt();
			GameUtils::sendEvent(event_battle_role_hit, (void*)atk);
		}
		return;
	}

	if (isBoss()) {
		setPlayerState(PlayerState::eWait);
		return;
	}

	if (getPlayerState() != PlayerState::eMove) {
		setPlayerState(PlayerState::eMove);
	}

	float monsterX = getPositionX();
	float monsterY = getPositionY();

	if (abs(monsterX - pos_next.x) >= 1) {
		if (monsterX > pos_next.x) {
			monsterX--;
		}
		else if (monsterX < pos_next.x) {
			monsterX++;
		}
	}

	if (abs(monsterY - pos_next.y) >= 1)
	{
		if (monsterY > pos_next.y) {
			monsterY--;
		}
		else {
			monsterY++;
		}
	}

	setPosition(monsterX, monsterY);
}

bool CMonster::isBoss()
{
	return getMonsterId() == 3 || getMonsterId() == 4;
}
