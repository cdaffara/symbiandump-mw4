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
* Description:
*      Implementation of class CFileSaver.
*
*
*/


// INCLUDE FILES
//#include <platform/mw/Browser_platform_variant.hrh>
#include <Browser_platform_variant.hrh>
#include "FileSaver.h"
#include "CodBuffStorage.h"
#include "CodLoadObserver.h"
#include "RFeatMgr.h"
#include "CodUtil.h"
#include "CodLogger.h"
#include "CodError.h"
#include "CodPanic.h"
#include "DrmHandler.h"
#include "CodData.h"

#include <f32file.h>
#include <HttpDownloadMgrCommon.h>
#include <DocumentHandler.h>
#include <pathinfo.h>

// following one line is temporary: AVKON dependency removal
#undef BRDO_APP_GALLERY_SUPPORTED_FF

#ifdef BRDO_APP_GALLERY_SUPPORTED_FF
#include <MGXFileManagerFactory.h>
#include <CMGXFileManager.h>
#endif

#include <DcfEntry.h>
#include <DcfRep.h>

const TInt KDefaultStorageBufferSize = 128 * 1024;
const TInt KDefaultStorageBufferSizePD = 16 * 1024;


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CFileSaver::NewL()
// ---------------------------------------------------------
//
CFileSaver* CFileSaver::NewL
        (
        const TDesC8& aType,
        RFs& aFs,
        CDocumentHandler& aDocHandler,
        const TFileName& aTempPath,
        const TFileName& aRootPath,
        const TFileName& aFname
        )
    {
    CFileSaver* saver = new (ELeave) CFileSaver
        ( aType, aFs, aDocHandler, aTempPath, aRootPath, aFname );
    CleanupStack::PushL( saver );
    saver->ConstructL();
    CleanupStack::Pop( saver );
    return saver;
    }
// ---------------------------------------------------------
// CFileSaver::ConstructL()
// ---------------------------------------------------------
//
void CFileSaver::ConstructL()
    {
    CCodSaver::ConstructL();
    iBufferSize =  KDefaultStorageBufferSize ;
    iProgressiveDownload = EFalse ;

    iStorage = CCodBuffStorage::NewL(this);
    }

// ---------------------------------------------------------
// CFileSaver::CFileSaver()
// ---------------------------------------------------------
//
CFileSaver::CFileSaver
    (
    const TDesC8& aType,
    RFs& aFs,
    CDocumentHandler& aDocHandler,
    const TFileName& aTempPath,
    const TFileName& aRootPath,
    const TFileName& aFname
    )
: CCodSaver( aType ),
  iDocHandler( aDocHandler ),
  iFs( aFs ),
  iTempPath( aTempPath ),
  iRootPath( aRootPath ),
  iFname( aFname)
    {
    CLOG(( ECodEng, 2, _L("*** CFileSaver::CFileSaver") ));
    }

// ---------------------------------------------------------
// CFileSaver::~CFileSaver()
// ---------------------------------------------------------
//
CFileSaver::~CFileSaver()
    {
    CLOG(( ECodEng, 2, _L("-> CFileSaver::~CFileSaver") ));
    delete iDrmHandler;
    delete iStorage;
    CLOG(( ECodEng, 2, _L("<- CFileSaver::~CFileSaver") ));
    }

// ---------------------------------------------------------
// CFileSaver::OpenStoreL()
// ---------------------------------------------------------
//
void CFileSaver::OpenStoreL()
    {
    CLOG(( ECodEng, 2, _L("CFileSaver::OpenStoreL") ));
    __ASSERT_DEBUG( iState == EInit, CodPanic( ECodInternal ) );

    // Store (temp file) is not opened now.
    // Opening is deferred until first data chunk arrives.

    TInt err = iFile.Open(  iFs,
                            iFname,
                            EFileShareAny |
                            EFileStream |
#ifdef BRDO_RFILE_WRITE_DIRECT_IO_FF
                            EFileWrite |
                            EFileWriteDirectIO );
#else
                            EFileWrite );
