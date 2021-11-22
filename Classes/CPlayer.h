#pragma once
#include "cocos2d.h"
#include "Context.h"
#include "Constant.h"
#include "FairyGUI.h"
#include "GLabel.h"
#include "GProgressBar.h"

class Context;
USING_NS_FGUI;
USING_NS_CC;
class CPlayer :
    public cocos2d::Node
{
public:
    CPlayer();
    virtual ~CPlayer();
    PlayerState getPlayerState();
    void setDir(MoveDir dir);
    void setPlayerState(PlayerState s);
    MoveDir getDir() const { return mDir; }
    void setVelocity(const cocos2d::Vec2& _velocity);
    void setSpeed(float s){walk_line_speed = s; }
	float getLineSpeed() const {return walk_line_speed;}
    float getSkewSpeed();
    void updateVelocityByDir();
    cocos2d::Vec2& getVelocity() { return velocity; }
    virtual bool init();
    void createName();
    virtual void updateAni();
    void showHit(int _hp);
    void hitEnd(float dt);
    virtual void setHp(int _hp);
    int getHp() const { return hp; }
    virtual void setHpMax(int _hp_max) { hp_max = _hp_max; }
    int getHpMax() const { return hp_max; }
    void setPlayerName(const std::string& name,const cocos2d::Color3B color);

    int getMonsterId() const { return mMonsterId; }
    void setMonsterId(int monster_id) { mMonsterId = monster_id; }

    int getUid() const { return uId; }
    void setUid(int _uid) { uId = _uid; }

    Sprite* getHeroSprite() const {return spRole;}

    void setBirthPos(const cocos2d::Vec2& pos) { pos_birth = pos; }
    cocos2d::Vec2& getBirthPos() { return pos_birth; }

    static const char* getStateStr(PlayerState s);
    static std::string getDirStr(MoveDir dir);
    static int getGid() { gId++; return gId; }

	static const float dis_atk;
    static const float dis_patrol;
    static int gId;
protected:
    int npc_Id;
private:
    Sprite* spRole;
    cocos2d::Label* labName;
    cocos2d::ProgressTimer* barHp;
    GProgressBar* hp_bar;
    Context* mContext;
    MoveDir mDir;
    cocos2d::Vec2 velocity;
    float walk_line_speed;
    int hp;
    int hp_max;
	int mMonsterId;
    int uId;
	cocos2d::Vec2 pos_birth;
};
