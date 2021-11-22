#include "MapLayer.h"
#include "GameUtils.h"
#include "CNpc.h"
#include "ChatWindow.h"
#include "AStar.h"
#include "DropItem.h"
#include "PlayerManager.h"
#include "ReliveWindow.h"
#include "TaskLogic.h"

MapLayer::MapLayer() :m_role(nullptr), tmap(nullptr), block_data(nullptr), m_merc(nullptr)
{

}

MapLayer::~MapLayer()
{
	GameUtils::removeEvent(this);
	if (block_data) {
		cocos2d::Size nSize = tmap->getMapSize();
		for (int i = 0; i < nSize.width; i++)
			CC_SAFE_DELETE_ARRAY(block_data[i]);
		CC_SAFE_DELETE_ARRAY(block_data);
	}

	for (auto com : compentPool)
	{
		CC_SAFE_RELEASE(com);
	}

	for (auto monster : monsterPool)
	{
		CC_SAFE_RELEASE(monster);
	}

	for (auto monster : vMonsters)
	{
		CC_SAFE_RELEASE(monster);
	}
}

bool MapLayer::init()
{
	if (!Layer::init())
		return false;
	loadMap();
	scheduleUpdate();

	GameUtils::registEvent(event_battle_role_hit, CC_CALLBACK_1(MapLayer::battle_role_hit, this), this);
	GameUtils::registEvent(event_monster_remove, CC_CALLBACK_1(MapLayer::monster_remove, this), this);
	GameUtils::registEvent(event_role_atk_end, CC_CALLBACK_1(MapLayer::role_atk_end, this), this);
	GameUtils::registEvent(event_boss_drop_item, CC_CALLBACK_1(MapLayer::boss_drop_item, this), this);
	GameUtils::registEvent(event_battle_role_death, CC_CALLBACK_1(MapLayer::battle_role_death, this), this);
	GameUtils::registEvent(event_battle_role_relive, CC_CALLBACK_1(MapLayer::battle_role_relive, this), this);
	GameUtils::registEvent(event_merc_hide, CC_CALLBACK_1(MapLayer::merc_hide, this), this);
	
	return true;
}