#endif

	if(err == KErrNotFound)
	    {
        err = iFile.Replace( iFs,
                           iFname,
                           EFileShareAny |
                           EFileStream |
#ifdef BRDO_RFILE_WRITE_DIRECT_IO_FF
                                       EFileWrite |
                                       EFileWriteDirectIO );
#else
                           EFileWrite );
#endif

	    }

    if( !err )
        {
        TInt pos( 0 );
        iFile.Seek( ESeekEnd, pos );
        }
    iSize = 0;
    iState = EStoreOpen;
    }

// ---------------------------------------------------------
// CFileSaver::AppendData()
// ---------------------------------------------------------
//
TInt CFileSaver::AppendData( const TDesC8& aData )
    {
    CLOG(( ECodEng, 4, \
        _L("-> CFileSaver::AppendData (%d) bytes"), aData.Length() ));
//TODO:    __ASSERT_DEBUG( iState == EStoreOpen, CodPanic( ECodInternal ) );
    TInt err = CheckMaxSize( aData.Size() );
    if ( !err && !iFile.SubSessionHandle() )
        {
//TODO:        __ASSERT_DEBUG( !iFname.Length(), CodPanic( ECodInternal ) );
        (void)iFs.MkDirAll( iTempPath );

        /*if( !iPausable )
            {
            // nothing persisted yet. If there's a file with the same name,
            // delete it.
            err = iFile.Replace( iFs,
                                   iFname,
                                   EFileShareAny |
                                   EFileStream |
                                   EFileWrite );
            }
        else*/

        }
    if ( !err )
        {
        err = iStorage->WriteOutNextBodyDataL(aData );
        }

    CLOG(( EHttpLoad, 2, _L("<- CFileSaver::AppendData returns (%d)"), err ));
    return err;
    }

// ---------------------------------------------------------
// CFileSaver::CloseStore()
// ---------------------------------------------------------
//
void CFileSaver::CloseStore()
    {
    CLOG(( ECodEng, 2, _L("-> CFileSaver::CloseStore") ));
    FlushL();
    iFile.Close();
    iState = EStoreClosed;
    CLOG(( ECodEng, 2, _L("<- CFileSaver::CloseStore") ));
    }

// ---------------------------------------------------------
// CFileSaver::CheckResponseAttributesL()
// ---------------------------------------------------------
//
void CFileSaver::CheckResponseAttributesL( const CCodData& aData )
    {
    CLOG(( ECodEng, 2, _L("-> CFileSaver::CheckResponseAttributesL") ));
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
//TODO:    __ASSERT_DEBUG( iFname.Length(), CodPanic( ECodInternal ) );

    // Compare content MIME type against descriptor.

    // Size is not checked, no exact match is required. Quote form spec:
    // "The storage size and the execution size are dependent on the
    // environment and may be different from the value of the size attribute.
    // The transport size may also be different, if compression or some
    // packaging format is used."
    //
    // There is a safety upper bound on the transaction size, that is already
    // applied. See SetMaxSize().

    TDataType drm( KOma1DrmMessageContentType );
    TDataType dcf( KOma1DcfContentType );

    if ( iType == drm )
        {
        // DRM message arrived.
        // DRM filter should have already turned this into a DCF. Fail now.
        CLOG(( ECodEng, 4, _L("  DRM unexpected") ));
        User::Leave( KErrCodAttributeMismatch );
        }
    else if ( iType == dcf )
        {
        // DCF arrived. MIME type check may be limited:
        // - Received file (inside DRM) must be DRM-supported (CDrmHandler
        //   does this).
        // - COD file must specify COD, DRM, or the received MIME type.
        __ASSERT_DEBUG( !iDrmHandler, CodPanic( ECodInternal ) );
        iDrmHandler = CDrmHandler::NewL( iFname );
        TDataType typeInsideDcf( iDrmHandler->Type() );
#ifdef __TEST_COD_LOG
        mime.Set( typeInsideDcf.Des8() );
        CLOG(( ECodEng, 4, _L8("  inside DCF: MIME==<%S>"), &mime ));
#endif /* def __TEST_COD_LOG */
        if ( ! ((*aData[aData.ActiveDownload()]).HasType( typeInsideDcf.Des8() ) ||
                (*aData[aData.ActiveDownload()]).HasType( KOma1DrmMessageContentType ) ||
                (*aData[aData.ActiveDownload()]).HasType( KOma1DcfContentType ) )
           )
            {
            // MIME type mismatch, attribute mismatch.
            CLOG(( ECodEng, 4,  _L("  type mismatch") ));
            User::Leave( KErrCodAttributeMismatch );
            }
        }
    else
        {
        // Other than DRM stuff arrived. Proper check for MIME type.
        if( !(*aData[aData.ActiveDownload()]).HasType( iType.Des8() ) )
            {
            CLOG(( ECodEng, 4, _L("  mismatch") ));
            User::Leave( KErrCodAttributeMismatch );
            }
        }

    iState = ERespChecked;
    CLOG(( ECodEng, 2, _L("<- CFileSaver::CheckResponseAttributesL (match)") ));
    }


