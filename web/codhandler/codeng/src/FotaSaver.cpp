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
* Description:  Implementation of class CFotaSaver.   
*
*/


// INCLUDE FILES

#include "FotaSaver.h"
#include "CodLogger.h"
#include "CodError.h"
#include "CodPanic.h"
#include "CodData.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CFotaSaver::NewL()
// ---------------------------------------------------------
//
CFotaSaver* CFotaSaver::NewL( const TDesC8& aType, TInt aPkgId )
    {
    CFotaSaver* saver = new (ELeave) CFotaSaver( aType, aPkgId );
    CleanupStack::PushL( saver );
    saver->ConstructL();
    CleanupStack::Pop( saver );
    return saver;
    }

// ---------------------------------------------------------
// CFotaSaver::~CFotaSaver()
// ---------------------------------------------------------
//
CFotaSaver::~CFotaSaver()
    {
    CLOG(( ECodEng, 2, _L("-> CFotaSaver::~CFotaSaver") ));

    iEng.Close();
    // iStore not closed (owned by the Engine).
    CLOG(( ECodEng, 2, _L("<- CFotaSaver::~CFotaSaver") ));
    }

// ---------------------------------------------------------
// CFotaSaver::OpenStoreL()
// ---------------------------------------------------------
//
void CFotaSaver::OpenStoreL()
    {
    CLOG(( ECodEng, 2, _L("-> CFotaSaver::OpenStoreL") ));
    __ASSERT_DEBUG( iState == EInit, CodPanic( ECodInternal ) );
    
	TInt err(KErrNone);
	
    if (!iEng.Handle() )
        {
        TRAP( err, iEng.OpenL() );
        }
    if ( !err && !iStore )
        {
        err = iEng.OpenUpdatePackageStore( iPkgId, iStore );
        }

    iSize = 0;
    iState = EStoreOpen;
    CLOG(( ECodEng, 2, _L("<- CFotaSaver::OpenStoreL") ));
    }
    
// ---------------------------------------------------------
// CFotaSaver::AppendData()
// ---------------------------------------------------------
//
TInt CFotaSaver::AppendData( const TDesC8& aData )
    {
    CLOG(( EHttpLoad, 2, \
        _L("-> CFotaSaver::AppendData (%d) bytes"), aData.Length() ));
    __ASSERT_DEBUG( iState == EStoreOpen, CodPanic( ECodInternal ) );
    TInt err = CheckMaxSize( aData.Size() );
    if ( !err && iStore )
        {
        TRAP( err, iStore->WriteL( aData ) );
        }
    if ( !err )
        {
        iSize += aData.Length();
        }
    CLOG(( EHttpLoad, 2, _L("<- CFotaSaver::AppendData returns (%d)"), err ));
    return err;
    }

// ---------------------------------------------------------
// CFotaSaver::CloseStore()
// ---------------------------------------------------------
//
void CFotaSaver::CloseStore()
    {
    CLOG(( ECodEng, 2, _L("CFotaSaver::CloseStore") ));
    // Nothing to do, iStore is owned by FOTA Engine.
    iState = EStoreClosed;
    }

// ---------------------------------------------------------
// CFotaSaver::CheckResponseAttributesL()
// ---------------------------------------------------------
//
void CFotaSaver::CheckResponseAttributesL( const CCodData& aData )
    {
    CLOG(( ECodEng, 2, _L("-> CFotaSaver::CheckResponseAttributesL") ));
    __ASSERT_DEBUG( iState == EStoreClosed, CodPanic( ECodInternal ) );
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
 
    if( iType.Des8().Find( (*aData[aData.ActiveDownload()]).Type() ) == KErrNotFound )
        {
        CLOG(( ECodEng, 4, _L(" content-type mismatch") ));
        User::Leave( KErrCodAttributeMismatch );
        }

    iState = ERespChecked;
    CLOG(( ECodEng, 2, _L("<- CFotaSaver::CheckResponseAttributesL (match)") ));
    }

