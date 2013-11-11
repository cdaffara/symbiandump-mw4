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
#include    "HttpClientAppInstance.h"
#include    "HttpClientApp.h"
#include    "HttpDownloadManagerServerEngine.h"
#include "HttpDownloadMgrLogger.h"

// EXTERNAL DATA STRUCTURES
//extern  ?external_data;

// EXTERNAL FUNCTION PROTOTYPES  
//extern ?external_function( ?arg_type,?arg_type );

// CONSTANTS
//const ?type ?constant_var = ?constant;

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
// CHttpClientAppInstance::CHttpClientAppInstance
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CHttpClientAppInstance::CHttpClientAppInstance( CHttpClientApp* aClientApp,
                                                CHttpConnHandler* aConnHandler,
                                                TBool aMaster,
                                                TInt aInstanceId,
                                                MDownloadStateObserver* aObserver )
    : iClientApp( aClientApp )
    , iInstanceId( aInstanceId )
    , iSilentMode( EFalse )
    , iExitAction( EExitNothing )
    , iConnHandler( aConnHandler )
    , iObserver( aObserver )
    , iMaster( aMaster )
    , iCookies( ETrue )
    , iAutoConnect( ETrue )
    , iFotaDownload( EFalse )
    {
    CLOG_CREATE;
    }

// -----------------------------------------------------------------------------
// CHttpClientAppInstance::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CHttpClientAppInstance::ConstructL()
    {
    CLOG_NAME_2( _L("CHttpClientAppInstance_%x_%x"), 
                    iClientApp->AppUid(), 
                    iInstanceId );
                    
    iProgressiveDownload = iClientApp->Engine()->ProgressiveDownloadFeature();
    }

// -----------------------------------------------------------------------------
// CHttpClientAppInstance::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CHttpClientAppInstance* 
        CHttpClientAppInstance::NewL( CHttpClientApp* aClientApp,
                                      CHttpConnHandler* aConnHandler,
                                      TBool aMaster,
                                      TInt aInstanceId,
                                      MDownloadStateObserver* aObserver  )
    {
    __ASSERT_DEBUG( aClientApp, DMPanic( KErrArgument ) );

    CHttpClientAppInstance* self = new( ELeave ) 
                                        CHttpClientAppInstance( aClientApp, 
                                                                aConnHandler,
                                                                aMaster,
                                                                aInstanceId,
                                                                aObserver  );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

    
// Destructor
CHttpClientAppInstance::~CHttpClientAppInstance()
    {
    CLOG_CLOSE;
    }

// -----------------------------------------------------------------------------
// CHttpClientAppInstance::SetObserver
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void 
        CHttpClientAppInstance::SetObserver( MDownloadStateObserver* aObserver )
    {
    iObserver = aObserver;
    }

// -----------------------------------------------------------------------------
// CHttpClientAppInstance::SearchDownloadWithUrl
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CHttpDownload* 
        CHttpClientAppInstance::SearchDownloadWithUrl( const TDesC8& /*aUrl*/ )
    {
    DMPanic( KErrNotSupported );
    
    return NULL;
    }

// -----------------------------------------------------------------------------
// CHttpClientAppInstance::CreateNewDownloadL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CHttpDownload* 
        CHttpClientAppInstance::CreateNewDownloadL( const TDesC8& aUrl )
    {
    return iClientApp->CreateNewDownloadL( this, aUrl );
    }

// -----------------------------------------------------------------------------
// CHttpClientAppInstance::AttachDownloadL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CHttpDownload* 
        CHttpClientAppInstance::AttachDownloadL( TInt32 aDownloadId )
    {
	// check if the download is attached already by other app.
	TBool isAttached = EFalse;
	isAttached = iClientApp->Engine()->IsAttachedAlready( aDownloadId );
	if (isAttached)
	{
		User::Leave(KErrInUse);
	}

    CHttpDownload* download = iClientApp->Engine()->FindDownload( aDownloadId );
	if( download)
	{
	download->Attach( this );
	iClientApp->RegisterDownloadL(download); 
	}
	

    return download;
    }

// -----------------------------------------------------------------------------
// CHttpClientAppInstance::DownloadsL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CArrayPtrFlat<CHttpDownload>* 
        CHttpClientAppInstance::DownloadsL() const
    {
    CArrayPtrFlat<CHttpDownload>* downloads = NULL;

    downloads = new (ELeave) CArrayPtrFlat<CHttpDownload>(2);
    CArrayPtrFlat<CHttpDownload>* allDownloads = iClientApp->Downloads();

    CleanupStack::PushL( downloads );
    for( TInt i = 0; i < allDownloads->Count(); ++i )
        {
        if( (*allDownloads)[i]->ClientAppInstance() == this )
            {
            downloads->AppendL( (*allDownloads)[i] );
            }
        }
    CleanupStack::Pop( downloads );

    return downloads;
    }

// -----------------------------------------------------------------------------
// CHttpClientAppInstance::AppUid
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TUint32 CHttpClientAppInstance::AppUid() const
    {
    return iClientApp->AppUid();
    }

// -----------------------------------------------------------------------------
// CHttpClientAppInstance::GetIntAttributeL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CHttpClientAppInstance::GetIntAttributeL( 
                                            THttpDownloadMgrAttrib aAttribute, 
							                TInt32& aValue )
    {
    TInt err( KErrNone );

    switch( aAttribute )
        {
        case EDlMgrAppUid:
            {
            aValue = iClientApp->AppUid();
            }
            break;

        case EDlMgrIap:
            {
            aValue = iConnHandler->IapId();
            }
            break;

        case EDlMgrExitAction:
            {
            aValue = iExitAction;
            }
            break;

        case EDlMgrAllDlsInMaster:
            {
            aValue = iClientApp->DownloadsInMaster( EFalse );
            }
            break;

        case EDlMgrNoMediaDlsInMaster:
            {
            aValue = iClientApp->DownloadsInMaster( ETrue );
            }
            break;

        case EDlMgrNumInprogressDownloads:
            {
            CArrayPtrFlat<CHttpDownload>* downloads = iClientApp->Downloads();
            aValue = 0;

            for( TInt i = 0; i < downloads->Count(); ++i )
                {
                if( (*downloads)[i]->State() == EHttpDlInprogress &&
                    (*downloads)[i]->ClientAppInstance() == this )
                    {
                    ++aValue;
                    }
                }
            }
            break;

        case EDlMgrNoMediaDls:
            {
            CArrayPtrFlat<CHttpDownload>* downloads = iClientApp->Downloads();
            aValue = 0;

            for( TInt i = 0; i < downloads->Count(); ++i )
                {
                if( (*downloads)[i]->ClientAppInstance() == this )
                    {
                    if( (*downloads)[i]->NoMedia() )
                        {
                        ++aValue;
                        }
                    }
                }
            }
            break;

        default:
#ifdef __WINS__
            DMPanic( KErrArgument );
#else
            err = KErrArgument;
#endif
            break;
        }

    User::LeaveIfError( err );
    }

