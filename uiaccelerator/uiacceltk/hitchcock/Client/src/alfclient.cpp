/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:   Server client for alfredserver.exe
*
*/



#include "alfclient.h"
#include "alf/alfconstants.h"
#include "alfuids.h"
#include "alflogger.h"
#include "alfclientserver.h"
#include "alf/alftexture.h"
#include "alf/alfcontrol.h"
#include <e32math.h>

#include "uiacceltk/HuiUtil.h"

const TInt KAlfClientDefaultBatchBufferSize = 
    KAlfClientServerDefaultCommandBatchBufferSize;

// ======== HELPER CLASSES ========

// ---------------------------------------------------------------------------
// CBatchBufferAutoFlusher
//
// This class is responsible of flushing the client side command buffer
// when it gets time - hopefully as fast as possible after synchronous 
// commands.
// ---------------------------------------------------------------------------
//
NONSHARABLE_CLASS( RAlfClient::CBatchBufferAutoFlusher ) : public CActive
    {
public:
    CBatchBufferAutoFlusher( RAlfClient& aClient );
    ~CBatchBufferAutoFlusher();
    
    void StartAutoFlush();
protected:
    void RunL();
    void DoCancel();
private:
    RAlfClient& iClient;
    };

RAlfClient::CBatchBufferAutoFlusher::CBatchBufferAutoFlusher( RAlfClient& aClient )
 : CActive( EPriorityHigh+5), // Priority must be bigger than any received events from the server
    iClient( aClient ) 
    {
    CActiveScheduler::Add( this );
    }
     
RAlfClient::CBatchBufferAutoFlusher::~CBatchBufferAutoFlusher()
    {
    Cancel();
    }
    
void RAlfClient::CBatchBufferAutoFlusher::StartAutoFlush()
    {
    if ( !IsActive() )
        {
        iStatus = KRequestPending;
        SetActive();
        
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, KErrNone );
        }
    }
    
void RAlfClient::CBatchBufferAutoFlusher::RunL()
    {
    iClient.FlushBatchBuffer();
    }
    
void RAlfClient::CBatchBufferAutoFlusher::DoCancel()
    {
    }

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
RAlfClient::RAlfClient()
    : RAlfClientBase(KAlfServiceUid.iUid), 
        iDefaultBatchBuffer(0), 
        iExtendedBatchBuffer(0),
        iUsedBatchBuffer(0),
        iAutoFlusher(0),
        iAutoFlushMode( EAlfAutoFlushDeferred ),
        iUsedMaxBufferSize( KAlfBatchBufferSizeNoLimit )
    {
    }

// ---------------------------------------------------------------------------
// "destructor"
// ---------------------------------------------------------------------------
//    
void RAlfClient::Disconnect()
    {
    if ( Handle() )
        {
        FlushBatchBuffer();
        }
   
    delete iExtendedBatchBuffer;
    iExtendedBatchBuffer = NULL;   
        
    delete iDefaultBatchBuffer;
    iDefaultBatchBuffer = NULL;

    iUsedBatchBuffer = NULL;
    
    delete iAutoFlusher;
    iAutoFlusher = NULL;
    
    Close();
    }
 
// ---------------------------------------------------------------------------
// Sets refresh mode
// ---------------------------------------------------------------------------
//   
void RAlfClient::EnvSetRefreshMode( TAlfRefreshMode aMode )
    {
    Send(EAlfEnvSetRefreshMode, TIpcArgs(aMode) );
    }

// ---------------------------------------------------------------------------
// Sets frame rate
// ---------------------------------------------------------------------------
//
void RAlfClient::EnvSetMaxFrameRate( TReal32 aFrameRate )
    {
    TPckg<TReal32> frameRatePckg( aFrameRate );
    SendReceive(EAlfEnvSetMaxFrameRate, TIpcArgs(&frameRatePckg) );
    }

// ---------------------------------------------------------------------------
// Shows control group in roster.
// ---------------------------------------------------------------------------
// 
TInt RAlfClient::RosterShow( 
    TInt aCntrlGroupHandle, 
    TInt aWhere, 
    TInt aDisplayHandle )
    {
    // Need to flush.
    FlushBatchBuffer();
    
    TInt err = SendReceive(
        EAlfRosterShow, 
        TIpcArgs(aCntrlGroupHandle, aWhere, aDisplayHandle) );
    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "RAlfClient::RosterShow return error %d", err )
        }
    return err;
    }
 
