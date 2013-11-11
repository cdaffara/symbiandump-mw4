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
#include "DownloadMgrDefAttrib.h"
#include <HttpClientApp.h>
#include <HttpClientAppInstance.h>
#include <HttpDownload.h>
#include <e32svr.h>
#include <basched.h>
//#include <e32uid.h>

// CONSTANTS

// GLOBAL FUNCTIONS

// ---------------------------------------------------------
// CDownloadSubSession::NewL
// ---------------------------------------------------------
//
CDownloadSubSession * CDownloadSubSession::NewL( CDownloadMgrSession* aSession,
                                           CHttpDownload* aDownload )
	{
    CLOG_ENTERFN_NULL( "CDownloadSubSession::NewL" )
	CDownloadSubSession* self = new (ELeave) CDownloadSubSession();
	CleanupStack::PushL( self );
	self->ConstructL( aSession, aDownload );
	CleanupStack::Pop( self ); // self
	return self;
	}

// ---------------------------------------------------------
// CDownloadSubSession::CDownloadSubSession
// ---------------------------------------------------------
//
CDownloadSubSession::CDownloadSubSession()
:iEvent( EFalse )
    {
    CLOG_CREATE;
    }


// ---------------------------------------------------------
// CDownloadSubSession::ConstructL
// ---------------------------------------------------------
//
void CDownloadSubSession::ConstructL( CDownloadMgrSession *aSession,
                                   CHttpDownload* aDownload )
	{
    CLOG_ENTERFN( "CDownloadSubSession::ConstructL" )
	iSession = aSession;
    iDownload = aDownload;

#ifdef _DEBUG
    TInt32 dlId;
    
    aDownload->GetIntAttributeL( EDlAttrId, dlId );
    CLOG_NAME_2( _L("Download_%d_%d"), aSession->SessionId(), dlId );
#endif
	}

// ---------------------------------------------------------
// CDownloadSubSession::~CDownloadSubSession
// ---------------------------------------------------------
//
CDownloadSubSession::~CDownloadSubSession()
    {
    CLOG_WRITE( "CDownloadSubSession::~CDownloadSubSession()" )
    // Trigger the event queue
    TUint dummy( 0 );
    if( iSession )
        {
        iSession->Event( NULL, (THttpDownloadState)dummy, (THttpProgressState)dummy, (TInt32)dummy );
        }
    if( iAttribs )
        {
        iAttribs->ResetAndDestroy();
        delete iAttribs;
        iAttribs = NULL;
        }    
    CLOG_CLOSE;
    }

// ---------------------------------------------------------
// CDownloadSubSession::CloseCounter
// ---------------------------------------------------------
//
void CDownloadSubSession::CloseDownload()
	{
    CLOG_WRITE( "CDownloadSubSession::CloseDownload" )
   
    iDownload->DetachClientInstance(iSession->ClientAppInstance());
    iSession->ClientAppInstance()->ClientApp()->UnregisterDownload( iDownload ); 
	iSession->DeleteDownload( Message().Int3() );
	}


/// ---------------------------------------------------------
// CDownloadSubSession::Message
// ---------------------------------------------------------
//

const RMessage2& CDownloadSubSession::Message() const
	{
    CLOG_WRITE( "CDownloadSubSession::Message()" );
    CLOG_WRITE_FORMAT( "iSession = %x", iSession );
	return iSession->CurrentMessage();
	}

