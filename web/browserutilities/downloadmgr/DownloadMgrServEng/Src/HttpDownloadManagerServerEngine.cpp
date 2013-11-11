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
* Description:  ?Description
*
*/



// INCLUDE FILES

	//User Includes
#include "HttpDownloadManagerServerEngine.h"
#include "HttpClientApp.h"
#include "HttpDownloadMgrLogger.h"

	//System Includes
#include <bldvariant.hrh>
#include <e32std.h>
#include <centralrepository.h>
#include <BrowserUiSDKCRKeys.h>
#include <DocumentHandler.h>
#include <featmgr.h>

#ifdef RD_MULTIPLE_DRIVE
#include <driveinfo.h>
#endif //RD_MULTIPLE_DRIVE

#ifdef __DOWNLOADMGR_CLOG__
#pragma message ( "DMgr Engine logger enabled" )
#endif

// EXTERNAL DATA STRUCTURES
//extern  ?external_data;

// EXTERNAL FUNCTION PROTOTYPES  
//extern ?external_function( ?arg_type,?arg_type );

// CONSTANTS
//const ?type ?constant_var = ?constant;
_LIT( KDmDefaultDir, "c:\\system\\dmgr\\" );
_LIT( KDefaultDriveList, "C;E" );
#ifdef RD_MULTIPLE_DRIVE
_LIT( KDefaultDriveListSep, ";" );
#endif
// TODO: Modify KGrandMasterAppUid if stand-alone DMgr app's uid is known")

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

// ============================= LOCAL FUNCTIONS ===============================
#if defined(_DEBUG)
void DMPanic( TInt aPanicCode )
    {
    User::Panic( _L("DMEngine"), aPanicCode );
    }
#endif

// ============================ MEMBER FUNCTIONS ===============================

CMMCNotifier::CMMCNotifier( RProperty* aProperty )
	: CActive( EPriorityStandard ) 
	,iProperty( aProperty )
	{
    CActiveScheduler::Add( this );
	}

CMMCNotifier::~CMMCNotifier()
    {
    Cancel();
    }
    
void CMMCNotifier::StartObserving( MMMCObserver* aObserver )
	{
	__ASSERT_DEBUG( !IsActive(), User::Panic( KNullDesC, KErrInUse) );
	
	iObserver = aObserver;
	
	iProperty->Subscribe( iStatus );
	SetActive();
	}
	
void CMMCNotifier::RunL()
    {
	TInt value( 0 );
	iProperty->Get( value);
	
	iObserver->MMCModeChanged( value == 1 );
	
	iProperty->Subscribe( iStatus );
	SetActive();
    }
    
void CMMCNotifier::DoCancel()
    {
    iProperty->Cancel();
    }

// -----------------------------------------------------------------------------
// CHttpDownloadManagerServerEngine::CHttpDownloadManagerServerEngine
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CHttpDownloadManagerServerEngine::CHttpDownloadManagerServerEngine()
    : iMMCUniqueID( (TUint)KErrNotFound ),
    iFeatProgressiveDownload(EFalse)
    {
    CLOG_CREATE;
    }

// -----------------------------------------------------------------------------
// CHttpDownloadManagerServerEngine::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CHttpDownloadManagerServerEngine::ConstructL()
    {
    CLOG_NAME( _L("Engine") );
    LOGGER_ENTERFN( "ConstructL()" );

    User::LeaveIfError( iRfs.Connect() );
	User::LeaveIfError(iRfs.ShareProtected());

    iDocHandler = CDocumentHandler::NewL();
    
    iFeatProgressiveDownload = FeatureManager::FeatureSupported( KFeatureIdBrowserProgressiveDownload );
    
    iProperty = new (ELeave) RProperty;
    CLOG_ATTACH( iProperty, this );
    iProperty->Attach( KPSUidUikon,KUikMMCInserted );
    
    iMMCNotifier = new (ELeave) CMMCNotifier( iProperty );
    CLOG_ATTACH( iMMCNotifier, this );
    iMMCNotifier->StartObserving( this );

    QueryMMCUniqueId();

    QueryDriveListL();

    iClientApps = new (ELeave) CArrayPtrFlat<CHttpClientApp>(2);

    // make directory for the server
    // No problem if it already exists
    TInt error = iRfs.MkDirAll( KDmDefaultDir );
    if( error != KErrNone && error != KErrAlreadyExists )
        // leave if makedir failed in some way
        // don't leave if already exists
        {
        CLOG_WRITE8_1( "MkDirAll: %d", error );
        User::Leave( error );
        }

    LoadClientsL();
    
    // This code generates a unique id for the next download
    // client will create. It is always the the highest unique
    // id of all downloads + 1.
    iNextDownloadId = 1;

    for( TInt apps = 0; apps < iClientApps->Count(); ++apps )
        {
        CArrayPtrFlat<CHttpDownload>* allDownloads = (*iClientApps)[apps]->Downloads();

        for( TInt i = 0; i < allDownloads->Count(); ++i )
            {
            if( iNextDownloadId <= (*allDownloads)[i]->Id() )
                {
                iNextDownloadId = (*allDownloads)[i]->Id() + 1;
                }
            }
        }
    
    error = iSocketServ.Connect();
    User::LeaveIfError(error);
    }

