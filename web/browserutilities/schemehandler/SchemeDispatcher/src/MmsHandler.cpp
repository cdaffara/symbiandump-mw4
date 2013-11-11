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
*      Implementation of Scheme handler interface implementation for mms:// scheme
*      
*
*/


// INCLUDE FILES

#include "MmsHandler.h"
#include "SchemeDispLogger.h"
#include <ecom/ecom.h>		// For REComSession
#include <eikenv.h>
#include <DocumentHandler.h>
#include <apgcli.h>
#include <apparc.h>
#include <eikdoc.h>
#include <eikproc.h>
#include <f32file.h>
#include <AknLaunchAppService.h>

// ================= CONSTANTS =======================

LOCAL_C const TUid KUidMediaPlayer = { 0x10005A3E };

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CMmsHandler::NewL()
// ---------------------------------------------------------
//
CMmsHandler* CMmsHandler::NewL( const TDesC& aUrl )
	{
	CLOG_ENTERFN( "CMmsHandler::NewL()" );

	CMmsHandler* self=new(ELeave) CMmsHandler();
	CleanupStack::PushL(self);
	self->ConstructL( aUrl );
	CleanupStack::Pop(self);

	CLOG_LEAVEFN( "CMmsHandler::NewL()" );

	return self;
	}

// ---------------------------------------------------------
// CMmsHandler::~CMmsHandler()
// ---------------------------------------------------------
//
CMmsHandler::~CMmsHandler()
	{
	CLOG_ENTERFN( "CMmsHandler::~CMmsHandler()" );

	if(iDoc != NULL)
		{
		CEikProcess*  hostProcess = CEikonEnv::Static()->Process();
		hostProcess->DestroyDocument(iDoc);
		iDoc = NULL;
		}
		
	if( iLaunchAppService )
		{
		delete iLaunchAppService;
		}		

	CLOG_LEAVEFN( "CMmsHandler::~CMmsHandler()" );
	}

// ---------------------------------------------------------
// CMmsHandler::CMmsHandler()
// ---------------------------------------------------------
//
CMmsHandler::CMmsHandler() : CBaseHandler()
	{
	// Deliberately do nothing here : See ConstructL() for initialisation completion.
	}

// ---------------------------------------------------------
// CMmsHandler::ConstructL()
// ---------------------------------------------------------
//
void CMmsHandler::ConstructL( const TDesC& aUrl )
	{
	BaseConstructL( aUrl );

	iSync = EFalse;
	}

// ---------------------------------------------------------
// CMmsHandler::HandleUrlEmbeddedL()
// ---------------------------------------------------------
//
void CMmsHandler::HandleUrlEmbeddedL()
    {
    CLOG_ENTERFN( "CMmsHandler::HandleUrlEmbeddedL()" );

    /* Launch the appropriate application in embedded mode */
    
    CAiwGenericParamList* paramList = CAiwGenericParamList::NewLC();

    TAiwVariant filename( iParsedUrl );
    TAiwGenericParam param( EGenericParamFile, filename );
    paramList->AppendL( param );    

    // Allow save among Options
    TBool allowSave( ETrue );
    TAiwVariant allowSaveVariant( allowSave );
    TAiwGenericParam genericParamAllowSave
                     ( EGenericParamAllowSave, allowSaveVariant );
    paramList->AppendL( genericParamAllowSave );

    // If iParamList is not empty, let's get it and attach to the paramlist 
    if (iParamList)
    {
    	paramList->AppendL(*iParamList);  
    }


    iLaunchAppService = 
        CAknLaunchAppService::NewL( KUidMediaPlayer, this, paramList );
    iSync = ETrue;
    iWait.Start();
    CleanupStack::PopAndDestroy( paramList );   

    CLOG_LEAVEFN( "CMmsHandler::HandleUrlEmbeddedL()" );
    }

// ---------------------------------------------------------
// CMmsHandler::HandleUrlStandaloneL()
// ---------------------------------------------------------
//
void CMmsHandler::HandleUrlStandaloneL()
	{
	CLOG_ENTERFN( "CMmsHandler::HandleUrlStandaloneL()" );

	RApaLsSession appArcSession;
	User::LeaveIfError( appArcSession.Connect() );
	TThreadId id;

	appArcSession.StartDocument( iParsedUrl->Des(), KUidMediaPlayer , id );

	appArcSession.Close();

	CLOG_LEAVEFN( "CMmsHandler::HandleUrlStandaloneL()" );
	}
		
// ---------------------------------------------------------
// CMmsHandler::HandleServerAppExit()
// ---------------------------------------------------------
//
void CMmsHandler::HandleServerAppExit(TInt aReason)
	{
	CLOG_ENTERFN( "CMmsHandler::HandleServerAppExit" );

	if( iSync )
		{
	    if( iWait.IsStarted() )
            {
            iWait.AsyncStop();   // stop the wait loop.                                  
            }                    // Now DoMakeCallL will return
		}

	if( NULL != iSchemeDoc )
		{
	    iSchemeDoc->HandleServerAppExit( aReason );
		}

	CLOG_LEAVEFN( "CMmsHandler::HandleServerAppExit" );	
	}