// ---------------------------------------------------------
// CDownloadSubSession::DispatchDownloadMessageL
// ---------------------------------------------------------
//
void CDownloadSubSession::DispatchMessageL( const RMessage2& aMessage )
    {
    CLOG_ENTERFN( "CDownloadSubSession::DispatchMessageL" )
	switch( aMessage.Function() )
        {
        case EHttpDownloadStart:
            {
            StartL();
            return;
            }
        case EHttpDownloadEventSubscription:
            {
            InitDownloadEvent( aMessage );
            // Send event from the queue
            TUint dummy( 0 );
            iSession->Event( NULL, (THttpDownloadState)dummy, (THttpProgressState)dummy, (TInt32)dummy );
            // This is an asyncron request will be completed later!
            iSession->SetComplete( EFalse );
            return;
            }
        case EHttpDownloadEventCancel:
            {
            CancelDownloadEvent();
            return;
            }
        case EHttpDownloadPause:
            {
            PauseL();
            return;
            }
        case EHttpDownloadReset:
            {
            Reset();
            return;
            }
        case EHttpDownloadDelete:
            {
            Delete();
            return;
            }
        case EHttpDownloadMove:
            {
            MoveL();
            return;
            }
        case EHttpDownloadClose:
            {
            CloseDownload();
            return;
            }
            
        case EHttpDownloadCloseCompleted:
            {
            CloseCompletedDownload();
            return;
            }
            
        case EHttpDownloadGetIntAttribute:
            {
            GetIntAttributeL();
            return;
            }
        case EHttpDownloadGetBoolAttribute:
            {
            GetBoolAttributeL();
            return;
            }
        case EHttpDownloadGetStringAttribute:
            {
            GetStringAttributeL();
            return;
            }
        case EHttpDownloadGetString8Attribute:
            {
            GetString8AttributeL();
            return;
            }
        case EHttpDownloadSetIntAttribute:
            {
            SetIntAttributeL();
            return;
            }
        case EHttpDownloadSetBoolAttribute:
            {
            SetBoolAttributeL();
            return;
            }
        case EHttpDownloadSetStringAttribute:
            {
            SetStringAttributeL();
            return;
            }
        case EHttpDownloadSetString8Attribute:
            {
            SetString8AttributeL();
            return;
            }
        case EHttpDownloadBufferAttributes:
            {
            BufferAttributesL();
            return;
            }
        case EHttpDownloadGetRFile:
            {
            GetFileHandleAttributeL();
            return;
            }
        case EHttpDownloadSetRFile:
            {
            SetFileHandleAttributeL();
            return;
            }
        case EHttpDownloadDataAttribute:
            {
            SetDownloadDataAttributeL();
            return;
            }
        case EHttpDownloadTrackAttribute:
        	{
        	SetTrackDataAttributeL();
        	return;
        	}
        case EHttpDownloadSetOnError:
            {
            const TInt error = Message().Int0();
            const THttpDownloadMgrError dlerror 
                = (THttpDownloadMgrError)Message().Int1();
            OnError( error, dlerror );
            return;
            }

    	default:
            {
	    	iSession->PanicClient( EBadRequest );
            return;
            }
        }
    }


/// ---------------------------------------------------------
// CDownloadSubSession::Start
// ---------------------------------------------------------
//
void CDownloadSubSession::StartL()
    {
    CLOG_ENTERFN( "CDownloadSubSession::StartL" )
    iDownload->StartL();
    }

/// ---------------------------------------------------------
// CDownloadSubSession::Attributes
// ---------------------------------------------------------
//
void CDownloadSubSession::PauseL()
    {
    CLOG_ENTERFN( "CDownloadSubSession::PauseL" )
    iDownload->PauseL( ETrue );
    }

/// ---------------------------------------------------------
// CDownloadSubSession::Reset
// ---------------------------------------------------------
//
void CDownloadSubSession::Reset()
    {
    CLOG_ENTERFN( "CDownloadSubSession::Reset" )
    iDownload->Reset();
    }

/// ---------------------------------------------------------
// CDownloadSubSession::Delete
// ---------------------------------------------------------
//
void CDownloadSubSession::Delete()
    {
    CLOG_ENTERFN( "CDownloadSubSession::Delete()" )
    iSession->RemoveFromQueue( iDownload );
    iDownload->Delete( iSession->ClientAppInstance() );
    }

/// ---------------------------------------------------------
// CDownloadSubSession::CloseCompletedDownload
// ---------------------------------------------------------
//
void CDownloadSubSession::CloseCompletedDownload()
	{
    CLOG_WRITE( "CDownloadSubSession::CloseCompletedDownload" )
    iDownload->DeleteInfoFile( iSession->ClientAppInstance() );
    iSession->RemoveFromQueue( iDownload );
    iSession->DeleteDownload( Message().Int3() );
	}

/// ---------------------------------------------------------
// CDownloadSubSession::MoveL
// ---------------------------------------------------------
//
void CDownloadSubSession::MoveL()
    {
    CLOG_ENTERFN( "CDownloadSubSession::MoveL" )
    iDownload->MoveL();
    }

/// ---------------------------------------------------------
// CDownloadSubSession::InitDownloadEvent
// ---------------------------------------------------------
//
void CDownloadSubSession::InitDownloadEvent( const RMessage2& aMessage )
    {
    CLOG_ENTERFN( "CDownloadSubSession::InitDownloadEvent" )
    __ASSERT_DEBUG( iEvent == EFalse, iSession->PanicClient( EMultipleInitDownloadEvent ) );
    iMessage = aMessage;
    iEvent = ETrue;
    }

