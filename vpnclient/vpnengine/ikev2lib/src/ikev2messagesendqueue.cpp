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

#include "ikev2messagesendqueue.h"
#include "ikemsgheader.h"
#include "ikedebug.h"

_LIT8(KKeepaliveData, 0xff);

CIkev2MessageSendQueue* CIkev2MessageSendQueue::NewL(MIkeDataInterface& aDataInterface,
                                                     const TInetAddr& aDestinationAddress,
                                                     TUint8 aDscp,
                                                     TUint aNatKeepAliveInterval,
                                                     MIkeDebug& aDebug)
    {
    CIkev2MessageSendQueue* self = new (ELeave) CIkev2MessageSendQueue(aDataInterface,
                                                                       aDestinationAddress,
                                                                       aDscp,
                                                                       aNatKeepAliveInterval,
                                                                       aDebug);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    
    return self;
    }


CIkev2MessageSendQueue::CIkev2MessageSendQueue(MIkeDataInterface& aDataInterface,
                                               const TInetAddr& aDestinationAddress,
                                               TUint8 aDscp,
                                               TUint aNatKeepAliveInterval,
                                               MIkeDebug& aDebug)
:CActive(EPriorityStandard), 
 iDataInterface(aDataInterface), 
 iNatKeepAliveInterval(aNatKeepAliveInterval),
 iDestinationAddress(aDestinationAddress),
 iDscp(aDscp),
 iDebug(aDebug)
    {
    CActiveScheduler::Add(this);
    
    if (iNatKeepAliveInterval == 0)
        {
        iNatKeepAliveInterval = 120;
        }
    }


void CIkev2MessageSendQueue::ConstructL()
    {
    iSender = CIkev2Sender::NewL( iDataInterface,
                                  *this);        
    User::LeaveIfError(iNatKeepaliveTimer.CreateLocal());
    }


CIkev2MessageSendQueue::~CIkev2MessageSendQueue()
    {
    Cancel();
    iNatKeepaliveTimer.Close();
    iSasBehindNat.Close();
    delete iSender;
    iIkeMsgSendBuffer.Reset();
    iIkeMsgSendBuffer.Close();
    }

void CIkev2MessageSendQueue::SendIkeMessageL(const TPtrC8 aIkeMsg, 
                                             TBool aFloatedPort)
    {        
    Cancel();
    if (!iSender->IsActive())
        {        
        iIkeMsgInSending.Set(aIkeMsg);
        TUint port = (aFloatedPort) ? FLOATED_IKE_PORT : IKE_PORT;

#ifdef _DEBUG
        TBuf<80> destAddr;
        iDestinationAddress.Output(destAddr);
        DEBUG_LOG3(_L("No sending active. Message sent immediately to %S:%d DSCP=%d."), &destAddr, port, iDscp);
#endif //_DEBUG        
        iDestinationAddress.SetPort(port);
        iSender->SendIkeMsg(port, iDestinationAddress, iDscp, iIkeMsgInSending);        
        }
    else
        {
        DEBUG_LOG(_L("Sending in progress, message added to send queue."));
        TIkeMsgWaitQueueObject waitObject = { aIkeMsg, aFloatedPort };
        User::LeaveIfError(iIkeMsgSendBuffer.Append(waitObject));
        }
    }


void CIkev2MessageSendQueue::CancelSend(const TPtrC8& aIkeMsg)
    {
    DEBUG_LOG(_L("Sending of one datagram canceled."));
    if (aIkeMsg.Ptr() == iIkeMsgInSending.Ptr())
        {
        __ASSERT_DEBUG(iSender->IsActive(), User::Invariant());
        //The NAT keepalive timer should not be active, because sending
        //is in progress.
        __ASSERT_DEBUG(!IsActive(), User::Invariant()); 
        iSender->Cancel();
        iIkeMsgInSending.Set(NULL, 0);
        // Send next message from queue.    
        SendIkeMsgCompleted( KErrCancel );
        }
    else
        {        
        for (TUint i = 0; i < iIkeMsgSendBuffer.Count(); ++i)
            {
            if (iIkeMsgSendBuffer[i].iIkeMsg.Ptr() == aIkeMsg.Ptr())
                {
                iIkeMsgSendBuffer.Remove(i);
                break;
                }            
            }        
        }              
    }

