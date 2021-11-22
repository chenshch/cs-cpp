#pragma once
#include "CSingleton.h"
#include "Constant.h"
class PlayerManager :
    public CSingleton<PlayerManager>
{
public:
    PlayerManager();
    ~PlayerManager();

    void initData();
    int getLevel();
    void setLevel(int lv);
    int getGold();
    void setGold(int _gold);
    void setName(const char* sz_name);
    std::string getName();

    void setExp(int _exp);
    int getExp();
    void addExp(int _add);

    int getHp();
    void setHp(int _hp);

    int getHpMax();
    void setHpMax(int _hp_max);

    int getItemCount(int id);
    void addItem(int id, int count = 1);
    void updateItems();
    bool checkItemEnough(int id, int count = 1);

    void addEquip(int id, int _lv = 0,int gid = 0);

    void removeEquip(int gid);
    bool hasEquip(int gid);
    std::vector<tItem>& getItems() { return items; }
    std::vector<tItem> getItemsByType(int _type);

    std::vector<tEquip>& getEquips() { return equips; }
    void updateEquips();
    tEquip getPartEquip(int part);
    void takeOffEquips(int part);
    void swapEquip(int gid);
    void updatePartEquips();
    void sortEquip();
    void sortItem(int type);
    static int getEquipPart(int id);
private:
    int level;
    std::string name;
    int hp;
    int max_hp;
    int exp;

    std::vector<tItem> items;
    int gold;
    std::vector<tEquip>equips;
    tEquip partEquips[(int)EquipPart::eMax];  //´©´÷µÄ×°±¸
};

