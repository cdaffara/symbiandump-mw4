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
*     This file contains the declaration of the Download Mgr Server.
*
*/


// INCLUDE FILES
#include "DownloadMgrLogger.h"
#include "DownloadMgrServer.h"
#include "DownloadMgrDef.h"
#include "DownloadMgrEventQueue.h"

#include <HttpClientAppInstance.h>
#include <HttpDownload.h>
#include <e32svr.h>
#include <basched.h>

// CONSTANTS

// GLOBAL FUNCTIONS

// ---------------------------------------------------------
// CDownloadMgrSession::NewL
// ---------------------------------------------------------
//
CDownloadMgrSession* CDownloadMgrSession::NewL( CDownloadMgrServer* aServer )
	{
    CLOG_ENTERFN_NULL( "CDownloadMgrSession::NewL" )

    CDownloadMgrSession* self = new (ELeave) CDownloadMgrSession();
	CleanupStack::PushL( self );
	self->ConstructL( aServer );
	CleanupStack::Pop( self ); // self

	return self;
	}

// ---------------------------------------------------------
// CDownloadMgrSession::CDownloadMgrSession
// ---------------------------------------------------------
//
CDownloadMgrSession::CDownloadMgrSession()
: iComplete( ETrue )
,iIsMaster( EFalse )
	{
	__DECLARE_NAME( _S( "CDownloadMgrSession" ) );
	
	CLOG_CREATE;
	}


// ---------------------------------------------------------
// CDownloadMgrSession::CDownloadMgrSession
// ---------------------------------------------------------
//
void CDownloadMgrSession::ConstructL( CDownloadMgrServer* aServer )
	{
    CLOG_ENTERFN( "CDownloadMgrSession::ConstructL" )

    iSessionId = aServer->NextSessionId();
    
	// second-phase construct base class
    //CSession2::CreateL();
	iDownloadMgrServer = aServer;
	// create new object index
	iObjectIx = CObjectIx::NewL();
	// initialize the object container using the object container index in the server.
	iObjectContainer = iDownloadMgrServer->NewContainerL();
    // Increment the serves's session count and cancel shutdown timer
    iDownloadMgrServer->IcrementSessions();
    iEventQueue = new (ELeave) CDownloadMgrEventQueue();
    CLOG_ATTACH( iEventQueue, this );
	}

// ---------------------------------------------------------
// CDownloadMgrSession::~CDownloadMgrSession
// ---------------------------------------------------------
//
CDownloadMgrSession::~CDownloadMgrSession() 
	{
    CLOG_WRITE_FORMAT( "CDownloadMgrSession::~CDownloadMgrSession iClientAppInstance %d", iClientAppInstance );
    if( iDownloadMgrServer )
        {
	    CLOG_WRITE_FORMAT( "Number of sessions: %d", iDownloadMgrServer->SessionsCount() );
        }
    TRAP_IGNORE( PauseAllL() );
    if( iDownloadMgrServer )
        {
        iDownloadMgrServer->CloseClientInstance( iClientAppInstance );
        // Decrement the servers's session count and start shutdown timer if nedded
        iDownloadMgrServer->DecrementSessions();
        }
	//delete the object index (this stores the objects for this session)
    //it calls Close() on all reference counting objects in the index.
	delete iObjectIx;
    iObjectIx = NULL;
    if( iDownloadMgrServer )
        {
        // Remove and delete the object container
        iDownloadMgrServer->RemoveContainer( iObjectContainer );
        }
    delete iEventQueue;
    iEventQueue = NULL;
    
    CLOG_CLOSE;
	}


// ---------------------------------------------------------
// CDownloadMgrSession::CounterFromHandle
// ---------------------------------------------------------
//
CDownloadSubSession* CDownloadMgrSession::DownloadFromHandle(TUint aHandle)
    {
    CLOG_ENTERFN( "CDownloadMgrSession::DownloadFromHandle" )
	CDownloadSubSession* download = ( CDownloadSubSession* )iObjectIx->At( aHandle );
	if( download == NULL )
        {
		PanicClient( EBadSubsessionHandle ); 
        }
	return download;
    }


// ---------------------------------------------------------
// CDownloadMgrSession::CounterFromHandle
// ---------------------------------------------------------
//
void CDownloadMgrSession::ServiceL( const RMessage2& aMessage )
	{
    CLOG_ENTERFN( "CDownloadMgrSession::ServiceL" )
	TRAPD( err, DispatchMessageL( aMessage ) );
    if( iComplete || ( KErrNone != err ) )
        {
        // Only the syncron requests has to be completed here!
	    aMessage.Complete( err );
        }
	}


