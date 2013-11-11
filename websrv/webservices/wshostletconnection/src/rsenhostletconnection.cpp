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











#include <e32math.h>

//#include "SenHostletConnectionLog.h"
#include "senservicemanagerdefines.h" // internal Core\inc  - IPC enumerations

#include "rsenhostletconnection.h"
#include "senchunk.h"

#include "sendebug.h"
#include "senlogger.h"

#include <SenSoapMessage.h>

#if defined (__WINS__) && !defined(EKA2)
static const TUint KServerMinHeapSize =   0x1000;  // 4K
static const TUint KServerMaxHeapSize = 0x100000;  // 1000K
#endif

static TInt StartServerL();
static TInt CreateServerProcessL();

RSenHostletConnection::RSenHostletConnection()
    : RSessionBase(), iTlsLogChannel(1000)
    {
    // No implementation required
    }

TInt RSenHostletConnection::Connect()
    {
    TLSLOG_L(iTlsLogChannel, KMinLogLevel, "RSenHostletConnection::Connect");
    TInt error(KErrNone);
    TRAPD(err, error = ::StartServerL());
    
    if(err != KErrNone)
        {
        error = err;
        }
    if(KErrNone == error)
        {
        TLSLOG(iTlsLogChannel, KMinLogLevel,
            _L("RSenHostletConnection::Connect - Now creating session"));
        error = CreateSession(KSenServiceManager,
                              Version(),
                              KDefaultMessageSlots);
#ifdef _SENDEBUG
        if(error != KErrNone)
            {
            TLSLOG_FORMAT((iTlsLogChannel, KMinLogLevel,
            _L("RSenHostletConnection::Connect - CreateSession returned error: (%d)"), error));
            }
        else
            {
            TLSLOG_L(iTlsLogChannel, KMinLogLevel, "RSenHostletConnection::Connect - CreateSession returned KErrNone");
            }
#endif // _SENDEBUG
        }
#ifdef _SENDEBUG
    else
        {
        TLSLOG_FORMAT((iTlsLogChannel, KMinLogLevel,
        _L8("RSenHostletConnection::Connect - StartServerL failed, error: (%d)"), error));
        }
#endif // _SENDEBUG
    return error;
    }

TVersion RSenHostletConnection::Version() const
    {
    TLSLOG(iTlsLogChannel, KMinLogLevel, _L("RSenHostletConnection::Version"));
    return(TVersion(KWsfServMajorVersionNumber,
                    KWsfServMinorVersionNumber,
                    KWsfServBuildVersionNumber));
    }
    
void RSenHostletConnection::CancelSession(TRequestStatus& aStatus)
    {
    TLSLOG_L(iTlsLogChannel, KMinLogLevel, "RSenServiceConnection::CancelSession(sync)");
    SendReceive(ESenServCancelSession, aStatus);
    }


// SYNC: Called from CSenHostletConnection::ConstructL
TInt RSenHostletConnection::Establish(TDesC8& aHostletServiceDescription)
    {
    TLSLOG_L(iTlsLogChannel, KMinLogLevel, "RSenHostletConnection::EstablishL");

    TLSLOG_ALL(iTlsLogChannel, KMaxLogLevel, aHostletServiceDescription);

    TInt sdLength = aHostletServiceDescription.Length();

    TIpcArgs args;
    args.Set(0, &aHostletServiceDescription);
    args.Set(1, sdLength);

    return SendReceive(ESenServEstablishHostletConnection, args);
    }

// ASYNC: Completes when request handle is ready to be acquired
void RSenHostletConnection::AwaitRequest(TRequestStatus& aStatus,
                                         TDes8& aErrPtr,
                                         TDes8& aTxnPtr)
    {
    TLSLOG_L(iTlsLogChannel, KMinLogLevel, "RSenHostletConnection::AwaitRequest");

    TIpcArgs args;
    args.Set(0, &aErrPtr);
    args.Set(1, &aTxnPtr);
    SendReceive(ESenServAwaitHostletRequest, args, aStatus);
    }

// SYNC: returns RHandleBase ID if successful
TInt RSenHostletConnection::AcquireRequestHandle( TDes8& aErrPtr, TDes8& aTxnPtr )
    {
    TLSLOG_L(iTlsLogChannel, KMinLogLevel, "RSenHostletConnection::AcquireRequestHandle");

    TIpcArgs args;
    args.Set(0, &aErrPtr);
    args.Set(1, &aTxnPtr);
    return SendReceive(ESenServTransaction, args);
    }

