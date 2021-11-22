#include "MainScene.h"
#include "Constant.h"
#include "GameUtils.h"
#include <stdio.h>
#include "CommonWindow.h"
#include "PlayerManager.h"
#include "CNpc.h"
#include "ChatWindow.h"
#include "FairyScene.h"
#include "Toast.h"
#include "TaskLogic.h"

MainScene::MainScene() :r_roystick_bg(0.0f), map_layer(nullptr)
{

}

MainScene::~MainScene()
{

}

void MainScene::onInit()
{
	BaseWindow::onInit();

	map_layer = MapLayer::create();
	displayObject()->addChild(map_layer, -1);

	auto btn_atk = getContentPane()->getChild("n2");
	if (btn_atk) {
		btn_atk->addClickListener(CC_CALLBACK_1(MainScene::atkClick, this));
	}

	auto add_hp = getContentPane()->getChild("n22");
	if (add_hp) {
		add_hp->addClickListener(CC_CALLBACK_1(MainScene::addHpClick, this));
	}

	auto btn_add = getContentPane()->getChild("n32");
	if (btn_add) {
		btn_add->addClickListener(CC_CALLBACK_1(MainScene::addGoldClick, this));
	}

	for (int i = 0; i < 3; i++)
	{
		auto btn_skill = getContentPane()->getChild(cocos2d::StringUtils::format("n%d", 19 + i));
		if (btn_skill) {
			btn_skill->addClickListener(CC_CALLBACK_1(MainScene::skillClick, this, i));
		}
	}

	roystick_bg = getContentPane()->getChild("n0");
	r_roystick_bg = roystick_bg->getWidth() / 2;
	roystick = getContentPane()->getChild("n1");
	pos_roystick = roystick->getPosition();

	roystick->addEventListener(UIEventType::TouchBegin, CC_CALLBACK_1(MainScene::joystickBegin, this));
	roystick->addEventListener(UIEventType::TouchMove, CC_CALLBACK_1(MainScene::joystickMove, this));
	roystick->addEventListener(UIEventType::TouchEnd, CC_CALLBACK_1(MainScene::joystickEnd, this));

	auto btn_role_icon = getContentPane()->getChild("n23");
	if (btn_role_icon)
		btn_role_icon->addClickListener(CC_CALLBACK_1(MainScene::iconClick, this));

	setBringToFrontOnClick(false);

	scheduleUpdate();
	auto node = displayObject();
	GameUtils::registEvent(event_update_role_hp, CC_CALLBACK_1(MainScene::update_role_hp,this), node);
	GameUtils::registEvent(event_update_role_lv, CC_CALLBACK_1(MainScene::updateLevel, this), node);
	GameUtils::registEvent(event_update_items, CC_CALLBACK_1(MainScene::update_items, this), node);
	GameUtils::registEvent(event_update_tasks, CC_CALLBACK_1(MainScene::update_tasks, this), node);
	GameUtils::registEvent(event_update_gold, CC_CALLBACK_1(MainScene::update_gold, this), node);

	updateLevel(nullptr);
	update_role_hp(nullptr);
	updateHpCount();
	update_tasks(nullptr);
	update_gold(nullptr);
}

void MainScene::onExit()
{
	BaseWindow::onExit();
	GameUtils::removeEvent(displayObject());
}

void MainScene::atkClick(EventContext* con) {
	con->stopPropagation();
	if (map_layer->getRole()->getPlayerState() == PlayerState::eAtk || map_layer->getRole()->getPlayerState() == PlayerState::eDeath)
		return;
	map_layer->getRole()->setPlayerState(PlayerState::eAtk);
}

void MainScene::iconClick(EventContext* con) {
	con->stopPropagation();

	CommonWindow* window = CommonWindow::create();
	window->show();
}

void MainScene::skillClick(EventContext* con, int index) {
	con->stopPropagation();
	if (map_layer->getRole()->getPlayerState() == PlayerState::eAtk || map_layer->getRole()->getPlayerState() == PlayerState::eDeath)
		return;
	map_layer->getRole()->setSkillId(index + 1);
	map_layer->getRole()->setPlayerState(PlayerState::eAtk);
}

