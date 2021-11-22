#include "DpManager.h"
#include "PlayerManager.h"
#include "GameUtils.h"
#include "Toast.h"

DpManager::DpManager()
{

}

DpManager::~DpManager()
{

}

void DpManager::did_cq(int _cost, int _gold,const std::function<void ()>& cb_sucess, const std::function<void()>& cb_faild)
{
	gold = _gold;
	cost = _cost;

	sucess_callback = cb_sucess;
	faild_callback = cb_faild;
}

void DpManager::cq_finish(bool sucess,int _cost /* = 0 */,int _gold /* = 0 */)
{
	if (sucess) {
		if ((_cost == 0 || cost == _cost) && (_gold == 0 || gold == _gold)) {
			if (sucess_callback) {
				sucess_callback();
			}

			PlayerManager::getInstance()->setGold(PlayerManager::getInstance()->getGold() + gold);
			GameUtils::sendEvent(event_update_gold);
			Toast::getInstance()->showText(GameUtils::gbk_to_utf8("充值成功"));
		}
	}
	else {
		if (faild_callback) {
			faild_callback();
		}
		Toast::getInstance()->showText(GameUtils::gbk_to_utf8("充值失败"));
	}
}
