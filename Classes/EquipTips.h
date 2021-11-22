#pragma once
#include "BaseWindow.h"
#include "Constant.h"
class EquipTips :
    public BaseWindow
{
public:
    EquipTips();
	EquipTips(const tEquip& _equip_part, const tEquip& _equip_bag);
    ~EquipTips();

	void onInit();

	const char* getPackName() {
		return "UI/Common";
	}

	const char* getUIPackName() {
		return "Common";
	}

	const char* getUIWindowName() {
		return "EquipTips";
	}
	CREATE_FUNC(EquipTips);
	static EquipTips* createWith(const tEquip& equip_part, const tEquip& equip_bag);
private:
	void takeoffClick(EventContext* context);
	void equipClick(EventContext* context);
	void sellClick(EventContext* context);
	void rootClick(EventContext* context);
	void tipsClick(EventContext* context);
	tEquip equip_part;
	tEquip equip_bag;
};

