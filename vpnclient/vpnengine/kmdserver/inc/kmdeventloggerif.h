/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  KMD event logger interface
*
*/


#ifndef M_KMDEVENTLOGGERIF_H
#define M_KMDEVENTLOGGERIF_H

#include <e32base.h>

#include "eventlogger.h"
#include "eventmediatorapi.h"

// FORWARD DECLARATIONS
class TInetAddr;

#define LOG_KMD_EVENT(a,b,c,d,e) EventLogger().LogEvent(a,b,c,d,e);
#define LOG_KMD_EVENT2(a,b,c,d,e,f) EventLogger().LogEvent(a,b,c,d,e,f);

/**
 *  KMD event logger interface.
 *
 *  Logger interface for logging KMD specific events to event log.
 *
 *  @lib internal (kmdserver.exe)
 */
class MKmdEventLoggerIf
    {        
public:    

    enum TKmdLogCategory
        {
        KLogInfo = 0,
        KLogWarning,
        KLogError,
        KLogDebug
        };
    
    /**
     * Writes event to event log.
     *
     * @param aCategory Log event category
     * @param aMsgId Msg Id
     * @param aStatus Status
     * @param aVpnIapId VPN IAP Id
     * @param aSgwIp SGW IP address
     * @param aLocalAddr Local IP address
     */
    virtual void LogEvent( TLogCategory aCategory,
                           TInt aMsgId,
                           TInt aStatus,
                           TUint32 aVpnIapId,
                           const TInetAddr* aSgwIp,
                           const TInetAddr* aLocalAddr = NULL ) = 0;
    
    /**
     * Writes event to event log.
     *
     * @param aCategory Log event category 
     * @param aMsgId Msg Id
     * @param aStatus Status
     * @param aVpnIapId VPN IAP Id
     * @param aSgwIp SGW IP address
     * @param aLocalAddr Local IP address
     */
    virtual void LogEvent( TKmdLogCategory aCategory,
                           TInt aMsgId,
                           TInt aStatus,
                           TUint32 aVpnIapId,
                           const TInetAddr* aSgwIp,
                           const TInetAddr* aLocalAddr = NULL ) = 0;
        
    };


#endif // M_KMDEVENTLOGGERIF_H
