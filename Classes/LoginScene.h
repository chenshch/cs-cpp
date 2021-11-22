#pragma once
#include "BaseWindow.h"
class LoginScene :
    public BaseWindow
{
public:
    const char* getPackName() {
        return "UI/Login";
    }

    const char* getUIPackName() {
        return "Login";
    }

    const char* getUIWindowName() {
        return "LoginView";
    }
    CREATE_FUNC(LoginScene);
    void onInit();
private:
    void loginClick(EventContext* context);
};

