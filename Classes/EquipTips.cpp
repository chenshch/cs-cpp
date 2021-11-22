#include "EquipTips.h"
#include "GameUtils.h"
#include "PlayerManager.h"
#include "Toast.h"

EquipTips* EquipTips::createWith(const tEquip& equip_part, const tEquip& equip_bag)
{
	auto tips = new (std::nothrow) EquipTips(equip_part, equip_bag);
	if (tips && tips->init())
	{
		tips->autorelease();
		return tips;
	}
	else
	{
		delete tips;
		tips = nullptr;
		return nullptr;
	}
}

EquipTips::EquipTips()
{

}

EquipTips::EquipTips(const tEquip& _equip_part, const tEquip& _equip_bag)
{
	equip_part = _equip_part;
	equip_bag = _equip_bag;

	m_full = false;
	//m_popUI = true;
}

EquipTips::~EquipTips()
{

}

void EquipTips::onInit()
{
	BaseWindow::onInit();
	setModal(true);

	getContentPane()->addClickListener(CC_CALLBACK_1(EquipTips::rootClick, this));
	auto port_tips = getContentPane()->getChild("n14")->as<GComponent>();
	auto bag_tips = getContentPane()->getChild("n15")->as<GComponent>();

	port_tips->addClickListener(CC_CALLBACK_1(EquipTips::tipsClick, this));
	bag_tips->addClickListener(CC_CALLBACK_1(EquipTips::tipsClick, this));

	if (equip_part.id <= 0) {
		port_tips->setVisible(false);
	}
	else {
		rapidjson::Document doc;
		auto config = GameUtils::getConfig(doc,"configs/eequip.json", "id", equip_part.id);
		if (config) {
			std::string icon = (*config)["icon"].GetString();
			auto loader = port_tips->getChild("n4")->as<GLoader>();
			icon.erase(icon.end() - 4, icon.end());
			loader->setIcon("ui://Common/" + icon);

			auto text_name = port_tips->getChild("n5")->as<GTextField>();
			auto text_prop = port_tips->getChild("n6")->as<GTextField>();

			std::string name = (*config)["name"].GetString();
			if (equip_part.up_lv > 0) {
				name = name + StringUtils::format("+%d", equip_part.up_lv);
			}
			name = name + GameUtils::gbk_to_utf8("£¨ÒÑ×°±¸£©");
			text_name->setText(name);

			std::string str_prop;
			int atk = (*config)["atk"].GetInt();
			if (atk > 0)
				str_prop.append(StringUtils::format("¹¥»÷Á¦+%d\n", atk));
			int hp = (*config)["hp"].GetInt();
			if (hp > 0)
				str_prop.append(StringUtils::format("ÉúÃü+%d\n", hp));
			int def = (*config)["def"].GetInt();
			if (def > 0)
				str_prop.append(StringUtils::format("·ÀÓù+%d\n", def));
			float crit = (*config)["crit"].GetFloat();
			if (crit > 0.0f)
				str_prop.append(StringUtils::format("±©»÷+%0.3f", crit));
			float doge = (*config)["doge"].GetFloat();
			if (doge > 0.0f)
				str_prop.append(StringUtils::format("ÉÁ±Ü+%0.3f\n", doge));
			float phy = (*config)["phy"].GetFloat();
			if (phy > 0.0f)
				str_prop.append(StringUtils::format("±©»÷ÉËº¦+%0.3f\n", phy));
			float pow = (*config)["pow"].GetFloat();
			if (pow > 0.0f)
				str_prop.append(StringUtils::format("ÉËº¦¼õÃâ+%0.3f\n", pow));

			text_prop->setText(GameUtils::gbk_to_utf8(str_prop));


			auto btn_take_off = port_tips->getChild("n7")->as<GButton>();
			auto type_int = (*config)["type_int"].GetInt();
			btn_take_off->setCustomData(Value(type_int));
			btn_take_off->addClickListener(CC_CALLBACK_1(EquipTips::takeoffClick, this));

		}
	}

	if (equip_bag.id <= 0) {
		bag_tips->setVisible(false);
	}
	else {
		rapidjson::Document doc;
		auto config = GameUtils::getConfig(doc,"configs/eequip.json", "id", equip_bag.id);
		if (config) {
			std::string icon = (*config)["icon"].GetString();
			auto loader = bag_tips->getChild("n4")->as<GLoader>();
			icon.erase(icon.end() - 4, icon.end());
			loader->setIcon("ui://Common/" + icon);

			auto text_name = bag_tips->getChild("n5")->as<GTextField>();
			auto text_prop = bag_tips->getChild("n6")->as<GTextField>();

			std::string name = (*config)["name"].GetString();
			if (equip_bag.up_lv > 0) {
				name = name + StringUtils::format("+%d", equip_bag.up_lv);
			}
			text_name->setText(name);

			std::string str_prop;
			int atk = (*config)["atk"].GetInt();
			if (atk > 0)
				str_prop.append(StringUtils::format("¹¥»÷Á¦+%d\n", atk));
			int hp = (*config)["hp"].GetInt();
			if (hp > 0)
				str_prop.append(StringUtils::format("ÉúÃü+%d\n", hp));
			int def = (*config)["def"].GetInt();
			if (def > 0)
				str_prop.append(StringUtils::format("·ÀÓù+%d\n", def));
			float crit = (*config)["crit"].GetFloat();
			if (crit > 0.0f)
				str_prop.append(StringUtils::format("±©»÷+%0.3f", crit));
			float doge = (*config)["doge"].GetFloat();
			if (doge > 0.0f)
				str_prop.append(StringUtils::format("ÉÁ±Ü+%0.3f\n", doge));
			float phy = (*config)["phy"].GetFloat();
			if (phy > 0.0f)
				str_prop.append(StringUtils::format("±©»÷ÉËº¦+%0.3f\n", phy));
			float pow = (*config)["pow"].GetFloat();
			if (pow > 0.0f)
				str_prop.append(StringUtils::format("ÉËº¦¼õÃâ+%0.3f\n", pow));

			text_prop->setText(GameUtils::gbk_to_utf8(str_prop));

			auto btn_equip = bag_tips->getChild("n8")->as<GButton>();
			btn_equip->addClickListener(CC_CALLBACK_1(EquipTips::equipClick, this));

			auto btn_sell = bag_tips->getChild("n9")->as<GButton>();
			auto sell_price = (*config)["sell"].GetInt();
			btn_sell->setCustomData(Value(sell_price));
			btn_sell->addClickListener(CC_CALLBACK_1(EquipTips::sellClick, this));
		}
	}
}