// ---------------------------------------------------------------------------
// Hides the control group from the roster.
// ---------------------------------------------------------------------------
//   
TInt RAlfClient::RosterHide( TInt aCntrlGroupHandle, TInt aDisplayHandle )
    {
    TInt err = SendReceive(
        EAlfRosterHide, 
        TIpcArgs(aCntrlGroupHandle, aDisplayHandle) );
    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "RAlfClient::RosterHide return error %d", err )
        }
    return err;
    }
    
// ---------------------------------------------------------------------------
// 
// --------------------------------------------------------------------------
//  
TInt RAlfClient::RosterShowVisual( TInt aVisualHandle, TInt aDisplayHandle )
    {
    TInt err = SendReceive(
        EAlfRosterShowVisual, 
        TIpcArgs(aVisualHandle, aDisplayHandle) );
    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "RAlfClient::RosterShowVisual return error %d", err )
        }
    return err;
    }
   
// ---------------------------------------------------------------------------
// 
// --------------------------------------------------------------------------
//  
TInt RAlfClient::RosterHideVisual( TInt aVisualHandle,  TInt aDisplayHandle )
    {
    TInt err = SendReceive(
        EAlfRosterHideVisual, 
        TIpcArgs(aVisualHandle, aDisplayHandle) );
    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "RAlfClient::RosterHideVisual return error %d", err )
        }
    return err;
    }
   
// ---------------------------------------------------------------------------
// 
// --------------------------------------------------------------------------
//  
TInt RAlfClient::RosterSetPointerEventObservers( 
    TInt aFlags, TInt aCntrlHandle, TInt aDisplayHandle )
    {
    TInt err = SendReceive(
        EAlfRosterSetPointerEventFlags, 
        TIpcArgs(aFlags, aCntrlHandle, aDisplayHandle) );
    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "RAlfClient::RosterSetPointerEventObservers return error %d", err )
        }
    return err;
    }

// ---------------------------------------------------------------------------
// 
// --------------------------------------------------------------------------
//    
TInt RAlfClient::RosterAddPointerEventObserver( 
    TAlfPointerEventFlags aObserver, TInt aCntrlHandle, TInt aDisplayHandle )
    {
    TInt err = SendReceive(
        EAlfRosterAddPointerEventObserver, 
        TIpcArgs(aObserver, aCntrlHandle, aDisplayHandle) );
    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "RAlfClient::RosterAddPointerEventObserver return error %d", err )
        }
    return err;
    }

// ---------------------------------------------------------------------------
// 
// --------------------------------------------------------------------------
//
TInt RAlfClient::RosterRemovePointerEventObserver( 
    TAlfPointerEventFlags aObserver, TInt aCntrlHandle, TInt aDisplayHandle )
    {
    TInt err = SendReceive(
        EAlfRosterRemovePointerEventObserver, 
        TIpcArgs(aObserver, aCntrlHandle, aDisplayHandle) );
    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "RAlfClient::RosterRemovePointerEventObserver return error %d", err )
        }
    return err;
    }
    
// ---------------------------------------------------------------------------
// 
// --------------------------------------------------------------------------
//  
TInt RAlfClient::RosterMoveVisualToFront( TInt aVisualHandle, TInt aDisplayHandle )
    {
    TInt err = SendReceive(
        EAlfRosterMoveVisualToFront, 
        TIpcArgs(aVisualHandle, aDisplayHandle) );
    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "RAlfClient::RosterMoveVisualToFront return error %d", err )
        }
    return err;
    }

// ---------------------------------------------------------------------------
// Asks server to update owner id.
// --------------------------------------------------------------------------
//   
TInt RAlfClient::TextureUpdateOwnerId()
    {
    return SendReceive( EAlfTextureUpdateOwnerId, TIpcArgs() );
    }

