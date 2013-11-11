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
* Description: 
*
*/



// INCLUDE FILES
#include "HttpClientAppInstance.h"
#include "HttpConnHandler.h"
#include "HttpDownloadManagerServerEngine.h"
#include "FileExt.h"
#include "HttpClientApp.h"
#include "HttpDownloadMgrLogger.h"

// EXTERNAL DATA STRUCTURES
//extern  ?external_data;

// EXTERNAL FUNCTION PROTOTYPES  
//extern ?external_function( ?arg_type,?arg_type );

// CONSTANTS
_LIT( KDownloadFilenameFormat, "%S%S");

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

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CHttpClientApp::CHttpClientApp
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CHttpClientApp::CHttpClientApp( TUint32 aAppUid, 
                                CHttpDownloadManagerServerEngine* aEngine )
    : iAppUid( aAppUid )
    , iEngine( aEngine )
    {
    CLOG_CREATE;
    }

// -----------------------------------------------------------------------------
// CHttpClientApp::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CHttpClientApp::ConstructL()
    {
    CLOG_NAME_1( _L("CHttpClientApp_%x"), iAppUid );

    iInstances = new (ELeave) CArrayPtrFlat<CHttpClientAppInstance>(2);
    iDownloads = new (ELeave) CArrayPtrFlat<CHttpDownload>(2);
    iConnections = new (ELeave) CArrayPtrFlat<CHttpConnHandler>(2);

    TBuf<KMaxPath> folder;
    iEngine->ClientAppFolder( this, folder );

    // Create client app's folder
    TInt error = iEngine->Fs().MkDirAll( folder );
    if( error != KErrNone && error != KErrAlreadyExists )
        // leave if makedir failed in some way
        // don't leave if already exists
        {
        CLOG_WRITE8_1( "MkDirAll: %d", error );
        User::Leave( error );
        }

    // create folder for info files
    iEngine->DownloadInfoFolder( this, folder );
    
    error = iEngine->Fs().MkDirAll( folder );
    if( error != KErrNone && error != KErrAlreadyExists )
        // leave if makedir failed in some way
        // don't leave if already exists
        {
        CLOG_WRITE8_1( "info dir: %d", error );
        User::Leave( error );
        }

    // create folder for COD info files
    iEngine->CODDownloadInfoFolder( this, folder );

    error = iEngine->Fs().MkDirAll( folder );
    if( error != KErrNone && error != KErrAlreadyExists )
        // leave if makedir failed in some way
        // don't leave if already exists
        {
        CLOG_WRITE8_1( "info dir: %d", error );
        User::Leave( error );
        }

    // create folder for content files
    iEngine->DownloadContentFolder( this, folder );
    error = iEngine->Fs().MkDirAll( folder );
    if( error != KErrNone && error != KErrAlreadyExists )
        // leave if makedir failed in some way
        // don't leave if already exists
        {
        CLOG_WRITE8_1( "content dir: %d", error );
        User::Leave( error );
        }

    iEngine->ClientAppFolder( this, folder );
    LoadClientInfoL( folder );
    
    iEngine->CodFolder( this, folder );

    CFileMan* fileman = CFileMan::NewL( iEngine->Fs() );
    fileman->RmDir( folder );
    delete fileman;
    
    iEngine->Fs().MkDir( folder );
    }

