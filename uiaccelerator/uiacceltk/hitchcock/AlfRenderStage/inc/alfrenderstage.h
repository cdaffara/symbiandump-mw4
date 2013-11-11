/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   AlfRenderStage provides a method for redirect avkon drawing to a graphics accelerator
*
*/



#ifndef __ALFRENDERSTAGE_H__
#define __ALFRENDERSTAGE_H__

#define AMT_CONTROL() static_cast<CAlfModuleTestDataControl*>(Dll::Tls())
#include "alfmoduletest.h"

#include <graphics/wsrenderstage.h>
#include <graphics/wscursor.h>
#include "alfrsgc.h"
#include <alfhintapi.h>

#include <graphics/wsgraphicdrawer.h>
#include <e32hashtab.h>
#include <alfstreamerconsts.h>
#include "alfrssendbuffer.h"
#include "alfrenderstageutils.h"

class CAlfFader;
class CFbsBitGc;
class MWsGraphicDrawerEnvironment;
class CAlfRenderStage;
class CAlfAsynchGoomSession;

 
/**
 *  This is the implementation of CAlfRenderStage which is created by a CAlfRenderStageFactory
 *
 *  This class provides interface for applications to draw and manipulate the screen. It listenes
 *  window events and draw commands (passed via CAlfGraphicsContext) and serializes the data. After frame
 *  draw is complete, the serialized data is passed to AlfStreamerServer for further processing.
 *
 *  @lib alfrenderstage.lib
 *  @since S60 v5.2
 */

