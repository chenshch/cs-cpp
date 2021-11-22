#pragma once
#include "BaseWindow.h"
class ChatWindow :
    public BaseWindow
{
public:
	const char* getPackName() {
		return "UI/Common";
	}

	const char* getUIPackName() {
		return "Common";
	}

	const char* getUIWindowName() {
		return "ChatWin";
	}

	void setNpc(int _npcId);
	CREATE_FUNC(ChatWindow);
	static ChatWindow* createWith(int _npcId);
private:
	ChatWindow();
	~ChatWindow();
	void onInit();
	void qhEvent(EventContext* context);
	int npcId;

	static std::string  chat_contexts[];
};

