/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implements storage functionality in the DownloadManager
*
*/



// INCLUDE FILES
#include <bldvariant.hrh>
#include <Browser_platform_variant.hrh>

#include "FileExt.h"
#include "HttpClientApp.h"
#include "HttpDownload.h"
#include "HttpStorage.h"
#include "HttpDownloadManagerServerEngine.h"
#include "HttpDownloadMgrLogger.h"
#include "HeaderField.h"
#include "BuffStorage.h"

#include <sysutil.h>
#include <DocumentHandler.h>
#include <apmstd.h>

#ifdef __SYNCML_DM_FOTA
#include <fotaengine.h>
#endif

#ifdef RD_MULTIPLE_DRIVE
#include <driveinfo.h>
#endif //RD_MULTIPLE_DRIVE

// EXTERNAL DATA STRUCTURES
//extern  ?external_data;

// EXTERNAL FUNCTION PROTOTYPES  
//extern ?external_function( ?arg_type,?arg_type );

// CONSTANTS
//const ?type ?constant_var = ?constant;
const TInt KDefaultStorageBufferSize = 128 * 1024;
const TInt KFileNameExtensionMaxSize = 5; // practical maximum length of filename extension for renamed downloads
const TInt KDefaultStorageBufferSizePD = 16 * 1024;
// MACROS
//#define ?macro ?macro_def

// LOCAL CONSTANTS AND MACROS
//const ?type ?constant_var = ?constant;
//#define ?macro_name ?macro_def

// MODULE DATA STRUCTURES
//enum ?declaration
//typedef ?declaration

// LOCAL FUNCTION PROTOTYPES
//?type ?function_name( ?arg_type, ?arg_type );

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CHttpStorage::CHttpStorage
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CHttpStorage::CHttpStorage( CHttpDownload* aDownload )
    :iDownload( aDownload )
    ,iProgressiveDownload( EFalse )
    ,iLength( KDefaultContentLength )
    ,iStorageMethod( EStoreFile )
    ,iBufferingEnabled( EFalse )
    ,iBufferSize( KDefaultStorageBufferSize )
    ,iPartialLength( KDefaultContentLength )
    {
    CLOG_WRITE_1( "Storage created: 0x%x", this );
    }

// -----------------------------------------------------------------------------
// CHttpStorage::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CHttpStorage::ConstructL()
    {
    iStorage = CBuffStorage::NewL(this);
    }