/// ---------------------------------------------------------
// CDownloadSubSession::CancelDownloadEvent
// ---------------------------------------------------------
//
void CDownloadSubSession::CancelDownloadEvent()
    {
    CLOG_ENTERFN( "CDownloadSubSession::CancelDownloadEvent" )
    CLOG_WRITE_FORMAT( "CDownloadSubSession::CancelDownloadEvent iEvent = %d", iEvent )
    if( iEvent )
        {
        iMessage.Complete( KErrCancel );
        iEvent = EFalse;
        }
    }

// ---------------------------------------------------------
// CDownloadSubSession::Event
// ---------------------------------------------------------
//
TBool CDownloadSubSession::EventL( TInt32 aDownloadState, TInt32 aProgressState, TInt32 aMoIndex )
    {
    TBool retVal( iEvent );
    CLOG_ENTERFN( "CDownloadSubSession::Event" )
    if( iEvent )
        {
        CLOG_WRITE_FORMAT( " aDownloadState = %d", aDownloadState )
        CLOG_WRITE_FORMAT( " aProgressState = %d", aProgressState )
        CLOG_WRITE_FORMAT( " aMoIndex = %d", aMoIndex )

        TPckgBuf<TInt32> pckgDownloadState( aDownloadState );
        TPckgBuf<TInt32> pckgProgressState( aProgressState );
        
        TInt ret = iMessage.Write( 0, pckgDownloadState );
	    if( ret != KErrNone )
            {
            CLOG_WRITE_FORMAT( " iMessage.Write( 0, pckgDownloadState ) returned error code: %d", ret )
		    iSession->PanicClient( EBadDescriptor );
            }
        else
            {
            ret = iMessage.Write( 1, pckgProgressState );
	        if( ret != KErrNone )
                {
                CLOG_WRITE_FORMAT( " iMessage.Write( 1, pckgProgressState ) returned error code: %d", ret )
		        iSession->PanicClient( EBadDescriptor );
                }
	     	else
	            {
	            HBufC8* eventAttr = NULL;
	            
	            TInt32 userdata( 0 );
	            iDownload->GetIntAttributeL( EDlAttrUserData, userdata );
	            
	            TInt32 moDownloadedsize( 0 );
	            iDownload->GetIntAttributeL( EDlAttrMultipleMODownloadedSize, moDownloadedsize );
	            
	            TInt32 moLength( 0 );
	            iDownload->GetIntAttributeL( EDlAttrMultipleMOLength, moLength );
	            
	            TInt32 downloadedsize( 0 );
	            iDownload->GetIntAttributeL( EDlAttrDownloadedSize, downloadedsize );
	            
	            TInt32 length( 0 );
	            iDownload->GetIntAttributeL( EDlAttrLength, length );
	            	            
	            // Mask  media object index with userdata.
			    // For example if aMoIndex = 0x00000004 and userdata = 0x00000005,
			    // then maskData = 0x00040005.
			    TInt32 maskedData = (aMoIndex << 16);
			    maskedData = (maskedData | (userdata & 0x0000FFFF));
	            
	            TRAPD( err, eventAttr = TDMgrUtils::PackEventAttribL( maskedData, moDownloadedsize, downloadedsize, moLength, length ) );           
	            if( KErrNone == err )
	                {      
	                TPtr8 ptr = eventAttr->Des();
	                ret = iMessage.Write( 2, ptr );
	    	        if( ret != KErrNone )
	                    {
	                    CLOG_WRITE_FORMAT( " iMessage.Write( 2, ptr ) returned error code: %d", ret )
	    		        iSession->PanicClient( EBadDescriptor );
	                    }
	                }
	            delete eventAttr;
	            }
            }
        EventComplete( KErrNone );
        }
    CLOG_WRITE_FORMAT( "Event sent: %d", retVal )
    return retVal;
    }

