#pragma once
#include "BaseWindow.h"
class BagScene :
    public BaseWindow
{
public:
	const char* getPackName() {
		return "UI/Bag";
	}

	const char* getUIPackName() {
		return "Bag";
	}

	const char* getUIWindowName() {
		return "Main";
	}
	CREATE_FUNC(BagScene);
	void onInit();
private:
	void btnClick(EventContext* con);
};