// ---------------------------------------------------------------------------
// 
// --------------------------------------------------------------------------
//   
TInt RAlfClient::TextureCreateAnimatedL(TInt aId, TAlfTextureFlags aFlags, TInt aManagerId, const TDesC& aFilename)
    {
    TInt flags = aFlags;
    flags &= ~EAlfTextureFlagSkinContent; // Skin content is handled in the client side    
	
    TInt handle = 0;
    TPckg<TInt> handlePckg(handle);
    
    TAlfCreateTextureAnimatedParams params;
    params.iId = aId; 
    params.iBitmapHandle = 0;
    params.iMaskBitmapHandle = 0;
    params.iFlags = flags;
    params.iManagerId = aManagerId;
    params.iFilename = aFilename;
    
    TPckg<TAlfCreateTextureAnimatedParams> paramsPckg(params);
    TInt err = SendReceive(EAlfTextureCreateAnimated, TIpcArgs(&handlePckg, &paramsPckg) );
    
    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "RAlfClient::TextureCreateAnimatedL leave error %d", err )
        }
    
    User::LeaveIfError( err );
    return handle;		
    }

// ---------------------------------------------------------------------------
// 
// --------------------------------------------------------------------------
// 
TInt RAlfClient::TextureStartAnimation( TInt aId )
    {
    TPckg<TInt> idPckg(aId);
    return SendReceive(EAlfTextureStartAnimation, TIpcArgs(aId) );
    }
   
// ---------------------------------------------------------------------------
// 
// --------------------------------------------------------------------------
//      
TInt RAlfClient::TextureStopAnimation( TInt aId )
    {
    TPckg<TInt> idPckg(aId);
    return SendReceive(EAlfTextureStopAnimation, TIpcArgs(aId) );
    }

// ---------------------------------------------------------------------------
// Creates a texture.
// ---------------------------------------------------------------------------
// 
TInt RAlfClient::TextureCreateL(TInt aId, 
    TInt aBitmapHandle, 
    TInt aMaskBitmapHandle, 
    TAlfTextureFlags aFlags,
    TInt aManagerId )
	{
    TInt flags = aFlags;
    flags &= ~EAlfTextureFlagSkinContent; // Skin content is handled in the client side    
	
    TInt handle = 0;
    TPckg<TInt> handlePckg(handle);

    TAlfCreateTextureParams params;
    params.iId = aId; 
    params.iBitmapHandle = aBitmapHandle;
    params.iMaskBitmapHandle = aMaskBitmapHandle;
    params.iFlags = flags;
    params.iManagerId = aManagerId;
    
    TPckg<TAlfCreateTextureParams> paramsPckg(params);
    TInt err = SendReceive(EAlfTextureCreate, TIpcArgs(&handlePckg, &paramsPckg) );
    
    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "RAlfClient::TextureCreateL leave error %d", err )
        }
    
    User::LeaveIfError( err );
    return handle;		
	}

// ---------------------------------------------------------------------------
// Loads a texture.
// ---------------------------------------------------------------------------
// 
TInt RAlfClient::TextureLoadL(TInt aId, 
    TInt aBitmapHandle, 
    TInt aMaskBitmapHandle, 
    TAlfTextureFlags aFlags,
    TInt aManagerId )
	{
    TInt flags = aFlags;
    flags &= ~EAlfTextureFlagSkinContent; // Skin content is handled in the client side    

    TInt handle = 0;
    TPckg<TInt> handlePckg(handle);

    TAlfLoadTextureParams params;
    params.iId = aId; 
    params.iBitmapHandle = aBitmapHandle;
    params.iMaskBitmapHandle = aMaskBitmapHandle;
    params.iFlags = flags;
    params.iManagerId = aManagerId;
    
    TPckg<TAlfLoadTextureParams> paramsPckg(params);
    TInt err = SendReceive(EAlfTextureLoad, TIpcArgs(&handlePckg, &paramsPckg) );
    
    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "RAlfClient::TextureLoadL leave error %d", err )
        }
    
    User::LeaveIfError( err );
    return handle;		
	}

// ---------------------------------------------------------------------------
// Unloads a texture.
// ---------------------------------------------------------------------------
// 
TInt RAlfClient::TextureUnload( TInt aId, TInt aManagerId )
    {
    TInt err =  SendReceive(EAlfTextureUnload, TIpcArgs(aId, aManagerId));
    
    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "RAlfClient::TextureUnload return error %d", err )
        }
        
    return err;
    }	

// ---------------------------------------------------------------------------
// Delete a texture.
// ---------------------------------------------------------------------------
// 
TInt RAlfClient::TextureDelete( TInt aId, TInt aManagerId )
    {
    TInt err = SendReceive(EAlfTextureDelete, TIpcArgs(aId, aManagerId));
    
    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "RAlfClient::TextureDelete return error %d", err )
        }	
    
    return err;	                    
    }	

