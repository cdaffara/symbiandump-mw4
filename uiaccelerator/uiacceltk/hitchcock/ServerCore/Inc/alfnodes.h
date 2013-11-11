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



#ifndef C_ALFNODES_H
#define C_ALFNODES_H

#include <e32base.h>

// Define this to get window tree traces. By default, traces are printed only when a new node is created, in DoNodeCreatedL().
// FYI: you can get similiar window tree traces from WSERV by calling: RWsSession::LogCommand(ELoggingStatusDump).  
//#define ALF_DEBUG_PRINT_NODE_INFO


#include "alfhierarchymodel.h"
// forward declarations
class CAlfWindow;
class CAlfNode;

/**
 *  Base class for window tree hierarcy items
 *  Hierarcy will have a single CAlfRootNode. CAlfVisualNode is base for nodes that can receive drawing. 
 *
 *  @code
 *   ?good_class_usage_example(s)
 *  @endcode
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */

class CAlfNodeSprite;
class CAlfNodeAnim;
class CAlfNodeTextCursor;
class CAlfNodeVisual;

class CAlfNode : public CBase
    {
// ---------------------------------------------------------------------------
public:

    static CAlfNode* NewL( 
            CAlfHierarchyModel* aModel, 
            RMemReadStream* aStream, 
            MWsWindowTreeNode::TType aType );

protected:

	void ConstructL( 
	        CAlfHierarchyModel* aModel, 
	        RMemReadStream* aStream );

protected:
 
    CAlfNode();
    
    void RemoveDependencies( CAlfNode* aFirstChildOfMyType );
	
    /*
     * UpdateChildrenOrdinalPositions
     * 
     * When ordinal position of a window is updated, it affects also the children
     * ordinal positions. These must be calculated and sent to alfbridge
     */
    void UpdateChildrenOrdinalPositions(CAlfNode* aNode);
    
    /*
     * UpdateOrdinalPosition
     * 
     * Creates a flat structure out of the linked tree structure and calculates
     * node ordinal position in it. Passes ordinal position to alfbridge.
     */
    void UpdateOrdinalPosition();
    
public:
    
    /*
     * HasChildren
     * 
     * Return ETrue, if node has sprite, anim, text cursor or child windows.
     */
    TBool HasChildren();
    
    void ResolveParent( TUint32 aParentId, TInt aMyId );
    
    void SetFirstChild( );
    
    void SetFirstAnim();
    
    void SetFirstSprite();
    
    CAlfNode* OrphanMe();
    
    CAlfNode* FindPreviousChild();
    
    TUint32 FindParentGroup();
  
    class TIter
        {
    public:
        
        
    TBool iChildrenProcessed;
  
        TIter(CAlfNode* aTopNode);
    
        CAlfNode* Current(); 
    
        CAlfNode* Next();
        
        CAlfNode* iTopNode;
        
        CAlfNode* iCurrentNode;
        
        CAlfNode* iCurrentSibling;
        
        CAlfNode* iCurrentParent;
        
        CAlfNode* iMasterNode;
        
        
    private:
        
        enum TState
        {
        ESearchingSprites,
        ESearchMoreSprites,
        ESearchingTextCursor,
        ESearchingMoreTextCursor,
        ESearchingAnims,
        ESearchMoreAnims,
        ESearchingChildWindows
        };
        
        TState iState;
        };
    
    void GetAllChildrenInGroup( RPointerArray<CAlfNode>& aNodes, TUint32 aParentId );
    
    TInt OrdinalPosition();
    
    void TraverseNodeTree( CAlfNode* node,  RPointerArray<CAlfNode>& nodes, TBool aTraverseOnlySiblings, TBool aAddMe = ETrue);

#ifdef ALF_DEBUG_PRINT_NODE_INFO    
public:    
        
    static void PrintInfo( 
            CAlfNode* node, TInt aHighLightNode, TInt aDepth = 0);
    
    static void PrintNodeTree(
            CAlfNodeRoot* aNode,
            TInt aHighlightNode = 0 );
    
    static void PrintSubTree( CAlfNode* aNode, TInt aHighlightNode, TInt& aDepth  );
    
    TBool Activated()
        {
        return iNodeActivated;
        }
    
    TBool Visible()
        {
        return iVisible;
        }

    TBool Faded()
        {
        return iFadeState;
        }

    TBool NonFading()
        {
        return iNonFading;
        }
    
    TInt PrintOrdinalPositions();

#endif    // ALF_DEBUG_PRINT_NODE_INFO
    
    void MovedToWindowGroup( RMemReadStream* aStream );
    
   /*
	* ListOffspring
	* @param	aList, list where children pointers will be added
	* @param	aListChildren, When EFalse then only sprite, anim and text cursor child will be
	*			added. No children nodes will be inserted to the list.
	*/
    void ListOffspring( RArray<CAlfNode*>& aList, TBool aListChildren = ETrue);
    
    virtual void FlagChanged( MWsWindowTreeObserver::TFlags aFlag, TBool aNewValue );
    
    void FadeAllChildren( TBool aFaded ); 

    void FadeCountChanged( TInt aFadeCount ); 
    
    void SetFaded( TBool aFaded );
    virtual void ActivateNode(){ iNodeActivated = ETrue;};

    TAny* CreateWindowAttributes(TInt& aIndex, TInt aSize );
    
    void SiblingOrderChanged( TInt aNewPos );
        
    virtual ~CAlfNode();
    
    void SetTracking( TBool aValue );
       
    inline TBool Tracking( )
        {
        return iTrackNode;
        }
    
    #ifdef __WINS__
    void Debug_CheckSiblingOrder(const TDesC16& aContext);
    #endif
    
    /**
     * Oprhans aNode and all its siblings.
     */
    static void OrphanAll(CAlfNode* aNode);
    
public: // Data    
    
    CAlfNode* iParent;
    
    CAlfNode* iChild;
    
    CAlfNode* iSibling;
    
    CAlfNodeSprite* iSpriteChild;
    
    CAlfNodeAnim* iAnimChild;
    
    CAlfNodeTextCursor* iTextCursor;
    

    TUint32 iId;
    
    TUint32   iGroupId;
	
	TInt iScreenNumber;
	
	MWsWindowTreeNode::TType Type()
	    {
	    return iType;
	    }
	
    
    CAlfWindow* iWindow;
    
    TInt iFadeCount;
   
protected:

    /**
     * ReadInt32L
     * Convinience function for reading a TInt32 from stream. This function is inlined.
     *
     * @since S60 ?S60_version
     * @param aPoint
     */
    inline void ReadInt32L( TInt& aValue, RMemReadStream* aStream  )
        {
        TRAPD( err,
        aValue = aStream->ReadInt32L(););
        if ( err )
            {
            RDebug::Print(_L("Eof in err ReadInt32L %d value %d"), err, aValue);
            }
        }

    inline TInt8 ReadInt8L(  RMemReadStream* aStream )
        {

        TInt8 value;
        TRAPD( err, value = aStream->ReadInt8L() );
        if ( err )
            {
            RDebug::Print(_L("Eof in err ReadInt8L %d value %d"), err, value);
            }
        return value;
        }
    

    
protected:
 


    CAlfHierarchyModel* iModel;

    /*
     * Flags
     */
    TBool iVisible;
    
    TBool iNonFading;
    
    TBool iAlphaChannelTransparencyEnabled;

    TBool iNodeActivated;

    MWsWindowTreeNode::TType  iType;
    
    TBool iTrackNode;
    TBool iFadeState;
    TBool iPostedFadeState;
    };