// ---------------------------------------------------------
// CDownloadSubSession::CreateAttribPackL
// ---------------------------------------------------------
//
HBufC8* CDownloadSubSession::CreateAttribPackL( TBool aBuffered )
    {
    CLOG_ENTERFN( "CDownloadSubSession::CreateEventAttribPackLC" )
    iAttribs = 
        new (ELeave) CArrayPtrFlat< CDefaultAttrib >( 1 );
    
    TInt32 value( 0 );
    iDownload->GetIntAttributeL( EDlAttrUserData, value );
    CDefaultAttrib* attrib = CDefaultAttrib::NewL( EDlAttrUserData, value );
    CleanupStack::PushL( attrib );
    iAttribs->AppendL( attrib );
    CleanupStack::Pop( attrib );
    
    iDownload->GetIntAttributeL( EDlAttrMultipleMODownloadedSize, value );
    attrib = CDefaultAttrib::NewL( EDlAttrMultipleMODownloadedSize, value );
    CleanupStack::PushL( attrib );
    iAttribs->AppendL( attrib );
    CleanupStack::Pop( attrib );
    
    iDownload->GetIntAttributeL( EDlAttrDownloadedSize, value );
    attrib = CDefaultAttrib::NewL( EDlAttrDownloadedSize, value );
    CleanupStack::PushL( attrib );
    iAttribs->AppendL( attrib );
    CleanupStack::Pop( attrib );
    
    iDownload->GetIntAttributeL( EDlAttrMultipleMOLength, value );
    attrib = CDefaultAttrib::NewL( EDlAttrMultipleMOLength, value );
    CleanupStack::PushL( attrib );
    iAttribs->AppendL( attrib );
    CleanupStack::Pop( attrib ); 
    
    iDownload->GetIntAttributeL( EDlAttrLength, value );
    attrib = CDefaultAttrib::NewL( EDlAttrLength, value );
    CleanupStack::PushL( attrib );
    iAttribs->AppendL( attrib );
    CleanupStack::Pop( attrib ); 
    
    if( aBuffered )
        {
        TBool deleteStr;
        HBufC8* tempBuf = iDownload->GetString8AttributeL( 
                                     EDlAttrReqUrl, 
                                     deleteStr );
        if( deleteStr )
            {
            CleanupStack::PushL( tempBuf );
            }
        TInt length = tempBuf->Length();                             
        attrib = CDefaultAttrib::NewL( EDlAttrReqUrl, tempBuf->Des() );
        CleanupStack::PushL( attrib );
        iAttribs->AppendL( attrib );
        CleanupStack::Pop( attrib );
        if( deleteStr )
            {
            CleanupStack::PopAndDestroy( tempBuf );  // tempBuf
            }
        
        iDownload->GetIntAttributeL( EDlAttrId, value );    
        attrib = CDefaultAttrib::NewL( EDlAttrId, value );
        CleanupStack::PushL( attrib );
        iAttribs->AppendL( attrib );
        CleanupStack::Pop( attrib );          
        }
    iDownload->GetIntAttributeL( EDlAttrActiveDownload, value );
    attrib = CDefaultAttrib::NewL( EDlAttrActiveDownload, value );
    CleanupStack::PushL( attrib );
    iAttribs->AppendL( attrib );
    CleanupStack::Pop( attrib ); 

    HBufC8* buf = TDMgrUtils::PackedAttributesL( iAttribs );
    iAttribs->ResetAndDestroy();
    delete iAttribs;
    iAttribs = NULL;
    return buf;
    }

// ---------------------------------------------------------
// CDownloadSubSession::OnError
// ---------------------------------------------------------
//
void CDownloadSubSession::OnError( TInt aError,
                                   THttpDownloadMgrError aDlError )
    {
    CLOG_ENTERFN( "CDownloadSubSession::OnError" )

    iDownload->OnError( aError, aDlError );
    }

// ---------------------------------------------------------
// CDownloadSubSession::EventComplete
// ---------------------------------------------------------
//
void CDownloadSubSession::EventComplete( TInt aErrorStatus )
    {
    CLOG_ENTERFN( "CDownloadSubSession::EventComplete" )
    if( iEvent )
        {
        iMessage.Complete( aErrorStatus );
        iEvent = EFalse;
        }
    }

// ---------------------------------------------------------
// CDownloadSubSession::GetIntAttributeL
// ---------------------------------------------------------
//
void CDownloadSubSession::GetIntAttributeL()
    {
    CLOG_ENTERFN( "CDownloadSubSession::GetIntAttributeL" )

    const TInt attr = Message().Int0();
    TInt32 moIndex = Message().Int1();
    TInt32 value;
	
	if (moIndex == KNonMoIndex)
    	iDownload->GetIntAttributeL( (THttpDownloadAttrib)attr, value );
	else
		iDownload->GetIntAttributeL( (THttpDownloadAttrib)attr, moIndex, value );

    TPckg<TInt32> pckg( value );

    Write( 2, pckg );
    }

