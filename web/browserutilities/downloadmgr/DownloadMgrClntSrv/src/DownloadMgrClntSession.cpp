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
*     This file contains the declaration of the Client interface of Download Mgr Server.
*
*/


// INCLUDE FILES
#include "DownloadMgrLogger.h"
#include "DownloadMgrClient.h"
#include "DownloadMgrServer.h"
#include "DownloadMgrStart.h"
#include "DownloadMgrDef.h"
#include "DownloadMgrDefAttrib.h"
#include "DownloadMgrHandler.h"
#include <e32svr.h>
#include <escapeutils.h>
#include <centralrepository.h>
#include "DownloadMgrCRKeys.h"
#include <BrowserUiSDKCRKeys.h>
#include <CodDownload.h>

// CONSTANTS
const TInt KHttpDownloadMgrObserverArrayGranularity = 4;
const TInt KHttpDownloadMgrDefalutAttribsGranularity = 4;
const TInt KDefaultMsgSlots = 16;

_LIT8( KHttpScheme, "http" );
_LIT8( KHttpsScheme, "https" );

// GLOBAL FUNCTIONS
#if defined(_DEBUG)
void DMPanic( TInt aPanicCode )
    {
    User::Panic( _L("DownloadManager"), aPanicCode );
    }
#endif
 
// ================= MEMBER FUNCTIONS =======================

/**
* Extension class.
*/
NONSHARABLE_CLASS( CRHttpDownloadMgrExtension ) : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CRHttpDownloadMgrExtension* NewL();
        
        /**
        * Destructor.
        */
        virtual ~CRHttpDownloadMgrExtension();
        
        /*
        * Next uri observer setter function
        */
        void SetNextUriObserver( MHttpDownloadMgrNextUriObserver* aObserver );

        /*
        * Next uri observer getter function
        */
        MHttpDownloadMgrNextUriObserver* NextUriObserver() const;


    protected: // Constructors

        /**
        * C++ default constructor.
        */
        CRHttpDownloadMgrExtension();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    public:      
        
        static TInt GetRepositoryValue( TUint32 aSettingId, TInt& aSettingValue );

        static TInt SetRepositoryValue( TUint32 aSettingId, TInt aSettingValue );

    public: // Data
        TInt iEventBroadcastLevel;
        
        MHttpDownloadMgrNextUriObserver* iNextUriObserver; ///< Not Owned.
  
        TInt iSessionId;
        
        TInt iAdjustEventPriorityFlag;
    };

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CRHttpDownloadMgrExtension::CRHttpDownloadMgrExtension
// -----------------------------------------------------------------------------
//
CRHttpDownloadMgrExtension::CRHttpDownloadMgrExtension()
:iEventBroadcastLevel( 0 )
,iAdjustEventPriorityFlag( 0 )
    {
    iNextUriObserver = NULL;
    }

// -----------------------------------------------------------------------------
// CRHttpDownloadMgrExtension::ConstructL
// -----------------------------------------------------------------------------
//
void CRHttpDownloadMgrExtension::ConstructL()
    {
    CLOG_ENTERFN("CRHttpDownloadMgrExtension::ConstructL");
    }

