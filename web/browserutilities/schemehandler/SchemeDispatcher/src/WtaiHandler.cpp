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

#include "WtaiHandler.h"
#include "SchemeDispLogger.h"
#include <ecom/ecom.h>		// For REComSession
#include <eikenv.h>
#include <apparc.h>
#include <apgcli.h>

// ================= CONSTANTS =======================
LOCAL_C const TText KSemiColon = ';';
LOCAL_C const TText KExclamationMark = '!';
// NW_STAT_WTAI_UNSUPPORTED_LIBRARY
LOCAL_C const TInt KErrUnsuppLibrary = (-26179);
// NW_STAT_WTAI_UNSUPPORTED_FUNCTION
LOCAL_C const TInt KErrUnsupFunction = (-26178);

_LIT( KWtai, "wtai://");
_LIT( KWPMC, "wp/mc" );
_LIT( KWPSD, "wp/sd" );
_LIT( KWPAP, "wp/ap" );

_LIT( KWP, "wp/" );
_LIT( KMC, "mc;" );
_LIT( KSD, "sd;" );
_LIT( KAP, "ap;" );

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CWtaiHandler::NewL()
// ---------------------------------------------------------
//
CWtaiHandler* CWtaiHandler::NewL( const TDesC& aUrl )
	{
	CLOG_ENTERFN( "CWtaiHandler::NewL()" );

	CWtaiHandler* self=new(ELeave) CWtaiHandler();
	CleanupStack::PushL(self);
	self->ConstructL( aUrl );
	CleanupStack::Pop(self);

	CLOG_LEAVEFN( "CWtaiHandler::NewL()" );

	return self;
	}

// ---------------------------------------------------------
// CWtaiHandler::~CWtaiHandler()
// ---------------------------------------------------------
//
CWtaiHandler::~CWtaiHandler()
	{
	CLOG_ENTERFN( "CWtaiHandler::~CWtaiHandler()" );

    if( iTelService != NULL )
        {
	    iTelService->RemoveObserver(this);
	    delete iTelService;
        iTelService = NULL;
        }

	CLOG_LEAVEFN( "CWtaiHandler::~CWtaiHandler()" );
	}

// ---------------------------------------------------------
// CWtaiHandler::CWtaiHandler()
// ---------------------------------------------------------
//
CWtaiHandler::CWtaiHandler() : CBaseHandler()
	{
	// Deliberately do nothing here : See ConstructL() for initialisation 
    // completion.
	}

// ---------------------------------------------------------
// CWtaiHandler::ConstructL()
// ---------------------------------------------------------
//
void CWtaiHandler::ConstructL( const TDesC& aUrl )
	{
	CLOG_ENTERFN( "CWtaiHandler::ConstructL()" );

	BaseConstructL( aUrl );

	CLOG_ENTERFN( "calling CBrowserTelService::NewL()" );

	CLOG_LEAVEFN( "CWtaiHandler::ConstructL()" );
	}

// ---------------------------------------------------------
// CWtaiHandler::HandleUrlEmbeddedL()
// ---------------------------------------------------------
//
void CWtaiHandler::HandleUrlEmbeddedL()
	{
	CLOG_ENTERFN( "CWtaiHandler::HandleUrlEmbeddedL()" );
    TInt err;

	TPtrC library_function;

	iTelService = CBrowserTelService::NewL();
    iTelService->AddObserver(this);

	TRAP( err, library_function.Set( GetWtaiLibraryFunctionL() ) );
	TInt count = GetParameterCountL();
    TBool confirmDtmfValue = ReadSdConfirmDtmfValueL();
	if( ( err == KErrNone ) || ( count <= 0 ) )
		{
		if( 0 == library_function.CompareF( KWPMC ) )
			{
			TPtrC number = GetParameterL( ESchemeWtaiNumber );

			CLOG_WRITE_FORMAT( "CWtaiHandler::GetParameter: number: %S", &number );

            err = iTelService->MakeCall( number, confirmDtmfValue );
			}
		else if( 0 == library_function.CompareF( KWPSD ) )
			{
			TPtrC dtmf = GetParameterL( ESchemeWtaiNumber );

			CLOG_WRITE_FORMAT( "CWtaiHandler::GetParameter: dtmf: %S", &dtmf );

            err = iTelService->SendDTMF(dtmf, confirmDtmfValue);
			}
		else if( 0 == library_function.CompareF( KWPAP ) )
			{
			TPtrC number = GetParameterL( ESchemeWtaiNumber );
			CLOG_WRITE_FORMAT( "CWtaiHandler::GetParameter: number: %S", &number );
			TPtrC name = GetParameterL( ESchemeWtaiName );
			CLOG_WRITE_FORMAT( "CWtaiHandler::GetParameter: name: %S", &name );
			TPtrC email = GetParameterL( ESchemeWtaiEmail );
			CLOG_WRITE_FORMAT( "CWtaiHandler::GetParameter: email: %S", &email );
			err = iTelService->AddPBEntryL( number, name, email );
			}
		}

    NotifyClient();

    ErrorHandlerL( err );

	CLOG_LEAVEFN( "CWtaiHandler::HandleUrlEmbeddedL()" );
	}

