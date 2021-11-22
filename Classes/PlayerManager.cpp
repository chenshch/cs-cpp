#include "PlayerManager.h"
#include "cocos2d.h"
#include "GameUtils.h"
#include "TaskLogic.h"

USING_NS_CC;
PlayerManager::PlayerManager():level(1), exp(0)
, hp(0)
, max_hp(0)
, gold(0)
{

}

PlayerManager::~PlayerManager() {

}

void PlayerManager::initData()
{
	getExp();
	getLevel();
	getHp();
	getHpMax();
	getGold();

	items.clear();
	auto str_items = UserDefault::getInstance()->getStringForKey("bag_items");
	auto vstr = GameUtils::split(str_items, "|");
	for (auto str : vstr)
	{
		auto str_item = GameUtils::split(str, ",");
		tItem item;
		item.id = Value(str_item.at(0)).asInt();
		item.count = Value(str_item.at(1)).asInt();
		items.push_back(item);
	}

	auto str_equips = UserDefault::getInstance()->getStringForKey("bag_equips");
	auto vstr_equip = GameUtils::split(str_equips, "|");
	for (auto str : vstr_equip)
	{
		auto str_equip = GameUtils::split(str, ",");
		tEquip equip;
		equip.id = Value(str_equip.at(0)).asInt();
		equip.up_lv = Value(str_equip.at(1)).asInt();
		equip.gid = Value(str_equip.at(2)).asInt();
		equips.push_back(equip);
	}

	auto str_part_equips = UserDefault::getInstance()->getStringForKey("part_equips");
	auto vstr_part = GameUtils::split(str_part_equips, "|");
	for (size_t i = 0; i < vstr_part.size();i++)
	{
		auto str = vstr_part.at(i);
		auto str_equip = GameUtils::split(str, ",");
		tEquip equip;
		equip.id = Value(str_equip.at(0)).asInt();
		equip.up_lv = Value(str_equip.at(1)).asInt();
		equip.gid = Value(str_equip.at(2)).asInt();
		partEquips[i] = equip;
	}

	if (!UserDefault::getInstance()->getBoolForKey("first_login", false)) {
		addItem(3, 10);
		addItem(4, 10);
		addEquip(111101, 1);
		setGold(getGold() + 10);
		UserDefault::getInstance()->setBoolForKey("first_login", true);
	}

	TaskLogic::getInstance()->initTask();
}

int PlayerManager::getLevel()
{
	if (level == 1) {
		level = UserDefault::getInstance()->getIntegerForKey("level",1);
	}
	return level;
}

void PlayerManager::setLevel(int lv)
{
	level = lv;
	UserDefault::getInstance()->setIntegerForKey("level", lv);
	GameUtils::sendEvent(event_update_role_lv);
}

int PlayerManager::getGold()
{
	if (gold == 0) {
		gold = UserDefault::getInstance()->getIntegerForKey("gold", 0);
	}
	return gold;
}

void PlayerManager::setGold(int _gold)
{
	gold = _gold;
	UserDefault::getInstance()->setIntegerForKey("gold", _gold);
	GameUtils::sendEvent(event_update_gold);
	TaskLogic::getInstance()->updateLogic();
}

void PlayerManager::setName(const char* sz_name)
{
	name = sz_name;
	UserDefault::getInstance()->setStringForKey("name", name);
}

std::string PlayerManager::getName()
{
	if (name.compare("") == 0) {
		name = UserDefault::getInstance()->getStringForKey("name","½£Ê¿");
	}
	return name;
}

void PlayerManager::setExp(int _exp)
{
	exp = _exp;
	UserDefault::getInstance()->setIntegerForKey("exp", _exp);
}

int PlayerManager::getExp()
{
	if (exp == 0) {
		exp = UserDefault::getInstance()->getIntegerForKey("exp", 0);
	}
	return exp;
}

void PlayerManager::addExp(int _add)
{
	auto exp = getExp();
	int ext_t = exp + _add;
	setExp(ext_t);
	int lv = getLevel();
	rapidjson::Document doc;
	auto config = GameUtils::getConfig(doc,"configs/drealm.json","id",lv);
	if (config) {
		auto upgrade_exp = (*config)["exp"].GetInt();
		if (ext_t >= upgrade_exp) {
			setLevel(getLevel() + 1);
		}
	}
}

int PlayerManager::getHp()
{
	if (hp == 0) {
		hp = UserDefault::getInstance()->getIntegerForKey("hp", 100);
	}
	return hp;
}

void PlayerManager::setHp(int _hp)
{
	hp = _hp;
	UserDefault::getInstance()->setIntegerForKey("hp", _hp);
	GameUtils::sendEvent(event_update_role_hp);
}

int PlayerManager::getHpMax()
{
	if (max_hp == 0) {
		max_hp = UserDefault::getInstance()->getIntegerForKey("max_hp", 100);
	}
	return max_hp;
}

void PlayerManager::setHpMax(int _hp_max)
{
	max_hp = _hp_max;
	UserDefault::getInstance()->setIntegerForKey("max_hp", _hp_max);

}

int PlayerManager::getItemCount(int id)
{
	for (auto item : items)
	{
		if (item.id == id) {
			return item.count;
		}
	}
	return 0;
}

void PlayerManager::addItem(int id, int count /* = 1 */)
{
	if (id == 1) {
		addExp(count);
	}
	else if (id == 2) {
		setGold(getGold() + count);
	}
	else {
		bool bfind = false;
		for (auto& item : items)
		{
			if (item.id == id) {
				item.count += count;
				bfind = true;
				break;
			}
		}

		if (!bfind) {
			tItem item;
			item.id = id;
			item.count = count;
			items.push_back(item);
		}

		updateItems();

		GameUtils::sendEvent(event_update_items, (void*)id);
	}
}

