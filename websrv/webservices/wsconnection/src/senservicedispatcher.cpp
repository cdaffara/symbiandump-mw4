/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:        
*
*/








#include <s32strm.h>
#include <f32file.h>

#include "senserviceconnectionimpl.h"

#include "sendebug.h"                 // internal Utils\inc - logging MACROs
#include "senlogger.h"
#include "senservicemanagerdefines.h" // internal Core\inc  - IPC enumerations, plus 
                                      // to include KServerUid3 for SEN.EXE SecureID 
                                      // (UID3) that can be nicely used as Property 
                                      // Category UID
#include "senservicedispatcher.h"

namespace
    {
    _LIT(KSenServiceDispatcherThreadName, "SenServiceDispatcher");
    const TInt KTransactionResetValue = 1000;
    }


CSenServiceDispatcher* CSenServiceDispatcher::NewL( RSenServiceConnection&  aConnection,
                                                                        TInt aConnectionID)
    {
    CSenServiceDispatcher* pNew = NewLC(aConnection,aConnectionID);
    CleanupStack::Pop();
    return(pNew) ;
    }

CSenServiceDispatcher* CSenServiceDispatcher::NewLC(RSenServiceConnection&  aConnection,
                                                                        TInt aConnectionID)
    {
    CSenServiceDispatcher* pNew = new (ELeave) CSenServiceDispatcher(aConnection);
    CleanupStack::PushL(pNew);
    pNew->ConstructL(aConnectionID);
    return pNew;
    }

CSenServiceDispatcher::CSenServiceDispatcher(RSenServiceConnection&  aConnection)
    :iConnection(aConnection)
    {
    }

void CSenServiceDispatcher::ConstructL(TInt aConnectionID)
    {
    iConnectionID = aConnectionID;
    
    ipTransactionsMap = new (ELeave) RTransactionsMap(ETrue, ETrue);

    User::LeaveIfError(iCsMessageQueue.CreateLocal());
    User::LeaveIfError(iCsSynchronizer.CreateLocal());
    User::LeaveIfError(iCsTransctnsMap.CreateLocal());
       
    
    RProcess process;
    TFileName threadName;
    threadName.Append( KSenServiceDispatcherThreadName);
    threadName.AppendNum( aConnectionID );
    threadName.Append( KSenUnderline );
    threadName.Append( process.Name().Left(32));
    
    RAllocator& heap = User::Allocator(); 
    User::LeaveIfError(iDispatcherThread.Create(threadName,
        TThreadFunction(DispatcherThreadL), KDefaultStackSize, &heap, this));
    
    //Resume the thread so that it should start waiting for any request.
    iDispatcherThread.Resume();
    
    //set iDispatchMessages = TRUE so that it should start dispacthing the messages
    //when thread gets signaled.
    iDispatchMessages = ETrue;
    }

CSenServiceDispatcher::~CSenServiceDispatcher()
    {
    //Set iDispatchMessages = FALSE to stop dispacthing messages if it already 
    //dispacthing and also to stop the thread which is waiting for any request.
    iDispatchMessages = EFalse;
    if(iCsSynchronizer.IsBlocked())
        {
        iCsSynchronizer.Wait();
        }

    TRequestStatus requestStatus;
    iDispatcherThread.Logon(requestStatus);
    iDispatcherThread.RequestSignal();
    
    //Wait until dispatcher thread exits
    User::WaitForRequest(requestStatus);

    if(ipTransactionsMap)
        {
        ipTransactionsMap->Reset();
        delete ipTransactionsMap;
         }

    iCsMessageQueue.Close();
    iCsSynchronizer.Close();
    iCsTransctnsMap.Close();

    iDispatcherThread.Close();
    iMessageQueue.Close();
    }


TInt CSenServiceDispatcher::DispatcherThreadL(CSenServiceDispatcher* aThis)
    {
    CTrapCleanup* pCleanup = CTrapCleanup::New();
    CActiveScheduler::Install(NULL); // remove one
    CActiveScheduler* pScheduler = new (ELeave) CActiveScheduler();
    CActiveScheduler::Install(pScheduler);

    RSenDocument::ManualXmlEngineTlsAttachL();
        
    TInt leaveCode(KErrNone);
    TRAP(leaveCode, CSenServiceDispatcher::ExecuteL(aThis));

    RSenDocument::ManualXmlEngineTlsCleanup();

    CActiveScheduler::Install(NULL); // uninstall scheduler
    delete pScheduler;
    delete pCleanup;

    return leaveCode;
    }

