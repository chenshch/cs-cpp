#include "CommonWindow.h"
#include "PlayerManager.h"
#include "GameUtils.h"
#include "EquipTips.h"
#include "ItemTips.h"
#include "Toast.h"
#include "TaskLogic.h"
#include "MercTips.h"
#include "FMessageBox.h"
#include "DpManager.h"


static const int MIN_BAG_GRID = 24;
CommonWindow::CommonWindow():listEquip(nullptr),listBag(nullptr), listSkill(nullptr), listTask(nullptr),listMerc(nullptr), skill_index(0)
{

}

CommonWindow::~CommonWindow()
{

}

void CommonWindow::onInit()
{
	BaseWindow::onInit();
	setModal(true);

	auto btn_back = getContentPane()->getChild("n3");
	btn_back->addClickListener(CC_CALLBACK_1(CommonWindow::backClick, this));

	auto btn_sell_all = getContentPane()->getChild("n34");
	btn_sell_all->addClickListener(CC_CALLBACK_1(CommonWindow::sellAllClick, this));

	auto btn_sort = getContentPane()->getChild("n35");
	btn_sort->addClickListener(CC_CALLBACK_1(CommonWindow::sortItems, this));

	auto control = getContentPane()->getController("sub_button");
	control->addEventListener(UIEventType::Changed, CC_CALLBACK_0(CommonWindow::subSelect, this));

	auto taskControl = getContentPane()->getController("task_tab");
	taskControl->addEventListener(UIEventType::Changed, CC_CALLBACK_0(CommonWindow::taskSelect, this));

	initBag();
	subSelect();
	update_gold(nullptr);

	initShop();
	initSkill();
	initTask();
	initMerc();
	updatePower();

	auto node = displayObject();
	GameUtils::registEvent(event_update_gold, CC_CALLBACK_1(CommonWindow::update_gold, this), node);
	GameUtils::registEvent(event_update_items, CC_CALLBACK_1(CommonWindow::update_items, this), node);
	GameUtils::registEvent(event_update_equip_part, CC_CALLBACK_1(CommonWindow::update_equip_part, this), node);
	GameUtils::registEvent(event_update_equip_bag, CC_CALLBACK_1(CommonWindow::update_equip_bag, this), node);
	GameUtils::registEvent(event_update_role_power, CC_CALLBACK_1(CommonWindow::update_role_power, this), node);
}

void CommonWindow::onExit()
{
	BaseWindow::onExit();
	auto node = displayObject();
	GameUtils::removeEvent(node);
}

void CommonWindow::setPage(int index)
{
	auto control = getContentPane()->getController("button");
	if (control) {
		control->setSelectedIndex(index);
	}
}

void CommonWindow::backClick(EventContext* context) {
	this->hide();
}

void CommonWindow::sortItems(EventContext* context) {
	auto index = getContentPane()->getController("sub_button")->getSelectedIndex();
	if (index > 0) {
		PlayerManager::getInstance()->sortItem(index);
	}
	else {
		PlayerManager::getInstance()->sortEquip();
	}
}

void CommonWindow::initBag()
{
	listBag = getContentPane()->getChild("n20")->as<GList>();
	listBag->addEventListener(UIEventType::ClickItem, CC_CALLBACK_1(CommonWindow::onClickBagItem, this));
	listBag->itemRenderer = CC_CALLBACK_2(CommonWindow::renderListItem, this);

	listEquip = getContentPane()->getChild("n78")->as<GList>();
	auto equips = PlayerManager::getInstance()->getEquips();
	listEquip->addEventListener(UIEventType::ClickItem, CC_CALLBACK_1(CommonWindow::onClickBagEquip, this));
	listEquip->itemRenderer = CC_CALLBACK_2(CommonWindow::renderListEquip, this);
	listEquip->setNumItems(std::max(equips.size(), (size_t)MIN_BAG_GRID));

	initPartEquip();
}