// -----------------------------------------------------------------------------
// CRHttpDownloadMgrExtension::NewL
// -----------------------------------------------------------------------------
//
CRHttpDownloadMgrExtension* CRHttpDownloadMgrExtension::NewL()
    {
    CRHttpDownloadMgrExtension* self = new(ELeave)CRHttpDownloadMgrExtension();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// Destructor
CRHttpDownloadMgrExtension::~CRHttpDownloadMgrExtension()
    {
    CLOG_ENTERFN("CRHttpDownloadMgrExtension::~CRHttpDownloadMgrExtension");
    }

// -----------------------------------------------------------------------------
// CRHttpDownloadMgrExtension::SetNextUriObserver
// -----------------------------------------------------------------------------
//
void CRHttpDownloadMgrExtension::SetNextUriObserver( MHttpDownloadMgrNextUriObserver* aObserver )
    {
    iNextUriObserver = aObserver;
    }

// -----------------------------------------------------------------------------
// CRHttpDownloadMgrExtension::NextUriObserver
// -----------------------------------------------------------------------------
//
MHttpDownloadMgrNextUriObserver* CRHttpDownloadMgrExtension::NextUriObserver() const
    {
    return iNextUriObserver;
    }


// -----------------------------------------------------------------------------
// CRHttpDownloadMgrExtension::GetRepositoryValue
// -----------------------------------------------------------------------------
//
TInt CRHttpDownloadMgrExtension::GetRepositoryValue( TUint32 aSettingId, TInt& aSettingValue )
    {
    aSettingValue = KErrNotFound;
    CRepository* rep = NULL;

    TRAPD( err, rep = CRepository::NewL( KCRUidBrowserUiLV ) );
    if( (err == KErrNone) && rep )
        {
        err = rep->Get( aSettingId, aSettingValue );
        }
    delete rep;
    return err;
    }

// -----------------------------------------------------------------------------
// CRHttpDownloadMgrExtension::SetRepositoryValue
// -----------------------------------------------------------------------------
//
TInt CRHttpDownloadMgrExtension::SetRepositoryValue( TUint32 aSettingId, TInt aSettingValue )
    {
    CRepository* rep = NULL;
   
    TRAPD( err, rep = CRepository::NewL( KCRUidBrowserUiLV ) );
    if( (err == KErrNone) && rep )
        {
        err = rep->Set( aSettingId, aSettingValue );
        }
    delete rep;
    return err;
    }

// ---------------------------------------------------------
// RHttpDownloadMgr::RHttpDownloadMgr
// ---------------------------------------------------------
//
EXPORT_C RHttpDownloadMgr::RHttpDownloadMgr()
	{
    // Reset data members as R-objects are not zero'd.
    iAppUid = TUid::Null();
    iDownloadArray = NULL;
    iObservers = NULL;
    iDefaultAttribs = NULL;
    iManagerHandler = NULL;
    iExtension = NULL;
	}

// ---------------------------------------------------------
// RHttpDownloadMgr::ConnectL
// ---------------------------------------------------------
//
EXPORT_C void RHttpDownloadMgr::ConnectL( TUid aAppUid,
                                          MHttpDownloadMgrObserver& aObserver,
                                          TBool aMaster )
    {
	CLOG_CREATE;
	
    __ASSERT_DEBUG( !Handle(), DMPanic( KErrAlreadyExists ) );
    
    TRAPD( err, DoConnectL( aAppUid, aObserver, aMaster ) );
    if( err )
        {
        Close();
        User::Leave( err );
        }
    }
    
// ---------------------------------------------------------
// RHttpDownloadMgr::DoConnectL
// ---------------------------------------------------------
//
void RHttpDownloadMgr::DoConnectL( TUid aAppUid,
                                   MHttpDownloadMgrObserver& aObserver,
                                   TBool aMaster )   
	{
    CLOG_ENTERFN( "RHttpDownloadMgr::ConnectL" );

    TInt error = KErrNone;
    RProcess server;
    error = server.Create( KDownloadMgrServerExe, TPtr( NULL, 0 ),
                           TUidType( KNullUid, KNullUid, KUidDownloadMgrSrvExe ) );
    CLOG_WRITE_FORMAT( "RHttpDownloadMgr::ConnectL after process creation, err %d", error );

    if( !error )
        {
        TRequestStatus status;
        server.Rendezvous( status );
        if( status != KRequestPending )
            {
            CLOG_WRITE( "RHttpDownloadMgr::ConnectL pending status error." );
            server.Kill( 0 );
            }
        else
            {
            CLOG_WRITE( "RHttpDownloadMgr::ConnectL pending resume server." );
            server.Resume( );
            }
        User::WaitForRequest( status );
        error = ( server.ExitType() == EExitPanic ) ? KErrGeneral : status.Int();
        server.Close();
        }
    User::LeaveIfError( error );
    CLOG_WRITE( "Creating..." );
    User::LeaveIfError(  CreateSession( KDownloadMgrServerName, Version(), KDefaultMsgSlots ) );
    CLOG_WRITE( "Created" );

    CLOG_WRITE( "Starting initialization" );
    InitializeL( aAppUid, aObserver, aMaster );
    CLOG_WRITE( "Init OK" );
	}

// ---------------------------------------------------------
// RHttpDownloadMgr::InitializeL
// ---------------------------------------------------------
//
void RHttpDownloadMgr::InitializeL( TUid aAppUid,
                                    MHttpDownloadMgrObserver& aObserver,
                                    TBool aMaster )
    {
    CLOG_ENTERFN( "RHttpDownloadMgr::InitializeL" )

    iAppUid = aAppUid;
    AddObserverL( aObserver );
    iExtension = CRHttpDownloadMgrExtension::NewL();
    CLOG_ATTACH( iExtension, this );
    iDownloadArray = new (ELeave) CArrayPtrFlat< RHttpDownload >( 1 );
    if( aMaster )
        {
        iManagerHandler = new (ELeave) CDownloadMgrHandler ( this );
        CLOG_ATTACH( iManagerHandler, this );
        iManagerHandler->StartL();
        }

    TPckg<TUid> uidPckg( iAppUid );
    TInt count( 0 );
    TPckg<TInt> pckgCount( count );
    TPckg<TInt> sessionId( iExtension->iSessionId );
    // 0st param - application UID
    // 1st param - the amount of downloads at server side belongs to the same UID
    // 2st param - master flag
    // 3rd param - session id

    User::LeaveIfError( SendReceive( EHttpDownloadMgrInitialize, 
                                     TIpcArgs( &uidPckg, &pckgCount, aMaster, 
                                     &sessionId ) ) );

    CLOG_NAME_2( _L("Session_%x_%x"), aAppUid.iUid, iExtension->iSessionId );

    CLOG_WRITE_FORMAT( "count: %d", count )
    // There are downloads at server side which needs to be attached
    if( count )
        {
        // Create a buffer which will contain the handles.
        TPckgBuf<TInt> arrayPckg;
        HBufC8* buf = HBufC8::NewLC( count*arrayPckg.Size() );
        // Get the handles from the server
        TPtr8 ptr = buf->Des();
        AttachL( ptr );
        // Create subsessions one by one.
        for( TInt i = 0; i < count; i++ )
            {
            RHttpDownload* download = new (ELeave) RHttpDownload ( this );
            CleanupStack::PushL( download );
            // Parse the handle from the buffer.
            arrayPckg.Copy( buf->Mid( i*arrayPckg.Size(), arrayPckg.Size() ) );
            // Attach subsession to the server by the handle.
            download->AttachL( arrayPckg() );
            TBool isCodDownload ( EFalse );
            download->GetBoolAttribute ( EDlAttrCodDownload , isCodDownload );
            if( isCodDownload )
            	{
            	//paused Download
            	download->InitPausedCodDownloadL( iAppUid );
            	
            	// Update CodData in Download Mgr Server
            	HBufC8* mediaInfo8 = NULL;
            	if(download->iCodDownload)
            	    {
            	    mediaInfo8 = download->iCodDownload->UpdatedDownloadDataL();
            	    }
    	    	if (mediaInfo8)
    	        	{
    	        	download->SetDownloadDataAttribute(*mediaInfo8);
    	        	delete mediaInfo8;
    	        	
    	        	// Buffer attributes in CRHttpDownloadExtension class
    	        	download->BufferAttributesL();
    	        	}
                User::LeaveIfError( download->GetProductDownloadedSize( ) );                	        	
            	}
            AddToArrayL( download );	
            CleanupStack::Pop( download ); //download
            }

        CleanupStack::PopAndDestroy( buf ); // buf
        }
    }

// ---------------------------------------------------------
// RHttpDownloadMgr::AttachL
// ---------------------------------------------------------
//
void RHttpDownloadMgr::AttachL( TDes8& aBuf )
    {
    CLOG_ENTERFN( "RHttpDownloadMgr::AttachL" )
    // 0st param - buffer will contain handles.
    // The handles idetifies the download subsessions.

    User::LeaveIfError( SendReceive( EHttpDownloadMgrAttach, 
                                     TIpcArgs( &aBuf ) ) );
    }
    
// ---------------------------------------------------------
// RHttpDownloadMgr::CloseDownload
// ---------------------------------------------------------
//
void RHttpDownloadMgr::CloseDownload( RHttpDownload* aElement )
    {
    CLOG_ENTERFN( "RHttpDownloadMgr::CloseDownload" );

    // As this class owns the array of subsessions, this role is 
    // to close them.
    RemoveFromArray( aElement );
    aElement->Close();
    delete aElement;
    aElement = NULL;
    }
    
// ---------------------------------------------------------
// RHttpDownloadMgr::Version
// ---------------------------------------------------------
//
EXPORT_C TVersion RHttpDownloadMgr::Version( void ) const
	{
	return TVersion( KDownloadMgrMajorVersionNumber,
                     KDownloadMgrMinorVersionNumber,
                     KDownloadMgrBuildVersionNumber );
	}


// ---------------------------------------------------------
// RHttpDownloadMgr::Close
// ---------------------------------------------------------
//
EXPORT_C void RHttpDownloadMgr::Close()
	{
    
 	if( !Handle() )
 	    {
 	    return;
 	    }

    CLOG_WRITE( "RHttpDownloadMgr::Close" )
    if( iDownloadArray )
        {
        TInt count = iDownloadArray->Count();
        while( count-- )
            {
            CLOG_WRITE_FORMAT( "Close download count[%d]", count );
		    RHttpDownload* download = (*iDownloadArray)[count];
		    CloseDownload( download );
            }
        iDownloadArray->Reset();
        delete iDownloadArray;
        iDownloadArray = NULL;
        }

    if( iExtension )
        {
        delete iExtension;
        iExtension = NULL;
        }

    if( iObservers )
        {
        iObservers->Reset();
        delete iObservers;
        iObservers = NULL;
        }

    if( iDefaultAttribs )
        {
        iDefaultAttribs->ResetAndDestroy();
        delete iDefaultAttribs;
        iDefaultAttribs = NULL;
        }

    if( iManagerHandler )
        {
        iManagerHandler->Cancel();
        delete iManagerHandler;
        iManagerHandler = NULL;
        }

	RHandleBase::Close();
	
	CLOG_CLOSE;
	}

// ---------------------------------------------------------
// RHttpDownloadMgr::AddObserverL
// ---------------------------------------------------------
//
EXPORT_C void RHttpDownloadMgr::AddObserverL( MHttpDownloadMgrObserver& aObserver )
    {
    __ASSERT_DEBUG( Handle(), DMPanic( KErrDisconnected ) );
    
    CLOG_ENTERFN( "RHttpDownloadMgr::AddObserverL" )

	if ( iObservers == NULL )
        {
		iObservers = new (ELeave) CDownloadMgrObserver( 
            KHttpDownloadMgrObserverArrayGranularity );
        }

    TInt index( 0 );
    if ( FindObserver( aObserver, index ) == KErrNone )
        {
        // Observer already added.
        }
    else
        {
	    iObservers->AppendL( &aObserver );
        }
    }

// ---------------------------------------------------------
// RHttpDownloadMgr::RemoveObserver
// ---------------------------------------------------------
//
EXPORT_C void RHttpDownloadMgr::RemoveObserver( MHttpDownloadMgrObserver& aObserver )
    {
    __ASSERT_DEBUG( Handle(), DMPanic( KErrDisconnected ) );
    
    CLOG_ENTERFN( "RHttpDownloadMgr::RemoveObserver" )

    if ( iObservers )
        {
        TInt index( 0 );
        if ( FindObserver( aObserver, index ) != KErrNone )
            {
            // Observer not found. Do nothing.
            }
        else
            {
            // Observer found.
            iObservers->Delete( index );
            // Free up memory.
            if ( iObservers->Count() == 0 )
                {
                delete iObservers;
                iObservers = NULL;
                }
            }
        }
    }

// ---------------------------------------------------------
// RHttpDownloadMgr::FindObserver
// ---------------------------------------------------------
//
TInt RHttpDownloadMgr::FindObserver( MHttpDownloadMgrObserver& aObserver, TInt& aIndex ) const
    {
    CLOG_ENTERFN( "RHttpDownloadMgr::FindObserver" )

    TInt ret( KErrNotFound );

    if ( iObservers == NULL )
        {
        ret = KErrNotFound;
        }
    else
        {
        MHttpDownloadMgrObserver* obs = &aObserver;
        TKeyArrayFix key( 0, ECmpTUint32 ); // Compare pointers
        ret = iObservers->Find( obs, key, aIndex );
        }

    return ret;
    }

// ---------------------------------------------------------
// RHttpDownloadMgr::CurrentDownloads
// ---------------------------------------------------------
//
EXPORT_C const CDownloadArray& RHttpDownloadMgr::CurrentDownloads() const
    {
    __ASSERT_DEBUG( Handle(), DMPanic( KErrDisconnected ) );
    
    CLOG_WRITE_FORMAT( "RHttpDownloadMgr::CurrentDownloads %d", iDownloadArray->Count() )
    return *iDownloadArray;
    }

// ---------------------------------------------------------
// RHttpDownloadMgr::CreateDownloadL
// ---------------------------------------------------------
//
EXPORT_C RHttpDownload& RHttpDownloadMgr::CreateDownloadL( const TDesC8& aUrl, 
                                                           TBool& aResult )
    {
    __ASSERT_DEBUG( Handle(), DMPanic( KErrDisconnected ) );
    
    CLOG_ENTERFN( "RHttpDownloadMgr::CreateDownloadL" )
    RHttpDownload* download = DoFindDownloadL( aUrl, KNullDesC8 );
    
    if( !download )
        {
        // there is no ongoing download with this URL
        // create one and add to the array
        aResult = ETrue;
        return CreateDownloadL( aUrl );
        }
    else
        {
        // we already have a download, just return a reference to it.
        aResult = EFalse;
        return *download;
        }
    }

// ---------------------------------------------------------
// RHttpDownloadMgr::CreateDownloadL
// ---------------------------------------------------------
//
EXPORT_C RHttpDownload& RHttpDownloadMgr::CreateDownloadL( const TDesC8& aUrl )
    {
    __ASSERT_DEBUG( Handle(), DMPanic( KErrDisconnected ) );
    
    CLOG_ENTERFN( "RHttpDownloadMgr::CreateDownloadL" )
    
    RHttpDownload* download = new (ELeave) RHttpDownload ( this );
    CleanupStack::PushL( download );
    CleanupClosePushL( *download );
    HBufC8* buf = PackedDefaultAttributesLC();
    download->CreateL( aUrl, buf->Des() );
    RHttpDownload& item = AddToArrayL( download );
    CleanupStack::PopAndDestroy( buf ); // buf
    CleanupStack::Pop( 2 );  // download->Close, download
   
    return item;
    }

// ---------------------------------------------------------
// RHttpDownloadMgr::CreateCodDownloadL
// ---------------------------------------------------------
//
EXPORT_C RHttpDownload& RHttpDownloadMgr::CreateCodDownloadL( const TDesC8& aUrl,
                                                              const TDesC8& aBuf,
                                                              const TDesC8& aMimeType,
                                                              CEikonEnv* aEikEnv,
                                                              TBool& aResult )
	{
	__ASSERT_DEBUG( Handle(), DMPanic( KErrDisconnected ) );
	
	CLOG_ENTERFN( "RHttpDownloadMgr::CreateCodDownloadL" )
	
	RHttpDownload& download = CreateDownloadL( aUrl, aResult );
	download.InitCodDownloadL( aBuf, aMimeType, aEikEnv );
	download.SetBoolAttribute( EDlAttrCodDownload, ETrue );
	
	return download;
	}

// ---------------------------------------------------------
// RHttpDownloadMgr::CreateClientSideDownloadL
// ---------------------------------------------------------
//
EXPORT_C RHttpDownload& RHttpDownloadMgr::CreateClientSideDownloadL( 
    TInt aHttpTransaction,TBool& aResult )
    {
    __ASSERT_DEBUG( Handle(), DMPanic( KErrDisconnected ) );
    
    RHTTPTransaction* httpTransaction = 
        REINTERPRET_CAST( RHTTPTransaction* , aHttpTransaction );
    RHttpDownload& download = 
    		CreateDownloadL( httpTransaction->Request().URI().UriDes() );
    		
    download.SetBoolAttribute( EDlAttrContinue, ETrue );
    download.SetHttpTransactionL( aHttpTransaction );

    // Set the disconnect notification
    httpTransaction->PropertySet().SetPropertyL
        (
        httpTransaction->Session().StringPool().StringF( HTTP::ENotifyOnDisconnect, RHTTPSession::GetTable() ), 
        httpTransaction->Session().StringPool().StringF( HTTP::EEnableDisconnectNotification, RHTTPSession::GetTable() )
        ); 

    aResult = ETrue; // download is created
        
    return download;
    }

// ---------------------------------------------------------
// RHttpDownloadMgr::FindDownload
// ---------------------------------------------------------
//
EXPORT_C RHttpDownload* RHttpDownloadMgr::FindDownload( const TDesC8& aUrl, 
                                                        const TDesC8& aMsgBody )
    {
    __ASSERT_DEBUG( Handle(), DMPanic( KErrDisconnected ) );
    
    CLOG_ENTERFN( "RHttpDownloadMgr::FindDownload" )
    RHttpDownload* retVal = NULL;
    
    TRAP_IGNORE( retVal = DoFindDownloadL( aUrl, aMsgBody) );

    return retVal;
    }

// ---------------------------------------------------------
// RHttpDownloadMgr::DownloadFromHandleL
// ---------------------------------------------------------
//
RHttpDownload& RHttpDownloadMgr::DownloadFromHandleL( TUint aHandle )
    {
    CLOG_ENTERFN( "RHttpDownloadMgr::DownloadFromHandleL" )
    // Check the download's handle in the array
    for( TInt i = 0; i < iDownloadArray->Count(); i++ )
        {
        if( (*iDownloadArray)[i]->IsDownload( aHandle ) )
            {
            return *(*iDownloadArray)[i];
            }
        }
    User::Leave( KErrNotFound );

    // Just let the compiler be satisfied
    return *(*iDownloadArray)[iDownloadArray->Count()];
    }

// ---------------------------------------------------------
// RHttpDownloadMgr::AddToArrayL
// ---------------------------------------------------------
//
RHttpDownload& RHttpDownloadMgr::AddToArrayL( RHttpDownload* aElement )
    {
    CLOG_ENTERFN( "RHttpDownloadMgr::AddToArrayL" );

    TInt index( 0 );
    TInt err = FindInArray( aElement, index );
    if ( !err )
        {
        // Already added. Do nothing, just return a reference to the 
        // element in the array.
        }
    else
        {
        TInt count = iDownloadArray->Count();
        // 'count' will be the index of the new appended element:
        index = count;
        iDownloadArray->AppendL( aElement );
        }

    return  *(*iDownloadArray)[index];
    }

// ---------------------------------------------------------
// RHttpDownloadMgr::RemoveFromArray
// ---------------------------------------------------------
//
void RHttpDownloadMgr::RemoveFromArray( RHttpDownload* aElement )
    {
    CLOG_ENTERFN( "RHttpDownloadMgr::RemoveFromArray" )

    TInt index( 0 );
    TInt err = FindInArray( aElement, index );
    if ( !err )
        {
        // Found. Remove from array:
        iDownloadArray->Delete( index );
        iDownloadArray->Compress();
        }
    }

// ---------------------------------------------------------
// RHttpDownloadMgr::FindInArray
// ---------------------------------------------------------
//
TInt RHttpDownloadMgr::FindInArray( RHttpDownload* aElement, TInt& aIndex ) const
    {
    CLOG_ENTERFN( "RHttpDownloadMgr::FindInArray" );
    
    //__ASSERT_DEBUG( iDownloadArray, Panic...)
    TKeyArrayFix key( 0, ECmpTUint32 ); // Compare pointers
    TInt ret = iDownloadArray->Find( aElement, key, aIndex );

    return ret;
    }

// ---------------------------------------------------------
// RHttpDownloadMgr::PauseAll
// ---------------------------------------------------------
//
EXPORT_C TInt RHttpDownloadMgr::PauseAll()
    {
    __ASSERT_DEBUG( Handle(), DMPanic( KErrDisconnected ) );
    
    CLOG_ENTERFN( "RHttpDownloadMgr::PauseAll" )

    for( TInt i = 0; i < iDownloadArray->Count(); i++ )
        {
        (*iDownloadArray)[i]->Pause();
        }

    return KErrNone;
    }

// ---------------------------------------------------------
// RHttpDownloadMgr::StartAll
// ---------------------------------------------------------
//
EXPORT_C TInt RHttpDownloadMgr::StartAll()
    {
    __ASSERT_DEBUG( Handle(), DMPanic( KErrDisconnected ) );
    
    CLOG_ENTERFN( "RHttpDownloadMgr::StartAllL" )

    for( TInt i = 0; i < iDownloadArray->Count(); i++ )
        {
        (*iDownloadArray)[i]->Start();
        }

    return KErrNone;
    }

// ---------------------------------------------------------
// RHttpDownloadMgr::ResetAll
// ---------------------------------------------------------
//
EXPORT_C TInt RHttpDownloadMgr::ResetAll()
    {
    __ASSERT_DEBUG( Handle(), DMPanic( KErrDisconnected ) );
    
    CLOG_ENTERFN( "RHttpDownloadMgr::ResetAll" )

    for( TInt i = 0; i < iDownloadArray->Count(); i++ )
        {
        (*iDownloadArray)[i]->Reset();
        }

    return KErrNone;
    }

// ---------------------------------------------------------
// RHttpDownloadMgr::DeleteAll
// ---------------------------------------------------------
//
EXPORT_C TInt RHttpDownloadMgr::DeleteAll()
    {
    __ASSERT_DEBUG( Handle(), DMPanic( KErrDisconnected ) );
    
    CLOG_ENTERFN( "RHttpDownloadMgr::DeleteAll" )

    TInt err( KErrNone );

    TInt i = iDownloadArray->Count();
    while( i )
        {
        TInt temp = (*iDownloadArray)[0]->Delete();
        if( temp )
            {
            err = temp;
            }
        --i;
        }

    return err;
    }

// ---------------------------------------------------------
// RHttpDownloadMgr::Disconnect
// ---------------------------------------------------------
//
EXPORT_C TInt RHttpDownloadMgr::Disconnect()
    {
    __ASSERT_DEBUG( Handle(), DMPanic( KErrDisconnected ) );
    
    CLOG_ENTERFN( "RHttpDownloadMgr::Disconnect" )

    // First pause all downloads.
    // It'd be difficult to pause client side downloads from
    // server side.
    for( TInt i = 0; i < iDownloadArray->Count(); i++ )
        {
        (*iDownloadArray)[i]->Pause();
        }

    return SendReceive( EHttpDownloadMgrDisconnect, TIpcArgs() );
    }

// ---------------------------------------------------------
// RHttpDownloadMgr::GetIntAttribute
// ---------------------------------------------------------
//
EXPORT_C TInt RHttpDownloadMgr::GetIntAttribute( const TUint aAttribute, 
                                                 TInt32& aValue )
    {
    switch ( aAttribute )
        {
        case EDlMgrHasActiveDownloads:
            {
            return CRHttpDownloadMgrExtension::GetRepositoryValue( KDownloadMgrHasActiveDownloads, (TInt&)aValue );
            }
        case EDlMgrAutoAcceptCod:
            {
            return CRHttpDownloadMgrExtension::GetRepositoryValue( KDownloadMgrAutoAcceptCod, (TInt&)aValue );
            }
        case EDlMgrNumOfClientSideDownload:
            {
            __ASSERT_DEBUG( Handle(), DMPanic( KErrDisconnected ) );
            return NumberOfClientSideDownload( aValue );
            }
        default:
            {
            __ASSERT_DEBUG( Handle(), DMPanic( KErrDisconnected ) );
            TPckg<TInt32> pckg( aValue );
            return SendReceive( EHttpDownloadMgrGetIntAttribute, 
                        TIpcArgs( aAttribute, &pckg ) );
            }
        }
    }

// ---------------------------------------------------------
// RHttpDownloadMgr::NumberOfClientSideDownload
// ---------------------------------------------------------
//
TInt RHttpDownloadMgr::NumberOfClientSideDownload( TInt32& aValue )
    {
    TInt err( KErrNone );

    aValue = 0;
    for( TInt i = 0; i < iDownloadArray->Count(); i++ )
        {
        if( (*iDownloadArray)[i]->IsClientSideDownload() )
            {
            aValue++;
            }
        }
    
    return err;
    }

// ---------------------------------------------------------
// RHttpDownloadMgr::GetBoolAttribute
// ---------------------------------------------------------
//
EXPORT_C TInt RHttpDownloadMgr::GetBoolAttribute( const TUint aAttribute, 
                                                  TBool& aValue )
    {
    __ASSERT_DEBUG( Handle(), DMPanic( KErrDisconnected ) );
    
    CLOG_ENTERFN( "RHttpDownloadMgr::GetIntAttribute" )

    TPckg<TBool> pckg( aValue );

    return SendReceive( EHttpDownloadMgrGetBoolAttribute, 
                        TIpcArgs( aAttribute, &pckg ) );
    }

// ---------------------------------------------------------
// RHttpDownloadMgr::GetStringAttributeL
// ---------------------------------------------------------
//
EXPORT_C TInt RHttpDownloadMgr::GetStringAttribute( const TUint aAttribute, 
                                                    TDes16& aValue  )
    {
    __ASSERT_DEBUG( Handle(), DMPanic( KErrDisconnected ) );
    
    CLOG_ENTERFN( "RHttpDownloadMgr::GetIntAttribute" )

    return SendReceive( EHttpDownloadMgrGetStringAttribute, 
                        TIpcArgs( aAttribute, &aValue ) );
    }

// ---------------------------------------------------------
// RHttpDownloadMgr::GetStringAttribute
// ---------------------------------------------------------
//
EXPORT_C TInt RHttpDownloadMgr::GetStringAttribute( const TUint aAttribute, 
                                                    TDes8& aValue  )
    {
    __ASSERT_DEBUG( Handle(), DMPanic( KErrDisconnected ) );
    
    CLOG_ENTERFN( "RHttpDownloadMgr::GetIntAttribute" )

    return SendReceive( EHttpDownloadMgrGetString8Attribute, 
                        TIpcArgs( aAttribute, &aValue ) );
    }

// ---------------------------------------------------------
// RHttpDownloadMgr::SetIntAttributeL
// ---------------------------------------------------------
//
EXPORT_C TInt RHttpDownloadMgr::SetIntAttribute( const TUint aAttribute, 
                                                 TInt32 aValue )
    {
    if( aAttribute == EDlMgrHasActiveDownloads )
        {
        return CRHttpDownloadMgrExtension::SetRepositoryValue(
                KDownloadMgrHasActiveDownloads, (TInt&)aValue );
        }
    else
        {
        __ASSERT_DEBUG( Handle(), DMPanic( KErrDisconnected ) );
        return SendReceive( EHttpDownloadMgrSetIntAttribute, 
                            TIpcArgs( aAttribute, aValue ) );
        }
    }

// ---------------------------------------------------------
// RHttpDownloadMgr::SetBoolAttributeL
// ---------------------------------------------------------
//
EXPORT_C TInt RHttpDownloadMgr::SetBoolAttribute( const TUint aAttribute, 
                                                  TBool aValue )
    {
    __ASSERT_DEBUG( Handle(), DMPanic( KErrDisconnected ) );
    
    CLOG_ENTERFN( "RHttpDownloadMgr::SetBoolAttributeL" )

    return SendReceive( EHttpDownloadMgrSetBoolAttribute, 
                        TIpcArgs( aAttribute, aValue ) );
    }

// ---------------------------------------------------------
// RHttpDownloadMgr::SetStringAttributeL
// ---------------------------------------------------------
//
EXPORT_C TInt RHttpDownloadMgr::SetStringAttribute( const TUint aAttribute, 
                                                    const TDesC16& aValue )
    {
    __ASSERT_DEBUG( Handle(), DMPanic( KErrDisconnected ) );
    
    CLOG_ENTERFN( "RHttpDownloadMgr::SetStringAttributeL" )

    return SendReceive( EHttpDownloadMgrSetStringAttribute, 
                        TIpcArgs( aAttribute, &aValue ) );
    }

// ---------------------------------------------------------
// RHttpDownloadMgr::SetStringAttribute
// ---------------------------------------------------------
//
EXPORT_C TInt RHttpDownloadMgr::SetStringAttribute( const TUint aAttribute, 
                                                    const TDesC8& aValue )
    {
    __ASSERT_DEBUG( Handle(), DMPanic( KErrDisconnected ) );
    
    CLOG_ENTERFN( "RHttpDownloadMgr::SetStringAttributeL" )

    return SendReceive( EHttpDownloadMgrSetString8Attribute, 
                        TIpcArgs( aAttribute, &aValue ) );
    }

// ---------------------------------------------------------
// RHttpDownloadMgr::SetDefaultIntAttribute
// ---------------------------------------------------------
//
EXPORT_C TInt RHttpDownloadMgr::SetDefaultIntAttribute( const TUint aAttribute, 
                                                        TInt32 aValue )
    {
    __ASSERT_DEBUG( Handle(), DMPanic( KErrDisconnected ) );
    
    CLOG_ENTERFN( "RHttpDownloadMgr::SetDefaultIntAttributeL" )

    TRAPD( err, DoSetDefaultAttributeL( aAttribute, aValue ) );

    return err;
    }

// ---------------------------------------------------------
// RHttpDownloadMgr::SetDefaultBoolAttribute
// ---------------------------------------------------------
//
EXPORT_C TInt RHttpDownloadMgr::SetDefaultBoolAttribute( const TUint aAttribute, 
                                                         TBool aValue )
    {
    __ASSERT_DEBUG( Handle(), DMPanic( KErrDisconnected ) );
    
    CLOG_ENTERFN( "RHttpDownloadMgr::SetDefaultBoolAttribute" )

    TRAPD( err, DoSetDefaultAttributeL( aAttribute, aValue ) );

    return err;
    }

// ---------------------------------------------------------
// RHttpDownloadMgr::SetDefaultStringAttribute
// ---------------------------------------------------------
//
EXPORT_C TInt RHttpDownloadMgr::SetDefaultStringAttribute( const TUint aAttribute, 
                                                           const TDesC16& aValue )
    {
    __ASSERT_DEBUG( Handle(), DMPanic( KErrDisconnected ) );
    
    CLOG_ENTERFN( "RHttpDownloadMgr::SetDefaultBoolAttribute" )

    TRAPD( err, DoSetDefaultAttributeL( aAttribute, aValue ) );

    return err;
    }

// ---------------------------------------------------------
// RHttpDownloadMgr::SetDefaultStringAttribute
// ---------------------------------------------------------
//
EXPORT_C TInt RHttpDownloadMgr::SetDefaultStringAttribute( const TUint aAttribute, 
                                                           const TDesC8& aValue )
    {
    __ASSERT_DEBUG( Handle(), DMPanic( KErrDisconnected ) );
    
    CLOG_ENTERFN( "RHttpDownloadMgr::SetDefaultBoolAttribute" )

    TRAPD( err, DoSetDefaultAttributeL( aAttribute, aValue ) );

    return err;
    }

// ---------------------------------------------------------
// RHttpDownloadMgr::SetNextUriObserver
// ---------------------------------------------------------
//
EXPORT_C void RHttpDownloadMgr::SetNextUriObserver( MHttpDownloadMgrNextUriObserver* aObserver )
    {
    __ASSERT_DEBUG( Handle(), DMPanic( KErrDisconnected ) );
    
    iExtension->SetNextUriObserver( aObserver );
    }

// ---------------------------------------------------------
// RHttpDownloadMgr::NextUriObserver
// ---------------------------------------------------------
//
MHttpDownloadMgrNextUriObserver* RHttpDownloadMgr::NextUriObserver() const
    {
    return iExtension->iNextUriObserver;
    }

// ---------------------------------------------------------
// RHttpDownloadMgr::SessionId
// ---------------------------------------------------------
//
TInt RHttpDownloadMgr::SessionId() const
    {
    return iExtension->iSessionId;
    }

// ---------------------------------------------------------
// RHttpDownloadMgr::DoSetStringAttributeL
// ---------------------------------------------------------
//
void RHttpDownloadMgr::DoSetDefaultAttributeL( const TUint aAttribute, 
                                               TBool aValue )
    {
    CLOG_ENTERFN( "RHttpDownloadMgr::SetDefaultBoolAttribute" )

    if( iDefaultAttribs == NULL )
        {
		iDefaultAttribs = new (ELeave) CArrayPtrFlat< CDefaultAttrib >( 
            KHttpDownloadMgrDefalutAttribsGranularity );
        }
    CDefaultAttrib* attrib = CDefaultAttrib::NewL( (THttpDownloadAttrib)aAttribute,
                                                   aValue );
    iDefaultAttribs->AppendL( attrib );
    }

// ---------------------------------------------------------
// RHttpDownloadMgr::DoSetStringAttributeL
// ---------------------------------------------------------
//
void RHttpDownloadMgr::DoSetDefaultAttributeL( const TUint aAttribute, 
                                               TInt32 aValue )
    {
    CLOG_ENTERFN( "RHttpDownloadMgr::SetDefaultBoolAttribute" )

    if( iDefaultAttribs == NULL )
        {
		iDefaultAttribs = new (ELeave) CArrayPtrFlat< CDefaultAttrib >( 
            KHttpDownloadMgrDefalutAttribsGranularity );
        }
    CDefaultAttrib* attrib = CDefaultAttrib::NewL( (THttpDownloadAttrib)aAttribute,
                                                   aValue );
    iDefaultAttribs->AppendL( attrib );
    }

// ---------------------------------------------------------
// RHttpDownloadMgr::DoSetStringAttributeL
// ---------------------------------------------------------
//
void RHttpDownloadMgr::DoSetDefaultAttributeL( const TUint aAttribute, 
                                               const TDesC8& aValue )
    {
    CLOG_ENTERFN( "RHttpDownloadMgr::SetDefaultBoolAttribute" )

    if( iDefaultAttribs == NULL )
        {
		iDefaultAttribs = new (ELeave) CArrayPtrFlat< CDefaultAttrib >( 
            KHttpDownloadMgrDefalutAttribsGranularity );
        }
    CDefaultAttrib* attrib = CDefaultAttrib::NewL( (THttpDownloadAttrib)aAttribute,
                                                   aValue );
    iDefaultAttribs->AppendL( attrib );
    }

// ---------------------------------------------------------
// RHttpDownloadMgr::DoSetStringAttributeL
// ---------------------------------------------------------
//
void RHttpDownloadMgr::DoSetDefaultAttributeL( const TUint aAttribute, 
                                               const TDesC16& aValue )
    {
    CLOG_ENTERFN( "RHttpDownloadMgr::SetDefaultBoolAttribute" )

    if( iDefaultAttribs == NULL )
        {
		iDefaultAttribs = new (ELeave) CArrayPtrFlat< CDefaultAttrib >( 
            KHttpDownloadMgrDefalutAttribsGranularity );
        }
    CDefaultAttrib* attrib = CDefaultAttrib::NewL( (THttpDownloadAttrib)aAttribute,
                                                   aValue );
    iDefaultAttribs->AppendL( attrib );
    }

// ---------------------------------------------------------
// RHttpDownloadMgr::HandleDMgrEventL
// ---------------------------------------------------------
//

void RHttpDownloadMgr::HandleDMgrEventL( RHttpDownload& aDownload, 
                                         THttpDownloadEvent aEvent )
    {
    CLOG_ENTERFN( "RHttpDownloadMgr::HandleDMgrEventL" )
    iExtension->iEventBroadcastLevel++;
    CLOG_WRITE_FORMAT( "aDownload = %x", &aDownload );
    CLOG_WRITE_FORMAT( "Inc iEventBroadcastLevel = %d", iExtension->iEventBroadcastLevel );
    // In case of EHttpDlDeleted getting most of the attributes are forbidden,
    // because the engine is most likely is not ready to serve! Some of the
    // attributes are buffered in client side.
    aDownload.SetEventType( aEvent.iDownloadState );

    if( (( EHttpDlInprogress == aEvent.iDownloadState ) && ( EHttpProgCodDescriptorDownloaded == aEvent.iProgressState ))||
        (( EHttpDlInprogress == aEvent.iDownloadState ) && ( EHttpProgCodDownloadShouldResume == aEvent.iProgressState ))||
        (( EHttpDlPaused == aEvent.iDownloadState ) && ( EHttpProgCodDescriptorDownloaded == aEvent.iProgressState ))
      )
        {
        TBool isCod( ETrue );

        if(EHttpProgCodDownloadShouldResume == aEvent.iProgressState)
        	{
        	//resume only for apps with non-zero uid. otherwise the resume will happen from music player which sets the appuid as zero.
        	TInt32 clientAppUid(0); 
            GetIntAttribute(EDlMgrAppUid , clientAppUid);         
            if( !clientAppUid )
                {
                iExtension->iEventBroadcastLevel--;
                return;
                }
        	}
        	
        else
            {
            TRAPD( err, isCod = aDownload.CheckContentTypeAndCreateCodDownloadL() );
            if( isCod )
                {
                if( err )
                    {
                    // The error code is processed by the server
                    aDownload.SetOnError( err );
                    iExtension->iEventBroadcastLevel--;
                    return;                
                    }
                }                
                
            }
            TRAPD( err, aDownload.DoStartCodL() );
            if( err )
                {
                aDownload.SetOnError( err );
                iExtension->iEventBroadcastLevel--;
                return; 
                }              
  
        }
    else if( EHttpDlCancelTransaction == aEvent.iDownloadState )
        {
        aDownload.DeleteTransaction();
        }     
           
    else if( EHttpProgCodDownloadPause == aEvent.iProgressState )
        {
        if(aDownload.iCodDownload)
            {
            aDownload.iCodDownload->Pause();
            }
        }
    else if( EHttpDlProgProgressive == aEvent.iProgressState )
        {
        if(aDownload.iCodDownload)
            {
            aDownload.iCodDownload->SetCodDlAttached(ETrue);
            }
        }        
    else if( EHttpDlProgNonProgressive == aEvent.iProgressState )
        {
        if(aDownload.iCodDownload)
            {
            aDownload.iCodDownload->SetCodDlAttached(EFalse);
            }
        }        

    // TODO: make sure that all COD test cases works OK

    // ...and close the download.
    if( EHttpDlDeleted == aEvent.iDownloadState )
        {
        CloseDownload( &aDownload );
        }        
        
    iExtension->iEventBroadcastLevel--;

    // When a download completes, close the subsession to make a message slot free.
    // Also the required information will be cached before closing the subsession
    if( EHttpDlMultipleMOCompleted == aEvent.iDownloadState  && 
    		( 
    		EHttpProgContentFileMoved == aEvent.iProgressState ||
    		EHttpProgContentFileMovedAndDestFNChanged == aEvent.iProgressState 
    		)
    	)
    	{
    		//Store all the info clinet side in RHttpDownload's extension API
    		aDownload.SaveDownloadInfoL();
    		//Close the subsession to free up the message slot
    		aDownload.CloseSubSession();
    	}    
	TInt count = iObservers->Count();
    for( TInt i = 0; i < count; i++ )
        {
        CLOG_WRITE_FORMAT( "HandleDMgrEventL( aDownload = 0x%x)", this );
        TRAP_IGNORE( (*iObservers)[i]->HandleDMgrEventL( aDownload, aEvent ) );
        if(!iObservers)break;
        
        }
    
    //CLOG_WRITE_FORMAT( "Dec iEventBroadcastLevel = %d", iExtension->iEventBroadcastLevel );
    }

// ---------------------------------------------------------
// RHttpDownloadMgr::BroadcastEvent
// ---------------------------------------------------------
//
void RHttpDownloadMgr::BroadcastEvent( RHttpDownload& aDownload, 
                                       THttpDownloadEvent aEvent )
    {
    CLOG_WRITE_3( "BroadcastEvent: dl [%x], DownloadState [%d], ProgressState [%d]", this, aEvent.iDownloadState, aEvent.iProgressState );
    for( TInt i = 0; i < iObservers->Count(); i++ )
        {        
        TRAP_IGNORE( (*iObservers)[i]->HandleDMgrEventL( aDownload, aEvent ) );
        // The download shouldn't be deleted in nested loops!!!
        if( 1 == iExtension->iEventBroadcastLevel )
            {
            if( aDownload.IsToBeDeleted() )
                {
                // Decremented before delete because we check the
                // value in aDownload.Delete();
                iExtension->iEventBroadcastLevel--;
                CLOG_WRITE_FORMAT( "Dec iEventBroadcastLevel = %d", iExtension->iEventBroadcastLevel );
                aDownload.Delete();
                iExtension->iEventBroadcastLevel++;
                return;
                }
            }
        }    
    }

// ---------------------------------------------------------
// RHttpDownloadMgr::HandleSessionEventL
// ---------------------------------------------------------
//
void RHttpDownloadMgr::HandleSessionEventL( TInt aHandle, THttpDownloadEvent aEvent )
    {
    TRAP_IGNORE( DoHandleSessionEventL( aHandle, aEvent ) );
    }

// ---------------------------------------------------------
// RHttpDownloadMgr::DoHandleSessionEventL
// ---------------------------------------------------------
//
void RHttpDownloadMgr::DoHandleSessionEventL( TInt aHandle, THttpDownloadEvent aEvent )
    {
    RHttpDownload* download = new (ELeave) RHttpDownload ( this );
    CleanupStack::PushL( download );
    CleanupClosePushL( *download );
    // Attach subsession to the server by the handle.
    download->AttachL( aHandle );
    AddToArrayL( download );
    CleanupStack::Pop( download ); //download
    // Forward the event to the client
    HandleDMgrEventL( *download, aEvent );
    }

// ---------------------------------------------------------
// RHttpDownloadMgr::StartSessionEventNotification
// ---------------------------------------------------------
//
void RHttpDownloadMgr::StartSessionEventNotification( const TDesC8& aDownloadState,
                                                      const TDesC8& aProgressState,
                                                      const TDesC8& aHandle,
                                                      TRequestStatus& aStatus )
    {
    CLOG_ENTERFN( "RHttpDownload::StartSessionEventNotification" )

    SendReceive( EHttpDownloadMgrSessionEventSubscription, 
                 TIpcArgs( &aDownloadState, &aProgressState, &aHandle ), 
                 aStatus );
    }

// ---------------------------------------------------------
// RHttpDownloadMgr::CancelSessionEventNotification
// ---------------------------------------------------------
//
void RHttpDownloadMgr::CancelSessionEventNotification()
    {
    CLOG_WRITE( "RHttpDownload::CancelSessionEventNotification" )

    SendReceive( EHttpDownloadMgrSessionEventCancel, TIpcArgs() );
    }

// ---------------------------------------------------------
// RHttpDownloadMgr::PackedDefaultAttributesLC
// ---------------------------------------------------------
//
HBufC8* RHttpDownloadMgr::PackedDefaultAttributesLC()
    {
    HBufC8* buf = NULL;
    if( iDefaultAttribs )
        {
        
        buf = TDMgrUtils::PackedAttributesL( iDefaultAttribs );
        CleanupStack::PushL( buf );
        }
    else
        {
        buf = HBufC8::NewLC( 0 );
        }

    return buf;
    }

// ---------------------------------------------------------
// RHttpDownloadMgr::DoFindDownloadL
// ---------------------------------------------------------
//
RHttpDownload* RHttpDownloadMgr::DoFindDownloadL( const TDesC8& aUrl, const TDesC8& aMsgBody )
    {
    RHttpDownload* retVal = NULL;
    TInt pushCounter( 0 );

    HBufC8* escaped = EscapeUtils::EscapeDecodeL( aUrl );
    CleanupStack::PushL( escaped ); ++pushCounter;

    TUriParser8 compUri; 
    compUri.Parse( *escaped );

    TPtrC8 scheme( compUri.Extract( EUriScheme ) );

    CUri8* url = CUri8::NewLC(); ++pushCounter;

    if( scheme.Length() )
        {
        if( scheme != KHttpScheme &&
            scheme != KHttpsScheme )
            // this url has an unsupported scheme
            {
            User::Leave( KErrArgument );
            }
        else
            {
            url->SetComponentL( scheme, EUriScheme );
            }
        }
    else
        {
        url->SetComponentL( KHttpScheme, EUriScheme );
        }
   
    url->SetComponentL( compUri.Extract( EUriHost ), EUriHost );
    url->SetComponentL( compUri.Extract( EUriPath ), EUriPath );
    url->SetComponentL( compUri.Extract( EUriQuery ), EUriQuery );
    url->SetComponentL( compUri.Extract( EUriFragment ), EUriFragment );
    
    if( compUri.IsPresent( EUriPort ) )
        {
        url->SetComponentL( compUri.Extract( EUriPort ), EUriPort );
        }
    else
        {
        TBuf8<10> port;

        port.Format( _L8("%d"), KDefaultPort );

        url->SetComponentL( port, EUriPort );
        }
   
    TBuf8<KHashLength> hash_req;
    HashL( aMsgBody, hash_req );
    
    TUriParser8 parsed_req;
    
    parsed_req.Parse( url->Uri().UriDes() );

    for( TInt i = 0; i < iDownloadArray->Count(); i++ )
        {
        if( (*iDownloadArray)[i]->IsDownloadL( parsed_req, hash_req ) )
            {
            retVal = (*iDownloadArray)[i];
            break;
            }
        }    
        
    CleanupStack::PopAndDestroy( pushCounter );

    return retVal;
    }

// ---------------------------------------------------------
// RHttpDownloadMgr::IsEventBroadcasting
// ---------------------------------------------------------
//
TBool RHttpDownloadMgr::IsEventBroadcasting()
    {
    CLOG_WRITE( "RHttpDownload::IsEventBroadcasting" )
    return (0 < iExtension->iEventBroadcastLevel);
    }
    
// ---------------------------------------------------------
// RHttpDownloadMgr::IsEventBroadcasting
// ---------------------------------------------------------
//       
TInt RHttpDownloadMgr::EventPriorityFlag() const
    {
    return iExtension->iAdjustEventPriorityFlag;
    }
        
// ---------------------------------------------------------
// RHttpDownloadMgr::IncrementEventPriorityFlag
// ---------------------------------------------------------
//       
void RHttpDownloadMgr::IncrementEventPriorityFlag()
    {
    iExtension->iAdjustEventPriorityFlag++;
    }
        
// ---------------------------------------------------------
// RHttpDownloadMgr::IncrementEventPriorityFlag
// ---------------------------------------------------------
//       
void RHttpDownloadMgr::DecrementEventPriorityFlag()
    {
    iExtension->iAdjustEventPriorityFlag--;
    }