// ---------------------------------------------------------------------------
class CAlfNodeVisual : public CAlfNode
    {
protected:
	
	  CAlfNodeVisual();

	  
public:
	
	void SetExtent( TRect &aRect );
	
    void DrawWindowFrameL( RMemReadStream& aStream );
    
    inline CAlfWindow* Window(){return iWindow;}
    
    void FlagChanged( MWsWindowTreeObserver::TFlags aFlag, TBool aNewValue );

    void UpdateChildrenFlags( MWsWindowTreeObserver::TFlags aFlag, TBool aNewValue );
    
    void ActivateNode();
    
    void ReadEndMarkerL( RMemReadStream& aStream );
    
    void CommitCommands( TInt aCurrentPos, TInt aFrameSize, TBool aPartial, TBool aLastPart, TInt aChunkInUse );

    void CreateWindowL(TInt aWindowGroupHandle, TInt aWindowHandle, TInt aParentWindowHandle);
    
    ~CAlfNodeVisual();
    
    TSize iSize;
        
    TPoint iOrigin;

protected:
	
	void ConstructL( CAlfHierarchyModel* aModel, RMemReadStream* aStream );
    
	void UpdateParentPosition(TPoint aPosDelta);
	    
	void UpdateChildrenExtents(TPoint aPosDelta);
	    
	TBool iReadingPartialBuffer;
    };


