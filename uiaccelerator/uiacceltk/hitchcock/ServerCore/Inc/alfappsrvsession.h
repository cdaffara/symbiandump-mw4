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
* Description:   Server session
*
*/



#ifndef C_ALFAPPSRVSESSION_H
#define C_ALFAPPSRVSESSION_H

#include <e32hashtab.h>
#include <alf/alfappsrvsessionbase.h>
#include <uiacceltk/HuiEnv.h>
#include <uiacceltk/huigifanimationtexture.h>
#include "malfsrvtexturemanagerclient.h"
#include "alfframestamp.h"

class CHuiControl;
class CHuiLayout;
class CAlfSrvEffectEnv;
class THuiTexturePreferredProperties;
class CAlfSrvTextureManager;

/**
 *  Session/service class in the server side.
 *
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS(CAlfAppSrvSession)
    : public CAlfAppSrvSessionBase, 
      public MWindowVisibilityObserver,
      private MAlfSrvTextureManagerClient
    {
public:
    
    /**
     * Destructor, singals server to free resources specific for this client
     */
    ~CAlfAppSrvSession();

    /**
     * Public, two phased constructor, stores pointer to server instance
     * 
     * @param aServer Server which the client has connected.
     */
    static CAlfAppSrvSession* NewL(const CAlfAppServer* aServer);
    
    
    /**
     * Gets the drawing area of the session
     * 
     * @return TRect of the drawing area
     */
    TRect ClientDrawingArea() const;
    
    /**
     * Sets the drawint area. This is set by the display sub session
     * 
     * @param aRect Drawing area rect.
     */
    void SetClientDrawingArea( const TRect& aRect );

    /**
     * Sets the drawint area. This is set by the display sub session
     * 
     * @param aRect Drawing area rect.
     */
    void SetClientDrawingAreaL( const TRect& aRect );
    
    /**
     * Is client application focused?
     * 
     * @return ETrue if client application is focused.
     */
    TBool IsFocused() const;
    
    /**
     * Sets session's max fps limiter to half, used if client's window does not have keyboard focus
     * @param aOnBackground use lowered max fps (ETrue/EFalse)
     */
    void SetBackgroundMaxFps( TBool aOnBackground );
              
// from CAlfAppSrvSessionBase
    /**
     * From CAlfAppSrvSessionBase
     * Called when a message is received from the client.
     * 
     * @param aMessage Message parameters.
     */
    void DoHandleCommandL(const RMessage2& aMessage);
    
    /**
     * From CAlfAppSrvSessionBase
     * Client's window group received focus.
     * 
     * @param aDoTransitionEffect Do fade in transision (RnD)
     */
    void FocusGainedL( TBool aDoTransitionEffect );
    
    /**
     * From CAlfAppSrvSessionBase
     * Client's window group lost focus.
     * 
     * @param aDoTransitionEffect Do fade out transision (RnD)
     * @return Did transition? (RnD)
     */
    TBool FocusLostL( TBool aDoTransitionEffect );
    
    /**
     * From CAlfAppSrvSessionBase
     * Returns the preferred window group posistion.
     * 
     * @return WG position
     */
    CAlfAppServer::TAlfWGPostion PreferredWindowGroupPosition() const;
    
    /**
     * Returns the preferred refresh mode of the session.
     *
     * @return Preferred refresh mode.
     */
    THuiRefreshMode PreferredRefreshMode() const;
    
    void ReOrderControlGroupSessionsL( RPointerArray<CAlfSrvSubSessionBase>& aGroupSessions );
    
 
// from MWindowVisibilityObserver 
    /**
     * From MWindowVisibilityObserver
     * Called when window becomes fully visible
     */
    void WindowFullyVisible();
    
    /**
     * From MWindowVisibilityObserver
     * Called when window becomes partially visible
     */
    void WindowPartiallyVisible();
    
    /**
     * From MWindowVisibilityObserver
     * Called when window losts visibility.
     */
    void WindowNotVisible();
    
    /**
     * From MWindowVisibilityObserver
     * Returns the control witch creates the window (CHuiDisplayCoeControl)
     * @return Contorol. NULL if not have one. Ownership not transferred.
     */
    CCoeControl* CoeControl();
    
