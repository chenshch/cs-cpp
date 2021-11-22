#pragma once
#include "cocos2d.h"
#include "CRole.h"
#include "CMonster.h"
#include "AStar.h"

USING_NS_CC;
class MapLayer :
    public Layer
{
public:
    MapLayer();
    ~MapLayer();
    bool init();
	void loadMap();
	void createRole();
	bool isCollide(const cocos2d::Vec2& pos);
	bool isShadow(const cocos2d::Vec2& pos);
	void updateMapPos(const cocos2d::Vec2& targetPos);
	void update(float delta);	
	Vec2 tileCoorFromPos(const Vec2& pos);
	CRole* getRole() { return m_role; }
	CREATE_FUNC(MapLayer);
private:
	void battle_role_hit(EventCustom* event);
	void monster_remove(EventCustom* event);
	void role_atk_end(EventCustom* event);
	CMonster* createMonsterFromPool(int monster_id);
	void boss_drop_item(EventCustom* event);
	void battle_role_death(EventCustom* event);
	void battle_role_relive(EventCustom* event);
	void merc_hide(EventCustom* event);
	CRole* m_role;
	CRole* m_merc;
	cocos2d::TMXTiledMap* tmap;
	char** block_data;
	std::vector<CMonster*> vMonsters;
	std::vector<GComponent*> compentPool;
	std::vector<CMonster*> monsterPool;
	AStarDef star_def;
	AStar astar;
};

