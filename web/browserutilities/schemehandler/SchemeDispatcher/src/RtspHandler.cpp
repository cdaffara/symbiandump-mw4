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
*      Implementation of Scheme handler interface implementation for rtsp:// scheme
*      
*
*/


// INCLUDE FILES

#include "RtspHandler.h"
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
_LIT( KRtspFileName, "c:\\system\\temp\\RtspTemp.ram" );

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CRtspHandler::NewL()
// ---------------------------------------------------------
//
CRtspHandler* CRtspHandler::NewL( const TDesC& aUrl )
	{
	CLOG_ENTERFN( "CRtspHandler::NewL()" );

	CRtspHandler* self=new(ELeave) CRtspHandler();
	CleanupStack::PushL(self);
	self->ConstructL( aUrl );
	CleanupStack::Pop(self);

	CLOG_LEAVEFN( "CRtspHandler::NewL()" );

	return self;
	}

// ---------------------------------------------------------
// CRtspHandler::~CRtspHandler()
// ---------------------------------------------------------
//
CRtspHandler::~CRtspHandler()
	{
	CLOG_ENTERFN( "CRtspHandler::~CRtspHandler()" );

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

    if( iDocHandler )
        {
	    delete iDocHandler;
        }

	CLOG_LEAVEFN( "CRtspHandler::~CRtspHandler()" );
	}

// ---------------------------------------------------------
// CRtspHandler::CRtspHandler()
// ---------------------------------------------------------
//
CRtspHandler::CRtspHandler() : CBaseHandler()
	{
	// Deliberately do nothing here : See ConstructL() for initialisation completion.
	}

// ---------------------------------------------------------
// CRtspHandler::ConstructL()
// ---------------------------------------------------------
//
void CRtspHandler::ConstructL( const TDesC& aUrl )
	{
	BaseConstructL( aUrl );

	iSync = EFalse;
	}

// ---------------------------------------------------------
// CRtspHandler::HandleUrlEmbeddedL()
// ---------------------------------------------------------
//
void CRtspHandler::HandleUrlEmbeddedL()
    {
    CLOG_ENTERFN( "CRtspHandler::HandleUrlEmbeddedL()" );

    /* Launch the appropriate application in embedded mode */

    CLOG_WRITE(" :launching RTSP url via temp RAM file");
    //
    RFs             rfs;
    RFile           ramFile;

    // 8-bit buffer is required.
    HBufC8* urlBuf = HBufC8::NewLC( iParsedUrl->Size() );
    CLOG_WRITE(" :HBufC8::NewL() ok");
    urlBuf->Des().Copy( *iParsedUrl );
    // Open the file.
    User::LeaveIfError(rfs.Connect());
    CleanupClosePushL(rfs);

    // Replace file if exists or Create file if not exist yet
    User::LeaveIfError( ramFile.Replace( rfs, KRtspFileName, EFileWrite | EFileShareAny ) );
    CleanupClosePushL(ramFile);

    // Write to file      
    User::LeaveIfError( ramFile.Write( *urlBuf ) );
    ramFile.Flush();
    ramFile.Close();

    CleanupStack::PopAndDestroy(/*ramFile*/);
    CleanupStack::PopAndDestroy(/*rfs*/);
    CleanupStack::PopAndDestroy( /*urlBuf*/ );

    RFile ramDocTempFile;
    if( !iDocHandler )
        {
        iDocHandler = CDocumentHandler::NewL( CEikonEnv::Static()->Process() );
        CLOG_WRITE(" :CDocumentHandler::NewL() ok");
        }
    iDocHandler->SetExitObserver( this );

    iDocHandler->OpenTempFileL( KRtspFileName, ramDocTempFile );
    CLOG_WRITE(" :iDocHandler->OpenTempFileL() ok");
    CleanupClosePushL( ramDocTempFile );
    //
    TDataType dataType( _L8("audio/x-pn-realaudio-plugin") );
    CAiwGenericParamList* paramList = CAiwGenericParamList::NewLC();
    TAiwVariant filename( _L8("c:\\system\\temp\\RtspTemp.ram") );
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

    // launch RAM file via DocHandler
    iDocHandler->OpenFileEmbeddedL( ramDocTempFile, dataType, *paramList );
    CLOG_WRITE(" OpenFileEmbeddedL OK");
    //
    CleanupStack::PopAndDestroy( paramList );
    CleanupStack::PopAndDestroy( &ramDocTempFile );

    CLOG_LEAVEFN( "CRtspHandler::HandleUrlEmbeddedL()" );
    }

// ---------------------------------------------------------
// CRtspHandler::HandleUrlStandaloneL()
// ---------------------------------------------------------
//
void CRtspHandler::HandleUrlStandaloneL()
	{
	CLOG_ENTERFN( "CRtspHandler::HandleUrlStandaloneL()" );

	RApaLsSession appArcSession;
	User::LeaveIfError( appArcSession.Connect() );
	TThreadId id;

	appArcSession.StartDocument( iParsedUrl->Des(), KUidMediaPlayer , id );

	appArcSession.Close();

	CLOG_LEAVEFN( "CRtspHandler::HandleUrlStandaloneL()" );
	}
		
// ---------------------------------------------------------
// CRtspHandler::HandleServerAppExit()
// ---------------------------------------------------------
//
void CRtspHandler::HandleServerAppExit(TInt aReason)
	{
	CLOG_ENTERFN( "CRtspHandler::HandleServerAppExit" );

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

	CLOG_LEAVEFN( "CRtspHandler::HandleServerAppExit" );	
	}