// -----------------------------------------------------------------------------
// CHttpStorage::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CHttpStorage* CHttpStorage::NewL( CHttpDownload* aDownload )
    {
    CHttpStorage* self = new( ELeave ) CHttpStorage( aDownload );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

    
// Destructor
CHttpStorage::~CHttpStorage()
    {
    delete iLocalFilename;
    delete iDestFilename;
    delete iDdFilename;
    
    
	if(iStorage)
		{
		delete iStorage;
		iStorage = NULL;	
		}
    if( iFile )
        {
        iFile->Close();
        delete iFile;
        }
        
#ifdef __SYNCML_DM_FOTA        
    if( iFotaEngine )
        {
        iFotaEngine->UpdatePackageDownloadComplete( iDownload->iFotaPckgId);
        iFotaStream = NULL;
        iFotaEngine->Close();
        delete iFotaEngine; iFotaEngine = NULL;
        }
#endif // __SYNCML_DM_FOTA

    CLOG_WRITE_1( "Storage destroy: (0x%x)", this );
    }

// -----------------------------------------------------------------------------
// CHttpStorage::SetStorageMethod
// 
// iDownloadedSize comes from the destination file size.
// -----------------------------------------------------------------------------
//
void CHttpStorage::SetStorageMethod( TStorageMethod aMethod )
    {
    iStorageMethod = aMethod;
    }

// -----------------------------------------------------------------------------
// CHttpStorage::CheckContentFileIntegrityL
// 
// iDownloadedSize comes from the destination file size.
// -----------------------------------------------------------------------------
//
TBool CHttpStorage::CheckContentFileIntegrityL()
    {
    LOGGER_ENTERFN( "CheckContentFileIntegrityL" );
    TBool retVal = ETrue;

    if( iDestFilename && iDestFilename->Length() )
        {
        if( iDriveId == GetDestinationDriveId() )
            // this is not the same drive as where content file was
            // previously saved
            {
            TEntry entry;
            RFs& fs = iDownload->ClientApp()->Engine()->Fs();

            if( fs.Entry( *iDestFilename, entry ) == KErrNone )
                {
                //Check if PD was on and if the file was being downloaded to MMC when last time we exited the session
                //Use the Downloaded size as stored in info file 
                //because when we exited from prev session
                //size was not set back to downloaded size from total size
                //
                
                if( !iProgressiveDownload && !iDownload->iNoMedia)
                    {
                    iDownloadedSize = entry.iSize;
                    } 
                else 
                    {
                    iFile = new (ELeave) RFile;
                    TInt err = iFile->Open( fs , 
                                          *iLocalFilename, 
                                          EFileShareAny | 
                                          EFileStream | 
                                           EFileWrite );
                    if( !err )
                        {
                        iFile->SetSize( iDownloadedSize );
                        iProgressiveDownload = EFalse; 
                        iFile->Close();
                        delete iFile; iFile = NULL;       
                        }
                        iDownload->iNoMedia = EFalse;
                    }                    

                CLOG_WRITE_2( "Length: %d, size: %d", iLength, iDownloadedSize );

                // if full size is unknown, we can't decide
                // if the content file integrity is ok, or wrong.
                // Let's assume that it is ok.
                if( iLength != KDefaultContentLength )
                    // Full size is known
                    {
                    if( !((iDownload->iDlState == EHttpDlMultipleMOCompleted &&
                        (iDownload->iProgState == EHttpProgContentFileMoved || 
                        iDownload->iProgState == EHttpProgContentFileMovedAndDestFNChanged ||
                        iLength == iDownloadedSize)) ||
                        (iDownload->iDlState != EHttpDlMultipleMOCompleted &&
                        iLength > iDownloadedSize)) )
                        {
                        CLOG_WRITE8( "Wrong file size" );
                        retVal = EFalse;
                        }
                    }
                }
            else
                // Entry failed but we can still assume that there's no problem
                {
                CLOG_WRITE8( "Unknown size" );
                }
            }
        else
            {
            CLOG_WRITE8_1( "Different drive: %d", iDriveId );
            // MMC removed.
            iDownload->MediaRemoved( (TUint)KErrNotFound, ETrue );
            }
        }
    else
        {
        CLOG_WRITE( "No dest filename");
         iDownload->iNoMedia = EFalse; 
        }

    return retVal;
    }

// -----------------------------------------------------------------------------
// CHttpStorage::CreateDestinationFileL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CHttpStorage::CreateDestinationFileL()
    {
    LOGGER_ENTERFN( "CreateDestinationFileL" );

    TInt err( KErrNone );
    
    if( iStorageMethod == EStoreFota )
        {
#ifdef __SYNCML_DM_FOTA        
        iFotaEngine = new (ELeave) RFotaEngineSession;
        iFotaEngine->OpenL();
        User::LeaveIfError( iFotaEngine->OpenUpdatePackageStore( iDownload->iFotaPckgId, 
                                                                 iFotaStream ) );
        
        CLOG_WRITE( "FOTA -> no file" );
#else   // __SYNCML_DM_FOTA
        DMPanic( KErrNotSupported );
#endif  // __SYNCML_DM_FOTA   
        }
    else
        {
        if( iRFileSetByClient )
            {
            CLOG_WRITE( "RFile is adopted from client" );
            
            TBuf<KMaxFileName> tempFileName; 
            iFile->FullName(tempFileName);           
            ReallocateStringL( iLocalFilename, tempFileName, KMaxPath );

            TBuf8<KMaxContentTypeLength> contentType;
            contentType.Copy( *iDownload->iDDType );

            if( (0 == contentType.Compare( KCodMimeType() )) ||
            	(0 == contentType.Compare( KDdMimeType() ))	 ||
            	(0 == contentType.Compare( KDd2MimeType() )) ||
            	(0 == contentType.Compare( KMultiPartMimeType() )) )
            	{
            	iDdFilename = iLocalFilename->Des().AllocL();
            	}            
            
            
            // check how many bytes are already persisted
            iFile->Size( (TInt&)iDownloadedSize );

            CLOG_WRITE_1( "iDownloadedSize: %d", iDownloadedSize );

            return;
            }

        CLOG_WRITE8_1( "iDownloadedSize: %d", iDownloadedSize );
            
        __ASSERT_DEBUG( !iFile, DMPanic( KErrInUse) );

        CreateDestinationFilenameL();

        iFile = new (ELeave) RFile;

        for( TInt i = 0; i < 2; ++i )
            {
            if( !iDownloadedSize )
                // nothing persisted yet. If there's a file with the same name,
                // delete it.
                {
                err = iFile->Replace( iDownload->ClientApp()->Engine()->Fs(), 
                                       *iLocalFilename, 
                                       EFileShareAny | 
                                       EFileStream | 
#ifdef BRDO_RFILE_WRITE_DIRECT_IO_FF
                                       EFileWrite |
                                       EFileWriteDirectIO );
#else
                                       EFileWrite );
#endif
                }
            else
                {
                err = iFile->Open( iDownload->ClientApp()->Engine()->Fs(), 
                                   *iLocalFilename, 
                                   EFileShareAny | 
                                   EFileStream | 
#ifdef BRDO_RFILE_WRITE_DIRECT_IO_FF
                                   EFileWrite |
                                   EFileWriteDirectIO );
#else
                                   EFileWrite );
#endif

                if( !err )
                    {
                    // append new chunks to the file
                    TInt pos( 0 );
                    iFile->Seek( ESeekEnd, pos );
                    }
                }

            if( err == KErrPathNotFound )
                {
                TPath folder;

                folder.Copy( *iLocalFilename );

                TInt slash = folder.LocateReverse( '\\' );
                if( slash != KErrNotFound )
                    {
                    folder.SetLength( slash+1 );
                    iDownload->ClientApp()->Engine()->Fs().MkDirAll( folder );
                    }
                }
            else 
                {
                break;
                }
            }
        }
    CLOG_WRITE8_1( "CreateDestinationFileL: %d", err );

    if( !err )
        {
        if( iFile && iLength != KDefaultContentLength && iProgressiveDownload )
            {
            iFile->SetSize( iLength );
            }
        }
    else
        {
        iDownload->OnError( err, EDestFileInUse );
        User::Leave( err );
        }
    }

