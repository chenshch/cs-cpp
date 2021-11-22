#include "ReliveWindow.h"
#include "GameUtils.h"

ReliveWindow::ReliveWindow()
{

}

ReliveWindow::~ReliveWindow()
{

}

void ReliveWindow::onInit()
{
	BaseWindow::onInit();
	setModal(true);

	auto btn = getContentPane()->getChild("n7");
	btn->addClickListener(CC_CALLBACK_1(ReliveWindow::reliveClick,this));
}

void ReliveWindow::reliveClick(EventContext* context) {
	hide();
	GameUtils::sendEvent(event_battle_role_relive);
}