void CommonWindow::initPartEquip()
{
	for (auto i = 0; i < (int)EquipPart::eMax; i++)
	{
		auto item = getContentPane()->getChild(StringUtils::format("item%d", i))->as<GLabel>();
		item->setCustomData(Value(i));
		item->addClickListener(CC_CALLBACK_1(CommonWindow::onClickPartEquip, this));
		item->getChild("n3")->setVisible(false);
		auto loader = item->getChild("n2")->as<GLoader>();

		auto equip = PlayerManager::getInstance()->getPartEquip(i);
		if (equip.id > 0) {
			loader->setVisible(true);
			rapidjson::Document doc;
			auto config = GameUtils::getConfig(doc,"configs/eequip.json", "id", equip.id);
			if (config) {
				std::string icon = (*config)["icon"].GetString();
				icon.erase(icon.end() - 4, icon.end());
				loader->setIcon("ui://Common/" + icon);
			}
		}
		else {
			loader->setVisible(false);
		}
	}

	rapidjson::Document doc;
	auto config = GameUtils::getConfig(doc, "configs/drealm.json", "id", PlayerManager::getInstance()->getLevel());
	if (config) {
		auto cfg_atk = (*config)["atk"].GetInt();
		auto cfg_hp = (*config)["hp"].GetInt();
		auto cfg_def = (*config)["def"].GetInt();
		auto cfg_crit = (*config)["crit"].GetFloat();
		auto cfg_doge = (*config)["doge"].GetFloat();
		auto cfg_phy = (*config)["phy"].GetFloat();
		auto cfg_pow = (*config)["pow"].GetFloat();

		auto text_atk = getContentPane()->getChild("n25");
		auto text_crit = getContentPane()->getChild("n26");
		auto text_doge = getContentPane()->getChild("n27");
		auto text_def = getContentPane()->getChild("n28");
		auto text_phy = getContentPane()->getChild("n29");
		auto text_pow = getContentPane()->getChild("n30");
		auto text_hp = getContentPane()->getChild("n31");

		text_atk->setText(GameUtils::gbk_to_utf8(StringUtils::format("攻击：%d", cfg_atk)));
		text_crit->setText(GameUtils::gbk_to_utf8(StringUtils::format("暴击率：%0.3f", cfg_crit)));
		text_doge->setText(GameUtils::gbk_to_utf8(StringUtils::format("暴击伤害：%0.3f", cfg_doge)));
		text_def->setText(GameUtils::gbk_to_utf8(StringUtils::format("防御：%d", cfg_def)));
		text_phy->setText(GameUtils::gbk_to_utf8(StringUtils::format("闪避率：%0.3f", cfg_phy)));
		text_pow->setText(GameUtils::gbk_to_utf8(StringUtils::format("伤害减免：%0.3f", cfg_pow)));
		text_hp->setText(GameUtils::gbk_to_utf8(StringUtils::format("生命：%d", cfg_hp)));
	}
}

void CommonWindow::initTask()
{
	listTask = getContentPane()->getChild("n55")->as<GList>();
	listTask->itemRenderer = CC_CALLBACK_2(CommonWindow::renderTaskItem, this);
	listTask->addEventListener(UIEventType::ClickItem, CC_CALLBACK_1(CommonWindow::onClickTaskItem, this));
	updateTask(0);
}

void CommonWindow::initSkill()
{

	listSkill = getContentPane()->getChild("n42")->as<GList>();
	listSkill->itemRenderer = CC_CALLBACK_2(CommonWindow::renderSkillItem, this);
	listSkill->addEventListener(UIEventType::ClickItem, CC_CALLBACK_1(CommonWindow::onClickSkillItem, this));
	std::string str_skills = UserDefault::getInstance()->getStringForKey("role_skill","1,1,1,1");
	auto vstr = GameUtils::split(str_skills, ",");
	listSkill->setNumItems(vstr.size());

	getContentPane()->getChild("n84")->addClickListener(CC_CALLBACK_1(CommonWindow::onClickSkillUpgrade,this));

	updateSkillInfo(skill_index);
}