// -----------------------------------------------------------------------------
// CHttpStorage::CloseDestinationFile
// -----------------------------------------------------------------------------
//
void CHttpStorage::CloseDestinationFile( TFileCloseOperation aOperation )
    {
    LOGGER_ENTERFN( "CloseDestinationFile" );

    CLOG_WRITE_2( "iFile: %x, iRFileSetByClient: %d", iFile, iRFileSetByClient );
    if( iFile )
        // RFile received from client is not close only in
        // destructor, because there's no way to reopen it.
        {
        CLOG_WRITE( "HttpStorage: Writing final data and deallocating buffer" );
        
        // Make sure everything is written on the file
        TRAP_IGNORE(iStorage->FlushBuffersL()); // Might be good to somehow relay error somewhere?
       
        if( iProgressiveDownload && iDownloadedSize >= 0 )
                {
                iFile->SetSize( iDownloadedSize );
                }

        
        // Free up memory
        iStorage->ResetBuffers();
        iStorage->ClearErrors();

        if( iDownload->iDlState == EHttpDlMultipleMOCompleted || !iRFileSetByClient )
            // do not close file of a progressive download,
            // only on complete.
            {
            iFile->Close();
            delete iFile; iFile = NULL;
            }
        }

#ifdef __SYNCML_DM_FOTA        
    if( iFotaEngine )
        {
        iFotaEngine->UpdatePackageDownloadComplete( iDownload->iFotaPckgId);
        iFotaStream = NULL;
        iFotaEngine->Close();
        delete iFotaEngine; iFotaEngine = NULL;
        }
#endif  // __SYNCML_DM_FOTA

    if( iLocalFilename )
        {
        if( aOperation == EDeleteFile )
            {
            CLOG_WRITE_1( "Delete: %S", iLocalFilename );
            iDownload->ClientApp()->Engine()->Fs().Delete( *iLocalFilename );
            iDownloadedSize = 0;
            }
        else if( aOperation == EReplaceFile )
            {
            CLOG_WRITE_1( "Replace: %S", iLocalFilename );
            RFile file;
            file.Replace( iDownload->ClientApp()->Engine()->Fs(), *iLocalFilename, EFileWrite );
            file.Close();
            iDownloadedSize = 0;
            }
        }
        
    if( iDestFilename )
        {
        if( aOperation == EDeleteFile )
            {
            CLOG_WRITE_1( "Delete: %S", iDestFilename );
            iDownload->ClientApp()->Engine()->Fs().Delete( *iDestFilename );
            }
        else if( aOperation == EReplaceFile )
            {
            CLOG_WRITE_1( "Replace: %S", iDestFilename );
            RFile file;
            file.Replace( iDownload->ClientApp()->Engine()->Fs(), *iDestFilename, EFileWrite );
            file.Close();  
            }
        }
    }

// -----------------------------------------------------------------------------
// CHttpStorage::WriteOutNextBodyDataL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CHttpStorage::WriteOutNextBodyDataL( const TDesC8& aBuf )
    {

    return iStorage->WriteOutNextBodyDataL(aBuf);

    }

// -----------------------------------------------------------------------------
// CHttpStorage::OnComplete
// -----------------------------------------------------------------------------
//
void CHttpStorage::OnComplete()
    {
    LOGGER_ENTERFN( "CHttpStorage::OnComplete()" );

    // If destination filename is not set by client app
    // here we have to update it with the local filename
    if( !iDestFNameSet && 
        !iRFileSetByClient && 
        iStorageMethod != EStoreFota )
        {
        TRAP_IGNORE( 
            ReallocateStringL( iDestFilename, *iLocalFilename, KMaxPath ) );
        }

    CloseDestinationFile( CHttpStorage::EKeepFile );
    }

