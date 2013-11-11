/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of class CRoapSaver.   
*
*/


// INCLUDE FILES

#include <RoapEng.h>
#include <RoapTrigger.h>
#include "RoapSaver.h"
#include "RoapData.h"
#include "CodLoadObserver.h"
#include "CodLogger.h"
#include "CodError.h"
#include "CodPanic.h"
#include "CodProgress.h"
#include "CodData.h"
#include <AiwGenericParam.h>

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CRoapSaver::NewL()
// ---------------------------------------------------------
//
CRoapSaver* CRoapSaver::NewL
        (
        const TDesC8& aType,
        CRoapData*& aData,
        TCodProgress* aProgress,
        TInt aProgressBytes,
        const TFileName& aTempPath,
        const TFileName& aRootPath,
        const TFileName& aFname
        )
    {
    CRoapSaver* saver = new (ELeave) CRoapSaver
        ( aType, aData, aProgress, aProgressBytes, aTempPath, aRootPath, aFname );
    CleanupStack::PushL( saver );
    saver->ConstructL();
    CleanupStack::Pop( saver );
    return saver;
    }

// ---------------------------------------------------------
// CRoapSaver::~CRoapSaver()
// ---------------------------------------------------------
//
CRoapSaver::~CRoapSaver()
    {
    CLOG(( ECodEng, 2, _L("-> CRoapSaver::~CRoapSaver") ));
    delete iEng;
    delete iBuf;
    CLOG(( ECodEng, 2, _L("<- CRoapSaver::~CRoapSaver") ));
    }

// ---------------------------------------------------------
// CRoapSaver::OpenStoreL()
// ---------------------------------------------------------
//
void CRoapSaver::OpenStoreL()
    {
    CLOG(( ECodEng, 2, _L("-> CRoapSaver::OpenStoreL") ));
    __ASSERT_DEBUG( iState == EInit, CodPanic( ECodInternal ) );
    __ASSERT_DEBUG( !iBuf, CodPanic( ECodInternal ) );
    iSize = 0;
    iState = EStoreOpen;
    CLOG(( ECodEng, 2, _L("<- CRoapSaver::OpenStoreL") ));
    }
    
// ---------------------------------------------------------
// CRoapSaver::AppendData()
// ---------------------------------------------------------
//
TInt CRoapSaver::AppendData( const TDesC8& aData )
    {
    CLOG(( ECodEng, 4, \
        _L("-> CRoapSaver::AppendData (%d) bytes"), aData.Length() ));
//TODO: iState = EInstalling for dd2    __ASSERT_DEBUG( iState == EStoreOpen, CodPanic( ECodInternal ) );
    TInt err = CheckMaxSize( aData.Size() );
    if ( !err )
        {
        TInt needed = iSize + aData.Length();
        if ( !iBuf )
            {
            iBuf = HBufC8::New( needed );
            if ( !iBuf )
                {
                err = KErrNoMemory;
                }
            }
        if ( !err && needed > iBuf->Des().MaxLength() )
            {
            HBufC8* buf = iBuf->ReAlloc( needed );
            if ( buf )
                {
                iBuf = buf;
                }
            else
                {
                err = KErrNoMemory;
                }
            }
        if ( !err )
            {
            iBuf->Des().Append( aData );
            iSize += aData.Length();
            }
        }
    CLOG(( EHttpLoad, 2, _L("<- CFileSaver::AppendData returns (%d)"), err ));
    return err;
    }

// ---------------------------------------------------------
// CRoapSaver::CloseStore()
// ---------------------------------------------------------
//
void CRoapSaver::CloseStore()
    {
    CLOG(( ECodEng, 2, _L("CRoapSaver::CloseStore") ));
    iState = EStoreClosed;
    }