void CommonWindow::initMerc()
{
	listMerc = getContentPane()->getChild("n61")->as<GList>();
	listMerc->itemRenderer = CC_CALLBACK_2(CommonWindow::renderMercItem, this);
	listMerc->setNumItems(35);

	int levels[] = { 0,30,50,60 };
	for (int i = 0; i < 4; i++)
	{
		auto  merc_item = getContentPane()->getChild(StringUtils::format("n%d",62+i))->as<GLabel>();
		if (i == 0) {
			merc_item->getChild("n3")->setVisible(false);
			merc_item->getChild("n4")->setVisible(false);
		}
		else {
			merc_item->getChild("n2")->setVisible(false);
			merc_item->getChild("n4")->setText(GameUtils::gbk_to_utf8(StringUtils::format("%d级解锁", levels[i])));
		}
		merc_item->setCustomData(Value(levels[i]));
		merc_item->addClickListener(CC_CALLBACK_1(CommonWindow::mercItemClick, this,i));
	}

}

void CommonWindow::mercItemClick(EventContext* context,int index) {
	if (index == 0) {
		auto tips = MercTips::create();
		tips->show();
	}
	else {
		auto btn = (GButton*)context->getSender();
		auto level = btn->getCustomData().asInt();
		Toast::getInstance()->showText(GameUtils::gbk_to_utf8(StringUtils::format("%d级解锁", level)),Color3B::RED);
	}
}

void CommonWindow::onClickBagItem(EventContext* context) {
	auto lab = (GComponent*)context->getData();
	auto id = lab->getCustomData().asInt();
	if (id > 0) {
		auto tips = ItemTips::createWith(id);
		tips->show();
	}
}

void CommonWindow::renderListItem(int index, GObject* obj)
{
	auto lab = obj->as<GLabel>();
	auto text_count = lab->getChild("n3")->as<GTextField>();
	auto gload = lab->getChild("n2")->as<GLoader>();
	auto type = getContentPane()->getController("sub_button")->getSelectedIndex();
	if (type > 0) {
		auto items = PlayerManager::getInstance()->getItemsByType(type);
		if ((size_t)index >= items.size()) {
			gload->setVisible(false);
			text_count->setVisible(false);
			obj->setCustomData(Value::Null);
		}
		else {
			gload->setVisible(true);
			text_count->setVisible(true);
			auto item = items.at(index);
			rapidjson::Document doc;
			auto config = GameUtils::getConfig(doc,"configs/item.json", "id", item.id);
			if (config) {
				std::string icon = (*config)["icon"].GetString();
				icon.erase(icon.end() - 4, icon.end());
				gload->setIcon("ui://Common/" + icon);
			}
			obj->setCustomData(Value(item.id));
			text_count->setText(Value(item.count).asString());
		}
	}
}

void CommonWindow::onClickBagEquip(EventContext* context)
{
	auto lab = (GComponent*)context->getData();
	auto index = lab->getCustomData().asInt();
	auto equips = PlayerManager::getInstance()->getEquips();
	if ((size_t)index < equips.size()) {
		auto equip = equips.at(index);
		if (equip.id > 0) {
			auto part = PlayerManager::getEquipPart(equip.id);
			auto part_equip = PlayerManager::getInstance()->getPartEquip(part);
			auto tips = EquipTips::createWith(part_equip, equip);
			tips->show();
		}
	}
}

void CommonWindow::renderListEquip(int index, GObject* obj)
{
	auto label = obj->as<GLabel>();
	auto text_count = label->getChild("n3")->as<GTextField>();
	text_count->setVisible(false);
	auto gload = label->getChild("n2")->as<GLoader>();

	auto equips = PlayerManager::getInstance()->getEquips();
	if ((size_t)index >= equips.size()) {
		gload->setVisible(false);
	}
	else {
		
		auto item = equips.at(index);
		if (item.id > 0) {
			gload->setVisible(true);
			rapidjson::Document doc;
			auto config = GameUtils::getConfig(doc,"configs/eequip.json", "id", item.id);
			if (config) {
				std::string icon = (*config)["icon"].GetString();
				icon.erase(icon.end() - 4, icon.end());
				gload->setIcon("ui://Common/" + icon);
			}
		}
		else {
			gload->setVisible(false);
		}
	}
	obj->setCustomData(Value(index));
}

