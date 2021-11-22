#pragma once
#include "CPlayer.h"
class CRole :
    public CPlayer
{
public:
    CRole();
    ~CRole();
    void updateAni();
    void updateFollow(const cocos2d::Vec2& map_pos);
    bool init();
    bool init(bool main);
    void setHp(int _hp);
    void setHpMax(int _hp_max);
    void addHp(int add);
    cocos2d::Sprite* getSkillSprite() { return skill_effect; }
	void setSkillId(int _sid) {
        skillId = _sid;
	}


    void setMain(bool b) {
        bMain = b;
    }

    bool  isMain() const {
        return bMain;
    }
    CRole* getFollowTarget() const {
        return followTarget;
    }
    void setFollowTarget(CRole* role);
    cocos2d::Vec2 getFollowBackPos();
    cocos2d::Vec2 getSkillPos();
    static const int queue_max_frame_length;
    static const float follow_near;
    struct FollowData
    {
        cocos2d::Point pos;
        cocos2d::Point map_pos;
        MoveDir dir = MoveDir::eUnkown;
        PlayerState state = PlayerState::eWait;
    };
   FollowData getNextPathRecord();
	static CRole* createRole(bool bmain);
	CREATE_FUNC(CRole);
private:
    cocos2d::DrawNode* drawNode;
    cocos2d::Sprite* skill_effect;
    int skillId;
    bool bMain;
    CRole* followTarget;
    std::list<FollowData> captainPathQueue;
};

