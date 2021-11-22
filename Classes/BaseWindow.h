#pragma once
#include "FairyGUI.h"

USING_NS_FGUI;
USING_NS_CC;
class BaseWindow : 
	public fairygui::Window
{
public:
	BaseWindow();
	virtual ~BaseWindow();
protected:
	virtual const char* getPackName() { return ""; }
	virtual const char* getUIPackName() { return ""; }
	virtual const char* getUIWindowName() { return ""; }
	virtual void onInit();
	virtual void doShowAnimation();
	virtual void doHideAnimation();
	virtual void onShown();
public:
	virtual void hideImmediately();

	void addToScene();
	void addToNewScene();
	void scheduleUpdate();
	void unscheduleUpdate();
	virtual void update(float delta);
private:
	void _update(float delta);
protected:
	bool m_popUI;
	bool m_full;
};

