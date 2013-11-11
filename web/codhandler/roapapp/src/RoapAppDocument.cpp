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
* Description: 
*      Implementation of class CRoapAppDocument.   
*      
*
*/


// INCLUDE FILES

#include <RoapEng.h>
#include <RoapTrigger.h>
#include <AiwGenericParam.h>
#include "RoapAppDocument.h"
#include "RoapAppData.h"
#include "RoapAppUi.h"
#include "RequestCompleteCallback.h"
#include "RoapAppLogger.h"
#include "DRMRights.h" 
// ================= CONSTANTS =======================
_LIT8( KRoResponseHeader, "<roap:roResponse" );

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CRoapAppDocument::NewL()
// ---------------------------------------------------------
//
CRoapAppDocument* CRoapAppDocument::NewL( CEikApplication& aApp )
    {
    CRoapAppDocument* doc = new (ELeave) CRoapAppDocument( aApp );
    CleanupStack::PushL( doc );
    doc->ConstructL();
    CleanupStack::Pop( doc );
    return doc;
    }
    
// ---------------------------------------------------------
// CRoapAppDocument::~CRoapAppDocument()
// ---------------------------------------------------------
//
CRoapAppDocument::~CRoapAppDocument()
    {
    delete iData;
    delete iEngine;
    iParsedRights.ResetAndDestroy(); 
    }

// ---------------------------------------------------------
// CRoapAppDocument::Engine()
// ---------------------------------------------------------
//
Roap::CRoapEng& CRoapAppDocument::Engine() const
    {
    return *iEngine;
    }

// ---------------------------------------------------------
// CRoapAppDocument::Data()
// ---------------------------------------------------------
//
const CRoapAppData& CRoapAppDocument::Data() const
    {
    return *iData;
    }

// ---------------------------------------------------------
// CRoapAppDocument::CreateAppUiL()
// ---------------------------------------------------------
//
CEikAppUi* CRoapAppDocument::CreateAppUiL()
    {
    return new (ELeave) CRoapAppUi;
    }