private:

    /**
     * Default constructor, stores pointer to server instance
     * 
     * @param aServer Server which the client has connected.
     */
    CAlfAppSrvSession(const CAlfAppServer* aServer);

    /**
     * Second phase constructor
     */
    void ConstructL();

    void EnvSetRefreshModeL(const RMessage2& aMessage);
    void EnvSetMaxFrameRateL(const RMessage2& aMessage);
    void EnvContinueRefreshL(const RMessage2& aMessage);
    void EnvPauseRefreshL(const RMessage2& aMessage);
    void EnvRefreshCallBackL(const RMessage2& aMessage);
    void EnvSetIdleThresholdL(const RMessage2& aMessage);
    void EnvRendererL(const RMessage2& aMessage);
    //void BorderArrayAppendBrushL(const RMessage2& aMessage);
    void RosterHideL(const RMessage2& aMessage);
    void RosterShowVisualL(const RMessage2& aMessage);
    void RosterHideVisualL(const RMessage2& aMessage);
    void RosterMoveVisualToFrontL(const RMessage2& aMessage);
    void RosterSetPointerEventFlagsL(const RMessage2& aMessage);
    void RosterAddPointerEventObserverL(const RMessage2& aMessage);
    void RosterRemovePointerEventObserverL(const RMessage2& aMessage);
    void RosterSetPointerDragTresholdL(const RMessage2& aMessage);    
    void RosterDisableLongTapEventWhenDraggingL(const RMessage2& aMessage);
    void TextureCreateAnimatedL(const RMessage2& aMessage); 
    void TextureStartAnimation(const RMessage2& aMessage);
    void TextureStopAnimation(const RMessage2& aMessage);
    void TextureCreateL(const RMessage2& aMessage);
    void TextureLoadL(const RMessage2& aMessage);
    void TextureUnload(const RMessage2& aMessage);
    void TextureDelete(const RMessage2& aMessage);
    void TextureRelease(const RMessage2& aMessage);
    void TextureRestore(const RMessage2& aMessage);
    void TextureNotifySkinChanged(const RMessage2& aMessage);
    void TextureBlurL(const RMessage2& aMessage);
    void TextureHasContentL(const RMessage2& aMessage);
    void TextureSetAutoSizeParamsL(const RMessage2& aMessage);    
    void LayoutMetricsTextStyleDataL(const RMessage2& aMessage);

    void DoFocusLostEffect(TInt aEffect, CHuiVisual& aRootVisual );
    void DoFocusGainedEffect(TInt aEffect, CHuiVisual& aRootVisual );

    /**
     * Checks that texture owner id has been updated.
     * @param aMessage message.
     * @return ETrue if it's ok to continue, EFalse otherwise.
     */
    TBool RequireTextureOwnerId(const RMessage2& aMessage);
    
    /**
     * Updates @c iTextureOwnedId.
     * This method must be called successfully before @c TextureOwnerId 
     * method can be called.
     * @param aMessage message.
     */
    void UpdateTextureOwnerIdL(const RMessage2& aMessage);
    
    /**
     * Returns texture owner id.
     * This method must not be called before @c UpdateTextureOwnerIdL
     * has been called successfully.
     * @return texture owner id.
     */
    inline TProcessId TextureOwnerId() const;
    
    /**
     * Configure batch commands execution.
     * This is used by unit tests in order to test all branches.
     * @param aMessage message containing configuration.
     */
    void ConfigureBatchCommandExecutionL( const RMessage2& aMessage );
    
    /**
     * Executes batch commands.
     * @param aMessage message to be executed.
     */
    void ExecuteBatchCommandsL( const RMessage2& aMessage );

    /**
     * Executes batch commands using dynamic buffer, i.e. common buffer or memory allocated.
     * @param aMessage message to be executed.
     * @return ETrue if command was executed, EFalse if not.
     */
    TBool ExecuteBatchCommandsUsingDynamicBufferL( const RMessage2& aMessage );

    /**
     * Executes batch commands using fixed buffer, i.e. common buffer or stack allocated.
     * @param aMessage message to be executed.
     */
    void ExecuteBatchCommandsUsingFixedBufferL( const RMessage2& aMessage );

    /**
     * Executes commands using specified buffer.
     * @param aBuffer buffer to be used.
     * @param aMessageLength message length.
     * @param aMessage message to be executed.
     * @return amount of bytes executed commands took.
     */
    TInt ExecuteBatchCommandsUsingBufferL( 
        TDes8& aBuffer, TInt aMessageLength, const RMessage2& aMessage );
    
    /**
     * Execute commands from descriptor.
     * @param aDescriptor descriptor which contains commands.
     * @param aSpaceNeeded this will be updated to contain value
     *        indicating how much space is needed.
     * @return amount of bytes executed commands took.
     */
    TInt ExecuteCommandsFromDescriptorL( 
        const TDesC8& aSource, 
        TInt& aSpaceNeeded );

    /**
     * Checks whether it is allowed to execute using dynamic buffer.
     * @return ETrue if it's allowed, EFalse otherwise.
     */
    inline TBool AllowExecuteUsingDynamicBuffer() const;

    /**
     * Checks whether it is allowed to execute using common buffer.
     * @return ETrue if it's allowed, EFalse otherwise.
     */
    inline TBool AllowExecuteUsingCommonBuffer() const;

    /**
     * Returns reference to texture manager.
     * @return reference to texture manager.
     */
    inline CAlfSrvTextureManager& AlfTextureManager();

    /**
     * Starts waiting for texture information for client.
     */
    void NotifyTextureInfo( const RMessage2& aMessage );
    
    /**
     * Checks if this session has pending texture information to be
     * delivered to client side.
     */
    inline TBool HasPendingTextureInfo() const;
    
    /**
     * Tries to deliver texture info to client.
     */
    void DeliverTextureInfo();
   
    /**
     * Finds position of next texture info for delivery.
     * @param aPriority priority to look for (0 or 1).
     * @return position, or KErrNotFound if not found.
     */
    TInt FindNextTextureInfoForDelivery( 
        TInt aPriority, TInt aStartPos ) const;
    
    /**
     * Finds texture info structure position by texture id.
     * @param aTextureId texture id to be searched.
     * @return position, or KErrNotFound if not found.
     */
    TInt FindTextureInfoById( TInt aTextureId ) const;    

    /**
     * Removes texture info based on id.
     * @param aTextureId texture id
     */
    void RemoveTextureInfo( TInt aTextureId );
    
    void ShowControlGroupsInOrderL(CHuiDisplay& aDisplay);
    void StoreControlGroupOrderL(CHuiDisplay& aDisplay, TBool aAlsoHide );
    
   /**
    * Post QT drawing command buffer to CanvasVisual    
    */
    void PostQtCommandBufferL( const RMessage2& aMessage );
    
    
    /*
     *  Frame rate reporting enablers
     */
    void ReportFrameRateBegin( const RMessage2& aMessage );
    
    void ReportFrameRateEnd( const RMessage2& aMessage );
    