void PlayerManager::updateItems()
{
	std::string str_items;
	for (auto item : items)
	{
		if (!str_items.empty()) {
			str_items = str_items.append("|");
		}

		auto str = StringUtils::format("%d,%d", item.id, item.count);
		str_items = str_items.append(str);
	}

	UserDefault::getInstance()->setStringForKey("bag_items", str_items);
}

bool PlayerManager::checkItemEnough(int id, int count /*= 1*/)
{
	for (auto item : items)
	{
		if (item.id == id) {
			if (item.count >= count) {
				return true;
			}
		}
	}
	return false;
}

void PlayerManager::addEquip(int id, int _lv /* = 0 */,int gid /* = 0 */)
{
	rapidjson::Document doc;
	auto config = GameUtils::getConfig(doc,"configs/eequip.json","id",id);
	if (config) {
		_lv = (*config)["lv"].GetInt();
	}
	bool bfind = false;
	for (auto& equip: equips)
	{
		if (equip.isNull()) {
			equip.id = id;
			equip.up_lv = _lv;
			if (gid == 0)
				equip.gid = time(nullptr);
			else
				equip.gid = gid;
			bfind = true;
			break;
		}
	}
	if (!bfind) {
		tEquip equip;
		equip.id = id;
		equip.up_lv = _lv;
		if (gid == 0)
			equip.gid = time(nullptr);
		else
			equip.gid = gid;
		equips.push_back(equip);
	}
	updateEquips();
}

void PlayerManager::removeEquip(int gid) {
	for (auto& equip : equips)
	{
		if (equip.gid == gid) {
			equip.clear();
		}
	}
	updateEquips();
}

bool PlayerManager::hasEquip(int gid)
{
	for (auto& equip : equips)
	{
		if (equip.gid == gid) {
			return true;
		}
	}
	return false;
}

std::vector<tItem> PlayerManager::getItemsByType(int _type)
{
	std::vector<tItem> ret_items;
	for (auto item : items)
	{
		rapidjson::Document doc;
		auto config = GameUtils::getConfig(doc,"configs/item.json", "id", item.id);
		if (config) {
			auto type = (*config)["type"].GetInt();
			if (type == _type) {
				ret_items.push_back(item);
			}
		}
	}
	return ret_items;
}

void PlayerManager::updateEquips() {
	std::string str_equips;
	for (const auto& equip : equips)
	{
		if (!str_equips.empty()) {
			str_equips = str_equips.append("|");
		}

		auto str = StringUtils::format("%d,%d,%d", equip.id, equip.up_lv,equip.gid);
		str_equips = str_equips.append(str);
	}

	UserDefault::getInstance()->setStringForKey("bag_equips", str_equips);
}

tEquip PlayerManager::getPartEquip(int part) {
	return partEquips[part];
}

void PlayerManager::takeOffEquips(int part)
{
	if (part >= 0 && part < (int)EquipPart::eMax) {
		if (partEquips[part].id > 0) {
			tEquip equip = partEquips[part];
			partEquips[part].clear();
			addEquip(equip.id, equip.up_lv, equip.gid);
			updateEquips();
			updatePartEquips();
			GameUtils::sendEvent(event_update_equip_part, (void*)part);
			GameUtils::sendEvent(event_update_equip_bag);
		}
	}
}

void PlayerManager::swapEquip(int gid)
{
	for (auto& equip : equips)
	{
		if (equip.gid == gid) {
			auto part = PlayerManager::getEquipPart(equip.id);
			if (partEquips[part].id > 0) {
				auto equip_old  = partEquips[part];
				partEquips[part] = equip;
				equip = equip_old;
			}
			else {
				partEquips[part] = equip;
				equip.clear();
			}

			updateEquips();
			updatePartEquips();

			GameUtils::sendEvent(event_update_equip_part, (void*)part);
			GameUtils::sendEvent(event_update_equip_bag);
			return;
		}
	}
}

void PlayerManager::updatePartEquips() {
	std::string str_part_equips;
	for (auto i = 0; i < (int)EquipPart::eMax;i++)
	{
		auto equip = partEquips[i];
		if (!str_part_equips.empty()) {
			str_part_equips.append("|");
		}
		auto str = StringUtils::format("%d,%d,%d", equip.id, equip.up_lv, equip.gid);
		str_part_equips.append(str);
	}
	UserDefault::getInstance()->setStringForKey("part_equips", str_part_equips);
}

void PlayerManager::sortEquip() {
	for (auto it = equips.begin(); it != equips.end();) {
		if ((*it).id == 0) {
			it = equips.erase(it);
		}
		else {
			++it;
		}
	}
	updateEquips();
	GameUtils::sendEvent(event_update_equip_bag);
}

void PlayerManager::sortItem(int type)
{
	for (auto it = items.begin(); it != items.end();) {
		if ((*it).id == 0) {
			it = items.erase(it);
		}
		else {
			++it;
		}
	}
	GameUtils::sendEvent(event_update_items);
}

int PlayerManager::getEquipPart(int id) {
	int part = 0;
	rapidjson::Document doc;
	auto config = GameUtils::getConfig(doc,"configs/eequip.json", "id", id);
	if (config) {
		part = (*config)["type_int"].GetInt();
	}
	return part;
}