#pragma once
#include "CPlayer.h"
class CNpc :
    public CPlayer
{
public:
    CNpc(int npc_id);
    ~CNpc();
    static CNpc* createWith(int npc_id);
    bool init();

    void onExit();

    bool touchBegain(Touch* touch, Event* event);
    void touchEnd(Touch* touch, Event* event);

    void setNpcClickFunc(std::function<void(int npc_id)> func) {
        funcNpcClick = func;
    }

private:
    Sprite* spTitle;
    std::function<void(int)> funcNpcClick;
    EventListenerTouchOneByOne* lister;
};

