/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  active object, that monitors the real connection starting
*
*/


#ifndef C_CONNECTIONSTARTER_H
#define C_CONNECTIONSTARTER_H

#include <e32base.h>

// FORWARD DECLARATIONS
class CVpnConnection;

/**
 *  Connection starter callback interface.
 *  Callback interface which is used by CConnectionStarter object to notify
 *  about completion of real network connection's starting.
 * 
 *  @lib internal (kmdserver.exe)
  */
class MConnectionStarterCallback
    {
public:        
    /**
     * Notifies about completion of real connection starting.
     * @param aStatus Completion status
     * @param aRealIapId IAP Id of started connection
     * @param aRealNetId Net Id of started connection
     */
    virtual void RealConnectionStarted( TInt aStatus,
                                        TInt aRealIapId,
                                        TInt aRealNetId ) = 0;
    };


/**
 *  Connection starter.
 *  Active object provides functionality for starting of real network
 *  connection.
 *
 *  @lib internal (kmdserver.exe)
  */
class CConnectionStarter : public CActive
    {
public:
    /**
     * Two-phased constructor.
     * @param aConnection VPN connection object
     * @param aCallback Callback interface
     */
    static CConnectionStarter* NewL( CVpnConnection& aConnection,
                                     MConnectionStarterCallback& aCallback );
    
    /**
     * Destructor.
     */
    ~CConnectionStarter();

    /**
     * Starts real network connection. Completion is notified via
     * MConnectionStarterCallback interface.
     */
    void StartRealConnection();

private:
    
    CConnectionStarter( CVpnConnection& aConnection,
                        MConnectionStarterCallback& aCallback );
    
// from base class CActive
    
    /**
     * From CActive
     * Handles completion of asynchronous connection starting request.
     */    
    void RunL();
    
    /**
     * From CActive
     * Handles cancellation of asynchronous connection starting request.
     */    
    void DoCancel();
    
private: // data
    
    /**
     * VPN connection object.
     * Not own.
     */
    CVpnConnection&             iConnection;
    
    /**
     * Callback interface.
     * Not own.
     */
    MConnectionStarterCallback& iCallback;
    };

#endif // C_CONNECTIONSTARTER_H