// ---------------------------------------------------------
// CDownloadMgrSession::DispatchMessageL
// ---------------------------------------------------------
//
void CDownloadMgrSession::DispatchMessageL( const RMessage2& aMessage )
	{
    CLOG_ENTERFN( "CDownloadMgrSession::DispatchMessageL" )
    CLOG_WRITE_FORMAT( "Message function: %d", aMessage.Function() )
    // Set this false in case of the asyncron requests!!!
    iComplete = ETrue;
    SetCurrentMessage( aMessage );
    
    // Fixed for Bug id - JJUN-78WCJ3 (DownloadMgrServer crashes under IPC attack)
    //This check will make sure that before start of any download, InitializeL() should
    // be called so that all the member variables will be initialized.
    if(aMessage.Function() != EHttpDownloadMgrInitialize && iClientAppInstance == NULL)
        {
        User::Leave(KErrGeneral);
        }
        
    // check for session-relative requests
	switch( aMessage.Function() )
		{
        case EHttpDownloadMgrInitialize:
            {
            InitializeL();
            return;
            }
	    case EHttpDownloadMgrCreateDownload:
            {
		    NewDownloadL();
            return;
            }
        case EHttpDownloadMgrAttach:
            {
            DownloadMgrAttachL();
            return;
            }
        case EHttpDownloadAttach:
            {
            DownloadAttachL();
            return;
            }
		case EHttpDownloadAttachToDownload:
		    {
		    AttachToDownloadL();
		    return;
		    }
        case EHttpDownloadMgrPauseAll:
            {
            PauseAllL();
            return;
            }
        case EHttpDownloadMgrStartAllL:
            {
            StartAllL();
            return;
            }
        case EHttpDownloadMgrResetAll:
            {
            ResetAll();
            return;
            }
        case EHttpDownloadMgrDeleteAll:
            {
            DeleteAll();
            return;
            }
        case EHttpDownloadMgrDisconnect:
            {
            Disconnect();
            return;
            }
        case EHttpDownloadMgrGetIntAttribute:
            {
            GetIntAttributeL();
            return;
            }
        case EHttpDownloadMgrGetBoolAttribute:
            {
            GetBoolAttributeL();
            return;
            }
        case EHttpDownloadMgrGetStringAttribute:
            {
            GetStringAttributeL();
            return;
            }
        case EHttpDownloadMgrGetString8Attribute:
            {
            GetString8AttributeL();
            return;
            }
        case EHttpDownloadMgrSetIntAttribute:
            {
            SetIntAttributeL();
            return;
            }
        case EHttpDownloadMgrSetBoolAttribute:
            {
            SetBoolAttributeL();
            return;
            }
        case EHttpDownloadMgrSetStringAttribute:
            {
            SetStringAttributeL();
            return;
            }
        case EHttpDownloadMgrSetString8Attribute:
            {
            SetString8AttributeL();
            return;
            }
        case EHttpDownloadMgrSessionEventSubscription:
            {
            InitSessionEvent( aMessage );
            // Send event from the queue
            TUint dummy( 0 );
            Event( NULL, (THttpDownloadState)dummy, (THttpProgressState)dummy, dummy );
            // This is an asyncron request will be completed later!
            SetComplete( EFalse );
            return;
            }
        case EHttpDownloadMgrSessionEventCancel:
            {
            CancelMovedEvent();
            return;
            }
		}
	// ok, it must be subsession relative
	CDownloadSubSession* download = DownloadFromHandle( aMessage.Int3() );
    __ASSERT_DEBUG( download, PanicClient( EBadSubsessionHandle ) );

    TRAPD( err, download->DispatchMessageL( aMessage ) );

    if( err )
        {
        download->OnError( err );
        User::Leave( err );
        }
	}

// ---------------------------------------------------------
// CDownloadMgrSession::SetCurrentMessage
// ---------------------------------------------------------
//
void CDownloadMgrSession::SetCurrentMessage( const RMessage2& aMessage )
    {
    iCurrentMessage = aMessage;
    }

// ---------------------------------------------------------
// CDownloadMgrSession::CurrentMessage
// ---------------------------------------------------------
//
const RMessage2& CDownloadMgrSession::CurrentMessage() const
    {
    return iCurrentMessage;
    }