// ---------------------------------------------------------
// CFileSaver::BulkInstallL()
// ---------------------------------------------------------
//
void CFileSaver::BulkInstallL( TRequestStatus* aStatus, const CCodData &aData, const TBool aAttached )
    {
    CLOG(( ECodEng, 2, _L("-> CFileSaver::BulkInstallL") ));
    __ASSERT_DEBUG( iState == ERespChecked, CodPanic( ECodInternal ) );
    __ASSERT_DEBUG( aStatus, CodPanic( ECodInternal ) );

/* check if the PD app has been started (PD appl is responsible for file moving after play is done)
   In this case leave and go to the next state.
*/
    if (!aAttached)
	    {
#if 0
        RFs fs;
        TInt err( KErrNone );
        HBufC* filename = HBufC::NewLC(KMaxFileName);
        TPtr filenamePtr = filename->Des();
        filenamePtr = iRootPath;
        filenamePtr.Append(_L("download\\"));
        User::LeaveIfError( fs.Connect() );
        CleanupClosePushL(fs);
        CFileMan* file=CFileMan::NewL(fs);
        CleanupStack::PushL(file);
        TInt error = fs.MkDirAll(filenamePtr);
        if (error!=KErrNone && error!=KErrAlreadyExists)
           {
            User::Leave(error);
           }

        // Find a unique name to avoid any conflict.
        // Here iFname has full path of current location of file
        // and filename has destination path.
        FindUniqueDestinationFileNameL( iFname, filename );

        filenamePtr = filename->Des();

        err = file->Move(iFname, filenamePtr, CFileMan::EOverWrite);

        if(err != KErrNone)
           {
           User::Leave(err);
           }

        iFname = filenamePtr;
        NotifyMediaGalleryL( filenamePtr );
        CleanupStack::PopAndDestroy(file);
        CleanupStack::PopAndDestroy(&fs);
        CleanupStack::PopAndDestroy(filename);
#else
        RFs fs;
        User::LeaveIfError( fs.Connect() );
        CleanupClosePushL(fs);
        CFileMan* file=CFileMan::NewL(fs);
        CleanupStack::PushL(file);


        for( TInt i = 1; i <= aData.Count() ; ++i )
            {
            HBufC* filename = HBufC::NewLC(KMaxFileName);
            TPtr filenamePtr = filename->Des();
            filenamePtr = (*aData[i]).iRootPath;

            filenamePtr.Append(_L("download\\"));
            TInt error = fs.MkDirAll(filenamePtr);
            if (error!=KErrNone && error!=KErrAlreadyExists)
               {
                User::Leave(error);
               }
            iFname = (*aData[i]).iFullName->Des();

            // Find a unique name to avoid any conflict.
            // Here iFname has full path of current location of file
            // and filename has destination path.
            FindUniqueDestinationFileNameL( iFname, filename );

            filenamePtr = filename->Des();
            TInt err = file->Move(iFname, filenamePtr, CFileMan::EOverWrite);
            if(err != KErrNone)
            	{
            	User::LeaveIfError(err);
            	}
            iFname = filenamePtr;
            NotifyMediaGalleryL( filenamePtr );

            (*aData[i]).iFileName = iFname;
            (*aData[i]).iFullName = NameL();
            CleanupStack::PopAndDestroy(filename);
            }
        CleanupStack::PopAndDestroy(file);
        CleanupStack::PopAndDestroy(&fs);

#endif // RD_MULTIPLE_DRIVE
    // TODO set iType to that of the packaged object (??)
    CLOG(( ECodEng, 4, _L("CFileSaver::InstallL: saved==<%S> handler(0x%x)"), \
        &iFname, iHandler ));

	}

    // Normal file saving is not async.
    *aStatus = KRequestPending;
    User::RequestComplete( aStatus, KErrNone );

    iState = EInstalled;
    CLOG(( ECodEng, 2, _L("<- CFileSaver::BulkInstallL") ));
    }