// ---------------------------------------------------------------------------
// Release a texture.
// ---------------------------------------------------------------------------
// 
TInt RAlfClient::TextureRelease( TInt aId, TInt aManagerId )
    {
    TInt err = SendReceive(EAlfTextureRelease, TIpcArgs(aId, aManagerId));
    
    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "RAlfClient::TextureRelease return error %d", err )
        }	
    
    return err;	                    
    }	

// ---------------------------------------------------------------------------
// Restore a texture.
// ---------------------------------------------------------------------------
// 
TInt RAlfClient::TextureRestore( TInt aId, TInt aManagerId )
    {
    TInt err = SendReceive(EAlfTextureRestore, TIpcArgs(aId, aManagerId));
    
    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "RAlfClient::TextureRestore return error %d", err )
        }	
    
    return err;	                    
    }	

// ---------------------------------------------------------------------------
// Notifies a skin change for a texture.
// ---------------------------------------------------------------------------
// 
TInt RAlfClient::TextureNotifySkinChanged( TInt aId, TInt aManagerId )
    {
    TInt err = SendReceive(EAlfTextureNotifySkinChanged, TIpcArgs(aId, aManagerId));
    
    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "RAlfClient::TextureNotifySkinChanged return error %d", err )
        }	
    
    return err;	                    
    }	


// ---------------------------------------------------------------------------
// Blurs a texture.
// ---------------------------------------------------------------------------
// 
TInt RAlfClient::TextureProcessorBlurL( const TInt aServerSideSrcHandle, 
	    TInt aServerSideDstHandle, 
	    TInt aManagerId, 
	    const TSize& aPreferredSize, 
	    TInt aFilterSize, 
	    TInt aFlag )
    {
    TAlfBlurTextureParams params;
    params.iServerSideSrcHandle = aServerSideSrcHandle;
    params.iServerSideDstHandle = aServerSideDstHandle;
    params.iManagerId = aManagerId;
    params.iPreferredSize = aPreferredSize;     
    params.iFilterSize = aFilterSize;
    params.iFlag = aFlag;
    
    TPckg<TAlfBlurTextureParams> paramsPckg(params);
    TInt err = SendReceive(EAlfTextureBlur, TIpcArgs(&paramsPckg) );   
    
    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "RAlfClient::TextureProcessorBlurL return error %d", err )
        }	
    
    return err;	  
    }

// ---------------------------------------------------------------------------
// Checks texture content.
// ---------------------------------------------------------------------------
// 
TInt RAlfClient::TextureHasContent( TBool& aHasContent, TInt aId, TInt aManagerId )
    {
    TPckg<TBool> retValPckg(aHasContent);
    TInt err = SendReceive(EAlfTextureHasContent, TIpcArgs(&retValPckg, aId, aManagerId));		                            
    
    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "RAlfClient::TextureHasContent return error %d", err )
        }	
    
    return err;	  
    }

// ---------------------------------------------------------------------------
// Sends texture information notification request to alfred server.
// ---------------------------------------------------------------------------
//
void RAlfClient::TextureNotifyInfo( TRequestStatus& aStatus, TDes8& aDest )
    {
    SendReceive( EAlfNotifyTextureInfo, TIpcArgs( &aDest ), aStatus );
    }

// ---------------------------------------------------------------------------
// Cancels texture information notification request.
// ---------------------------------------------------------------------------
//
void RAlfClient::TextureCancelNotifyInfo()
    {
    SendReceive( EAlfCancelNotifyTextureInfo, TIpcArgs() );
    }

// ---------------------------------------------------------------------------
// Sets params for texture preferred size calculation.
// ---------------------------------------------------------------------------
//
TInt RAlfClient::TextureSetAutoSizeParams( TInt aId, TInt aManagerId, 
    const TAlfTextureAutoSizeParams& aParams )
    {
    TPckg<TAlfTextureAutoSizeParams> paramsPckg(aParams);

    TInt err = SendReceive(EAlfTextureSetAutoSizeParams, 
        TIpcArgs(aId, aManagerId, &paramsPckg) );
    
    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "RAlfClient::TextureSetAutoSizeParams return error %d", err )
        }	
    
    return err;	                            
    }

