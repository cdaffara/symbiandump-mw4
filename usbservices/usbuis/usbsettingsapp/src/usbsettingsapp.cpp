/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/
#include "usbsettingsapp.h"
#include <hblistviewitem.h>
#include <QItemSelectionModel>
#include <hbstyleloader.h>
#include <hbdocumentloader.h>
#include <hbview.h>
#include <hblistview.h>
#include <hbgroupbox.h>
#include "usbuisettingmodel.h"
#include "usbdebug.h"

#define DOCML ":/xml/usbsettingsapp.docml"

USBSettingsApp::USBSettingsApp(QWidget *parent)
    : HbMainWindow(parent), mView(NULL), mListViewItem(NULL)
{
    myDebug() << ">>> USBSettingsApp::USBSettingsApp";
    //Init the document loader and we load the view
    mModel = new UsbUiSettingModel(this);
    mSelectionModel = new QItemSelectionModel(mModel);
    mDocumentLoader = new HbDocumentLoader();
    connect(this, SIGNAL(orientationChanged(Qt::Orientation)),
    					this,SLOT(setOrientation(Qt::Orientation)));
    load(orientation());
    myDebug() << "<<< USBSettingsApp::USBSettingsApp";
}

USBSettingsApp::~USBSettingsApp()
{
    myDebug() << ">>> USBSettingsApp::~USBSettingsApp";
    delete mView;
    delete mModel;
    delete mDocumentLoader;
    myDebug() << "<<< USBSettingsApp::~USBSettingsApp";
}
/*!
loads the DOCML and build the view by finding the widgets from it 
*/
void USBSettingsApp::load(Qt::Orientation orientation) 
{
    myDebug() << ">>> USBSettingsApp::load";
    bool loaded = true;
 
   //Sets loader to initial state
    mDocumentLoader->reset();
    //Load the common section
    mDocumentLoader->load(DOCML, &loaded);
   
    //Loads the view
    if ( loaded ) {
        myDebug() << "    USBSettingsApp::load DOCML loaded";
        QGraphicsWidget *widget = mDocumentLoader->findWidget(QString("view"));
        if (widget) {
            myDebug() << "    USBSettingsApp::load view found";
            mView = qobject_cast<HbView*>(widget);
            this->addView(mView);
            this->setCurrentView(mView);
        }
        QGraphicsWidget *listViewWidget = mDocumentLoader->findWidget(QString("listView"));
        if (listViewWidget) {
            myDebug() << "    USBSettingsApp::load listView found";
            HbListView *listView = qobject_cast<HbListView*>(listViewWidget);
            listView->setItemPixmapCacheEnabled(true);
            listView->setModel(mModel);
            listView->setSelectionMode(HbAbstractItemView::SingleSelection);
            listView->setSelectionModel(mSelectionModel);
            mModel -> setSelectionModel(mSelectionModel);
            HbStyleLoader::registerFilePath(":/");
            listView->setLayoutName("usb");
            //scroll to the selected index, if not hidden personality
            QModelIndexList indexList = mSelectionModel->selectedIndexes();
            if ( !indexList.isEmpty() ) {
                myDebug() << "    USBSettingsApp::load scrollTo " << indexList[0].row();
                myDebug() << "    USBSettingsApp::load scrollTo disabled";
                //listView->scrollTo(indexList[0]);
            }
        }

        QGraphicsWidget *listViewItemWidget = mDocumentLoader->findWidget(QString("listItemPrototype"));
        if (listViewItemWidget) {
            myDebug() << "    USBSettingsApp::load mListViewItem found";
            mListViewItem = qobject_cast<HbListViewItem*>(listViewItemWidget);
            mListViewItem->setGraphicsSize(HbListViewItem::LargeIcon);
            setOrientation(orientation);
        }
        
        QGraphicsWidget *groupBoxWidget = mDocumentLoader->findWidget(QString("groupBox"));
        if (groupBoxWidget) {
            myDebug() << "    USBSettingsApp::load groupBox found";
            HbGroupBox *groupBox = qobject_cast<HbGroupBox*>(groupBoxWidget);
            groupBox->setHeading(QString(hbTrId("txt_usb_subhead_select_connection_type")));
        }        
    }  
    myDebug() << "<<< USBSettingsApp::load";
}
/*!
	Handles the orientation change
*/
void USBSettingsApp::setOrientation(Qt::Orientation orientation)
{
    myDebug() << ">>> USBSettingsApp::setOrientation";
    if (mListViewItem) {
        switch (orientation) {
            case Qt::Horizontal:
                mListViewItem->setSecondaryTextRowCount(KUSBLandscapeMinLine, KUSBLandscapeMaxLine);
                break;
            case Qt::Vertical:
                mListViewItem->setSecondaryTextRowCount(KUSBPortrateMinLine, KUSBPortrateMaxLine);
                break;
            default:
            		break;
        }               
    }    
    myDebug() << "<<< USBSettingsApp::setOrientation";
}