// ---------------------------------------------------------
// CDownloadMgrSession::InitializeL
// ---------------------------------------------------------
//
void CDownloadMgrSession::InitializeL()
    {
    CLOG_ENTERFN( "CDownloadMgrSession::DownloadMgrAttachCountL" )
    iIsMaster = (TBool)CurrentMessage().Int2();

    TUid uid;
    TPckg<TUid> uidPckg( uid );

    Read( 0, uidPckg );

    CreateClientAppInstanceL( uidPckg().iUid );

    // Check how many download we have and return it to the client
    CLOG_WRITE_FORMAT( "CDownloadMgrSession::InitializeL iClientAppInstance %d", iClientAppInstance );
    CArrayPtrFlat<CHttpDownload>* currentDownloads = iClientAppInstance->DownloadsL();
	TPckg<TInt> countPckg( currentDownloads->Count() );
	TPckg<TInt> sessionId( iSessionId );

    Write( 1, CurrentMessage(), countPckg );
    Write( 3, CurrentMessage(), sessionId );

    CLOG_NAME_2( _L("Session_%x_%x"), uidPckg().iUid, iSessionId );

    currentDownloads->Reset();
    delete currentDownloads;
    }

// ---------------------------------------------------------
// CDownloadMgrSession::DownloadMgrAttachL
// ---------------------------------------------------------
//
void CDownloadMgrSession::DownloadMgrAttachL()
    {
    CLOG_ENTERFN( "CDownloadMgrSession::DownloadMgrAttachL" )
    // The client would like to attach the dowmloads were created previously.
    // Here, we create a buffer with the unique handles of download subsessions
    // and copy it to the client's address space.
    CLOG_WRITE_FORMAT( "CDownloadMgrSession::DownloadMgrAttachL iClientAppInstance %d", iClientAppInstance );
    CArrayPtrFlat<CHttpDownload>* currentDownloads = iClientAppInstance->DownloadsL();
    CleanupStack::PushL( currentDownloads );
    TPckgBuf<TInt> arrayPckg;
    HBufC8* buf = HBufC8::NewLC( currentDownloads->Count() * arrayPckg.Size() );
    // Cerate subsessions one by one and write the unique handles to the client.
    // Later the handles is used by the client subsession to attach to the server.
    for( TInt i = 0; i < currentDownloads->Count(); i++ )
        {
        CHttpDownload* httpDownload = (*currentDownloads)[i];
	    // make a new download object
	    CDownloadSubSession* download = CDownloadSubSession::NewL( this, httpDownload );
        CleanupStack::PushL( download );
	    // add object to object container to gererate unique id
	    iObjectContainer->AddL( download );
	    // add object to object index; this returns a unique handle so we can get it again
        CLOG_WRITE_FORMAT( "CDownloadMgrSession::DownloadMgrAttachL, download %d", download );
	    arrayPckg() = iObjectIx->AddL( download );
        CLOG_WRITE_FORMAT( "CDownloadMgrSession::DownloadMgrAttachL, iObjectIx->ActiveCount() %d", iObjectIx->ActiveCount() );
        // Store handle to the subsession. It is used when the URL is checked.
        download->SetHandle( arrayPckg() );
        CleanupStack::Pop( download ); //download
        buf->Des().Append( arrayPckg );
        }
    TPtr8 ptr = buf->Des();

    Write( 0, CurrentMessage(), ptr );

    CleanupStack::PopAndDestroy( buf ); // buf
    currentDownloads->Reset();
    CleanupStack::PopAndDestroy( currentDownloads ); // currentDownloads
    }

// ---------------------------------------------------------
// CDownloadMgrSession::DownloadAttachL
// ---------------------------------------------------------
//
void CDownloadMgrSession::DownloadAttachL()
	{
    CLOG_ENTERFN( "CDownloadMgrSession::DownloadAttachL" )
    // All we have to do to write the handle to the 3rd parameter.
    // The client-server framework will take care of the 3rd parameter
    // whenever the client calls a subsession related service.
    TInt handle = CurrentMessage().Int1();
    TPckg<TInt> handlePckg( handle );
    WriteHandle( handlePckg() );
	}

// ---------------------------------------------------------
// CDownloadMgrSession::AttachToDownloadL
// ---------------------------------------------------------
//
void CDownloadMgrSession::AttachToDownloadL()
    {
    CLOG_ENTERFN( "CDownloadMgrSession::AttachToDownload" )
    __ASSERT_DEBUG( iClientAppInstance, PanicClient( EMultipleInitDownloadEvent ) );

    const TInt32 id = CurrentMessage().Int0();

    CHttpDownload* dl = iClientAppInstance->AttachDownloadL( id );
		if (dl)
		{
			CreateDownloadL( dl );
		}
		else
		{
			User::LeaveIfError(KErrNotFound);
		}
    }