TInt CSenServiceDispatcher::ExecuteL(CSenServiceDispatcher* aThis)
    {
    
    aThis->OpenDispatcherLogL();
    
    TLSLOG_L(KSenDispatcherLogChannel, KSenDispatcherLogLevel, "CSenServiceDispatcher::ExecuteL - Resumed.");
    
    for(;;)
        {
        TLSLOG_L(KSenDispatcherLogChannel, KSenDispatcherLogLevel, "CSenServiceDispatcher::ExecuteL Before User::WaitForAnyRequest()");
        User::WaitForAnyRequest();
        TLSLOG_L(KSenDispatcherLogChannel, KSenDispatcherLogLevel, "CSenServiceDispatcher::ExecuteL After User::WaitForAnyRequest()");

        //If iDispatchMessages = FALSE then stop dispacthing messages.
        //Is called from the destructor and thread will end in cleaner way
        //by deleting the allocated objects on it's heap.
        if(!aThis->iDispatchMessages)
            {
            TLSLOG_L(KSenDispatcherLogChannel, KSenDispatcherLogLevel, "CSenServiceDispatcher::ExecuteL- Called from owner thread's destructor");
            break;
            }

        //Wait on this critical section until all the messages gets dispacthed or 
        //iDispatchMessages = FALSE is set from the destructor.
        aThis->iCsSynchronizer.Wait();

        TLSLOG_L(KSenDispatcherLogChannel, KSenDispatcherLogLevel, "CSenServiceDispatcher::ExecuteL- Dispacthing messages..");

        while(aThis->iMessageQueue.Count() && aThis->iDispatchMessages)
            {
            TThreadMessage thrMessage = aThis->iMessageQueue[0];

            //Send message and get the actual transaction id.
            TInt retVal = 
            aThis->iConnection.SendMsgAndReceiveTxnId(*(thrMessage.iMessage.iSenConnectionChunk));

            TLSLOG_FORMAT((KSenDispatcherLogChannel, KSenDispatcherLogLevel, 
                            _L("- SendMsgAndReceiveTxnId Returned: (%d)"), retVal));


            //Check if remove failed, possible in scenario where transaction has
            //been cancled by client using CancleTransaction(aTransactionID) API,
            //then cancel the request instaed of adding to the transaction map.
            if(aThis->RemoveFromQueue(thrMessage.iVrtalTrnsnID))
                {
                
                //If RemoveFromQueue is sucess and also SendMsgAndReceiveTxnId 
                //returns actual transaction ID then add to the transaction MAP.    
                TInt* pVrtlaTxnId = new (ELeave) TInt(0);
                *pVrtlaTxnId = thrMessage.iVrtalTrnsnID;
                CleanupStack::PushL(pVrtlaTxnId);

                TInt* pActlTxnId = new (ELeave) TInt(0);
                if (retVal>0)
                    {
                    *pActlTxnId = retVal;
                    }
                
                CleanupStack::PushL(pActlTxnId);

                //Add the to map as key=virtual transaction ID and 
                //value = actual transaction ID.
                TInt transRetVal = KErrNone;
                transRetVal  = aThis->AddToTheTransMap(pVrtlaTxnId,pActlTxnId);

                TLSLOG_FORMAT((KSenDispatcherLogChannel, KSenDispatcherLogLevel, 
                                _L("- AddToTheTransMap Returned: (%d)"), transRetVal));

                if (transRetVal == KErrNone)
                    {
                    TLSLOG_FORMAT((KSenDispatcherLogChannel, KSenDispatcherLogLevel, 
                                    _L("- Virtual Transaction ID: (%d)"), pVrtlaTxnId));
                    TLSLOG_FORMAT((KSenDispatcherLogChannel, KSenDispatcherLogLevel, 
                                    _L("- Actual Transaction ID: (%d)"), pActlTxnId));

                    CleanupStack::Pop(2);   //pVrtlaTxnId,pActlTxnId
                    }
                else
                    {
                    CleanupStack::PopAndDestroy(2); //pVrtlaTxnId,pActlTxnId
                    }
                }
            else
                {
                    //Mean time if the transaction has been canceled by client
                    //using CancelTransaction(aTranscationID) and then cancel the
                    //same using CancelRequest(aVirtualTransactionID)
                    aThis->iConnection.CancelRequest(thrMessage.iVrtalTrnsnID);
                }
            }   // End of while

        TLSLOG_L(KSenDispatcherLogChannel, KSenDispatcherLogLevel, "CSenServiceDispatcher::ExecuteL- All the message has been dispacthed");

        //Signal the critical section that message dispacthing is over so that it
        // can wait for the another request.
        aThis->iCsSynchronizer.Signal();

        }

    TLSLOG_L(KSenDispatcherLogChannel, KSenDispatcherLogLevel, "CSenServiceDispatcher::ExecuteL - Closing.");
    
    aThis->CloseDispatcherLogL();
    
    return KErrNone;
    }
    
