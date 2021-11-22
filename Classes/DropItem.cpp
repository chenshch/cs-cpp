#include "DropItem.h"
#include "GameUtils.h"

DropItem::DropItem():lister(nullptr)
{

}

DropItem::~DropItem()
{

}

bool DropItem::init()
{
	if (!Node::init())
		return false;

	lab_name = Label::create();
	lab_name->setPosition(0.0f, 60.0f);
	lab_name->setSystemFontSize(22);
	lab_name->setTextColor(Color4B::GREEN);
	lab_name->setHorizontalAlignment(TextHAlignment::CENTER);
	addChild(lab_name);
	sp_item = Sprite::create();
	addChild(sp_item);

	float des_x = 0.0f;
	if (rand_0_1() > 0.5f) {
		des_x = 150.0f;
	}
	else {
		des_x = -150.0f;
	}

	ccBezierConfig config;
	config.controlPoint_1 = Vec2(des_x / 3, 100);
	config.controlPoint_2 = Vec2(des_x / 2, 100);
	config.endPosition = Vec2(des_x, 0);
	auto bezier = BezierBy::create(0.5f, config);
	runAction(bezier);

	sp_item->runAction(RotateBy::create(0.5f, 360));

	lister = EventListenerTouchOneByOne::create();
	lister->setSwallowTouches(true);
	lister->onTouchBegan = CC_CALLBACK_2(DropItem::touchBegain, this);
	lister->onTouchEnded = CC_CALLBACK_2(DropItem::touchEnd, this);


	//getEventDispatcher()->addEventListenerWithSceneGraphPriority(lister, sp_item);

	sp_item->getEventDispatcher()->addEventListenerWithFixedPriority(lister, 1);

	return true;
}

void DropItem::setItem(int id,int count /* = 1 */)
{
	mId = id;
	rapidjson::Document doc;
	auto config = GameUtils::getConfig(doc, "configs/item.json", "id", id);
	if (config) {
		std::string icon = (*config)["icon"].GetString();
		std::string name = (*config)["name"].GetString();

		sp_item->setTexture("drop/" + icon);
		lab_name->setString(name + StringUtils::format("x%d", count));
	}
}

void DropItem::setEquip(int id, int up_level /*= 0*/)
{
	mId = id;
	rapidjson::Document doc;
	auto config = GameUtils::getConfig(doc, "configs/eequip.json", "id", id);
	if (config) {
		std::string icon = (*config)["icon"].GetString();
		std::string name = (*config)["name"].GetString();
		auto lv = (*config)["lv"].GetInt();

		sp_item->setTexture("drop/" + icon);
		lab_name->setString(name + StringUtils::format("+%d", lv));
	}
}

void DropItem::onExit()
{
	Node::onExit();
	sp_item->getEventDispatcher()->removeEventListener(lister);
}

bool DropItem::touchBegain(Touch* touch, Event* event)
{
	if (sp_item != nullptr) {
		auto rect = sp_item->getBoundingBox();
		auto local_pos = convertTouchToNodeSpace(touch);
		if (rect.containsPoint(local_pos)) {
			return true;
		}
	}
	return false;
}

void DropItem::touchEnd(Touch* touch, Event* event)
{
	if (func_dropitem_click) {
		func_dropitem_click();
	}
}