void MapLayer::loadMap() {
	if (tmap != nullptr) {
		cocos2d::Size nSize = tmap->getMapSize();
		for (int i = 0; i < nSize.width; i++)
			CC_SAFE_DELETE_ARRAY(block_data[i]);
		CC_SAFE_DELETE_ARRAY(block_data);
		block_data = nullptr;

		tmap->removeFromParent();
		CC_SAFE_DELETE(tmap);
		GameUtils::fadeScene(0.5f);
	}

	tmap = cocos2d::TMXTiledMap::create("tmap/map.tmx");
	addChild(tmap, 0);
	tmap->setLocalZOrder(-1);

	cocos2d::Size nSize = tmap->getMapSize();
	block_data = new char* [(int)nSize.width];
	for (int i = 0; i < nSize.width; i++)
	{
		block_data[i] = new char[(int)nSize.height];
	}

	auto birth_layer = tmap->getLayer("chusheng");
	if (birth_layer != nullptr) {
		auto sz_birth = birth_layer->getLayerSize();
		for (auto x = 0; x < sz_birth.width; x++)
		{
			for (auto y = 0; y < sz_birth.height; y++)
			{
				auto gid_type = birth_layer->getTileGIDAt(cocos2d::Vec2(x, y));
				if (gid_type > 0) {
					auto pos = birth_layer->getPositionAt(cocos2d::Vec2(x, y));
					createRole();
					m_role->setBirthPos(pos);
					updateMapPos(pos);
					break;
				}
			}
		}
	}

	vMonsters.clear();
	auto monster_layer = tmap->getLayer("monster");
	if (monster_layer != nullptr) {
		auto sz_monster = monster_layer->getLayerSize();
		for (auto x = 0; x < sz_monster.width; x++)
		{
			for (auto y = 0; y < sz_monster.height; y++)
			{
				auto gid_type = monster_layer->getTileGIDAt(cocos2d::Vec2(x, y));
				if (gid_type > 0) {
					auto info = tmap->getPropertiesForGID(gid_type);
					if (!info.isNull()) {
						auto properties = info.asValueMap();
						auto monster_id = properties["monster_id"].asInt();
						auto monster = createMonsterFromPool(monster_id);
						//CCLOG("gid_type = %d,  monster_id = %d", gid_type, monster_id);
						tmap->addChild(monster);
						auto pos = monster_layer->getPositionAt(cocos2d::Vec2(x, y));
						monster->setPosition(pos);
						monster->setBirthPos(pos);
						monster->setLocalZOrder(tmap->getLayerNum() + 3);
						vMonsters.push_back(monster);
					}
				}
			}
		}
	}

	auto block_layer = tmap->getLayer("zuai");
	if (block_layer != nullptr) {
		auto sz_block = block_layer->getLayerSize();
		for (auto x = 0; x < sz_block.width; x++)
		{
			for (auto y = 0; y < sz_block.height; y++)
			{
				auto gid_type = block_layer->getTileGIDAt(cocos2d::Vec2(x, y));
				if (gid_type > 0) {
					block_data[x][y] = (char)BlockType::eBlock;
				}
				else {
					block_data[x][y] = (char)BlockType::eNormal;
				}
			}
		}
	}

	auto shadow_layer = tmap->getLayer("shadow");
	if (shadow_layer != nullptr) {
		auto sz_shadow = shadow_layer->getLayerSize();
		for (auto x = 0; x < sz_shadow.width; x++)
		{
			for (auto y = 0; y < sz_shadow.height; y++)
			{
				auto gid_type = shadow_layer->getTileGIDAt(cocos2d::Vec2(x, y));
				if (gid_type > 0) {
					auto v_block = block_data[x][y];
					block_data[x][y] = v_block | (char)BlockType::eShawdom;
				}
			}
		}
	}


	auto npc_layer = tmap->getLayer("npc");
	if (npc_layer != nullptr) {
		auto sz_npc = npc_layer->getLayerSize();
		for (auto x = 0; x < sz_npc.width; x++)
		{
			for (auto y = 0; y < sz_npc.height; y++)
			{
				auto gid_type = npc_layer->getTileGIDAt(cocos2d::Vec2(x, y));
				if (gid_type > 0) {
					auto info = tmap->getPropertiesForGID(gid_type);
					if (!info.isNull()) {
						auto properties = info.asValueMap();
						auto npc_id = properties["npc_id"].asInt();
						auto pos = npc_layer->getPositionAt(cocos2d::Vec2(x, y));
						auto npc = CNpc::createWith(npc_id);
						tmap->addChild(npc);
						npc->setPosition(pos);
						npc->setLocalZOrder(tmap->getLayerNum() + 1);
						auto func = [](int npc_id) {
							CCLOG("npc click == %d", npc_id);

							auto chat_window = ChatWindow::createWith(npc_id);
							chat_window->show();
						};
						npc->setNpcClickFunc(func);
					}
				}
			}
		}
	}

	auto CanReach = [&](const APoint& grid)->bool
	{
		auto block = block_data[grid.row][grid.col];
		return (block & (char)BlockType::eBlock) == 0;
	};

	star_def.row = nSize.height;
	star_def.col = nSize.width;
	star_def.can_reach = CanReach;
	star_def.allow_corner = false;
}

void MapLayer::createRole() {
	m_role = CRole::createRole(true);
	addChild(m_role, 1);
	auto sz_win = Director::getInstance()->getWinSize();
	m_role->setPosition(sz_win.width / 2, sz_win.height / 2);

	m_merc = CRole::createRole(false);
	addChild(m_merc, 1);
	m_merc->setFollowTarget(m_role);
}

