#pragma once
#include "CSingleton.h"
#include "CTask.h"
#include <map>

class TaskLogic :
    public CSingleton<TaskLogic>
{
public:
    TaskLogic();
    ~TaskLogic();
private:
    std::map<int, CTask*> mapTasks;

public:
    void initTask();
    int getMainTask();
    void updateLogic(int monster_id = 0);

    std::map<int, CTask*> getTasks() { return mapTasks; }

    CTask* getTask(int id);

    CTask* addTask(int id);
    void delTask(int id);

    void finishTask(int id);

    void addTaskAward(const std::string& str_award,bool mTask = false);
};