void CIkev2MessageSendQueue::SendIkeMsgCompleted( TInt /*aStatus*/ )
    {
    DEBUG_LOG(_L("Ike message send complete."));
    iIkeMsgInSending.Set(NULL, 0);
    
    if (iIkeMsgSendBuffer.Count() > 0)
        {
        TIkeMsgWaitQueueObject& waitObject = iIkeMsgSendBuffer[0];
        TUint port = (waitObject.iFloatedPort) ? FLOATED_IKE_PORT : 500;
        iSender->SendIkeMsg(port, iDestinationAddress, iDscp, waitObject.iIkeMsg);
        iIkeMsgSendBuffer.Remove(0);
        iIkeMsgInSending.Set(waitObject.iIkeMsg);
        }
    else if (iSasBehindNat.Count() > 0 && iNatKeepAliveInterval > 0)
        {
        ArmKeepaliveTimer();
        }
    }


void CIkev2MessageSendQueue::CancelAll()
    {
    iSender->Cancel();
    iIkeMsgSendBuffer.Reset();
    }


void CIkev2MessageSendQueue::NewSaBehindNatL(TUint aSaId)
    {
    DEBUG_LOG1(_L("CIkev2MessageSendQueue::NewSaBehindNatL: SaId=%d"), aSaId);
    __ASSERT_DEBUG(iSasBehindNat.Find(aSaId) == KErrNotFound, User::Invariant());
    User::LeaveIfError(iSasBehindNat.Append(aSaId));    
    if (!IsActive() && !iSender->IsActive() && iNatKeepAliveInterval > 0)
        {
        //No sending acticve arm the nat keepalive timer.
        ArmKeepaliveTimer();
        }
    }


void CIkev2MessageSendQueue::SaBehindNatDeleted(TUint aSaId)
    {
    DEBUG_LOG1(_L("CIkev2MessageSendQueue::SaBehindNatDeleted: SaId=%d"), aSaId);
    TInt index = iSasBehindNat.Find(aSaId);
    __ASSERT_DEBUG(index >= 0, User::Invariant());
    iSasBehindNat.Remove(index);   
    if (iSasBehindNat.Count() == 0)
        {
        //Cancel keepalive timer
        Cancel();
        }
    }

void CIkev2MessageSendQueue::RunL()
    {
    //Send the NAT keepalive
    
    __ASSERT_DEBUG(!iSender->IsActive(), User::Invariant());
    __ASSERT_DEBUG(iSasBehindNat.Count() > 0 && iNatKeepAliveInterval > 0, User::Invariant());
    
    if (iRemainingTime == 0)
        {
        DEBUG_LOG(_L("Sending NAT keepalive"));
        iSender->SendIkeMsg(FLOATED_IKE_PORT, iDestinationAddress, iDscp, KKeepaliveData);
        }
    else if (iRemainingTime > KMaxTInt/1000000)
        {
        iRemainingTime -= KMaxTInt/1000000;
        iNatKeepaliveTimer.After(iStatus, KMaxTInt);
        SetActive();
        }
    else
        {        
        iNatKeepaliveTimer.After(iStatus, iRemainingTime*1000000);
        iRemainingTime = 0;
        SetActive();
        }
    }

void CIkev2MessageSendQueue::DoCancel()
    {    
    iNatKeepaliveTimer.Cancel();
    iRemainingTime = 0;
    DEBUG_LOG(_L("Keepalive timer canceled"));
    }

void CIkev2MessageSendQueue::ArmKeepaliveTimer()
    {
    DEBUG_LOG(_L("CIkev2MessageSendQueue::ArmKeepaliveTimer"));    
    __ASSERT_DEBUG(!IsActive(), User::Invariant());
    //Arm NAT keepalive timer.
    if (iNatKeepAliveInterval > KMaxTInt/1000000 ) 
        {
        iRemainingTime = iNatKeepAliveInterval - KMaxTInt/1000000;
        iNatKeepaliveTimer.After(iStatus, KMaxTInt);
        }
    else
        {
        iRemainingTime = 0;
        iNatKeepaliveTimer.After(iStatus, iNatKeepAliveInterval * 1000000);
        }        
    SetActive();    
    }
