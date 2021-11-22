#include "TaskLogic.h"
#include "GameUtils.h"
#include "cocos2d.h"
#include "Constant.h"
#include "PlayerManager.h"
USING_NS_CC;

TaskLogic::TaskLogic()
{

}

TaskLogic::~TaskLogic()
{

}

void TaskLogic::initTask()
{
	auto main_id = getMainTask();
	if (main_id > 0) {
		auto task = addTask(main_id);
		task->task_type = 1;
		auto main_task_state = UserDefault::getInstance()->getIntegerForKey("main_task_state", 0);
		task->task_state = main_task_state;
	}
	rapidjson::Document doc;
	GameUtils::readJson("configs/task.json", doc);
	for (size_t i = 0; i < doc.Size(); i++)
	{
		const rapidjson::Value& p = doc[i];
		auto type = p["tapy"].GetInt();
		auto id = p["id"].GetInt();
		auto task_state = UserDefault::getInstance()->getIntegerForKey(StringUtils::format("sub_task_%d", id).c_str(), 0);
		if (type == 2 && task_state != (int)TaskState::eFinish) {
			auto task = addTask(id);
			task->task_type = type;
			task->task_state = task_state;
		}
	}
}

int TaskLogic::getMainTask()
{
	int main_task = 0;
	auto mainTask = UserDefault::getInstance()->getIntegerForKey("main_task_id", 1);
	rapidjson::Document doc;
	GameUtils::readJson("configs/task.json", doc);
	for (size_t i = 0; i < doc.Size(); i++)
	{
		const rapidjson::Value& p = doc[i];
		auto type = p["tapy"].GetInt();
		auto id = p["id"].GetInt();
		if (type == 1 && mainTask <= id) {
			main_task = id;
			break;
		}
	}
	return main_task;
}

void TaskLogic::updateLogic(int monster_id /* = 0 */)
{
	for (auto it = mapTasks.begin(); it != mapTasks.end(); ++it)
	{
		CTask* task = it->second;
		task->updateLogic(monster_id);
	}
}

CTask* TaskLogic::getTask(int id)
{
	auto it = mapTasks.find(id);
	if (it != mapTasks.end()) {
		return it->second;
	}
	return nullptr;
}

CTask* TaskLogic::addTask(int id)
{
	CTask* task = new CTask(id);
	task->task_state = (int)TaskState::eAccpet;
	mapTasks.insert(std::make_pair(id, task));
	return task;
}

void TaskLogic::delTask(int id)
{
	for (auto it = mapTasks.begin(); it != mapTasks.end(); ++it)
	{
		if (it->first == id) {
			delete it->second;
			mapTasks.erase(it);
			break;
		}
	}
}

void TaskLogic::finishTask(int id)
{
	auto task = getTask(id);
	if (task == nullptr)
		return;
	if (task->task_type == 1) {
	
		UserDefault::getInstance()->setIntegerForKey("main_task_id", task->task_id + 1);
		delTask(task->task_id);

		auto task_next = getMainTask();
		if (task_next > 0) {
			auto task_new = addTask(task_next);
			task_new->task_type = 1;
			task_new->setTaskState((int)TaskState::eAccpet);
		}
	}
	else if (task->task_type == 2) {
		task->setTaskState((int)TaskState::eFinish);
		delTask(task->task_id);
	}

	rapidjson::Document dc;
	auto t_config = GameUtils::getConfig(dc, "configs/task.json", "id", id);
	if (t_config) {
		std::string str1 = (*t_config)["award_1"].GetString();
		std::string str2 = (*t_config)["award_2"].GetString();

		std::string str3;
		if (t_config->HasMember("award_3")) {
			str3 = (*t_config)["award_3"].GetString();
		}
			
		if (!str1.empty()) {
			addTaskAward(str1, task->task_type == 1);
		}

		if (!str2.empty()) {
			addTaskAward(str2, task->task_type == 1);
		}

		if (!str3.empty()) {
			addTaskAward(str3, task->task_type == 1);
		}
	}
}

void TaskLogic::addTaskAward(const std::string& str_award,bool mTask /*= false*/)
{
	auto v1 = GameUtils::split(str_award, ",");
	auto id = Value(v1[0]).asInt();
	auto count = Value(v1[1]).asInt();

	if (mTask) {
		PlayerManager::getInstance()->addEquip(id);
	}
	else {
		PlayerManager::getInstance()->addItem(id,count);
	}
}