//----------------------------------------------------------------------------
// Notify about visibility of an application this instance is running
// ---------------------------------------------------------------------------
// 
void RAlfClient::ApplicationIsForeground(TBool aIsPartiallyOrFullyForeground)
    {
    SendReceive(EAlfNotifyAppVisibility, TIpcArgs(aIsPartiallyOrFullyForeground));
    }
  
// ---------------------------------------------------------------------------
// Continue refresh
// ---------------------------------------------------------------------------
//   
void RAlfClient::EnvContinueRefresh()
    {
    Send(EAlfEnvContinueRefresh, TIpcArgs() );
    }
   
// ---------------------------------------------------------------------------
// Pause refresh
// ---------------------------------------------------------------------------
//    
void RAlfClient::EnvPauseRefresh()
    {
    Send(EAlfEnvPauseRefresh, TIpcArgs() );
    }

// ---------------------------------------------------------------------------
// Refresh call back
// ---------------------------------------------------------------------------
//  
void RAlfClient::EnvRefreshCallBack()
    {
    Send(EAlfEnvRefreshCallBack, TIpcArgs() );
    }

// ---------------------------------------------------------------------------
// Get renderer type.
// ---------------------------------------------------------------------------
//    
TInt RAlfClient::EnvRenderer() const
    {
    TInt retVal = 0;
    TPckg<TInt> retValPckg(retVal);
    TInt err = SendReceive(EAlfEnvRenderer, TIpcArgs(&retValPckg));		                            
    
    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "RAlfClient::Renderer return error %d", err )
        }	
    
    return retVal;	          
    }

// ---------------------------------------------------------------------------
// Set idle treshold
// ---------------------------------------------------------------------------
//    
void RAlfClient::EnvSetIdleThreshold(TInt aMilliseconds)
    {
    TInt err = SendReceive(EAlfEnvSetIdleThreshold, TIpcArgs(aMilliseconds));
    
    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "RAlfClient::EnvSetIdleTreshold return error %d", err )
        }	
    
    }

// ---------------------------------------------------------------------------
// 
// --------------------------------------------------------------------------
// 
void RAlfClient::CancelPointerEvents()
    {
    SendReceive(EAlfCancelPtrEvents);
    }

// ---------------------------------------------------------------------------
// 
// --------------------------------------------------------------------------
// 
void RAlfClient::RequestPointerEvents(TDes8& aEventAsDescriptor,TRequestStatus& aStatus)
    {
    SendReceive(EAlfGetPointerEvent, TIpcArgs(&aEventAsDescriptor), aStatus);
    }

// ---------------------------------------------------------------------------
// 
// --------------------------------------------------------------------------
// 
void RAlfClient::SetFullScreenDrawing(TBool aEnable)
    {
    Send(ESetFullScreenDrawing, TIpcArgs(aEnable));
    }
        
// ---------------------------------------------------------------------------
// 
// --------------------------------------------------------------------------
// 
TInt RAlfClient::LayoutMetricsTextStyleData(TInt& aFontCategory, TInt aTextStyle)
    {
    TPckg<TInt> retValPckg(aFontCategory);
    TInt err = SendReceive(EAlfLayoutMetricsTextStyleData, TIpcArgs(&retValPckg, aTextStyle));		                                
    return err;	          
    }
    
// ---------------------------------------------------------------------------
// 
// --------------------------------------------------------------------------
// 
void RAlfClient::CancelSystemEvents()
    {
    SendReceive(EAlfCancelSystemEvents);
    }

// ---------------------------------------------------------------------------
// 
// --------------------------------------------------------------------------
// 
void RAlfClient::RequestSystemEvents(TDes8& aEventAsDescriptor,TRequestStatus& aStatus)
    {
    SendReceive(EAlfGetSystemEvent, TIpcArgs(&aEventAsDescriptor), aStatus);
    }

void RAlfClient::SetWgParent(TInt aParentId)
    {
    Send(EAlfSetWgParent, TIpcArgs(aParentId));
    }


// ---------------------------------------------------------------------------
// 
// --------------------------------------------------------------------------
//  
TInt RAlfClient::RosterSetPointerDragTreshold( 
     TInt aCntrlHandle, const TAlfXYMetric& aXYMetric, TInt aDisplayHandle )
    {
    TAlfPointerEventDragTreshold params;
    params.iControlHandle = aCntrlHandle;
    params.iXYMetric = aXYMetric;
    params.iDisplayHandle = aDisplayHandle;
    
    TPckg<TAlfPointerEventDragTreshold> paramsPckg(params);
    TInt err = SendReceive(EAlfRosterSetPointerDragTreshold, TIpcArgs(&paramsPckg) );   

    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "RAlfClient::RosterSetPointerDragTreshold return error %d", err )
        }
    return err;
    }