// -----------------------------------------------------------------------------
// CHttpDownloadManagerServerEngine::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CHttpDownloadManagerServerEngine* 
                                    CHttpDownloadManagerServerEngine::NewL()
    {
    CHttpDownloadManagerServerEngine* self = 
                            new( ELeave ) CHttpDownloadManagerServerEngine();

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

    
// Destructor
CHttpDownloadManagerServerEngine::~CHttpDownloadManagerServerEngine()
    {
    CLOG_WRITE( "~CHttpDownloadManagerServerEngine" );

    CloseEngine( EFalse );
    
    CLOG_CLOSE;
    }

// -----------------------------------------------------------------------------
// CHttpDownloadManagerServerEngine::CloseEngine
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CHttpDownloadManagerServerEngine::CloseEngine( TBool /*bStore*/ )
    {
    LOGGER_ENTERFN( "CloseEngine" );
    iEngineClosing = ETrue;

    if( iClientApps )
        {
        iClientApps->ResetAndDestroy();
        delete iClientApps; iClientApps = NULL;
        }

    if( iMMCNotifier )
        {
        iMMCNotifier->Cancel();
        delete iMMCNotifier; iMMCNotifier = NULL;
        }
        
    delete iProperty; iProperty = NULL;

    iRfs.Close();
    iSocketServ.Close();
    
    delete iDocHandler; iDocHandler = NULL;
    }

// -----------------------------------------------------------------------------
// CHttpDownloadManagerServerEngine::CreateNewClientAppL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CHttpClientAppInstance* 
    CHttpDownloadManagerServerEngine::CreateNewClientAppInstanceL( 
                                            TUint32 aAppUid, 
                                            MDownloadStateObserver* aObserver,
                                            TBool aMaster )
    {
    LOGGER_ENTERFN( "CreateNewClientAppInstanceL" );

    // clientApp is stored in iCliendApps array
    CHttpClientApp* clientApp = CreateNewClientAppL( aAppUid );

    CHttpClientAppInstance* instance = 
                        clientApp->CreateNewInstanceL( aObserver,
                                                       aMaster );

    return instance;
    }

// -----------------------------------------------------------------------------
// CHttpDownloadManagerServerEngine::CloseClientInstance
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CHttpDownloadManagerServerEngine::CloseClientInstance( 
                                              CHttpClientAppInstance* aAppInst )
    {
    LOGGER_ENTERFN( "CloseClientInstance" );

    if( !aAppInst )
        {
        return;
        }

    __ASSERT_DEBUG( aAppInst->ClientApp(), DMPanic( KErrArgument ) );

    aAppInst->ClientApp()->CloseInstance( aAppInst );
    }

// -----------------------------------------------------------------------------
// CHttpDownloadManagerServerEngine::ActiveDownloads
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CHttpDownloadManagerServerEngine::ActiveDownloads() const
    {
    LOGGER_ENTERFN( "ActiveDownloads" );

    TInt downloads( 0 );

    for( TInt apps = 0; apps < iClientApps->Count(); ++apps )
        {
        CArrayPtrFlat<CHttpDownload>* dlArray = (*iClientApps)[apps]->Downloads();

        for( TInt dl = 0; dl < dlArray->Count(); ++dl )
            {
            if( (*dlArray)[dl]->State() == EHttpDlInprogress )
                {
                ++downloads;
                }
            }
        }

    return downloads;
    }

// -----------------------------------------------------------------------------
// CHttpDownloadManagerServerEngine::AllDownloadsSizeL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CHttpDownloadManagerServerEngine::AllDownloadsSizeL(const CHttpDownload *aDownload) const
    {
    LOGGER_ENTERFN( "AllDownloadsSize" );

    TInt allDownloadsSize( 0 );
    TInt numClientApps = iClientApps->Count();
    for( TInt apps = 0; apps < numClientApps; ++apps )
        {
        CArrayPtrFlat<CHttpDownload>* dlArray = (*iClientApps)[apps]->Downloads();
        TInt numDownloads = dlArray->Count();
        for( TInt dl = 0; dl < numDownloads; ++dl )
            {
            if( ( aDownload != (*dlArray)[dl]) && (*dlArray)[dl]->State() != EHttpDlMultipleMOCompleted)
                {
                TInt32 totalSize(0);
                TInt32 downloadedSize(0);
                
                (*dlArray)[dl]->GetIntAttributeL(EDlAttrMultipleMOLength, totalSize);
                (*dlArray)[dl]->GetIntAttributeL(EDlAttrMultipleMODownloadedSize, downloadedSize);

                //Do not conside the downloads with unknown size
                if(KDefaultContentLength != totalSize)
                	allDownloadsSize += (totalSize - downloadedSize);
                }
            }
        }
    
    return allDownloadsSize;
    }


// -----------------------------------------------------------------------------
// CHttpDownloadManagerServerEngine::AllDownloadsSizeInDriveL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CHttpDownloadManagerServerEngine::AllDownloadsSizeInDriveL(const CHttpDownload *aDownload, TInt aDriveId) const
    {
    LOGGER_ENTERFN( "AllDownloadsSizeInDriveL" );

    TInt allDownloadsSize( 0 );
    TInt numClientApps = iClientApps->Count();
    for( TInt apps = 0; apps < numClientApps; ++apps )
        {
        CArrayPtrFlat<CHttpDownload>* dlArray = (*iClientApps)[apps]->Downloads();
        TInt numDownloads = dlArray->Count();
        for( TInt dl = 0; dl < numDownloads; ++dl )
            {
            if( ( aDownload != (*dlArray)[dl]) && (*dlArray)[dl]->State() != EHttpDlMultipleMOCompleted 
                  && aDriveId == (*dlArray)[dl]->GetDestinationDriveID())
                {
                TInt32 totalSize(0);
                TInt32 downloadedSize(0);
                
                (*dlArray)[dl]->GetIntAttributeL(EDlAttrMultipleMOLength, totalSize);
                (*dlArray)[dl]->GetIntAttributeL(EDlAttrMultipleMODownloadedSize, downloadedSize);

                //Do not conside the downloads with unknown size
                if(KDefaultContentLength != totalSize)
                	allDownloadsSize += (totalSize - downloadedSize);
                }
            }
        }
    
    return allDownloadsSize;
    }
// -----------------------------------------------------------------------------
// CHttpDownloadManagerServerEngine::IsDownloadAttached
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CHttpDownloadManagerServerEngine::IsAttachedAlready(TInt32 aDownloadId)
{
    LOGGER_ENTERFN( "IsAttachedAlready" );

    for( TInt apps = 0; apps < iClientApps->Count(); ++apps )
        {
        CArrayPtrFlat<CHttpDownload>* dlArray = (*iClientApps)[apps]->Downloads();

        for( TInt dl = 0; dl < dlArray->Count(); ++dl )
            {
            if( (*dlArray)[dl]->Id() == aDownloadId )
                {
				if ((*dlArray)[dl]->PDClientAppInstance())
					{
						return ETrue;
					}
                }
            }
        }

    return EFalse;

}

// -----------------------------------------------------------------------------
// CHttpDownloadManagerServerEngine::FindDownload
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CHttpDownload* 
    CHttpDownloadManagerServerEngine::FindDownload( TInt32 aDownloadId )
    {
    for( TInt apps = 0; apps < iClientApps->Count(); ++apps )
        {
        CArrayPtrFlat<CHttpDownload>* allDownloads = (*iClientApps)[apps]->Downloads();

        for( TInt i = 0; i < allDownloads->Count(); ++i )
            {
            if( (*allDownloads)[i]->Id() == aDownloadId )
                {
                return (*allDownloads)[i];
                }
            }
        }

    return NULL;        
    }

// -----------------------------------------------------------------------------
// CHttpDownloadManagerServerEngine::CreateNewClientAppL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CHttpClientApp* 
    CHttpDownloadManagerServerEngine::CreateNewClientAppL( TUint32 aAppUid )
    {
    LOGGER_ENTERFN( "CreateNewClientAppL" );
    CHttpClientApp* clientApp = NULL;

    // Is this client app already registered?
    for( TInt index = 0; index < iClientApps->Count(); ++index )
        {
        if( (*iClientApps)[index]->AppUid() == aAppUid )
            {
            // we found it
            clientApp = (*iClientApps)[index];
            clientApp->LoadClientInfoL(KNullDesC);
            break;
            }
        }

    if( !clientApp )
        {
        clientApp = CHttpClientApp::NewL( aAppUid,
                                          this );
        CleanupStack::PushL( clientApp );
        iClientApps->AppendL( clientApp );
        CleanupStack::Pop( clientApp );
        }

    return clientApp;
    }

// -----------------------------------------------------------------------------
// CHttpDownloadManagerServerEngine::LoadClientsL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CHttpDownloadManagerServerEngine::LoadClientsL()
    {
    LOGGER_ENTERFN( "LoadClientsL" );

    CDir* dirs = 0;
    // hidden + system + dir + only these
    TUint mask = KEntryAttMatchMask | KEntryAttMatchExclusive; 

    User::LeaveIfError( iRfs.GetDir( KDmDefaultDir, 
                                     mask, 
                                     EAscending | EDirsFirst, 
                                     dirs) );
    if( dirs && dirs->Count() )
        {
        CleanupStack::PushL( dirs );

        for( TInt ii = 0; ii < dirs->Count(); ++ii )
            {
            TEntry entry = (*dirs)[ii];

            CLOG_WRITE_1( "Found: %S", &entry.iName );

            TUint32 clientUid = CheckClientDirName( entry.iName );
            if( clientUid )
                {
                CreateNewClientAppL( clientUid );
                }
            }

        CleanupStack::Pop( dirs );
        }

    delete dirs;
    }

// -----------------------------------------------------------------------------
// CHttpDownloadManagerServerEngine::CheckClientDirName
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUint32 CHttpDownloadManagerServerEngine::CheckClientDirName( TDesC& aClientDir )
    {
    TLex temp( aClientDir );
    TUint32 clientUid( 0 );

    temp.Val( clientUid, EHex );

    return clientUid;
    }

// -----------------------------------------------------------------------------
// CHttpDownloadManagerServerEngine::ClientAppFolder
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CHttpDownloadManagerServerEngine::ClientAppFolder( 
                                                    const CHttpClientApp* aClientApp, 
                                                    TDes& aFolder )
    {
    _LIT( KClientAppFolderFormat, "%S%x\\" );

    aFolder.Format( KClientAppFolderFormat, &KDmDefaultDir, aClientApp->AppUid() );
    }

// -----------------------------------------------------------------------------
// CHttpDownloadManagerServerEngine::ClientAppFolder
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CHttpDownloadManagerServerEngine::ClientAppFolderWithoutDriveLetter( 
                                        const CHttpClientApp* aClientApp, 
                                        TDes& aFolder )
    {
    _LIT( KClientAppFolderFormat, "%s%x\\" );

    // [3] cut off the drive letter + ':\\' part
    aFolder.Format( KClientAppFolderFormat, 
                    &KDmDefaultDir()[3], 
                    aClientApp->AppUid() );
    }

// -----------------------------------------------------------------------------
// CHttpDownloadManagerServerEngine::DownloadInfoFolder
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CHttpDownloadManagerServerEngine::DownloadInfoFolder( 
                                                const CHttpClientApp* aClientApp,  
                                                TDes& aFolder ) const
    {
    _LIT( KDownloadFolderFormat, "%S%x\\%S\\" );

    aFolder.Format( KDownloadFolderFormat, 
                        &KDmDefaultDir, 
                        aClientApp->AppUid(),
                        &KInfoFilesDirName );
    }

// -----------------------------------------------------------------------------
// CHttpDownloadManagerServerEngine::DownloadInfoFolder
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CHttpDownloadManagerServerEngine::CODDownloadInfoFolder( 
                                                const CHttpClientApp* aClientApp,  
                                                TDes& aFolder ) const
    {
    _LIT( KDownloadFolderFormat, "%S%x\\%S\\" );

    aFolder.Format( KDownloadFolderFormat, 
                        &KDmDefaultDir, 
                        aClientApp->AppUid(),
                        &KInfoFilesCodDirName );
    }
// -----------------------------------------------------------------------------
// CHttpDownloadManagerServerEngine::DownloadInfoFolder
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CHttpDownloadManagerServerEngine::DownloadContentFolder( 
                                                const CHttpClientApp* aClientApp, 
                                                TDes& aFolder ) const
    {
    _LIT( KDownloadFolderFormat, "%S%x\\%S\\" );

    aFolder.Format( KDownloadFolderFormat, 
                        &KDmDefaultDir, 
                        aClientApp->AppUid(),
                        &KContentsDirName );
    }

// -----------------------------------------------------------------------------
// CHttpDownloadManagerServerEngine::DownloadInfoFolder
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CHttpDownloadManagerServerEngine::CodFolder( 
                                                const CHttpClientApp* aClientApp, 
                                                TDes& aFolder ) const
    {
    _LIT( KDownloadFolderFormat, "%S%x\\%S\\" );

    aFolder.Format( KDownloadFolderFormat, 
                        &KDmDefaultDir, 
                        aClientApp->AppUid(),
                        &KCodsDirName );
    }

#ifdef DEAD_CODE
// -----------------------------------------------------------------------------
// CHttpDownloadManagerServerEngine::AllDownloadsL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CArrayPtrFlat<CHttpDownload>* 
                        CHttpDownloadManagerServerEngine::AllDownloadsL() const
    {
    CArrayPtrFlat<CHttpDownload>* downloads = 
                                    new (ELeave) CArrayPtrFlat<CHttpDownload>(2);

    CleanupStack::PushL( downloads );

    for( TInt apps = 0; apps < iClientApps->Count(); ++apps )
        {
        CArrayPtrFlat<CHttpDownload>* allDownloads = (*iClientApps)[apps]->Downloads();

        for( TInt i = 0; i < allDownloads->Count(); ++i )
            {
            downloads->AppendL( (*allDownloads)[i] );
            }
        }
    CleanupStack::Pop( downloads );

    return downloads;
    }
#endif

// -----------------------------------------------------------------------------
// CHttpDownloadManagerServerEngine::Fs
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
RFs& CHttpDownloadManagerServerEngine::Fs()
    {
    return iRfs;
    }

// -----------------------------------------------------------------------------
// CHttpDownloadManagerServerEngine::SocketServ
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
RSocketServ& CHttpDownloadManagerServerEngine::SocketServ()
    {
    return iSocketServ;
    }

// -----------------------------------------------------------------------------
// CHttpDownloadManagerServerEngine::NextDownloadId
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CHttpDownloadManagerServerEngine::NextDownloadId()
    {
    return iNextDownloadId++;
    }

//------------------------------------------------------------------------
//CHttpDownloadManagerServerEngine::MMCModeChanged
//------------------------------------------------------------------------
void CHttpDownloadManagerServerEngine::MMCModeChanged( TBool aMmcPresent )
    {
    for( TInt apps = 0; apps < iClientApps->Count(); ++apps )
        {
        CArrayPtrFlat<CHttpDownload>* allDownloads = (*iClientApps)[apps]->Downloads();

        for( TInt i = 0; i < allDownloads->Count(); ++i )
            {
            QueryMMCUniqueId();
            
            if( aMmcPresent )
                {
                (*allDownloads)[i]->MediaInserted( iMMCUniqueID );
                }
            else
                {
                (*allDownloads)[i]->MediaRemoved( iMMCUniqueID );
                }
            }
        }
    }

//------------------------------------------------------------------------
//CHttpDownloadManagerServerEngine::MMCModeChanged
//------------------------------------------------------------------------
void CHttpDownloadManagerServerEngine::QueryMMCUniqueId()
    {
    //
    // Query the MMC's or Default Removable Mass Storage unique id
    //
    TInt drive( 0 );
#ifdef RD_MULTIPLE_DRIVE
    DriveInfo::GetDefaultDrive( DriveInfo::EDefaultRemovableMassStorage, drive );
#else
    iRfs.CharToDrive( 'E', drive );
#endif

    TVolumeInfo info;

    if( iRfs.Volume( info, drive ) == KErrNone )
        {
        iMMCUniqueID = info.iUniqueID;
        }
    else
        {
        iMMCUniqueID = (TUint)KErrNotFound;
        }


    CLOG_WRITE_1( "MMC id: %d", iMMCUniqueID );
    }

#ifdef RD_MULTIPLE_DRIVE    
//------------------------------------------------------------------------
//CHttpDownloadManagerServerEngine::QueryDynDriveListLC
//------------------------------------------------------------------------
HBufC8* CHttpDownloadManagerServerEngine::QueryDynDriveListLC()
    {
    TDriveList driveList;
    TInt driveCount( 0 );
    TInt err;
    TChar driveLetter;
    TBuf8<KMaxDriveListStrLen> driveLettersDyn;

    // Checking validity of drives in Cenrep List
    // drive letters are separated by semicolons
    // Destination is FFS in default
    TInt drive;
    User::LeaveIfError(
        DriveInfo::GetDefaultDrive( DriveInfo::EDefaultPhoneMemory, drive ) );

    for( TInt i = 0; i < iDriveLettersCenRep.Length(); i = i + 2 )
        {
        if( iRfs.CharToDrive( iDriveLettersCenRep[i], drive ) == KErrNone )
            {
            TUint status;
            if ( DriveInfo::GetDriveStatus( iRfs, drive, status ) == KErrNone )
                {
                if ( ( status & ( DriveInfo::EDriveUserVisible | DriveInfo::EDrivePresent ) ) )
                    {
                    CLOG_WRITE_1( "Drive %d is present and visible", drive);
                    }
                }
            }
        }

    if ( iDriveLettersCenRep.Length() > 0 )
        {
        driveLettersDyn.Append( iDriveLettersCenRep );
        if ( driveLettersDyn[driveLettersDyn.Length() - 1] != ';' )
            {
            driveLettersDyn.Append( KDefaultDriveListSep );
            }
        }

    // get the list of drives available in real time
    err = DriveInfo::GetUserVisibleDrives( iRfs, driveList, driveCount );
    if ( err == KErrNone )
        {
        TInt count( driveList.Length() );
        for ( TInt i( 0 ); i < count; ++i )
            {
            if ( driveList[ i ] )
                {
                User::LeaveIfError( iRfs.DriveToChar( i, driveLetter) );
                TInt drivePos = driveLettersDyn.LocateF( driveLetter );
                if ( drivePos == KErrNotFound )
                    {
                    driveLettersDyn.Append( driveLetter ); 
                    driveLettersDyn.Append( KDefaultDriveListSep ); 
                    }
                }
            }

        CLOG_WRITE_1( "Pref. drive list dynamic: [%S]", &driveLettersDyn );
        }
    else
        {
        CLOG_WRITE_1( "GetUserVisibleDrives failed with %d", err );
        }

    HBufC8* driveLetters = HBufC8::NewLC( KMaxDriveListStrLen );
    driveLetters->Des().Copy( driveLettersDyn );
    return driveLetters;
    }
#endif

//------------------------------------------------------------------------
//CHttpDownloadManagerServerEngine::QueryDriveList
//------------------------------------------------------------------------
void CHttpDownloadManagerServerEngine::QueryDriveListL()
    {
    CRepository* repository = CRepository::NewLC( KCRUidBrowser );
    
    if( repository->Get(KBrowserDrivePrefListForDownloadedContent, iDriveLettersCenRep) != KErrNone )
        {
        CLOG_WRITE( "Drive list not found" );
        iDriveLettersCenRep.Copy( KDefaultDriveList );
        }
    
    CLOG_WRITE_1( "Pref. drive list: [%S]", &iDriveLettersCenRep );
    
    CleanupStack::PopAndDestroy( repository );
    }

//------------------------------------------------------------------------
//CHttpDownloadManagerServerEngine::DocHandler
//------------------------------------------------------------------------
CDocumentHandler* CHttpDownloadManagerServerEngine::DocHandler() const
    {
    return iDocHandler;
    }
    
//  End of File  
