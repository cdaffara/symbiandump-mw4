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
*      Implementation of Scheme handler interface implementation for file:// scheme
*      
*
*/


// INCLUDE FILES

#include "BaseHandler.h"
//#include "BrowserTelService.h"
#include "SchemeDispLogger.h"
#include <e32base.h>
#include <AknQueryDialog.h>
#include <StringLoader.h>
#include <bautils.h>
#include <escapeutils.h>
//#include <BrowserUiSDKCRKeys.h>
#include <centralrepository.h>

#include <apgcli.h>
#include <apparc.h>

#include <e32std.h>
#include <s32mem.h>

// ================= CONSTANTS =======================


// ================= MEMBER FUNCTIONS =======================


// ---------------------------------------------------------
// CBaseHandler::CBaseHandler()
// ---------------------------------------------------------
//
CBaseHandler::CBaseHandler() : CSchemeHandler()
	{
	iSchemeDoc = NULL;
	}

// ---------------------------------------------------------
// CCtiHandler::~CCtiHandler()
// ---------------------------------------------------------
//
CBaseHandler::~CBaseHandler()
	{
	delete iParsedUrl;
	}


// ---------------------------------------------------------
// CBaseHandler::ConstructL()
// ---------------------------------------------------------
//
void CBaseHandler::BaseConstructL( const TDesC& aUrl )
	{
	CLOG_ENTERFN( "CBaseHandler::BaseConstructL()" );

    iParsedUrl = EscapeUtils::EscapeDecodeL( aUrl );

	CLOG_LEAVEFN( "CBaseHandler::BaseConstructL()" );
	}

// ---------------------------------------------------------
// CBaseHandler::Observer()
// ---------------------------------------------------------
//
void CBaseHandler::Observer( MAknServerAppExitObserver* aSchemeDoc )
	{
	iSchemeDoc = aSchemeDoc;
	}

// ---------------------------------------------------------
// CBaseHandler::RemoveSchemeFromUrlL()
// ---------------------------------------------------------
//
TPtrC CBaseHandler::RemoveSchemeFromUrlL( const TDesC& aScheme )
	{
	/* Serach the beginning of the Url */
	TInt schPos;
    schPos = iParsedUrl->Des().FindF( aScheme );
    if( schPos == KErrNotFound )
        {
        User::Leave( KErrNotFound );
        }
	
	/* Get the Url without scheme */
    TPtrC path = iParsedUrl->Des().Right
        ( iParsedUrl->Des().Length() - ( schPos + aScheme.Length()) );

	if( 0 == path.Length() )
		{
		User::Leave( KErrArgument );
		}

	return path;
	}

// ---------------------------------------------------------
// CBaseHandler::ErrorHandler
// ---------------------------------------------------------
//
void CBaseHandler::ErrorHandlerL( TInt aErrorCode )
    {
    TInt errCode = aErrorCode;
    switch( aErrorCode )
        {
         /* There was no error */
        case KErrNone:
            return;
        /* to be shown note 
        case KErrInvocationError:
        case KErrNoCallActive:
        case KErrUserNoAnswer:
        case KErrNoService:
        case KErrUserBusy:
        case KErrPBPhoneBookFull:
        case KErrPBWriteError:
        case KErrPBNumberTooLong:
        case KErrPBNameTooLong:
        case KErrUnspecified:
        case KErrCancel:
            {
            errCode = KErrCancel;
            }
            break;
            */
        default:
            break;
        }
    User::Leave( errCode );
    }

// ---------------------------------------------------------
// CBaseHandler::ReadSdConfirmDtmValueL
// ---------------------------------------------------------
//
TBool CBaseHandler::ReadSdConfirmDtmfValueL()
    {
//    CRepository* repository = CRepository::NewL( KCRUidBrowser );
    TInt retVal = 0;  
//    repository->Get( KBrowserConfirmDTMFSending, retVal );    
//    delete repository;
    return retVal;
    }

// ---------------------------------------------------------
// CBaseHandler::LaunchSchemeAppWithCommandLineL
// ---------------------------------------------------------
//
void CBaseHandler::LaunchSchemeAppWithCommandLineL()
    {
    // Try launching...
    RApaLsSession   appArcSession;
    User::LeaveIfError( appArcSession.Connect() );
    CleanupClosePushL<RApaLsSession>( appArcSession );
    TThreadId dummyId;

    TApaAppInfo aInfo;
    appArcSession.GetAppInfo( aInfo, KUidSchemeApp );

    CApaCommandLine* cmdLine = CApaCommandLine::NewL();
    CleanupStack::PushL( cmdLine );
    cmdLine->SetDocumentNameL( *iParsedUrl );
    cmdLine->SetCommandL( EApaCommandCreate );
    cmdLine->SetExecutableNameL( aInfo.iFullName );

    // Get the whole parameter list and put to the command line
    if ( iParamList )
        {
        TInt size = iParamList->Size();
        CBufFlat* store = CBufFlat::NewL( size );
        CleanupStack::PushL( store );
	    RBufWriteStream outStream;
		outStream.Open( *store );
        CleanupClosePushL<RBufWriteStream>( outStream );
		iParamList->ExternalizeL( outStream );
        cmdLine->SetTailEndL( store->Ptr( 0 ) );
        CleanupStack::PopAndDestroy( 2 );  // store, close outStream
        }

    User::LeaveIfError( appArcSession.StartApp( *cmdLine,dummyId) );
    CLOG_WRITE( "Launched SchemeApp succesfully" )
  
    CleanupStack::PopAndDestroy( 2 ); // close appArcSession, cmdLine
    }
