/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Client class for the alfredserver.exe
*
*/



#ifndef R_ALFCLIENT_H
#define R_ALFCLIENT_H

#include <alf/alfconstants.h>
#include <alf/alfclientbase.h>
#include "alf/alfvisual.h"
#include "alf/alflayout.h"
#include "alf/alfenv.h"
#include "alf/alftexture.h"
#include "alf/alfimage.h"
#include "alf/alfroster.h"
#include "alf/alfbatchbuffer.h"

#include <AknsItemID.h>
#include <gdi.h>

/**
 *  Alfred client.
 *
 *  @lib alfclient.lib
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS( RAlfClient ): public RAlfClientBase
    {
    
public:

    /**
     * Constructor
     */
    RAlfClient();
    
    /**
     * Disconnect from the server. Must be called before destruction (not just Close() )
     */ 
    void Disconnect();
    
// Env

    /**
     * Sets the refresh mode.
     *
     * @Todo: should be protected with capability or deprecated completely(?)
     * @param aMode  New refresh mode.
     *
     * @see TAlfRefreshMode
     */
    void EnvSetRefreshMode( TAlfRefreshMode aMode );
    
    /**
     * @Todo: should be protected with capability or deprecated completely(?)
     * Sets the maximum allowed frame rate. Specify 0.0 to use the default limit.
     *
     * @param aFrameRate  New maximum frame rate in frames per second.
     */
    void EnvSetMaxFrameRate( TReal32 aFrameRate );
    
    /**
     * Continues normal display refreshing. If display refreshing is already
     * being done, calling this has no effect. This is called automatically
     * from inside the toolkit when something is done that may lead to a
     * visible change on a display. The application may also call this if it
     * has previously paused refresh with PauseRefresh().
     */
    void EnvContinueRefresh();

    /**
     * Pauses display refreshing temporarily. This is called when it is clear
     * that any displays will not be updated soon. The application may call
     * this at times when it needs more processing time for other tasks.
     */
    void EnvPauseRefresh();
    
    /**
     * Called on manual refresh.
     */
    void EnvRefreshCallBack();
    
    /**
     * Determines which renderer is currently in use. Always returns the real
     * explicit identifier of the renderer, and not an ambiguous identifier
     * like "Default".
     */
    TInt EnvRenderer() const;
    
    /**
     * Sets the interval before an idle notification is sent. 
     *
     * @param aSeconds  Threshold in milliseconds.
     */
    void EnvSetIdleThreshold(TInt aMilliseconds);
   
// Roster

    /**
     * Shows given control group in the roster using given display.
     *
     * @param aCntrlGroupHandle Handle to the server side object.
     * @param aWhere Where on the roster (top/under/middle/...)
     * @param aDisplayHandle Handle to the server side object.
     * @return Error code.
     */
    TInt RosterShow( TInt aCntrlGroupHandle, TInt aWhere, TInt aDisplayHandle );
    
    /**
     * Hides given control group.
     *
     * @param aCntrlGroupHandle Handle to the server side object.
     * @param aDisplayHandle Handle to the server side object.
     * @return Error code.
     */
    TInt RosterHide( TInt aCntrlGroupHandle, TInt aDisplayHandle );
    
    TInt RosterShowVisual( TInt aVisualHandle, TInt aDisplayHandle );
    TInt RosterHideVisual( TInt aVisualHandle,  TInt aDisplayHandle );
    TInt RosterMoveVisualToFront( TInt aVisualHandle, TInt aDisplayHandle );
    
    TInt RosterSetPointerEventObservers( TInt aFlags, TInt aCntrlHandle, TInt aDisplayHandle );
    TInt RosterAddPointerEventObserver( TAlfPointerEventFlags aObserver, TInt aCntrlHandle, TInt aDisplayHandle );
    TInt RosterRemovePointerEventObserver( TAlfPointerEventFlags aObserver, TInt aCntrlHandle, TInt aDisplayHandle );

    TInt RosterSetPointerDragTreshold( TInt aCntrlHandle, const TAlfXYMetric& aXYMetric, TInt aDisplayHandle );
    TInt RosterDisableLongTapEventsWhenDragging( TInt aCntrlHandle, TBool aDisable, TInt aDisplayHandle );
    
// Static

