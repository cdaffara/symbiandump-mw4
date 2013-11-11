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
* Description:  Receiver of UDP datagrams
*
*/


#ifndef C_IKEV2RECEIVER_H
#define C_IKEV2RECEIVER_H

#include <e32base.h>
#include <in_sock.h>

// FORWARD DECLARATIONS
class MIkeDataInterface;
class ThdrISAKMP;

/**
 *  IKEv2 receiver callback interface.
 *  Callback interface which is used by CIkev2Receiver object to notify
 *  about received IKE message.
 * 
 *  @lib internal (ikev2lib.lib)
  */
NONSHARABLE_CLASS(MIkev2ReceiverCallback)
    {
public:        
    /**
     * Notifies about received IKE message.
     *
     * @param aIkeMsg IKE message
     * @param aSrcAddr Source IP address/port
     * @param aLocalPort Local port
     */
    virtual void IkeMsgReceived( const ThdrISAKMP& aIkeMsg,
                                 const TInetAddr& aSrcAddr,
                                 TInt aLocalPort ) = 0;
    
    /**
     * Notifies about receive error.
     * @param aStatus Error value
     */
    virtual void ReceiveError( TInt aError ) = 0;    
    };

/**
 *  IKEv2 receiver.
 *  Active object provides functionality for receiving UDP datagrams.
 *
 *  @lib internal (ikev2lib.lib)
  */
NONSHARABLE_CLASS(CIkev2Receiver) : public CActive
    {
public:
    /**
     * Two-phased constructor.
     * @param aDataInterface IKE data interface
     * @param aCallback Callback interface
     */
    static CIkev2Receiver* NewL( MIkeDataInterface& aDataInterface,
                                 MIkev2ReceiverCallback& aCallback );

    /**
    * Destructor.
    */
    ~CIkev2Receiver();

    /**
    * Starts receiving.
    */
    void StartReceive();
    
    /**
    * Stops receiving.
    */
    void StopReceive();
    
private:
    
    CIkev2Receiver( MIkeDataInterface& aDataInterface,
                    MIkev2ReceiverCallback& aCallback );

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
     * Informs if data is received.
     * Own.
     */
    TBool                       iReceivingData;
    
    /**
     * IKE data interface.
     * Not own.
     */
    MIkeDataInterface&          iDataInterface;
    
    /**
     * Callback interface.
     * Not own.
     */
    MIkev2ReceiverCallback&     iCallback;
    };

#endif // C_Ikev2Receiver_H