// ---------------------------------------------------------
// CDownloadMgrSession::CreateDownloadL
// ---------------------------------------------------------
//
void CDownloadMgrSession::CreateDownloadL( CHttpDownload* aDownload, TBool aDefAttrib )
	{
	// make a new download object
	CDownloadSubSession* download = CDownloadSubSession::NewL( this, aDownload );
    CleanupStack::PushL( download );
    if( aDefAttrib )
        {
        download->SetDefAttributesL();
        }
	// add object to object container to gererate unique id
	iObjectContainer->AddL( download );
	// add object to object index; this returns a unique handle so we can get it again
    CLOG_WRITE_FORMAT( "CDownloadMgrSession::DownloadMgrAttachL, download %d", download );
	TInt handle = iObjectIx->AddL( download );
    CLOG_WRITE_FORMAT( "CDownloadMgrSession::DownloadMgrAttachL, iObjectIx->ActiveCount() %d", iObjectIx->ActiveCount() );
    // Store handle to the subsession. It is used when the URL is checked.
    download->SetHandle( handle );
    CleanupStack::Pop( download ); //download
	// write the handle to client
    TPckg<TInt> handlePckg( handle );

    Write( 1, CurrentMessage(), handlePckg );

    // write the handle to the framework
	WriteHandle( handle );	
	}

// ---------------------------------------------------------
// CDownloadMgrSession::CreateClientAppInstanceL
// ---------------------------------------------------------
//
void CDownloadMgrSession::CreateClientAppInstanceL( TUint32 aAppUid )
    {
    CLOG_ENTERFN( "CDownloadMgrSession::CreateClientAppInstanceL" )
    if( !iClientAppInstance )
        {
        CLOG_ENTERFN( "CDownloadMgrSession::CreateClientAppInstanceL REALLY!!!" )

        iClientAppInstance = iDownloadMgrServer->CreateNewClientAppInstanceL(
            aAppUid, *this, iIsMaster );
        CLOG_WRITE_FORMAT( "CDownloadMgrSession::CreateClientAppInstanceL iClientAppInstance %d", iClientAppInstance );
        }
    }

// ---------------------------------------------------------
// CDownloadMgrSession::WriteHandle
// ---------------------------------------------------------
//
void CDownloadMgrSession::WriteHandle( TInt aHandle )
    {
    CLOG_ENTERFN( "CDownloadMgrSession::WriteHandle" )
	TPckg<TInt> handlePckg( aHandle );

    TInt res = CurrentMessage().Write( 3, handlePckg );

	if( res != KErrNone )
		{
		iObjectIx->Remove( aHandle );
		PanicClient( EBadDescriptor );
		return;
		}
    }

// ---------------------------------------------------------
// CDownloadMgrSession::NewDownloadL
// ---------------------------------------------------------
//
void CDownloadMgrSession::NewDownloadL()
	{
    CLOG_ENTERFN( "CDownloadMgrSession::NewDownloadL" )

    TInt desLen = CurrentMessage().GetDesLength( 0 );

	HBufC8* urlBuf = HBufC8::NewLC( desLen );
	TPtr8 urlptr = urlBuf->Des();

    Read( 0, urlptr );

    CLOG_WRITE_FORMAT( "CDownloadMgrSession::NewDownloadL iClientAppInstance %d", iClientAppInstance );
    CHttpDownload* httpDownload = 
        iClientAppInstance->CreateNewDownloadL( urlptr );
    iClientAppInstance->SetObserver( this );
    CleanupStack::PopAndDestroy( urlBuf ); // urlBuf
    
    CreateDownloadL( httpDownload, ETrue );
	}

