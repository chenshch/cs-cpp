#pragma once
#include "cocos2d.h"
#include "FairyGUI.h"
#include "GRoot.h"

USING_NS_FGUI;
USING_NS_CC;
class FairyScene :
	public Scene
{
public:
	CREATE_FUNC(FairyScene);
	FairyScene();
	virtual ~FairyScene();
	virtual bool init();
	GRoot* getRoot() { return _groot; }
private:
	GRoot* _groot;
};