// ---------------------------------------------------------
// CDownloadSubSession::GetBoolAttributeL
// ---------------------------------------------------------
//
void CDownloadSubSession::GetBoolAttributeL()
    {
    CLOG_ENTERFN( "CDownloadSubSession::GetBoolAttributeL" )

    const TInt attr = Message().Int0();
    TInt32 moIndex = Message().Int1();
    TBool value;

	if (moIndex == KNonMoIndex)
    	iDownload->GetBoolAttributeL( (THttpDownloadAttrib)attr, value );
	else
		iDownload->GetBoolAttributeL( (THttpDownloadAttrib)attr, moIndex, value );

    TPckg<TBool> pckg( value );

    Write( 2, pckg );
    }

// ---------------------------------------------------------
// CDownloadSubSession::GetStringAttributeL
// ---------------------------------------------------------
//
void CDownloadSubSession::GetStringAttributeL()
    {
    CLOG_ENTERFN( "CDownloadSubSession::GetStringAttributeL" )
    const TInt attr = Message().Int0();
    TInt32 moIndex = Message().Int1();

    TBool deleteStr;
	HBufC* tempBuf = NULL;
	if (moIndex == KNonMoIndex)
		tempBuf = iDownload->GetStringAttributeL( (THttpDownloadAttrib)attr, deleteStr );
	else
		tempBuf = iDownload->GetStringAttributeL( (THttpDownloadAttrib)attr, moIndex, deleteStr );

    TPtr value( tempBuf->Des() );

    Write( 2, value );

    if( deleteStr )
        {
        delete tempBuf;
        }
    }

// ---------------------------------------------------------
// CDownloadSubSession::GetString8AttributeL
// ---------------------------------------------------------
//
void CDownloadSubSession::GetString8AttributeL()
    {
    CLOG_ENTERFN( "CDownloadSubSession::GetString8AttributeL" )
    const TInt attr = Message().Int0();
    TInt32 moIndex = Message().Int1();

    TBool deleteStr;
	HBufC8* tempBuf = NULL;
	if (moIndex == KNonMoIndex)
		tempBuf = iDownload->GetString8AttributeL( (THttpDownloadAttrib)attr, deleteStr );
	else
		tempBuf = iDownload->GetString8AttributeL( (THttpDownloadAttrib)attr, moIndex, deleteStr );

    TPtr8 value( tempBuf->Des() );

    Write( 2, value );

    if( deleteStr )
        {
        delete tempBuf;
        }
    }

// ---------------------------------------------------------
// CDownloadSubSession::GetFileHandleAttributeL
// ---------------------------------------------------------
//
void CDownloadSubSession::GetFileHandleAttributeL()
    {
    CLOG_ENTERFN( "CDownloadSubSession::GetFileHandleAttributeL" )

    RFile* file = iDownload->GetFileHandleAttributeL();

    // transfer to client: store the RFile handle into the package buffer in slot 0 
    // and complete the message with the RFs handle
    User::LeaveIfError( file->TransferToClient( Message(), 0) );
    }
    
// ---------------------------------------------------------
// CDownloadSubSession::SetIntAttributeL
// ---------------------------------------------------------
//
void CDownloadSubSession::SetIntAttributeL()
    {
    CLOG_ENTERFN( "CDownloadSubSession::SetIntAttributeL" )
    const TInt attr = Message().Int0();
    const TInt32 value = Message().Int1();
    iDownload->SetIntAttributeL( (THttpDownloadAttrib)attr, value );
    }
        
// ---------------------------------------------------------
// CDownloadSubSession::SetBoolAttributeL
// ---------------------------------------------------------
//
void CDownloadSubSession::SetBoolAttributeL()
    {
    CLOG_ENTERFN( "CDownloadSubSession::SetBoolAttributeL" )
    const TInt attr = Message().Int0();
    const TBool value = (TBool)Message().Int1();
    iDownload->SetBoolAttributeL( (THttpDownloadAttrib)attr, value );
    }
		