// ---------------------------------------------------------
// CWtaiHandler::HandleUrlStandaloneL()
// ---------------------------------------------------------
//
void CWtaiHandler::HandleUrlStandaloneL()
	{
	CLOG_ENTERFN( "CWtaiHandler::HandleUrlStandaloneL()" );

    LaunchSchemeAppWithCommandLineL();

	CLOG_LEAVEFN( "CWtaiHandler::HandleUrlStandaloneL()" );
	}

// ---------------------------------------------------------
// CWtaiHandler::GetWtaiLibraryFunctionL()
// ---------------------------------------------------------
//
TPtrC CWtaiHandler::GetWtaiLibraryFunctionL()
	{
	CLOG_ENTERFN( "CWtaiHandler::GetWtaiLibraryFunction()" );
    // wtai://<library>/<function> (; <parameter>)*

	TPtrC path = RemoveSchemeFromUrlL( KWtai );

	// <library>/<function> (; <parameter>)*
    TInt colonPos = VerifyWtaiSchemeL( path );

	TPtrC library_function = path.Left( colonPos );

	CLOG_LEAVEFN( "CWtaiHandler::GetWtaiLibraryFunction()" );

	return library_function; // <library>/<function>
	}

// ---------------------------------------------------------
// CWtaiHandler::VerifyWtaiSchemeL()
// ---------------------------------------------------------
//
TInt CWtaiHandler::VerifyWtaiSchemeL( TPtrC path )
    {
	TInt colonPos = path.Locate( ';' ); // first occurance

	if ( colonPos <= 0 )
		{
 		User::Leave( KErrGeneral );
		}
    else if( KErrNotFound == path.FindF( KWP() ) ) // check if it's valid library
        {
        User::Leave( KErrUnsuppLibrary );
        }
    else if( ( KErrNotFound != path.FindF( KMC() ) ) ||
             ( KErrNotFound != path.FindF( KSD() ) ) ||
             ( KErrNotFound != path.FindF( KAP() ) ) ) // check if it's valid function
        {
        return colonPos;
        }
    else
        {
        User::Leave( KErrUnsupFunction );
        }

    return colonPos;
    }

// ---------------------------------------------------------
// CWtaiHandler::GetParameterCountL()
// ---------------------------------------------------------
//
TInt CWtaiHandler::GetParameterCountL()
	{
	CLOG_ENTERFN( "CWtaiHandler::GetParameterCount()" );

	// wtai://<library>/<function> (; <parameter>)*

	TInt retVal = 0;
	TPtrC path = RemoveSchemeFromUrlL( KWtai );
	TInt length = path.Length();

	/* Count for the amount of parameter */
	for( TInt i = 0; i < length; i++ )
		{
		if( path[i] == KSemiColon )
			{
			retVal++;
			}
		}

	CLOG_LEAVEFN( "CWtaiHandler::GetParameterCount()" );

	return retVal;
	}

// ---------------------------------------------------------
// CWtaiHandler::GetParameterL()
// ---------------------------------------------------------
//
TPtrC CWtaiHandler::GetParameterL(TInt aPos)
	{
	CLOG_ENTERFN( "CWtaiHandler::GetParameter()" );

	// wtai://<library>/<function> (; <parameter>)*

	TPtrC path = RemoveSchemeFromUrlL( KWtai );

	// <library>/<function> (; <parameter>)*

	TInt length = path.Length();
	TInt start = 0; // starting position of the required parameter
	TInt pos = 1;   // current parameter
	TInt i = 0;     // character iterator

	while( i < length )
		{
		if( ( path[i] == KSemiColon )  || ( path[i] == KExclamationMark ) )
			{
			// begining of a parameter is found
			if( ( pos == aPos ) &&  ( path[i] != KExclamationMark ) )
				{
				// begining of the required parameter
				start = i + 1; // skipping semicolon
				}
			else
				{
				if( start != 0 )
					{
					// end of the required parameter
					break;
					}
				}
			pos++;
			}
		i++;
		}

	start = ( start == 0 ) ? i : start; // there is n parameter but the (n+1). is required

	CLOG_LEAVEFN( "CWtaiHandler::GetParameter()" );

	return path.Mid( start, i-start ); // found parameter
	}

// ---------------------------------------------------------
// CWtaiHandler::BrowserTelServiceEvent()
// ---------------------------------------------------------
//
void CWtaiHandler::BrowserTelServiceEvent( TBrowserTelServiceState aEvent )
	{
	CLOG_ENTERFN( "CWtaiHandler::BrowserTelServiceEvent()" );

	if( EIdle == aEvent )
		{
		NotifyClient();
		}
	}

// ---------------------------------------------------------
// CWtaiHandler::BrowserTelServiceError()
// ---------------------------------------------------------
//
void CWtaiHandler::BrowserTelServiceError( TBrowserTelServiceError /*aError*/)
	{
	}

// ---------------------------------------------------------
// CWtaiHandler::BrowserTelServiceError()
// ---------------------------------------------------------
//
void CWtaiHandler::NotifyClient()
	{
	if( NULL !=iSchemeDoc )
		{
        iSchemeDoc->HandleServerAppExit( KErrNone );
		}
	}
