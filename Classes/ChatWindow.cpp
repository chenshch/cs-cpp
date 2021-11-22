#include "ChatWindow.h"
#include "QhWindow.h"
#include "CommonWindow.h"

std::string ChatWindow::chat_contexts[] = { "", "我这里有很多宝贝哦", "完成任务可获取奖励","您要找我强化装备吗" };

ChatWindow* ChatWindow::createWith(int _npcId)
{
	auto chat = new (std::nothrow) ChatWindow();
	chat->setNpc(_npcId);
	if (chat->init())
	{
		chat->autorelease();
		return chat;
	}
	else {
		delete chat;
		chat = nullptr;
		return nullptr;
	}
	return chat;
}

ChatWindow::ChatWindow():npcId(0){

}

ChatWindow::~ChatWindow() {

}

void ChatWindow::setNpc(int _npcId)
{
	npcId = _npcId;
}

void ChatWindow::onInit() {
	BaseWindow::onInit();
	setModal(true);
	getContentPane()->getChild("n6")->addClickListener(CC_CALLBACK_1(ChatWindow::qhEvent,this));

	setCloseButton(getContentPane()->getChild("n5"));

	auto text_context = getContentPane()->getChild("n7");
	auto npc_icon = getContentPane()->getChild("n8");
	auto npc_title = getContentPane()->getChild("n9");

	npc_icon->setIcon(StringUtils::format("ui://Common/npc%d",npcId));
	npc_title->setIcon(StringUtils::format("ui://Common/npc_title%d", npcId));

	if (npcId > 0) {
		text_context->setText(GameUtils::gbk_to_utf8(chat_contexts[npcId]));
	}
}

void ChatWindow::qhEvent(EventContext* context) {
	if (npcId == 1) {
		CommonWindow* window = CommonWindow::create();
		window->show();
		window->setPage(4);
	}
	else if (npcId == 2) {
		CommonWindow* window = CommonWindow::create();
		window->show();
		window->setPage(2);
	}
	else if (npcId == 3) {
		auto qh = QhWindow::create();
		qh->show();
	}
}