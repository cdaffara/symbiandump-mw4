/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Server client for alfredserver.exe
*
*/



#include <apgcli.h>
#include <centralrepository.h>
#include <alf/ftokenclient.h>

#include <eikenv.h>
#include "alf/alfenv.h"
#include "alfuids.h"
#include "alf/alfclientbase.h"
#include "alflogger.h"
#include "alf/alfconstants.h"
#include <goommonitor.h>
#include <pslninternalcrkeys.h>

// ======== MEMBER FUNCTIONS ========

#ifndef __WINS__
#define START_GOOM_FROM_ALF_CLIENT	 
#endif

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
EXPORT_C RAlfClientBase::RAlfClientBase(TInt aServiceUid)
    : RAknAppServiceBase(), iServiceUid(aServiceUid), iEnv(0), iCmdLine(0), iApa(0)
    {
    }

// ---------------------------------------------------------------------------
// Opens the connection
// ---------------------------------------------------------------------------
//    
EXPORT_C void RAlfClientBase::OpenL()
    {
    __ALFLOGSTRING( "RAlfClient::OpenL start" )
    TUint differentiator = 
        StartServerL( TUid::Uid(KAlfAppServerInterfaceUid3) );
    TName serverName;
    ConstructServerName( 
        serverName, 
        TUid::Uid(KAlfAppServerInterfaceUid3), 
        differentiator );
    ConnectExistingByNameL( serverName );
    __ALFLOGSTRING( "RAlfClient::OpenL end" )
    }
  
// ---------------------------------------------------------------------------
// Creates server name
// ---------------------------------------------------------------------------
//    
EXPORT_C void RAlfClientBase::ConstructServerName( TName& aServerName, 
                                        TUid aAppServerUid, 
                                        TUint aServerDifferentiator )
    {

    _LIT(KServerNameFormat, "%08x_%08x_AppServer");
    aServerName.Format( 
        KServerNameFormat, 
        aServerDifferentiator, 
        aAppServerUid.iUid );
    }    
    
// ---------------------------------------------------------------------------
// Creates name, launches server and waits it to start.
// ---------------------------------------------------------------------------
// 
EXPORT_C TUint RAlfClientBase::StartServerL( TUid aAppUid )
    {
    // Start the server application
    TName serverName;
    TUint differentiator( 0 );
    
    differentiator = KAlfAppServerInterfaceUid3;
    ConstructServerName( 
        serverName, 
        TUid::Uid(KAlfAppServerInterfaceUid3) , 
        differentiator );
        
    TFindServer find( serverName );
    TFullName fullName;
    if ( find.Next( fullName ) == KErrNone )
        {
        return differentiator;
        }
    
    TThreadId serverThreadId;
    LaunchAppL( aAppUid, differentiator, serverThreadId );

    return differentiator;
    }
    
// ---------------------------------------------------------------------------
// Launches the app server.
// ---------------------------------------------------------------------------
// 
EXPORT_C void RAlfClientBase::LaunchAppL( 
    TUid aAppUid, 
    TUint aServerDifferentiator, 
    TThreadId& aThreadId )
	{
	RApaLsSession apa;
	User::LeaveIfError( apa.Connect() );
	CleanupClosePushL( apa );
	
	TApaAppInfo info;
	User::LeaveIfError( apa.GetAppInfo( info, aAppUid ) );

	CApaCommandLine* cmdLine = CApaCommandLine::NewLC();
	cmdLine->SetExecutableNameL( info.iFullName );
	cmdLine->SetServerRequiredL( aServerDifferentiator );
    // Set the command to start the server in background
    cmdLine->SetCommandL( EApaCommandBackground );        

	TRequestStatus status;	
	TInt err = apa.StartApp( *cmdLine, aThreadId, &status );
    
    User::LeaveIfError( err );
    
    User::WaitForRequest(status);
    
    User::LeaveIfError( status.Int() );
    
	CleanupStack::PopAndDestroy( cmdLine );
	CleanupStack::PopAndDestroy( &apa );
	}

// ---------------------------------------------------------------------------
// From class RAknAppServiceBase.
// Returns the service UID supported.
// ---------------------------------------------------------------------------
//
EXPORT_C TUid RAlfClientBase::ServiceUid() const
    {
    return TUid::Uid(iServiceUid);
    }


// ---------------------------------------------------------------------------
// Grant access
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RAlfClientBase::GrantAccessToPrivateFile( 
                                            const TDesC& aFileName, 
                                            TInt aObjectIdentfier, 
                                            TInt aCommandId )
    {
    if (!iEnv || !iEnv->TokenClient())
        {
        return KErrNotReady;        
        }
    
    TInt64 token = 0;
    TPckg<TInt64> tokenBuf(token);
    TInt err = iEnv->TokenClient()->GenerateToken(aFileName, TUid::Uid(KAlfAppServerInterfaceUid3), tokenBuf);
    if (!err)
        {
        err = SendReceive(EAlfDoSubSessionCmd, TIpcArgs(aCommandId,&tokenBuf,&tokenBuf,aObjectIdentfier));
        }
 
    return err;
    }