// from base class MAlfSrvTextureManagerClient
    
    virtual TBool GetTextureSize( 
        TInt aTextureId, 
        TSize& aTextureSize,
        TBool& aHasBeenDelivered );
    virtual void SetTextureSize(
        TInt aTextureId,
        const TSize& aTextureSize,
        TInt aPriority );
    virtual void RemoveTextureSize( TInt aTextureId );
    virtual void TextureSizeChangesCompleted();
    
    void EnvEnableLowMemoryState(const RMessage2& aMessage);
    void EnvForceSwRendering(const RMessage2& aMessage);
    void EnvGetSizeAndRotation(const RMessage2& aMessage);
    void EnvReadPixels(const RMessage2& aMessage);
        
private:

    // Owned items which need to be deleted with the session is terminated.
    RPointerArray<CBase> iOwnershipItems; 
    RPointerArray<CHuiTexture> iTextures;
    RPointerArray<CHuiGifAnimationTexture> iAnimatedTextures;
    
    // Is client the focused window group?
    TBool iFocused;   
    
    // Is client partially visible (matters only if iFocused = EFalse )
    TBool iPartiallyVisible;
    
    // Refresh mode
    THuiRefreshMode iRefreshMode;
    
    // Max frame rate
    TReal32 iMaxFrameRate;
    
    // Idle threshold
    TInt iIdleThreshold;
    
    TBool iUsesFullScreen;
    
    TRect iClientDisplayRect;
    
    CAlfSrvEffectEnv* iEffectEnv;

    /**
     * ETrue if @c iTextureOwnerId contains valid value.
     * EFalse otherwise.
     */
    TBool iTextureOwnedIdSet;    
    
    /**
     * Texture owner id.
     * This contains valid value only if @c iTextureOwnedIdSet
     * is set to true.
     */
    TProcessId iTextureOwnerId;
    
    enum TAlfExecutionFlags
        {
        // Allow execution using dynammic buffer.
        EAlfExecuteUsingDynamicBuffer = 0x1,
        // Allow execution using common buffer.
        EAlfExecuteUsingCommonBuffer = 0x2
        };
            
    // Current execution flags, see TAlfExecutionFlags.
    TInt iExecutionFlags;
    
    /**
     * Texture info observer event.
     */
    RMessagePtr2 iTextureInfoEvent;
    
    /**
     * Structure containing information about a texture.
     */
    struct TAlfTextureInfo
        {
        enum TAlfTextureInfoFlags
            {
            // Indicates that size has been changed
            // and new size should be (eventually)
            // be delivered to client.
            EAlfFlagTextureSizeChanged = 1,
            // Indicates that texture size has been
            // delivered to client.
            EAlfFlagTextureDelivered = 2
            };
        
        // Flags. Sum of subset of TAlfTextureInfoFlags.
        TInt8 iFlags;
        // Delivery priority. The smaller the value
        // the higher the priority.
        TInt8 iDeliveryPriority;
        // Client side texture id.
        TInt iTextureId;
        // New texture size.
        TSize iTextureSize;
        };

    /**
     * Array of texture information. 
     * Own.
     */
    RArray< TAlfTextureInfo > iTextureInfo;
    
    /**
     * Previous index used for iTextureInfo
     */
    mutable TInt iPreviousTextureInfoIndex;
    
	// Contains references to all ongoing FPS measurements
    RHashMap<TUint32,TFrameStamp> iFPSMeasurementArray;
    
    RPointerArray<CHuiControlGroup> iControlGroupOrder;
    };

#endif // C_ALFAPPSRVSESSION_H
