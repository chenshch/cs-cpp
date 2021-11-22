#include "BagWindow.h"
#include "MainScene.h"

BagWindow::BagWindow() {
	m_popUI = false;
	m_full = false;
}

BagWindow::~BagWindow() {

}

void BagWindow::onInit() {
	BaseWindow::onInit();
}

void BagWindow::btnClick(EventContext* con) {
	auto guide = MainScene::create();
	guide->addToNewScene();
}