// ---------------------------------------------------------
// CDownloadMgrSession::CreateSubSessionAndSendEventL
// ---------------------------------------------------------
//
TBool CDownloadMgrSession::CreateSubSessionAndSendEventL( CHttpDownload* aDownload,
                                                          TInt32 aDownloadState,
                                                          TInt32 aProgressState,
                                                          TInt32 aMoIndex )
    {
    CLOG_ENTERFN( "CDownloadMgrSession::CreateSubSessionAndSendEventL" )
	// make a new download object
	CDownloadSubSession* download = CDownloadSubSession::NewL( this, aDownload );
    CleanupStack::PushL( download );
	// add object to object container to gererate unique id
	iObjectContainer->AddL( download );
	// add object to object index; this returns a unique handle so we can get it again
    CLOG_WRITE_FORMAT( "CDownloadMgrSession::DownloadMgrAttachL, download %d", download );
	TInt handle = iObjectIx->AddL( download );
    CLOG_WRITE_FORMAT( "CDownloadMgrSession::DownloadMgrAttachL, iObjectIx->ActiveCount() %d", iObjectIx->ActiveCount() );
    // Store handle to the subsession. It is used when the URL is checked.
    download->SetHandle( handle );
    CleanupStack::Pop( download ); //download

    TBool retVal( iEvent );
    if( iEvent )
        {
        TPckg<TInt32> downloadStatePckg( aDownloadState );
        TPckg<TInt32> progressStatePckg( aProgressState );
        
	    // Mask media object index with handle.
	    // For example if aMoIndex = 0x00000004 and handle = 0x00000005,
	    // then maskData = 0x00040005.
	    TInt32 maskedData = ((aMoIndex << (sizeof(TInt32)/2)) | (handle & 0x0000FFFF));
        TPckg<TInt> maskPckg( maskedData );

        Write( 0, iEventMessage, downloadStatePckg );
        Write( 1, iEventMessage, progressStatePckg );
        Write( 2, iEventMessage, maskPckg );

        EventComplete( KErrNone );
        }
    CLOG_WRITE_FORMAT( "Session event sent: %d", retVal )
    return retVal;
    }

// ---------------------------------------------------------
// CDownloadMgrSession::EventComplete
// ---------------------------------------------------------
//
void CDownloadMgrSession::EventComplete( TInt aErrorStatus )
    {
    if( iEvent )
        {
        iEventMessage.Complete( aErrorStatus );
        iEvent = EFalse;
        }
    }

// ---------------------------------------------------------
// CDownloadMgrSession::DeleteDownload
// ---------------------------------------------------------
//
void CDownloadMgrSession::DeleteDownload( TUint aHandle )
	{
    CLOG_ENTERFN( "CDownloadMgrSession::DeleteDownload" )
	// panic if bad handle
	CDownloadSubSession* download = ( CDownloadSubSession* )iObjectIx->At( aHandle );
    CLOG_WRITE_FORMAT( "CDownloadMgrSession::DeleteDownload, download %d", download );
	if( download == NULL )
        {
		PanicClient( EBadCounterRemove );
        }
    //Removes the reference counting object identified by handle number 
    //from this object index and closes it.
    iObjectIx->Remove( aHandle );
	}

// ---------------------------------------------------------
// CDownloadMgrSession::PanicClient
// ---------------------------------------------------------
//
void CDownloadMgrSession::PanicClient( TInt aPanic ) const
	{
    CLOG_ENTERFN( "CDownloadMgrSession::PanicClient" )
    CLOG_WRITE_FORMAT( " aPanic=%d", aPanic )
	_LIT( KTxtDownloadMgrSession,"CDlMgrSession");

    CurrentMessage().Panic( KTxtDownloadMgrSession, aPanic );
	}

/// ---------------------------------------------------------
// CDownloadMgrSession::PauseAllL
// ---------------------------------------------------------
//
void CDownloadMgrSession::PauseAllL()
    {
    CLOG_ENTERFN( "CDownloadMgrSession::PauseAllL" )
    for( TInt i = 0; i < iObjectIx->ActiveCount(); i++ )
        {
        CDownloadSubSession* downloadSess 
            = ( CDownloadSubSession* )(*iObjectIx)[i];
        if( downloadSess == NULL )
            {
            // Already removed.
            }
        else
            {
            downloadSess->PauseL();
            }
        }
    }

/// ---------------------------------------------------------
// CDownloadMgrSession::StartAllL
// ---------------------------------------------------------
//
void CDownloadMgrSession::StartAllL()
    {
    CLOG_ENTERFN( "CDownloadMgrSession::StartAllL" )
    for( TInt i = 0; i < iObjectIx->Count(); i++ )
        {
        CDownloadSubSession* downloadSess 
            = ( CDownloadSubSession* )(*iObjectIx)[i];
        if( downloadSess == NULL )
            {
            // Already removed.
            }
        else
            {
            downloadSess->StartL();
            }
        }
    }

/// ---------------------------------------------------------
// CDownloadMgrSession::ResetAll
// ---------------------------------------------------------
//
void CDownloadMgrSession::ResetAll()
    {
    CLOG_ENTERFN( "CDownloadMgrSession::CancelAll" )
    for( TInt i = 0; i < iObjectIx->ActiveCount(); i++ )
        {
        CDownloadSubSession* downloadSess 
            = ( CDownloadSubSession* )(*iObjectIx)[i];
        if( downloadSess == NULL )
            {
            // Already removed.
            }
        else
            {
            downloadSess->Reset();
            }
        }
    }

