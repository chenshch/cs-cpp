#include "BagScene.h"
#include "BagWindow.h"

void BagScene::onInit() {
	BaseWindow::onInit();

	UIPackage::addPackage("UI/Basics");

	auto btn = getContentPane()->getChild("bagBtn");
	btn->addClickListener(CC_CALLBACK_1(BagScene::btnClick, this));
}

void BagScene::btnClick(EventContext* con) {
	auto bag = BagWindow::create();
	bag->addToScene();
}