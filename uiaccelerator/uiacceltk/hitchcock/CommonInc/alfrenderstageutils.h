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
* Description:   
*
*/

#ifndef _ALFRSUAPI_H_
#define _ALFRSUAPI_H_

#include <e32std.h>

class MWsWindowTreeNode;

const TInt KAlfCompositionAgnosticWindowTreeObserverInterfaceId(0x2002BCFB);
    
class MAlfCompositionAgnosticWindowTreeObserver
    {
public:
	enum TFlags
		{
		/** For a window to be drawn, it needs to be activated and visible and have visible parents.
	 	Making a window invisible, implies that its children should be invisible as well.
	 	The default value for a window is true. */
		EVisible = 1,
		/** Non-fading windows should not be drawn faded regardless of their fade-count.
		The default value is false. */
		ENonFading,
		/** Windows with this attribute enabled uses alpha channel to control its transparency.
		The default value is false. */
		EAlphaChannelTransparencyEnabled,
		/** True if a clipping rect should be applied when drawing a standard text cursor, false otherwise.
		The default value is false. */
		ECursorClipRectSet
		};
	/**
	 This enum encapsulates the set of non-boolean attribute changes that can be observed
	 using the MWsWindowTreeObserver interface. 
	 @see AttributeChanged */
	enum TAttributes
		{
		/** Cursor type.
		@see MWsStandardTextCursor::Type()*/
		ECursorType = 1,
		/** Cursor clip rect.
		@see MWsStandardTextCursor::ClipRect()*/
		ECursorClipRect,
		/** Cursor flags.
		@see MWsStandardTextCursor::Flags()*/
		ECursorFlags,
		/** Cursor color.
		@see MWsStandardTextCursor::Color()*/
		ECursorColor,
		/** Window shape.
		@see MWsWindow::WindowArea()*/
		EWindowShape,
		/** Window group name
		@see MWsWindowGroup::Name()*/
		EWindowGroupName
      };
      
    virtual void NodeCreated(const MWsWindowTreeNode& aWindowTreeNode, const MWsWindowTreeNode* aParent) = 0;
    virtual void NodeReleased(const MWsWindowTreeNode& aWindowTreeNode) = 0;
    virtual void NodeActivated(const MWsWindowTreeNode& aWindowTreeNode) = 0;
    virtual void NodeExtentChanged(const MWsWindowTreeNode& aWindowTreeNode, const TRect& aRect) = 0;
    virtual void SiblingOrderChanged(const MWsWindowTreeNode& aWindowTreeNode, TInt aNewPos) = 0;
    virtual void FlagChanged(const MWsWindowTreeNode& aWindowTreeNode, TInt aFlag, TBool aNewValue) = 0;
    virtual void AttributeChanged(const MWsWindowTreeNode& aWindowTreeNode, TInt aAttribute) = 0;
    virtual void FadeCountChanged(const MWsWindowTreeNode& aWindowTreeNode, TInt aFadeCount) = 0;
    virtual void TransparentRegionChanged(const MWsWindowTreeNode& aWindowTreeNode, const TRegion& aNewTransparentRegion, const TRegion* aNewOpaqueRegion) = 0;
    //virtual void ElementAdded(const MWsWindowTreeNode& aWindowTreeNode, const TRect& aElement) = 0;
    virtual void MovedToWindowGroup(const MWsWindowTreeNode& aWindowTreeNode, const MWsWindowTreeNode& aNewWindowGroupNode) = 0;
    virtual void WindowGroupChained(const MWsWindowTreeNode& aParent, const MWsWindowTreeNode& aChild) = 0;
    virtual void WindowGroupChainBrokenAfter(const MWsWindowTreeNode& aWindowGroupNode) = 0;
    virtual void FadeAllChildren(const MWsWindowTreeNode& aWindowTreeNode, TBool aFaded) = 0;    
    };

class RAlfBridgerClient;

class MAlfBridge
    {
    public:
    enum TEvent {
        EAlfBridgeCreated, // data: MAlfBridge
        EAlfNativeWindowCreated, // data: TAlfNativeWindowData
        ESetWindowTreeObserver, // data: MAlfCompositionAgnosticWindowTreeObserver*
        EEndCallbackInitiated, // Renderstage above us has started progressing End(), i.e. rendering UI surface, no data 
        EExternalize, // externalize complete state data: Todo
        EInternalize  // reset the state from data data: Todo
        };
    
    virtual void CommitL() = 0;
    virtual void WriteIntsL(TUint8 aCommand, TInt aCount, TInt* aArray) = 0;
    virtual RAlfBridgerClient* Client() = 0;
    virtual void SetOrientation(TInt aOrientation) = 0;
    virtual void SetCompositionModified() = 0;
    };

/**
 * Synchronization interface
 */
class MAlfSynchronizationInterface
    {
public:
    /**
     * Start synchronization. 
     * @param aId This will contain identifier for this synchronization.
     * @return error code, KErrNone if synchronization is started.
     */
    virtual TInt Synchronize(TInt& aId) = 0;
    };

/**
 * Use this UID as parameter when resolving MAlfSynchronizationInterface 
 * interface.
 */
const TUint32 KAlfSynchronizationInterfaceUid = 0x2002C357;

/**
 * Category for P&S variable for synchronization finished indication.
 */
const TUid KAlfPSUidSynchronizer = { 0x10003B20 };

/**
 * Key for P&S variable for synchronization finished indication.
 */
const TUint32 KAlfPSKeySynchronizer = 0x2002C357;

#endif // _ALFRSUAPI_H_
