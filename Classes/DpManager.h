#pragma once
#include <functional>
#include "CSingleton.h"
class DpManager :
    public CSingleton<DpManager>
{
public:
    DpManager();
    ~DpManager();

    void did_cq(int _cost, int _gold,const std::function<void ()>& cb_sucess, const std::function<void()>& cb_faild);
    void cq_finish(bool sucess,int _cost = 0,int _gold = 0);
private:
    int gold;
    int cost;

    std::function<void()> sucess_callback;
    std::function<void()> faild_callback;
};