// ---------------------------------------------------------
// CRoapAppDocument::OpenFileL()
// ---------------------------------------------------------
//
CFileStore* CRoapAppDocument::OpenFileL
( TBool aDoOpen, const TDesC& aFilename, RFs& aFs )
    {
    CLOG(( 2, _L("-> CRoapAppDocument::OpenFileL (TBool+TDesC+RFs)") ));
    CLOG(( 2, _L("   CRoapAppDocument: aDoOpen(%d) aFname<%S>"), \
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
        CLOG(( 2, _L("   CRoapAppDocument: RFile.Open() -> err=%d)"), err ));
        User::LeaveIfError( err );
        CleanupClosePushL<RFile>( file );
        store = OpenFileL( aFs, file );
        CleanupStack::PopAndDestroy();  // close file
        }
    CLOG(( 2, _L("<- CRoapAppDocument::OpenFileL") ));
    return store;
    }

// ---------------------------------------------------------
// CRoapAppDocument::OpenFileL()
// ---------------------------------------------------------
//
CFileStore* CRoapAppDocument::OpenFileL( RFs& /*aFs*/, RFile& aFile )
    {
    CLOG(( 2, _L("-> CRoapAppDocument::OpenFileL (RFs+RFile)") ));
    CFileStore* store( NULL );
    OpenFileL( store, aFile );
    CLOG(( 2, _L("<- CRoapAppDocument::OpenFileL (RFs+RFile)") ));
    return store;
    }

// ---------------------------------------------------------
// CRoapAppDocument::OpenFileL()
// ---------------------------------------------------------
//
void CRoapAppDocument::OpenFileL( CFileStore*& aFileStore, RFile& aFile )
    {
    CLOG(( 2, _L("-> CRoapAppDocument::OpenFileL (CFileStore+RFile)")  ));
    if( iFileOpened )
        {
        CLOG(( 2, _L("<- CRoapAppDocument::OpenFileL: iFileOpened == ETrue") ));
        return;
        }
    if( iEngine->IsActive() )
        {
        // Busy with something else.
        // Maybe we should Cancel the ongoing operation? I don't think so.
        CLOG(( 2, _L("  leave with KErrNotReady") ));
        User::Leave( KErrNotReady );
        }
    TInt fSize;
    User::LeaveIfError( aFile.Size( fSize ) );
    HBufC8* buf = HBufC8::NewLC( fSize );
    TPtr8 bufP( buf->Des() );
    User::LeaveIfError( aFile.Read( bufP ) );
    iFileOpened = ETrue;
    // 1-Pass RO addition    
    if( buf->Find( KRoResponseHeader ) != KErrNotFound )
        {
        TInt ret = 0; 
        TRAP (ret, iEngine->HandleRoReponseL( bufP, iParsedRights ) );
        if ( ret == KErrNone && iParsedRights.Count() )
            {
            // Ro's parsed succesfully, buf can be re-used,
            CleanupStack::PopAndDestroy( buf );
            aFileStore = NULL;
            buf = NULL;
            for( TUint i = 0; i < iParsedRights.Count(); i++ )
                {
                iParsedRights[i]->GetContentURI( buf );
                CleanupStack::PushL( buf );
                iData->iContentIdList.AppendL( buf );
                CleanupStack::Pop(); // buf
                }               
            if( iAppUi )
                {
                // Show notes
                iData->iType = Roap::ERoAcquisitionTrigger;
                STATIC_CAST( CRoapAppUi*, iAppUi )->HandleOnePassRoL(KErrNone);
                }     
            }
        else 
            {
            CleanupStack::PopAndDestroy( buf );
            if( iAppUi )
                {
                // Show notes
                iData->iType = Roap::ERoAcquisitionTrigger;
                STATIC_CAST( CRoapAppUi*, iAppUi )->HandleOnePassRoL(ret);
                }     
            }  
        // close the file
        aFile.Close();  
        iFileOpened = EFalse;
        return; 
        }
        // end of 1-Pass RO addition 
    const CAiwGenericParamList* params = GetInputParameters();
    iEngine->SetTriggerL
        (
        bufP,
        params,
        iData->iType,
        iData->iContextStatus,
        iData->iDomainOperation,
        iData->iContentIdList
        );
    if ( iEngine->Trigger().iRiAlias )
        {
        iData->iRiAlias = iEngine->Trigger().iRiAlias->AllocL();
        }
    CleanupStack::PopAndDestroy( buf );
    iFileOpened = EFalse;
    aFile.Close();
#ifdef __TEST_ROAP_APP_LOG
    CLOG(( 4, _L("  Trigger type(%d) contextStatus(%d) domainOp(%d)"), \
        iData->iType, iData->iContextStatus, iData->iDomainOperation ));
    TPtrC8 ptr;
    for ( TInt i = 0; i < iData->iContentIdList.Count(); i++ )
        {
        ptr.Set( iData->iContentIdList[i]->Des() );
        CLOG(( 4, _L8("  cid#%d:<%S>"), i, &ptr ));
        }
    if ( iData->iRiAlias )
        {
        ptr.Set( iData->iRiAlias->Des() );
        CLOG(( 4, _L8("  riAlias <%S>"), &ptr ));
        }
    else
        {
        CLOG(( 4, _L8("  riAlias NULL") ));
        }
#endif /* def __TEST_ROAP_APP_LOG */
    if ( iAppUi )
        {
        STATIC_CAST( CRoapAppUi*, iAppUi )->HandleTriggerParsedL();
        }
    CLOG(( 2, _L("<- CRoapAppDocument::OpenFileL (CFileStore+RFile)") ));
    aFileStore = NULL;
    }

// ---------------------------------------------------------
// CRoapAppDocument::CRoapAppDocument()
// ---------------------------------------------------------
//
CRoapAppDocument::CRoapAppDocument( CEikApplication& aApp )
: CAiwGenericParamConsumer( aApp )
    {
    }

// ---------------------------------------------------------
// CRoapAppDocument::ConstructL()
// ---------------------------------------------------------
//
void CRoapAppDocument::ConstructL()
    {
    CLOG(( 2, _L("-> CRoapAppDocument::ConstructL")  ));
    iEngine = Roap::CRoapEng::NewL();
    iData = CRoapAppData::NewL();
    CLOG(( 2, _L("<- CRoapAppDocument::ConstructL")  ));
    }
