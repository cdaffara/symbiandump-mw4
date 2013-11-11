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
* Description: USB Indicator Plugin Class
*
*/

#ifndef USBINDICATORPLUGIN_H
#define USBINDICATORPLUGIN_H


#include <hbindicatorplugininterface.h>
#include <hbindicatorinterface.h>
#include <QTranslator>

const QString ConnectedIndicator = ("com.nokia.hb.indicator.usb.device.connected/1.0");
const QString AddressedIndicator = ("com.nokia.hb.indicator.usb.device.inprogress/1.0" );
const QString MassStorageIndicator = ("com.nokia.hb.indicator.usb.host.massstorage/1.0" );
const QString UsbDisconnectingIndicator = ("com.nokia.hb.indicator.usb.host.disconnecting/1.0" );

_LIT(KUSBExe, "USBSettingsApp.exe");
const TInt KUSBUIUid = 0x2002E70C;

/*!
   USB indicator plugin class. 
   Main class of USB indicator plugin. 
 */
class UsbIndicatorPlugin : public QObject, public HbIndicatorPluginInterface
{
    Q_OBJECT
    Q_INTERFACES(HbIndicatorPluginInterface)

public:
    /*!
       Constructor
     */
    UsbIndicatorPlugin();
     
     /*!
        Destructor
      */
    ~UsbIndicatorPlugin();
    
    /*!
       @see HbIndicatorPluginInterface
     */
    QStringList indicatorTypes() const;
    
    /*!
       @see HbIndicatorPluginInterface
     */
    inline bool accessAllowed(const QString &indicatorType,
            const QVariantMap &securityInfo) const;   
    /*!
       @see HbIndicatorPluginInterface
     */
    HbIndicatorInterface* createIndicator(const QString &indicatorType);
    
    /*!
       @see HbIndicatorPluginInterface
     */
    inline int error() const;
    
private:
    Q_DISABLE_COPY(UsbIndicatorPlugin)
    
    /*!
       Error value
     */
    int mError;
    
    /*!
       Indicator types 
       supported indicator types. 
     */
    QStringList mIndicatorTypes;
    
    bool mTranslatorLoaded;
    
    QTranslator mTranslator;
};

// ----------------------------------------------------------------------------
// UsbIndicatorPlugin::accessAllowed
// ----------------------------------------------------------------------------
bool UsbIndicatorPlugin::accessAllowed(const QString &indicatorType,
    const QVariantMap &securityInfo) const
{
    Q_UNUSED(indicatorType)
    Q_UNUSED(securityInfo)

    // This plugin doesn't perform operations that may compromise security.
    // All clients are allowed to use.
    return true;
}

// ----------------------------------------------------------------------------
// UsbIndicatorPlugin::error
// ----------------------------------------------------------------------------
inline int UsbIndicatorPlugin::error() const
{
    return mError;
}


#endif /* USBIndicatorPlugin_H */
