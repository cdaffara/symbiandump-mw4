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

#ifndef USBDISCONNECTINGINDICATOR_H
#define USBDISCONNECTINGINDICATOR_H

#include <hbindicatorinterface.h>
#include "usbindicatorplugin.h"

/**
 * USB indicator class. 
 * Handles client request and showing the indications. 
 */
class USBDisconnectingIndicator : public HbIndicatorInterface
{
    friend class TestUsbIndicatorPlugin;

public:
    /**
     * Constructor
     */
    USBDisconnectingIndicator(const QString &indicatorType);
    
    /**
     * Destructor
     */
    ~USBDisconnectingIndicator();
       
    /**
     * @see HbIndicatorInterface
     */
    QVariant indicatorData(int role) const;
    
    
protected:
    /**
     * @see HbIndicatorInterface
     */
    bool handleClientRequest(RequestType type, const QVariant &parameter);
    
    
};

#endif /* USBDISCONNECTINGINDICATOR_H */




