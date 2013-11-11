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

#include "rsenservicemanager.h"

#include "senservicemanagerdefines.h" // IPC enumerations
#include "sendebug.h" // internal Utils\inc - filelogging MACROs
#include "senlogger.h"
#include "senchunk.h"

#if defined(__WINS__) && !defined(EKA2)
    static const TUint KServerMinHeapSize =  0x1000;  //  4K
    static const TUint KServerMaxHeapSize = 0x100000;  // 1000K
#endif

static TInt StartServerL();
static TInt CreateServerProcessL();

RSenServiceManager::RSenServiceManager()//(RFileLogger* aLog)
    : RSessionBase()//,
      //iLog(aLog)
    {
    // No implementation required
    }

/*
void RSenServiceManager::SetLog(RFileLogger* aLog)
    {
    iLog = aLog;
    }
*/    
void RSenServiceManager::SetChannel(TInt aChannel)
{
	iTLSLogChannel = aChannel;
}
TInt RSenServiceManager::Connect()
    {
    TLSLOG(KSenServiceManagerLogChannel, KMinLogLevel,(_L("RSenServiceManager::Connect")));
    TInt error(KErrNone);
    TRAPD (err, error = ::StartServerL();)
    
    if (err != KErrNone)
        {
        error = err;
        }

    if (KErrNone == error)
        {
        error = CreateSession(KSenServiceManager,
                              Version(),
                              KDefaultMessageSlots);

#ifdef _SENDEBUG
        if(error != KErrNone)
            {
            TLSLOG_L(KSenServiceManagerLogChannel, KMinLogLevel,"CSenServiceManagerImpl::DoCancel");
            TLSLOG_FORMAT((KSenServiceManagerLogChannel, KMinLogLevel , _L8("- CreateSession returned error: (%d)"), error));
            }
        else
            {
            TLSLOG_L(KSenServiceManagerLogChannel, KMinLogLevel,"RSenServiceManager::Connect");
            TLSLOG_L(KSenServiceManagerLogChannel, KMinLogLevel,"- CreateSession returned KErrNone");
            }
#endif // _SENDEBUG
        }
#ifdef _SENDEBUG
    else
        {
        TLSLOG_FORMAT((KSenServiceManagerLogChannel, KMinLogLevel , _L8("RSenServiceManager::Connect - StartServer failed, error: (%d)"),
                    error));
        }
#endif // _SENDEBUG

    return error;
    }

TVersion RSenServiceManager::Version() const
    {
    TLSLOG(iTLSLogChannel, KMinLogLevel,(_L("RSenServiceManager::Version")));
    return(TVersion(KWsfServMajorVersionNumber,
                    KWsfServMinorVersionNumber,
                    KWsfServBuildVersionNumber));
    }

TInt RSenServiceManager::InstallFramework(TDesC8& aMessage)
    {
#ifdef _SENDEBUG
    TLSLOG(iTLSLogChannel, KMinLogLevel,(_L("RSenServiceManager::InstallFramework")));

    TPtrC8 outBufferLeft = aMessage.Left(30);
    TLSLOG_FORMAT((iTLSLogChannel, KNormalLogLevel , _L8("Out: %S ..., length: (%d)"),
                        &outBufferLeft, aMessage.Length()));
#endif // _SENDEBUG
    TInt length = aMessage.Length();

    TIpcArgs args;
    args.Set(0,&aMessage);
    args.Set(1, length);

    return SendReceive(ESenServInstallFramework, args);
    }

// Note: cannot log here (without TLS), since flogger server session has not yet been opened(!)
TInt RSenServiceManager::ConnectionID()
    {
    TInt connectionID = SendReceive( ESenServConnectionID );
                   
//    iTlsLogChannel = KSenHostletConnectionLogChannelBase + connectionID;
//    TLSLOG_FORMAT((iTlsLogChannel, KSenServiceManagerLogLevel,
//                   _L("- Connection ID: (%d)"), connectionID));
    return connectionID;
    }



TInt RSenServiceManager::AssociateService( TDesC8& aServiceID, TDesC8& aProviderID )
    {
#ifdef _SENDEBUG
    TLSLOG_L(iTLSLogChannel, KMinLogLevel,"RSenServiceManager::AssociateService");

    TPtrC8 outBufferLeft = aServiceID.Left(30);
    TLSLOG_FORMAT((iTLSLogChannel, KNormalLogLevel , _L8("Out: %S ..., length: (%d)"),
                    &outBufferLeft, aServiceID.Length()));
#endif // _SENDEBUG

    TInt servIdLength = aServiceID.Length();
    TInt provIdLength = aProviderID.Length();

    TIpcArgs args;
    args.Set(0, &aServiceID);
    args.Set(1, servIdLength);
    args.Set(2, &aProviderID);
    args.Set(3, provIdLength);

    return SendReceive(ESenServAssociateService, args);
    }