void RSenHostletConnection::ProvideResponse( TRequestStatus& aStatus, 
                                             TDes8& aErrPtr,
                                             TDes8& aTxnPtr,
                                             CSenChunk& aClientOp,
                                             const TInt aResponseCode )
    {
    TLSLOG_L(iTlsLogChannel, KMinLogLevel, "RSenHostletConnection::ProvideResponse");

    TIpcArgs args;
    args.Set(0, &aErrPtr);
    args.Set(1, &aTxnPtr);
    aClientOp.ChunkToArgs(args,2);
    args.Set(3, aResponseCode);
    SendReceive(ESenServProvideHostletResponse, args, aStatus);
    }

TInt RSenHostletConnection::ConnectionID()
    {
    TInt connectionID = SendReceive( ESenServConnectionID );
                   
    iTlsLogChannel = KSenHostletConnectionLogChannelBase + connectionID;
    //TLSLOG_FORMAT((iTlsLogChannel, KMinLogLevel, _L("- Connection ID: (%d)"), connectionID));
    return connectionID;
    }
    
TInt RSenHostletConnection::RegisterTransferObserver(
        const TDesC* aServerName, TAny* aConnection)
    {
    TLSLOG_L(iTlsLogChannel, KMinLogLevel, "RSenHostletConnection::RegisterTransferObserver");
    CActiveSchedulerWait asWait;
    TIpcArgs args(aServerName, aConnection, &asWait);
    TRequestStatus rs = KRequestPending;
    SendReceive(ESenObserveTransfer, args, rs);
    Mem::FillZ(&asWait, sizeof(asWait));
    asWait.Start();
    User::WaitForRequest(rs);
    TLSLOG_L(iTlsLogChannel, KMinLogLevel, "RSenHostletConnection::RegisterTransferObserver Completed");
    return rs.Int();
    }

//void RSenHostletConnection::SetTlsLogChannel(TInt aTlsLogChannel)
//    {
//    iTlsLogChannel = aTlsLogChannel;
//    }

static TInt StartServerL()
    {
    TInt result(KErrNotFound);
    
#ifdef _SENDEBUG    
    RFileLogger log;
    CleanupClosePushL(log);
    User::LeaveIfError( log.Connect() );
    log.CreateLog(KSenHostletConnectionLogDir, _L("SenStartServer.log"), EFileLoggingModeAppend);
    log.Write(_L("StartServerL() invoked by HC."));
#endif

    TFindServer findSenServiceManager(KSenServiceManager);
    TFullName name;

    result = findSenServiceManager.Next(name);
    if (result == KErrNone)
        {
        // Server already running
#ifdef _SENDEBUG        
        log.Write(_L8("- Server already running, KErrNone"));
        log.CloseLog();
        CleanupStack::PopAndDestroy(); // log.Close()
#endif // _SENDEBUG        
        return KErrNone;
        }

    RSemaphore semaphore;
    CleanupClosePushL(semaphore);

    // First, try to open the semaphore (if someone else created it already):
    result = semaphore.OpenGlobal( KSenServiceManagerSemaphoreName );
    if( result == KErrNone )
        {
        // If another client is starting up SEN.EXE, this client should return
        // to re-connect loop (wait one sec, max 5 times)
#ifdef _SENDEBUG        
        log.Write(_L8("- OpenGlobal OK => Another client already starting up SEN.EXE. About to WAIT and RE-CONNECT."));
#endif        
        result = KErrServerBusy;
        }
    else // could not find any already opened global semaphore ==> it is safe to create new one
        {   
        // Semaphore has not yet been created, so instantiate it now(!)
        result = semaphore.CreateGlobal( KSenServiceManagerSemaphoreName, 0 );
#ifdef _SENDEBUG        
        if( result != KErrNone )
            {
            log.Write(_L8("CreateGlobal failed"));
            log.WriteFormat(_L8("- error: %d"), result);
            }
#endif        
        }
        
    if (result != KErrNone)
        {
        CleanupStack::PopAndDestroy(); // semaphore.Close()
#ifdef _SENDEBUG        
        log.CloseLog();
#endif // _SENDEBUG        
        if( result != KErrServerBusy )
            {
#ifdef _SENDEBUG        
            log.Write(_L8("- CreateGlobal failed => Another client already starting up SEN.EXE. About to WAIT and RE-CONNECT."));
            CleanupStack::PopAndDestroy(); // log.Close()
#endif // _SENDEBUG        
            result = KErrServerBusy; // again, this error will activate re-connect loop (max 5 times)
            }
        return result;
        }

    result = CreateServerProcessL();
    if (result != KErrNone)
        {
        CleanupStack::PopAndDestroy(); // semaphore.Close()
#ifdef _SENDEBUG        
        log.Write(_L8("CreateServerProcessL failed"));
        log.CloseLog();
        CleanupStack::PopAndDestroy(); // log.Close()
#endif // _SENDEBUG         
        return result;
        }

    // Now start waiting for signal from server: it will release this semaphore from wait:
    semaphore.Wait();  
    CleanupStack::PopAndDestroy(); // semaphore.Close()

#ifdef _SENDEBUG        
    log.Write(_L("Log file closed."));
    log.CloseLog();
    CleanupStack::PopAndDestroy(); // log.Close()    
#endif // _SENDEBUG
    return  KErrNone;
    }