void EquipTips::takeoffClick(EventContext* context) {
	context->stopPropagation();
	auto btn = (GButton*)context->getSender();
	auto type_int = btn->getCustomData().asInt();
	PlayerManager::getInstance()->takeOffEquips(type_int);
	GameUtils::sendEvent(event_update_role_power);
	hide();
}

void EquipTips::equipClick(EventContext* context) {
	context->stopPropagation();
	PlayerManager::getInstance()->swapEquip(equip_bag.gid);
	Toast::getInstance()->showText(GameUtils::gbk_to_utf8("×°±¸³É¹¦"),Color3B::GREEN);
	GameUtils::sendEvent(event_update_role_power);
	hide();
}

void EquipTips::sellClick(EventContext* context) {
	context->stopPropagation();
	if (PlayerManager::getInstance()->hasEquip(equip_bag.gid)) {
		auto btn = (GButton*)context->getSender();
		auto sell_price = btn->getCustomData().asInt();
		PlayerManager::getInstance()->removeEquip(equip_bag.gid);
		PlayerManager::getInstance()->setGold(PlayerManager::getInstance()->getGold() + sell_price);
		GameUtils::sendEvent(event_update_equip_bag);
		Toast::getInstance()->showText(GameUtils::gbk_to_utf8("³öÊÛ³É¹¦"), Color3B::GREEN);
	}
	hide();
}


void EquipTips::rootClick(EventContext* context) {
	hide();
}

void EquipTips::tipsClick(EventContext* context) {
	context->stopPropagation();
}