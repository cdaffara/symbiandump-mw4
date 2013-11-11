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
* Description: USB Mass Storage indicator implementation 
*
*/

#include "usbmassstorageindicator.h" 
#include <QVariant>
#include <e32uid.h>
#include <apadef.h>
#include <usb/hostms/msmmindicatorclient.h>
#include "usbdebug.h"

// icon name for mass storage
const QString KUsbIconFile("qtg_large_usb_memory");

/*!
   Constructor
   @param indicatorType The indicator type name
*/ 
UsbMassStorageIndicator::UsbMassStorageIndicator(const QString &indicatorType) :
HbIndicatorInterface(indicatorType,
        HbIndicatorInterface::SettingCategory,
        InteractionActivated),
        mEjectStarted(false)
{
}

/*!
   Destructor
*/
UsbMassStorageIndicator::~UsbMassStorageIndicator()
{
    myDebug() << ">>> UsbMassStorageIndicator::~UsbMassStorageIndicator";
}


/*!
   Handle user interaction
   Request eject for all USB drives.
*/
bool UsbMassStorageIndicator::handleInteraction(InteractionType type)
{
    myDebug() << ">>> UsbMassStorageIndicator::handleInteraction";
    bool handled = false;
    if (type == InteractionActivated) {
        if (!mEjectStarted) {
            mEjectStarted = true;
            // dismount all drives without waiting for result
            RHostMassStorage hostMassStorage;
            myDebug() << "    UsbMassStorageIndicator::handleInteraction hostMassStorage connecting";
            TInt err = hostMassStorage.Connect();
            myDebug() << "    UsbMassStorageIndicator::handleInteraction hostMassStorage connected";
            if (err == KErrNone) {
                hostMassStorage.EjectUsbDrives();
                myDebug() << "UsbMassStorageIndicator::handleInteraction EjectUsbDrives called";
            }
            else {
                myDebug() << "    UsbMassStorageIndicator::handleInteraction "
                        <<"RHostMassStorage Connect fail " << err;
            }
            hostMassStorage.Disconnect();
            myDebug() << ">>> UsbMassStorageIndicator::handleInteraction disconnected";
            hostMassStorage.Close();
            myDebug() << ">>> UsbMassStorageIndicator::handleInteraction closed";
        }
        handled = true;                
    }
    myDebug() << "<<< UsbMassStorageIndicator::handleInteraction";
    return handled;
}

/*!
   Return the data and icon that needs to be displayed in the universal indicator menu 
*/
QVariant UsbMassStorageIndicator::indicatorData(int role) const
{
    myDebug() << ">>> UsbMassStorageIndicator::indicatorData";
    switch (role) {
        case PrimaryTextRole: 
            return QString(hbTrId("txt_usb_dblist_usb_connected"));
        case SecondaryTextRole:
            return QString(hbTrId("txt_usb_dpinfo_click_to_eject"));;
        case DecorationNameRole:
            return KUsbIconFile;
        default: 
            return QVariant(); //empty variant      
    }
}

/*!
   USBIndicator::handleClientRequest
   handles client's activate and deactivate request
*/
bool UsbMassStorageIndicator::handleClientRequest( RequestType type, 
        const QVariant &parameter)
{ 
    myDebug() << ">>> UsbMassStorageIndicator::handleClientRequest";
    Q_UNUSED(parameter);
    if (type == RequestDeactivate) {
        myDebug() << "    UsbMassStorageIndicator::handleClientRequest deactivate";
        mEjectStarted = false;
        emit deactivate();        
    }
    myDebug() << "<<< UsbMassStorageIndicator::handleClientRequest";
    return true;
}
