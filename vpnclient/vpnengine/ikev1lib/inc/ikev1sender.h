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
* Description:  Sender of UDP datagrams
*
*/


#ifndef C_IKEV1SENDER_H
#define C_IKEV1SENDER_H

#include <e32base.h>

// FORWARD DECLARATIONS
class MIkeDataInterface;
class MIkeDebug;
class TInetAddr;

/**
 *  IKEv1 sender callback interface.
 *  Callback interface which is used by CIkev1Sender object to notify
 *  about completion of sending.
 * 
 *  @lib internal (ikev1lib.lib)
  */
NONSHARABLE_CLASS(MIkev1SenderCallback)
    {
public:        
    /**
     * Notifies about completion of sending.
     * @param aStatus Completion status
     */
    virtual void SendUdpDataCompleted( TInt aStatus ) = 0;

    };

/**
 *  IKEv1 sender.
 *  Active object provides functionality for sending UDP datagrams.
 *
 *  @lib internal (ikev1lib.lib)
  */
NONSHARABLE_CLASS(CIkev1Sender) : public CActive
    {
public:
    /**
     * Two-phased constructor.
     * @param aDataInterface IKE data interface
     * @param aCallback Callback interface
     * @param aDebug Debug trace interface
     */
    static CIkev1Sender* NewL( MIkeDataInterface& aDataInterface,
                               MIkev1SenderCallback& aCallback,
                               MIkeDebug& aDebug );

    /**
    * Destructor.
    */
    ~CIkev1Sender();

    /**
    * Sends UDP data.
    * @param aUdpData UDP data. Ownership transferred.
    * @param aDestAddr Destination IP address/port
    * @param aLocalPort Local port
    * @param aDscp DSCP value
    */
    void SendUdpData( HBufC8* aUdpData,                     
                      const TInetAddr& aDestAddr,
                      TInt aLocalPort,
                      TUint8 aDscp );
        
private:
    
    CIkev1Sender( MIkeDataInterface& aDataInterface,
                  MIkev1SenderCallback& aCallback,
                  MIkeDebug& aDebug );

// from base class CActive            
    
    /**
     * From CActive
     * Handles completion of asynchronous sending.
     */    
    void RunL();

    /**
     * From CActive
     * Handles cancellation of asynchronous sending.
     */    
    void DoCancel();
        
private: // data
    
    /**
     * UDP data.
     * Own.
     */    
    HBufC8*                    iUdpData;
    
    /**
     * IKE data interface.
     * Not own.
     */
    MIkeDataInterface&          iDataInterface;
    
    /**
     * Callback interface.
     * Not own.
     */
    MIkev1SenderCallback&       iCallback;
    
    /**
     * Debug trace interface.
     * Not own.
     */
    MIkeDebug&                  iDebug;
    };

#endif // C_IKEV1SENDER_H
