#include "CNpc.h"

CNpc* CNpc::createWith(int npc_id) {
	CNpc* npc = new(std::nothrow) CNpc(npc_id);
	if (npc && npc->init())
	{
		npc->autorelease();
		return npc;
	}
	else
	{
		delete npc;
		npc = nullptr;
		return nullptr;
	}
}

CNpc::CNpc(int npc_id):
spTitle(nullptr), lister(nullptr){
	npc_Id = npc_id;
}

CNpc::~CNpc() {

}

bool CNpc::init() {
	if (!CPlayer::init())
		return false;

	getHeroSprite()->setTexture(StringUtils::format("npc/npc%d.png", npc_Id) );
	spTitle = Sprite::create();
	spTitle->setTexture(StringUtils::format("npc/title%d.png", npc_Id));
	addChild(spTitle);
	spTitle->setPositionY(20 + getHeroSprite()->getContentSize().height / 2);
	lister = EventListenerTouchOneByOne::create();
	lister->onTouchBegan = CC_CALLBACK_2(CNpc::touchBegain, this);
	lister->onTouchEnded = CC_CALLBACK_2(CNpc::touchEnd, this);
	getHeroSprite()->getEventDispatcher()->addEventListenerWithFixedPriority(lister, 1);
	return true;
}

void CNpc::onExit()
{
	CPlayer::onExit();
	getHeroSprite()->getEventDispatcher()->removeEventListener(lister);
}

bool CNpc::touchBegain(Touch* touch, Event* event) {
	auto rect = getHeroSprite()->getBoundingBox();
	auto local_pos = convertTouchToNodeSpace(touch);
	if (rect.containsPoint(local_pos)) {
		return true;
	}
	return false;
}

void CNpc::touchEnd(Touch* touch, Event* event) {
	if (funcNpcClick) {
		funcNpcClick(npc_Id);
	}
}