void CommonWindow::onClickPartEquip(EventContext* context) {
	auto btn = (GLabel*)context->getSender();
	auto part = btn->getCustomData().asInt();
	auto part_equip = PlayerManager::getInstance()->getPartEquip(part);
	if (!part_equip.isNull()) {
		tEquip equip;
		auto tips = EquipTips::createWith(part_equip, equip);
		tips->show();
	}
}

void CommonWindow::sellAllClick(EventContext* context) {
	int add_gold = 0;
	auto index = getContentPane()->getController("sub_button")->getSelectedIndex();
	auto& equips = PlayerManager::getInstance()->getEquips();
	if (index == 0) {
		for (auto equip : equips)
		{
			if (equip.id > 0) {
				rapidjson::Document doc;
				auto config = GameUtils::getConfig(doc,"configs/eequip.json", "id", equip.id);
				if (config) {
					add_gold += (*config)["sell"].GetInt();
				}
			}
		}
	}
	else {
		auto& items = PlayerManager::getInstance()->getItems();
		for (auto it = items.begin(); it != items.end();)
		{
			rapidjson::Document doc;
			auto config = GameUtils::getConfig(doc,"configs/item.json", "id", (*it).id);
			if (config) {
				auto type = (*config)["type"].GetInt();
				if (type == index) {
					auto sell_price = (*config)["sell"].GetInt();
					add_gold += (sell_price * (*it).count);
					it = items.erase(it);
				}
				else {
					++it;
				}
			}
			else {
				++it;
			}
		}
	}
	if (add_gold > 0) {
		if (index == 0) {
			equips.clear();
			PlayerManager::getInstance()->updateEquips();
			update_equip_bag(nullptr);
		}
		else {
			PlayerManager::getInstance()->updateItems();
			update_items(nullptr);
		}
		PlayerManager::getInstance()->setGold(PlayerManager::getInstance()->getGold() + add_gold);
	}
}

void CommonWindow::updateItems(int type)
{
	auto items = PlayerManager::getInstance()->getItemsByType(type);
	listBag->setNumItems(std::max(items.size(), (size_t)MIN_BAG_GRID));
}

void CommonWindow::initShop()
{
	rapidjson::Document doc;
	GameUtils::readJson("configs/jrecharge.json", doc);
	listShop = getContentPane()->getChild("n72")->as<GList>();
	listShop->addEventListener(UIEventType::ClickItem, CC_CALLBACK_1(CommonWindow::onClickShopItem, this));
	listShop->itemRenderer = CC_CALLBACK_2(CommonWindow::renderListShopItem, this);
	listShop->setNumItems(doc.Size());

	auto listEquipShop = getContentPane()->getChild("n77")->as<GList>();
	listEquipShop->itemRenderer = CC_CALLBACK_2(CommonWindow::renderShopEquipItem, this);
	listEquipShop->setNumItems(50);
	
}

void CommonWindow::updatePower()
{
	int power_total = 0;
	for (auto i = 0; i < (int)EquipPart::eMax; i++)
	{
		auto equip = PlayerManager::getInstance()->getPartEquip(i);
		if (equip.id > 0) {
			rapidjson::Document doc;
			auto config = GameUtils::getConfig(doc, "configs/eequip.json", "id", equip.id);
			if (config) {
				auto power = (*config)["score"].GetInt();
				power_total += power;
			}
		}
	}

	auto text_power = getContentPane()->getChild("n24")->as<GTextField>();
	text_power->setText(StringUtils::format("%d", power_total));
}

void CommonWindow::subSelect() {
	auto index = getContentPane()->getController("sub_button")->getSelectedIndex();
	if (index > 0) {
		updateItems(index);
	}
}

