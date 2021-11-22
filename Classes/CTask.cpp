#include "CTask.h"
#include "PlayerManager.h"
#include "cocos2d.h"

USING_NS_CC;
CTask::CTask()
{

}

CTask::CTask(int _id)
{
	task_id = _id;
	if (task_id == 1) {
		totol_progress = 5;
	}
	else if (task_id == 2) {
		totol_progress = 10;
	}
	else if (task_id == 3) {
		totol_progress = 15;
	}
	else if (task_id == 4) {
		totol_progress = 20;
	}
	else if (task_id == 5) {
		totol_progress = 10;
	}
	else if (task_id == 6) {
		totol_progress = 10;
	}
	else if (task_id == 7) {
		totol_progress = 1;
	}
	else if (task_id == 8) {
		totol_progress = 3;
	}
}

CTask::~CTask()
{

}

void CTask::updateLogic(int monster_id /* = 0 */)
{
	if (task_state == (int)TaskState::eDoing) {
		bool active = false;
		if (task_id == 1) {
			if (PlayerManager::getInstance()->getLevel() >= 5) {
				setTaskState((int)TaskState::eCondition);
				active = true;
			}
		}
		else if (task_id == 2) {
			if (PlayerManager::getInstance()->getLevel() >= 10) {
				setTaskState((int)TaskState::eCondition);
				active = true;
			}
		}
		else if (task_id == 3) {
			if (PlayerManager::getInstance()->getLevel() >= 15) {
				setTaskState((int)TaskState::eCondition);
				active = true;
			}
		}
		else if (task_id == 4) {
			if (PlayerManager::getInstance()->getLevel() >= 20) {
				setTaskState((int)TaskState::eCondition);
				active = true;
			}
		}
		else if (task_id == 5) {
			if (monster_id == 1) {
				monster_kill++;
				if (monster_kill >= 10) {
					setTaskState((int)TaskState::eCondition);
					active = true;
				}
			}
		}
		else if (task_id == 6) {
			if (monster_id == 2) {
				monster_kill++;
				if (monster_kill >= 10) {
					setTaskState((int)TaskState::eCondition);
					active = true;
				}
			}
		}
		else if (task_id == 7) {
			if (monster_id == 3) {
				monster_kill++;
				if (monster_kill >= 1) {
					setTaskState((int)TaskState::eCondition);
					active = true;
				}
			}
		}
		else if (task_id == 8) {
			if (monster_id == 100) {
				equip_up_times++;
				if (equip_up_times >= 3) {
					setTaskState((int)TaskState::eCondition);
					active = true;
				}
			}
		}
	}
}

void CTask::setTaskState(int state)
{
	task_state = state;
	if (task_type == 1) {
		UserDefault::getInstance()->setIntegerForKey("main_task_state", state);
	}
	else if (task_type == 2) {
		UserDefault::getInstance()->setIntegerForKey(StringUtils::format("sub_task_%d", task_id).c_str(), state);
	}
}

std::string CTask::getProgressStr()
{
	std::string str;
	if (task_id > 0 && task_id <= 4) {
		str = StringUtils::format("%d//%d", PlayerManager::getInstance()->getLevel(), totol_progress);
	}
	else if (task_id >= 5 && task_id <= 7) {
		str = StringUtils::format("%d//%d", monster_kill, totol_progress);
	}
	else if (task_id == 8) {
		str = StringUtils::format("%d//%d", equip_up_times, totol_progress);
	}
	return str;
}