bool MapLayer::isCollide(const cocos2d::Vec2& pos) {
	if (block_data) {
		auto coor = tileCoorFromPos(pos);
		int x = coor.x;
		int y = coor.y;
		char block = block_data[x][y];
		return (block & (char)BlockType::eBlock) > 0;
	}
	return false;
}

bool MapLayer::isShadow(const cocos2d::Vec2& pos) {
	if (block_data) {
		auto coor = tileCoorFromPos(pos);
		int x = coor.x;
		int y = coor.y;
		char block = block_data[x][y];
		return (block & (char)BlockType::eShawdom) > 0;
	}
	return false;
}

void MapLayer::updateMapPos(const cocos2d::Vec2& targetPos) {
	auto map_size = tmap->getContentSize();

	auto width = GRoot::getInstance()->getWidth();
	auto height = GRoot::getInstance()->getHeight();

	auto szWin = cocos2d::Director::getInstance()->getWinSize();
	auto v_center = cocos2d::Vec2(szWin.width / 2, szWin.height / 2);
	if (targetPos.x >= map_size.width - szWin.width / 2) {
		tmap->setPositionX(szWin.width - map_size.width);
		m_role->setPositionX(std::min(targetPos.x + tmap->getPositionX(), szWin.width));
	}
	else if (targetPos.x <= szWin.width / 2) {
		tmap->setPositionX(0);
		m_role->setPositionX(std::max(0.0f, targetPos.x + tmap->getPositionX()));
	}
	else {
		m_role->setPositionX(v_center.x);
		tmap->setPositionX(m_role->getPositionX() - targetPos.x);
	}

	if (targetPos.y >= map_size.height - szWin.height / 2) {
		tmap->setPositionY(szWin.height - map_size.height);
		m_role->setPositionY(std::min(targetPos.y + tmap->getPositionY(), szWin.height));
	}
	else if (targetPos.y <= szWin.height / 2) {
		tmap->setPositionY(0);
		m_role->setPositionY(std::max(0.0f, targetPos.y + tmap->getPositionY()));
	}
	else {
		m_role->setPositionY(v_center.y);
		tmap->setPositionY(m_role->getPositionY() - targetPos.y);
	}
}

void MapLayer::update(float delta)
{
	if (m_role == nullptr)
		return;
	auto role_pos = m_role->getPosition();
	auto des_pos = role_pos - tmap->getPosition();
	if (m_role->getPlayerState() == PlayerState::eMove) {
		auto s = m_role->getVelocity() * delta;
		des_pos = des_pos + s;
	}

	auto b = isCollide(des_pos);
	if (!b)
	{
		updateMapPos(des_pos);
	}

	auto role_pos_new = m_role->getPosition() - tmap->getPosition();
	auto shadom = isShadow(role_pos_new);
	if (shadom) {
		m_role->setOpacity(128);
	}
	else {
		m_role->setOpacity(255);
	}

	auto win_sz = Director::getInstance()->getWinSize();
	Rect rc_win(Vec2::ZERO, win_sz);
	for (auto monster : vMonsters)
	{
		auto monster_scene_pos = monster->getPosition() + tmap->getPosition();
		if (rc_win.containsPoint(monster_scene_pos)) {
			monster->setVisible(true);
			if (m_role->getPlayerState() == PlayerState::eDeath) {
				monster->setPlayerState(PlayerState::eWait);
			}
			else {

				auto monster_pos = monster->getPosition();
				auto monster_coor = tileCoorFromPos(monster_pos);

				auto role_coor = tileCoorFromPos(role_pos_new);

				star_def.start_point = APoint(monster_coor.x, monster_coor.y);
				star_def.end_point = APoint(role_coor.x, role_coor.y);

				Vec2 pos_next = role_pos_new;
				auto path = astar.Search(star_def);
				if (path.size() > 0) {
					auto layer_block = tmap->getLayer("zuai");
					pos_next = layer_block->getPositionAt(cocos2d::Vec2(path.at(0).row, path.at(0).col));
				}
				monster->updatePos(role_pos_new, pos_next);
			}
		}
		else {
			monster->setPlayerState(PlayerState::eWait);
			monster->setVisible(false);
		}

		m_role->updateFollow(tmap->getPosition());
		if (m_merc) {
			m_merc->updateFollow(tmap->getPosition());
		}
	}
}

