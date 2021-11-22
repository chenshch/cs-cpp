#pragma once
#include "BaseWindow.h"

class BagWindow :
	public BaseWindow
{
public:
	BagWindow();
	~BagWindow();
	CREATE_FUNC(BagWindow);
public:
	const char* getPackName() {
		return "UI/Bag";
	}

	const char* getUIPackName() {
		return "Bag";
	}

	const char* getUIWindowName() {
		return "BagWin";
	}

	void onInit();
private:
	void btnClick(EventContext* con);
};

