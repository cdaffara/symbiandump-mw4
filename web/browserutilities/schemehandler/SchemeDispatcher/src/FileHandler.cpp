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

#include "FileHandler.h"
#include "SchemeDispLogger.h"
#include <ecom/ecom.h>		// For REComSession
#include <eikenv.h>
#include <apmstd.h>
#include <apparc.h>
#include <eikproc.h>
#include <f32file.h>
#include <apgtask.h>
#include <apgcli.h>

// ================= CONSTANTS =======================
_LIT( KLocalhostPattern,"file://localhost/");
_LIT( KEmptyPattern,"file:///");
_LIT( KFilePattern,"file://");
_LIT( KBrowserPrefix, "4 " );

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CFileHandler::NewL()
// ---------------------------------------------------------
//
CFileHandler* CFileHandler::NewL( const TDesC& aUrl )
	{
	CLOG_ENTERFN( "CFileHandler::NewL()" );

	CFileHandler* self=new(ELeave) CFileHandler();
	CleanupStack::PushL( self );
	self->ConstructL( aUrl );
	CleanupStack::Pop( self );

	CLOG_LEAVEFN( "CFileHandler::NewL()" );
	return self;
	}

// ---------------------------------------------------------
// CFileHandler::~CFileHandler()
// ---------------------------------------------------------
//
CFileHandler::~CFileHandler()
	{
	CLOG_ENTERFN( "CFileHandler::~CFileHandler()" );

    if( iDocHandler )
        {
	    delete iDocHandler;
        }

	if(iDoc != NULL)
		{
		CEikProcess*  hostProcess = CEikonEnv::Static()->Process();
		hostProcess->DestroyDocument(iDoc);
		iDoc = NULL;
		}

	if( iLauncher )
		{
		delete iLauncher;
		iLauncher = NULL;
		}		

	CLOG_LEAVEFN( "CFileHandler::~CFileHandler()" );
	}

// ---------------------------------------------------------
// CFileHandler::CFileHandler()
// ---------------------------------------------------------
//
CFileHandler::CFileHandler() : CBaseHandler()
	{
	// Deliberately do nothing here : See ConstructL() for initialisation 
    // completion.
	}

// ---------------------------------------------------------
// CFileHandler::ConstructL()
// ---------------------------------------------------------
//
void CFileHandler::ConstructL( const TDesC& aUrl )
	{
	iParsedUrl = HBufC::NewL( aUrl.Length() );
	iParsedUrl->Des().Copy( aUrl );
	}

// ---------------------------------------------------------
// CFileHandler::HandleUrlEmbeddedL()
// ---------------------------------------------------------
//
void CFileHandler::HandleUrlEmbeddedL()
	{
	CLOG_ENTERFN( "CFileHandler::HandleUrlEmbeddedL()" );

    TBool dochandler( ETrue );
    HBufC* buf = ParseFileSchemeLC( dochandler );

    if( dochandler )
        {
        /* Documnet Handler launches the file */
        iDocHandler = CDocumentHandler::NewL(
              CEikonEnv::Static()->Process() );

        iDocHandler->SetExitObserver( this );
        TDataType dataType;

        CAiwGenericParamList& paramList = iDocHandler->InParamListL();
        RFile file;  
        iDocHandler->OpenTempFileL( buf->Des(), file );
        CLOG_WRITE(" OpenTempFileL OK");
        CleanupClosePushL( file );
        iDocHandler->OpenFileEmbeddedL( file, dataType, paramList );
        CLOG_WRITE(" OpenFileEmbeddedL OK");
        CleanupStack::PopAndDestroy( &file ); // file
        CLOG_WRITE(" PopAndDestroy( &file ) OK");
        }
    else
        {
	    HBufC* buf16 = HBufC::NewLC( 
		    iParsedUrl->Des().Length() + KBrowserPrefix.iTypeLength );
	    buf16->Des().Copy( KBrowserPrefix );
	    buf16->Des().Append( *buf );
	            
/*        iLauncher = CBrowserLauncher::NewL();
        iLauncher->LaunchBrowserEmbeddedL( buf16->Des(), 
                                           NULL, 
                                           this );*/
        CleanupStack::PopAndDestroy();  // buf16        
        }
	CleanupStack::PopAndDestroy(); //buf

	CLOG_LEAVEFN( "CFileHandler::HandleUrlEmbeddedL()" );
	}

