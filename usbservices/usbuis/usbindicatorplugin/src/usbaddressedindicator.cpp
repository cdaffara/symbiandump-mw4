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
#include "usbaddressedindicator.h" 
#include <QVariant>
#include <e32uid.h>
#include <apadef.h>
#include "usbindicatorplugin.h"
#include "usbindicator.h"
#include "usbdebug.h"


/*!
   UsbAddressedIndicator::UsbAddressedIndicator
*/ 
UsbAddressedIndicator::UsbAddressedIndicator(const QString &indicatorType) :
HbIndicatorInterface(indicatorType,
        HbIndicatorInterface::ProgressCategory,
        InteractionActivated)
{
}

/*!
   UsbAddressedIndicator::~UsbAddressedIndicator
*/
UsbAddressedIndicator::~UsbAddressedIndicator()
{
}


/*!
   UsbAddressedIndicator::handleInteraction
*/
bool UsbAddressedIndicator::handleInteraction(InteractionType type)
{ 
    myDebug() << ">>> UsbAddressedIndicator::handleInteraction";
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
    myDebug() << "<<< UsbAddressedIndicator::handleInteraction";
    return handled;
}

/*!
   UsbAddressedIndicator::indicatorData
   returns the data and icon that needs to be displayed in the universal pop up and indicator menu 
*/
QVariant UsbAddressedIndicator::indicatorData(int role) const
{
    myDebug() << ">>> UsbAddressedIndicator::indicatorData";
    switch (role) {
    	case PrimaryTextRole:
		{ 
        	QString text = QString(hbTrId("txt_usb_dblist_usb_connecting"));
        	return text;
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
   UsbAddressedIndicator::handleClientRequest
   handles client's activate and deactivate request
*/
bool UsbAddressedIndicator::handleClientRequest( RequestType type, 
        const QVariant &parameter)
{ 
    myDebug() << ">>> UsbAddressedIndicator::handleClientRequest";
    switch (type) {
    	case RequestActivate:
    		emit dataChanged();
        	break;
    	default:
    		emit deactivate();
        	break;
    }
   	myDebug() << "<<< UsbAddressedIndicator::handleClientRequest";
   	//request always handled
   	return true;
}


