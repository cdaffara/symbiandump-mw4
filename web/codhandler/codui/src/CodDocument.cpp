/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of class CCodDocument.   
*
*/


// INCLUDE FILES

#include <AiwGenericParam.h>
#include <CodEng.h>
#include <DdEng.h>
#include <CodUtil.h>
#include "CodDocument.h"
#include "CodAppUi.h"
#include "RequestCompleteCallback.h"
#include "CodLogger.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CCodDocument::CodAppUi()
// ---------------------------------------------------------
//
CCodAppUi* CCodDocument::CodAppUi()
    {
    // Inline method must be defined before it is used.
    return STATIC_CAST( CCodAppUi*, iAppUi );
    }
    
// ---------------------------------------------------------
// CCodDocument::NewL()
// ---------------------------------------------------------
//
CCodDocument* CCodDocument::NewL( CEikApplication& aApp, TBool aCod )
    {
    CCodDocument* doc = new (ELeave) CCodDocument( aApp, aCod );
    CleanupStack::PushL( doc );
    doc->ConstructL();
    CleanupStack::Pop( doc );
    return doc;
    }
    
// ---------------------------------------------------------
// CCodDocument::~CCodDocument()
// ---------------------------------------------------------
//
CCodDocument::~CCodDocument()
    {
    delete iModel;      // Delete (==Cancel) the model...
    delete iCallback;   // ...before deleting (==Cancelling) the callback.
    }

// ---------------------------------------------------------
// CCodDocument::Model()
// ---------------------------------------------------------
//
CCodEngBase& CCodDocument::Model()
    {
    return *iModel;
    }

// ---------------------------------------------------------
// CCodDocument::CreateAppUiL()
// ---------------------------------------------------------
//
CEikAppUi* CCodDocument::CreateAppUiL()
    {
    return new (ELeave) CCodAppUi( iCod );
    }

// ---------------------------------------------------------
// CCodDocument::OpenFileL()
// ---------------------------------------------------------
//
CFileStore* CCodDocument::OpenFileL
( TBool aDoOpen, const TDesC& aFilename, RFs& aFs )
    {
    CLOG(( 2, _L("-> CCodDocument::OpenFileL aDoOpen(0x%x) aFname<%S>"), \
        aDoOpen, &aFilename ));
    CFileStore* store( NULL );
    if ( aDoOpen )
        {
        RFile file;
        TInt err = file.Open( aFs, aFilename,
            EFileShareReadersOnly | EFileStream | EFileRead );
        if ( err == KErrInUse )
            {
            err = file.Open( aFs, aFilename,
                EFileShareAny | EFileStream | EFileRead );
            }
        User::LeaveIfError( err );
        CleanupClosePushL<RFile>( file );
        store = OpenFileL( aFs, file );
        CleanupStack::PopAndDestroy();  // close file
        }
    CLOG(( 2, _L("<- CCodDocument::OpenFileL") ));
    return store;
    }

// ---------------------------------------------------------
// CCodDocument::OpenFileL()
// ---------------------------------------------------------
//
CFileStore* CCodDocument::OpenFileL( RFs& /*aFs*/, RFile& aFile )
    {
    CLOG(( 2, _L("-> CCodDocument::OpenFileL (RFs+RFile)") ));
    CFileStore* store( NULL );
    OpenFileL( store, aFile );
    CLOG(( 2, _L("<- CCodDocument::OpenFileL (RFs+RFile)") ));
    return store;
    }

// ---------------------------------------------------------
// CCodDocument::OpenFileL()
// ---------------------------------------------------------
//
void CCodDocument::OpenFileL( CFileStore*& aFileStore, RFile& aFile )
    {
    CLOG(( 2, _L("-> CCodDocument::OpenFileL (RFile)") ));
    if( iModel->IsActive() )
        {
        // Busy with something else.
        // Maybe we should Cancel the ongoing operation? I don't think so.
        CLOG(( 2, _L("  leave with KErrNotReady") ));
        User::Leave( KErrNotReady );
        }

    // Get params, extract the UI related ones.
    const CAiwGenericParamList* params = GetInputParameters();
    if ( params )
        {
        TBool silent( EFalse );
        TBool suppressNextUrl( EFalse );
        TBool suppressLaunch( EFalse );
        CodUtil::GetBoolParam( silent, EGenericParamSilentDownload, *params );
        CodUtil::GetBoolParam
            ( suppressNextUrl, EGenericParamSuppressNextUrl, *params );
        CodUtil::GetBoolParam
            ( suppressLaunch, EGenericParamSuppressLaunch, *params );
        CodAppUi()->SetServiceFlow( silent, suppressNextUrl, suppressLaunch );
        }

    // Read descriptor data from file.
    TInt fSize;
    User::LeaveIfError( aFile.Size( fSize ) );
    HBufC8* buf = HBufC8::NewLC( fSize );
    TPtr8 bufP( buf->Des() );
    User::LeaveIfError( aFile.Read( bufP ) );
    
    iModel->SetL( bufP, &iCallback->iStatus, params, NULL );
    iCallback->CallbackOnCompletion
        ( TCallBack( StaticOpenFileDone, this ) );
    CleanupStack::PopAndDestroy( buf );
    aFileStore = NULL;
    CLOG(( 2, _L("<- CCodDocument::OpenFileL (RFile)") ));
    }
    
// ---------------------------------------------------------
// CCodDocument::CCodDocument()
// ---------------------------------------------------------
//
CCodDocument::CCodDocument( CEikApplication& aApp, TBool aCod )
: CAiwGenericParamConsumer( aApp ), iCod( aCod )
    {
    }

// ---------------------------------------------------------
// CCodDocument::ConstructL()
// ---------------------------------------------------------
//
void CCodDocument::ConstructL()
    {
    if ( iCod )
        {
        iModel = CCodEng::NewL( iEikProcess, NULL );
        }
    else
        {
        iModel = CDdEng::NewL( iEikProcess, NULL );
        }
        
    iCallback = new ( ELeave ) CRequestCompleteCallback();
    }

// ---------------------------------------------------------
// CCodDocument::OpenFileDone()
// ---------------------------------------------------------
//
void CCodDocument::OpenFileDone()
    {
    CLOG(( 2, _L("-> CCodDocument::OpenFileDone") ));
    // Note that it is possible that we don't have iAppUi here. There is
    // blank screen with "Back" softkey while parsing the COD.
    // During that, "Back" command may be issued and
    // processed by AppUi during the wait.
    if ( iAppUi )
        {
        CodAppUi()->OpenFileDone( iCallback->iStatus.Int() );
        }
    CLOG(( 2, _L("<- CCodDocument::OpenFileDone") ));
    }

// ---------------------------------------------------------
// CCodDocument::StaticOpenFileDone()
// ---------------------------------------------------------
//
TInt CCodDocument::StaticOpenFileDone( TAny* aPtr )
    {
    STATIC_CAST( CCodDocument*, aPtr )->OpenFileDone();
    return EFalse;  // TCallback should return EFalse as TInt.
    }