void MainScene::addHpClick(EventContext* context) {
	if (!PlayerManager::getInstance()->checkItemEnough(3)) {

		Toast::getInstance()->showText(GameUtils::gbk_to_utf8("µÀ¾ß²»×ã"), Color3B::RED);
		return;
	}
	map_layer->getRole()->addHp(100);
	PlayerManager::getInstance()->addItem(3, -1);
}

void MainScene::addGoldClick(EventContext* context) {
	CommonWindow* window = CommonWindow::create();
	window->show();
	window->setPage(4);
}

void MainScene::joystickBegin(EventContext* con) {

}

void MainScene::joystickMove(EventContext* con) {
	if (map_layer == nullptr || map_layer->getRole() == nullptr || map_layer->getRole()->getPlayerState() == PlayerState::eDeath) {
		return;
	}
	auto pos = con->getInput()->getPosition();

	if (!pos.isZero()) {
		map_layer->getRole()->setPlayerState(PlayerState::eMove);
	}

	auto detal = pos - pos_roystick;
	auto a = atanf(detal.y / detal.x);
	auto s = std::abs(detal.y / sinf(a));
	if (s > r_roystick_bg) {
		auto x = cosf(a) * r_roystick_bg;
		auto y = sinf(a) * r_roystick_bg;
		if (detal.x < 0) {
			x = -x;
			y = -y;
		}
		roystick->setPosition(pos_roystick.x + x, pos_roystick.y + y);
	}
	else {
		roystick->setPosition(pos.x, pos.y);
	}

	auto dir = GameUtils::getFGuiDir(pos_roystick, pos);
	map_layer->getRole()->setDir(dir);
}

void MainScene::joystickEnd(EventContext* con) {
	roystick->setPosition(pos_roystick.x, pos_roystick.y);
	map_layer->getRole()->setPlayerState(PlayerState::eWait);
}

void MainScene::updateLevel(EventCustom* event)
{
	auto lab_level = getContentPane()->getChild("n16");
	auto level = PlayerManager::getInstance()->getLevel();
	auto name = PlayerManager::getInstance()->getName();
	auto str = cocos2d::StringUtils::format("%s Lv%d", name.c_str(), level);
	lab_level->setText(GameUtils::gbk_to_utf8(str));
}

void MainScene::update(float delta) {

}

void MainScene::update_role_hp(EventCustom* event)
{
	auto hp = PlayerManager::getInstance()->getHp();
	auto hp_max = PlayerManager::getInstance()->getHpMax();
	float value = (float)hp / hp_max * 100.0f;
	auto hp_bar = getContentPane()->getChild("n13")->as<GProgressBar>();
	hp_bar->tweenValue(value,0.2f);
}

void MainScene::update_items(EventCustom* event) {
	long id = (long)event->getUserData();
	if (id == 3 || id == 0) {
		updateHpCount();
	}
}

void MainScene::update_tasks(EventCustom* event)
{
	std::string str;
	auto tasks = TaskLogic::getInstance()->getTasks();
	for (auto it : tasks)
	{
		auto task = it.second;
		if (task->task_state == int(TaskState::eDoing)) {
			rapidjson::Document doc;
			auto config = GameUtils::getConfig(doc, "configs/task.json", "id", task->task_id);
			if (config) {
				std::string task_name = (*config)["details"].GetString();
				if (!str.empty()) {
					str.append("\n");
				}
				str.append(task_name);
			}
		}
	}

	getContentPane()->getChild("n26")->setText(str);

	getContentPane()->getChild("n26")->addClickListener([=](EventContext* contect) {
		auto common = CommonWindow::create();
		common->show();
		common->setPage(2);
	});
}

void MainScene::updateHpCount() {
	auto count = PlayerManager::getInstance()->getItemCount(3);
	auto text_count = getContentPane()->getChild("n25");
	text_count->setText(cocos2d::StringUtils::format("%d", count));
}

void MainScene::update_gold(EventCustom* event) {
	auto text = getContentPane()->getChild("n31")->as<GTextField>();
	text->setText(Value(PlayerManager::getInstance()->getGold()).asString());
}