TInt RSenServiceManager::DissociateService(TDesC8& aServiceID,
                                           TDesC8& aProviderID)
    {
#ifdef _SENDEBUG
    TLSLOG_L(iTLSLogChannel, KMinLogLevel,"RSenServiceManager::DissociateService");

    TPtrC8 outBufferLeft = aServiceID.Left(30);
    TLSLOG_FORMAT((iTLSLogChannel, KNormalLogLevel , _L8("Out: %S ..., length: (%d)"),
                    &outBufferLeft, aServiceID.Length()));
#endif // _SENDEBUG

    TInt servIdLength = aServiceID.Length();
    TInt provIdLength = aProviderID.Length();

    TIpcArgs args;
    args.Set(0, &aServiceID);
    args.Set(1, servIdLength);
    args.Set(2, &aProviderID);
    args.Set(3, provIdLength);

    return SendReceive(ESenServDissociateService, args);
    }

TInt RSenServiceManager::RegisterIdentityProvider(TDesC8& aMessage)
    {
#ifdef _SENDEBUG
    TLSLOG_L(iTLSLogChannel, KMinLogLevel,"RSenServiceManager::RegisterIdentityProvider");

    TPtrC8 outBufferLeft = aMessage.Left(30);
    TLSLOG_FORMAT((iTLSLogChannel, KNormalLogLevel , _L8("Out: %S ..., length: (%d)"),
                        &outBufferLeft, aMessage.Length()));
#endif // _SENDEBUG

    TInt length = aMessage.Length();

    TIpcArgs args;
    args.Set(0, &aMessage);
    args.Set(1, length);

    return SendReceive(ESenServRegisterIdentityProvider, args);
    }

TInt RSenServiceManager::UnregisterIdentityProvider(TDesC8& aMessage)
    {
#ifdef _SENDEBUG
    TLSLOG_L(iTLSLogChannel, KMinLogLevel,"RSenServiceManager::UnregisterIdentityProvider");

    TPtrC8 outBufferLeft = aMessage.Left(30);
    TLSLOG_FORMAT((iTLSLogChannel, KNormalLogLevel , _L8("Out: %S ..., length: (%d)"),
                    &outBufferLeft, aMessage.Length()));
#endif // _SENDEBUG

    TInt length = aMessage.Length();
    TIpcArgs args;
    args.Set(0, &aMessage);
    args.Set(1, length);

    return SendReceive(ESenServUnregisterIdentityProvider, args);
    }


TInt RSenServiceManager::UnregisterServiceDescription(TDesC8& aMessage)
    {
#ifdef _SENDEBUG
    TLSLOG(iTLSLogChannel, KMinLogLevel,(_L("RSenServiceManager::UnregisterServiceDescription")));

    TPtrC8 outBufferLeft = aMessage.Left(30);
    TLSLOG_FORMAT((iTLSLogChannel, KNormalLogLevel , _L8("Out: %S ..., length: (%d)"),
                    &outBufferLeft, aMessage.Length()));
#endif // _SENDEBUG

    TInt length = aMessage.Length();
    TIpcArgs args;
    args.Set(0, &aMessage);
    args.Set(1, length);

    return SendReceive(ESenServUnregisterServiceDescription, args);
    }

TInt RSenServiceManager::RegisterServiceDescription(TDesC8& aMessage)
    {
#ifdef _SENDEBUG
    TLSLOG(iTLSLogChannel, KMinLogLevel,(_L("RSenServiceManager::RegisterServiceDescription")));

    TPtrC8 outBufferLeft = aMessage.Left(30);
    TLSLOG_FORMAT((iTLSLogChannel, KNormalLogLevel , _L8("Out: %S ..., length: (%d)"),
        &outBufferLeft, aMessage.Length()));
#endif // _SENDEBUG

    TInt length = aMessage.Length();
    TIpcArgs args;
    args.Set(0, &aMessage);
    args.Set(1, length);

    return SendReceive(ESenServRegisterServiceDescription, args);
    }

TInt RSenServiceManager::ServiceDescriptionsByUriL(CSenChunk& aSenChunk)
    {
    TLSLOG_L(iTLSLogChannel, KMinLogLevel,"RSenServiceManager::GetServiceDescriptionsByUri");

    TIpcArgs args;
    aSenChunk.ChunkToArgs(args, 0);

    TInt retVal = SendReceive(ESenServGetLengthOfServiceDescriptionByUri, args);
    
    return retVal;
    }