/// ---------------------------------------------------------
// CDownloadMgrSession::DeleteAll
// ---------------------------------------------------------
//
void CDownloadMgrSession::DeleteAll()
    {
    CLOG_ENTERFN( "CDownloadMgrSession::DeleteAll" )
    for( TInt i = 0; i < iObjectIx->ActiveCount(); i++ )
        {
        CDownloadSubSession* downloadSess 
            = ( CDownloadSubSession* )(*iObjectIx)[i];
        if( downloadSess == NULL )
            {
            // Already removed.
            }
        else
            {
            downloadSess->Delete();
            }
        }
    }

/// ---------------------------------------------------------
// CDownloadMgrSession::Disconnect
// ---------------------------------------------------------
//
void CDownloadMgrSession::Disconnect()
    {
    CLOG_WRITE_FORMAT( "CDownloadMgrSession::Disconnect iClientAppInstance %d", iClientAppInstance );
    iClientAppInstance->Disconnect();
    }

// ---------------------------------------------------------
// CDownloadMgrSession::Write
// ---------------------------------------------------------
//
void CDownloadMgrSession::Write( TInt aParam, RMessage2 aMessage, TDesC8& aDes )
	{
    CLOG_ENTERFN( "CDownloadMgrSession::Write" )

    TInt ret = aMessage.Write( aParam, aDes );

	if( ret != KErrNone )
        {
		PanicClient( EBadDescriptor );
        }
	}

// ---------------------------------------------------------
// CDownloadMgrSession::Write
// ---------------------------------------------------------
//
void CDownloadMgrSession::Write( TInt aParam, RMessage2 aMessage, TDesC16& aDes )
	{
    CLOG_ENTERFN( "CDownloadMgrSession::Write" )

    TInt ret = aMessage.Write( aParam, aDes );

	if( ret != KErrNone )
        {
		PanicClient( EBadDescriptor );
        }
	}

// ---------------------------------------------------------
// CDownloadMgrSession::Read
// ---------------------------------------------------------
//
void CDownloadMgrSession::Read( TInt aParam, TPtr8& aDes )
	{
    CLOG_ENTERFN( "CDownloadMgrSession::Read" )

    TInt ret = CurrentMessage().Read( aParam, aDes );

	if( ret != KErrNone )
        {
		PanicClient( EBadDescriptor );
        }
	}

// ---------------------------------------------------------
// CDownloadMgrSession::Read
// ---------------------------------------------------------
//
void CDownloadMgrSession::Read( TInt aParam, TPtr16& aDes )
	{
    CLOG_ENTERFN( "CDownloadMgrSession::Read" )

    TInt ret = CurrentMessage().Read( aParam, aDes );

	if( ret != KErrNone )
        {
		PanicClient( EBadDescriptor );
        }
	}

// ---------------------------------------------------------
// CDownloadMgrSession::Event
// ---------------------------------------------------------
//
void CDownloadMgrSession::Event( CHttpDownload* aDownload, 
                                 THttpDownloadState aDlState,
                                 THttpProgressState aProgState,
                                 TInt32 aMoIndex )
    {
    CLOG_ENTERFN( "CDownloadMgrSession::Event" )
    TBool not_sent( ETrue );
    // First we add the download to the queue then send the next event.
    // aDownload could be NULL, this case we send the next event.
    // If aDownload == NULL the state parameters are ignored. (They don't hold information)
    if( aDownload )
        {
        if( !iEventQueue->In( aDownload, aDlState, aProgState, aMoIndex ) )
            {
            // OOM
            return;
            }
        }
    // Then we send an event from the queue
    if( !iEventQueue->IsEmpty() )
        {
        TInt32 downloadState( 0 );
        TInt32 progressState( 0 );
        TInt32 mediaIndex( 0 );

        CHttpDownload* download = iEventQueue->Next( downloadState, progressState, mediaIndex );
        for( TInt i = 0; i < iObjectIx->Count(); i++ )
            {
            CLOG_WRITE_FORMAT( "CDownloadMgrSession::Event, i %d", i );
            CDownloadSubSession* downloadSess 
                = ( CDownloadSubSession* )(*iObjectIx)[i];
            CLOG_WRITE_FORMAT( "CDownloadMgrSession::Event, downloadSess %d", downloadSess );
            if( downloadSess == NULL )
                {
                // Already removed.
                }
            else if ( downloadSess->IsDownload( download ) )
                {
                CLOG_WRITE_FORMAT( "Download: 0x%x", download )
                CLOG_WRITE_FORMAT( "State: %d", downloadState )
                CLOG_WRITE_FORMAT( "Progress: %d",progressState )
                CLOG_WRITE_FORMAT( "Media Index: %d",mediaIndex )
                
                TBool isSend( EFalse );
                TRAP_IGNORE( isSend = downloadSess->EventL( downloadState, progressState, mediaIndex ) );
                if( isSend )
                    {
                    // the event was sent, it can be removed from the queue
                    iEventQueue->Out();
                    not_sent = EFalse;
                    }
                }
            }

        // Then we must deal with the moved event 
        // if it has not been sent and the event handler is initialized
        if( (downloadState == EHttpDlMoved) && not_sent )
            {
            TBool retVal( EFalse );
            TRAPD( err, retVal = CreateSubSessionAndSendEventL( aDownload, downloadState, progressState, mediaIndex ) );
            
            if( err )
            	{
            	// Do't care, never leave
            	}

            if ( !retVal )
                {
                //OOM
                return;
                }
            else
                {
                // the event was sent, it can be removed from the queue
                iEventQueue->Out();
                not_sent = EFalse;
                }
            }
        }
    }