// ---------------------------------------------------------
// CRoapSaver::CheckResponseAttributesL()
// ---------------------------------------------------------
//
void CRoapSaver::CheckResponseAttributesL( const CCodData& /*aData*/ )
    {
    CLOG(( ECodEng, 2, _L("-> CRoapSaver::CheckResponseAttributesL") ));
//TODO: tmp EInstalling for dd2    __ASSERT_DEBUG( iState == EStoreClosed, CodPanic( ECodInternal ) );
#ifdef __TEST_COD_LOG
    TPtrC8 mime( iType.Des8() );
    CLOG(( ECodEng, 4, _L8("  MIME==<%S>, size(%d)"), &mime, iSize ));
#endif /* def __TEST_COD_LOG */

    if ( !iSize )
        {
        CLOG(( ECodEng, 4, _L("  0 bytes data") ));
        User::Leave( KErrCodAttributeMismatch );
        }

    // Compare content MIME type against descriptor.

    // Size is not checked, no exact match is required. Quote form spec:
    // "The storage size and the execution size are dependent on the
    // environment and may be different from the value of the size attribute.
    // The transport size may also be different, if compression or some
    // packaging format is used."
    //
    // There is a safety upper bound on the transaction size, that is already
    // applied. See SetMaxSize().


// TODO (??) type not checked, ROAP Trigger is always accepted.
// HACK HACK DRM HACK
//    if( iType != TDataType( aData.Type() ) )
//        {
//        CLOG(( ECodEng, 4, _L("  mismatch") ));
//        User::Leave( KErrCodAttributeMismatch );
//        }

    iState = ERespChecked;
    CLOG(( ECodEng, 2, _L("<- CRoapSaver::CheckResponseAttributesL (match)") ));
    }

// ---------------------------------------------------------
// CRoapSaver::InstallL()
// ---------------------------------------------------------
//
void CRoapSaver::InstallL( TRequestStatus* aStatus, const TDesC& aName, const TBool /*aAttached*/ )
    {
    CLOG(( ECodEng, 2, _L("-> CRoapSaver::InstallL") ));
//TODO:    __ASSERT_DEBUG( iState == ERespChecked, CodPanic( ECodInternal ) );
    __ASSERT_ALWAYS( aStatus, CodPanic( ECodInternal ) );
    __ASSERT_DEBUG( iBuf, CodPanic( ECodInternal ) );
//TODO:    __ASSERT_DEBUG( iType == TDataType( KOma2TriggerContentType ), \
//        CodPanic( ECodInternal ) );

#ifdef __TEST_COD_LOG
    TPtrC8 ptr( iBuf->Des() );
    CDUMP(( ECodEng, 2, _S("Trigger:"), _S("        "), \
        (const TUint8*)ptr.Ptr(), ptr.Size() ));
#endif /* def __TEST_COD_LOG */

    iContentName.Set( aName );  // TODO, must presist - use CodData instead?
    __ASSERT_DEBUG( !iEng, CodPanic( ECodInternal ) );
    iEng = Roap::CRoapEng::NewL();
    if ( iData )
        {
        iData->Reset();
        }
    else
        {
        iData = CRoapData::NewL();
        }
    iEng->SetTriggerL
        (
        *iBuf,
        iParams,
        iData->iType,
        iData->iContextStatus,
        iData->iDomainOperation,
        iData->iContentIdList
        );

//TODO:    __ASSERT_DEBUG( !iData->iRiAlias, CodPanic( ECodInternal ) );
    if ( iEng->Trigger().iRiAlias )
        {
        iData->iRiAlias = iEng->Trigger().iRiAlias->AllocL();
        }

#ifdef __TEST_COD_LOG
    CLOG(( ECodEng, 3, _L("  Type(%d)"), iData->iType ));
    CLOG(( ECodEng, 3, _L("  ContextStatus(%d)"), iData->iContextStatus ));
    CLOG(( ECodEng, 3, _L("  DomainOp(%d)"), iData->iDomainOperation ));
    for ( TInt i = 0; i < iData->iContentIdList.Count(); i++ )
        {
        ptr.Set( iData->iContentIdList[i]->Des() );
        CLOG(( ECodEng, 3, _L8("  ContentId #%d <%S>"), i, &ptr ));
        }
    if ( iData->iRiAlias )
        {
        ptr.Set( iData->iRiAlias->Des() );
        CLOG(( ECodEng, 3, _L8("  RiAlias <%S>"), &ptr ));
        }
    else
        {
        CLOG(( ECodEng, 3, _L8("  RiAlias NULL") ));
        }
#endif /* def __TEST_COD_LOG */
    
    if ( iObserver )
        {
        CLOG(( ECodEng, 3, \
            _L("CRoapSaver::StartRoapL notify RoapTriggerParsedL") ));
        iObserver->RoapTriggerParsedL( *iData );
        }

    // CodHandler implements 'auto-accept' for ROAP Trigger.
    // ROAP is initiated without user interaction.
    iEng->AcceptL( this, aStatus );
    iState = EInstalling;

    CLOG(( ECodEng, 2, _L("<- CRoapSaver::InstallL") ));
    }