// -----------------------------------------------------------------------------
// CHttpClientAppInstance::GetBoolAttributeL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CHttpClientAppInstance::GetBoolAttributeL( 
                                            THttpDownloadMgrAttrib aAttribute, 
								            TBool& aValue )
    {
    TInt err( KErrNone );

    switch( aAttribute )
        {
        case EDlMgrSilentMode:
            {
            aValue = iSilentMode;
            }
            break;

        case EDlMgrMaster:
            {
            aValue = iMaster;
            }
            break;

        case EDlMgrConnected:
            {
            aValue = iConnHandler->IsConnected();
            }
            break;
            
        case EDlMgrEnableCookies:
            {
            aValue = iCookies;
            }
            break;

        case EDlMgrAutoConnect:
            {
            aValue = iAutoConnect;
            }
            break;

        case EDlMgrFotaDownload:
            {
            aValue = iFotaDownload;
            }
            break;
            
        case EDlMgrProgressiveDownload:
            {
            aValue = iProgressiveDownload;
            }
            break;
            

        default:
#ifdef __WINS__
            DMPanic( KErrArgument );
#else
            err = KErrArgument;
#endif
            break;
        }

    User::LeaveIfError( err );
    }

// -----------------------------------------------------------------------------
// CHttpClientAppInstance::GetStringAttributeL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC* CHttpClientAppInstance::GetStringAttributeL( 
                                            THttpDownloadMgrAttrib aAttribute, 
								            TBool& aDelete )
    {
    aDelete = EFalse;

    switch( aAttribute )
        {
        case EDlMgrConnectionName:
            {
            return iConnHandler->ConnectionNameL( aDelete );
            }

        case EDlMgrAPName:
            {
//            iConnHandler->APNameL( aValue );
            }
            break;
            
        case EDlMgrCodFolder:
            {
            HBufC* codFolder = HBufC::NewL( KMaxPath );
            TPtr buffer( codFolder->Des() );
            
            iClientApp->Engine()->CodFolder( iClientApp, buffer );
            
            return codFolder;
            }

        default:
            {
#ifdef __WINS__
            DMPanic( KErrArgument );
#else
            User::Leave( KErrArgument );
#endif
            }
        }

    return NULL;
    }

// -----------------------------------------------------------------------------
// CHttpClientAppInstance::GetString8AttributeL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC8* CHttpClientAppInstance::GetString8AttributeL( 
                                            THttpDownloadMgrAttrib /*aAttribute*/, 
								            TBool& aDelete )
    {
    aDelete = EFalse;

#ifdef __WINS__
    DMPanic( KErrArgument );
#else
    User::LeaveIfError( KErrArgument );
#endif

    return NULL;
    }

