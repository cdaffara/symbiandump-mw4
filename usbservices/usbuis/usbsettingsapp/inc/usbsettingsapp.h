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

#ifndef USBSETTINGSAPP_H
#define USBSETTINGSAPP_H

#include <QObject>
#include <hbmainwindow.h>

const int KUSBPortrateMinLine = 4;
const int KUSBPortrateMaxLine = 4;
const int KUSBLandscapeMinLine = 2;
const int KUSBLandscapeMaxLine = 2;

class UsbUiSettingModel;
class QItemSelectionModel;
class HbView;
class HbDocumentLoader;
class HbListViewItem;

class USBSettingsApp : public HbMainWindow
{
    Q_OBJECT

public:
	USBSettingsApp(QWidget *parent = 0);
    ~USBSettingsApp();

public slots:
		/*
		 * Handles the orientation change
		 * @param orientation is teh new orientation to be set
     */
    void setOrientation(Qt::Orientation orientation);

private:
		/*
		 * Loads the DOCML and build the view by finding the widgets from it 
		 * @param Orientation at loading time
		 */
    void load(Qt::Orientation);

private:
		// pointer to the HbDocumentLoader to load the DOCML owned by this class
    HbDocumentLoader *mDocumentLoader;
    // Pointer to the view, owned
    HbView *mView;
    //Pointer to model, owned
    UsbUiSettingModel *mModel;
    //Pointer to selection model, owned
    QItemSelectionModel *mSelectionModel;
    HbListViewItem *mListViewItem;
};

#endif // USBSETTINGSAPP_H
