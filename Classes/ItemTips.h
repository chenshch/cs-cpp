#pragma once
#include "BaseWindow.h"
class ItemTips :
    public BaseWindow
{
public:
	ItemTips();
	ItemTips(int id);
	~ItemTips();

	void onInit();

	const char* getPackName() {
		return "UI/Common";
	}

	const char* getUIPackName() {
		return "Common";
	}

	const char* getUIWindowName() {
		return "ItemTips";
	}
	static ItemTips* createWith(int id);
	CREATE_FUNC(ItemTips);
private:
	void rootClick(EventContext* context);
	void tipClick(EventContext* context);
	int mId;
};

