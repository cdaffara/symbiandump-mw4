/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*      Implementation of Scheme handler interface implementation for http:// scheme
*      
*
*/


// INCLUDE FILES

#include "HttpHandler.h"
#include "SchemeDispLogger.h"
#include <ecom/ecom.h>		// For REComSession
#include <eikenv.h>
#include <DocumentHandler.h>
#include <apgcli.h>
#include <apparc.h>
#include <eikdoc.h>
#include <eikproc.h>
#include <f32file.h>
#include <apgtask.h>
#include <w32std.h>
#include <BrowserOverriddenSettings.h>

_LIT( KBrowserPrefix, "4 " );
LOCAL_C const TUid KUidBrowser = { 0x10008D39 };

// ================= CONSTANTS =======================

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CHttpHandler::NewL()
// ---------------------------------------------------------
//
CHttpHandler* CHttpHandler::NewL( const TDesC& aUrl )
	{
	CLOG_ENTERFN( "CHttpHandler::NewL()" );

	CHttpHandler* self=new(ELeave) CHttpHandler();
	CleanupStack::PushL(self);
	self->ConstructL( aUrl );
	CleanupStack::Pop(self);

	CLOG_LEAVEFN( "CHttpHandler::NewL()" );

	return self;
	}

// ---------------------------------------------------------
// CHttpHandler::~CHttpHandler()
// ---------------------------------------------------------
//
CHttpHandler::~CHttpHandler()
	{
	CLOG_ENTERFN( "CHttpHandler::~CHttpHandler()" );

	if(iDoc != NULL)
		{
		CEikProcess*  hostProcess = CEikonEnv::Static()->Process();
		hostProcess->DestroyDocument(iDoc);
		iDoc = NULL;
		}

    delete iLauncher;

	CLOG_LEAVEFN( "CHttpHandler::~CHttpHandler()" );
	}

// ---------------------------------------------------------
// CHttpHandler::CHttpHandler()
// ---------------------------------------------------------
//
CHttpHandler::CHttpHandler() : CBaseHandler()
	{
	// Deliberately do nothing here : See ConstructL() for initialisation 
    // completion.
	}

// ---------------------------------------------------------
// CHttpHandler::ConstructL()
// ---------------------------------------------------------
//
void CHttpHandler::ConstructL( const TDesC& aUrl )
	{
	iParsedUrl = HBufC::NewL( aUrl.Length() );
	iParsedUrl->Des().Copy( aUrl );
	}

// ---------------------------------------------------------
// CHttpHandler::HandleUrlEmbeddedL()
// ---------------------------------------------------------
//
void CHttpHandler::HandleUrlEmbeddedL()
	{
	CLOG_ENTERFN( "CHttpHandler::HandleUrlEmbeddedL()" );
	/* Launch the appropriate application in embedded mode */
    HBufC* buf16 = HBufC::NewLC( 
	    iParsedUrl->Des().Length() + KBrowserPrefix.iTypeLength );
    buf16->Des().Copy( KBrowserPrefix );
    buf16->Des().Append( *iParsedUrl );
            
//    iLauncher = CBrowserLauncher::NewL();
    TBrowserOverriddenSettings settings;
    settings.SetBrowserSetting( EBrowserOverSettingsFullScreen, 1 );
    settings.SetBrowserSetting( EBrowserOverSettingsAutoLoadImages, 1 );
    settings.SetBrowserSetting( EBrowserOverSettingsFontSize, EBrowserOverFontSizeLevelNormal );
/*    iLauncher->LaunchBrowserEmbeddedL( buf16->Des(), 
                                       NULL, 
                                       this,
                                       &settings );*/
    CleanupStack::PopAndDestroy();  // buf16  
	CLOG_LEAVEFN( "CHttpHandler::HandleUrlEmbeddedL()" );
	}

// ---------------------------------------------------------
// CHttpHandler::HandleUrlStandaloneL()
// ---------------------------------------------------------
//
void CHttpHandler::HandleUrlStandaloneL()
	{
	CLOG_ENTERFN( "CHttpHandler::HandleUrlStandaloneL()" );

	HBufC* buf16 = HBufC::NewLC( 
		iParsedUrl->Des().Length() + KBrowserPrefix.iTypeLength );
	buf16->Des().Copy( KBrowserPrefix );
	buf16->Des().Append( *iParsedUrl );

    RWsSession wsSession;
	User::LeaveIfError( wsSession.Connect() );
	CleanupClosePushL<RWsSession>( wsSession );
    TApaTaskList taskList( wsSession );
    TApaTask task = taskList.FindApp( KUidBrowser );
    if ( task.Exists() )
        {
        HBufC8* param8 = HBufC8::NewLC( buf16->Length() );
        param8->Des().Append( buf16->Des() );
        task.SendMessage( TUid::Uid( 0 ), *param8 ); // Uid is not used
        CleanupStack::PopAndDestroy( param8 );
        }
    else 
        {
	    RApaLsSession appArcSession;
	    User::LeaveIfError( appArcSession.Connect() );
	    CleanupClosePushL<RApaLsSession>( appArcSession );
	    TThreadId id;
        appArcSession.StartDocument( *buf16, KUidBrowser , id ); 
	    CleanupStack::PopAndDestroy(); // appArcSession
        }

    CleanupStack::PopAndDestroy( 2 );  // buf16, wsSession

	CLOG_LEAVEFN( "CHttpHandler::HandleUrlStandaloneL()" );
	}
	
// ---------------------------------------------------------
// CHttpHandler::HandleServerAppExit()
// ---------------------------------------------------------
//
void CHttpHandler::HandleServerAppExit(TInt aReason)
	{
	CLOG_ENTERFN( "CHttpHandler::HandleServerAppExit" );

	if( NULL != iSchemeDoc )
		{
	    iSchemeDoc->HandleServerAppExit( aReason );
		}

	CLOG_LEAVEFN( "CHttpHandler::HandleServerAppExit" );	
	}

