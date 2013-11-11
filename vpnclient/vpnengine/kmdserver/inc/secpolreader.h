/*
* Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Security policy module
*
*/


#ifndef C_SECPOLREADER_H
#define C_SECPOLREADER_H

#include <es_sock.h>
#include <in_sock.h>

// FORWARD DECLARATIONS
class MIkeDebug;
class MKmdEventLoggerIf;

const TInt KMaxSecpolMsgSize( 1000 );

/**
 *  Secpol reader.
 *  Class for reading messages from SECPOL socket.
 *
 *  @lib internal (kmdserver.exe)
 */
class CSecpolReader : public CActive
    {
public:
    /**
     * Two-phased constructor.
     * @param aEventLogger Event logger
     * @param aDebug Debug trace interface
     */
    static CSecpolReader* NewL( MKmdEventLoggerIf& aEventLogger,
                                MIkeDebug& aDebug );

    /**
     * Destructor.
     */
    ~CSecpolReader();

// from base class CActive
    
    /**
     * From CActive
     * Handles completion of asynchronous reading.
     */    
    void RunL();
    
    /**
     * From CActive
     * Handles cancellation of asynchronous reading.
     */    
    void DoCancel();     

private:
    CSecpolReader( MKmdEventLoggerIf& aEventLogger,
                   MIkeDebug& aDebug );
    void ConstructL();
    
    /**
     * Returns event logger.
     * @return Event logger
     */
    MKmdEventLoggerIf& EventLogger();
    
private: // data

    /**
     * Socket server.
     * Own.
     */
    RSocketServ                 iSocketServer;
    
    /**
     * Secpol socket.
     * Own.
     */
    RSocket                     iSocket;
    
    /**
     * Secpol message.
     * Own.
     */
    TBuf8<KMaxSecpolMsgSize>    iMsg;
    
    /**
     * Remote source address.
     * Own.
     */
    TInetAddr                   iAddr;
    
    /**
     * Event logger.
     * Not own.
     */
    MKmdEventLoggerIf&          iEventLogger;
    
    /**
     * Debug trace interface.
     * Not own.
     */
    MIkeDebug&                  iDebug;
    };

#endif // C_SECPOLREADER_H
