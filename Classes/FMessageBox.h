#pragma once
#include "BaseWindow.h"

struct FMessData 
{
	std::string strContext;
	std::function<void()> func_ok;
	std::function<void()> func_cancel;
};

class FMessageBox :
    public BaseWindow
{
public:
    FMessageBox();
    ~FMessageBox();
	const char* getPackName() {
		return "UI/Common";
	}

	const char* getUIPackName() {
		return "Common";
	}

	const char* getUIWindowName() {
		return "messagebox";
	}
    void onInit();
	void setOkHandler(const std::function<void()>& ok);
	void setCancelHandler(const std::function<void()>& cancel);
	void setMsgString(const std::string& str);
	CREATE_FUNC(FMessageBox);
	static FMessageBox* createWith(const FMessData& data);
private:
	void okClick(EventContext* context);
	void cancelClick(EventContext* context);

	std::function<void()> ok_handler;
	std::function<void()> cancel_handler;
};