// ---------------------------------------------------------
// CFileSaver::InstallL()
// ---------------------------------------------------------
//
void CFileSaver::InstallL( TRequestStatus* aStatus, const TDesC& /* aName */, const TBool aAttached )
    {
    CLOG(( ECodEng, 2, _L("-> CFileSaver::InstallL") ));
    __ASSERT_DEBUG( iState == ERespChecked, CodPanic( ECodInternal ) );
//    __ASSERT_DEBUG( iFname.Length(), CodPanic( ECodInternal ) );
    __ASSERT_DEBUG( aStatus, CodPanic( ECodInternal ) );

/* check if the PD app has been started (PD appl is responsible for file moving after play is done)
   In this case leave and go to the next state.
*/
    if (!aAttached)
	    {
#ifdef RD_MULTIPLE_DRIVE
        RFs fs;
        TInt err( KErrNone );
        HBufC* filename = HBufC::NewLC(KMaxFileName);
        TPtr filenamePtr = filename->Des();
        filenamePtr = iRootPath;
        filenamePtr.Append(_L("download\\"));
        User::LeaveIfError( fs.Connect() );
        CleanupClosePushL(fs);
        CFileMan* file=CFileMan::NewL(fs);
        CleanupStack::PushL(file);
        TInt error = fs.MkDirAll(filenamePtr);
        if (error!=KErrNone && error!=KErrAlreadyExists)
           {
            User::Leave(error);
           }

        // Find a unique name to avoid any conflict.
        // Here iFname has full path of current location of file
        // and filename has destination path.
        FindUniqueDestinationFileNameL( iFname, filename );

        filenamePtr = filename->Des();

        err = file->Move(iFname, filenamePtr, CFileMan::EOverWrite);

        if(err != KErrNone)
           {
           User::Leave(err);
           }

        iFname = filenamePtr;
        NotifyMediaGalleryL( filenamePtr );
        CleanupStack::PopAndDestroy(file);
        CleanupStack::PopAndDestroy(&fs);
        CleanupStack::PopAndDestroy(filename);
#else
            RFs fs;
            HBufC* filename = HBufC::NewLC(KMaxFileName);
            TPtr filenamePtr = filename->Des();
            filenamePtr = iRootPath;

            filenamePtr.Append(_L("download\\"));
            User::LeaveIfError( fs.Connect() );
            CleanupClosePushL(fs);
            CFileMan* file=CFileMan::NewL(fs);
            CleanupStack::PushL(file);
            TInt error = fs.MkDirAll(filenamePtr);
            if (error!=KErrNone && error!=KErrAlreadyExists)
               {
                User::Leave(error);
               }

            // Find a unique name to avoid any conflict.
            // Here iFname has full path of current location of file
            // and filename has destination path.
            FindUniqueDestinationFileNameL( iFname, filename );

            filenamePtr = filename->Des();
            TInt err = file->Move(iFname, filenamePtr, CFileMan::EOverWrite);

            if(err != KErrNone)
               {
               User::Leave(err);
                }
            iFname = filenamePtr;
            NotifyMediaGalleryL( filenamePtr );
            CleanupStack::PopAndDestroy(file);
            CleanupStack::PopAndDestroy(&fs);
            CleanupStack::PopAndDestroy(filename);
#endif // RD_MULTIPLE_DRIVE
    // TODO set iType to that of the packaged object (??)
    CLOG(( ECodEng, 4, _L("CFileSaver::InstallL: saved==<%S> handler(0x%x)"), \
        &iFname, iHandler ));

	}

    // Normal file saving is not async.
    *aStatus = KRequestPending;
    User::RequestComplete( aStatus, KErrNone );

    iState = EInstalled;
    CLOG(( ECodEng, 2, _L("<- CFileSaver::InstallL") ));
    }