void CommonWindow::taskSelect() {
	auto index = getContentPane()->getController("task_tab")->getSelectedIndex();
	updateTask(index);
}

void CommonWindow::updateTask(int type /*= 0*/)
{
	task_ids.clear();

	auto mapTask = TaskLogic::getInstance()->getTasks();

	for (auto it = mapTask.begin(); it != mapTask.end(); ++it)
	{
		if (type == 0) {
			task_ids.push_back(it->first);
		}
		else if (type == 1) {
			if (it->second->task_type == 1) {
				task_ids.push_back(it->first);
			}
		}
		else if (type == 2) {
			if (it->second->task_type == 2) {
				task_ids.push_back(it->first);
			}
		}
	}

	listTask->setNumItems(task_ids.size());
}

void CommonWindow::update_gold(EventCustom* event) {
	auto text = getContentPane()->getChild("n40")->as<GTextField>();
	text->setText(Value(PlayerManager::getInstance()->getGold()).asString());
}

void CommonWindow::update_items(EventCustom* event) {
	auto index = getContentPane()->getController("sub_button")->getSelectedIndex();
	if (index > 0)
	{
		updateItems(index);
	}
}

void CommonWindow::update_equip_part(EventCustom* event) {
	long part = (long)event->getUserData();
	auto item = getContentPane()->getChild(StringUtils::format("item%d", part))->as<GLabel>();
	auto loader = item->getChild("n2")->as<GLoader>();
	auto equip = PlayerManager::getInstance()->getPartEquip(part);
	if (equip.id > 0) {
		loader->setVisible(true);
		rapidjson::Document doc;
		auto config = GameUtils::getConfig(doc,"configs/eequip.json", "id", equip.id);
		if (config) {
			std::string icon = (*config)["icon"].GetString();
			icon.erase(icon.end() - 4, icon.end());
			loader->setIcon("ui://Common/" + icon);
		}
	}
	else {
		loader->setVisible(false);
	}
}

void CommonWindow::update_equip_bag(EventCustom* event) {
	auto equips = PlayerManager::getInstance()->getEquips();
	listEquip->setNumItems(std::max(equips.size(), (size_t)MIN_BAG_GRID));
}

void CommonWindow::onClickShopItem(EventContext* context) {
	auto item = (GObject*)context->getData();
	auto index = listShop->childIndexToItemIndex(listShop->getChildIndex(item));
	rapidjson::Document doc;
	auto config = GameUtils::getConfig(doc,"configs/jrecharge.json", "id", index + 1);
	if (config) {
		auto price = (*config)["recharge"].GetInt();
		auto gold = (*config)["get"].GetInt();
		auto mess = FMessageBox::create();
		mess->show();
		mess->setMsgString(GameUtils::gbk_to_utf8(StringUtils::format("确定花费%d购买%d元宝吗", price, gold)));
		mess->setOkHandler(CC_CALLBACK_0(CommonWindow::conformBuy, this, price, gold));
	}
}

void CommonWindow::conformBuy(int cost, int gold) {
	DpManager::getInstance()->did_cq(cost, gold,nullptr,nullptr);
}

void CommonWindow::renderListShopItem(int index, GObject* obj) {
	auto lab = obj->as<GLabel>();
	auto text_price = lab->getChild("n4");
	auto text_gold = lab->getChild("n5");
	auto loader = lab->getChild("n8")->as<GLoader>();
	rapidjson::Document doc;
	auto config = GameUtils::getConfig(doc,"configs/jrecharge.json", "id", index + 1);
	if (config) {
		auto price = (*config)["recharge"].GetInt();
		text_price->setText(GameUtils::gbk_to_utf8(StringUtils::format("￥%d", price)));

		std::string icon = (*config)["icon"].GetString();
		icon.erase(icon.end() - 4,icon.end());
		loader->setIcon("ui://Common/"+icon);

		auto gold = (*config)["get"].GetInt();
		text_gold->setText(StringUtils::format("%d", gold));
	}
}