// ---------------------------------------------------------
// CDownloadSubSession::SetStringAttributeL
// ---------------------------------------------------------
//
void CDownloadSubSession::SetStringAttributeL()
    {
    CLOG_ENTERFN( "CDownloadSubSession::SetStringAttributeL" )
    const TInt attr = Message().Int0();
    const TInt moIndex = Message().Int1();

    TInt desLen = Message().GetDesLength( 2 );

	HBufC* writeBuf = HBufC::NewLC( desLen );
	TPtr initptr = writeBuf->Des();

    Read( 2, initptr );
    
    if(moIndex != KNonMoIndex)
        {
        iDownload->SetStringAttributeL( (THttpDownloadAttrib)attr, moIndex, initptr );    
        }
    else
        {
        iDownload->SetStringAttributeL( (THttpDownloadAttrib)attr, initptr );    
        }
    
    CleanupStack::PopAndDestroy( writeBuf ); // writeBuf
    }

// ---------------------------------------------------------
// CDownloadSubSession::SetString8AttributeL
// ---------------------------------------------------------
//
void CDownloadSubSession::SetString8AttributeL()
    {
    CLOG_ENTERFN( "CDownloadSubSession::SetString8AttributeL" )
    const TInt attr = Message().Int0();

    TInt desLen = Message().GetDesLength( 1 );

	HBufC8* writeBuf = HBufC8::NewLC( desLen ); 
	TPtr8 initptr = writeBuf->Des();

    Read( 1, initptr );

    iDownload->SetStringAttributeL( (THttpDownloadAttrib)attr, initptr );
    CleanupStack::PopAndDestroy( writeBuf ); // writeBuf;
    }

// ---------------------------------------------------------
// CDownloadSubSession::SetFileHandleAttributeL
// ---------------------------------------------------------
//
void CDownloadSubSession::SetFileHandleAttributeL()
    {
    CLOG_ENTERFN( "CDownloadSubSession::SetFileHandleAttributeL" )
    
    RFile* file = new (ELeave) RFile;
    CleanupStack::PushL( file );
    
    User::LeaveIfError( file->AdoptFromClient( Message(), 0, 1 ) );
    
    // ownership is passed to the engine
    iDownload->SetFileHandleAttributeL( file );
    
    CleanupStack::Pop( file );
    }

// ---------------------------------------------------------
// CDownloadSubSession::SetDownloadDataAttributeL
// ---------------------------------------------------------
//
void CDownloadSubSession::SetDownloadDataAttributeL()
    {
    CLOG_ENTERFN( "CDownloadSubSession::SetDownloadDataAttributeL" )
    
    TInt desLen = Message().GetDesLength( 0 );
    
    HBufC8* writeBuf = HBufC8::NewLC( desLen );
    TPtr8 initPtr = writeBuf->Des();
    
    Read( 0, initPtr);
    
    // ownership is passed to the engine
    iDownload->SetDownloadDataAttributeL(writeBuf);
    
    CleanupStack::PopAndDestroy(writeBuf);
    }

// ---------------------------------------------------------
// CDownloadSubSession::SetTrackDataAttributeL
// ---------------------------------------------------------
//
void CDownloadSubSession::SetTrackDataAttributeL()
    {
    CLOG_ENTERFN( "CDownloadSubSession::SetTrackDataAttributeL" )
    
    const TInt index = Message().Int0();
    TInt desLen = Message().GetDesLength( 1 );
    
    HBufC8* writeBuf = HBufC8::NewLC( desLen );
    TPtr8 initPtr = writeBuf->Des();
    
    Read( 1, initPtr);
    
    // ownership is passed to the engine
    iDownload->SetTrackDataAttributeL(index, writeBuf);
    
    CleanupStack::PopAndDestroy(writeBuf);
    }

