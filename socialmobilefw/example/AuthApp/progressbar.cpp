#include "progressbar.h"

progressbar::progressbar(QWidget *parent)
    : QWidget(parent)
{
	ui.setupUi(this);
	ui.progressBar->setTextVisible(false);
}


progressbar::~progressbar()
{

}
