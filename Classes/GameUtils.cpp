#include "GameUtils.h"
#include <locale>
#include <codecvt>
#if(CC_TARGET_PLATFORM == CC_PLATFORM_IOS) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
#include <iconv.h>
#endif
#include <time.h>

cocos2d::Action* GameUtils::runFrameAction(const stFramesAniData& ani_data) {
	cocos2d::Animation* animation = nullptr;
	if (ani_data.cache_name.length() > 0) {
		auto ani = cocos2d::AnimationCache::getInstance()->getAnimation(ani_data.cache_name);
		if (ani) {
			animation = ani;
		}
	}
	if (animation == nullptr) {
		animation = cocos2d::Animation::create();
		for (int i = 1; i <= ani_data.count; i++)
		{
			std::string str_name = cocos2d::StringUtils::format("%s%d.png", ani_data.pre_name.c_str(), i);
			if (ani_data.bPlist) {
				auto sp = cocos2d::SpriteFrameCache::getInstance()->getSpriteFrameByName(str_name);
				if (sp) {
					animation->addSpriteFrame(sp);
				}
			}
			else {
				animation->addSpriteFrameWithFile(str_name);
			}
		}

		if (ani_data.cache_name.length() > 0) {
			cocos2d::AnimationCache::getInstance()->addAnimation(animation, ani_data.cache_name);
		}
	}
	if (ani_data.delay > 0) {
		animation->setDelayPerUnit(ani_data.delay / ani_data.count);
	}
	animation->setLoops(ani_data.loops);
	auto vFrames = animation->getFrames();
	if (vFrames.size() > 0) {
		auto frame1 = vFrames.at(0);
		ani_data.node->setSpriteFrame(frame1->getSpriteFrame());
	}
	if (ani_data.loops == 1 && ani_data.func_end) {
		auto animate = cocos2d::Animate::create(animation);
		auto sq = cocos2d::Sequence::create(animate, cocos2d::CallFunc::create([=] {
			ani_data.func_end();
		}), nullptr);
		ani_data.node->runAction(sq);
		return sq;
	}
	else {
		auto animate = cocos2d::Animate::create(animation);
		ani_data.node->runAction(animate);
		return animate;
	}
}

void GameUtils::registEvent(const std::string& event_name, const std::function<void(cocos2d::EventCustom*)>& callback) {
	cocos2d::Director::getInstance()->getEventDispatcher()->addCustomEventListener(event_name, callback);
}

void GameUtils::registEvent(const std::string& event_name, const std::function<void(cocos2d::EventCustom*)>& callback, cocos2d::Node* node) {
	cocos2d::EventListenerCustom* listener = cocos2d::EventListenerCustom::create(event_name, callback);
	cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, node);
}

void GameUtils::removeEvent(const std::string& event_name) {
	cocos2d::Director::getInstance()->getEventDispatcher()->removeCustomEventListeners(event_name);
}

void GameUtils::removeEvent(cocos2d::Node* node) {
	cocos2d::Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(node);
}

void GameUtils::sendEvent(const std::string& event_name, void* data /* = nullptr */) {
	cocos2d::Director::getInstance()->getEventDispatcher()->dispatchCustomEvent(event_name, data);
}

void GameUtils::readJson(const std::string& file_json, rapidjson::Document& doc) {
	auto str_file_context = cocos2d::FileUtils::getInstance()->getStringFromFile(file_json);
	doc.Parse<0>(str_file_context.c_str());
	if (!doc.IsObject() && !doc.IsArray()) {
		CC_ASSERT("json error");
	}
}

/*去除空格换行*/
void GameUtils::trim(std::string& str, bool left /* = true */, bool right /* = true */)
{
	static const std::string delims = " \t\r";
	if (right)
		str.erase(str.find_last_not_of(delims) + 1); // trim right
	if (left)
		str.erase(0, str.find_first_not_of(delims)); // trim left
}

std::vector<std::string> GameUtils::split(const std::string& s, const std::string& delim)
{
	std::vector<std::string> v;
	size_t start = 0;
	size_t end = s.find(delim);
	while (std::string::npos != end) {
		v.push_back(s.substr(start, end - start));
		start = end + delim.size();
		end = s.find(delim, start);
	}
	if (start != s.length())
		v.push_back(s.substr(start));
	return v;
}

std::string GameUtils::gbk_to_utf8(const std::string& str)
{
#if(CC_TARGET_PLATFORM == CC_PLATFORM_IOS) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
	const char* _Source = str.c_str();
	size_t _Ssize = str.size() + 1;
	char* src = new char[_Ssize];
	strcpy(src, _Source);
	size_t _Dsize = 3 * str.size() + 1;
	char* _Dest = new char[_Dsize];
	memset(_Dest, 0, _Dsize);
	GameUtils::code_convert_iconv("gb2312", "utf-8", src, _Ssize, _Dest, _Dsize);
	std::string str_ret = _Dest;
	delete[] _Dest;
	delete[] src;
	return str_ret;
#else
	std::wstring w_str = GameUtils::string2wstring(str);
	return GameUtils::wstring2utf8string(w_str);
#endif
}