// ---------------------------------------------------------
// CFileHandler::HandleUrlStandaloneL()
// ---------------------------------------------------------
//
void CFileHandler::HandleUrlStandaloneL()
	{
	CLOG_ENTERFN( "CFileHandler::HandleUrlStandaloneL()" );

    LaunchSchemeAppWithCommandLineL();

	CLOG_LEAVEFN( "CFileHandler::HandleUrlStandaloneL()" );
	}
	
// ---------------------------------------------------------
// CFileHandler::HandleServerAppExit()
// ---------------------------------------------------------
//
void CFileHandler::HandleServerAppExit(TInt aReason)
	{
	CLOG_ENTERFN( "CFileHandler::HandleServerAppExit" );

	if( NULL != iSchemeDoc )
		{
	    iSchemeDoc->HandleServerAppExit( aReason );
		}

	CLOG_LEAVEFN( "CFileHandler::HandleServerAppExit" );	
	}

// ---------------------------------------------------------
// CFileHandler::ConvertUrlToPathL()
// ---------------------------------------------------------
//
void CFileHandler::ConvertUrlToPathL( TDes& aLiteral )
	{
	/* Search backslash in the path and change it to slash*/
	_LIT16(KBackSlash16,"/");
	_LIT16(KSlash16,"\\");

	/* Find until the last occurance */
	TInt pos;
	while( KErrNotFound != ( pos = aLiteral.FindF( KBackSlash16 ) ) )
		{
		aLiteral.Replace(pos,1,KSlash16);
		}
	}

// ---------------------------------------------------------
// CFileHandler::ConvertUrlToPathL()
// ---------------------------------------------------------
//
void CFileHandler::ConvertPathToUrlL( TDes& aLiteral )
	{
	/* Search backslash in the path and change it to slash*/
	_LIT16(KBackSlash16,"/");
	_LIT16(KSlash16,"\\");

	/* Find until the last occurance */
	TInt pos;
	while( KErrNotFound != ( pos = aLiteral.FindF( KSlash16 ) ) )
		{
		aLiteral.Replace( pos, 1, KBackSlash16);
		}
	}

// ---------------------------------------------------------
// CFileHandler::ParseFileSchemeLC()
// ---------------------------------------------------------
//
HBufC* CFileHandler::ParseFileSchemeLC( TBool& aDocHandler )
    {
    TPtrC path;
    HBufC* pathCopy = NULL;
    TRAPD( err, path.Set( RemoveSchemeFromUrlL( KLocalhostPattern() ) ) );
    if( !err )
        {
	    pathCopy = HBufC::NewLC( path.Length() );
	    pathCopy->Des().Copy( path );
        TPtr ptr = pathCopy->Des();
        ConvertUrlToPathL( ptr );
        aDocHandler = ETrue;
        return pathCopy;
        }

    TRAP( err,  path.Set( RemoveSchemeFromUrlL( KEmptyPattern() ) ) );
    if( !err )
        {
	    pathCopy = HBufC::NewLC( path.Length() );
	    pathCopy->Des().Copy( path );
        TPtr ptr = pathCopy->Des();
        ConvertUrlToPathL( ptr );
        aDocHandler = ETrue;
        return pathCopy;
        }

    TRAP( err,  path.Set( RemoveSchemeFromUrlL( KFilePattern() ) ) );
    if( !err )
        {
        // Does our storage file exist?
	    pathCopy = HBufC::NewLC( path.Length() );
	    pathCopy->Des().Copy( path );
        TPtr ptr = pathCopy->Des();
        ConvertUrlToPathL( ptr );
        RFs fs;
        User::LeaveIfError( fs.Connect() );
        TEntry storageFile;
        TInt err = fs.Entry( ptr, storageFile );
        fs.Close();
        if ( err )
            {
            // It does not exist.
            ConvertPathToUrlL( ptr );
            aDocHandler = EFalse;
            return pathCopy;
            }
        else
            {
            aDocHandler = ETrue;
            return pathCopy;
            }
        }

    User::Leave( err );
    return pathCopy;
    }
