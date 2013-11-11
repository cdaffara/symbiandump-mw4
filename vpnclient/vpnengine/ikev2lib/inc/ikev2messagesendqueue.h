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
* Description:  IKEv2 IKE message send que.
*
*/

#ifndef IKEV2MESSAGESENDQUEUE_H_
#define IKEV2MESSAGESENDQUEUE_H_

#include <e32base.h>
#include <in_sock.h>
#include "ikev2sender.h"


class MIkeDataInterface;
class MIkeDebug;

class TIkeMsgWaitQueueObject
    {
public:    
    TPtrC8       iIkeMsg;
    TBool        iFloatedPort;
    };


NONSHARABLE_CLASS(CIkev2MessageSendQueue) : public CActive,
                                            public MIkev2SenderCallback
    {
public:
    static CIkev2MessageSendQueue* NewL(MIkeDataInterface& aDataInterface,
                                        const TInetAddr& aDestinationAddress,
                                        TUint8 aDscp,
                                        TUint aNatKeepAliveInterval,
                                        MIkeDebug& aDebug);
    ~CIkev2MessageSendQueue();
    
    void SendIkeMessageL(const TPtrC8 aIkeMsg, TBool aFloatedPort);    
    void CancelSend(const TPtrC8& aIkeMsg);
    
    void CancelAll();
    
    void NewSaBehindNatL(TUint aSaId);
    void SaBehindNatDeleted(TUint aSaId);
    
protected:
    void SendIkeMsgCompleted( TInt aStatus );
    
    void RunL();
    void DoCancel();
    
private:
    CIkev2MessageSendQueue(MIkeDataInterface& aDataInterface,
                           const TInetAddr& aDestinationAddress,
                           TUint8 aDscp,
                           TUint aNatKeepAliveInterval,
                           MIkeDebug& aDebug);
    void ConstructL();      
    void ArmKeepaliveTimer();
    
    MIkeDataInterface& iDataInterface;
    TUint              iNatKeepAliveInterval;    
    TInetAddr          iDestinationAddress;
    TUint8             iDscp;
    MIkeDebug&         iDebug;
    
    CIkev2Sender*      iSender; 
    RTimer             iNatKeepaliveTimer;
    
    RArray<TIkeMsgWaitQueueObject> iIkeMsgSendBuffer; //IkeMsgs waiting for send
    TPtrC8 iIkeMsgInSending; //IkeMsgs, which is currently in sending
    
    RArray<TUint> iSasBehindNat;
    TUint iRemainingTime;
    };

#endif /* IKEV2MESSAGESENDQUEUE_H_ */
