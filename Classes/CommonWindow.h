#pragma once
#include "BaseWindow.h"
#include "GameUtils.h"

class CommonWindow :
    public BaseWindow
{
public:
	CommonWindow();
	~CommonWindow();
	const char* getPackName() {
		return "UI/Common";
	}

	const char* getUIPackName() {
		return "Common";
	}

	const char* getUIWindowName() {
		return "Common";
	}
	CREATE_FUNC(CommonWindow);
	void onInit();
	void onExit();

	void setPage(int index);
private:
	void backClick(EventContext* context);
	void sortItems(EventContext* context);
	void subSelect();

	void taskSelect();
	void updateTask(int type = 0);
	void update_gold(EventCustom* event);
	void update_items(EventCustom* event);
	void update_equip_part(EventCustom* event);
	void update_equip_bag(EventCustom* event);
	void onClickShopItem(EventContext* context);
	void conformBuy(int cost, int gold);
	void renderListShopItem(int index, GObject* obj);
	void renderShopEquipItem(int index, GObject* obj);
	void renderSkillItem(int index, GObject* obj);
	void onClickSkillItem(EventContext* context);
	const rapidjson::Value* getSkillConfig(rapidjson::Document& doc, int _type, int _lv);
	void updateSkillInfo(int index);
	void onClickSkillUpgrade(EventContext* context);
	void renderTaskItem(int index, GObject* obj);
	void onClickTaskItem(EventContext* context);
	void renderMercItem(int index, GObject* obj);
	void update_role_power(EventCustom* data);
	void initBag();
	void initPartEquip();
	void initTask();
	void initSkill();
	void initMerc();

	void mercItemClick(EventContext* context,int index);
	void onClickBagItem(EventContext* context);
	void renderListItem(int index, GObject* obj);

	void onClickBagEquip(EventContext* context);
	void renderListEquip(int index, GObject* obj);
	void onClickPartEquip(EventContext* context);
	void sellAllClick(EventContext* context);
	void updateItems(int type);

	void initShop();
	void updatePower();


	GList* listEquip;
	GList* listBag;
	GList* listShop;
	GList* listSkill;
	GList* listTask;
	GList* listMerc;

	int skill_index = 0;
	std::vector<int>task_ids;
};