// ---------------------------------------------------------
// CFotaSaver::InstallL()
// ---------------------------------------------------------
//
void CFotaSaver::InstallL( TRequestStatus* aStatus, const TDesC& /*aName*/, const TBool /*aAttached*/ )
    {
    CLOG(( ECodEng, 2, _L("CFotaSaver::InstallL") ));
    __ASSERT_DEBUG( iState == ERespChecked, CodPanic( ECodInternal ) );
    // Do nothing. Update package is already in place.
    *aStatus = KRequestPending;
    User::RequestComplete( aStatus, KErrNone );

    iState = EInstalled;
    }

// ---------------------------------------------------------
// CFotaSaver::CancelInstall()
// ---------------------------------------------------------
//
void CFotaSaver::CancelInstall()
    {
    CLOG(( ECodEng, 2, _L("CFotaSaver::CancelInstall") ));
    // Do nothing, saving is completed already (not async).
    }

// ---------------------------------------------------------
// CFotaSaver::ReleaseContent()
// ---------------------------------------------------------
//
void CFotaSaver::ReleaseContent( TFileName& aFname, TUid& aHandler )
    {
    CLOG(( ECodEng, 2, _L("-> CFotaSaver::ReleaseContent") ));
    __ASSERT_DEBUG( iState == EInstalled, CodPanic( ECodInternal ) );
    if ( iStore )
        {
        iEng.UpdatePackageDownloadComplete( iPkgId );
        }
    aFname = KNullDesC;
    aHandler = KNullUid; // TODO get UID of handler app (DeviceManager)?
    iEng.Close();
    iStore = NULL;  // Store not closed (owned by the Engine).
    iState = EInit;
    CLOG(( ECodEng, 2, _L("<- CFotaSaver::ReleaseContent") ));
    }

// ---------------------------------------------------------
// CFotaSaver::ReleaseFileName()
// ---------------------------------------------------------
//
void CFotaSaver::ReleaseFileName( TFileName& /*aFname*/)
{
}
    
// ---------------------------------------------------------
// CFotaSaver::Cleanup()
// ---------------------------------------------------------
//
void CFotaSaver::Cleanup( TBool aDeleteFile)
    {
    CLOG(( ECodEng, 2, _L("-> CFotaSaver::Cleanup") ));

    if( iEng.Handle() )
    	{
        iEng.UpdatePackageDownloadComplete( iPkgId );
		if( aDeleteFile )
		    {
			iEng.DeleteUpdatePackage( iPkgId );
		    }
		iEng.Close();   // Close without committing to clean up.
    	}
    iStore = NULL;  // Store not closed (owned by the Engine).
    iState = EInit;
    CLOG(( ECodEng, 2, _L("<- CFotaSaver::Cleanup") ));
    }

// ---------------------------------------------------------
// CFotaSaver::CFotaSaver()
// ---------------------------------------------------------
//
CFotaSaver::CFotaSaver( const TDesC8& aType, TInt aPkgId )
: CCodSaver( aType ),
  iPkgId( aPkgId )
    {
    CLOG(( ECodEng, 2, _L("*** CFotaSaver::CFotaSaver") ));
    }

// ---------------------------------------------------------
// CFotaSaver::DownloadedFileSize()
// ---------------------------------------------------------
//
TInt CFotaSaver::DownloadedFileSize()
    {
    // check how many bytes are already persisted
    TInt DownloadedSize(0);
    TInt completeSize (0);

    //Get the downloaded size from Fota Server
    TInt err = iEng.GetDownloadUpdatePackageSize( iPkgId, DownloadedSize, completeSize );

    if(err != KErrNone)
        return err;
    
    return DownloadedSize;
    }

// ---------------------------------------------------------
// CFotaSaver::ResetL()
// ---------------------------------------------------------
//
void CFotaSaver::ResetL()
    {
    iEng.UpdatePackageDownloadComplete( iPkgId );
    
    TInt error = iEng.DeleteUpdatePackage( iPkgId );
    User::LeaveIfError(error);
    
	error = iEng.OpenUpdatePackageStore( iPkgId, iStore );
	User::LeaveIfError(error);
    }