// ---------------------------------------------------------
// CDownloadMgrSession::SetComplete
// ---------------------------------------------------------
//
void CDownloadMgrSession::SetComplete( TBool aComplete )
    {
    iComplete = aComplete;
    }

// ---------------------------------------------------------
// CDownloadMgrSession::GetIntAttributeL
// ---------------------------------------------------------
//
void CDownloadMgrSession::GetIntAttributeL()
    {
    CLOG_ENTERFN( "CDownloadMgrSession::GetIntAttributeL" )

    const TInt attr = CurrentMessage().Int0();
    TInt32 value;

    CLOG_WRITE_FORMAT( "CDownloadMgrSession::GetIntAttributeL iClientAppInstance %d", iClientAppInstance );
    iClientAppInstance->GetIntAttributeL( (THttpDownloadMgrAttrib)attr, value );

    TPckg<TInt32> pckg( value );

    Write( 1, CurrentMessage(), pckg );
    }

// ---------------------------------------------------------
// CDownloadMgrSession::GetBoolAttributeL
// ---------------------------------------------------------
//
void CDownloadMgrSession::GetBoolAttributeL()
    {
    CLOG_ENTERFN( "CDownloadMgrSession::GetBoolAttributeL" )

    const TInt attr = CurrentMessage().Int0();
    TBool value;

    CLOG_WRITE_FORMAT( "CDownloadMgrSession::GetBoolAttributeL iClientAppInstance %d", iClientAppInstance );
    iClientAppInstance->GetBoolAttributeL( (THttpDownloadMgrAttrib)attr, value );

    TPckg<TBool> pckg( value );

    Write( 1, CurrentMessage(), pckg );
    }

// ---------------------------------------------------------
// CDownloadMgrSession::GetStringAttributeL
// ---------------------------------------------------------
//
void CDownloadMgrSession::GetStringAttributeL()
    {
    CLOG_ENTERFN( "CDownloadMgrSession::GetStringAttributeL" )
    const TInt attr = CurrentMessage().Int0();

    CLOG_WRITE_FORMAT( "CDownloadMgrSession::GetStringAttributeL iClientAppInstance %d", iClientAppInstance );

    TBool deleteStr;
    HBufC* attrStr = iClientAppInstance->GetStringAttributeL( 
                                            (THttpDownloadMgrAttrib)attr,
                                            deleteStr );
    TPtr value( attrStr->Des() );

    Write( 1, CurrentMessage(), value );

    if( deleteStr )
        {
        delete attrStr;
        }
    }

// ---------------------------------------------------------
// CDownloadMgrSession::GetStringAttributeL
// ---------------------------------------------------------
//
void CDownloadMgrSession::GetString8AttributeL()
    {
    CLOG_ENTERFN( "CDownloadMgrSession::GetString8AttributeL" )
    const TInt attr = CurrentMessage().Int0();

    CLOG_WRITE_FORMAT( "CDownloadMgrSession::GetStringAttributeL iClientAppInstance %d", iClientAppInstance );

    TBool deleteStr;
    HBufC8* attrStr = iClientAppInstance->GetString8AttributeL( 
                                            (THttpDownloadMgrAttrib)attr,
                                            deleteStr );
    TPtr8 value( attrStr->Des() );

    Write( 1, CurrentMessage(), value );

    if( deleteStr )
        {
        delete attrStr;
        }
    }

