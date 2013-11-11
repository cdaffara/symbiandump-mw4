#include "GridView.h"
#include "ScreenSize.h"
#include <qgridlayout.h>
#include <qpushbutton.h>
#include <qdebug.h>

GridView::GridView()
{

}
GridView::~GridView()
{

}
void GridView::CreateGridView(QStringList aIconPath, QStringList names)
{
	qDebug()<<"Inside GridView::CreateGridView()";
	qDebug()<<"aIconPath count = "<<aIconPath.count();
	qDebug()<<"names count = "<<names.count();

    int row;
    int col;
    QWidget* widget = new QWidget();
    QGridLayout* GridLayout = new QGridLayout(widget);
    if((6 == aIconPath.count()) || (5 == aIconPath.count()))
    {
        row = 3;
        col = 2;
    }
    else if((4 == aIconPath.count()) || (3 == aIconPath.count()))
    {
        row = col = 2;
    }
    else if((2 == aIconPath.count()) || (1 == aIconPath.count()))
    {
        row = 1;
        col = 2;
    }
    else if(0 == aIconPath.count())
    {
        row = col = 0;
    }

    int rowSpace = (ScreenSize::GetScreenRect().height() - (KHeadNameHeight+ToolbarIconHeight+2*KWidgetGapFactor))/row;
    int colSpace = (ScreenSize::GetScreenRect().width() - 2*KWidgetGapFactor )/col;

    int index = 0;
    for(int i=0;i<row;i++)
    {
        for(int j=0;j<col;j++)
        {
        QPushButton *pushBtn = new QPushButton();
        qDebug()<<"name = "<<names[index];
        //pushBtn->
        pushBtn->setGeometry(i*colSpace,(j*rowSpace + KWidgetGapFactor),colSpace,rowSpace);
        pushBtn->setIconSize(QSize(colSpace,colSpace));
        qDebug()<<"icon = "<<aIconPath[index];
        pushBtn->setIcon(QIcon(aIconPath[index]));
        GridLayout->addWidget(pushBtn,i,j);
        pushBtn->show();
        
        index++;
        if(index == names.count())
        	{
        	qDebug()<<"Count became same so exiting";
			break;
        	}
        }
	if(index == names.count())
		{
		qDebug()<<"Count became same so exiting";
		break;
		}
    }
	//return widget;

}