std::string GameUtils::utf8_to_gbk(const std::string& str)
{
#if(CC_TARGET_PLATFORM == CC_PLATFORM_IOS) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
	const char* _Source = str.c_str();
	size_t slen = str.size() + 1;
	char* src = new char[slen];
	strcpy(src, _Source);
	size_t _Dsize = str.size() + 1;
	char* _Dest = new char[_Dsize];
	memset(_Dest, 0, _Dsize);
	GameUtils::code_convert_iconv("utf-8", "gb2312", src, slen, _Dest, _Dsize);
	std::string str_ret = _Dest;
	delete[] _Dest;
	delete[] src;
	return str_ret;
#else
	std::wstring w_str = GameUtils::utf8string2wstring(str);
	return GameUtils::wstring2string(w_str);
#endif
}

std::string GameUtils::wstring2utf8string(const std::wstring& str)
{
	static std::wstring_convert<std::codecvt_utf8<wchar_t> > strCnv;
	return strCnv.to_bytes(str);
}

std::wstring GameUtils::utf8string2wstring(const std::string& str)
{
	static std::wstring_convert< std::codecvt_utf8<wchar_t> > strCnv;
	return strCnv.from_bytes(str);
}

//locale = "chs"或"zh-cn"
std::string GameUtils::wstring2string(const std::wstring& str, const std::string& locale /* = "chs" */)
{
#if(CC_TARGET_PLATFORM == CC_PLATFORM_IOS) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
	return "";
#else
	typedef std::codecvt_byname<wchar_t, char, std::mbstate_t> F;
	static std::wstring_convert<F> strCnv(new F(locale));
	return strCnv.to_bytes(str);
#endif
}

std::wstring GameUtils::string2wstring(const std::string& str, const std::string& locale /* = "chs" */)
{
#if(CC_TARGET_PLATFORM == CC_PLATFORM_IOS) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
	return L"";
#else
	typedef std::codecvt_byname<wchar_t, char, std::mbstate_t> F;
	static std::wstring_convert<F> strCnv(new F(locale));
	return strCnv.from_bytes(str);
#endif
}

void GameUtils::fadeScene(float _duration) {
	cocos2d::LayerColor* l = cocos2d::LayerColor::create(cocos2d::Color4B(0, 0, 0, 255));
	auto s = cocos2d::Director::getInstance()->getRunningScene();
	s->addChild(l, 999, "kSceneFade");
	cocos2d::Node* f = s->getChildByName("kSceneFade");
	auto sq = cocos2d::Sequence::create
	(
		cocos2d::FadeIn::create(_duration / 2),
		cocos2d::FadeOut::create(_duration / 2),
		cocos2d::CallFunc::create([=] {
		s->removeChildByName("kSceneFade");
	}), nullptr);
	f->runAction(sq);
}

MoveDir GameUtils::getFGuiDir(const cocos2d::Vec2& pos1, const cocos2d::Vec2& pos2) {
	auto vDir = pos2 - pos1;
	auto rd = atan2f(vDir.y,vDir.x);
	float angle = CC_RADIANS_TO_DEGREES(rd);
	if (angle >= -22.5f && angle < 22.5f) {
		return MoveDir::eRight;
	}
	else if (angle >= 22.5f && angle < 67.5f) {
		return MoveDir::eRight_Down;
	}
	else if (angle >= 67.5 && angle < 112.5f) {
		return MoveDir::eDown;
	}
	else if (angle >= 112.5f && angle < 157.5f) {
		return MoveDir::eLeft_Down;
	}
	else if (angle >= -157.5f && angle < -112.5f) {
		return MoveDir::eLeft_Up;
	}
	else if (angle >= -112.5f && angle < -67.5f) {
		return MoveDir::eUp;
	}
	else if (angle >= -67.5f && angle < -22.5f) {
		return MoveDir::eRight_Up;
	}
	else if (angle >= 157.5f || angle < -157.5f) {
		return MoveDir::eLeft;
	}
	return MoveDir::eUnkown;
}