Vec2 MapLayer::tileCoorFromPos(const Vec2& pos) {
	Vec2 ret;
	if (tmap) {
		auto sz = tmap->getTileSize();
		auto szCount = tmap->getMapSize();
		ret.x = ceilf(pos.x / sz.width);
		int y = ceilf(pos.y / sz.height) + 1;
		ret.y = szCount.height - y;
	}
	return ret;
}

void MapLayer::battle_role_hit(EventCustom* event) {
	int hp_s = (long)event->getUserData();
	auto hp = cocos2d::RandomHelper::random_int((int)(hp_s * 0.95f), (int)(hp_s * 1.05f));
	m_role->showHit(hp);
	GComponent* hit_ani = nullptr;
	if (compentPool.size() > 0) {
		hit_ani = compentPool.at(compentPool.size() - 1);
		compentPool.pop_back();
	}
	else {
		hit_ani = UIPackage::createObject("Main", "hit_ani")->as<GComponent>();
		hit_ani->retain();
	}
	auto text = hit_ani->getChild("n0");
	text->setText(StringUtils::format("%d", hp));
	addChild(hit_ani->displayObject());
	hit_ani->displayObject()->setLocalZOrder(999);
	hit_ani->displayObject()->setPosition(m_role->getPosition());
	auto t = hit_ani->getTransition("t0");
	t->play([=]() {
		hit_ani->displayObject()->removeFromParent();
		compentPool.push_back(hit_ani);
	});
}

void MapLayer::monster_remove(EventCustom* event) {
	int uid = (long)event->getUserData();

	for (size_t i = 0; i < vMonsters.size(); i++)
	{
		auto monster = vMonsters.at(i);
		if (monster->getUid() == uid) {
			vMonsters.erase(vMonsters.begin() + i);
			float tDelay = 5.0f;
			if (monster->isBoss()) {
				tDelay = 60.0f;
				PlayerManager::getInstance()->addExp(300);
			}
			else {
				PlayerManager::getInstance()->addExp(30);
			}
			TaskLogic::getInstance()->updateLogic(monster->getMonsterId());
			runAction(cocos2d::Sequence::create(cocos2d::DelayTime::create(tDelay), cocos2d::CallFunc::create([=] {
				monsterPool.push_back(monster);
				auto monster_relive = createMonsterFromPool(monster->getMonsterId());
				tmap->addChild(monster_relive, tmap->getLayerNum() + 1);
				monster_relive->setHp(monster_relive->getHpMax());
				monster_relive->setPosition(monster->getBirthPos());
				monster_relive->setPlayerState(PlayerState::eWait);
				vMonsters.push_back(monster_relive);
			}), nullptr));

			break;
		}
	}
}

