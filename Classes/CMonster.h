#pragma once
#include "CPlayer.h"
class CMonster :
    public CPlayer
{
public:
    CMonster(int monsterId);
    ~CMonster();
    static CMonster* createWith(int monsterId);
    void updateAni();
    bool init();
    void updatePos(const cocos2d::Vec2& pos_hero, const cocos2d::Vec2& pos_next,float delta);
    bool isBoss();
    float get_atk_dis() const { return atk_dis; }
    void atk();
private:
    float atk_dis;
    float atk_delay;
};

