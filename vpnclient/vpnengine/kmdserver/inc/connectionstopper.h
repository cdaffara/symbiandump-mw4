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
* Description:  active object, that monitors stopping of connection
*
*/


#ifndef C_CONNECTIONSTOPPER_H
#define C_CONNECTIONSTOPPER_H

#include <e32base.h>

// FORWARD DECLARATIONS
class CVpnConnection;

/**
 *  Connection stopper callback interface.
 *  Callback interface which is used by CConnectionStopper object to notify
 *  about completion of VPN connection stopping.
 * 
 *  @lib internal (kmdserver.exe)
  */
class MConnectionStopperCallback
    {
public:
    /**
     * Notifies about completion of VPN connection stopping.
     * @param aStatus Completion status
     */
    virtual void VpnConnectionStopped( TInt aStatus ) = 0;
    };

/**
 *  Connection stopper.
 *  Active object provides functionality for stopping of VPN connection.
 *
 *  @lib internal (kmdserver.exe)
  */
class CConnectionStopper : public CActive
    {
public:
    /**
     * Two-phased constructor.
     * @param aConnection VPN connection object
     * @param aCallback Callback interface
     */
    static CConnectionStopper* NewL( CVpnConnection& aVpnConnection,
                                     MConnectionStopperCallback& aCallback );

    /**
     * Destructor.
     */
    ~CConnectionStopper();

    /**
     * Stops VPN connection. Completion is notified via MConnectionStopperCallback
     * interface.
     * @param aSilentClose Specified if a silent close in question (Delete
     *                     payloads not transmitted to remote host)
     */
    void StopVpnConnection( TBool aSilentClose );            

private:
    CConnectionStopper( CVpnConnection& aVpnConnection,
                        MConnectionStopperCallback& aCallback );
    
// from base class CActive
    
    /**
     * From CActive
     * Handles completion of asynchronous connection stopping request.
     */    
    void RunL();

    /**
     * From CActive
     * Handles cancellation of asynchronous connection stopping request.
     */    
    void DoCancel();
        
private: // data
    
    /**
     * VPN connection object.
     * Not own.
     */
    CVpnConnection&                 iVpnConnection;    

    /**
     * Callback interface.
     * Not own.
     */
    MConnectionStopperCallback&     iCallback;
    };

#endif // C_CONNECTIONSTOPPER_H