// -----------------------------------------------------------------------------
// CHttpClientAppInstance::SetIntAttributeL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CHttpClientAppInstance::SetIntAttributeL( 
                                            THttpDownloadMgrAttrib aAttribute, 
                                            const TInt32 aValue )
    {
    TInt err( KErrNone );

    switch( aAttribute )
        {
        case EDlMgrIap:
            {
            iConnHandler->SetIapId( (TUint32)aValue );
            }
            break;

        case EDlMgrExitAction:
            {
            iExitAction = (THttpDownloadMgrExitAction)aValue;
            }
            break;

        default:
#ifdef __WINS__
            DMPanic( KErrArgument );
#else
            err = KErrArgument;
#endif
            break;
        }

    User::LeaveIfError( err );
    }

// -----------------------------------------------------------------------------
// CHttpClientAppInstance::SetBoolAttributeL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CHttpClientAppInstance::SetBoolAttributeL( 
                                            THttpDownloadMgrAttrib aAttribute, 
								            const TBool aValue )
    {
    TInt err( KErrNone );

    switch( aAttribute )
        {
        case EDlMgrSilentMode:
            {
            iSilentMode = aValue;
            }
            break;

        case EDlMgrEnableCookies:
            {
            iCookies = aValue;
            }
            break;

        case EDlMgrAutoConnect:
            {
            iAutoConnect = aValue;
            }
            break;
            
        case EDlMgrFotaDownload:
            {
            iFotaDownload = aValue;
            }
            break;
            
        default:
#ifdef __WINS__
            DMPanic( KErrArgument );
#else
            err = KErrArgument;
#endif
            break;
        }

    User::LeaveIfError( err );
    }

// -----------------------------------------------------------------------------
// CHttpClientAppInstance::SetStringAttributeL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CHttpClientAppInstance::SetStringAttributeL( 
                                            THttpDownloadMgrAttrib aAttribute, 
								                  const TDesC16& aValue )
    {
    switch( aAttribute )
        {
        case EDlMgrConnectionName:
            {
            iConnHandler->SetConnectionNameL( aValue );
            }
            break;

        default:
            {
#ifdef __WINS__
            DMPanic( KErrArgument );
#else
            User::LeaveIfError( KErrArgument );
#endif
            }
        }
    }

// -----------------------------------------------------------------------------
// CHttpClientAppInstance::SetStringAttributeL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CHttpClientAppInstance::SetStringAttributeL( 
                                            THttpDownloadMgrAttrib /*aAttribute*/, 
								            const TDesC8& /*aValue*/ )
    {
#ifdef __WINS__
    DMPanic( KErrArgument );
#else
    User::LeaveIfError( KErrArgument );
#endif
    }

// -----------------------------------------------------------------------------
// CHttpClientAppInstance::Disconnect
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CHttpClientAppInstance::Disconnect()
    {
    CArrayPtrFlat<CHttpDownload>* downloads = iClientApp->Downloads();

    for( TInt i = 0; i < downloads->Count(); ++i )
        {
        if( (*downloads)[i]->ClientAppInstance() == this )
            {
            // this won't surely leave on this call
            TRAP_IGNORE( (*downloads)[i]->PauseL() );
            }
        }

    iConnHandler->Disconnect( ETrue );
    // to forget the RConnection's name
    TRAP_IGNORE( iConnHandler->SetConnectionNameL( KNullDesC ) );
    }

// -----------------------------------------------------------------------------
// CHttpClientAppInstance::ClientApp
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CHttpClientApp* CHttpClientAppInstance::ClientApp() const
    {
    return iClientApp;
    }


// -----------------------------------------------------------------------------
// CHttpClientAppInstance::InstanceId
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CHttpClientAppInstance::InstanceId() const
    {
    return iInstanceId;
    }


// -----------------------------------------------------------------------------
// CHttpClientAppInstance::ConnHandler
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CHttpConnHandler* CHttpClientAppInstance::ConnHandler() const
    {
    return iConnHandler;
    }

// -----------------------------------------------------------------------------
// CHttpClientAppInstance::Observer
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
MDownloadStateObserver* CHttpClientAppInstance::Observer() const
    {
    return iObserver;
    }

// -----------------------------------------------------------------------------
// CHttpClientAppInstance::ExitAction
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
THttpDownloadMgrExitAction CHttpClientAppInstance::ExitAction() const
    {
    return iExitAction;
    }

// -----------------------------------------------------------------------------
// CHttpClientAppInstance::Master
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CHttpClientAppInstance::Master() const
    {
    return iMaster;
    }

// -----------------------------------------------------------------------------
// CHttpClientAppInstance::Cookies
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CHttpClientAppInstance::Cookies() const
    {
    return iCookies;
    }

// -----------------------------------------------------------------------------
// CHttpClientAppInstance::AutoConnect
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CHttpClientAppInstance::AutoConnect() const
    {
    return iAutoConnect;
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File  
