#pragma once
#include <string>

class CTask{
public:
	CTask();
	CTask(int _id);
	~CTask();

	void updateLogic(int monster_id = 0);
	void setTaskState(int state);
	std::string getProgressStr();

	int task_id = 0;
	int task_type = 0;
	int task_state = 0;
private:
	int monster_kill = 0;
	int equip_up_times = 0;
	int progress = 0;
	int totol_progress = 0;
};

