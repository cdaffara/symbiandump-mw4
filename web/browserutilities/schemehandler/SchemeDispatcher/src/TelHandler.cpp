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

#include "SchemeDispLogger.h"
#include "TelHandler.h"
#include <ecom/ecom.h>		// For REComSession
#include <eikenv.h>
#include <apparc.h>
#include <apgcli.h>

// ================= CONSTANTS =======================
_LIT( KPattern,"tel:");
_LIT( KSIPPattern,"sip:");

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CTelHandler::NewL()
// ---------------------------------------------------------
//
CTelHandler* CTelHandler::NewL( const TDesC& aUrl )
	{
	CLOG_ENTERFN( "CTelHandler::NewL()" );

	CTelHandler* self=new(ELeave) CTelHandler();
	CleanupStack::PushL(self);
	self->ConstructL( aUrl );
	CleanupStack::Pop(self);

	CLOG_LEAVEFN( "CTelHandler::NewL()" );

	return self;
	}

// ---------------------------------------------------------
// CTelHandler::~CTelHandler()
// ---------------------------------------------------------
//
CTelHandler::~CTelHandler()
	{
	CLOG_ENTERFN( "CTelHandler::~CTelHandler()" );

    if( iTelService != NULL )
        {
	    iTelService->RemoveObserver(this);
	    delete iTelService;
        iTelService = NULL;
        }

	CLOG_LEAVEFN( "CTelHandler::~CTelHandler()" );
	}

// ---------------------------------------------------------
// CTelHandler::CTelHandler()
// ---------------------------------------------------------
//
CTelHandler::CTelHandler() : CBaseHandler()
	{
	// Deliberately do nothing here : See ConstructL() for initialisation 
    // completion.
	}

// ---------------------------------------------------------
// CTelHandler::ConstructL()
// ---------------------------------------------------------
//
void CTelHandler::ConstructL( const TDesC& aUrl )
	{
	CLOG_ENTERFN( "CTelHandler::ConstructL()" );

	BaseConstructL( aUrl );

	CLOG_LEAVEFN( "CTelHandler::ConstructL()" );
	}

// ---------------------------------------------------------
// CTelHandler::HandleUrlEmbeddedL()
// ---------------------------------------------------------
//
void CTelHandler::HandleUrlEmbeddedL()
	{
	CLOG_ENTERFN( "CTelHandler::HandleUrlEmbeddedL()" );

	TPtrC path;
    TInt err;

	iTelService = CBrowserTelService::NewL();
	iTelService->AddObserver(this);

	TRAP( err, path.Set( RemoveSchemeFromUrlL( KPattern ) ) );
	if( err ==  KErrNone )
		{
        TBool confirmDtmfValue = ReadSdConfirmDtmfValueL();
		CLOG_WRITE_FORMAT( "CTelHandler::HandleUrlEmbeddedL: path: %S", &path );

        if( 0 != path.Length() )
            {
			err = iTelService->MakeCall(path, confirmDtmfValue);
            }
        else
            {
            err = KErrCancel;
            }
        }
    else
        {
            TRAP( err, path.Set( RemoveSchemeFromUrlL( KSIPPattern ) ) );
            if( err ==  KErrNone )
				{
        		TBool confirmDtmfValue = ReadSdConfirmDtmfValueL();
				CLOG_WRITE_FORMAT( "CTelHandler::HandleUrlEmbeddedL: path: %S", &path );

        		if( 0 != path.Length() )
            		{
					err = iTelService->MakeVOIPCall(path, confirmDtmfValue);
            		}
        		else
            		{
            		err = KErrCancel;
            		}
				}
        }	
    

    NotifyClient();

    ErrorHandlerL( err );

	CLOG_LEAVEFN( "CTelHandler::HandleUrlEmbeddedL()" );
	}

// ---------------------------------------------------------
// CTelHandler::HandleUrlStandaloneL()
// ---------------------------------------------------------
//
void CTelHandler::HandleUrlStandaloneL()
	{
	CLOG_ENTERFN( "CTelHandler::HandleUrlStandaloneL()" );

    LaunchSchemeAppWithCommandLineL();

	CLOG_LEAVEFN( "CTelHandler::HandleUrlStandaloneL()" );
	}

// ---------------------------------------------------------
// CTelHandler::BrowserTelServiceEvent()
// ---------------------------------------------------------
//
void CTelHandler::BrowserTelServiceEvent( TBrowserTelServiceState aEvent )
	{
	if( ( EIdle == aEvent ) && ( NULL !=iSchemeDoc ) )
		{
        iSchemeDoc->HandleServerAppExit( KErrNone );
		}
	}

// ---------------------------------------------------------
// CTelHandler::BrowserTelServiceError()
// ---------------------------------------------------------
//
void CTelHandler::BrowserTelServiceError( TBrowserTelServiceError /*aError*/)
	{
	}

// ---------------------------------------------------------
// CTelHandler::BrowserTelServiceError()
// ---------------------------------------------------------
//
void CTelHandler::NotifyClient()
	{
	if( NULL !=iSchemeDoc )
		{
        iSchemeDoc->HandleServerAppExit( KErrNone );
		}
	}