void CommonWindow::renderShopEquipItem(int index, GObject* obj) {
	auto lab = obj->as<GLabel>();
	auto loader = lab->getChild("n2")->as<GLoader>();
	auto text_count = lab->getChild("n3");
	loader->setVisible(false);
	text_count->setVisible(false);
}

void CommonWindow::renderSkillItem(int index, GObject* obj) {
	auto skill_item = obj->as<GComponent>();
	std::string str_skills = UserDefault::getInstance()->getStringForKey("role_skill", "1,1,1,1");
	auto vstr = GameUtils::split(str_skills, ",");
	auto skill_lv = Value(vstr[index]).asInt();
	rapidjson::Document doc;
	auto config = getSkillConfig(doc, index + 1, skill_lv);
	if (config) {
		auto text_name = skill_item->getChild("n3")->as<GTextField>();
		auto loader = skill_item->getChild("n4")->as<GLoader>();

		auto name = (*config)["name"].GetString();
		std::string icon = (*config)["icon"].GetString();
		icon.erase(icon.end() - 4, icon.end());
		text_name->setText(name);
		loader->setIcon("ui://Common/" + icon);
	}
}

void CommonWindow::onClickSkillItem(EventContext* context) {
	auto item = (GComponent*)context->getData();
	auto index = listSkill->childIndexToItemIndex(listSkill->getChildIndex(item));
	skill_index = index;
	updateSkillInfo(index);
}

const rapidjson::Value* CommonWindow::getSkillConfig(rapidjson::Document& doc, int _type, int _lv) {
	GameUtils::readJson("configs/fskill.json", doc);
	for (size_t i = 0; i < doc.Size(); i++)
	{
		const rapidjson::Value& p = doc[i];
		auto type = p["type"].GetInt();
		auto lv = p["lv"].GetInt();
		if ((type == _type) && lv == _lv) {
			return &p;
		}
	}
	return nullptr;
}

void CommonWindow::updateSkillInfo(int index) {
	std::string str_skills = UserDefault::getInstance()->getStringForKey("role_skill", "1,1,1,1");
	auto vstr = GameUtils::split(str_skills, ",");
	auto skill_lv = Value(vstr[index]).asInt();
	rapidjson::Document doc;
	auto config = getSkillConfig(doc, index + 1, skill_lv);
	if (config) {

		auto text_name = getContentPane()->getChild("n45")->as<GTextField>();
		auto loader = getContentPane()->getChild("n75")->as<GLoader>();
		auto text_lv = getContentPane()->getChild("n79")->as<GTextField>();
		auto text_des = getContentPane()->getChild("n80")->as<GTextField>();

		auto name = (*config)["name"].GetString();
		std::string icon = (*config)["icon"].GetString();
		auto level = (*config)["lv"].GetInt();
		icon.erase(icon.end() - 4, icon.end());
		text_name->setText(name);
		loader->setIcon("ui://Common/" + icon);
		text_lv->setText(Value(level).asString());

		auto desc = (*config)["desc"].GetString();
		text_des->setText(desc);

		auto cost = (*config)["cost"].GetInt();
		auto text_cost = getContentPane()->getChild("n87")->as<GTextField>();
		text_cost->setText(Value(cost).asString());
		if (PlayerManager::getInstance()->getGold() >= cost) {
			text_cost->setColor(Color3B(255,204,102));
		}
		else {
			text_cost->setColor(Color3B::RED);
		}
	}

	rapidjson::Document doc_next;
	auto config_next = getSkillConfig(doc_next, index + 1, skill_lv + 1);
	auto text_lv_next = getContentPane()->getChild("n81")->as<GTextField>();
	auto text_des_next = getContentPane()->getChild("n82");
	if (config_next) {
		auto lv = (*config_next)["lv"].GetInt();
		text_lv_next->setText(Value(lv).asString());
		auto desc = (*config_next)["desc"].GetString();
		text_des_next->setText(desc);
	}
	else {
		text_lv_next->setText(GameUtils::gbk_to_utf8("无"));
		text_des_next->setText("");
	}
}

