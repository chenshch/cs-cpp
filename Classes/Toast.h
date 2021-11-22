#pragma once
#include "CSingleton.h"
#include "cocos2d.h"
#include "GObjectPool.h"
#include "FairyGui.h"

USING_NS_FGUI;
class Toast : public CSingleton<Toast>
{
public:
	Toast();
	~Toast();
	void showText(const std::string& str, const cocos2d::Color3B& color = cocos2d::Color3B::RED);
private:
	GComponent* createFromPool();
	GObjectPool* pool;
	std::vector<GComponent*>vPool;
};