// Texture 
    
    /**
     * Updates owner id.
     * This method must be called before calling other texture related methods.
     * @return Error code.
     */
    TInt TextureUpdateOwnerId();
    
    /**
     * Creates new texture. Leaves on error.
     *
     * @param aId ID of the texture.
     * @param aFlags Texture flags.
     * @param aFilename the filename of the image
     * @return Error code.
     */
    TInt TextureCreateAnimatedL(TInt aId, TAlfTextureFlags aFlags, TInt aManagerId, const TDesC& aFilename);

    /**
     * Creates new texture. Leaves on error.
     *
     * @param aId ID of the texture.
     * @param aBitmapHandle Handle to the bitmap.
     * @param aMaskBitmapHandle Handle to the bitmap mask.
     * @param aFlags Texture flags.
     * @param aManagerId Id of texture manager
     * @return Error code.
     */
    TInt TextureCreateL( TInt aId, TInt aBitmapHandle, TInt aMaskBitmapHandle, TAlfTextureFlags aFlags, TInt aManagerId );

    /**
     * Loads texture. Leaves on error.
     *
     * @param aId ID of the texture.
     * @param aBitmapHandle Handle to the bitmap.
     * @param aMaskBitmapHandle Handle to the bitmap mask.
     * @param aFlags Texture flags.
     * @param aManagerId Id of texture manager
     * @return Error code.
     */
	TInt TextureLoadL( TInt aId, TInt aBitmapHandle, TInt aMaskBitmapHandle, TAlfTextureFlags aFlags, TInt aManagerId  );

    /**
     * Unloads given texture. 
     *
     * @param aId ID of the texture.
     * @param aManagerId Id of texture manager
     * @return Error code.
     */
	TInt TextureUnload( TInt aId, TInt aManagerId );

    /**
     * Releases given texture.
     * @param aId ID of the texture.
     * @param aManagerId Id of texture manager
     * @return Error code.
     */
	TInt TextureRelease( TInt aId, TInt aManagerId );

    /**
     * Restores given texture.
     * @param aId ID of the texture.
     * @param aManagerId Id of texture manager
     * @return Error code.
     */
	TInt TextureRestore( TInt aId, TInt aManagerId );

    /**
     * Notifies skin change for given texture.
     * @param aId ID of the texture.
     * @param aManagerId Id of texture manager
     * @return Error code.
     */
	TInt TextureNotifySkinChanged( TInt aId, TInt aManagerId );

    /**
     * Blurs given texture. 
     *
     */
	TInt TextureProcessorBlurL( const TInt aServerSideSrcHandle, 
	    TInt aServerSideDstHandle, 
	    TInt aManagerId, 
	    const TSize& aPreferredSize, 
	    TInt aFilterSize, 
	    TInt aFlag );

    /**
     * Checks if given texture has content. 
     *
     */
	TInt TextureHasContent( TBool& aHasContent, TInt aId, TInt aManagerId );

    /**
     * Delete given texture. 
     *
     * @param aId ID of the texture.
     * @param aManagerId Id of texture manager
     * @return Error code.
     */
	TInt TextureDelete( TInt aId, TInt aManagerId );

    /**
     * Starts to animate the texture if the texture
     * was animated.
     *
     * @param aId ID of the texture.
     */
    TInt TextureStartAnimation( TInt aId );
    
    /**
     * Stops the texture animation if the texture
     * was animated.
     *
     * @param aId ID of the texture.
     */
    TInt TextureStopAnimation( TInt aId );

    /** 
     * Notifies texture information.
     * @param aStatus request status.
     * @param aDest destination buffer which will contain events.
     */
    void TextureNotifyInfo( TRequestStatus& aStatus, TDes8& aDest );

    /**
     * Cancels texture information notification.
     */
    void TextureCancelNotifyInfo();

    /**
     * Sets parameters for the preferred size calculation.
     * 
     * @param aId ID of the texture.
     * @param aManagerId Id of texture manager
     * @param aParams auto size paramters.
     */
    TInt TextureSetAutoSizeParams( TInt aId, TInt aManagerId, const TAlfTextureAutoSizeParams& aParams );

// Misc

    /**
    * Notify server about this application foreground status
    */ 
    void ApplicationIsForeground(TBool aIsPartiallyOrFullyForeground);
    
    void RequestPointerEvents(TDes8& aEventAsDescriptor, TRequestStatus& aStatus);

    void CancelPointerEvents();
    
    void SetFullScreenDrawing(TBool aEnable);
    
    TInt LayoutMetricsTextStyleData(TInt& aFontCategory, TInt aTextStyle);
    
    void RequestSystemEvents(TDes8& aEventAsDescriptor, TRequestStatus& aStatus);

    void CancelSystemEvents();
    
    void SetWgParent(TInt aParentId);
    
    /** 
     * Generic command
     */
	void SendCmd(TUint aCmd, TDes8& aBuf);
    
    /** 
     * Generic synchronous command
     */
	TInt SendSyncCmd(TUint aCmd, TDes8& aBuf);

    /** 
     * Generic asynchronous command
     */
	void SendAsyncCmd(TUint aCmd, TDes8& aBuf, TRequestStatus& aStatus);
     
     
    TInt SendNoReply(TUint aOp, const TDesC8& aInputBuf, TInt aSubSessionHandle);
    
    TAlfAutoFlushMode AutoFlushMode() const;
    void SetAutoFlushMode( TAlfAutoFlushMode aAutoFlushMode );
    TInt FlushBatchBuffer();
    void SetMaxBatchBufferSize( TInt aBufferSize );
    TInt GetBatchBufferInfo( TAlfBatchBufferInfoType aBufferInfoType ) const;
    
   /** 
    * Post QT command buffer to server side canvas
    */
    TInt PostQtBuffer( TInt aWsWgId, TInt aWsClientId, TDes8& aCommandBuffer, 
           TAlfCommandBufferStatus aStatus );
private:
    TInt AllocateExtendedBuffer( TInt aMoreSpaceNeeded );
    inline static void AppendPadding( TDes8& aBuffer, TInt aAmount );
    
private:

    HBufC8* iDefaultBatchBuffer; // owned
    HBufC8* iExtendedBatchBuffer; // owned
    HBufC8* iUsedBatchBuffer; // not owned. points to iDefaultBatchBuffer or iExtendedBatchBuffer
    
    class CBatchBufferAutoFlusher;
    CBatchBufferAutoFlusher* iAutoFlusher;
    
    TAlfAutoFlushMode iAutoFlushMode;
    TInt iUsedMaxBufferSize;
    };


#endif // R_ALFCLIENT_H