// ---------------------------------------------------------
// CRoapSaver::CancelInstall()
// ---------------------------------------------------------
//
void CRoapSaver::CancelInstall()
    {
    CLOG(( ECodEng, 2, _L("-> CRoapSaver::CancelInstall") ));
    if ( iEng )
        {
        iEng->Cancel();
        }
    CLOG(( ECodEng, 2, _L("<- CRoapSaver::CancelInstall") ));
    }

// ---------------------------------------------------------
// CRoapSaver::ReleaseContent()
// ---------------------------------------------------------
//
void CRoapSaver::ReleaseContent( TFileName& aFname, TUid& aHandler )
    {
    CLOG(( ECodEng, 2, _L("CRoapSaver::ReleaseContent") ));
    // Note: assert EInstalling state, not EInstalled. This object never gets
    // EInstalled as the request status is not ours -> it does not know
    // when installing is completed.
//TODO: EStoreClose for dd2    __ASSERT_DEBUG( iState == EInstalling, CodPanic( ECodInternal ) );
    aFname = iFname;
    aHandler = iHandler;
    iFname = KNullDesC;
    iHandler = KNullUid;
    iContentName.Set( KNullDesC );
    delete iEng;
    iEng = NULL;
    iState = EInit;
    }

// ---------------------------------------------------------
// CRoapSaver::ReleaseFileName()
// ---------------------------------------------------------
//
void CRoapSaver::ReleaseFileName( TFileName& /*aFname*/)
{
}

// ---------------------------------------------------------
// CRoapSaver::Cleanup()
// ---------------------------------------------------------
//
void CRoapSaver::Cleanup( TBool /* aDeleteFile */)
    {
    CLOG(( ECodEng, 2, _L("-> CRoapSaver::Cleanup") ));
    if ( iEng )
        {
        iEng->Cancel();
#ifdef __TEST_COD_LOG
        TInt err =  // ('Log-only' variable.)
#endif /* def __TEST_COD_LOG */
        iEng->DoCleanup();
        CLOG(( ECodEng, 2, _L("  CRoapSaver::Cleanup err(%d)"), err ));
        delete iEng;
        iEng = NULL;
        }
    iContentName.Set( KNullDesC );
    iState = EInit;
    CLOG(( ECodEng, 2, _L("<- CRoapSaver::Cleanup") ));
    }

// ---------------------------------------------------------
// CRoapSaver::CRoapSaver()
// ---------------------------------------------------------
//
CRoapSaver::CRoapSaver
    (
    const TDesC8& aType,
    CRoapData*& aData,
    TCodProgress* aProgress,
    TInt aProgressBytes,
    const TFileName& aTempPath,
    const TFileName& aRootPath,
    const TFileName& aFname
    )
: CCodSaver( aType ),
  iData( aData ),
  iProgress( aProgress ),
  iProgressBytes( aProgressBytes ),
  iTempPath( aTempPath ),
  iRootPath( aRootPath ),
  iFname( aFname)
    {
    CLOG(( ECodEng, 2, _L("*** CRoapSaver::CRoapSaver") ));
    }

// ----------------------------------------------------------
// CRoapSaver::ConnectionConfL()
// ----------------------------------------------------------
//
TBool CRoapSaver::ConnectionConfL()
    {
    CLOG(( ECodEng, 2, _L("-> CRoapSaver::ConnectionConfL") ));
    TBool conf( EFalse );
    if ( iObserver )
        {
        CLOG(( ECodEng, 3, \
            _L("CRoapSaver::ConnectionConfL notify ConnectionConfL") ));
        conf = iObserver->ConfirmConnectL();
        }
    CLOG(( ECodEng, 2, _L("<- CRoapSaver::ConnectionConfL (%d)"), conf ));
    return conf;
    }
    