// ---------------------------------------------------------
// CFileSaver::CancelInstall()
// ---------------------------------------------------------
//
void CFileSaver::CancelInstall()
    {
    CLOG(( ECodEng, 2, _L("CFileSaver::CancelInstall") ));
    // Do nothing, saving is completed already (not async).
    }

// ---------------------------------------------------------
// CFileSaver::ReleaseContent()
// ---------------------------------------------------------
//
void CFileSaver::ReleaseContent( TFileName& aFname, TUid& aHandler )
    {
    CLOG(( ECodEng, 2, _L("-> CFileSaver::ReleaseContent") ));
    //__ASSERT_DEBUG( iState == EInstalled, CodPanic( ECodInternal ) );
    aFname = iFname;
    aHandler = iHandler;
    iFname = KNullDesC;
    iHandler = KNullUid;
    iSize = 0;
    delete iDrmHandler;
    iDrmHandler = NULL;
    iState = EInit;  //for PD iState = StartFeatch
    CLOG(( ECodEng, 2, _L("<- CFileSaver::ReleaseContent") ));
    }

// ---------------------------------------------------------
// CFileSaver::ReleaseFileName()
// ---------------------------------------------------------
//
void CFileSaver::ReleaseFileName( TFileName& aFname)
{
    CLOG(( ECodEng, 2, _L("-> CFileSaver::ReleaseFileName") ));
    aFname = iFname;
}

// ---------------------------------------------------------
// CFileSaver::Cleanup()
// ---------------------------------------------------------
//
void CFileSaver::Cleanup( TBool aDeleteFile )
    {
    CLOG(( ECodEng, 2, _L("-> CFileSaver::Cleanup") ));
    // Clean up the temporary file, ignore errors.
    // Expected error: KErrNotFound (if there is no temp file).
    // Unexpected error: all the rest -> nothing we can do with them.
    CloseStore();

	if( aDeleteFile )
	    {
#ifdef __TEST_COD_LOG
        TInt err =  // ('Log-only' variable.)
#endif /* def __TEST_COD_LOG */
        iFs.Delete( iFname );
        CLOG(( ECodEng, 4, \
        _L("CFileSaver::Cleanup <%S> err(%d)"), &iFname, err ));
	    }

    iFname = KNullDesC; // Now we are done with the file.
    iHandler = KNullUid;
    iSize = 0;
    if ( iDrmHandler )
        {
        iDrmHandler->Cleanup();
        delete iDrmHandler;
        iDrmHandler = NULL;
        }
    iState = EInit;
    CLOG(( ECodEng, 2, _L("<- CFileSaver::Cleanup") ));
    }


// ---------------------------------------------------------
// CFileSaver::UpdateDCFRepositoryL()
// Update saved file to DCFRepository
// ---------------------------------------------------------
//
void CFileSaver::UpdateDCFRepositoryL( const TDesC& aFileName )
    {
    CDcfEntry* dcfEntry = NULL;
    dcfEntry = CDcfEntry::NewL();
    CleanupStack::PushL( dcfEntry );

    CDcfRep* dcfRep = NULL;
    dcfRep = CDcfRep::NewL();
    CleanupStack::PushL( dcfRep );

    dcfEntry->SetLocationL( aFileName, 0 );
    dcfRep->UpdateL( dcfEntry );
    CleanupStack::PopAndDestroy(2); // dcfEntry, dcfRep
    }