void CSenServiceDispatcher::OpenDispatcherLogL()
    {
#ifdef _SENDEBUG
    RThread thread;
    RProcess process;
    TFileName logFile;
    logFile.Append( KSenDispactherThreadLog().Left(KSenDispactherThreadLog().Length()-4) ); // exclude ".log" file extension
    logFile.AppendNum( iConnectionID );
    logFile.Append( KSenUnderline );
    logFile.Append( process.Name().Left(32));
    logFile.Append( KSenUnderline );
    logFile.Append( thread.Name().Left(20));
    logFile.Append( KSenDispactherThreadLog().Right(4) ); // postfix with ".log" file extension

    // Open connection to the file logger server
    TLSLOG_OPEN( KSenDispatcherLogChannel, KSenDispatcherLogLevel, KSenDispactherThread, logFile );
    TLSLOG_L(KSenDispatcherLogChannel, KSenDispatcherLogLevel, "CSenServiceDispatcher::ExecuteL - About to create new dispatcher thread..");
    TLSLOG_FORMAT((KSenDispatcherLogChannel, KSenDispatcherLogLevel, 
                                _L("- Connection ID: (%d)"), iConnectionID));
#endif
    }

void CSenServiceDispatcher::CloseDispatcherLogL()
    {
    TLSLOG_L(KSenDispatcherLogChannel, KSenDispatcherLogLevel, "Log file closed.");
    TLSLOG_CLOSE( KSenDispatcherLogChannel );
    }
    

TInt CSenServiceDispatcher::GetDispactherThreadId()
    {
    return iDispatcherThread.Id();
    }


TInt CSenServiceDispatcher::AddToTheQueue(TThreadMessage aThreadMessage)
    {
    TInt appendRetVal(KErrNone);
    //Wait on message queue critical section if it is locked, and then add to 
    //the queue.
    //Critical section is required as RemoveFromQueue(aTrasnactionID) is called
    //from child thread(dispacther thread) also. To avoid crash as iMessageQueue
    //is used by two threads to append as well as to delete from the queue.
    iCsMessageQueue.Wait();
    appendRetVal = iMessageQueue.Append(aThreadMessage);
    iCsMessageQueue.Signal();

    if (appendRetVal == KErrNone)
        {
        //If dispatcher thread already dispatching the messages then no need to signal the 
        //thread as it is already working that is dispacthing the messages from queue.
        if(!iCsSynchronizer.IsBlocked())
            {
            //Signal the thread as it free and waiting for the reqest.
            iDispatcherThread.RequestSignal();
            }
        }  
    return appendRetVal;
    }
TBool CSenServiceDispatcher::RemoveFromQueue(TInt aTransactionID)
    {
    TLSLOG_FORMAT((KSenDispatcherLogChannel, KSenDispatcherLogLevel, _L("CSenServiceDispatcher::RemoveFromQueue - VrtlTxnID (%d)"), aTransactionID));
    //This method gets called from two places 1)main thread from
    //CancelTransaction(aTransactionID) method and 2)child thread from
    //ExecuteL method once message has been dispacthed.
    TBool found = EFalse;
    iCsMessageQueue.Wait();
    for(TInt i=0;i<iMessageQueue.Count();i++)
        {
        TThreadMessage threadMessage = iMessageQueue[i];
        if(threadMessage.iVrtalTrnsnID == aTransactionID)
            {   
            found = ETrue;
            iMessageQueue.Remove(i);
            if ((++iMessagesQueueCounter) >= KTransactionResetValue)
                {
                iMessageQueue.Compress();
                iMessagesQueueCounter = 0;
                }
            }
        }      
    iCsMessageQueue.Signal();    
    return found;
    }    

