#pragma once

#include "Constant.h"
#include "json/rapidjson.h"
#include "json/document.h"
#include "EventConst.h"

class GameUtils{
public: 
	static cocos2d::Action* runFrameAction(const stFramesAniData& ani_data);
	static void registEvent(const std::string& event_name, const std::function<void(cocos2d::EventCustom*)>& callback);
	static void registEvent(const std::string& event_name, const std::function<void(cocos2d::EventCustom*)>& callback,cocos2d::Node* node);
	static void removeEvent(const std::string& event_name);
	static void removeEvent(cocos2d::Node* node);
	static void sendEvent(const std::string& event_name, void* data = nullptr);
	static void readJson(const std::string& file_json, rapidjson::Document& doc);
	static void trim(std::string& str, bool left = true, bool right = true);
	static std::vector<std::string> split(const std::string& s, const std::string& delim);
	static std::string gbk_to_utf8(const std::string& str);
	static std::string utf8_to_gbk(const std::string& str);
	static std::string wstring2utf8string(const std::wstring& str);
	static std::wstring utf8string2wstring(const std::string& str);
	static std::string wstring2string(const std::wstring& str, const std::string& locale = "chs");
	static std::wstring string2wstring(const std::string& str, const std::string& locale = "chs");
	static void fadeScene(float _duration = 0.5);
	static MoveDir getFGuiDir(const cocos2d::Vec2& pos1, const cocos2d::Vec2& pos2);
	static MoveDir getCocosDir(const cocos2d::Vec2& pos1, const cocos2d::Vec2& pos2);
	static int getFramesCountInPlist(const std::string& plist, const std::string& pre_name);
	static cocos2d::Color4B getTextColorByQulity(int qulity);
    static int code_convert_iconv(char *from_charset,char *to_charset, char *inbuf,size_t inlen,char *outbuf,size_t outlen);
	static const rapidjson::Value* getConfig(rapidjson::Document& doc,const std::string& file_name, const std::string& key, const std::string& value);
	static const rapidjson::Value* getConfig(rapidjson::Document& doc, const std::string& file_name, const std::string& key, const int value);
	static void popUI(cocos2d::Node* node);
	static bool isToday(time_t t);
};