// -----------------------------------------------------------------------------
// CHttpStorage::ResetAndDestroy
// -----------------------------------------------------------------------------
//
void CHttpStorage::ResetAndDestroy()
    {
    LOGGER_ENTERFN( "CHttpStorage::ResetAndDestroy()" );

    if( iFile && iRFileSetByClient )
        // do not delete or close this file, only set its size to 0.
        {
        iFile->SetSize( 0 );
        }

#ifdef __SYNCML_DM_FOTA
    if( iFotaEngine )
        {
        iFotaEngine->UpdatePackageDownloadComplete( iDownload->iFotaPckgId);
        iFotaStream = NULL;
        iFotaEngine->Close();
        delete iFotaEngine; iFotaEngine = NULL;
        }
#endif  // __SYNCML_DM_FOTA        

    CloseDestinationFile( EReplaceFile );

    iLength = KDefaultContentLength;
    iDownloadedSize = 0;
    iMoDownloadedSize = 0;

    iDriveId = (TUint)KErrNotFound;

    if( iLocalFilename )
        {
        iLocalFilename->Des().SetLength( 0 );
        }

    HBufC* destName = iDestFilename;
    if( destName && destName->Length() && !iDestFNameSet )
        {
        destName->Des().SetLength( 0 );
        }
    }

// -----------------------------------------------------------------------------
// CHttpStorage::AdoptFileHandleL
// -----------------------------------------------------------------------------
//
void CHttpStorage::AdoptFileHandleL( RFile* aFile )
    {
    LOGGER_ENTERFN( "AdoptFileHandleL" );
    
    __ASSERT_DEBUG( !iRFileSetByClient, DMPanic( KErrInUse ) );
    __ASSERT_DEBUG( !iFile, DMPanic( KErrInUse ) );
#ifdef __SYNCML_DM_FOTA
    if( iRFileSetByClient || iFile || iFotaEngine )
#else
    if( iRFileSetByClient || iFile )
#endif
        {
        CLOG_WRITE_2( "iRFileSetByClient: %d, iFile: %d", iRFileSetByClient, iFile );
        User::Leave( KErrInUse );
        }

    // forget any previously set filename
    delete iDestFilename; iDestFilename = NULL;
    delete iLocalFilename; iLocalFilename = NULL;
    iDestFNameSet = EFalse;
    
    iFile = aFile;
    
    // check how many bytes are already persisted
    iFile->Size( (TInt&)iDownloadedSize );

    CLOG_WRITE_1( "File size: %d", iDownloadedSize );
    
    iRFileSetByClient = ETrue;
    }

// -----------------------------------------------------------------------------
// CHttpStorage::SetProgressiveMode
// -----------------------------------------------------------------------------
//
void CHttpStorage::SetProgressiveMode( TBool aValue )
    {
    iProgressiveDownload = aValue;
    if( iProgressiveDownload )
        {
        iBufferSize = KDefaultStorageBufferSizePD;
        }
    EnableBufferingL();    
      
    if( iFile )
        {
        if( iProgressiveDownload )
            {
            if( iLength != KDefaultContentLength )    
                {
                iFile->SetSize( iLength );
                }
            }
        else
            {
            iFile->SetSize( iDownloadedSize );
            }
        }
    }