TMessage CSenServiceDispatcher::GetMessageFromQueue(TInt aTransactionID)
    {
    TMessage message;
    message.iSenAsyncOperation = NULL;
    message.iSenConnectionChunk = NULL;
    iCsMessageQueue.Wait();
    for(TInt i=0;i<iMessageQueue.Count();i++)
        {
        TThreadMessage threadMessage = iMessageQueue[i];
        if(threadMessage.iVrtalTrnsnID == aTransactionID)
            {
            message.iSenAsyncOperation = threadMessage.iMessage.iSenAsyncOperation;
            message.iSenConnectionChunk = threadMessage.iMessage.iSenConnectionChunk;
            }
        }      
    iCsMessageQueue.Signal();    
    return message;
    }

void CSenServiceDispatcher::ResetQueue()
    {
    iCsMessageQueue.Wait();
    iMessageQueue.Reset();
    iCsMessageQueue.Signal();     
    }    
    
RTransactionsMap& CSenServiceDispatcher::TransactionMap()
    {
        return *ipTransactionsMap;
    }

TInt CSenServiceDispatcher::AddToTheTransMap(TInt* pVrtlaTxnId,TInt* pActlTxnId)
    {
    TLSLOG_FORMAT((KSenDispatcherLogChannel, KSenDispatcherLogLevel, _L("CSenServiceDispatcher::AddToTheTransMap - VrtlTxnID [%d], ActlTxnId [%d]"), pVrtlaTxnId, pActlTxnId));
    TInt returnValue = KErrNone;
    //Wait on transaction map critical section if it is locked, and then add to 
    //the map.
    //Critical section is required as RemoveFromTransMap(aTrasnactionID) is called
    //from main thread also. To avoid crash as ipTransactionsMap
    //is used by two threads to append as well as to delete from the map.
    iCsTransctnsMap.Wait();
    returnValue = TransactionMap().Append(pVrtlaTxnId,pActlTxnId);
    iCsTransctnsMap.Signal(); 
    return returnValue;
    }

TInt CSenServiceDispatcher::RemoveFromTransMap(TInt* pVrtlaTxnId)
    {
    TLSLOG_FORMAT((KSenDispatcherLogChannel, KSenDispatcherLogLevel, _L("CSenServiceDispatcher::RemoveFromTransMap - VrtlTxnID [%d]"), pVrtlaTxnId));
    TInt returnValue = KErrNone;
    //Is called from the main thread after we recieve the response.
    returnValue = TransactionMap().Find(*pVrtlaTxnId);
    if( returnValue != KErrNotFound )
        {
        iCsTransctnsMap.Wait();
        returnValue = TransactionMap().RemoveByKey(*pVrtlaTxnId);
        iCsTransctnsMap.Signal(); 
        }
    return returnValue;
    }

TInt CSenServiceDispatcher::UpdateTransMap(TInt* pVrtlaTxnId,TInt* pActlTxnId)
    {
    TLSLOG_FORMAT((KSenDispatcherLogChannel, KSenDispatcherLogLevel, _L("CSenServiceDispatcher::UpdateTransMap - VrtlTxnID [%d], ActlTxnId [%d]"), pVrtlaTxnId, pActlTxnId));
    TInt returnValue = KErrNone;
    //Is called from the main thread from HandleMessageChildAOL method.
    //Typically gets called when ESenReAuthAndResendNeeded and ESenResendNeeded
    //is required.
    returnValue = TransactionMap().Find(*pVrtlaTxnId);
    if( returnValue != KErrNotFound )
        {
        iCsTransctnsMap.Wait();
        returnValue = TransactionMap().UpdateValue(pVrtlaTxnId,pActlTxnId);
        iCsTransctnsMap.Signal(); 
        }
    return returnValue;
    }