TInt RSenServiceManager::IdentityProvidersL(CSenChunk& aSenChunk)
    {
    TLSLOG_L(iTLSLogChannel, KMinLogLevel,"RSenServiceManager::IdentityProvidersL()");

    TIpcArgs args;
    aSenChunk.ChunkToArgs(args, 0);

    TInt retVal = SendReceive(ESenServGetIdentityProviders, args);
    
    return retVal;
    }

TInt RSenServiceManager::ServiceDescriptionsByPatternL(CSenChunk& aSenChunk)
    {
    TLSLOG_L(iTLSLogChannel, KMinLogLevel,"RSenServiceManager::ServiceDescriptionsByPatternL()");

    TIpcArgs args;
    aSenChunk.ChunkToArgs(args, 0);
    
    TInt retVal = SendReceive(ESenServGetLengthOfServiceDescriptionByPattern, args);
    
    return retVal;
    }

TInt RSenServiceManager::CredentialsL(CSenChunk& aSenChunk)
    {
    TLSLOG_L(iTLSLogChannel, KMinLogLevel,"RSenServiceManager::CredentialsL");

    TIpcArgs args;
    aSenChunk.ChunkToArgs(args, 0);
    
    TInt retVal = SendReceive(ESenServGetGredentials, args);
    
    return retVal;
    }
    
TInt RSenServiceManager::AddCredentialL(CSenChunk& aSenChunk)
    {
    TLSLOG_L(iTLSLogChannel, KMinLogLevel,"RSenServiceManager::AddCredentialL()");

    TIpcArgs args;
    aSenChunk.ChunkToArgs(args, 0);
    
    TInt retVal = SendReceive(ESenServAddCredential, args);
    
    return retVal;
    }

TInt RSenServiceManager::RemoveCredentialsL(CSenChunk& aSenChunk)
    {
    TLSLOG_L(iTLSLogChannel, KMinLogLevel,"RSenServiceManager::RemoveCredentialsL()");

    TIpcArgs args;
    aSenChunk.ChunkToArgs(args, 0);
    
    TInt retVal = SendReceive(ESenServRemoveCredential, args);
    
    return retVal;
    }
/*
RFileLogger* RSenServiceManager::Log() const
    {
    return iLog;
    }
*/
static TInt StartServerL()
    {
    TInt result(KErrNotFound);

#ifdef _SENDEBUG   
    RFileLogger log;
    CleanupClosePushL(log);
    User::LeaveIfError( log.Connect() );
    log.CreateLog( KSenServiceManagerLogDir, _L("SenStartServer.log"), EFileLoggingModeOverwrite );
    log.Write(_L("- StartServerL() - Log file opened."));
#endif // _SENDEBUG

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
    TInt result(KErrNotFound);

#ifdef _SENDEBUG
    RFileLogger log;
    CleanupClosePushL(log);
    User::LeaveIfError( log.Connect() );
    // Note: this appends to the log created in StartServer()
    log.CreateLog(KSenServiceManagerLogDir, _L("SenStartServer.log"), EFileLoggingModeAppend);
    log.Write(_L("- CreateServerProcessL() - invoked by SM."));
#endif // _SENDEBUG

    const TUidType serverUid(KNullUid, KNullUid, KServerUid3);

#if defined(__WINS__) && !defined(EKA2)

    RLibrary lib;
    CleanupClosePushL(lib);

    RThread server;
    CleanupClosePushL(server);

    result = lib.Load(KSenServiceManagerFileName, serverUid);

    if ( result == KErrNone )
        {
#ifdef _SENDEB
        log.Write(_L8("Library successfully loaded, KErrNone"));
#endif // _SENDEBUG

        //  Get the WinsMain function
        TLibraryFunction functionWinsMain = lib.Lookup(1);

        //  Call it and cast the result to a thread function
        TThreadFunction serverThreadFunction = reinterpret_cast<TThreadFunction>(functionWinsMain());

        TName threadName(KSenServiceManagerFileName);

        // Append a random number to make it unique
        threadName.AppendNum(Math::Random(), EHex);

        result = server.Create( threadName,   // create new server thread
                                serverThreadFunction, // thread's main function
                                KDefaultStackSize,
                                NULL,
                                &lib,
                                NULL,
                                KServerMinHeapSize,
                                KServerMaxHeapSize,
                                EOwnerProcess );

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

    result = server.Create(KSenServiceManagerFileName, KNullDesC, serverUid);	//CodeScannerWarnings
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

// END OF FILE



