#pragma once
#include "BaseWindow.h"
class ReliveWindow :
    public BaseWindow
{
public:
    ReliveWindow();
    ~ReliveWindow();
    CREATE_FUNC(ReliveWindow);

	const char* getPackName() {
		return "UI/Main";
	}

	const char* getUIPackName() {
		return "Main";
	}

	const char* getUIWindowName() {
		return "dlg_relive";
	}
	void onInit();
private:
	void reliveClick(EventContext* context);
};

