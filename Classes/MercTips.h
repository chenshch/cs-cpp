#pragma once
#include "BaseWindow.h"
class MercTips :
    public BaseWindow
{
public:
    MercTips();
    ~MercTips();
	const char* getPackName() {
		return "UI/Common";
	}

	const char* getUIPackName() {
		return "Common";
	}

	const char* getUIWindowName() {
		return "MercTips";
	}
	void onInit();
	CREATE_FUNC(MercTips);

private:
	void levelClick(EventContext* context);
	void fightClick(EventContext* context);
	void rootClick(EventContext* context);
	void tipClick(EventContext* context);
};

