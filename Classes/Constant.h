#pragma once
#include "cocos2d.h"

struct stFramesAniData
{
	stFramesAniData() :node(nullptr)
		, count(0)
		, delay(1.0f)
		, loops(-1)
		, flags(0)
		, func_end(nullptr)
		, bPlist(false) {

	}
	cocos2d::Sprite* node;
	std::string pre_name;
	int count;
	float delay;
	int loops;
	int flags;
	std::string cache_name;
	std::function<void()> func_end;
	bool bPlist;
};

enum class MoveDir
{
	eUnkown = -1,
	eUp,
	eRight_Up,
	eRight,
	eRight_Down,
	eDown,
	eLeft_Down,
	eLeft,
	eLeft_Up,
};

enum class PlayerState {
	eWait,
	eMove,
	eAtk,
	eDeath,
};

enum class FairyUserEvent {
	eJoystickMove = 100,
	eJoystickEnd,
};

enum class BlockType {
	eNormal,
	eBlock = 0x1,
	eShawdom = 0x10,
};

struct tItem 
{
	int id = 0;
	int count = 0;

	void clear() {
		id = 0;
		count = 0;
	}

	bool isNull() {
		return id == 0;
	}
};

struct tEquip
{
	int id = 0;
	int up_lv = 0;
	int gid = 0;

	void clear() {
		id = 0;
		up_lv = 0;
		gid = 0;
	}

	bool isNull() {
		return id == 0 || gid == 0;
	}
};

enum class EquipPart {
	eWeapon,
	eHat,
	eClothes,
	eSkirt,
	eShoes,
	eGirdle,
	eBangle,
	eNecklace,
	eMax,
};

enum class TaskState {
	eAccpet,
	eDoing,
	eCondition,
	eFinish,
};

struct DropData
{
	cocos2d::Vec2 pos;
	bool is_boss = false;
};