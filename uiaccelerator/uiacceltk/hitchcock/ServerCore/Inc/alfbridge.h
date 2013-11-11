/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Server declaration, internal for Nokia
*
*/



#ifndef __ALF_BRIDGE_H__
#define __ALF_BRIDGE_H__

#include <e32hashtab.h>

#define AMT_CONTROL() static_cast<CAlfModuleTestDataControl*>(Dll::Tls())
#include "alfmoduletest.h" 

#include "alfscreen.h"
#include "alfstreamerbridge.h"
#include "alfdecoderserverclient.h"

#include <alf/alfconstants.h>

#include "HuiFxEffect.h"
#include "huisynchronizationobserver.h"
#include <alf/AlfTransEffectPlugin.h>
#include <uiacceltk/HuiDisplay.h>

class CAlfEffectEndTimer;
class CAlfCommandDebug;
class CAlfLayoutSwitchEffectCoordinator;
class RMemReadStream;
class CHuiCanvasVisual;
class CFullScreenEffectState;
class CControlEffectState;
    
const TInt KAlfBridgeRegionGranularity = 10;

NONSHARABLE_CLASS(CAlfBridge): 
	public CBase, 
    public MAlfStreamerListener, 
    public MHuiDisplayRefreshObserver, 
    public MHuiBitmapProvider,
    public MAlfGfxEffectObserver,
    public MHuiSynchronizationObserver
    {
	// Helper class for keeping ongoing effects in order. Kept in iEffectCleanupStack
    private:

        struct TEffectCleanupStruct
            {
            public:
                TEffectCleanupStruct(
                        TInt aHandle,
                        CHuiVisual* aEffectedVisual,
                        CHuiVisual* aTemporaryPresenterVisual,
                        TBool aIsLayout, 
                        TBool aHideWhenFinished,
                        TBool aCanDestroyOrHideImmediately,
                        TBool aIsFullScreenEffect) 
                : iHandle(aHandle), 
                iEffectedVisual(aEffectedVisual),
                iTemporaryPresenterVisual(aTemporaryPresenterVisual),
                iIsLayout(aIsLayout),
                iHideWhenFinished(aHideWhenFinished),
                iCanDestroyOrHideImmediately(aCanDestroyOrHideImmediately),
                iIsFullScreenEffect(aIsFullScreenEffect)
                        {
//                        RDebug::Printf("TEffectCleanupStruct - 0x%x 0x%x, %d", iEffectedVisual, iTemporaryPresenterVisual, iIsLayout );
                        };

                TInt iHandle;

                TBool iIsLayout;

                TBool iHideWhenFinished;
                
                TBool iCanDestroyOrHideImmediately;

                TBool iIsFullScreenEffect;
                
				// not own, unless iEffectedVisual has EShouldDestroy flag enabled
                CHuiVisual* iEffectedVisual;
				// owned
                CHuiVisual* iTemporaryPresenterVisual;
            };
            
public:

    static CAlfBridge* NewL( CAlfStreamerBridge** aHost, CHuiEnv* aEnv );
    ~CAlfBridge();
    
	void AddNewScreenL(CAlfSharedDisplayCoeControl* aSharedCoeControl);
	void AddNewScreenFromWindowL(RWindow* aWindow);
    /**
     * This method returns the index of first alf controlgroup in the roster.
     * 
     */
    TInt FirstAlfControlGroupIndex( TInt aScreenNumber );
        
    /**
     * This method returns the index of last alf controlgroup in the roster.
     * 
     */
    TInt LastAlfControlGroupIndex( TInt aScreenNumber );        
    
    /**
     * Sets flag to indicated that visual tree has changed (in every screen).
     * 
     */
    void SetVisualTreeVisibilityChanged(TBool aChanged);
    
    /**
     * Solution to reduce active visuals until we handle window tree hierarchy correctly
     */
    void HandleVisualVisibility( TInt aScreenNumber );

    /**
     * Solution to reduce active visuals until we handle window tree hierarchy correctly. 
	 *  Note: Recursive method
     */
    TBool HandleLayoutVisualVisibility(
            CHuiLayout* aLayout,
            CHuiControlGroup& aControlGroup,
            CHuiControl& aControl,
            TBool& aFullscreenCovered, 
            TRect& aFullscreen,
            CAlfScreen* aScreen,
            TBool& aSubtreeVisible,
            TBool& aHasVisualsWithLayers,
            TBool aChildCanBeOpaque, TBool aOnlyForEmbeddedAlfApp = EFalse );

    /**
     * Updates layer visibilities.
     */
    void HandleLayerVisibility(
            CHuiLayout* aLayout,
            CHuiControlGroup& aControlGroup,
            CHuiControl& aControl,
            TBool aVisible );
    
    /**
     * This method shows the control group in roster, however the aWhere param
     * is modified so that controlgroup appears at the right position depending on
     * controlgroup type
     *
     */
    void ShowControlGroupL(
            CHuiRoster& aRoster, 
            CHuiControlGroup& aGroup, 
            TInt aWhere, 
            TInt aScreenNumber );

    /**
     * This method sets the windowgroup id for the alf window.
     * 
     */
    void SetAlfWindowGroupId(TInt aAlfWindowGroupId);
    
    CHuiDisplay* Display(TInt aScreenNum);
    
    /**
     * AlfAppUi delegates the QT command buffer posting functionality to bridge 
     * through this function. It identifies the corrcet CHuiCanvasVisual, 
     * from the RWindow::ClientId(), that it has got from client
     * and posts the command buffer to that visual. 
     *
     * @param params  QTParams containg window information and command buffer
     * 
     */
    void PostQTCommandBufferL( TAlfQtCommandBufferParams params );
       
    /**
     * Set the client window where the visual or layout will be drawn to.
     * @param aWindowGroupId Window group id.
     * @param aClientWindowHandle Client side window handle.
     * @param aVisual Visual or layout.
     */
    void SetClientWindowForDrawingL(TInt aWindowGroupId, TInt aClientWindowHandle, CHuiVisual* aVisual);

    // This is the final callback that does the actual work
    // AlfGfxEffectEndCallBack starts a timer, and when the timer finishes,
    // it calls this function

    void TransitionFinishedHandlerL( TInt aHandle );
    
    // from  MAlfGfxEffectObserver
    
  	/**
 	 * Function to be called when CHuiFxEffect class effect has ended
  	 * @param aHandle handle of the effect, given when the effect is started
  	 */
    void AlfGfxEffectEndCallBack( TInt aHandle );
   
    TInt FindClientWindowGroupId( TInt aScreenNumber, CHuiControlGroup& aControlGroup );
    TInt FindWindowGroupNodeId( TInt aScreenNumber, CHuiControlGroup& aControlGroup ) const;
    
    void CleanAllFxVisuals();

    /**
     * Helper function to abort fullscreen effect
     * @param aClientRequest ETrue if requested by client.
     */
    void HandleGfxStopEvent( TBool aClientRequest );
    
    void EnableSwRenderingL(TBool aEnable = ETrue);
    TBool PrepareSwRenderingTarget( CAlfScreen* aScreen );
    void InitializeSwRenderingTarget(CFbsBitmap* aBitmap);

    /**
     * Sets memory level.
     * @param aMemoryLevel memory level.
     */
    void SetMemoryLevel(THuiMemoryLevel aMemoryLevel);
    
    /**
     * Sets low memory. This is used by this class.
     * @param aEnabled ETrue if enabled, EFalse if disabled.
     */
    void SetLowMemory(TBool aEnabled);
    
    /**
     * Requests to force SW rendering to be used.
     * @param aEnabled enabled status.
     * @return error code.
     */
    TInt ForceSwRendering(TBool aEnabled);
    
    /**
     * Chooses appropriate memory level and informs
     * core toolkit.
     */
    void DoUpdateMemoryLevel();
    
    /**
     * Modify all effects due to low memory.
     */
    void OnLowMemoryModifyAllEffects();

    /**
     * Gets size & rotation.
     * @param aSize size.
     * @param aRotation rotation.
     * @return error code.
     */
    TInt GetSizeAndRotation(TSize& aSize, TInt& aRotation);
    
    /**
     * Reads pixels to bitmap.
     * @param aBitmap bitmap.
     * @return error code.
     */
    TInt ReadPixels(CFbsBitmap* aBitmap);

    /**
      * Sets HuiControlGroup as Alf application window group
      */
    void SetWindowGroupAsAlfApp(TInt aId);

    /**
      * removes Alf application window group tag
      */
    void RemoveWindowGroupAsAlfApp(TInt aId);
    
    // From MHuiSynchronizationObserver
    void Synchronized(TInt aId);
    
    /*
     * GfxTriggerEndFullScreen
     * 
     * GfxTransEffect API gives EndFullScreen events too late. Thus there is a two stage process for triggering 
     * the EndFullScreen effect after BeginFullScreen event arrived.
     * 
     */
    void GfxTriggerEndFullScreen(CFullScreenEffectState* aFullScreenEffectData);

    /**
	 * UpdateSecondaryEffectL
	 *
	 * Resolve filename for two part effect that is used with slowly starting applications
	 * 
	 */
    TBool UpdateSecondaryEffectL(const CFullScreenEffectState& aState);

   /*
	*	GfxTriggerEffectWhenFullScreenDrawn
	*
	*	Goes through the given CHuiControl (group which is supposed to show the effect). 
	*   Triggers the effect, if  drawing buffers in group cover the whole screen in the 
	*   current orientation.
	*
	*	@param	aToGroup	Group to be analyzed
	*/
    TBool GfxTriggerEffectWhenFullScreenDrawn(CHuiControlGroup* aToGroup = NULL);
    
	// Experimental
	TBool IsLayoutSwitchReady( TInt aDuration );
    TBool IsLayoutSwitchReadyRecursive(
            CHuiLayout* aLayout,
            CHuiControlGroup& aControlGroup,
            CHuiControl& aControl,
            TBool& aFullscreenCovered, 
            const TRect& aFullscreen,
            CAlfScreen* aScreen,
            TBool aChildCanBeOpaque, 
            TBool aOnlyForEmbeddedAlfApp,
            TInt aOrientation,
            TInt aDuration,
            TBool& aCoverageRegionModified );

	
    void LayoutSwitchStart();
    void LayoutSwitchComplete();
    
    RAlfBridgerClient* BridgerClient();

    /**
     * Refresh requested. This is meant to be used by alfstreamerbridge when
     * frame has ended.
     */
    void RefreshNow(TBool aSyncWait);

    /**
     * Asynchronous refresh when in SW rendering mode.
     */
    void AsynchRefresh();

private:    
    
    
    CAlfBridge( CAlfStreamerBridge** aHost );
    
    void ConstructL(CHuiEnv* aEnv);
    
	/**
	* RegisterFadeEffectL
	*
	* Register existing fade effect from MMC (RND ONLY) or ROM.
	*/
    void RegisterFadeEffectL();

    /**
	* DoSetCachedFadeEffectL
	*
	* Resolves the fade effect filename. Speeds up fade effect loading.
	*/    
    void DoSetCachedFadeEffectL();
    
	void HandleDestroyDisplay( TInt aScreenNumber );
	
	void HandleRegisterEffectL( TAlfBridgerData data );
	
	/**
	* DoRegisterEffectL
	*
	* Register effect filename to a certain action in HuiFxEngine
	*/
	void DoRegisterEffectL(const TDesC& aFilename, TInt aAction);
	
	// effect handling
	void HandleGfxEffectsL( TAlfBridgerData data );

    // remove effect
    // This is needed if we have to abort an effect
    void HandleGfxStopEffectsL( TAlfBridgerData data );
    
    void HandleGfxStopControlEffectsL( TAlfBridgerData data );
    
	/**
 	 *  HandleSetDistractionWindowL
	 *  
	 *  Define or undefine, window that should be discarded in fullscreen
	 *  heuristic analysus.
	 *
	 *  Used by avkon to mark CAknLocalScreenClearer window.
	 */
    void HandleSetDistractionWindowL( TAlfBridgerData data );
	
	/**
	*	RemoveEffectFromApp
	*
	*	Removes effects on appui.
	*/
    void RemoveEffectFromApp(TInt aSecureId, TInt aWgId = -1 );
    
    // component effect handling
    void HandleGfxControlEffectsL( TAlfBridgerData data );

   /**
	*	StoreLayoutIfRequiredByEffectL
	*
	*   @param aNeededStoredLayout	Returns ETrue, if storing is required. Otherwise EFalse.
    *
	*	@return ETrue	If storing was required and could be done.
    *					OR storing was not required. 
	*/    
    TBool StoreLayoutIfRequiredByEffectL(CHuiLayout* aLayout, CFullScreenEffectState& aEvent, TBool& aNeededStoredLayout);

    /**
     * Handles begin and end fullscreen events
     */
    TBool HandleGfxEventL(CFullScreenEffectState& aEvent, CHuiLayout* aToLayout, CHuiLayout *aFromLayout);
    
    
    /*
     * ResolveAfterEffectAppearingApplicationL
     * 
     * In case of an exit effect the appearing application is not notifed to alf
     * by the GfxTransEffect API. In such case it is being resolved from Roster by
     * taking the application directly under the application that is being moved
     * to the background (when called by HandleReorderWindow) or being destroyed
     * (called by DeleteControlGroupL).
     * 
     * In some situation Alf may end up with wrong appearing application uid. In such
     * case the worst that can happen, is that we must wait the frameworks
     * EndFullScreen event to arrive.
     */
    void ResolveAfterEffectAppearingApplicationL(CHuiControlGroup* aGroup);

    
	/**
	*	FreezeLayoutUntilEffectDestroyedL
	*
	*	Tries to take screenshot of the layout. If that fails, the layout will not be effected at all.
	*/
    void FreezeLayoutUntilEffectDestroyedL(CHuiLayout* aLayout, TInt aHandle);

    /**
     * Handles control effect events
     */    
    void HandleGfxControlEventL(CControlEffectState& aEvent, CHuiCanvasVisual* aCanvasVisual);
    
    /*
     * SetupEffectLayoutContainerL
     * 
     * This method creates an own layout into iFullScreenEffectControlGroup. Then it creates
     * temporary presenter visual visuals for each visual in aSourceLayout and binds them
     * using SetExternalContentL. Finally the effect entry is added to iEffectCleanupStack, 
     * thus it can be cleaned after effect completion.
	 *
	 *
	 *	@return	ETrue, 	if layout have been initialized succesfully for the effect
	 			EFalse,	if any visuals have been removed. Effect should not be applied.
     */
    TBool SetupEffectLayoutContainerL(TInt aHandle,CHuiLayout* aSourceLayout, TBool aIsFullScreenEffect, TBool aIsExitEffect, TBool aCanDestroyOrHideImmediately);

    /*
     *	AddFxItemL
	 *
	 *	Prepares visual for the effect. Removes previous effect if necessary. @aItemDestroyed will return EFalse,
	 *	if the visual has been destroyed during the previous effect.
	 *	
	 *	@param	aItemDestroyed	
     */
    CHuiCanvasVisual* AddFxItemL(
            TInt aEffectHandle,
            CHuiVisual* aSourceVisual, 
            CHuiLayout* aTargetLayout, 
            CHuiControl* aEffectControl,
            TBool aInsertTemporaryVisual,
            TBool& aItemDestroyed,
            TBool aIsFullScreenEffect,
            TBool aIsExitEffect = EFalse,
            TBool aCanDestroyOrHideImmediately = EFalse
            );
            

    /*
     *	AddToEffectLayoutContainerL
     */
    void AddToEffectLayoutContainerL(
            TInt aEffectHandle,
            CHuiLayout* aSourceLayout, 
            CHuiLayout* aTargetLayout,
            CHuiControl* aEffectControlGroup,
            TInt& aItemsDestroyed,
            TBool aIsFullScreenEffect,
            TBool aAddLayout = ETrue,
            TBool aIsExitEffect = EFalse,
            TBool aCanDestroyOrHideImmediately = EFalse);

    /*
     *	ListFamilyTreeL
     */   
    void ListFamilyTreeL( RPointerArray<CHuiLayout>& aArray, const CHuiLayout* aLayout );

    /*
     * CleanFxItem
     * 
     * Effects are shown indirectly by a another visual, that is in iFullScreenEffect control group. 
     * This methods unbinds the external content and the removes temporary presenter visual 
	 * from iFullScreenEffect group and . It does not delete the CHuiFxEffect 
	 * object associated with the content visual. See also iEffectCleanupStack.  
     */
    TBool CleanFxItem(TEffectCleanupStruct& aEffectItem);

    /*
     * FindTemporaryPresenterLayout
     * 
     * Finds temporary presenter layout based on a visual.
     *
     * @param   aVisual 
     * 
     * @return  NULL, if visual does not have temporary presenter visual in the active effect list
     */
    CHuiLayout* FindTemporaryPresenterLayout(CHuiVisual* aVisual);
    
    /*
     * FindLayoutByEffectHandle
     * 
     * Application exit effect for application that does not stay background is destroyed and removed
     * from normal control groups. FindLayoutByEffectHandle is used to find the correct group 
     * in order to apply the exit effect on already "destroyed" application. 
     *
     * @param   aHandle     Effect handle.
     * 
     * @return  NULL, if visual does not have temporary presenter visual in the active effect list
     */
    CHuiLayout* FindLayoutByEffectHandle(TInt aHandle);
    
    /**
     * HasActiveEffect
	 *
	 *	Note. The visual might not have effect, but it is child for visual that has effect. 
	 *	In both cases, this method returns ETrue. Otherwise EFalse.
	 *
	 *	@param	aVisual	Visual to be checked for effect participation.
     */
    TBool HasActiveEffect(CHuiVisual* aVisual);

    /**
     * HasActiveEffect
	 *
	 *	Note. The visual might not have effect, but it is child for visual that has effect. 
	 *	In both cases, this method returns ETrue. Otherwise EFalse.
	 *
	 *	@param	aVisual	Visual to be checked for effect participation.
	 *	@param  aIndex	Index of the effect item in iEffectCleanupStack, if found. Otherwise KErrNotFound
     */    
    TBool HasActiveEffect(CHuiVisual* aVisual, TInt& aIndex);
    
   /*
	*	FindEffectHandle
	*
	*	@return 	Returns the effect handle number, which is associated with this visual. If none found, then 
	*               returns KErrNotFound.
	*/
    TInt FindEffectHandle(CHuiVisual* aVisual);
    /*
     * CleanFxVisual
     * 
     * Effects are shown indirectly by a another visual, that is in iFullScreenEffect control group. 
     * This methods unbinds the external content and the removes temporary presenter visual 
	 * from iFullScreenEffect group and . It does not delete the CHuiFxEffect 
	 * object associated with the content visual. See also iEffectCleanupStack.  
     */
    TBool CleanFxVisual(CHuiVisual* aVisual, TInt aHandle = KErrNotFound);
    
    /*
     *	CleanFxVisuals
     *	
     *	See above. 
     */
    TBool CleanFxVisuals();
    
	void HandleNewWindowL( TAlfBridgerData& aData );
	
	void DestroyWindow(CHuiVisual* aVisual, TBool aForce = EFalse);
	
	void HandleDestroyWindowL( TAlfBridgerData& aData );
	
	void HandleSetWindowPosL( TAlfBridgerData& aData );
	
	void HandleSetWindowSizeL( TAlfBridgerData& aData );
	
	void HandleSetWindowRotationL( TAlfBridgerData& aData );
	
	void HandleSetWindowOpacityL( TAlfBridgerData& aData );
	
	void HandleSetTransparencyAlphaChannelL( TAlfBridgerData& aData );
	
	void HandleIncludeToVisibilityCalculationL( TAlfBridgerData& aData );
	
	void HandleSetWindowAreaL( TAlfBridgerData& aData );
	
	void HandleReorderWindowL( TAlfBridgerData& aData );
	
	void HandlePostCanvasBufferL( TAlfBridgerData& aData );
	
	void HandleSetWindowFlagL( TAlfBridgerData& aData, TInt aOp );
	
	void SetWindowActiveL(CHuiVisual* aVisual, TBool aActive);
	
	void HandleSetWindowActiveL( TAlfBridgerData& aData );
	
	void HandleSetSurfaceExtentL( TAlfBridgerData& aData );
	
	void HandleLayerUsesAlphaFlagChanged( TAlfBridgerData& aData );
	
	void HandleGetNativeWindowDataL( TAlfBridgerData& aData );

	void HandleSetCoveringBitmapL( TAlfBridgerData& aData );
	
	void HandleSetCursorDataL( TAlfBridgerData& aData );
	
	void HandleSetNodeTracking( TAlfBridgerData& aData );
	
	void HandleSetFadeEffectL( TAlfBridgerData& aData );
	
	void HandleMoveWindowToNewGroupL( TAlfBridgerData& aData );

    void HandleSetLayoutSwitchEffectL();
	
    void ClearCanvasVisualCommandSets(TBool aInactiveOnly);
    static void ClearCanvasVisualCommandSetsRecursive(CHuiCanvasVisual* aVisual, TBool aInactiveOnly);
    
    void ClipVisualRect(TRect& aRect, const TRect& aClippingRect);

    TBool IsRectCoveredByRegion(TRect aRect, TRegion& aCoveringRegion);

    void ShowSessionContainerControlGroupL(
            CHuiRoster& aRoster, 
            CHuiControlGroup& aGroup, 
            TInt aWhere, 
            TInt aScreenNumber );

    void ShowWindowGroupControlGroupL(CHuiRoster& aRoster, CHuiControlGroup& aGroup, TInt aWhere, TInt aScreenNumber );
    
	/**
	 * HasActiveAlfContent
	 *
	 * Determine if the window group in question has active Alf visuals
	 *
	 *	@return 	ETrue, if active clients
	 */
    TBool HasActiveAlfContent( TInt aClientWgId );
    
private:
         
    /**
     * Provide a bitmap and mask from the given UID.
     * This method should create a bitmap and its mask, usually by loading it from disk.
     *
     * @param aId      TextureManager id of this bitmap, may be used for extracting
     *                 an icon resource.
     * @param aBitmap  Output parameter for the provider: On return, has to store 
     *                 a pointer to the generated bitmap. Ownership transferred to caller.
	 *                 The support for input CFbsBitmap formats varies between 
	 *                 different renderers. See \ref cfbsbitmapsupport 
	 *                 Table describing renderer bitmap support.      
	 * @param aMaskBitmap  
	 *                 Output parameter for the provider: On return, has to store 
     *                 a pointer to the generated alpha mask or <code>NULL</code> 
     * 				   if there is no mask. Ownership of the bitmap is transferred 
     *                 to the caller.
     * 
     * @see CHuiTextureManager::CreateTextureL()
     */
    virtual void ProvideBitmapL(TInt /*aId*/, CFbsBitmap*& aBitmap, CFbsBitmap*& aMaskBitmap);
    
    /**
     * InsertVisualL
     *  
     * @param  aKey    The key object of type K to add to the array.
     * @param  aVisual  The value object of type V to associate with aKey.
     * @leave  KErrNoMemory if memory could not be allocated to store the copies of aKey and aValue.
     */
    void AddVisual( TInt aKey, TInt aClientSideId, TInt aClientSideGroupId, CHuiCanvasVisual* aVisual );
    
    /**
     * RemoveVisual
     * 
     * @param  aKey    The key to be removed.
     * @return         KErrNone if the key object and corresponding value object were
     *                 removed successfully. KErrNotFound if the key object was not present in the array.
     */
    void RemoveVisual( TInt aWindowNodeId );
    
    /**
     * FindVisual
     * 
     * AddVisual and FindVisual set the iPreviouslySearchedVisual, which speeds
     * up the sequential search of the same visual.
     * 
     * @param  aKey    The key to be removed.
     * @return         NULL if the key object is not found. Otherwise returns 
     *                 the pointer to visual corresponding the key.
     */    
    CHuiCanvasVisual* FindVisual( TInt aKey );
 
     /**
     * FindVisualByClientSideIds
     * 
     * Finds a visual based on the provided client side ids. 
	 *
     * @param  aClientSideId    The key to be removed.
     * @param  aClientSideId    The key to be removed.
     * @return         NULL if the visual was not not found. Otherwise returns 
     *                 the pointer to visual corresponding the client side keys.
     */       
    CHuiCanvasVisual* FindVisualByClientSideIds(TUint32 aClientSideId, TUint32 aClientSideGroupId );

    /**
     * This method finds controlgroup which has been assosiated with given window group id. 
     * Control group may or may not be active in roster.
     *
     * @param aWindowGroupNodeId Node id of the window group, internal.
     */
    CHuiControlGroup* FindControlGroup(TInt aWindowGroupNodeId, TInt aScreenNumber );

    /**
     * This method finds controlgroup which has been assosiated with window server window group id. 
     *
     * @param aWindowGroupNodeId window server window group id of the window group, internal.
     * @param aScreenNumber Screen number where to search, if not known KErrNotFound
     *        at return will contain the screen number where the group was found
     * @return pointer to the window group if found, NULL if not found.
     */
    CHuiControlGroup* FindControlGroupByWindowGroupId( 
            TInt aWindowGroupId, 
            TInt& aScreenNumber, 
            TAlfControlGroupEntry** aAlfGroup = NULL );

    /**
     * Finds control group which matches the given secure ID. For more detailed search, 
     * you can provide the client window group ID.
     * 
     * @param aSecureId Secure ID to seacrh for
     * @param aWgId Client window group ID. Ignored if -1.
     * @return Pointer to found control gruop. NULL if not found.
     */
    CHuiControlGroup* FindControlGroupBySecureId( TInt aSecureId, TInt aWgId = -1 ) const;
    
    /**
     * Finds control gruop which matches the full screen effect end state.
     * 
     * @return Pointer to found control gruop. NULL if not found.
     */
    CHuiControlGroup* FindControlGroupByFullScreenToEffect() const;
    
    /**
     * Finds control gruop which matches the full screen effect start state.
     * 
     * @return Pointer to found control gruop. NULL if not found.
     */
    CHuiControlGroup* FindControlGroupByFullScreenFromEffect() const;
    
    /**
     * This method deletes controlgroup which has been assosiated with given window group id. 
     * Control group may or may not be active in roster.
     *
     * @param aWindowGroupNodeId Node id of the window group, internal.
     */
    void DeleteControlGroupL(TInt aWindowGroupNodeId, TInt aScreenNumber );

    /**
     * Creates a control group
     *
     * @param aWindowGroupNodeId Node id of the window group, internal.
     * @param aClientWindowGroupId External window group id.
     */
    CHuiControlGroup& CreateControlGroupL(
            TInt aWindowGroupNodeId, 
            TInt aClientWindowGroupId,
            TInt aSecureId,
            TInt aScreenNumber );

    /**
     * This method prepares cached and HW accelerarted versions of texts & images from
     * canvas command buffers if those buffers has not yet been handled before. 
     */
    void PrepareCanvasVisualCaches( TInt aScreenNumber );

    /**
     * This method reorders alf controlgroups to just above alf servers
     * windowgroup.
     * 
     */

    void ReorderAlfControlGroupsL( TInt aScreenNumber );
        
    /**
     * Called to notify the observer that a display refresh is about to begin.
     * The observer should prepare its state for the display.
     */
    TInt ResolveScreenNumber( CHuiDisplay& aDisplay );
    
    void NotifyDisplayRefreshStarted(CHuiDisplay& aDisplay);

    /**
     * 
     */
    TBool IsVisualOpaque(CHuiVisual& aVisual);

    /**
     * Enables default transformation steps
     */
    void EnableVisualDefaultTransformationsL(CHuiVisual& aVisual);

    /**
     * 
     */
    void HandleCallback(TInt aStatus);
    
    /**
     * 
     */
    void DoDispatchL(TInt aStatus);

    void InsertImageBrushL(CHuiVisual& aVisual, TInt aBitmapHandle);
    void UpdateImageBrushL(CHuiVisual& aVisual, TInt aBitmapHandle);
    void RemoveImageBrushL(CHuiVisual& aVisual);

    
    void DebugPrintControlGroupOrder(
            CAlfScreen& aScreen, 
            CHuiRoster& aRoster, 
            CHuiControlGroup& aGroup);
    
    void VisualizeControlGroupOrderL(
            CAlfScreen& aScreen,
            CHuiRoster& aRoster, 
            CHuiControlGroup& aGroup);
    
    void PrintAllocMem();

    // Fading methods
    TBool SetupFadeEffectL(CHuiCanvasVisual& aVisual);
    void PrepareFadeEffects( CHuiCanvasVisual& aVisual );    
    TBool LoadFadeEffectsL( CHuiCanvasVisual& aVisual );        

    // Fading related utility methods
    TBool CanFadeChildren( CHuiCanvasVisual& aParent );
    TInt RecursiveChildCount( CHuiCanvasVisual& aParent, TInt aCanvasFlags );
    TBool IsFadedByParent( CHuiCanvasVisual& aVisual );
    TBool IsNearestParentEffectFade( CHuiCanvasVisual& aVisual );
    TBool HasActivePaintedAreas( CHuiCanvasVisual& aVisual, TBool aIncludeChildren );
    TBool HasActiveFadedChildren( CHuiCanvasVisual& aVisual );

    /*
     * This is for updating all the layout that are created to correspond the window server window groups.
     * This should be called immediately when layout switch happens. Layout extents must be update then otherwise
     * visibility calculations will clip to old sizes.
     */
    void UpdateRootVisualsToFullscreen();
    
private:

    RPointerArray<CAlfScreen> iAlfScreens;
    void SetCursorTimerL(TUint aTime = 0, CHuiVisual* aCursor = 0);
    TBool IsAlfOriginatedWindow(CHuiCanvasVisual& aVisual);
    
    
    TBool IsFullScreenDrawnRecursiveAlfContent(CHuiVisual* aVisual, TRect& aFullScreen);
    
   // Experimental
    TBool IsFullScreenDrawnRecursive(
            CHuiLayout* aLayout, 
            CHuiControlGroup& aControlGroup,
            CHuiControl& aControl,
            TBool& aFullscreenCovered, 
            TRect& aFullscreen,
            CAlfScreen* aScreen,
            TBool& aSubtreeVisible, 
            TBool aChildCanBeOpaque,
            TInt aOrientation);

    
    void MarkAllLayersHiddenRecursive(CHuiLayout* aLayout);
    
NONSHARABLE_CLASS ( TDeadControlGroup )
    {
public:
    TDeadControlGroup():
        iDeadGroup( 0 ),
        iScreen( 0 ),
        iLayout(NULL)
            {
            }
    TInt iDeadGroup;
    TInt iScreen;
    CHuiLayout* iLayout;
    };
    
NONSHARABLE_CLASS( TEffectControlGroupEntry )  
    {
public:
    TEffectControlGroupEntry() :    
        iHandle( KErrNotFound ),
        iVisual( NULL ),
        iEffect( NULL ),
        iAction( 0 ),
        iWindowGroupNodeId( 0 ),
        iClientWindowGroupId( 0 ),
        iScreenNumber( 0 )
            {
            }

    /** Handle of the attached effect */
    TInt iHandle;
    // Visual to which the effect has been attached */
    // This controls the case where a new effect replaces an old one.
    // If a new effect has been added to the same visual, the old one does not need
    // to be removed as adding the later effect has already removed the previous one.
    // Pointer not owned, value only saved for reference
    CHuiVisual* iVisual;
    // The effect that has been attached to the visual
    // Pointer not owned, value only saved for reference
    CHuiFxEffect* iEffect;
    // The action that is the cause of the effect (control appear or disappear)
    TInt iAction;
    /** Window group node id, internal */
    TInt iWindowGroupNodeId; 
    /** Window group id, external id seen e.g. by the application using wserv */
    TInt iClientWindowGroupId;
    TUint8 iScreenNumber;
    };

    // Array of window groups that have attached full screen effects
    RArray<TInt> iEffectWindowGroups;
    // Array of control groups that should have been deleted,
    // but have been left alive until the effect has run its course.
    RArray<TDeadControlGroup> iDeadControlGroups;
    // Array of individual controls that have attached effects
    RArray<TEffectControlGroupEntry> iEffectControls;
    
    // Array of callback data structures
    // The structures contain the handle that identifies the initiator of the effect
    // and a pointer to CAlfBridge instance to allow handling via static functions
    RArray<TInt> iEffectHandles;
        
    // iEffectCleanupStack contains the entries for effects that are currently associated 
    // with visuals in iFullscreenEffectControlGroup
    RArray<TEffectCleanupStruct> iEffectCleanupStack;
    
    // Same as iEffectCleanupStack, but only the ones that can be now cleaned away.
    // See method CleanFxVisuals.
    RArray<TInt> iFinishedCleanupStackEffects;
    
	
    /**
     * Full screen effect state.
     * Own.
     */
    CFullScreenEffectState* iFSFxData;
    
    /**
     *  Long app start effect consists of two effects. The app_start_long.fxml is 
     *  applied after a short timeout. Then system waits the EndFullScreen event is
     *  received or heuristics determine that new application has drawn itself 
     *  properly. When application is drawn, the original requested effect is applied. 
     *  
     *  iFSFXDataPart2 defines the original requested effect, while iFSFxData
     *  is used to define the coming or ongoing fullscreen app start effect.
     */
    CFullScreenEffectState* iFSFXDataPart2;
    
    /**
     * Control effect state.
     * Own.
     */
    CControlEffectState* iControlEffectData;
        
    // these save the current full screen transition data
    // so that the correct transition can be ended from the callback
    // when the effect ends
    
    CAlfEffectEndTimer* iEffectEndTimer;
    TBool iLayoutInitializedForExitEffect;
    
    CAlfStreamerBridge** iHost;

    TInt iAlfWindowGroupId;
    TInt iAlfWindowGroupNodeId;


    CFbsBitmap* iHack; // not excactly owned..
    CFbsBitmap* iDummyMask; // not excactly owned..
    
    TBool iActivated;
    RAlfBridgerClient iBridgerClient;
    
    class THashVisualStruct
        {
    public:
        THashVisualStruct( 
                CHuiCanvasVisual* aVisual, 
                TUint32 aClientSideId,
                TUint32 aClientSideGroupId) : 
            iVisual( aVisual ), 
            iClientSideId(aClientSideId),
            iClientSideGroupId(aClientSideGroupId)
                {};
        
    public:
        CHuiCanvasVisual* iVisual;  // not own
        TInt32 iClientSideId;
        TInt32 iClientSideGroupId;
        };
    
    RHashMap<TUint32,THashVisualStruct> iWindowHashArray;
    CHuiControl* iOrphanStorage; // owned. holds the visuals which are orphaned from their control group
    class TRegisteredEffectsStruct
        {
    public:
        TRegisteredEffectsStruct()
            {
            iAction = 0;
            iEffectFile = NULL;
            }
    public:
        TInt iAction;
        HBufC* iEffectFile; 
        };
		
public:
    
    RArray<TRegisteredEffectsStruct> iAlfRegisteredEffects;
    CHuiEnv* iHuiEnv;
	CAlfAppUi* iAppUi;
    CAlfLayoutSwitchEffectCoordinator* LayoutSwitchEffectCoordinator()
        {
        return iLayoutSwitchEffectCoordinator;
        }

private:    

    HBufC16* iFadeEffectFile;
    
	CHuiCanvasVisual* iPreviouslySearchedVisual;
    
	TInt iPreviouslySearchedVisualId;
	
	// For textcursor opacity
    CPeriodic* iCursorTimer;
    TUint iCursorInterval;
    CHuiVisual* iCursorVisual;

#ifdef SYMBIAN_BUILD_GCE    
    TBool iPrintFPS;
#endif

    CAlfCommandDebug* iCommandDebug;

    mutable RRegionBuf<KAlfBridgeRegionGranularity> iTempVisualRegion;
    mutable RRegionBuf<KAlfBridgeRegionGranularity> iTempIntersectingRegion;
    mutable RRegionBuf<KAlfBridgeRegionGranularity> iTempRegion;
    mutable RRegionBuf<KAlfBridgeRegionGranularity> iTempRegion2;
    
    TBool iBgSurfaceFound;
    TBool iInLowMemMode;
    TBool iLayoutSwitchInProgress;

    TBool iHomeScreenWallpaperWindowFound;
    TBool iBgAnimHidden;

    TBool iHomeScreenVisible;
    TInt iHomeScreenPSValue;
    
    CAlfLayoutSwitchEffectCoordinator* iLayoutSwitchEffectCoordinator;
    TInt iAlfSecureId; 	    
    TBool iSwRenderingEnabled;
    
    TBool iForcedSwRendering;
    TBool iLowMemoryMode;
    THuiMemoryLevel iCurrentMemoryLevel;
    
    TInt iIdForEAlfDSSynchronizeOp;
    TInt iIdForLayoutSwitchFrameSync;
    
    RArray<TInt> iAlfNativeClientsWgIds;
    
    #ifdef ALF_DEBUG_PRINT_WINDOWGROUP_ORDER
    TInt activevisualcount;
    TInt passivevisualcount;
    #endif
    
    CPeriodic* iManualRefreshTimer;
    };    

#endif // __ALF_BRIDGE_H__