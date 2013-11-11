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
* Description: USB Mass Storage Indicator Class
*
*/

#ifndef USBMASSSTORAGEINDICATOR_H
#define USBMASSSTORAGEINDICATOR_H

#include <hbindicatorinterface.h>
#include "usbindicatorplugin.h"

/**
 * USB mass storage indicator class. 
 * Handles client request and showing the indications. 
 */
class UsbMassStorageIndicator : public HbIndicatorInterface
{
    friend class TestUsbIndicatorPlugin;
    
public:
    /**
     * Constructor
     */
    UsbMassStorageIndicator(const QString &indicatorType);
    
    /**
     * Destructor
     */
    ~UsbMassStorageIndicator();
    
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
    
    bool mEjectStarted;
};

#endif /* USBMASSSTORAGEINDICATOR_H */
