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
*      Implementation of Scheme handler interface implementation for wtai:// scheme
*      
*
*/


// INCLUDE FILES

#include "CtiHandler.h"
#include "SchemeDispLogger.h"
#include <ecom/ecom.h>		// For REComSession
#include <eikenv.h>
#include <apparc.h>
#include <apgcli.h>

// ================= CONSTANTS =======================
_LIT( KPattern,"cti:");

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CCtiHandler::NewL()
// ---------------------------------------------------------
//
CCtiHandler* CCtiHandler::NewL( const TDesC& aUrl )
	{
	CLOG_ENTERFN( "CCtiHandler::NewL()" );

	CCtiHandler* self=new(ELeave) CCtiHandler();
	CleanupStack::PushL( self );
	self->ConstructL( aUrl );
	CleanupStack::Pop( self );

	CLOG_LEAVEFN( "CCtiHandler::NewL()" );
	return self;
	}

// ---------------------------------------------------------
// CCtiHandler::~CCtiHandler()
// ---------------------------------------------------------
//
CCtiHandler::~CCtiHandler()
	{
	CLOG_ENTERFN( "CCtiHandler::~CCtiHandler()" );

    if( iTelService != NULL )
        {
	    iTelService->RemoveObserver(this);
	    delete iTelService;
        iTelService = NULL;
        }

	CLOG_LEAVEFN( "CCtiHandler::~CCtiHandler()" );
	}

// ---------------------------------------------------------
// CCtiHandler::CCtiHandler()
// ---------------------------------------------------------
//
CCtiHandler::CCtiHandler() : CBaseHandler()
	{
	// Deliberately do nothing here : See ConstructL() for initialisation 
    // completion.
	}

// ---------------------------------------------------------
// CCtiHandler::ConstructL()
// ---------------------------------------------------------
//
void CCtiHandler::ConstructL( const TDesC& aUrl )
	{
	CLOG_ENTERFN( "CCtiHandler::ConstructL()" );

	BaseConstructL( aUrl );

	CLOG_LEAVEFN( "CCtiHandler::ConstructL()" );
	}

// ---------------------------------------------------------
// CCtiHandler::HandleUrlEmbeddedL()
// ---------------------------------------------------------
//
void CCtiHandler::HandleUrlEmbeddedL()
	{
	CLOG_ENTERFN( "CCtiHandler::HandleUrlEmbeddedL()" );

	TPtrC path;
    TInt err;

	iTelService = CBrowserTelService::NewL();
	iTelService->AddObserver( this );

	TRAP( err, path.Set( RemoveSchemeFromUrlL( KPattern ) ) );
	if( err ==  KErrNone)
		{
        TBool confirmDtmfValue = ReadSdConfirmDtmfValueL();
		CLOG_WRITE_FORMAT( "CCtiHandler::HandleUrlEmbeddedL: path: %S", &path );
		
        if( 0 != path.Length() )
            {
			err = iTelService->MakeCall( path, confirmDtmfValue );
            }
        else
            {
            err = KErrCancel;
            }
        }

    NotifyClient();

    ErrorHandlerL( err );

	CLOG_LEAVEFN( "CCtiHandler::HandleUrlEmbeddedL()" );

	}

// ---------------------------------------------------------
// CCtiHandler::HandleUrlStandaloneL()
// ---------------------------------------------------------
//
void CCtiHandler::HandleUrlStandaloneL()
	{
	CLOG_ENTERFN( "CCtiHandler::HandleUrlStandaloneL()" );

    LaunchSchemeAppWithCommandLineL();

	CLOG_LEAVEFN( "CCtiHandler::HandleUrlStandaloneL()" );
	}

// ---------------------------------------------------------
// CCtiHandler::BrowserTelServiceEvent()
// ---------------------------------------------------------
//
void CCtiHandler::BrowserTelServiceEvent( TBrowserTelServiceState aEvent )
	{
	if( ( EIdle == aEvent ) && ( NULL !=iSchemeDoc ) )
		{
        iSchemeDoc->HandleServerAppExit( KErrNone );
		}
	}

// ---------------------------------------------------------
// CCtiHandler::BrowserTelServiceError()
// ---------------------------------------------------------
//
void CCtiHandler::BrowserTelServiceError( TBrowserTelServiceError /*aError*/)
	{
	}

// ---------------------------------------------------------
// CCtiHandler::BrowserTelServiceError()
// ---------------------------------------------------------
//
void CCtiHandler::NotifyClient()
	{
	if( NULL !=iSchemeDoc )
		{
        iSchemeDoc->HandleServerAppExit( KErrNone );
		}
	}
