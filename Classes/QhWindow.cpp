#include "QhWindow.h"
#include "PlayerManager.h"
#include "GameUtils.h"
#include "Toast.h"
#include "TaskLogic.h"
#include "EquipTips.h"

QhWindow::QhWindow():listEquip(nullptr){
	m_full = false;
	m_popUI = true;
}

QhWindow::~QhWindow() {

}

void QhWindow::onInit() {
	BaseWindow::onInit();
	setModal(true);
	auto btn_close = getContentPane()->getChild("n9");
	setCloseButton(btn_close);

	auto btn_qh = getContentPane()->getChild("n1");
	btn_qh->addClickListener(CC_CALLBACK_1(QhWindow::onQhClick, this));

	listEquip = getContentPane()->getChild("n8")->as<GList>();
	auto equips = PlayerManager::getInstance()->getEquips();
	listEquip->addEventListener(UIEventType::ClickItem, CC_CALLBACK_1(QhWindow::onClickBagEquip, this));
	listEquip->itemRenderer = CC_CALLBACK_2(QhWindow::renderListEquip, this);
	size_t MIN_BAG_GRID = 48;
	listEquip->setNumItems(std::max(equips.size(), (size_t)MIN_BAG_GRID));

	getContentPane()->getChild("n5")->addClickListener(CC_CALLBACK_1(QhWindow::onEquipClick, this));
}

void QhWindow::setEquip(tEquip _equip)
{
	equip = _equip;
}

void QhWindow::updateInfo() {
	auto text1 = getContentPane()->getChild("n2")->as<GTextField>();
	auto text2 = getContentPane()->getChild("n3")->as<GTextField>();

	rapidjson::Document doc;
	auto config = GameUtils::getConfig(doc, "configs/eequip.json", "id", equip.id);
	if (config) {
		std::string icon = (*config)["icon"].GetString();
		icon.erase(icon.end() - 4, icon.end());
		auto loader = getContentPane()->getChild("n5")->as<GLoader>();
		loader->setIcon("ui://Common/" + icon);

		auto count1 = (*config)["stone_1"].GetInt();
		auto count2 = (*config)["gold"].GetInt();

		auto count = PlayerManager::getInstance()->getItemCount(4);
		auto gold = PlayerManager::getInstance()->getGold();
		text1->setText(StringUtils::format("%d/%d", count, count1));
		text2->setText(StringUtils::format("%d/%d", gold, count2));

		if (count >= count1) {
			text1->setColor(Color3B::WHITE);
		}
		else {
			text1->setColor(Color3B::RED);
		}

		if (gold >= count2) {
			text2->setColor(Color3B::WHITE);
		}
		else {
			text2->setColor(Color3B::RED);
		}

		text1->setVisible(true);
		text2->setVisible(true);
	}
	else {
		text1->setVisible(false);
		text2->setVisible(false);
	}
}

void QhWindow::renderListEquip(int index, GObject* obj)
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
			gload->setGrayed(equip.gid == item.gid);
			rapidjson::Document doc;
			auto config = GameUtils::getConfig(doc, "configs/eequip.json", "id", item.id);
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

void QhWindow::onClickBagEquip(EventContext* context)
{
	auto lab = (GComponent*)context->getData();
	auto index = lab->getCustomData().asInt();
	auto equips = PlayerManager::getInstance()->getEquips();
	if ((size_t)index < equips.size()) {
		auto eq = equips.at(index);
		setEquip(eq);
		listEquip->setNumItems(listEquip->getNumItems());
	}
	else {
		equip.clear();
	}
	updateInfo();
}

void QhWindow::onQhClick(EventContext* context) {
	if (equip.isNull()) {
		Toast::getInstance()->showText(GameUtils::gbk_to_utf8("请放入装备"),Color3B::RED);
		return;
	}

	if (!PlayerManager::getInstance()->hasEquip(equip.gid)) {
		Toast::getInstance()->showText(GameUtils::gbk_to_utf8("装备已不存在"), Color3B::RED);
		return;
	}

	rapidjson::Document doc;
	auto config_next = GameUtils::getConfig(doc, "configs/eequip.json", "id", equip.id + 1);
	if (config_next == nullptr) {
		Toast::getInstance()->showText(GameUtils::gbk_to_utf8("已达到满级"), Color3B::RED);
		return;
	}

	auto config = GameUtils::getConfig(doc, "configs/eequip.json", "id", equip.id);
	auto count1 = (*config)["stone_1"].GetInt();
	auto count2 = (*config)["gold"].GetInt();

	auto count = PlayerManager::getInstance()->getItemCount(4);
	auto gold = PlayerManager::getInstance()->getGold();

	if (count < count1 || gold < count2) {
		Toast::getInstance()->showText(GameUtils::gbk_to_utf8("材料不足"), Color3B::RED);
		return;
	}

	auto& equips = PlayerManager::getInstance()->getEquips();
	for (auto& eq : equips)
	{
		if (eq.gid == equip.gid) {
			PlayerManager::getInstance()->addItem(4, -count1);
			PlayerManager::getInstance()->setGold(PlayerManager::getInstance()->getGold() - count2);
			eq.up_lv = eq.up_lv + 1;
			eq.id = eq.id + 1;
			Toast::getInstance()->showText(GameUtils::gbk_to_utf8("强化成功"), Color3B::GREEN);
			equip = eq;
			PlayerManager::getInstance()->updateEquips();
			TaskLogic::getInstance()->updateLogic(100);
			break;
		}
	}

	updateInfo();
}

void QhWindow::onEquipClick(EventContext* context) {
	if (!equip.isNull()) {
		tEquip eq_part = PlayerManager::getInstance()->getPartEquip(PlayerManager::getEquipPart(equip.id));
		auto tips = EquipTips::createWith(eq_part,equip);
		tips->show();
	}
}