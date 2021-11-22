#include "ItemTips.h"
#include "GameUtils.h"

ItemTips* ItemTips::createWith(int id)
{
	ItemTips* tips = new (std::nothrow) ItemTips(id);
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
	return tips;
}

ItemTips::ItemTips():mId(0)
{

}

ItemTips::ItemTips(int id): mId(id)
{
	m_full = false;
	//m_popUI = true;
}

ItemTips::~ItemTips()
{

}

void ItemTips::onInit()
{
	BaseWindow::onInit();
	setModal(true);

	auto item_tip = getContentPane()->getChild("n0")->as<GComponent>();
	getContentPane()->addClickListener(CC_CALLBACK_1(ItemTips::rootClick, this));
	item_tip->addClickListener(CC_CALLBACK_1(ItemTips::tipClick, this));

	auto ss = getContentArea();

	if (mId > 0) {
		rapidjson::Document doc;
		auto config = GameUtils::getConfig(doc,"configs/item.json", "id", mId);
		if (config) {
			std::string icon = (*config)["icon"].GetString();
			icon.erase(icon.end() - 4, icon.end());

			auto loader = item_tip->getChild("n4")->as<GLoader>();
			loader->setIcon("ui://Common/" + icon);

			auto name = (*config)["name"].GetString();
			auto text_name = item_tip->getChild("n5")->as<GTextField>();
			text_name->setText(name);

			auto des = (*config)["describe"].GetString();
			auto text_des = item_tip->getChild("n6")->as<GTextField>();
			text_des->setText(des);
		}
	}
}

void ItemTips::rootClick(EventContext* context) {
	hide();
}

void ItemTips::tipClick(EventContext* context) {
	context->stopPropagation();
}