// ---------------------------------------------------------------------------
// Grant access
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RAlfClientBase::GrantAccessToPrivateFile( 
                                            const RFile& aFile, 
                                            TInt aObjectIdentfier, 
                                            TInt aCommandId )
   {
    if (!iEnv || !iEnv->TokenClient())
        {
        return KErrNotReady;        
        }
    
    TInt64 token = 0;
    TPckg<TInt64> tokenBuf(token);
    TInt err = iEnv->TokenClient()->GenerateToken(aFile, TUid::Uid(KAlfAppServerInterfaceUid3), tokenBuf);
    if (!err)
        {
        err = SendReceive(EAlfDoSubSessionCmd, TIpcArgs(aCommandId,&tokenBuf,&tokenBuf,aObjectIdentfier));
        }
 
    return err;
    }

// ---------------------------------------------------------------------------
// Sets env
// ---------------------------------------------------------------------------
//
void RAlfClientBase::SetAlfEnv(CAlfEnv& aAlf)
    {
    iEnv = &aAlf;
    }

void RAlfClientBase::StartAsyncL(TRequestStatus* aStatus)
    {
    ASSERT(iApa==0 && iCmdLine == 0);
    // Start the server application
    TName serverName;
    TUint differentiator( 0 );
    
    differentiator = KAlfAppServerInterfaceUid3;
    ConstructServerName( 
        serverName, 
        TUid::Uid(KAlfAppServerInterfaceUid3) , 
        differentiator );
    
    TFindServer serverFinder(serverName);
    TFullName fullName;
    if (serverFinder.Next(fullName) == KErrNone)
        {
        User::Leave(KErrAlreadyExists);
        }
        
    TThreadId threadId;
    // we don't have proper destructor and thus we don't take
    // "normal" ownership on our members...
    
    // assign to member after poping from cleanup stack - codescanner now happy
  RApaLsSession*  apa = new (ELeave) RApaLsSession;
	CleanupStack::PushL(apa);
	User::LeaveIfError( apa->Connect() );
	CleanupClosePushL( *apa );
	
	TApaAppInfo info;
	User::LeaveIfError( apa->GetAppInfo( info, TUid::Uid(KAlfAppServerInterfaceUid3) ) );

	CApaCommandLine* cmdLine = CApaCommandLine::NewLC();
	cmdLine->SetExecutableNameL( info.iFullName );
	cmdLine->SetServerRequiredL( differentiator );
    // Set the command to start the server in background
    cmdLine->SetCommandL( EApaCommandBackground );        

	User::LeaveIfError(apa->StartApp( *cmdLine, threadId, aStatus ));

    CleanupStack::Pop(3);
    iCmdLine=cmdLine;
    iApa = apa;
    }

#if !defined(__WINS__)    
_LIT(KBgAnimExeName, "z:\\sys\\bin\\backgroundanimhost.exe");
#endif

void RAlfClientBase::CompleteAsynchConstructionL()
    {
    if (iApa)
        {    
        delete iCmdLine;
        iCmdLine = 0; 
        iApa->Close();
        delete iApa;
        iApa = 0;

        TName serverName;
            ConstructServerName( 
            serverName, 
            TUid::Uid(KAlfAppServerInterfaceUid3), 
            KAlfAppServerInterfaceUid3 );
        ConnectExistingByNameL( serverName );
        }
    else
        {
    	TInt value = 0;
        CRepository* repo = CRepository::NewLC(KCRUidThemes);
        User::LeaveIfError(repo->Get(KThemesAnimBackgroundSupport, value));
        CleanupStack::PopAndDestroy();
#if !defined(__WINS__)
        if (!value)
            {         
            RProcess image;
            User::LeaveIfError(image.Create(KBgAnimExeName,KNullDesC));
            image.Resume();
            image.Close();
            }
#endif
        }
	       
    }

CAlfAsynchStartup::CAlfAsynchStartup():CActive(CActive::EPriorityHigh), iClient(RAlfClientBase(KAlfAppServerInterfaceUid3))
    {
    CActiveScheduler::Add(this);
    }

EXPORT_C void CAlfAsynchStartup::StartL()
    {
	  TInt err(KErrNone);
	
#ifdef START_GOOM_FROM_ALF_CLIENT	 
	  // do the actual goom startup : starting this first in case something goes wrong and startup needs to stop
    TRAP( err, CreateGOOMWatcherThreadL());
	if(err != KErrNone)
		{
	    RDebug::Print(_L("Creating out of graphics mem thread failed with err %d"), err);
		}
#endif
	
	//everything went ok for goom: moving over for alf now
    CAlfAsynchStartup* me = new (ELeave) CAlfAsynchStartup();
    TRAP(err, me->iClient.StartAsyncL(&(me->iStatus)));
    if ( err )
        {
        if (err == KErrAlreadyExists) // NGA Always
            {
            me->SetActive();    
            TRequestStatus* sptr = &(me->iStatus);
            User::RequestComplete(sptr, err);    
            }    
        else
            {
            // just die gracefully
            delete me;
            }
        return;
        }

    // everything fine, wait for rendezvous
    me->SetActive();    
    }
    
void CAlfAsynchStartup::RunL()
    {

    TRAP_IGNORE(iClient.CompleteAsynchConstructionL());
    iClient.Close();
    
    delete this;
    }

void CAlfAsynchStartup::DoCancel()
    {
    // not possible at the moment
    }

// end of file