// -----------------------------------------------------------------------------
// CHttpStorage::CheckFreeDiskSpaceL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CHttpStorage::CheckFreeDiskSpaceL()
    {
    LOGGER_ENTERFN( "CheckFreeDiskSpaceL" );
    TInt err( KErrNone );
    CHttpDownloadManagerServerEngine *DMSrvEngine = iDownload->ClientApp()->Engine();
    RFs& fs = DMSrvEngine->Fs();
    TInt bytesToWrite(0);
    
    if( iLength == KDefaultContentLength || iLength == 0 )
        // we don't know the size of the content -> no way to check that it can
        // fit into memory
        {
#ifdef RD_MULTIPLE_DRIVE
        TVolumeInfo volInfoCurrent;
        TInt driveSpaceMax;
        TInt driveSpaceCurrent;
        TInt64 freeSpaceCurrent( 0 );
        TInt64 freeSpaceMax( 0 );
        TInt errDrvStat;

        // initializing Drive variables to Default System drive/memory
        User::LeaveIfError(
            DriveInfo::GetDefaultDrive( DriveInfo::EDefaultPhoneMemory, driveSpaceMax ) );
        driveSpaceCurrent = driveSpaceMax;

        HBufC8* drivesDynList = iDownload->ClientApp()->Engine()->QueryDynDriveListLC();
        TPtrC8 drives( *drivesDynList );

        // drive letters are separated by semicolons
        for( TInt i = 0; i < drives.Length(); i = i + 2 )
            {
            if( (err = fs.CharToDrive( drives[i], driveSpaceCurrent )) == KErrNone )
                {
                TUint status;
                CLOG_WRITE_1( " Current Drive: %d", driveSpaceCurrent );
                errDrvStat = DriveInfo::GetDriveStatus( fs, driveSpaceCurrent, status );
                if ( errDrvStat == KErrNone )
                    {
                    if ( ( status & ( DriveInfo::EDriveUserVisible | DriveInfo::EDrivePresent ) ) )
                        {
                        CLOG_WRITE_1( "Drive %d is present and visible", driveSpaceCurrent);
                        TInt errVol = fs.Volume( volInfoCurrent, driveSpaceCurrent );
                        if (( errVol != KErrNone ) || ( volInfoCurrent.iDrive.iType <= EMediaUnknown)) 
                            {
                            CLOG_WRITE_1( " errVol: %d", errVol );
                            CLOG_WRITE_1( " Current Drive driveSpaceCurrent: %d", driveSpaceCurrent );
                            CLOG_WRITE_1( " volInfoCurrent.iDrive.iType: %d", volInfoCurrent.iDrive.iType );
                            continue;
                            }
                        else
                            {
                            CLOG_WRITE_1( " errVol: %d", errVol );
                            CLOG_WRITE_1( " Current Drive driveSpaceCurrent: %d", driveSpaceCurrent );
                            CLOG_WRITE_1( " volInfoCurrent.iDrive.iType: %d", volInfoCurrent.iDrive.iType );
                            CLOG_WRITE_1( "        volInfoCurrent.iFree: %Ld", volInfoCurrent.iFree );
                            freeSpaceCurrent = volInfoCurrent.iFree;
                            if ( freeSpaceCurrent > freeSpaceMax ) 
                                {
                                freeSpaceMax = freeSpaceCurrent;
                                driveSpaceMax = driveSpaceCurrent;
                                }
                            }
                        }
                    }
                else
                    {
                    CLOG_WRITE_1( "GetDriveStatus return error code: %d", errDrvStat);
                    CLOG_WRITE_1( "Current Drive: %d", driveSpaceCurrent );
                    }
                }
            else
                {
                CLOG_WRITE_1( "RFS::CharToDrive failed with %d", err );
                CLOG_WRITE8_1( "Bad drive letter [%c]", drives[i] );
                }            
            }
        CleanupStack::PopAndDestroy( drivesDynList );
        CLOG_WRITE_2( "Saving content to %d Drive with %d B free space", driveSpaceMax, freeSpaceMax );
        
        TDriveInfo driveInfo;
    
    	if( !iDownload->ClientApp()->Engine()->Fs().Drive( driveInfo, driveSpaceMax) )
        	{
        	iRemovableDest = (driveInfo.iDriveAtt & KDriveAttRemovable);
        	CLOG_WRITE_1( "Removable: [%d]", iRemovableDest );
        	CLOG_WRITE_1( "driveInfo.iDriveAtt: [%d]", driveInfo.iDriveAtt );
        	}
    	else
        	{
        	CLOG_WRITE("DriveInfo failed");
        	}
        	
        return driveSpaceMax;
#else
        // If no MMC is inserted, this will leave (and not set mmcOk).
        bytesToWrite = iLength;
        if (bytesToWrite < 0)
            bytesToWrite = 0;
        
        TBool mmcOk = EFalse;
        TRAP_IGNORE( mmcOk = !SysUtil::MMCSpaceBelowCriticalLevelL
        			       ( &fs, bytesToWrite ); )
        if(!mmcOk)
        	{
        	CLOG_WRITE( "no MMC present" );
        	return EDriveC;
        	}
        CLOG_WRITE( "MMC is present " );        
        TVolumeInfo volInfoC;
        TVolumeInfo volInfoE;
        fs.Volume(volInfoC,EDriveC);
        fs.Volume(volInfoE,EDriveE);
        TInt64 freeC = volInfoC.iFree;//free memory available in that drive
        TInt64 freeE = volInfoE.iFree;
        return  freeC>=freeE?EDriveC:EDriveE;//put the file in which ever drive has more memory
#endif
        }

    // Destination is FFS in default
#ifdef RD_MULTIPLE_DRIVE
    TInt drive;
    User::LeaveIfError(
        DriveInfo::GetDefaultDrive( DriveInfo::EDefaultPhoneMemory, drive ) );
#else
    TInt drive( EDriveC );
#endif

	TBool isSpace( EFalse );
	
	//Get size of currently ongoing downloads
	TInt currentDownloadsLen (0);
	
    if( iDestFilename && iDestFilename->Length() )
        // client app gave the destination filename -> only that drive needs
        // to be checked
        {
        CLOG_WRITE8( "cfds1" );
        if( (err = fs.CharToDrive( (*iDestFilename)[0], drive )) != KErrNone )
            {
            CLOG_WRITE8( "Bad drive letter" );
            iDownload->OnError( KErrUnknown, EWrongDestFilename );
            User::Leave( err );
            }
        else if( !fs.IsValidDrive( drive ) )
            // invalid drive letter in filename
            {
            CLOG_WRITE8( "Invalid drive" );
            iDownload->OnError( KErrUnknown, EWrongDestFilename );
            User::Leave( KErrBadName );
            }
        if( iPartialLength ==  KDefaultContentLength )
            //This is the case when web Server returns Partial Content Length 
            //Hence Disk Critical level should checked against partial Content Length
            {
            iPartialLength = iLength;
            }
            
        currentDownloadsLen = DMSrvEngine->AllDownloadsSizeInDriveL(iDownload, drive);

        // Check if there's enough memory in the phone
        bytesToWrite = iPartialLength + currentDownloadsLen;
        if (bytesToWrite < 0)
            bytesToWrite = 0;
        
	    TRAP( err, isSpace = !SysUtil::DiskSpaceBelowCriticalLevelL(
                                                &iDownload->ClientApp()->Engine()->Fs(),
			                                    bytesToWrite,
                                                drive ));
        }
    else
        {
#ifdef RD_MULTIPLE_DRIVE
        HBufC8* drivesDynList = iDownload->ClientApp()->Engine()->QueryDynDriveListLC();
        TPtrC8 drives( *drivesDynList );
#else
        TPtrC drives( iDownload->ClientApp()->Engine()->iDriveLettersCenRep );
#endif
        
        // drive letters are separated by semicolons
        for( TInt i = 0; i < drives.Length() && (err || !isSpace); i = i + 2 )
            {
            if( (err = fs.CharToDrive( drives[i], drive )) == KErrNone )
                {
                currentDownloadsLen = DMSrvEngine->AllDownloadsSizeInDriveL(iDownload, drive);

                // Check if there's enough memory in the phone
                bytesToWrite = iLength + currentDownloadsLen;
                if (bytesToWrite < 0)
                    bytesToWrite = 0;
                
        	    TRAP( err, isSpace = !SysUtil::DiskSpaceBelowCriticalLevelL(
                                                        &fs,
        			                                    bytesToWrite,
                                                        drive ));
                }
            else
                {
                CLOG_WRITE8_1( "Bad drive letter [%c]", drives[i] );
                }            
            }
#ifdef RD_MULTIPLE_DRIVE
        CleanupStack::PopAndDestroy( drivesDynList );
#endif
        }	

    TDriveInfo driveInfo;
    
    if( !iDownload->ClientApp()->Engine()->Fs().Drive( driveInfo, drive) )
        {
        iRemovableDest = (driveInfo.iDriveAtt & KDriveAttRemovable);
        CLOG_WRITE_1( "Removable: [%d]", iRemovableDest );
        }
    else
        {
        CLOG_WRITE("DriveInfo failed");
        }

    if( err || !isSpace )
        {
        CLOG_WRITE8( "OOD1" );
        iDownload->OnError( KErrDiskFull, EDiskFull );
        User::Leave( KErrDiskFull );
        }

    return drive;
    }