// ---------------------------------------------------------
// CDownloadSubSession::SetDefAttributesL
// ---------------------------------------------------------
//
void CDownloadSubSession::SetDefAttributesL()
    {
    CLOG_ENTERFN( "CDownloadSubSession::SetDefAttributesL" )

    TInt desLen = Message().GetDesLength( 2 );

	HBufC8* attribBuf = HBufC8::NewLC( desLen ); 
	TPtr8 attribs = attribBuf->Des();

    Read( 2, attribs );

    CDefaultAttrib* attrib = CDefaultAttrib::NewL();
    CleanupStack::PushL( attrib );
    TDMgrUtils util;
    util.iPtr.Set( attribs );
    util.iCurrent = 0;
    
    while( util.NextAttribL( attrib, attribs ) )
        {
        switch( attrib->iType )
            {
            case CDefaultAttrib::EString8:
                {
                iDownload->SetStringAttributeL( attrib->iAttribute,
                                                attrib->AsTPtrC8L() );
                }
                break;
            case CDefaultAttrib::EString16:
                {
                iDownload->SetStringAttributeL( attrib->iAttribute,
                                                attrib->AsTPtrC16L() );
                }
                break;
            case CDefaultAttrib::EInt:
                {
                iDownload->SetIntAttributeL( attrib->iAttribute,
                                             attrib->AsTInt32L() );
                }
                break;
            case CDefaultAttrib::EBool:
                {
                iDownload->SetBoolAttributeL( attrib->iAttribute,
                                              attrib->AsTBoolL() );
                }
                break;
            default:
                break;
            }
        }

    CleanupStack::PopAndDestroy( 2 ); // attribBuf, attrib;
    }

// ---------------------------------------------------------
// CDownloadSubSession::Write
// ---------------------------------------------------------
//
void CDownloadSubSession::Write( TInt aParam, TDesC8& aDes )
	{
    CLOG_ENTERFN( "CDownloadSubSession::Write" )

    TInt ret = Message().Write( aParam, aDes );

    CLOG_WRITE_FORMAT( "CDownloadSubSession::Write ret = %d", ret );
    CLOG_WRITE_FORMAT( "CDownloadSubSession = %x", this );
	if( ret != KErrNone )
        {
		iSession->PanicClient( EBadDescriptor );
        }
	}

// ---------------------------------------------------------
// CDownloadSubSession::Write
// ---------------------------------------------------------
//
void CDownloadSubSession::Write( TInt aParam, TDesC16& aDes )
	{
    CLOG_ENTERFN( "CDownloadSubSession::Write" )

    TInt ret = Message().Write( aParam, aDes );

	if( ret != KErrNone )
        {
		iSession->PanicClient( EBadDescriptor );
        }
	}

// ---------------------------------------------------------
// CDownloadSubSession::Read
// ---------------------------------------------------------
//
void CDownloadSubSession::Read( TInt aParam, TDes8& aDes )
	{
    CLOG_ENTERFN( "CDownloadSubSession::Read" )

    TInt ret = Message().Read( aParam, aDes );

	if( ret != KErrNone )
        {
		iSession->PanicClient( EBadDescriptor );
        }
	}

// ---------------------------------------------------------
// CDownloadSubSession::Read
// ---------------------------------------------------------
//
void CDownloadSubSession::Read( TInt aParam, TDes16& aDes )
	{
    CLOG_ENTERFN( "CDownloadSubSession::Read" )

    TInt ret = Message().Read( aParam, aDes );

	if( ret != KErrNone )
        {
		iSession->PanicClient( EBadDescriptor );
        }
	}

// ---------------------------------------------------------
// CDownloadSubSession::IsCurrentDownload
// ---------------------------------------------------------
//
TBool CDownloadSubSession::IsDownload( CHttpDownload* aDownload )
    {
    CLOG_WRITE( "CDownloadSubSession::IsDownload" );
    __ASSERT_DEBUG( iDownload, iSession->PanicClient( EBadSubsessionHandle ) );
    return (iDownload == aDownload) ? ETrue : EFalse;
    }

// ---------------------------------------------------------
// CDownloadSubSession::Handle
// ---------------------------------------------------------
//
TUint CDownloadSubSession::Handle()
    {
    CLOG_WRITE( "CDownloadSubSession::Handle" )
    return iHandle;
    }

// ---------------------------------------------------------
// CDownloadSubSession::SetHandle
// ---------------------------------------------------------
//
void CDownloadSubSession::SetHandle( TUint aHandle )
    {
    CLOG_ENTERFN( "CDownloadSubSession::SetHandle" )
    iHandle = aHandle;
    }

// ---------------------------------------------------------
// CDownloadSubSession::Download
// ---------------------------------------------------------
//
CHttpDownload* CDownloadSubSession::Download()
    {
    CLOG_WRITE( "CDownloadSubSession::Download" )
    return iDownload;
    }

// ---------------------------------------------------------
// CDownloadSubSession::BufferAttributesL
// ---------------------------------------------------------
//
void CDownloadSubSession::BufferAttributesL()
    {
    HBufC8* attr = CreateAttribPackL( ETrue );
    TPtr8 ptr = attr->Des();
    Write( 0, ptr );
    delete  attr;
    }
