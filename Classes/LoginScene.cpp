#include "LoginScene.h"
#include "MainScene.h"
#include "PlayerManager.h"

static const char* pLists[] = { "boss/boss","effect/effect","merc/merc","monster1/monster1","monster2/monster2","role/role" };
void LoginScene::onInit()
{
	BaseWindow::onInit();
	UIPackage::addPackage("UI/Common");
	auto btn_login = getContentPane()->getChild("n1");
	btn_login->addClickListener(CC_CALLBACK_1(LoginScene::loginClick, this));

	for (int i = 0; i < sizeof(pLists) / sizeof(pLists[0]); i++)
	{
		cocos2d::Director::getInstance()->getTextureCache()->addImageAsync(cocos2d::StringUtils::format("frames/%s.png", pLists[i]), [=](cocos2d::Texture2D* tex) {
			cocos2d::SpriteFrameCache::getInstance()->addSpriteFramesWithFile(cocos2d::StringUtils::format("frames/%s.plist", pLists[i]));
		});
	}
}

void LoginScene::loginClick(EventContext* context)
{
	context->stopPropagation();
	PlayerManager::getInstance()->initData();
	auto main = MainScene::create();
	main->addToNewScene();
}