static TInt CreateServerProcessL()
    {
    TInt result(KErrNone);

#ifdef _SENDEBUG
    RFileLogger log;
    CleanupClosePushL(log);
    User::LeaveIfError( log.Connect() );
    // Note: this appends to the log created in StartServer()
    log.CreateLog(KSenHostletConnectionLogDir, _L("SenStartServer.log"), EFileLoggingModeAppend);
    log.Write(_L("CreateServerProcessL() invoked by HC."));
#endif // _SENDEBUG

    const TUidType serverUid(KNullUid, KNullUid, KServerUid3);

#if defined(__WINS__) && !defined(EKA2)

    RLibrary lib;
    CleanupClosePushL(lib);

    RThread server;
    CleanupClosePushL(server);

    result = lib.Load(KSenServiceManagerFileName, serverUid);

    if( result == KErrNone )
        {
#ifdef _SENDEB
        log.Write(_L8("Library successfully loaded, KErrNone"));
#endif // _SENDEBUG

        //  Get the WinsMain function
        TLibraryFunction functionWinsMain = lib.Lookup(1);

        //  Call it and cast the result to a thread function
        TThreadFunction serverThreadFunction =
            reinterpret_cast<TThreadFunction>(functionWinsMain());

        TName threadName(KSenServiceManagerFileName);

        // Append a random number to make it unique
        threadName.AppendNum(Math::Random(), EHex);


        result = server.Create(threadName,   // create new server thread
                               serverThreadFunction, // thread's main function
                               KDefaultStackSize,
                               NULL,
                               &lib,
                               NULL,
                               KServerMinHeapSize,
                               KServerMaxHeapSize,
                               EOwnerProcess);

        // if successful, server thread now has the handle to library:

        if ( result == KErrNone )
            {
            server.SetPriority( EPriorityMore );
            }
#ifdef _SENDEBUG
        else
            {
            log.Write(_L8("Failed to create server thread."));
            }
#endif // _SENDEBUG
            
        }
#ifdef _SENDEBUG
    else
        {
        log.Write(_L8("Could not load library"));
        }
#endif // _SENDEBUG

    CleanupStack::Pop(); // server (thread)
    CleanupStack::PopAndDestroy(); // lib
    CleanupClosePushL(server);

#else // NON WINS TARGET(S):
    RProcess server;
    CleanupClosePushL(server);
    result = server.Create(KSenServiceManagerFileName, KNullDesC, serverUid);//CodeScannerWarning
#endif

    if( result == KErrNone )
        {
#ifdef _SENDEBUG
        log.Write(_L8("Server successfully created, KErrNone"));
#endif // _SENDEBUG
        server.Resume();
        }
#ifdef _SENDEBUG
    else
        {
        log.Write(_L8("Could not create server"));
        }
#endif // _SENDEBUG

    CleanupStack::PopAndDestroy(); // server.Close();

#ifdef _SENDEBUG
    CleanupStack::PopAndDestroy(); // log.Close();
#endif

    return result;
    }
    

// End of file