// -----------------------------------------------------------------------------
// CHttpStorage::CreateDestinationFilenameL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CHttpStorage::CreateDestinationFilenameL()
    {
    LOGGER_ENTERFN( "CreateDestinationFilenameL" );

    TInt drive = CheckFreeDiskSpaceL();

    if( iLocalFilename && iLocalFilename->Length() )
        // Destination filename is already created, or set
        // by client app -> nothing to do
        {
        CLOG_WRITE_1( "local: %S", iLocalFilename );
        return;
        }

    if( iDestFNameSet )
        // Use destination filename client app set
        {
        ReallocateStringL( iLocalFilename, *iDestFilename, KMaxPath );
        }
    else
        {
        TPath folder;

        iDownload->ClientApp()->Engine()->DownloadContentFolder( iDownload->ClientApp(),
                                                                 folder );

        delete iLocalFilename; iLocalFilename = NULL;

        TChar driveChar;

        // this is surely works because we got the driver letter
        // from the FS earlier
        iDownload->ClientApp()->Engine()->Fs().DriveToChar( drive, driveChar );
        // allocate filename buffer
        TInt pathLen = folder.Length();
        TInt dlNameLen = MIN( KDownloadNameMaxSize, iDownload->iDlName->Length() );
        iLocalFilename = HBufC::NewL( pathLen + dlNameLen + KFileNameExtensionMaxSize );
        TPtr locFilenamePtr = iLocalFilename->Des();
        // copy path and name to buffer
        locFilenamePtr.Copy( folder );
        locFilenamePtr.Append( iDownload->iDlName->Left( dlNameLen ) );

        // set drive letter
        locFilenamePtr[0] = (TUint16)TUint(driveChar);

        ReallocateStringL( iDestFilename, *iLocalFilename, KMaxPath );

        UpdateExtensionL();

		// Below code is to update the destination file path with new destination file name.
		// Check if the iDlName and filename in iLocalFileName are same. If not update the 
		// iLocalFilename and iDestFilename with iDlName. Note that iDlName has an unique file name.
    	if( iLocalFilename->Des().Right( iDownload->iDlName->Length() ).CompareF( *iDownload->iDlName ) )
        {
        TInt slash = iLocalFilename->Des().LocateReverse( '/' );

        if( slash == KErrNotFound )
            {
            slash = iLocalFilename->Des().LocateReverse( '\\' );
            }
           
        if( slash != KErrNotFound )
            {
            TInt newDLNameLen = iDownload->iDlName->Length();
            TInt newLength = slash + newDLNameLen + 1;
            iLocalFilename = iLocalFilename->ReAllocL(newLength);
            iLocalFilename->Des().SetLength( newLength );
            iLocalFilename->Des().Replace( slash+1, newDLNameLen, iDownload->iDlName->Des() );            
	        ReallocateStringL( iDestFilename, *iLocalFilename, KMaxPath );
            }
        }        
        
        }

	TBuf8<KMaxContentTypeLength> contentType;
	contentType.Copy( *iDownload->iDDType );
	
	if( (0 == contentType.Compare( KCodMimeType() )) ||
		(0 == contentType.Compare( KDdMimeType() ))	 ||
		(0 == contentType.Compare( KDd2MimeType() )) ||
		(0 == contentType.Compare( KMultiPartMimeType() )) )
		{
		iDdFilename = iLocalFilename->Des().AllocL();
		}

    iDriveId = GetDestinationDriveId();
    
    TText badChar( 32 );
    TBool isValid = iDownload->ClientApp()->Engine()->Fs().IsValidName( *iLocalFilename, badChar );
    
    CLOG_WRITE_1( "dest2: %S", iLocalFilename );
    CLOG_WRITE_2( "Valid: %d - %c", isValid, badChar );
    }

