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
#include "usbindicator.h" 
#include <QVariant>
#include <e32uid.h>
#include <apadef.h>
#include "usbdebug.h"

const QString TextIdPrefix = ("txt_usb_dblist_usb_connected_val_");
/*!
   USBIndicator::USBIndicator
*/ 
USBIndicator::USBIndicator(const QString &indicatorType) :
HbIndicatorInterface(indicatorType,
        HbIndicatorInterface::SettingCategory,
        InteractionActivated)
{
}

/*!
   USBIndicator::~USBIndicator
*/
USBIndicator::~USBIndicator()
{
}


/*!
   USBIndicator::handleInteraction
*/
bool USBIndicator::handleInteraction(InteractionType type)
{
    myDebug() << ">>> USBIndicator::handleInteraction";
    bool handled = false;
    TUidType uidtype(KExecutableImageUid, TUid::Uid(0x00),
                            TUid::Uid(KUSBUIUid));    
    if (type == InteractionActivated) {
        RProcess usbUiProcess;                
        TInt result = usbUiProcess.Create(KUSBExe(), KNullDesC, uidtype);
        if (result == KErrNone) {
            usbUiProcess.Resume();
        }
        usbUiProcess.Close();         
        handled = true;                
    }
    myDebug() << "<<< USBIndicator::handleInteraction";
    return handled;
}

/*!
   USBIndicator::indicatorData
   returns the data and icon that needs to be displayed in the universal pop up and indicator menu 
*/
QVariant USBIndicator::indicatorData(int role) const
{
    myDebug() << ">>> USBIndicator::indicatorData, role is " << role;
    switch(role) {
    	case PrimaryTextRole: 
        {
        	QString text = QString(hbTrId("txt_usb_dblist_usb_connected"));
        	return text;
        }
    	case SecondaryTextRole:
        {
        	return mSecDisplayName;
        }
   	 	case DecorationNameRole:
        {
        	QString iconName(KUsbIconFile);
        	return iconName;
        }
    	default: 
        	return QVariant();      
    }
}

/*!
   USBIndicator::handleClientRequest
   handles client's activate and deactivate request
*/
bool USBIndicator::handleClientRequest( RequestType type, 
        const QVariant &parameter)
{ 
    myDebug() << ">>> USBIndicator::handleClientRequest";
    switch (type) {
        case RequestActivate:
            {
            QString friendlyName = parameter.toString();
            friendlyName.replace( QChar(' '), QChar('_') );
            QString textId = TextIdPrefix + friendlyName;
            mSecDisplayName = hbTrId(textId.toAscii());  
            emit dataChanged();
            }
            break;
        default:
            mSecDisplayName.clear();
            emit deactivate();
            break;
    }
    //request always handled
   	myDebug() << "<<< USBIndicator::handleClientRequest";
    return true;
}
