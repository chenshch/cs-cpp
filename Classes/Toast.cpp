#include "Toast.h"

USING_NS_CC;
Toast::Toast()
{
	pool = new GObjectPool();
}

Toast::~Toast()
{

}

void Toast::showText(const std::string& str, const cocos2d::Color3B& color /* = cocos2d::Color3B::RED */)
{
	auto obj = createFromPool();
	GRoot::getInstance()->addChild(obj);
	obj->center();

	auto text = obj->getChild("n0")->as<GTextField>();
	text->setText(str);
	text->setColor(color);

	auto trans = obj->getTransition("t0");
	trans->play([=]() {
		obj->removeFromParent();
		vPool.push_back(obj);
	});
}

GComponent* Toast::createFromPool() {
	GComponent* obj = nullptr;
	if (vPool.size() > 0) {
		obj = vPool.at(vPool.size() - 1);
		vPool.pop_back();
	}
	else {
		obj = UIPackage::createObject("Main", "toast")->as<GComponent>();
		obj->retain();
	}
	return obj;
}