// -----------------------------------------------------------------------------
// CHttpStorage::GetDestinationDriveId
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUint CHttpStorage::GetDestinationDriveId()
    {
    LOGGER_ENTERFN( "GetDestinationDriveId" );
    TUint retVal( (TUint)KErrNotFound );

    if( iDestFilename && iDestFilename->Length() )
        {
        TInt drive;
        iDownload->ClientApp()->Engine()->Fs().CharToDrive( 
                                                    (*iDestFilename)[0], drive );

        TVolumeInfo info;

        if( iDownload->ClientApp()->Engine()->Fs().Volume( info, drive ) 
                                                                == KErrNone )
            {
            CLOG_WRITE_1( "Dest drive id: %x", info.iUniqueID );

            retVal = info.iUniqueID;
            }
        else
            {
            CLOG_WRITE( "Volume failed" );
            retVal = (TUint)KErrNotFound;
            }
        }
    CLOG_WRITE_1( " retVal: %d", retVal );
    return retVal;
    }
    
// -----------------------------------------------------------------------------
// CHttpStorage::UpdateDestinationFilenameL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CHttpStorage::UpdateDestinationFilenameL( const TDesC16& aFilename, TBool aUserSet )
	{
	LOGGER_ENTERFN( "UpdateDestinationFilenameL" );
	CLOG_WRITE_2( "FN: [%S], userSet: %d", &aFilename, aUserSet );
    TBufC<KMaxFileName> oldFileName;
	if( iDestFilename )
		{
		oldFileName = *iDestFilename;
		}
    ReallocateStringL( iDestFilename, aFilename, KMaxPath );
    iDestFNameSet = aUserSet;
    if ( aFilename.Length() == 0 )
        {
        return;
        }
        
    iDriveId = GetDestinationDriveId();
    TInt drive;
    if( !iDownload->ClientApp()->Engine()->Fs().CharToDrive((*iDestFilename)[0], drive) )
        {
        TDriveInfo driveInfo;
        if( !iDownload->ClientApp()->Engine()->Fs().Drive( driveInfo, drive) )
            {
            iRemovableDest = (driveInfo.iDriveAtt & KDriveAttRemovable);
            CLOG_WRITE_1( "Removable: [%d]", iRemovableDest );
            }
        else
            {
            CLOG_WRITE("DriveInfo failed");
            }
        }
    else
        {
        CLOG_WRITE("CharToDrive failed");
        }
    
	}

// -----------------------------------------------------------------------------
// CHttpStorage::AppendStorageInfoL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CHttpStorage::AppendStorageInfoL( TPtr8& aBuf ) const
    {
    AppendBufL( aBuf, iDestFilename );
    APPEND_BUF_INT( aBuf, iDestFNameSet );
    AppendBufL( aBuf, iLocalFilename );    
    APPEND_BUF_INT( aBuf, iLength );
    APPEND_BUF_INT( aBuf, iDriveId );
    APPEND_BUF_INT( aBuf, iStorageMethod );
    APPEND_BUF_INT( aBuf, iProgressiveDownload);
    APPEND_BUF_INT( aBuf, iDownloadedSize);
    AppendBufL( aBuf, iDdFilename );    
    APPEND_BUF_INT( aBuf, iDownload->iMoLength );
    APPEND_BUF_INT( aBuf, iRemovableDest );

    }