// ---------------------------------------------------------------------------
// 
// --------------------------------------------------------------------------
//  
TInt RAlfClient::RosterDisableLongTapEventsWhenDragging( 
     TInt aCntrlHandle, TBool aDisable, TInt aDisplayHandle )
    {    
    
    TInt err = SendReceive(EAlfRosterDisableLongTapEventsWhenDragging, TIpcArgs(aCntrlHandle, aDisable, aDisplayHandle) );   

    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "RAlfClient::RosterSetPointerDragTreshold return error %d", err )
        }
    return err;
    }

// ---------------------------------------------------------------------------
// Send synchronous command
// ---------------------------------------------------------------------------
//
void RAlfClient::SendCmd(TUint aCmd, TDes8& aBuf)
    {
    Send(aCmd, TIpcArgs(&aBuf));
    }

// ---------------------------------------------------------------------------
// Send asynchronous command
// ---------------------------------------------------------------------------
//
void RAlfClient::SendAsyncCmd(TUint aCmd, TDes8& aBuf, TRequestStatus& aStatus)
    {
    SendReceive(aCmd, TIpcArgs(&aBuf), aStatus);
    }

// ---------------------------------------------------------------------------
// Send command
// ---------------------------------------------------------------------------
//
TInt RAlfClient::SendSyncCmd(TUint aCmd, TDes8& aBuf)
    {
    return SendReceive(aCmd, TIpcArgs(&aBuf));
    }

