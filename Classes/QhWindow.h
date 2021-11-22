#pragma once
#include "BaseWindow.h"
#include "Constant.h"
class QhWindow :
    public BaseWindow
{
public:
	const char* getPackName() {
		return "UI/Common";
	}

	const char* getUIPackName() {
		return "Common";
	}

	const char* getUIWindowName() {
		return "QhWin";
	}
	CREATE_FUNC(QhWindow);
private:
	QhWindow();
	~QhWindow();
	void onInit();
	void setEquip(tEquip _equip);
	void updateInfo();

	void renderListEquip(int index, GObject* obj);
	void onClickBagEquip(EventContext* context);
	void onQhClick(EventContext* context);
	void onEquipClick(EventContext* context);
	tEquip equip;
	GList* listEquip;
};

