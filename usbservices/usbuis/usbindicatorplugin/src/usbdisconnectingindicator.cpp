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
#include "usbdisconnectingindicator.h" 
#include <QVariant>
#include "usbindicatorplugin.h"
#include "usbindicator.h"


/*!
   UsbDisconnectingIndicator::UsbDisconnectingIndicator
*/ 
USBDisconnectingIndicator::USBDisconnectingIndicator(const QString &indicatorType) :
HbIndicatorInterface(indicatorType,
        HbIndicatorInterface::ProgressCategory,
        NoInteraction)
{
}

/*!
   USBDisconnectingIndicator::~USBDisconnectingIndicator
*/
USBDisconnectingIndicator::~USBDisconnectingIndicator()
{
}


/*!
   USBDisconnectingIndicator::indicatorData
   returns the data and icon that needs to be displayed in the universal pop up and indicator menu 
*/
QVariant USBDisconnectingIndicator::indicatorData(int role) const
{
    switch(role) {
   		case PrimaryTextRole: 
    	{
        	QString text = QString(hbTrId("txt_usb_dblist_usb_disconnecting"));
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
   UsbDisconnectingIndicator::handleClientRequest
   handles client's activate and deactivate request
*/
bool USBDisconnectingIndicator::handleClientRequest( RequestType type, 
        const QVariant &parameter)
{ 
    switch (type) {
    	case RequestActivate:
			emit dataChanged();
			break;
        default:
            emit deactivate();
            break;
    }
    //request always handled
    return true;
}