class CAlfNodeWindow : public CAlfNodeVisual
    {
public:
	
    CAlfNodeWindow();
    
    ~CAlfNodeWindow();
        
    static CAlfNodeWindow* NewL( 
            CAlfHierarchyModel* aModel, 
            RMemReadStream* aStream, 
            TInt aScreenNumber );
    
    void MoveToWindowGroup( TUint32 aNewGroupId );
    void SetWindowAreaL( RMemReadStream* aStream );
    
protected:
	
    void ConstructL( CAlfHierarchyModel* aModel, RMemReadStream* aStream  );
    
protected:    
    
    struct TNodeWindowConstructionStruct
        {
        TInt    iOrdinalPriority;
        TInt32  iParentId;
        TInt    iWindowGroupHandle;
        TInt    iWindowHandle;
        };
    
    TNodeWindowConstructionStruct iNodeWindowConstructionStruct;
    
    TInt    iOrdinalPriority;

    };


// ---------------------------------------------------------------------------
class CAlfNodeRoot : public CAlfNodeVisual
    {
private:

	CAlfNodeRoot();

public:
  
	static CAlfNodeRoot* NewL( 
	        CAlfHierarchyModel* aModel, 
	        RMemReadStream* aStream , 
	        TInt aScreenNumber );

    ~CAlfNodeRoot( );

#ifdef ALF_DEBUG_PRINT_NODE_INFO 
    TBool iLogging;
#endif // ALF_DEBUG_PRINT_NODE_INFO

protected:
	
	void ConstructL( CAlfHierarchyModel* aModel, RMemReadStream* aStream );
	    
    };

// ---------------------------------------------------------------------------
class CAlfNodeGroup : public CAlfNode
    {
private:
	
	CAlfNodeGroup();
	 	
public:
    
	static CAlfNodeGroup* NewL( 
	        CAlfHierarchyModel* aModel, 
	        RMemReadStream* aStream, 
	        TInt aScreenNumber );
	
	void WindowGroupChained( TUint32 aChainedGroup );
	
	void GroupChainBrokenAfter( );
	
    ~CAlfNodeGroup();

#ifdef    ALF_DEBUG_PRINT_NODE_INFO
    const TUint32 SecureId()
        {
        return iSecureId;
        }
#endif
    /*
	 * The node, that this group is chained to.
	 */
	CAlfNodeGroup* iChainedFrom;
	CAlfNodeGroup* iChainedTo;
	
protected:
	
	void ConstructL( 
	        CAlfHierarchyModel* aModel, 
	        RMemReadStream* aStream );
private:
    
    TUint32 iSecureId;
	
    };

// ---------------------------------------------------------------------------
class CAlfNodeAnim: public CAlfNodeVisual
    {
private:
	
	CAlfNodeAnim();
    
	~CAlfNodeAnim( );
	
public:
    

    static CAlfNodeAnim* NewL( 
            CAlfHierarchyModel* aModel, 
            RMemReadStream* aStream, 
            TInt aScreenNumber );

protected:

	void ConstructL( CAlfHierarchyModel* aModel, RMemReadStream* aStream );
	    
    TBool iActive;
    };


// ---------------------------------------------------------------------------
class CAlfNodeSprite: public CAlfNodeVisual
    {
private:
	
    CAlfNodeSprite();

    ~CAlfNodeSprite( );
public:

    static CAlfNodeSprite* NewL( 
            CAlfHierarchyModel* aModel, 
            RMemReadStream* aStream, 
            TInt aScreenNumber );

protected:
	
	void ConstructL( CAlfHierarchyModel* aModel, RMemReadStream* aStream );

    };


class CAlfNodeTextCursor: public CAlfNodeVisual
    {
private: 
    
    CAlfNodeTextCursor();
public:
    
    static CAlfNodeTextCursor* NewL( 
            CAlfHierarchyModel* aModel, 
            RMemReadStream* aStream, 
            TInt aScreenNumber );

    void AttributeChangedL(  RMemReadStream* aStream );
    
    virtual ~CAlfNodeTextCursor();
protected:
    
    void ConstructL( 
            CAlfHierarchyModel* aModel, 
            RMemReadStream* aStream );
    
private:
    
    TRect iRect;
    
    TRect iClipRect;
    
    TInt iFlags;
    
    TInt iColor;
    
    TInt iFlashInterval;
    
    TInt iCursorType;
    };
#endif // C_ALFNODES_H
