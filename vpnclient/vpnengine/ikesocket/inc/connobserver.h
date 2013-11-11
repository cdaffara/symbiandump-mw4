/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Link connection status observer
*
*/

#ifndef C_CONNOBSERVER_H
#define C_CONNOBSERVER_H

// INCLUDES
#include <in_sock.h>

// FORWARD DECLARATIONS

class MIkeDebug;
class RConnection;

/**
 *  Link disconnection callback interface.
 *
 *  Callback interface for notifying about link disconnection.
 *
 *  @lib ikesocket.lib
 */
NONSHARABLE_CLASS( MConnObserverCallback )
    {
public:    
    /**
     * Notifies about link disconnection.
     * 
     * @param aStatus Link disconnection status
     */
    virtual void LinkDisconnected( const TInt aStatus ) = 0;    
    };

/**
 *  Link connection status observer.
 *
 *  This class provides functionality for observing when link connection has
 *  been disconnected.
 *
 *  @lib ikesocket.lib
 */
NONSHARABLE_CLASS( CConnObserver ) : private CActive
    {
public:
    /**
     * Two-phased constructor.
     * 
     * @param aConnection Connection which is used for observing
     * @param aCallback Callback interface
     * @param aDebug Debug trace interface
     */
    static CConnObserver* NewL( RConnection& aConnection,
                                MConnObserverCallback& aCallback,
                                MIkeDebug& aDebug );        

    /**
     * Destructor.
     */
    ~CConnObserver();

    /**
     * Requests notification about link disconnection. Link disconnection will
     * be notified via MConnObserverCallback callback interface, when link has
     * been disconnected.
     */
    void NotifyDisconnect();
    
    /**
     * Cancels link disconnection notification request.
     */
    void CancelNotify();                    

private:
    
// from base class CActive
    
    /**
     * From CActive.
     * Handles an active object's request completion event about link
     * disconnection progress notification. Notifies client about link
     * disconnection via callback interface. Does not leave. 
     */
    void RunL();
    
    /**
     * From CActive.
     * Implements cancellation of a link disconnection progress notification.
     */
    void DoCancel();

private:        
    CConnObserver( RConnection& aConnection,
                   MConnObserverCallback& aCallback,
                   MIkeDebug& aDebug );
    
    void ConstructL();
    
private: // data
    /**
     * Connection used for observing link disconnection.
     * Not own.
     */
    RConnection&               iConnection;
        
    /**
     * Buffer for Network Interface Progress.
     * Own.
     */
    TNifProgressBuf            iProgressBuf;

    /**
     * Callback interface for notifying link disconnection.
     * Not own.
     */
    MConnObserverCallback&     iCallback;
    
    /**
     * Debug trace interface.
     * Not own.
     */
    MIkeDebug&                 iDebug;
    };


#endif // C_CONNOBSERVER_H