MoveDir GameUtils::getCocosDir(const cocos2d::Vec2& pos1, const cocos2d::Vec2& pos2) {
	auto vDir = pos2 - pos1;
	auto rd = atan2f(vDir.y, vDir.x);
	float angle = CC_RADIANS_TO_DEGREES(rd);
	if (angle > -22.5f && angle <= 22.5f) {
		return MoveDir::eRight;
	}
	else if (angle > 22.5f && angle <= 67.5f) {
		return MoveDir::eRight_Up;
	}
	else if (angle > 67.5f && angle <= 112.5f) {
		return MoveDir::eUp;
	}
	else if (angle > 112.5f && angle <= 157.5f) {
		return MoveDir::eLeft_Up;
	}
	else if (angle > 157.5f || angle < -157.5f) {
		return MoveDir::eLeft;
	}
	else if (angle <= -22.5f && angle > -67.5f) {
		return MoveDir::eRight_Down;
	}
	else if (angle <= -67.5f && angle > -112.5f) {
		return MoveDir::eDown;
	}
	else if (angle <= -112.5f && angle > -157.5f) {
		return MoveDir::eLeft_Down;
	}
	return MoveDir::eUnkown;
}

int GameUtils::getFramesCountInPlist(const std::string& plist, const std::string& pre_name) {
	int count = 0;
	cocos2d::ValueMap dict = cocos2d::FileUtils::getInstance()->getValueMapFromFile(plist);
	cocos2d::ValueMap& framesDict = dict["frames"].asValueMap();
	for (auto iter = framesDict.begin(); iter != framesDict.end(); ++iter)
	{
		std::string spriteFrameName = iter->first;
		if (spriteFrameName.find(pre_name) != std::string::npos) {
			count++;
		}
	}
	return count;
}

cocos2d::Color4B GameUtils::getTextColorByQulity(int qulity) {
	switch (qulity)
	{
	case 1:
		return cocos2d::Color4B::WHITE;
		break;
	case 2:
		return cocos2d::Color4B::GREEN;
		break;
	case 3:
		return cocos2d::Color4B::YELLOW;
		break;
	case 4:
		return cocos2d::Color4B::ORANGE;
		break;
	default:
		break;
	}
	return cocos2d::Color4B::WHITE;
}

int GameUtils::code_convert_iconv(char* from_charset, char* to_charset, char* inbuf, size_t inlen, char* outbuf, size_t outlen) {
#if(CC_TARGET_PLATFORM == CC_PLATFORM_IOS) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
	iconv_t cd;
	char** pin = &inbuf;
	char** pout = &outbuf;
	cd = iconv_open(to_charset, from_charset);
	if (cd == 0) return -1;
	memset(outbuf, 0, outlen);
	if (iconv(cd, pin, &inlen, pout, &outlen) == -1)
		return -1;
	iconv_close(cd);
#endif
	return 0;
}

const rapidjson::Value* GameUtils::getConfig(rapidjson::Document& doc, const std::string& file_name, const std::string& key, const std::string& value) {
	GameUtils::readJson(file_name, doc);
	const rapidjson::SizeType len = doc.Size();
	for (rapidjson::SizeType i = 0; i < len; i++)
	{
		const rapidjson::Value& p = doc[i];
		if (p.HasMember(key.c_str())) {
			const rapidjson::Value& key_value = p[key.c_str()];
			if (key_value.IsString() && value.compare(key_value.GetString()) == 0) {
				return &p;
			}
		}
	}
	return nullptr;
}

const rapidjson::Value* GameUtils::getConfig(rapidjson::Document& doc, const std::string& file_name, const std::string& key, const int value) {
	GameUtils::readJson(file_name, doc);
	const rapidjson::SizeType len = doc.Size();
	for (rapidjson::SizeType i = 0; i < len; i++)
	{
		const rapidjson::Value& p = doc[i];
		if (p.HasMember(key.c_str())) {
			const rapidjson::Value& key_value = p[key.c_str()];
			if (key_value.IsInt() && key_value == value) {
				return &p;
			}
		}
	}
	return nullptr;
}

void GameUtils::popUI(cocos2d::Node* node) {
	node->setScale(0);
	auto* popupActions = cocos2d::Sequence::create(
		cocos2d::ScaleTo::create(0.2f, 1.05f),
		cocos2d::ScaleTo::create(0.1f, 0.95f),
		cocos2d::ScaleTo::create(0.1f, 1.0f), nullptr);
	node->runAction(popupActions);
}

bool GameUtils::isToday(time_t t)
{
	time_t time_cur = time(nullptr);
	struct tm* cur_date = localtime(&time_cur);
	auto cur_year = cur_date->tm_year;
	auto cur_mon = cur_date->tm_mon;
	auto cur_day = cur_date->tm_mday;
	struct tm* t_data = localtime(&t);
	if (cur_year == t_data->tm_year &&
		cur_mon == t_data->tm_mon &&
		cur_day == t_data->tm_mday) {
		return true;
	}
	return false;
}