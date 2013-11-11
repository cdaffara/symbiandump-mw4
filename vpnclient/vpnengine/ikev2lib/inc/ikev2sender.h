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
* Description:  Sender of UDP datagrams
*
*/


#ifndef C_IKEV2SENDER_H
#define C_IKEV2SENDER_H

#include <e32base.h>

// FORWARD DECLARATIONS
class MIkeDataInterface;
class TInetAddr;

/**
 *  IKEv2 sender callback interface.
 *  Callback interface which is used by CIkev2Sender object to notify
 *  about completion of sending.
 * 
 *  @lib internal (ikev2lib.lib)
  */
NONSHARABLE_CLASS(MIkev2SenderCallback)
    {
public:        
    /**
     * Notifies about completion of sending.
     *
     * @param aStatus Completion status
     * @param aMsg IKE message
     */
    virtual void SendIkeMsgCompleted( TInt aStatus ) = 0;

    };

/**
 *  IKEv1 sender.
 *  Active object provides functionality for sending UDP datagrams.
 *
 *  @lib internal (ikev1lib.lib)
  */
NONSHARABLE_CLASS(CIkev2Sender) : public CActive
    {
public:
    /**
     * Two-phased constructor.
     * @param aDataInterface IKE data interface
     * @param aCallback Callback interface
     */
    static CIkev2Sender* NewL( MIkeDataInterface& aDataInterface,
                               MIkev2SenderCallback& aCallback );

    /**
    * Destructor.
    */
    ~CIkev2Sender();

    /**
    * Sends IKE message.
    * @param aLocalPort Local port
    * @param aDestAddr Destination IP address/port
    * @param aIkeMsg IKE message
    */
    void SendIkeMsg( TInt aLocalPort,
                     TInetAddr& aDestAddr,
                     TUint8 aDscp,
                     const TDesC8& aIkeMsg );
        
    
private:
    
    CIkev2Sender( MIkeDataInterface& aDataInterface,
                  MIkev2SenderCallback& aCallback );

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
     * IKE data interface.
     * Not own.
     */
    MIkeDataInterface&          iDataInterface;
    
    /**
     * Callback interface.
     * Not own.
     */
    MIkev2SenderCallback&       iCallback;
    };

#endif // C_IKEV2SENDER_H
