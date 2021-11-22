#pragma once
#include "BaseWindow.h"
#include "CRole.h"
#include "CMonster.h"
#include "MapLayer.h"

class MainScene :
    public BaseWindow
{
public:
	const char* getPackName() {
		return "UI/Main";
	}

	const char* getUIPackName() {
		return "Main";
	}

	const char* getUIWindowName() {
		return "Main";
	}
	CREATE_FUNC(MainScene);
protected:
	void onShow();
private:
	MainScene();
	~MainScene();
	void onInit();
	void onExit();
	void atkClick(EventContext* con);
	void iconClick(EventContext* con);
	void skillClick(EventContext* con, int index);
	void addHpClick(EventContext* context);
	void addGoldClick(EventContext* context);
	void joystickBegin(EventContext* con);
	void joystickMove(EventContext* con);
	void joystickEnd(EventContext* con);

	virtual void update(float delta);

	void update_role_hp(EventCustom* event);

	void update_items(EventCustom* event);
	void update_tasks(EventCustom* event);
	void updateHpCount();
	void update_gold(EventCustom* event);
	bool isCollide(cocos2d::Vec2& pos);
	void updateMapPos(const cocos2d::Vec2& targetPos);
	void createRole();

	void loadMap();
	void updateLevel(EventCustom* event);
	GObject* roystick_bg;
	GObject* roystick;
	Vec2 pos_roystick;
	float r_roystick_bg;
	MapLayer* map_layer;
};

