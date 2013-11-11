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
* Description:  IKE protocol plugin session interface
*
*/


#ifndef M_IKEPLUGINSESSIONIF_H
#define M_IKEPLUGINSESSIONIF_H

#include <e32base.h>
#include "vpnmandefs.h"

// FORWARD DECLARATIONS
class CIkeData;
class MIkeDataInterface;

/**
 *  IKE protocol plugin session interface.
 *
 *  Session interface for handling an IKE connection with a remote host.
 *  Separate session is required for each VPN connection.
 *
 *  @lib internal (kmdserver.exe)
 */
class MIkePluginSessionIf
    {    
public:

    /**
     * Destructor.
     */
    virtual ~MIkePluginSessionIf() {};
    
    /**
     * Starts negotiation with a remote host.
     * @param aIkeData IKE policy data
     * @param aInternalAddress Internal address (returned)
     * @param aStatus Completion status (returned) 
     */
    virtual void NegotiateWithHost( const CIkeData& aIkeData,                                                                        
                                    TVPNAddress& aInternalAddress,
                                    TRequestStatus& aStatus ) = 0;
    
    /**
     * Cancels negotiate request. DeleteSession() method needs to be called
     * after this method to delete session.
     */
    virtual void CancelNegotiateWithHost() = 0;
    
    /**
     * Deletes session. IKE/IPSec SA:s are deleted.
     * @param aSilentClose Specified if a silent close in question (Delete
     * payloads not transmitted to remote host)
     * @param aStatus Completion status (returned)
     */
    virtual void DeleteSession( const TBool aSilentClose,
                                TRequestStatus& aStatus ) = 0;
    
    /**
     * Cancels deletion requests. IKE/IPSec SA:s are deleted.
     */
    virtual void CancelDeleteSession() = 0;
    
    /**
     * Requests notification about error condition. Error notification causes
     * IKE plugin session to be deleted silently and link disconnected. IKE
     * plugin session needs to cleanup session, before notifying error.
     * @param aStatus Completion status (returned)
     */
    virtual void NotifyError( TRequestStatus& aStatus ) = 0;
    
    /**
     * Cancels error notification request.
     */
    virtual void CancelNotifyError() = 0;
    
    /**
     * Requests notification about change of internal address.
     * @param aInternalAddress Internal address (returned)
     * @param aStatus KErrNone or KErrCancel. Error condition needs to be
     *                indicated via NotifyError() method. (returned)
     */
    virtual void NotifyInternalAddressChanged( TVPNAddress& aInternalAddress,
                                               TRequestStatus& aStatus ) = 0;
    
    /**
     * Cancels internal address change notification request.
     */
    virtual void CancelNotifyInternalAddressChanged() = 0;
    
    };


#endif // M_IKEPLUGINSESSIONIF_H