TInt CSenServiceDispatcher::GetActualTransactionID(TInt* pVrtlaTxnId)
    {
    //Getter for getting actual transaction ID
    TInt indexValue = TransactionMap().Find(*pVrtlaTxnId);

    if( indexValue != KErrNotFound )
        {
            const TInt* pValueAt = TransactionMap().ValueAt(indexValue);
            return *pValueAt;
        }

    return KErrNotFound;
    }

TInt CSenServiceDispatcher::GetVirtualTransactionID(TInt* pActlTxnId)
    {
    //Getter for getting virtual transaction ID
    TInt indexValue = TransactionMap().FindValue(*pActlTxnId);

    if( indexValue != KErrNotFound )
        {
        const TInt* pKeyAt = TransactionMap().KeyAt(indexValue);
        return *pKeyAt;
        }

    return KErrNotFound;
    }

void CSenServiceDispatcher::SetOwnerThreadId(TInt aOwnerThreadID)
    {
    iOwnerThreadId = aOwnerThreadID;
    }


CSenUnderTakerWaiter* CSenUnderTakerWaiter::NewL(
                            CSenServiceConnectionImpl* aSenServiceConnectionImpl,
                            TInt aDispatcherThreadID )
    {
    CSenUnderTakerWaiter* pNew = NewLC(aSenServiceConnectionImpl,aDispatcherThreadID);
    CleanupStack::Pop();
    return(pNew) ;
    }
    
CSenUnderTakerWaiter* CSenUnderTakerWaiter::NewLC(
                            CSenServiceConnectionImpl* aSenServiceConnectionImpl,
                            TInt aDispatcherThreadID )
    {
    CSenUnderTakerWaiter* pNew = new (ELeave) CSenUnderTakerWaiter();
    CleanupStack::PushL(pNew);
    pNew->ConstructL(aSenServiceConnectionImpl,aDispatcherThreadID);
    return pNew;
    }

CSenUnderTakerWaiter::~CSenUnderTakerWaiter()
    {
    iSenServiceConnectionImpl = NULL;
    Cancel();
    iUnderTaker.Close();
    }

CSenUnderTakerWaiter::CSenUnderTakerWaiter()
    :CActive(EPriorityNormal)
    {
    CActiveScheduler::Add(this);
    }

void CSenUnderTakerWaiter::ConstructL(CSenServiceConnectionImpl* aSenServiceConnectionImpl,
                                      TInt aDispatcherThreadID )
    {
    iSenServiceConnectionImpl = aSenServiceConnectionImpl;
    iDispatcherThreadID = aDispatcherThreadID;
    User::LeaveIfError(iUnderTaker.Create());
    }

void CSenUnderTakerWaiter::StartWaiter()
    {
    if(!IsActive())
        {
        iUnderTaker.Logon(iStatus,iDyingThreadNumber);
        SetActive();
        }
    }

void CSenUnderTakerWaiter::StopWaiter()
    {
    Cancel();
    }

void CSenUnderTakerWaiter::RunL()
    {
    if(iStatus == KErrDied)
        {
        RThread th;
        th.SetHandle(iDyingThreadNumber);
        TFullName name = th.FullName();
        TExitType type = th.ExitType();

        
        if(iDispatcherThreadID == th.Id())
            {
            //Notifies client that thread is died. Client has to restart the 
            //connection here.In this case client has to create new SC object.
            if(type == EExitKill)
                {
                if(iSenServiceConnectionImpl)
                    {
                    iSenServiceConnectionImpl->iErrorNumber = EExitKill;
                    iSenServiceConnectionImpl->iTxnId = -1;    
                    iSenServiceConnectionImpl->HandleMessageFromChildAOL(iStatus.Int());
                    }
                }
            else    // panic
                {
                TExitCategoryName categ = th.ExitCategory();
                if(iSenServiceConnectionImpl)
                    {
                    iSenServiceConnectionImpl->iErrorNumber = EExitPanic;
                    iSenServiceConnectionImpl->iTxnId = -1;
                    iSenServiceConnectionImpl->HandleMessageFromChildAOL(iStatus.Int());
                    }
                }
            }
        th.Close();
        StartWaiter();
        }           
    }

void CSenUnderTakerWaiter::DoCancel()
    {
    iUnderTaker.LogonCancel();
    }
