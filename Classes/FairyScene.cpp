#include "FairyScene.h"

FairyScene::FairyScene() : _groot(nullptr) {

}

FairyScene::~FairyScene() {
	CC_SAFE_RELEASE(_groot);
}

bool FairyScene::init()
{
	if (!Scene::init())
	{
		return false;
	}

	_groot = GRoot::create(this);
	_groot->retain();
	return true;
}