void CommonWindow::onClickSkillUpgrade(EventContext* context) {
	std::string str_skills = UserDefault::getInstance()->getStringForKey("role_skill", "1,1,1,1");
	auto vstr = GameUtils::split(str_skills, ",");
	auto skill_lv = Value(vstr[skill_index]).asInt();
	rapidjson::Document doc;
	auto config = getSkillConfig(doc, skill_index + 1, skill_lv);
	if (config) {
		auto cost = (*config)["cost"].GetInt();
		if (PlayerManager::getInstance()->getGold() < cost) {
			Toast::getInstance()->showText(GameUtils::gbk_to_utf8("元宝不足"),Color3B::RED);
			return;
		}

		auto skill_next = skill_lv + 1;
		rapidjson::Document doc1;
		auto config_next = getSkillConfig(doc1, skill_index + 1, skill_next);
		if (config_next) {
			PlayerManager::getInstance()->setGold(PlayerManager::getInstance()->getGold() - cost);
			vstr[skill_index] = Value(skill_next).asString();
			std::string new_level;
			for (auto str : vstr)
			{
				new_level.append(str);
				new_level.append(",");
			}
			UserDefault::getInstance()->setStringForKey("role_skill", new_level);
			updateSkillInfo(skill_index);
		}
		else {
			Toast::getInstance()->showText(GameUtils::gbk_to_utf8("已满级"), Color3B::RED);
			return;
		}
	}
}

