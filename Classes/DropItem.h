#pragma once
#include "cocos2d.h"
USING_NS_CC;
class DropItem :
    public Node
{
public:
    DropItem();
    ~DropItem();
    CREATE_FUNC(DropItem);
    bool init();
    void setItem(int id,int count = 1);
    void setEquip(int id, int up_level = 0);
	void setDropItemClickFunc(std::function<void()> func) {
		func_dropitem_click = func;
	}

    void onExit();

    int getItemId() const { return mId; }

private:
    Label* lab_name;
    Sprite* sp_item;
    EventListenerTouchOneByOne* lister;
    std::function<void()> func_dropitem_click;

	bool touchBegain(Touch* touch, Event* event);
	void touchEnd(Touch* touch, Event* event);
    int mId = 0;
};

