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
* Description:  KMD event logger
*
*/


#ifndef C_KMDEVENTLOGGER_H
#define C_KMDEVENTLOGGER_H

#include "kmdeventloggerif.h"

// FORWARD DECLARATIONS
class MIkeDebug;

/**
 *  KMD event logger.
 *
 *  Handles logging of KMD specific events to event log.
 *
 *  @lib internal (kmdserver.exe)
 */
class CKmdEventLogger : public CBase,
                        public MKmdEventLoggerIf
    {        
public:    

    /**
     * Two-phased constructor.
     * @param aDebug Debug trace interface
     */
    static CKmdEventLogger* NewL( MIkeDebug& aDebug );
        
    /**
     * Destructor.
     */    
    ~CKmdEventLogger();
    
// from base class MKmdEventLoggerIf
    
    /**
     * From MKmdEventLoggerIf
     * Writes event to event log.
     *
     * @param aCategory Log event category
     * @param aMsgId Msg Id
     * @param aStatus Status
     * @param aVpnIapId VPN IAP Id
     * @param aSgwIp SGW IP address
     * @param aLocalAddr Local IP address
     */    
    void LogEvent( TLogCategory aCategory,
                   TInt aMsgId,
                   TInt aStatus,
                   TUint32 aVpnIapId,
                   const TInetAddr* aGwIp,
                   const TInetAddr* aLocalAddr = NULL );
    
    /**
     * From MKmdEventLoggerIf
     * Writes event to event log.
     *
     * @param aCategory Log event category
     * @param aMsgId Msg Id
     * @param aStatus Status
     * @param aVpnIapId VPN IAP Id
     * @param aSgwIp SGW IP address
     * @param aLocalAddr Local IP address
     */    
    void LogEvent( TKmdLogCategory aCategory,
                   TInt aMsgId,
                   TInt aStatus,
                   TUint32 aVpnIapId,
                   const TInetAddr* aGwIp,
                   const TInetAddr* aLocalAddr = NULL );
        
private:
    CKmdEventLogger( MIkeDebug& aDebug );
    
    void ConstructL();        
    
private: // data
    
    /**
     * Event mediator.
     * Own.
     */
    REventMediator  iEventMediator;
    
    /**
     * Debug trace interface.
     * Not own.
     */
    MIkeDebug&      iDebug;    
    
    };


#endif // C_KMDEVENTLOGGER_H
