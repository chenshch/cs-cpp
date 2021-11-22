#include "BaseWindow.h"
#include "FairyScene.h"

BaseWindow::BaseWindow():m_popUI(false), m_full(true){

}

BaseWindow::~BaseWindow() {
	unscheduleUpdate();
}

void BaseWindow::onInit() {
	Window::onInit();
	UIPackage::addPackage(getPackName());
	auto rootCom = UIPackage::createObject(getUIPackName(), getUIWindowName())->as<GComponent>();
	if (rootCom) {
		if (m_full) {
			rootCom->makeFullScreen();
			rootCom->addRelation(GRoot::getInstance(), RelationType::Size);
			setBringToFrontOnClick(false);
		}
		setContentPane(rootCom);
		center();
	}
}

void BaseWindow::doShowAnimation() {
	if (m_popUI) {
		setScale(0.1f, 0.1f);
		setPivot(0.5f, 0.5f, false);

		GTween::to(getScale(), Vec2::ONE, 0.3f)->setTarget(this, TweenPropType::Scale)->onComplete(CC_CALLBACK_0(BaseWindow::onShown, this));
	}
	else {
		BaseWindow::onShown();
	}
}

void BaseWindow::doHideAnimation() {
	if (m_popUI) {
		GTween::to(getScale(), Vec2(0.1f, 0.1f), 0.3f)->setTarget(this, TweenPropType::Scale)->onComplete(CC_CALLBACK_0(BaseWindow::hideImmediately, this));
	}
	else {
		BaseWindow::hideImmediately();
	}
}

void BaseWindow::onShown() {
	Window::onShown();
}

void BaseWindow::hideImmediately() {
	Window::hideImmediately();
}

void BaseWindow::addToScene() {
	auto scene = Director::getInstance()->getRunningScene();
	auto fscene = static_cast<FairyScene*>(scene);
	fscene->getRoot()->showWindow(this);
}

void BaseWindow::addToNewScene() {
	auto fscene = FairyScene::create();
	fscene->getRoot()->showWindow(this);
	Director::getInstance()->replaceScene(TransitionFade::create(0.6f,fscene));
}

void BaseWindow::update(float delta) {

}

void BaseWindow::scheduleUpdate() {
	Director::getInstance()->getScheduler()->schedule(schedule_selector(BaseWindow::_update), this, 0, false);
}

void BaseWindow::unscheduleUpdate() {
	Director::getInstance()->getScheduler()->unschedule(schedule_selector(BaseWindow::_update), this);
}

void BaseWindow::_update(float delta) {
	this->update(delta);
}