// -----------------------------------------------------------------------------
// CHttpClientApp::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CHttpClientApp* CHttpClientApp::NewL( TUint32 aAppUid, 
                                      CHttpDownloadManagerServerEngine* aEngine )
    {
    CHttpClientApp* self = new( ELeave ) CHttpClientApp( aAppUid, aEngine );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

    
// Destructor
CHttpClientApp::~CHttpClientApp()
    {
    CloseClientApp( EFalse );
    
    CLOG_CLOSE;
    }


// -----------------------------------------------------------------------------
// CHttpClientApp::CreateNewInstanceL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CHttpClientAppInstance* 
        CHttpClientApp::CreateNewInstanceL( MDownloadStateObserver* aObserver,
                                            TBool aMaster )
    {
    LOGGER_ENTERFN( "CreateNewInstanceL" );

    TInt instanceId( 1 );
    TInt index;

    if( aMaster )
        {
        // Check that there can be only 1 master instance
        for( index = 0; index < iInstances->Count(); ++index )
            {
            if( (*iInstances)[index]->Master() )
                {
                User::Leave( KErrAlreadyExists );
                }
            }
        }

    // generate new instance id
    for( index = 0; index < iInstances->Count(); ++index )
        {
        if( instanceId <= (*iInstances)[index]->InstanceId() )
            {
            instanceId = (*iInstances)[index]->InstanceId() + 1;
            }
        }

    CLOG_WRITE8_1( "New instance id: [%d]", instanceId );

    
    CHttpConnHandler* conn = NULL;
    
    if(iInstances->Count() ==0 && iConnections->Count() > 0)
        {
          conn = iConnections->At(0) ;	 
        }
    else
        {
        // Create new connhandler used by the instance
        // Every instance has its own connhandler
        conn = CHttpConnHandler::NewL( this );

        // connhandlers are owned by CHttpClientApp.
        CleanupStack::PushL( conn );
        iConnections->AppendL( conn );
        CleanupStack::Pop( conn );
        }
    

    // Create new client instance
    CHttpClientAppInstance* instance = 
                    CHttpClientAppInstance::NewL( this, conn, aMaster, instanceId, aObserver );

    CleanupStack::PushL( instance );
    iInstances->AppendL( instance );
    CleanupStack::Pop( instance );

    // Associate connhandler with client instance
    conn->SetClientAppInst( instance );

    for( index = 0; index < iDownloads->Count(); ++index )
        {
        if( !(*iDownloads)[index]->ClientAppInstance() )
            // this download is not occupied by any client instance
            {
            CLOG_WRITE8_1( "Controlling download: [%d]", (*iDownloads)[index]->Id() );
            // From it's owned by this new instance
            (*iDownloads)[index]->SetClientInstance( instance, ETrue );
            if( (*iDownloads)[index]->ConnHandler() && aMaster )//In case of embedded browser(aMaster =0) the connection should be set by parent application
                {
                // conn handler in this download is not associated with
                // any client instance. do it now.
                (*iDownloads)[index]->ConnHandler()->SetClientAppInst( instance );
                }
            }
        }

    return instance;
    }

// -----------------------------------------------------------------------------
// CHttpClientApp::CloseInstance
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CHttpClientApp::CloseInstance( CHttpClientAppInstance* aInstance )
    {
    LOGGER_ENTERFN( "CloseInstance" );

    CLOG_WRITE_1( "Instance: %d", aInstance->InstanceId() );
    CLOG_WRITE_1( "ExitAction: %d", aInstance->ExitAction() );

    TInt index;

    // find this instance
    for( index = 0; index < iInstances->Count(); ++index )
        {
        if( aInstance == (*iInstances)[index] )
            {
            break;
            }
        }

    __ASSERT_DEBUG( index != iInstances->Count(), DMPanic( KErrCorrupt ) );
    if( index == iInstances->Count() )
        {
        return;
        }

    // delete instance from the array
    iInstances->Delete( index );

    CHttpClientAppInstance* masterInstance = NULL;

    // find which client instance will inherit 
    // the downloads of the closed instance
    for( index = 0; index < iInstances->Count(); ++index )
        {
        if( (*iInstances)[index]->Master() )
            {
            masterInstance = (*iInstances)[index];
            break;
            }
        }
        
    // Detach instance from conn handler(s) associated with it.
    for( index = 0; index < iConnections->Count(); ++index )
        {
        if( (*iConnections)[index]->ClientAppInst() == aInstance )
            {
              (*iConnections)[index]->SetClientAppInst( masterInstance );
              
              if(!masterInstance && iInstances->Count() == 0)
              {
               DestroyConnHandler((*iConnections)[index]);
              }
              
              
            }
        }

    // detach download(s) from this instance
    for( index = 0; index < iDownloads->Count(); ++index )
        {
        if( aInstance == (*iDownloads)[index]->ClientAppInstance() ||
            aInstance == (*iDownloads)[index]->PDClientAppInstance() )
            {
            //Unregister the download in case of PDClientAppInstance. This was registered during attach
            if( (*iDownloads)[index]->DetachClientInstance( aInstance ) )
                {
                   continue;
                }
            if( !(*iDownloads)[index]->Pausable() )
                // non-pausable download always deleted
                {
                CLOG_WRITE_1( "Download is not pausable: [%d]", (*iDownloads)[index]->Id() );
                (*iDownloads)[index]->Delete( aInstance );
                // index(th) element was delete
                --index;
                }
            else
                {
                switch( aInstance->ExitAction() )
                    {
                    case EExitPause:
                        {
                        TRAP_IGNORE( (*iDownloads)[index]->PauseL() );
                        }
                    // flow through

                    case EExitNothing:
                        {
                        (*iDownloads)[index]->SetClientInstance( masterInstance );
                        }
                        break;


                    case EExitDelete:
                        {
                        CLOG_WRITE_1( "Deleting: [%d]", (*iDownloads)[index]->Id() );
                        (*iDownloads)[index]->Delete( aInstance );
                        // index(th) element was delete
                        --index;
                        }
                        break;

                    default:
                        break;
                    }
                }
            }
           
        }

    // delete the instance
    delete aInstance;
    }

// -----------------------------------------------------------------------------
// CHttpClientApp::CloseClientApp
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CHttpClientApp::CloseClientApp( TBool /*bStore*/ )
    {
    LOGGER_ENTERFN( "CloseClientApp" );

    if( iConnections )
        {
        iConnections->ResetAndDestroy();
        delete iConnections;
        iConnections = NULL;
        }

    if( iDownloads )
        {
          for (TInt i=0; i<iDownloads->Count(); i++)
		  {
          iDownloads->At(i)->Deref();
		  }
        delete iDownloads;
        iDownloads = NULL;
        }

    if( iInstances )
        {
        iInstances->ResetAndDestroy();
        delete iInstances;
        iInstances = NULL;
        }
    }

// -----------------------------------------------------------------------------
// CHttpClientApp::Instances
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CArrayPtrFlat<CHttpClientAppInstance>* CHttpClientApp::Instances() const
    {
    return iInstances;
    }

// -----------------------------------------------------------------------------
// CHttpClientApp::CreateNewDownloadL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CHttpDownload* 
    CHttpClientApp::CreateNewDownloadL( CHttpClientAppInstance* aClAppInstance,
                                        const TDesC8& aUrl )
    {
    if( !aUrl.Length() )
        // w/o url a download is meaningless
        {
        User::Leave( KErrArgument );
        }

    TInt downloadId = iEngine->NextDownloadId();

    CHttpDownload* newDl = CHttpDownload::NewL( aUrl,
                                                this, 
                                                downloadId,
                                                aClAppInstance );

    CleanupStack::PushL( newDl );
    RegisterDownloadL( newDl );
    CleanupStack::Pop( newDl );

    return newDl;
    }

// -----------------------------------------------------------------------------
// CHttpClientApp::AppUid
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUint32 CHttpClientApp::AppUid() const
    {
    return iAppUid;
    }

// -----------------------------------------------------------------------------
// CHttpClientApp::Downloads
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CArrayPtrFlat<CHttpDownload>* CHttpClientApp::Downloads() const
    {
    return iDownloads;
    }

// -----------------------------------------------------------------------------
// CHttpClientApp::Engine
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CHttpDownloadManagerServerEngine* CHttpClientApp::Engine() const
    {
    return iEngine;
    }

// -----------------------------------------------------------------------------
// CHttpClientApp::RegisterDownloadL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CHttpClientApp::RegisterDownloadL( CHttpDownload* aDownload )
    {
    iDownloads->AppendL( aDownload );
    aDownload->Ref();
    }

// -----------------------------------------------------------------------------
// CHttpClientApp::UnregisterDownload
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CHttpClientApp::UnregisterDownload( CHttpDownload* aDownload )
    {
    CLOG_WRITE_1("Unregister download: %d", aDownload->Id() );

    TInt index;
    for( index = 0; index < iDownloads->Count(); ++index )
        {
        if( (*iDownloads)[index] == aDownload )
            {
            iDownloads->Delete( index );
            aDownload->Deref();
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CHttpClientApp::DownloadsInMaster
//
// Counts how many (no media) downloads the Master instance has.
//
// -----------------------------------------------------------------------------
//
TInt32 CHttpClientApp::DownloadsInMaster( TBool aNoMediasOnly )
    {
    TInt32 counter( 0 );

    for( TInt i = 0; i < iDownloads->Count(); ++i )
        {
        if( (*iDownloads)[i]->ClientAppInstance()&& 
            (*iDownloads)[i]->ClientAppInstance()->Master() )
            {
            if( aNoMediasOnly && (*iDownloads)[i]->NoMedia() )
                // count only the aNoMediasOnly
                {
                ++counter;
                }
            else if( !aNoMediasOnly )
                // count every download
                {
                ++counter;
                }
            }
        }

    return counter;
    }

// -----------------------------------------------------------------------------
// CHttpClientApp::DestroyConnHandler
//
// Deletes given connhandler from the array and destroys the object itself.
// -----------------------------------------------------------------------------
//
void CHttpClientApp::DestroyConnHandler( CHttpConnHandler* aConnHandler )
    {
    CLOG_WRITE( "DestroyConnHandler" );

    __ASSERT_DEBUG( aConnHandler, DMPanic( KErrCorrupt ));

    for( TInt index = 0; index < iConnections->Count(); ++index )
        {
        if( (*iConnections)[index] == aConnHandler )
            {
            __ASSERT_DEBUG( !aConnHandler->ClientAppInst(), DMPanic( KErrCorrupt ));
            iConnections->Delete( index );
            delete aConnHandler;

            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CHttpClientApp::LoadClientInfoL
// 
// Download info and all the downloads are loaded here
// -----------------------------------------------------------------------------
//
void CHttpClientApp::LoadClientInfoL( const TDesC& /*aFolder*/ )
    {
    LoadDownloadsL();
    }

// -----------------------------------------------------------------------------
// CHttpClientApp::StoreClientInfoL
// 
// For further improvement if any download info has to be persisted
// -----------------------------------------------------------------------------
//
void CHttpClientApp::StoreClientInfoL()
    {
    }

// -----------------------------------------------------------------------------
// CHttpClientApp::LoadDownloadsL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CHttpClientApp::LoadDownloadsL()
    {
    TPath folder;

    Engine()->DownloadInfoFolder( this, folder );

    CDir* dirs = NULL;

    TUint mask = KEntryAttMatchMask | KEntryAttMatchExclude; 

    User::LeaveIfError( Engine()->Fs().GetDir( folder, 
                                        mask, 
                                        EAscending | EDirsLast, 
                                        dirs ) );
    if( dirs && dirs->Count() )
        // there are downloads for this client
        {
        TInt err;

        CleanupStack::PushL( dirs );
        for( TInt i = 0; i < dirs->Count(); ++i )
            {
            if( (*dirs)[i].IsDir() )
                // directories are sorted to the end of the array
                // if this entry is dir we finished
                {
                break;
                }

            TInt32 id = DownloadIdL( (*dirs)[i].iName );
            
            if ( IsDownloadAlreadyLoaded( id ) )
                continue;

            // Download is not assigned to any instance
            // Id comes from the filename
            CHttpDownload* newDl = NULL;
            TRAP( err, newDl = CHttpDownload::NewL( KNullDesC8(), this, id ) );
            if( err != KErrEof &&
                err != KErrCorrupt &&
                err != KErrNotFound && 
                err != KErrNotSupported )
                // no problem load info file and the download is valid
                // and it was pausable, or non-pausable but completed
                {
                RegisterDownloadL( newDl );
                }
            else
                {
                TBuf<KMaxPath> fileName;
                
                fileName.Format( KDownloadFilenameFormat, 
                                    &folder, 
                                    &(*dirs)[i].iName );

                if( newDl )
                    {
                    newDl->Delete(NULL);
                    delete newDl;
                    }
                    
                if( !err )
                    // delete unusable info file
                    {
                    iEngine->Fs().Delete( fileName );
                    }
                }
            }

        CleanupStack::Pop( dirs );
        }

    delete dirs;
    }

// -----------------------------------------------------------------------------
// CHttpClientApp::OutputFilenameL
// 
// Gets the download id from the give folder name
// -----------------------------------------------------------------------------
//
TInt32 CHttpClientApp::DownloadIdL( const TDesC& aFilename ) const
    {
    TLex temp( aFilename );
    TUint32 id;
    User::LeaveIfError( temp.Val( id, EDecimal ) );

    return id;
    }
    
// -----------------------------------------------------------------------------
// CHttpClientApp::IsDownloadAlreadyLoaded
// 
// Check whether the download is already loaded or not for the client
// -----------------------------------------------------------------------------
//
TBool CHttpClientApp::IsDownloadAlreadyLoaded( TInt32 aId ) const
    {
    
     for( TInt index = 0; index < iDownloads->Count(); ++index )
         {
         if( (*iDownloads)[index]->Id() == aId)
             {
        	 return ETrue;
             }
         }
     return EFalse;         
    }    
    
    

// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File  