class CAlfRenderStage : 
    public CWsRenderStage, 
    public MWsEventHandler,
    public MWsTextCursor,
    public MAlfCompositionAgnosticWindowTreeObserver,
    public MAlfHintObserver,
    public MWsDrawAnnotationObserver,
    public MAlfSynchronizationInterface
        {
public:

    /**
     * NewL
     * 
     * Constructor
     * 
     * @param aEnv   
     * @param aScreen   
     * @param aScreenNumber   
     * @return  New instance of the class
     */
    static CAlfRenderStage* NewL(MWsGraphicDrawerEnvironment* aEnv, MWsScreen* aScreen, TInt aScreenNumber, CWsRenderStage* aNext);
	
    /**
     * C++ destructor
     */
    ~CAlfRenderStage();    

public: // New methods
        
    /**
     * EndCallBack
     *
     * Asynchronous SendBuffer
     * 
     * @param aFinished   
     */
    void EndCallBack( TRequestStatus* aFinished );

    /**
     * WsGraphicsContext
     *
     * Getter function
     * 
     * @return Graphics context
     */
    CAlfGraphicsContext* WsGraphicsContext();

    /**
     * AlfSendBuffer
     * 
     * Getter function
     * 
     * @return Send buffer
     */
    CAlfRsSendBuffer* AlfSendBuffer();

    /**
     * ScreenNumber
     * 
     * Getter function
     * 
     * @return Screen number
     */     
   TInt ScreenNumber() const;

public: // From CWsRenderStage

    TAny* ResolveObjectInterface(TUint aTypeId);
    void Begin(const TRegion* aRegion);
    void End(TRequestStatus* aFinished);	
public: // from MWsEventHandler

    /**
     * @note This will be soon obsolete.
     */
    void DoHandleEvent(const TWservCrEvent& aEvent);
    
public: // from MWsTextCursor
    
    void DrawTextCursor(const TTextCursorInfo& aTextCursorInfo);

public: // from MAlfCompositionWsWindowTreeObserver

	void NodeCreated(const MWsWindowTreeNode& aWindowTreeNode, MWsWindowTreeNode const* aParent);
	void NodeReleased(const MWsWindowTreeNode& aWindowTreeNode);
	void NodeActivated(const MWsWindowTreeNode& aWindowTreeNode);
	void NodeExtentChanged(const MWsWindowTreeNode& aWindowTreeNode, const TRect& aRect );
	void SiblingOrderChanged(const MWsWindowTreeNode& aWindowTreeNode, TInt aNewPos);
	void FlagChanged(const MWsWindowTreeNode& aWindowTreeNode, TInt aFlags, TBool aNewValue);
	void AttributeChanged(const MWsWindowTreeNode& aWindowTreeNode, TInt aAttribute );
	void FadeCountChanged(const MWsWindowTreeNode& aWindowTreeNode, TInt aFadeCount);
	void TransparentRegionChanged(const MWsWindowTreeNode& aWindowTreeNode, const TRegion& aNewTransparentRegion, const TRegion* aNewOpaqueRegion);
	void MovedToWindowGroup(const MWsWindowTreeNode& aWindowTreeNode, const MWsWindowTreeNode& aNewWindowGroupNode);
	void WindowGroupChained(const MWsWindowTreeNode& aParent, const MWsWindowTreeNode& aChild);
	void WindowGroupChainBrokenAfter(const MWsWindowTreeNode& aWindowGroupNode);
	void FadeAllChildren(const MWsWindowTreeNode& aWindowTreeNode, TBool aFaded);
	
public: // from MAlfSynchronizationInterface
    	
	TInt Synchronize(TInt& aId);
	
public: // from MWsDrawAnnotationObserver
	void WindowRedrawStart(const MWsWindowTreeNode& aWindowTreeNode, const TRegion& aRegion);
	void WindowRedrawEnd(const MWsWindowTreeNode& aWindowTreeNode);
	void WindowAnimRedrawStart(const MWsWindowTreeNode& aWindowTreeNode, const TRegion& aRegion);
	void WindowAnimRedrawEnd(const MWsWindowTreeNode& aWindowTreeNode);
	void SpriteRedrawStart(const MWsWindowTreeNode& aWindowTreeNode, const TRegion& aRegion);
	void SpriteRedrawEnd(const MWsWindowTreeNode& aWindowTreeNode);
	void SpriteFlash(const MWsWindowTreeNode& aWindowTreeNode, TBool aFlashOn);
	void SegmentRedrawStart( const TRegion& /*aRegion */);
	void SegmentRedrawEnd( );
	
public: // from MAlfHintObserver
    
    void InsertTagL( TInt aTag, TInt32 aParameter = 0 );
    void InsertTagL( TInt aTag, TRect aBoundingRect, TInt aLayerId );
    
protected: // New methods

    /**
     * C++ constructor
     */
    CAlfRenderStage(CWsRenderStage* aNext);

    /**
     * ConstructL
     *
     * @param aEnv
     * @param aScreen
     * @param aScreenNumber
     */
    virtual void ConstructL(MWsGraphicDrawerEnvironment* aEnv, MWsScreen* aScreen, TInt aScreenNumber);

    /**
     * InitL
     */
    virtual void InitL();

    /**
     * HasWindowRectChanged
     *
     * Temporary function until WServ can provide the same information.
     * 
     * @param aId
     * @param aRect
     * @return 
     */
    TBool HasWindowRectChanged( TUint32 aId, TRect& aRect );

    /**
     * DoHandleRedraw
     *
     * Handles window drawing related events.
     *
     * @param aEvent    Event to be handled
     */
    void DoHandleRedrawL( const TWservCrEvent& aEvent );

    /**
     * Draws standard text cursor. Client is expected to call commit.
     * @param aWindowTreeNode window tree node.
     * @param aExtent extent.
     * @param aClipRect clip rect.
	 * @param aCursorRect cursor rect.
     */
    void DoDrawTextCursor( 
        const MWsWindowTreeNode& aWindowTreeNode, 
        const TRect& aExtent, 
        const TRect& aClipRect,
        const TRect& aCursorRect );

protected:
	
    //TTimeStamp iTimeStamp;
		
    const TRegion* iUpdateRegion;
    
    /**
     * Own.
     */
    MWsScreen* iScreen;
        
    /**
     * Own.
     */
	CAlfGraphicsContext* iWsGraphicsContext; // MWsGraphicsContext

    /**
     * Own.
     */
	CAlfRsSendBuffer* iAlfSendBuffer;
    
    /**
     * Not own.
     */
	MWsGraphicDrawerEnvironment* iEnv;
    
    /**
     * Returns the stream size
     */
	TInt iWindowBufferIndex;

	/**
	 * @todo
	 * @note This is temporary structure for figuring out, if window size or position has changed since previous update.
	 *       WServ will provide event about these in the future.
	 */
	RHashMap<TUint32, TRect> iWindowData;
	
	/**
	 * Fader interface
	 * Own.
	 */
	CAlfFader*  iFader;
	
    TInt iScreenNumber;
    
    TBool iDrawingActive;
    
    RArray<TInt> iWindowIds;

    /**
     * @note Remove this when there is screen device for hw and correct sizes can be asked from there
     */
    TBool iNormalOrientation;

    /** For keeping track of segmentredraws inside windowredraws **/
    const MWsWindowTreeNode* iWindowDrawingNode;
    TBool iSegmentRedrawActive;
    
	CWsRenderStage* iNext;
private:
    // Goom connection. Owned.
    CAlfAsynchGoomSession* iGoomSession;
    
    // Current sprite draw node.
    const MWsWindowTreeNode* iSpriteRedraw;
    
    // Enumeration for sprite flash
    enum TSpriteFlash
        {
        // Sprite flash hasn't been set
        ESpriteFlashUndefined,
        // Sprite flash set with ETrue parameter
        ESpriteFlashOn,
        // Sprite flash set with EFalse parameter
        ESpriteFlashOff
        };
    // Sprite flash setting for iSpriteRedraw
    TSpriteFlash iSpriteRedrawFlash;

    // Synchronization identifier.
    TInt iSyncId;
	};

#endif //__ALFRENDERSTAGE_H__