// ---------------------------------------------------------
// CFileSaver::UpdateMediaGalleryIfNeededL()
// Calls MediaGallery Update method so that media gallery
// knows update its view.
// ---------------------------------------------------------
//


void CFileSaver::UpdateMediaGalleryIfNeededL( const TDesC& aFileName )
    {
#ifdef BRDO_APP_GALLERY_SUPPORTED_FF
    CMGXFileManager* fm = MGXFileManagerFactory::NewFileManagerL( iFs );
    CleanupStack::PushL(fm);
    fm->UpdateL(aFileName);
    CleanupStack::PopAndDestroy(); // fm
#endif
    }
// ---------------------------------------------------------
// CFileSaver::NotifyMediaGalleryL()
// Notify media gallery about the new file.
// ---------------------------------------------------------
//
void CFileSaver::NotifyMediaGalleryL( const TDesC& aFileName )
    {
#ifdef BRDO_APP_GALLERY_SUPPORTED_FF

    CMGXFileManager* mgFileManager = MGXFileManagerFactory::NewFileManagerL( iFs );
    CleanupStack::PushL( mgFileManager );

    TRAP_IGNORE( mgFileManager->UpdateL() );
    CleanupStack::PopAndDestroy( mgFileManager );

    // Notify Media Gallery
    TRAP_IGNORE( UpdateMediaGalleryIfNeededL( aFileName ) );

#endif


    // Notify DCF repository
    TRAP_IGNORE( UpdateDCFRepositoryL( aFileName ) );
    }
// ---------------------------------------------------------
// CFileSaver::DownloadedFileSize()
// ---------------------------------------------------------
//
TInt CFileSaver::DownloadedFileSize()
    {
    // check how many bytes are already persisted
    TInt DownloadedSize(0);
    TInt err = iFile.Size( (TInt&)DownloadedSize );

    CLOG(( ECodStorage, 2, _L("CFileSaver::DownloadedFileSize  Downloaded size = %d  error = %d"), \
               DownloadedSize,err ));

    if(err != KErrNone)
        return err;
    return DownloadedSize;
    }


/**
* Discard the old contents
*/
void CFileSaver::ResetL()
    {


    iFile.Close();

    User::LeaveIfError( iFile.Replace(  iFs,
                                        iFname,
                                        EFileShareAny |
                                        EFileStream |
                                        EFileWrite ) );
    }


// -----------------------------------------------------------------------------
// CHttpStorage::SetProgressiveMode
// -----------------------------------------------------------------------------
//
void CFileSaver::SetProgressiveMode( TBool aValue )
    {
    iProgressiveDownload = aValue;
    if( iProgressiveDownload )
        {
        iBufferSize = KDefaultStorageBufferSizePD;
        }


    if( iBufferSize != iStorage->CurrentBufferSize())
        {
        FlushL();
        }

    if ( iFile.SubSessionHandle() )
        {

        if( iProgressiveDownload )
            {
            if( iLength != KDefaultContentLength )
                {
                iFile.SetSize( iLength );
                }
            }
        else
            {
            iFile.SetSize( iSize );
            }
        }
    }


// -----------------------------------------------------------------------------
// CHttpStorage::Flush
// Flush internal buffer to disk
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CFileSaver::FlushL()
    {
    CLOG(( ECodEng, 2, _L("-> CFileSaver::FlushL") ));
    iStorage->FlushBuffersL();
    iStorage->ResetBuffers();
    CLOG(( ECodEng, 2, _L("<- CFileSaver::FlushL") ));
    }



// -----------------------------------------------------------------------------
// CFileSaver::OnCompleted
// Called if download completed.
// -----------------------------------------------------------------------------
//
void CFileSaver::OnComplete()
    {
    CLOG(( ECodEng, 2, _L("-> CFileSaver::OnComplete") ));
    FlushL();
    iStorage->ClearErrors();
    iFile.Close();
    CLOG(( ECodEng, 2, _L("<- CFileSaver::OnComplete") ));
    }




// ---------------------------------------------------------
// CFileSaver::Name()
// ---------------------------------------------------------
HBufC* CFileSaver::NameL() const
    {
    return iFname.AllocL();
    }