void CommonWindow::renderTaskItem(int index, GObject* obj) {
	auto skill_item = obj->as<GComponent>();
	auto task_id = task_ids.at(index);
	auto task = TaskLogic::getInstance()->getTask(task_id);
	rapidjson::Document doc;
	auto config = GameUtils::getConfig(doc, "configs/task.json", "id", task_id);
	if (config) {
		skill_item->setCustomData(Value(task_id));
		auto type = (*config)["tapy"].GetInt();
		auto text_type = skill_item->getChild("n2");
		std::string str;
		str = type == 1 ? "主线任务" : "支线任务";
		text_type->setText(GameUtils::gbk_to_utf8(str));

		auto context = (*config)["details"].GetString();
		auto text_context = skill_item->getChild("n3");
		text_context->setText(context);

		auto text_progress = skill_item->getChild("n4");
		auto str_progress = task->getProgressStr();
		text_progress->setText(str_progress);

		auto loader1 = skill_item->getChild("n15")->as<GLoader>();
		auto loader2 = skill_item->getChild("n16")->as<GLoader>();
		auto loader3 = skill_item->getChild("n17")->as<GLoader>();

		auto text_count1 = skill_item->getChild("n6");
		auto text_count2 = skill_item->getChild("n8");
		auto text_count3 = skill_item->getChild("n11");

		auto btn_get = skill_item->getChild("n13")->as<GButton>(); 
		if (task->task_state == (int)TaskState::eAccpet || task->task_state == (int)TaskState::eCondition) {
			btn_get->setVisible(true);
		}
		else {
			btn_get->setVisible(false);
		}
		btn_get->setCustomData(Value(task->task_state));

		auto award_1 = (*config)["award_1"].GetString();
		auto award_2 = (*config)["award_2"].GetString();
		std::string award_3;
		if (config->HasMember("award_3")) {
			award_3 = (*config)["award_3"].GetString();
		}
	
		auto v1 = GameUtils::split(award_1, ",");
		auto v2 = GameUtils::split(award_2, ",");
		auto v3 = GameUtils::split(award_3, ",");

		auto award_id_1 = Value(v1.at(0)).asInt();
		auto award_id_2 = Value(v2.at(0)).asInt();
		auto award_id_3 = v3.size() > 0 ? Value(v3.at(0)).asInt() : 0;

		auto award_count_1 = Value(v1.at(1)).asInt();
		auto award_count_2 = Value(v2.at(1)).asInt();
		auto award_count_3 = v3.size() > 0 ?  Value(v3.at(1)).asInt() : 0;

		if (type == 1) {
			rapidjson::Document doc1;
			auto config1 = GameUtils::getConfig(doc1, "configs/eequip.json", "id", award_id_1);
			std::string icon1 = (*config1)["icon"].GetString();
			icon1.erase(icon1.end() - 4, icon1.end());
			loader1->setIcon("ui://Common/" + icon1);
			text_count1->setText(Value(award_count_1).asString());

			rapidjson::Document doc2;
			auto config2 = GameUtils::getConfig(doc2, "configs/eequip.json", "id", award_id_2);
			std::string icon2 = (*config2)["icon"].GetString();
			icon2.erase(icon2.end() - 4, icon2.end());
			loader2->setIcon("ui://Common/" + icon2);
			text_count2->setText(Value(award_count_2).asString());

			if (award_id_3 > 0) {
				rapidjson::Document doc3;
				auto config3 = GameUtils::getConfig(doc3, "configs/eequip.json", "id", award_id_3);
				std::string icon3 = (*config3)["icon"].GetString();
				icon3.erase(icon3.end() - 4, icon3.end());
				loader3->setIcon("ui://Common/" + icon3);
				text_count3->setText(Value(award_count_3).asString());

				loader3->setVisible(true);
				text_count3->setVisible(true);
			}
			else {
				loader3->setVisible(false);
				text_count3->setVisible(false);
			}
		}
		else {
			rapidjson::Document doc1;
			auto config1 = GameUtils::getConfig(doc1, "configs/item.json", "id", award_id_1);
			std::string icon1 = (*config1)["icon"].GetString();
			icon1.erase(icon1.end() - 4, icon1.end());
			loader1->setIcon("ui://Common/" + icon1);
			text_count1->setText(Value(award_count_1).asString());

			rapidjson::Document doc2;
			auto config2 = GameUtils::getConfig(doc2, "configs/item.json", "id", award_id_2);
			std::string icon2 = (*config2)["icon"].GetString();
			icon2.erase(icon2.end() - 4, icon2.end());
			loader2->setIcon("ui://Common/" + icon2);
			text_count2->setText(Value(award_count_2).asString());

			if (award_id_3 > 0) {
				rapidjson::Document doc3;
				auto config3 = GameUtils::getConfig(doc3, "configs/item.json", "id", award_id_3);
				std::string icon3 = (*config3)["icon"].GetString();
				icon3.erase(icon3.end() - 4, icon3.end());
				loader3->setIcon("ui://Common/" + icon3);
				text_count3->setText(Value(award_count_3).asString());

				loader3->setVisible(true);
				text_count3->setVisible(true);
			}
			else {
				loader3->setVisible(false);
				text_count3->setVisible(false);
			}
		}
	}
}

void CommonWindow::onClickTaskItem(EventContext* context) {
	auto name = context->getInput()->getTarget()->name;
	if (name.compare("n13") == 0) {
		auto item = (GObject*)context->getData();
		auto task_id = item->getCustomData().asInt();
		CCLOG("click button task_id = %d", task_id);

		auto task = TaskLogic::getInstance()->getTask(task_id);
		if (task == nullptr) {
			return;
		}

		if (task->task_state == (int)TaskState::eAccpet) {
			auto task = TaskLogic::getInstance()->getTask(task_id);
			task->setTaskState((int)TaskState::eDoing);
			task->updateLogic();
			listTask->setNumItems(listTask->getNumItems());
			GameUtils::sendEvent(event_update_tasks);
		}
		else if (task->task_state == (int)TaskState::eCondition) {
			TaskLogic::getInstance()->finishTask(task_id);
			taskSelect();
		}

	}
}

void CommonWindow::renderMercItem(int index, GObject* obj) {
	auto lab = obj->as<GLabel>();
	auto loader = lab->getChild("n2")->as<GLoader>();
	auto text_count = lab->getChild("n3");
	loader->setVisible(false);
	text_count->setVisible(false);
}

void CommonWindow::update_role_power(EventCustom* data) {
	updatePower();
}