void MapLayer::role_atk_end(EventCustom* event) {
	int skill_id = 0;
	auto data = event->getUserData();
	if (data) {
		skill_id = (long)data;
	}

	auto role_pos = m_role->getPosition() - tmap->getPosition();
	for (auto monster : vMonsters)
	{
		auto dis = CPlayer::dis_atk;
		if (skill_id == 1) {
			dis = CPlayer::dis_atk * 2.0f;
		}
		else if (skill_id > 1) {
			dis = CPlayer::dis_atk * 1.5f;
		}
		if (role_pos.distance(monster->getPosition()) <= dis) {
			auto vec = monster->getPosition() - role_pos;
			auto vDir = m_role->getSkillPos();
			auto radis = Vec2::angle(vec, vDir);
			auto angle = CC_RADIANS_TO_DEGREES(radis);
			int rangle = 45;
			if (skill_id == 1)
				rangle = 90;
			if (angle > -rangle && angle < rangle) {    //ÔÚ¹¥»÷·¶Î§ÄÚ

				auto atk = 10;
				rapidjson::Document doc;
				auto config = GameUtils::getConfig(doc, "configs/drealm.json", "id", PlayerManager::getInstance()->getLevel());
				if (config) {
					atk = (*config)["atk"].GetInt();
				}
				monster->showHit(atk);

				auto monster_scene_pos = monster->getPosition() + tmap->getPosition();
				auto hp = RandomHelper::random_int((int)(atk * 0.95f), (int)(atk * 1.05f));
				GComponent* hit_ani = nullptr;
				if (compentPool.size() > 0) {
					hit_ani = compentPool.at(compentPool.size() - 1);
					compentPool.pop_back();
				}
				else {
					hit_ani = UIPackage::createObject("Main", "hit_ani")->as<GComponent>();
					hit_ani->retain();
				}
				auto text = hit_ani->getChild("n0");
				text->setText(StringUtils::format("%d", hp));
				addChild(hit_ani->displayObject());
				hit_ani->displayObject()->setLocalZOrder(999);
				hit_ani->displayObject()->setPosition(monster_scene_pos);
				auto t = hit_ani->getTransition("t0");
				t->play([=]() {
					hit_ani->displayObject()->removeFromParent();
					compentPool.push_back(hit_ani);
				});
			}
		}
	}
}

CMonster* MapLayer::createMonsterFromPool(int monster_id)
{
	CMonster* monster = nullptr;
	int  index = -1;
	for (size_t i = 0; i < monsterPool.size(); i++)
	{
		if (monsterPool.at(i)->getMonsterId() == monster_id) {
			index = i;
			break;
		}
	}
	if (index >= 0) {
		monster = monsterPool.at(index);
		monster->setUid(CPlayer::getGid());
		monsterPool.erase(monsterPool.begin() + index);
	}
	else {
		monster = CMonster::createWith(monster_id);
		monster->setUid(CPlayer::getGid());
		monster->retain();
	}
	return monster;
}

void MapLayer::boss_drop_item(EventCustom* event) {
	DropData* data = (DropData*)event->getUserData();
	auto drop = DropItem::create();
	
	if (data->is_boss) {
		int equips[] = { 1102,2001,3000,1100,4003,5001,6001,7000,8002 };
		int count = (int)sizeof(equips) / sizeof(equips[0]);
		int rd = RandomHelper::random_int(0, count);
		drop->setEquip(equips[rd]);
	}
	else {
		int items[] = { 3,4 };
		int count = (int)sizeof(items) / sizeof(items[0]);
		int rd = RandomHelper::random_int(0, count);
		drop->setItem(items[rd]);
	}

	auto boss = data->is_boss;
	tmap->addChild(drop);
	drop->setLocalZOrder(tmap->getLayerNum() + 1);
	drop->setPosition(data->pos.x, data->pos.y);
	drop->setDropItemClickFunc([=]() {
		CCLOG("pick up drop item");
		if (boss) {
			PlayerManager::getInstance()->addEquip(drop->getItemId());
		}
		else {
			PlayerManager::getInstance()->addItem(drop->getItemId());
		}
		drop->removeFromParent();
	});
	CC_SAFE_DELETE(data);
}

void MapLayer::battle_role_death(EventCustom* event) {
	auto relive = ReliveWindow::create();
	relive->show();
}

void MapLayer::battle_role_relive(EventCustom* event) {
	updateMapPos(m_role->getBirthPos());
	m_role->setPlayerState(PlayerState::eWait);
	m_role->setHp(m_role->getHpMax());
	m_role->setOpacity(255);
}

void MapLayer::merc_hide(EventCustom* event) {
	auto merc_v = UserDefault::getInstance()->getBoolForKey("merc_hide", false);
	m_merc->setVisible(!merc_v);
}