// ---------------------------------------------------------
// CDownloadMgrSession::SetIntAttributeL
// ---------------------------------------------------------
//
void CDownloadMgrSession::SetIntAttributeL()
    {
    CLOG_ENTERFN( "CDownloadMgrSession::SetIntAttributeL" )
    const TInt attr = CurrentMessage().Int0();
    const TInt32 value = CurrentMessage().Int1();
    CLOG_WRITE_FORMAT( "CDownloadMgrSession::SetIntAttributeL iClientAppInstance %d", iClientAppInstance );
    iClientAppInstance->SetIntAttributeL( (THttpDownloadMgrAttrib)attr, value );
    }
        
// ---------------------------------------------------------
// CDownloadMgrSession::SetBoolAttributeL
// ---------------------------------------------------------
//
void CDownloadMgrSession::SetBoolAttributeL()
    {
    CLOG_ENTERFN( "CDownloadMgrSession::SetBoolAttributeL" )
    const TInt attr = CurrentMessage().Int0();
    const TBool value = (TBool)CurrentMessage().Int1();
    CLOG_WRITE_FORMAT( "CDownloadMgrSession::SetBoolAttributeL iClientAppInstance %d", iClientAppInstance );
    iClientAppInstance->SetBoolAttributeL( (THttpDownloadMgrAttrib)attr, value );
    }
		
// ---------------------------------------------------------
// CDownloadMgrSession::SetStringAttributeL
// ---------------------------------------------------------
//
void CDownloadMgrSession::SetStringAttributeL()
    {
    CLOG_ENTERFN( "CDownloadMgrSession::SetStringAttributeL" )
    const TInt attr = CurrentMessage().Int0();

    TInt desLen = CurrentMessage().GetDesLength( 1 );

	HBufC* writeBuf = HBufC::NewLC( desLen );
	TPtr initptr = writeBuf->Des();

    Read( 1, initptr );

    CLOG_WRITE_FORMAT( "CDownloadMgrSession::SetStringAttributeL iClientAppInstance %d", iClientAppInstance );
    iClientAppInstance->SetStringAttributeL( (THttpDownloadMgrAttrib)attr, initptr );
    CleanupStack::PopAndDestroy( writeBuf ); // writeBuf
    }

// ---------------------------------------------------------
// CDownloadMgrSession::SetString8AttributeL
// ---------------------------------------------------------
//
void CDownloadMgrSession::SetString8AttributeL()
    {
    CLOG_ENTERFN( "CDownloadMgrSession::SetString8AttributeL" )
    const TInt attr = CurrentMessage().Int0();

    TInt desLen = CurrentMessage().GetDesLength( 1 );

	HBufC8* writeBuf = HBufC8::NewLC( desLen ); 
	TPtr8 initptr = writeBuf->Des();

    Read( 1, initptr );

    CLOG_WRITE_FORMAT( "CDownloadMgrSession::SetString8AttributeL iClientAppInstance %d", iClientAppInstance );
    iClientAppInstance->SetStringAttributeL( (THttpDownloadMgrAttrib)attr, initptr );
    CleanupStack::PopAndDestroy( writeBuf ); // writeBuf;
    }

/// ---------------------------------------------------------
// CDownloadMgrSession::InitSessionEvent
// ---------------------------------------------------------
//
void CDownloadMgrSession::InitSessionEvent( const RMessage2& aMessage )
    {
    CLOG_ENTERFN( "CDownloadMgrSession::InitMovedEvent" )
    __ASSERT_DEBUG( iEvent == EFalse, PanicClient( EMultipleInitDownloadEvent ) );
    iEventMessage = aMessage;
    iEvent = ETrue;
    }

/// ---------------------------------------------------------
// CDownloadMgrSession::CancelMovedEvent
// ---------------------------------------------------------
//
void CDownloadMgrSession::CancelMovedEvent()
    {
    CLOG_ENTERFN( "CDownloadMgrSession::CancelMovedEvent" )
    if( iEvent )
        {
        iEventMessage.Complete( KErrCancel );
        iEvent = EFalse;
        }
    }

/// ---------------------------------------------------------
// CDownloadMgrSession::RemoveFromQueue
// ---------------------------------------------------------
//
void CDownloadMgrSession::RemoveFromQueue( CHttpDownload* aDownload )
    {
    iEventQueue->Remove( aDownload );
    }

/// ---------------------------------------------------------
// CDownloadMgrSession::SessionId
// ---------------------------------------------------------
//
TInt CDownloadMgrSession::SessionId() const
    {
    return iSessionId;
    }

/// ---------------------------------------------------------
// CDownloadMgrSession::ClientAppInstance
// ---------------------------------------------------------
//
CHttpClientAppInstance* CDownloadMgrSession::ClientAppInstance()const
    {
    return iClientAppInstance;
    }
