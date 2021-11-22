#include "MercTips.h"
#include "GameUtils.h"

MercTips::MercTips()
{

}

MercTips::~MercTips()
{

}

void MercTips::onInit()
{
	setModal(true);
	BaseWindow::onInit();

	getContentPane()->addClickListener(CC_CALLBACK_1(MercTips::rootClick, this));
	auto tip = getContentPane()->getChild("n0")->as<GComponent>();
	tip->addClickListener(CC_CALLBACK_1(MercTips::tipClick, this));
	auto btn_levle = tip->getChild("n14")->as<GButton>();
	auto btn_fight = tip->getChild("n13")->as<GButton>();

	btn_levle->addClickListener(CC_CALLBACK_1(MercTips::levelClick, this));
	btn_fight->addClickListener(CC_CALLBACK_1(MercTips::fightClick, this));

	auto merc_hide = UserDefault::getInstance()->getBoolForKey("merc_hide", false);
	if (merc_hide) {
		btn_fight->setVisible(true);
		btn_levle->setVisible(false);
	}
	else {
		btn_levle->setVisible(true);
		btn_fight->setVisible(false);
	}
}

void MercTips::levelClick(EventContext* context) {
	UserDefault::getInstance()->setBoolForKey("merc_hide", true);
	GameUtils::sendEvent(event_merc_hide);
	hide();
}

void MercTips::fightClick(EventContext* context) {
	UserDefault::getInstance()->setBoolForKey("merc_hide", false);
	GameUtils::sendEvent(event_merc_hide);
	hide();
}

void MercTips::rootClick(EventContext* context) {
	hide();
}

void MercTips::tipClick(EventContext* context) {
	context->stopPropagation();
}