// ---------------------------------------------------------------------------
// Appends the command in the buffer.
// On any error case, the caller is expected to call "synch with reply" instead.
// ---------------------------------------------------------------------------
//
TInt RAlfClient::SendNoReply(TUint aOp, const TDesC8& aInputBuf, TInt aSubSessionHandle)
    {
    if ( iAutoFlushMode == EAlfAutoFlushInstant )
        {
        // this will generate a function calls flush and "send with reply"
        return KErrGeneral;
        }
    
    // Make sure the default buffer is allocated
    if ( !iDefaultBatchBuffer )
        {
        iDefaultBatchBuffer = HBufC8::New( KAlfClientDefaultBatchBufferSize );
        if ( !iDefaultBatchBuffer )
            {
            return KErrNoMemory;
            }
            
        // start adding commands on the default buffer
        iUsedBatchBuffer = iDefaultBatchBuffer;
        }
        
    TPtr8 prtToUsedBuffer = iUsedBatchBuffer->Des();
    
    TInt3 param( aOp, aSubSessionHandle, aInputBuf.Length() );
    TPckgC<TInt3> paramPckg( param );
    
    // check max length
    const TInt paddedParamsPckgLength = AlfPaddedLength( paramPckg.Length() );
    const TInt paddedInputBufLength = AlfPaddedLength( aInputBuf.Length() );
    const TInt lengthNeededByTheCommand = 
        paddedParamsPckgLength + paddedInputBufLength;
    
    // check if we can fit in the used buffer
    if ( lengthNeededByTheCommand > prtToUsedBuffer.MaxLength() - prtToUsedBuffer.Length() )
        {
        // we cannot fit. Let's try to allocate more buffer:
        TInt allocError = AllocateExtendedBuffer( lengthNeededByTheCommand );
        
        if ( allocError != KErrNone )
            {
            if ( iAutoFlushMode != EAlfAutoFlushOnlyForced )
                {
                FlushBatchBuffer();
                }
            }
        }
   
    // the used buffer might have changed in the previous if-statement
    prtToUsedBuffer.Set( iUsedBatchBuffer->Des() );
    
    // check if we can fit in the used buffer - again
    if ( lengthNeededByTheCommand > prtToUsedBuffer.MaxLength() - prtToUsedBuffer.Length() )
        {
        // we still do not fit...
        return KErrOverflow;
        }
    
    prtToUsedBuffer.Append( paramPckg );
    AppendPadding( 
        prtToUsedBuffer, 
        paddedParamsPckgLength - paramPckg.Length() ); 
    prtToUsedBuffer.Append( aInputBuf );
    AppendPadding( 
        prtToUsedBuffer, 
        paddedInputBufLength - aInputBuf.Length() );
    
    __ASSERT_DEBUG( 
        !( prtToUsedBuffer.Length() % KAlfClientServerRoundToBoundary ), 
        USER_INVARIANT() );
    
    if ( !iAutoFlusher && iAutoFlushMode == EAlfAutoFlushDeferred )
        {
        iAutoFlusher = new CBatchBufferAutoFlusher(*this);
        if ( !iAutoFlusher )
            {
            // if cannot allocate auto-flusher, flush now
            FlushBatchBuffer();
            return KErrNone;
            }
        }
        
    if ( iAutoFlusher )
        {
        iAutoFlusher->StartAutoFlush();
        }
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// Returns the flush mode
// ---------------------------------------------------------------------------
//
TAlfAutoFlushMode RAlfClient::AutoFlushMode() const
    {
    return iAutoFlushMode;
    }

// ---------------------------------------------------------------------------
// Sets the flush mode
// ---------------------------------------------------------------------------
//
void RAlfClient::SetAutoFlushMode( TAlfAutoFlushMode aAutoFlushMode )
    {
    if ( iAutoFlushMode == aAutoFlushMode )
        {
        // no action required
        return;
        }
    
    FlushBatchBuffer();
    iAutoFlushMode = aAutoFlushMode;
    
    switch( iAutoFlushMode  )
        {
        case EAlfAutoFlushOnlyForced:
            delete iAutoFlusher;
            iAutoFlusher = NULL;
            break;
            
        case EAlfAutoFlushInstant:
            // these are not needed 
            delete iDefaultBatchBuffer;
            iDefaultBatchBuffer = NULL;
            
            iUsedBatchBuffer = NULL;
            
            delete iAutoFlusher;
            iAutoFlusher = NULL;
            break;
            
        default:
            break;
        }
        
    __ALFLOGSTRING1( "RAlfClient::SetAutoFlushMode New flush mode %d", aAutoFlushMode )
    }

// ---------------------------------------------------------------------------
// Flushes the command buffer
// ---------------------------------------------------------------------------
//
TInt RAlfClient::FlushBatchBuffer()
    {
    TInt err = KErrNone;
    if ( iUsedBatchBuffer && iUsedBatchBuffer->Length() )
        {
        TPtr8 ptr(iUsedBatchBuffer->Des());
        err = SendSyncCmd( EAlfDoSubSessionBatchCmd, ptr );
        
        // all the commands in the buffer are handled -> it can be emptied
        delete iExtendedBatchBuffer;
        iExtendedBatchBuffer = NULL;
        
        iDefaultBatchBuffer->Des().Zero();
        
        iUsedBatchBuffer = iDefaultBatchBuffer;
        
        // the iDefaultBatchBuffer could be deleted here, but we might end up
        // fragmenting the memory
        }
        
    if ( iAutoFlusher ) 
        {
        iAutoFlusher->Cancel();
        }
    
    // the iAutoFlusher could be deleted here, but we might end up
    // fragmenting the memory
    
    return err;
    }

// ---------------------------------------------------------------------------
// Sets the flush buffer size
// ---------------------------------------------------------------------------
//
void RAlfClient::SetMaxBatchBufferSize( TInt aBufferSize )
    {
    FlushBatchBuffer();
   
    if ( aBufferSize == KAlfBatchBufferSizeNoLimit )
        {
        iUsedMaxBufferSize = KAlfBatchBufferSizeNoLimit;
        }
    else if ( aBufferSize == KAlfBatchBufferSizeLimitToSystemDefault )
        {
        // KAlfClientDefaultBatchBufferSize will be used
        iUsedMaxBufferSize = KAlfClientDefaultBatchBufferSize;
        }
    else
        {
        // The buffer >= KAlfClientDefaultBatchBufferSize always
        iUsedMaxBufferSize = Max( aBufferSize, KAlfClientDefaultBatchBufferSize );
        }
    }

// ---------------------------------------------------------------------------
// Tries to allocate the extended buffer
// ---------------------------------------------------------------------------
//
TInt RAlfClient::AllocateExtendedBuffer( TInt aMoreSpaceNeeded )
    {
    TPtr8 prtToUsedBuffer = iUsedBatchBuffer->Des();
    
    // Note that max buffer size may be slightly larger than
    // iUsedMaxBufferSize, since when you create instance using
    // HBufC8::NewL( len ) its max size may be larger than len.
    if ( ( iUsedMaxBufferSize != KAlfBatchBufferSizeNoLimit ) &&
         ( prtToUsedBuffer.MaxLength() >= iUsedMaxBufferSize ) )
        {
        // Cannot make make buffer any bigger
        return KErrGeneral;
        }
 
    // Go for twice the required size.
    TInt newExtendedSize = 
        ( prtToUsedBuffer.Length() + aMoreSpaceNeeded ) * 2;
    if ( iUsedMaxBufferSize != KAlfBatchBufferSizeNoLimit )
        {
        // There is a max limit - we need to balace with it
        newExtendedSize = Min( newExtendedSize, iUsedMaxBufferSize );
        }
    
    HBufC8* newBuffer = HBufC8::New( newExtendedSize );
    if ( !newBuffer )
        {
        return KErrNoMemory;
        }
    
    // Copy from the old one
    TPtr8 newBufferPtr = newBuffer->Des();
    newBufferPtr.Copy( *iUsedBatchBuffer );
    
    // Clear the old one
    if ( iUsedBatchBuffer == iExtendedBatchBuffer )
        {
        // delete old extended buffer
        delete iExtendedBatchBuffer;
        iExtendedBatchBuffer = NULL;
        }
    else
        {
        iDefaultBatchBuffer->Des().Zero();
        }
        
    // set the pointers point to the new buffer
    iExtendedBatchBuffer = newBuffer;
    iUsedBatchBuffer = iExtendedBatchBuffer;
    
    __ALFLOGSTRING1( "RAlfClient::AllocateExtendedBuffer New buffer size %d", newExtendedSize )
    
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// Appends padding to the command
// ---------------------------------------------------------------------------
//    
inline void RAlfClient::AppendPadding( TDes8& aBuffer, TInt aAmount )
    {
    if ( aAmount )
        {
        // Modify length
        TInt length = aBuffer.Length();
        aBuffer.SetLength( length + aAmount );
        
        // Zero bytes used for padding
        do 
            {
            aAmount--;
            aBuffer[ length ] = 0;
            length++;
            }
        while ( aAmount );
        }
    }

// ---------------------------------------------------------------------------
// Post Qt buffer received from Qt port
// ---------------------------------------------------------------------------
//

TInt RAlfClient::PostQtBuffer( TInt aWsWgId, TInt aWsClientId, TDes8& aCommandBuffer,
        TAlfCommandBufferStatus aStatus )
    {
    TAlfQtCommandBufferParams params;
    params.iWsClientId = aWsClientId;
    params.iPtr = (TAny*)aCommandBuffer.Ptr();
    params.iLength = aCommandBuffer.Length();
    params.iBufferStatus = aStatus;
    params.iVisualHandle = 0;
    params.iWsWgId = aWsWgId;
    TPckg<TAlfQtCommandBufferParams> paramsPckg(params);    
  
    TInt err = SendReceive( EAlfQtCommandBuffer, TIpcArgs(&paramsPckg) );

    if ( err != KErrNone )
       {
       __ALFLOGSTRING1( "RAlfClient::PostQtBuffer return error %d", err )
       }
    return err;
    }

// ---------------------------------------------------------------------------
// Returns the requested info about the batch buffer
// ---------------------------------------------------------------------------
//    
TInt RAlfClient::GetBatchBufferInfo( TAlfBatchBufferInfoType aBufferInfoType ) const
    {
    switch ( aBufferInfoType )
        {
        case EAlfBatchBufferInfoDefaultBufferSize:
            return KAlfClientDefaultBatchBufferSize;
    
        case EAlfBatchBufferInfoMaxSize:
            return iUsedMaxBufferSize == KAlfClientDefaultBatchBufferSize ? 
                KAlfBatchBufferSizeLimitToSystemDefault : iUsedMaxBufferSize;
    
        case EAlfBatchBufferInfoCurrentlyAllocated:
            return iUsedBatchBuffer ? iUsedBatchBuffer->Des().MaxLength() : 0;
    
        case EAlfBatchBufferInfoCommandsAllocate:
            return iUsedBatchBuffer ? iUsedBatchBuffer->Length() : 0;
           
        default:
            return 0;
        }
    }

// end of file
 