// -----------------------------------------------------------------------------
// CHttpStorage::SaveStorageInfoL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CHttpStorage::LoadStorageInfoL( RFile& aInFile )
    {
    ReadHBufCL( aInFile, iDestFilename );
    READ_INT_L( aInFile, iDestFNameSet );
    ReadHBufCL( aInFile, iLocalFilename );    
    READ_INT_L( aInFile, iLength );
    READ_INT_L( aInFile, iDriveId );
    READ_INT_L( aInFile, iStorageMethod );
    READ_INT_L( aInFile, iProgressiveDownload);
    READ_INT_L( aInFile, iDownloadedSize );
    ReadHBufCL( aInFile, iDdFilename );
    READ_INT_L( aInFile, iDownload->iMoLength  );
    READ_INT_L( aInFile, iRemovableDest  );

    }

// -----------------------------------------------------------------------------
// CHttpStorage::UpdateExtensionL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CHttpStorage::UpdateExtensionL()
    {
    LOGGER_ENTERFN( "UpdateExtensionL" );
    
    TPtr fileName( iDestFilename->Des() );
    TBool toDelete;
    const HBufC8* contentType = NULL;

#ifdef DRM_EXTENSION_SPECIAL_CASE
    // Check if we have the original DRM content-type    
    const CArrayPtrFlat<CHeaderField>* resp = iDownload->ResponseHeaders();
    
    TInt index = iDownload->FindHeaderField( resp, KDRMOldContentType );
    
    if( index != KErrNotFound )
        // Yes, we have it. Use the content-type set to this field.
        {
        CLOG_WRITE( "Old DRM content type" );
        contentType = (*resp)[index]->FieldRawData();
        }
    else
        {
        contentType = iDownload->GetString8AttributeL( EDlAttrContentType, toDelete );
        }
#else
    contentType = iDownload->GetString8AttributeL( EDlAttrContentType, toDelete );
#endif        

    CLOG_WRITE8_1( "Content-Type: [%S]", contentType );
    
    TDataType dataType( *contentType );
    
    iDownload->ClientApp()->Engine()->DocHandler()->CheckFileNameExtension( fileName, dataType );

    ReallocateStringL( iLocalFilename, *iDestFilename, KMaxPath );
    
    CLOG_WRITE_1( "filename: [%S]", &fileName );
    CLOG_WRITE_1( "DlName: [%S]", iDownload->iDlName );
    
    if( fileName.Right( iDownload->iDlName->Length() ).CompareF( *iDownload->iDlName ) )
        {
        CLOG_WRITE("Updating DlName");
        TInt slash = fileName.LocateReverse( '/' );

        if( slash == KErrNotFound )
            {
            slash = fileName.LocateReverse( '\\' );
            }
           
        if( slash != KErrNotFound )
            {
            iDownload->SetDownloadNameL( fileName.Right(fileName.Length()-slash-1) );
            }
        else
            {
            iDownload->SetDownloadNameL( fileName );
            }
        }
    }

// -----------------------------------------------------------------------------
// CHttpStorage::SetBufferSize
// set size of disk buffer: after this - call EnableBuffering() or buffering
// will be disabled.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CHttpStorage::SetBufferSizeL( TInt aBufferSize )
    {
    DisableBufferingL();
    iBufferSize = aBufferSize;
    }

// -----------------------------------------------------------------------------
// CHttpStorage::EnableBuffering
// turn on buffered writing of data to disk
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CHttpStorage::EnableBufferingL()
    {
    // first check if buffer exists, and if it does, has iBufferSize
    // been changed since buffering was last enabled
    
    CLOG_WRITE("CHttpStorage::EnableBufferingL >>");
    
    if( iBufferingEnabled && iBufferSize != iStorage->CurrentBufferSize())
        {
        FlushL();
        }

    iBufferingEnabled = ETrue;
    
    CLOG_WRITE("CHttpStorage::EnableBufferingL <<");
    }

// -----------------------------------------------------------------------------
// CHttpStorage::DisableBuffering
// turn off buffered writing of data to disk
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CHttpStorage::DisableBufferingL()
    {
    FlushL();
    iBufferingEnabled = EFalse;
    }

// -----------------------------------------------------------------------------
// CHttpStorage::Flush
// Flush internal buffer to disk
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CHttpStorage::FlushL()
    {
    iStorage->FlushBuffersL();
    iStorage->ResetBuffers();
    }

// -----------------------------------------------------------------------------
// CHttpStorage::SetLocalFilenameL
// -----------------------------------------------------------------------------
//
void CHttpStorage::SetLocalFilenameL(const TDesC16& aValue)
{
    TBool isCodDownload;
    iDownload->GetBoolAttributeL( EDlAttrCodDownload, isCodDownload );
    // allow to set the local file name only for OMA downloads in case PDL
	// In this case the MP/VP is responsible for file move. This app should set 
	// dest and local file names before move.
	if (isCodDownload && iProgressiveDownload)
	{
	  TRAP_IGNORE(ReallocateStringL( iLocalFilename, aValue, KMaxPath ));
	}
}
    
// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File  
