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
* Description:   ?Description
*
*/



#ifndef __ALFWINDOW_H__
#define __ALFWINDOW_H__

#include <e32base.h>
#include <alfwindowstructs.h>
#include <bitstd.h>

class CAlfWindowManager;
class TAlfWServInfo;
class CAlfWindowBuffer;
class CAlfWindowFxPlugin;
class TAlfWindowAttributes;
class TAlfWindowCommandBufferAttributes;

class TAlfWindowData;
    
/**
 *  CAlfWindow
 *
 * The class is used for abstracting different pixel data presentations
 *
 *  @lib alfappservercore.dll
 *  @since S60 5.0
 */
NONSHARABLE_CLASS(CAlfWindow): public CBase
    {
public:

    static CAlfWindow* NewL(TUint aId, CAlfWindowManager& aOwner, const TSize& aSizeInPixels);
    
    virtual ~CAlfWindow();
        
public: 
    // getters
    
    /**
     * WsInfo
     *
     * @since S60 5.0
     * @return  Specific wserv window data. For reference only.
     */
    IMPORT_C const TAlfWServInfo& WsInfo() const;
    
    /**
     * Position
     *
     * @since S60 5.0
     * @return  Position of the window
     */
    IMPORT_C TPoint Position() const;
    
    /**
     * Size
     *
     * @since S60 5.0
     * @return  The size of the window
     */
    IMPORT_C TSize Size() const;
    
    /**
     * Opacity
     *
     * @since S60 5.0
     * @return  The opacity of the window
     */
    IMPORT_C TReal32 Opacity() const __SOFTFP;
    
    /**
     * Rotation
     *
     * @since S60 5.0
     * @return  The rotation of the window
     */
    IMPORT_C TReal32 Rotation() const __SOFTFP;
    
    /**
     * Identifier
     *
     * @since S60 5.0
     * @return  The identifier of the window
     */
    IMPORT_C TUint Identifier() const;
    
    /**
     * IsActive
     *
     * @since S60 5.0
     * @return  ETrue, if window is active. Otherwise EFalse.
     */
    IMPORT_C TBool IsActive() const;
    
    /**
     * SurfaceExtent
     *
     * @since S60 5.0
     * @return  The extend of the composition surface
     */
	IMPORT_C TRect SurfaceExtent() const;
	
    /**
     * Size
     *
     * @since S60 5.0
     * @return  The index of the window. 0 means the topmost window.
     */
    IMPORT_C TUint Index() const;
    
    /**
     * InBuffer
     * This method returns the original buffer from wserv. Plugin must not modify the contents of it
     * Contents of this buffer is updated before callback CAlfWindowPlugin::WindowUpdated() is called. 
     *
     * Ownership of a buffer remains in AlfWindowManager
     *
     * @since S60 5.0
     * @return  CAlfWindowBuffer
     */
    IMPORT_C CAlfWindowBuffer* InBuffer();   
    
    /**
     * OutBufferL
     * This method returns new buffer for transition plugin to modify content that will finally display
     * on screen. There can't be more than one active outbuffers for a window at a time. Content of an
     * input buffer from wserv will be copied only upon creation of back buffer. After initialization, 
     * Fx plugin has solely responsibility to update complete frame.
     * 
     * Ownership of a buffer remains in AlfWindowManager
     *
     * @since S60 5.0
     * @param   aCopyInBufContents defines whether current frame should be copied to buffer
     * @return  CAlfWindowBuffer
     */
    IMPORT_C CAlfWindowBuffer* OutBufferL(TBool aCopyContents = EFalse);
    
    // Setters
    /**
     * Size
     *
     * TODO:CHECK COMMENT VALIDITY
     * Note, that SetACtive and SetOpacity(0) are different things. When window is not active, it is not 
     * processed at all in the refresh loop unlike a transparent window would be. 
     * 
     * @since S60 5.0
     * @return  The size of the window
     */
    IMPORT_C void SetActive(TBool aVisible);
    

    /**
     * SetSurfaceExtent
     *
     * Set the extent of the related composition surface
     * To remove extent, give aRect a value (0,0,0,0).
     * 
     * @since S60 5.0
     * 
     * @param aRect  New extent
     */
	IMPORT_C void SetSurfaceExtent(const TRect& aRect);

	
    /**
     * SetLayerUsesAplhaFlag
     *
     * Set compostion layer's EUsesAlpha flag state
     * Window that has background surface set, has layer
     * in composition scene
     * 
     * @since S60 5.0
     * 
     * @param aEnabled  True/False
     */	
	IMPORT_C void SetLayerUsesAplhaFlag(TBool aEnabled);
	
    /**
     * ContentReady
     *
     * @since S60 5.0
     * @param aOutBuffer
     * @param aUpdatedArea
     */
    IMPORT_C void ContentReady(const TRegion& aUpdatedArea);
    
    /**
     * Move
     * 
     * Sets to order of the windows. 0 equals the topmost window.
     *
     * @since S60 5.0
     * @param aIndex    New index of the window
     */
    IMPORT_C void Move(TUint aIndex);
    
    /**
     * EnableIndexChangedNotifications
     *
     * @since S60 5.0
     * @param aEnable
     */
    IMPORT_C void EnableIndexChangedNotifications(TBool aEnable);
   
    /**
     * SetPos
     *
     * Set the position of the window
     * 
     * @since S60 5.0
     * 
     * @param aPos  New position
     * @param aDuration Duration to new position
     */
    IMPORT_C void SetPos(TPoint aPos, TUint aDuration = 0);
    
    /**
     * SetSize
     *
     * @since S60 5.0
     * @param aSize  New size
     * @param aDuration Duration to new size
     */
    IMPORT_C void SetSize(TSize aSize, TUint aDuration = 0);
    
    /**
     * SetRotation
     *
     * @since S60 5.0
     * @param aAngle New rotation angle
     * @param Duration to new rotation angle
     */
    IMPORT_C void SetRotation(TReal32 aAngle, TUint aDuration = 0) __SOFTFP;
    /**
     * Size
     *
     * @since S60 5.0
     * @param aOpacity New opacity value
     * @param Duration to new opacity value
     */
    IMPORT_C void SetOpacity(TReal32 aOpacity, TUint aDuration = 0) __SOFTFP;

    /**
     * Window area
	 * @param aOrigin origin of shape.
	 * @param aRegion window shape.
     */
    void SetWindowArea(const TPoint& aOrigin, const TRegion& aRegion);
    
	/**
	 * Reset current window area.
	 */
    void SetWindowArea();

    /**
	 * Set transparency alpha channel.
	 * @param aActive ETrue if transparency alpha channel enabled, EFalse if not.
	 */
    void SetTransparencyAlphaChannel(TBool aActive);

    /**
	 * Returns transparency alpha channel.
	 */
    TBool TransparencyAlphaChannel() const;

    /**
     * Returns ETrue if included to visibility calculation. EFalse otherwise.
     */
    TBool IsIncludedToVisibilityCalculation() const;
    
    /**
     * Include to visibility calculation.
     */
    void IncludeToVisibilityCalculation(TBool aActive);
    
    public: // utils
    
    /**
	 * SetFlag
	 *
	 * @param flag
	 */
	 void SetFlag( TInt aFlag );

    /**
	 * ClearFlag
	 *
	 * @param flag
	 */    
    void ClearFlag( TInt aFlag );
    
    /**
     * SetWsInfo
     *
     * @since S60 5.0
     * @param aInfo
     */
    void SetWsInfo(const TAlfWServInfo& aInfo);
    
    /**
     * CompareIds
     *
     * Compares window id and window group identifier with the given window. 
     * 
     * @since S60 5.0
     * @return ETrue, if ids match. Otherwise EFalse.
     */
    static TBool CompareIds( const TWindowIdentifier* aWServId, const CAlfWindow& aWindow )
        {
        return aWServId->Equals(aWindow.WsInfo().iClientSideId);
        }

    /**
     * IsInsertedToScene
     * 
     * @since S60 5.0
     * @return ETrue, if visual window has been inserted to the scene. (meaning, Set operations will be posted forward)
     */
    TBool IsInsertedToScene();

    /**
     * InsertedToScene
     * 
     * @param aInserted New scene state for this window
     */ 
    void InsertedToScene(TBool aInserted);
    
    /**
     * Should the window be set inactive when it is released
     */
    TBool ShouldBeDeactivated(); 

    /*
     * PostPartialBuffer
     * 
     * Posts partial command buffer towards CHuiCanvas. Buffer is not processed until the final part is delivered.
     * 
     * @param aPtr  Pointer to the beginning of the command buffer in the shared junk
     * @param aSize Length of the commands for this frame of a window
     */ 
    void PostPartialBuffer( 
            TAny* aPtr, 
            TInt aSize, 
            TBool aPartial, 
            TBool aLastPart, 
            TBool aEmptyBuffers, 
            TInt aNodeFlags );
    
    /*
     * PostBuffer
     * 
     * Posts the command buffer towards CHuiCanvas. Previous buffers are cleared.
     * 
     * @param aPtr  Pointer to the beginning of the command buffer in the shared junk
     * @param aSize Length of the commands for this frame of a window
     */ 
    void PostBuffer( TAny* aPtr, TInt aSize, TInt aNodeFlags );
    
    // RnD only
    CFbsBitGc* GetBitGcL();
    
    void CommitGc();
    
    void SetNodeTracking( TInt aValue );

private:
    
    CAlfWindow();
    
    void ConstructL(TUint aId, CAlfWindowManager& aOwner, const TSize& aSizeInPixels);

    TAlfWindowAttributes* CreateWindowAttributes(TInt& aIndex);

    TAlfWindowCommandBufferAttributes* CreateWindowCommandBufferAttributes(TInt& aIndex);

    TAny* CreateSetWindowAreaL(TInt& aOffset, TInt aScreenNumber, 
        const TPoint& aOrigin, const TRegion& aRegion);

    void CreateInBufferL();

private:
    
    /**
     *  Own.
     */
    TAlfWindowData* iData;
    
    };

#endif
