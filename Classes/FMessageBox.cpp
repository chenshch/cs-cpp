#include "FMessageBox.h"

FMessageBox* FMessageBox::createWith(const FMessData& data)
{
	auto box = new (std::nothrow) FMessageBox();
	if (box && box->init()) {

	}
	return box;
}

FMessageBox::FMessageBox()
{
	m_full = false;
	m_popUI = true;
}

FMessageBox::~FMessageBox()
{

}

void FMessageBox::onInit()
{
	BaseWindow::onInit();
	setModal(true);
	auto ok = getContentPane()->getChild("n3");
	auto cancel = getContentPane()->getChild("n2");

	ok->addClickListener(CC_CALLBACK_1(FMessageBox::okClick, this));
	cancel->addClickListener(CC_CALLBACK_1(FMessageBox::cancelClick, this));
}

void FMessageBox::setOkHandler(const std::function<void()>& ok)
{
	ok_handler = ok;
}

void FMessageBox::setCancelHandler(const std::function<void()>& cancel)
{
	cancel_handler = cancel;
}

void FMessageBox::setMsgString(const std::string& str)
{
	auto text = getContentPane()->getChild("n1")->as<GTextField>();
	text->setText(str);
}

void FMessageBox::okClick(EventContext* context) {
	if (ok_handler) {
		ok_handler();
	}
	hide();
}

void FMessageBox::cancelClick(EventContext* context) {
	if (cancel_handler) {
		cancel_handler();
	}
	hide();
}