// ----------------------------------------------------------
// CRoapSaver::ContactRiConfL()
// ----------------------------------------------------------
//
TBool CRoapSaver::ContactRiConfL()
    {
    CLOG(( ECodEng, 2, _L("CRoapSaver::ContactRiConfL") ));
    // CodHandler implements 'auto-accept' for ROAP Trigger.
    return ETrue;
    }
    
// ----------------------------------------------------------
// CRoapSaver::TransIdConfL()
// ----------------------------------------------------------
//
TBool CRoapSaver::TransIdConfL()
    {
    CLOG(( ECodEng, 2, _L("CRoapSaver::TransIdConfL") ));
    return EFalse;
    }
    
// ----------------------------------------------------------
// CRoapSaver::RightsObjectDetailsL()
// ----------------------------------------------------------
//
void CRoapSaver::RightsObjectDetailsL
( const RPointerArray<CDRMRights>& /*aRightsList*/ )
    {
    CLOG(( ECodEng, 2, _L("CRoapSaver::RightsObjectDetailsL") ));
    }
		    
// ----------------------------------------------------------
// CRoapSaver::ContentDownloadInfoL()
// ----------------------------------------------------------
//
void CRoapSaver::ContentDownloadInfoL
( TPath& aTempFolder, TFileName& aContentName, TInt& aMaxSize )
    {
    aTempFolder = iTempPath;
    aContentName = iContentName;
    aMaxSize = iMaxSize;        // TODO must to be set.
    }

// ----------------------------------------------------------
// CRoapSaver::ContentDetailsL()
// ----------------------------------------------------------
//
void CRoapSaver::ContentDetailsL
( const TDesC& aPath, const TDesC8& aType, const TUid& aAppUid )
    {
    iFname = aPath;
    iType = aType;
    iHandler = aAppUid;
    }
                                      
// ----------------------------------------------------------
// CRoapSaver::HandleDCFPartL()
// ----------------------------------------------------------
//
void CRoapSaver::HandleDCFPartL( const TDesC8& /*aFilename*/ )
    {
    CLOG(( ECodEng, 2, _L("CRoapSaver::HandleDCFPartL") ));
    // DRM Engine does not support multipart ROAP response yet.
    // This method will never be called.
    User::Leave( KErrNotSupported );
    }
        
// ----------------------------------------------------------
// CRoapSaver::RoapProgressInfoL()
// ----------------------------------------------------------
//
void CRoapSaver::RoapProgressInfoL( const TInt aProgressInfo )
    {
    CLOG(( ECodEng, 2, _L("CRoapSaver::RoapProgressInfoL (%d)"), \
        aProgressInfo ));
    if ( iProgress )
        {
        iProgress->SetAmountL( aProgressInfo );
        }
    }


// ----------------------------------------------------------
// CRoapSaver::ErrorUrlL()
// ----------------------------------------------------------
//
void CRoapSaver::ErrorUrlL( const TDesC8& /* aErrorUrl */)
{
	
}
// ----------------------------------------------------------
// CRoapSaver::PostResponseUrlL()
// ----------------------------------------------------------
//
void CRoapSaver::PostResponseUrlL( const TDesC8& aPrUrl )
    {
    CLOG(( ECodEng, 2, _L("-> CRoapSaver::PostResponseUrlL") ));
    CLOG(( ECodEng, 2, _L("aPrUrl = '%S'"), &aPrUrl ));

    //Send an event to MCodLoadObserver which sends an event to DownloadMgr, which handles the 
    // PostResponse URL like NextUriL.
    if ( iObserver )
        {
        iObserver->HandleRoapPostResponseUrlL( aPrUrl );
        }

    CLOG(( ECodEng, 2, _L("<- CRoapSaver::PostResponseUrlL") ));

    }


