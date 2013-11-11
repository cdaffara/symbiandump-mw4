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
* Description: USB Indicator Class
*
*/

#ifndef USBINDICATOR_H
#define USBINDICATOR_H

#include "usbindicatorplugin.h"
#include <hbindicatorinterface.h>

#define KUsbIconFile "qtg_large_usb"

/**
 * USB indicator class. 
 * Handles client request and showing the indications. 
 */
class USBIndicator : public HbIndicatorInterface
{
    friend class TestUsbIndicatorPlugin;
    
public:
    /**
     * Constructor
     */
    USBIndicator(const QString &indicatorType);
    
    /**
     * Destructor
     */
    ~USBIndicator();
    
    /**
     * @see HbIndicatorInterface
     */
    bool handleInteraction(InteractionType type);
    
    /**
     * @see HbIndicatorInterface
     */
    QVariant indicatorData(int role) const;
    
protected:
    /**
     * @see HbIndicatorInterface
     */
    bool handleClientRequest(RequestType type, const QVariant &parameter);
    
private: 

    QString mSecDisplayName;
};

#endif /* USBINDICATOR_H*/




