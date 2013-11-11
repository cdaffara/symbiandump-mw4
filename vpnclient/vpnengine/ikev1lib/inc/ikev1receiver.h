/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Receiver of UDP datagrams
*
*/


#ifndef C_IKEV1RECEIVER_H
#define C_IKEV1RECEIVER_H

#include <e32base.h>
#include <in_sock.h>

// FORWARD DECLARATIONS
class MIkeDataInterface;
class ThdrISAKMP;

/**
 *  IKEv1 receiver callback interface.
 *  Callback interface which is used by CIkev1Receiver object to notify
 *  about received IKE message.
 * 
 *  @lib internal (ikev1lib.lib)
  */
NONSHARABLE_CLASS(MIkev1ReceiverCallback)
    {
public:        
    /**
     * Notifies about received IKE message.
     * @param aIkeMsg IKE message
     * @param aSrcAddr Source IP address/port
     * @param aLocalPort Local port
     */
    virtual void IkeMsgReceivedL( const ThdrISAKMP& aIkeMsg,
                                  const TInetAddr& aSrcAddr,
                                  TInt aLocalPort ) = 0;
    
    /**
     * Notifies about receive error.
     * @param aStatus Error value
     */
    virtual void ReceiveError( TInt aError ) = 0;
    };

/**
 *  IKEv1 receiver.
 *  Active object provides functionality for receiving UDP datagrams.
 *
 *  @lib internal (ikev1lib.lib)
  */
NONSHARABLE_CLASS(CIkev1Receiver) : public CActive
    {
public:
    /**
     * Two-phased constructor.
     * @param aDataInterface IKE data interface
     * @param aCallback Callback interface
     */
    static CIkev1Receiver* NewL( MIkeDataInterface& aDataInterface,
                                 MIkev1ReceiverCallback& aCallback );

    /**
    * Destructor.
    */
    ~CIkev1Receiver();

    /**
    * Starts receiving.
    */
    void StartReceive();
    
    /**
    * Cancels receiving.
    */
    void CancelReceive();
    
private:
    
    CIkev1Receiver( MIkeDataInterface& aDataInterface,
                    MIkev1ReceiverCallback& aCallback );

    void ConstructL();
    
    /**
     * Receives UDP data.
     */    
    void DoReceive();
    
// from base class CActive            
    
    /**
     * From CActive
     * Handles completion of receive.
     */    
    void RunL();

    /**
     * From CActive
     * Handles cancellation of receive.
     */    
    void DoCancel();
        
    /**
     * From CActive
     * Handles a leave occurring in RunL().
     * @param aError The leave code
     * @return KErrNone
     */    
    TInt RunError( TInt aError );
        
private: // data
    
    /**
     * UDP data.
     * Own.
     */
    HBufC8*                     iUdpData;
    
    /**
     * Source IP address/port.
     * Own.
     */
    TInetAddr                   iSrcAddr;
    
    /**
     * Local port.
     * Own.
     */
    TInt                        iLocalPort;
    
    /**
     * Informs if receiving data.
     * Own.
     */
    TBool                       iReceiving;
    
    /**
     * IKE data interface.
     * Not own.
     */
    MIkeDataInterface&          iDataInterface;
    
    /**
     * Callback interface.
     * Not own.
     */
    MIkev1ReceiverCallback&     iCallback;
    };

#endif // C_IKEV1RECEIVER_H
