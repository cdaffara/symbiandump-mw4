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

#include <uiacceltk/HuiCanvasVisual.h>
#include <alfwindowstructs.h>

#include "huiwscanvascommands.h"
#include "alfnodes.h"
#include "alfhierarchymodel.h"
#include "alfstreamerserver.h"
#include "alfwindowmanager.h"
#include "alfwindow.h"
#include "alfwindowdata.h"

#ifdef ALF_DEBUG_PRINT_NODE_INFO
    #define _ALF_LOGGING
    #include "alflogger.h"
    #undef _ALF_LOGGING
#else
    #include "alflogger.h"
#endif
// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CAlfNode::CAlfNode( ) :  
    iVisible( ETrue )
    {
    }

// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CAlfNode::ConstructL( CAlfHierarchyModel* aModel, RMemReadStream* aStream  )
    {
    iModel = aModel;
    iId = aStream->ReadInt32L();
    }

void CAlfNode::SetTracking( TBool aValue )
    {
    iTrackNode = aValue;
    if ( iWindow )
        {
        iWindow->SetNodeTracking( aValue );
        }
    }
// ---------------------------------------------------------------------------
// OrphanMe
// ---------------------------------------------------------------------------
//
CAlfNode* CAlfNode::OrphanMe()

    {
    __ALFLOGSTRING1("CAlfNode::OrphanMe %d", iId);
    // Parent node has been deleted. Orphan the node: remove the parent/sibling relationship.
    CAlfNode* sibling = iSibling;
    iSibling = NULL;
    iParent = NULL;
    return sibling;
    }

// ---------------------------------------------------------------------------
// ResolveParentL
// ---------------------------------------------------------------------------
//
void CAlfNode::ResolveParent( TUint32 aParentId, TInt aMyId )
    {
    CAlfNode* tmp = iModel->FindNode( aParentId );
    if ( tmp )
        {
        // 1. set parent, if there is one (root node is the only exception )
        iParent = tmp;
        // Possible sprite, textcursor, anim and child windows have different lists on the node
        switch( iType )
            {
            default:
                {
                SetFirstChild();
                break;
                }
            case MWsWindowTreeNode::EWinTreeNodeAnim:
                {
                SetFirstAnim();
                break;
                }
            case MWsWindowTreeNode::EWinTreeNodeSprite:
                {
                SetFirstSprite();
                break;
                }
            case MWsWindowTreeNode::EWinTreeNodeStandardTextCursor:
                {
                ASSERT( !iParent->iTextCursor );
                __ALFLOGSTRING1("parent->iTextCursor %d", iParent->iTextCursor );
                iParent->iTextCursor = (CAlfNodeTextCursor*)this;
                __ALFLOGSTRING1("parent->iTextCursor %d", iParent->iTextCursor );
                break;
                }
            }
        } 
    iId = aMyId;
    iModel->InsertNode( iId, this );
    ASSERT( iParent != NULL || iType == MWsWindowTreeNode::EWinTreeNodeRoot);
    
    if ( iType == MWsWindowTreeNode::EWinTreeNodeGroup )
        {
        iGroupId = iId;
        }
    else
        {
        iGroupId = FindParentGroup();
        }
    }

// ---------------------------------------------------------------------------
// SetFirstChild
// ---------------------------------------------------------------------------
//
void CAlfNode::SetFirstChild()
    {
    if ( iParent->iChild )
        {
        // pass over the previous first child and make it my sibling
        iSibling = iParent->iChild;
        }
    // jealously make me the first child
    iParent->iChild = this;
    }
// ---------------------------------------------------------------------------
// SetFirstChild
// ---------------------------------------------------------------------------
//
void CAlfNode::SetFirstAnim()
    {
    if ( iParent->iAnimChild )
        {
        // pass over the previous first child and make it my sibling
        iSibling = iParent->iAnimChild;
        }
    // jealously make me the first child
    iParent->iAnimChild = (CAlfNodeAnim*)this;
    }

// ---------------------------------------------------------------------------
// SetFirstChild
// ---------------------------------------------------------------------------
//
void CAlfNode::SetFirstSprite()
    {
    if ( iParent->iSpriteChild )
        {
        // pass over the previous first child and make it my sibling
        iSibling = iParent->iSpriteChild;
        }
    // jealously make me the first child
    iParent->iSpriteChild = (CAlfNodeSprite*)this;
    }

// ---------------------------------------------------------------------------
// FindParentGroup
// ---------------------------------------------------------------------------
//
TUint32 CAlfNode::FindParentGroup()
    {
    // go back in parents, until a group node is found. return its id.
    if ( iType == MWsWindowTreeNode::EWinTreeNodeSprite )
        {
        if ( iParent->iType == MWsWindowTreeNode::EWinTreeNodeRoot )
            {
            return 0;
            }
        else
            {
            return iParent->iId; // TODO: THIS IS ROOT, MIGHT NOT BE ALWAYS
            }
        }
    
    
    if ( iType == MWsWindowTreeNode::EWinTreeNodeRoot )
        {
        return 0;
        }
    ASSERT( iParent );
    CAlfNode* parent = iParent;
    while( parent->iType != MWsWindowTreeNode::EWinTreeNodeGroup )
        {
        parent = parent->iParent;
        }
    return parent->iId;
    }

// ---------------------------------------------------------------------------
// GetAllChildrenInGroup
// ---------------------------------------------------------------------------
//
void CAlfNode::GetAllChildrenInGroup( RPointerArray<CAlfNode>& aNodes, TUint32 aParentId )
	{
	if ( iParent->iType != MWsWindowTreeNode::EWinTreeNodeRoot)
		{
		CAlfNode* parentGroupNode = iModel->FindNode( aParentId );
		aNodes.Append(parentGroupNode); 
		for (CAlfNode::TIter iter( parentGroupNode ); iter.Current() != NULL;  iter.Next()) 
			{ 
			if (iter.Current() != parentGroupNode )
			    {
			    aNodes.Append(iter.Current());
			    }
			}
		}
	else
		{
		CAlfNode* child = iParent->iChild;
		do
			{
			aNodes.Append( child );
			child = child->iSibling; 
			}
		while( child );
		}
	}

// ---------------------------------------------------------------------------
// OrdinalPosition
// ---------------------------------------------------------------------------
//
TInt CAlfNode::OrdinalPosition()
    {
    //_LIT( KText, "OrdinalPosition");
    __ALFLOGSTRING("CAlfNode::OrdinalPosition");
    // __ALFLOGSTRING1("CAlfNode::OrdinalPosition - Looking for me %d", iId );
    RPointerArray<CAlfNode> nodes;
    TBool hasOrdinalPosition = ETrue;
    
    switch(iParent->iType)
        {
        case MWsWindowTreeNode::EWinTreeNodeClient:
            {
            CAlfNode* parentNode = iModel->FindNode( iParent->iId );
            __ASSERT_DEBUG(parentNode, USER_INVARIANT());
            if (parentNode  && parentNode->HasChildren())
                {
                // parent node is not added to the list
                // the anims, cursor and sprites directly in this parent are added to the list
                // siblings are added to the list, but their anims, cursors and sprites are not added
                TraverseNodeTree( parentNode, nodes, EFalse /* list all for this node */, EFalse /* dont add me */);
                }      
            break;
            }
        case MWsWindowTreeNode::EWinTreeNodeGroup:
            {
            TUint32 parentId = FindParentGroup();
            CAlfNode* parentGroupNode = iModel->FindNode( parentId );
            //__ALFLOGSTRING1("CAlfNode::OrdinalPosition - My parent %d", parentId );        
            CAlfNode* current;
            if ( parentGroupNode )
                {
                current = parentGroupNode->iChild;
                TraverseNodeTree( current, nodes, ETrue /* list siblings only */, ETrue /* add me */ );
                }      

            break;
            }


        case MWsWindowTreeNode::EWinTreeNodeRoot:
            {
            // Window group ordinal position calculation (their parent is EWinTreeNodeRoot)

            // Root node can have window groups, sprites and perhaps anims as children.
            // However, sprites & anims do not have ordinal position concept, but window groups do.
            // These sprites & anims are placed to floating sprite control group in alfbridge side 
            // and so they are excluded from ordinal position calculation.
            CAlfNode* child = iParent->iChild;
            if ( this == iParent->iTextCursor )
                {
                hasOrdinalPosition = EFalse;
                }
                
            if ( hasOrdinalPosition && iParent->iSpriteChild )
                {
                CAlfNode* animnode = iParent->iSpriteChild ;
                while ( animnode )
                    {
                    if ( this == animnode )
                        {
                        hasOrdinalPosition = EFalse;
                        break;
                        }
                    animnode = animnode->iSibling;
                    }
                }

            if( hasOrdinalPosition && iParent->iAnimChild )
                {
                CAlfNode* animnode = iParent->iAnimChild ;
                while ( animnode )
                    {
                    if ( this == animnode )
                        {
                        hasOrdinalPosition = EFalse;
                        break;
                        }                    
                    animnode = animnode->iSibling;
                    }
                }

            if ( child )
                {
                do
                    {
                    nodes.Append( child );
                    child = child->iSibling;
                    }
                while( child );
                }
            break;
            }
        }
        
    TInt ordinalPosition = -1;
    if ( hasOrdinalPosition )
        {        
        TInt i = nodes.Count();
        //   __ALFLOGSTRING1("Node count: %d", i );
        // return my position in the node array
        while( --i >=0 && nodes[i] != this ) 
            {
            }
        if ( i < 0)
            {
            __ALFLOGSTRING1("Node %d not found!", iId);
            __ASSERT_DEBUG( 0, USER_INVARIANT() );
            //USER_INVARIANT();
            }
        else
            {
            __ALFLOGSTRING2("Found %d at ordinal position %d!", nodes[i]->iId, i );
            }
        ordinalPosition = i;
        }
    
    nodes.Close();    
    return ordinalPosition;
    }

// ---------------------------------------------------------------------------
// CAlfNode::TraverseNodeTree
// Traverse through node tree and fill node array 
// ---------------------------------------------------------------------------
//

// ---------------------------------------------------------------------------
// CAlfNode::TraverseNodeTree
// Traverse through node tree and fill node array 
// ---------------------------------------------------------------------------
//

void CAlfNode::TraverseNodeTree( CAlfNode* node,  RPointerArray<CAlfNode>& nodes, TBool aTraverseOnlySiblings, TBool aAddMe)
    {
    // Exit if we've already finished walking the tree.
    if ( node == NULL) 
        { 
        __ALFLOGSTRING(" returning NULL");                                  
        return;
        }
    if (!aTraverseOnlySiblings)
        {
        if ( node->iSpriteChild ) 
            {
        CAlfNode* spritenode = node->iSpriteChild ;
            while ( spritenode )
                {
                nodes.Append( spritenode );                
                spritenode = spritenode->iSibling;
                }
            }
    
        if( node->iTextCursor )
            {
            nodes.Append(node->iTextCursor );            
            }
    
        if( node->iAnimChild )
            {
            CAlfNode* animnode = node->iAnimChild ;
            while ( animnode )
                {
                nodes.Append( animnode );                
                animnode = animnode->iSibling;
                }
            }
        
        if ( node->iChild)
            {                
            TraverseNodeTree(node->iChild , nodes, EFalse, ETrue);            
            }
        } 

    if (aAddMe)
        {
        nodes.Append( node );
        }

    if ( node->iSibling )
        {                
        TraverseNodeTree(node->iSibling, nodes, ETrue, ETrue);               
        }
    }

// ---------------------------------------------------------------------------
// OrdinalPosition
// ---------------------------------------------------------------------------
//
#ifdef ALF_DEBUG_PRINT_NODE_INFO
TInt CAlfNode::PrintOrdinalPositions()
    {
    TInt i = 0;
    __ALFLOGSTRING("CAlfNode::PrintOrdinalPositions - ");
    
    RPointerArray<CAlfNode> nodes;
    if ( iParent->iType != MWsWindowTreeNode::EWinTreeNodeRoot)
        {
        TUint32 parentId = FindParentGroup();
        CAlfNode* parentGroupNode = iModel->FindNode( parentId );
        for (CAlfNode::TIter iter( parentGroupNode ); iter.Current() != NULL;  iter.Next()) 
            { 
            nodes.Append(iter.Current()); 
            }
        }
    else
        {
        CAlfNode* child = iParent->iChild;
        do
            {
            nodes.Append( child );
            }
        while( ( child = child->iSibling) != NULL );
        }
    
    i = nodes.Count();
    // return my position in the node arr
    TInt groupLevel = 0;
    while( --i >=0  ) 
    	{
    	CAlfNodeVisual* node = (CAlfNodeVisual*)nodes[i];
    	_LIT(KText, "PrintOrdinalPositions");
    	// PrintInfo( i, node, TPtrC(KText), 0);
    	}
    nodes.Close();    
    return i;
    }

void CAlfNode::PrintInfo( CAlfNode* aNode, TInt aHighLightNode, TInt aDepth)
    {
    HBufC16* buffer = HBufC16::NewL(256);
    TPtr iDebugText = buffer->Des();
    iDebugText.Format( _L(""));
    MWsWindowTreeNode::TType type = aNode->Type();

    for(TInt i = 0; i < aDepth; i++)
        {
        iDebugText.AppendFormat(_L(" "));
        }
		
    // Print the parent id in front of every children. For debugging the debugging code...
    //CAlfNode* parent = aNode->iParent;
    //if (parent)
    //	{
	//	iDebugText.AppendFormat(_L("[0x%x] "), parent->iId);
    //	}
    if ( aHighLightNode == aNode->iId )
        {
        iDebugText.AppendFormat( _L("\n-- MODIFIED NODE -- \n"));
        }

    switch ( type )
        {
        case MWsWindowTreeNode::EWinTreeNodeRoot:
            {
            iDebugText.AppendFormat( _L("Root 0x%x"), aNode->iId);
            break;
            }
        case MWsWindowTreeNode::EWinTreeNodeClient:
            {
            CAlfNodeVisual* nodeVisual = (CAlfNodeVisual*)aNode;
            iDebugText.AppendFormat(_L("Client 0x%x: iTl: (%3d,%3d), size (%3dx%3d)"), nodeVisual->iId, nodeVisual->iOrigin.iX, nodeVisual->iOrigin.iY, nodeVisual->iSize.iWidth, nodeVisual->iSize.iHeight );
                         
            break;
            }
        case MWsWindowTreeNode::EWinTreeNodeGroup:
            {
            CAlfNodeGroup* nodeGroup = (CAlfNodeGroup*)aNode;
            iDebugText.AppendFormat(_L("Group (%d) 0x%x: SecureId: 0x%x"), nodeGroup->iGroupId, nodeGroup->iId, nodeGroup->SecureId());
            break;
            }               
        case MWsWindowTreeNode::EWinTreeNodeAnim:
            {
            CAlfNodeVisual* nodeVisual = (CAlfNodeVisual*)aNode;
            iDebugText.AppendFormat(_L("Anim 0x%x: iTl: (%3d,%3d), size (%3dx%3d)"), nodeVisual->iId, nodeVisual->iOrigin.iX, nodeVisual->iOrigin.iY,nodeVisual->iSize.iWidth, nodeVisual->iSize.iHeight );
            break;
            }               
        case MWsWindowTreeNode::EWinTreeNodeSprite:
            {
            CAlfNodeVisual* nodeVisual = (CAlfNodeVisual*)aNode;
            iDebugText.AppendFormat(_L("Sprite 0x%x: iTl: (%3d,%3d), size (%3dx%3d)"), nodeVisual->iId, nodeVisual->iOrigin.iX, nodeVisual->iOrigin.iY,nodeVisual->iSize.iWidth, nodeVisual->iSize.iHeight );
            break;
            }               
        case MWsWindowTreeNode::EWinTreeNodeStandardTextCursor:
            {
            // TODO, add more cursor info
            CAlfNodeVisual* nodeVisual = (CAlfNodeVisual*)aNode;
            iDebugText.AppendFormat(_L("Cursor 0x%x: iTl: (%3d,%3d), size (%3dx%3d)"), nodeVisual->iId, nodeVisual->iOrigin.iX, nodeVisual->iOrigin.iY,nodeVisual->iSize.iWidth, nodeVisual->iSize.iHeight );
            break;
            }
        }
    switch( type )
    	{
    	case MWsWindowTreeNode::EWinTreeNodeStandardTextCursor:
    	case MWsWindowTreeNode::EWinTreeNodeSprite:
    	case MWsWindowTreeNode::EWinTreeNodeAnim:
    	case MWsWindowTreeNode::EWinTreeNodeClient:
    		{
    		CAlfNodeVisual* nodeVisual = (CAlfNodeVisual*)aNode;
    		iDebugText.AppendFormat(_L("\t\t\t"));
			if (nodeVisual->NonFading())
				{
				iDebugText.AppendFormat(_L("(NON FADING)"));
				}

			if (nodeVisual->Visible())
				{
				iDebugText.AppendFormat(_L("(VISIBLE)"));
				}
			
			if (nodeVisual->Faded())
				{
				iDebugText.AppendFormat(_L("(FADED)"));
				}
			
			if (nodeVisual->Activated())
				 {
				 iDebugText.AppendFormat(_L("(ACTIVATED)"));
				 }
    		}
    	}
    RDebug::Print(_L("%S"), &iDebugText);
    delete buffer;
    }

void CAlfNode::PrintNodeTree(
        CAlfNodeRoot* aNode,
        TInt aHighlightNode)
    {
    if (!aNode->iLogging) // Change this value during run-time. This produces A LOT of output
        {
        return;
        }
    TInt depth(0);
    CAlfNode::PrintSubTree((CAlfNode*)aNode,aHighlightNode, depth);
    }

void CAlfNode::PrintSubTree( CAlfNode* aNode, TInt aHighlightNode, TInt& aDepth )
    {
    aDepth++;
    CAlfNode::PrintInfo(aNode, aHighlightNode, aDepth);
   
    CAlfNode* node = (CAlfNode*)aNode->iSpriteChild;
       while( node )
           {
           CAlfNode::PrintInfo( node, aHighlightNode, aDepth + 1);
           node = node->iSibling;
           }
       node = (CAlfNode*)aNode->iAnimChild;
       while( node )
           {
           CAlfNode::PrintInfo(node, aHighlightNode, aDepth + 1);
           node = node->iSibling;
           }
       if ( aNode->iTextCursor )
           {
           CAlfNode::PrintInfo( aNode->iTextCursor, aHighlightNode, aDepth + 1);
           }
       node = (CAlfNode*)aNode->iChild;
       while ( node )        
           {
           node->PrintSubTree( node, aHighlightNode, aDepth );
           node = node->iSibling;
           }
       aDepth--;
    }

 #endif
// ---------------------------------------------------------------------------
// TIter::TIter
// ---------------------------------------------------------------------------
//
CAlfNode::TIter::TIter(CAlfNode* aTopNode) 
        : iTopNode(aTopNode) 
        , iCurrentNode(aTopNode),
        iState( ESearchingSprites )
        { 
        // Find the first node 
        while(iCurrentNode->iChild!=NULL) 
                { 
                iCurrentNode=iCurrentNode->iChild; 
                } 
        
        iCurrentSibling=iCurrentNode->iSibling;        // De-reference iCurrent so it can be destroyed by caller 
        iCurrentParent=iCurrentNode->iParent; 
        } 

// ---------------------------------------------------------------------------
// TIter::Current
// ---------------------------------------------------------------------------
//
CAlfNode* CAlfNode::TIter::Current() 
        { 
        return iCurrentNode; 
        } 

// ---------------------------------------------------------------------------
// TIter::Next
// ---------------------------------------------------------------------------
//
CAlfNode* CAlfNode::TIter::Next() 
        { 
        // Exit if we've already finished walking the tree. Do not return the group node
        if (iCurrentNode == iTopNode || iCurrentNode == NULL) 
                { 
                __ALFLOGSTRING("Iter returning NULL");
                iCurrentNode = NULL;
                iState = ESearchingSprites;
                return NULL; 
                }
        
//Diagram shows visit order with parent (/) and sibling relationships (>) 
//  
//              -  14 - 
//            /           \ 
//          6       >       13 
//         / \           /  |   \ 
//        3 > 5        10 > 11 > 12 
//       / \   \     / | \ 
//      1 > 2   4  7 > 8 > 9 
//  
// 
        // Todo.
        // Return sprites in order
        
      
        if ( iState == ESearchingSprites )
            {
            iMasterNode = iCurrentNode;
                    
            if ( iCurrentNode->iSpriteChild )
                {
                // TODO: As long as we are putting sprites into their own group, we cannot count their ordinals
                //iCurrentNode = (CAlfNode*)iCurrentNode->iSpriteChild;
                //iState = ESearchMoreSprites;
                //return iCurrentNode;
                iState = ESearchingTextCursor;
                }
            else
                {
                iState = ESearchingTextCursor;
                }
            }
        
        if ( iState == ESearchMoreSprites )
            {
            if ( iCurrentNode->iSibling )
                {
                iCurrentNode = iCurrentNode->iSibling;
                return iCurrentNode;
                
                }
            else
                {
                iCurrentNode = iMasterNode;
                iState = ESearchingTextCursor;
                }
            }
        
        if ( iState == ESearchingTextCursor )
            {
            if ( iMasterNode->iTextCursor )
                {
                iCurrentNode = iMasterNode->iTextCursor;
                iState = ESearchingMoreTextCursor;
                return iCurrentNode;
                }
            else
                {
                iState = ESearchingAnims;
                }
            }
        
        if ( iState == ESearchingMoreTextCursor )
            {
            iCurrentNode = iMasterNode;
            iState = ESearchingAnims;        
            return iCurrentNode;
        
            }
        
        if ( iState == ESearchingAnims )
            {
            if ( iCurrentNode->iAnimChild )
                {
                iCurrentNode = (CAlfNode*)iCurrentNode->iAnimChild;
                iState = ESearchMoreAnims;
                return iCurrentNode;
                }
            else
                {
                iState = ESearchingChildWindows;
                iChildrenProcessed = ETrue;
                iCurrentSibling = iMasterNode->iSibling;
                }
            }
        
        if ( iState == ESearchMoreAnims )
            {
            if ( iCurrentNode->iSibling ) // anim sibling
                {
                iCurrentNode = iCurrentNode->iSibling;
                return iCurrentNode;
                
                }
            else
                {
                iCurrentNode = iMasterNode;

                iState = ESearchingChildWindows;
                // if this node is having children windows, then we need to go to them
                if ( iCurrentNode->iChild )
                    {
                    iCurrentSibling = iCurrentNode->iChild;
                    // Look the deepest child
                    iCurrentNode=iCurrentSibling; 
                    while(iCurrentNode->iChild!=NULL) 
                        { 
                        iCurrentNode=iCurrentNode->iChild; 
                        } 
                    
                    iState = ESearchingSprites;
                    return iCurrentNode;
                    }
                else
                    {
                    // if no children windows, then this node is done and it should be returned
                    return iCurrentNode;
                    }
                }
            }
            
            if ( iState == ESearchingChildWindows )
                {
                if (iCurrentSibling!=NULL) 
                    { 
                    iCurrentNode=iCurrentSibling; 
                    while(iCurrentNode->iChild!=NULL) 
                        { 
                        iCurrentNode=iCurrentNode->iChild; 
                        } 
                    } 
                else 
                    { 
                    iCurrentNode = iCurrentParent;
                    iChildrenProcessed = EFalse;
                    } 
    
                iCurrentSibling=iCurrentNode->iSibling;        // De-reference iCurrent so it can be destroyed by caller 
                iChildrenProcessed = EFalse;
                iState = ESearchingSprites;
                iCurrentParent=iCurrentNode->iParent; 
                if ( iCurrentNode == iTopNode )
                    {
                    iCurrentNode = NULL;
                    }
                return iCurrentNode; 
                }
        return NULL;
        } 

// ---------------------------------------------------------------------------
// FindPreviousChild
// ---------------------------------------------------------------------------
//
CAlfNode* CAlfNode::FindPreviousChild()
    {
    CAlfNode* previousSibling = NULL;
    switch ( iType )
        {
        default:
            {
            previousSibling = iParent->iChild;
            break;
            }
        case MWsWindowTreeNode::EWinTreeNodeAnim:
            {
            previousSibling = iParent->iAnimChild;
            break;
            }
        case MWsWindowTreeNode::EWinTreeNodeSprite:
            {
            previousSibling = iParent->iSpriteChild;
            break;
            }
        case MWsWindowTreeNode::EWinTreeNodeStandardTextCursor:
            {
            previousSibling = iParent->iTextCursor;
            break;
            }
        }
     
    if ( previousSibling == this ) 
        {
        return NULL; // return NULL, if I was the first child
        }
    // starting from the first child, loop until this one is found.
    
    while( previousSibling && previousSibling->iSibling != this)
        {
        previousSibling = previousSibling->iSibling;
        }
    
    return previousSibling; // return the previous child. return NULL, if this was the first child
    }

// ---------------------------------------------------------------------------
// FadeCountChanged
// Fade count changes are sent also to Anim, Sprite and text cursor nodes.
// ---------------------------------------------------------------------------
//
void CAlfNode::FadeCountChanged( TInt aFadeCount )
    {
	RArray<CAlfNode*> children;
	ListOffspring(children, EFalse); // EFalse = dont list chilren, only sprites, anims and textcursor
	TBool fade = aFadeCount ? ETrue : EFalse;
    
    for(TInt i=0;i<children.Count(); i++)
    	{
		children[i]->SetFaded(fade);        
    	}
    children.Close();
    iFadeCount = aFadeCount;
    }

// ---------------------------------------------------------------------------
// SetFaded
// ---------------------------------------------------------------------------
//
void CAlfNode::SetFaded( TBool aFaded )
    {
    iFadeState = aFaded && ( !iNonFading );
    if (Type()== MWsWindowTreeNode::EWinTreeNodeRoot)
        {
        // NO need to deliver fades for root
        return;
        }
    if (iFadeState != iPostedFadeState)
        {
        iModel->Server().Bridge()->AddData( EAlfDSSetFadeEffect, iId , iFadeState, (TAny*)iType);
        iPostedFadeState = iFadeState;
        }
    }

// ---------------------------------------------------------------------------
// FadeAllChildren
// ---------------------------------------------------------------------------
//
void CAlfNode::FadeAllChildren( TBool aFaded )
    {
    SetFaded(aFaded);
    
    CAlfNode* node = (CAlfNode*)iSpriteChild;
    while( node )
        {
        node->SetFaded( iFadeState );        
        node = node->iSibling;
        }
    node = (CAlfNode*)iAnimChild;
    while( node )
        {
        node->SetFaded( iFadeState );       
        node = node->iSibling;
        }
    if ( iTextCursor )
        {
        iTextCursor->SetFaded( iFadeState );
        }
    node = (CAlfNode*)iChild;
    while ( node )        
        {
        node->FadeAllChildren( iFadeState );
        node = node->iSibling;
        }
    }

// ---------------------------------------------------------------------------
// ListOffspring
// ---------------------------------------------------------------------------
//
void CAlfNode::ListOffspring( RArray<CAlfNode*>& aList, TBool aListChildren)
    {
    aList.Append(this);
    
    CAlfNode* node = (CAlfNode*)iSpriteChild;
    while( node )
        {
		aList.Append(node);        
        node = node->iSibling;
        }
    node = (CAlfNode*)iAnimChild;
    while( node )
        {
		aList.Append(node);       
        node = node->iSibling;
        }
    if ( iTextCursor )
        {
		aList.Append(iTextCursor);
        }
    if (aListChildren)
    	{
		node = (CAlfNode*)iChild;
		while ( node )        
			{
			node->ListOffspring( aList );
			node = node->iSibling;
			}
    	}
    }
// ---------------------------------------------------------------------------
// FlagChanged
// ---------------------------------------------------------------------------
//
void CAlfNode::FlagChanged( MWsWindowTreeObserver::TFlags aFlag, TBool aNewValue )
    {
    switch( aFlag )
        {
        case MWsWindowTreeObserver::EVisible:
            {
            // TODO: Do something!!!!
            iVisible = aNewValue;
            break;
            }
        case MWsWindowTreeObserver::ENonFading:
            {
            iNonFading = aNewValue;
            FadeAllChildren(iFadeState);
            break;
            }
        case MWsWindowTreeObserver::EAlphaChannelTransparencyEnabled:
            {
            iAlphaChannelTransparencyEnabled = aNewValue;
            break;
            }
        case MWsWindowTreeObserver::ECursorClipRectSet:
            {
            // TODO: take into account with cursor 
            // ECursorClipRectSet
            break;
            }
        }
    }

// ---------------------------------------------------------------------------
// CreateWindowAttributes
// ---------------------------------------------------------------------------
//
TAny* CAlfNode::CreateWindowAttributes(TInt& aIndex, TInt aSize )
    {
    TAny* attributes = NULL;
    TRAP_IGNORE(attributes = (TAny*)iModel->Server().Bridge()->AppendVarDataL( aSize, aIndex ))
    Mem::FillZ( (TUint8*)attributes, aSize ); // Initialize the returned memory area to 0
    return attributes;
    }

// ---------------------------------------------------------------------------
// SiblingOrderChanged
// ---------------------------------------------------------------------------
//
 void CAlfNode::SiblingOrderChanged( TInt aNewPos )
    {
    // __ALFLOGSTRING1("CAlfNode::SiblingOrderChanged - Id %d, Old ordinal position: %d"), iId, OrdinalPosition());
    ASSERT( iType != MWsWindowTreeNode::EWinTreeNodeRoot); 
    ASSERT( iType != MWsWindowTreeNode::EWinTreeNodeSprite);
    ASSERT( iType != MWsWindowTreeNode::EWinTreeNodeAnim);
    ASSERT( iType != MWsWindowTreeNode::EWinTreeNodeStandardTextCursor);
    ASSERT( iType == MWsWindowTreeNode::EWinTreeNodeGroup || iType == MWsWindowTreeNode::EWinTreeNodeClient );
    ASSERT(iParent != NULL); 
    ASSERT(iParent->iChild != NULL); 
    CAlfNode* previous = FindPreviousChild();
    ASSERT( previous || this == iParent->iChild );
        
    
    // Need to find the previous node to this node (oldPrevNode) 
    // and the node previous to aNewPos (newPrevNode) 
    // Then link oldPrevNode->iSibling to this->iSibling (unlink this node from the chain) 
    // And  link this->iSibling to newPrevNode->iSibling (link the right hand side of this node back in) 
    // And  link newPrevNode->iSibling to this                          (link the left hand side of this node back in) 

    // Will point to the prev node for the current/old position 
    CAlfNode* oldPrevNode = NULL; 
    // Will point to the prev node for the new position 
    CAlfNode* newPrevNode = NULL; 

    // parent 
    //   | 
    //   |   0   1   2   3   4 
    //   +-->A ->B ->C ->D ->E ->null 
    //      ^^^ 
    // position 0 is a special case... 
    // if the new position is 0 then newPrevNode is the parent 
    // if this is the leftmost sibling then oldPrevNode is the parent 

    TInt iterNodePosition=0; 
    CAlfNode* iterPrevNode = iParent; 
    CAlfNode* iterNode = iParent->iChild;         

    while (oldPrevNode == NULL || newPrevNode == NULL) 
        { 
        // Have we found the current/old position? 
            if (iterNode == this) 
                { 
                oldPrevNode = iterPrevNode; 

                if (newPrevNode == NULL && iterNodePosition != aNewPos) 
                    { 
                    // Found the old position first 
                    // and not at the new position yet 
                    // so "this" will be moving to the right 
                    // which will cause the following nodes to move left by one 
                    // so we need to compensate by adding one to the target position 
                    // 
                    //     0   1   2   3   4 
                    // p ->A ->B ->C ->D ->E ->n  if we are moving B to position 3 then need to find D->E link (iterPrevNode->iterNode) 
                    // p ->A ->C ->D ->B ->E ->n  but because C and D will be moving left, 
                    //                            D->E is actually position at 4. 
                    ++aNewPos; 
                    }                         
                } 

            // Have we found the new position? 
            if (iterNodePosition == aNewPos) 
                { 
                newPrevNode = iterPrevNode; 
                } 

            // Move onto the next node - if there is one 
            if (iterNode != NULL) 
                { 
                // move to next node 
                iterPrevNode = iterNode; 
                iterNode = iterNode->iSibling;                 
                ++iterNodePosition; 
                } 
            else 
                { 
                // parent 
                //   | 
                //   |   0   1   2   3   4 
                //   +-->A ->B ->C ->D ->E ->null 
                //                           ^^^ 
                // if iterNode gets to NULL, then we must have found both matches 
                // - i.e. either the new or current/old position is the rightmost node   
                if (oldPrevNode == NULL || newPrevNode == NULL )
                    {
                    // Fatal error! Node tree is corrupted.
                    __ALFLOGSTRING4("CAlfNode::SiblingOrderChanged(). Fatal error! Node tree is corrupted. oldPrevNode=0x%x, newPrevNode=0x%x, iterNodePosition=%d, aNewPos=%d", 
                            oldPrevNode, newPrevNode, iterNodePosition, aNewPos);                    
                    __ALFLOGSTRING4("CAlfNode::SiblingOrderChanged(). This node=0x%x, type = %d, parent node=0x%x, parent type=%d", this, Type(), iParent, iParent->Type());
                    __ALFLOGSTRING2("CAlfNode::SiblingOrderChanged(). This node id=0x%x, parent node id=0x%x", iId, iParent->iId);
                    __ASSERT_ALWAYS(EFalse, USER_INVARIANT());    
                    //__ASSERT_DEBUG(EFalse, USER_INVARIANT()); 
                    return;  
                    }
                } 
            ASSERT(iterPrevNode!=NULL);                 
        } // end while loop 

    if (newPrevNode == oldPrevNode) 
        { 
        // we are being asked to move to our current position 
        // nothing to do 

        return; 
        } 

    // In the following example, oldPrevNode is A, this is B and newPrevNode is D - as discussed above 
    // 
    // Unlink this node from the chain (A->B becomes A->C, nothing points to B) 
    //      0    1    2    3    4 
    // p--->A-¬  B-+->C--->D--->E--->n 
    //         \--/ 
    if (oldPrevNode == iParent) 
        { 
        iParent->iChild = iSibling; 
        } 
    else 
        { 
        oldPrevNode->iSibling = iSibling; 
        } 

    // Link the right hand side of this node back in (B->C becomes B->E, B half at position 3) 
    //      0         1    2 3  4 
    // p--->A-------->C--->D-+->E--->n 
    //                B-----/ 
    if (newPrevNode == iParent) 
        { 
        iSibling = iParent->iChild; 
        } 
    else 
        { 
        iSibling = newPrevNode->iSibling; 
        } 

    // Link the left hand side of this node back in (D->E becomes D->B, B fully linked into position 3) 
    //      0    1    2    3    4 
    // p--->A--->C--->D¬     +->E--->n 
    //                  \-B-/ 
    if (newPrevNode == iParent) 
        { 
        iParent->iChild = this; 
        } 
    else 
        { 
        newPrevNode->iSibling = this; 
        } 
    
    UpdateOrdinalPosition();
    
    if (HasChildren())
        {
        UpdateChildrenOrdinalPositions(this);
        }
    
    /*
    TInt ordinal = OrdinalPosition();
    __ALFLOGSTRING1("New ordinal position: %d"), OrdinalPosition());
#ifdef _DEBUG    
    PrintOrdinalPositions();
#endif    
    TInt offset;
    TAlfWindowAttributes* windowAttributes = (TAlfWindowAttributes*)CreateWindowAttributes(offset, sizeof(TAlfWindowAttributes));
    windowAttributes->iOrdinalPosition = ordinal; 
    windowAttributes->iWindowNodeType = iType;
    windowAttributes->iScreenNumber = iScreenNumber;
    
    iModel->Server().Bridge()->AddData( EAlfDSReorder, 
            iGroupId, 
            iId, 
            (TAny*)offset );
            */
            
            
    }

// ---------------------------------------------------------------------------
// destructor
// ---------------------------------------------------------------------------
// 
CAlfNode::~CAlfNode()
    {
    __ALFLOGSTRING1("CAlfNode::~CAlfNode %d", iId);
	if ( iWindow && iModel ) // just in case: the window should be deleted already
        {
        iModel->Server().WindowMgr()->DestroyWindow( *iWindow );
        iWindow = NULL;
        }
    }

// ---------------------------------------------------------------------------
// HasChildren
// ---------------------------------------------------------------------------
// 
TBool CAlfNode::HasChildren()
    {
    return iSpriteChild || iAnimChild || iTextCursor || iChild;
    }

// ---------------------------------------------------------------------------
// RemoveDependencies
// ---------------------------------------------------------------------------
// 
void CAlfNode::RemoveDependencies( CAlfNode* aFirstChildOfMyType )
    {
    // if I have siblings, then make my previous sibling's next sibling, that used to be my next sibling
    /*if ( iParent )
        {
        __ALFLOGSTRING1( "%d %d %d %d"), iParent->iChild , iParent->iSpriteChild , iParent->iAnimChild, iParent->iTextCursor );
        }*/

    if ( iModel && iParent && aFirstChildOfMyType )
        {
        CAlfNode* previousSibling = FindPreviousChild();
        if ( previousSibling )
            {
            previousSibling->iSibling = iSibling;
            }
        else
            {
            // If we do not have previous sibling
            switch ( iType )
                {
                default:
                    {
                    iParent->iChild = iSibling; // Parent's child needs to change ONLY IF I was the first one.
                    break;
                    }
                case MWsWindowTreeNode::EWinTreeNodeAnim:
                    {
                    iParent->iAnimChild = (CAlfNodeAnim*)iSibling;
                    break;
                    }
                case MWsWindowTreeNode::EWinTreeNodeSprite:
                    {
                    iParent->iSpriteChild = (CAlfNodeSprite*)iSibling;
                    break;
                    }
                }
            
            }
        }
    else
        {
        __ALFLOGSTRING1("My parent says, that I'm not his child :..( or no parent %d ", iParent );
        }
    
    // Orphan all the children
    OrphanAll(iChild);
    iChild = NULL;
    
    OrphanAll(iAnimChild);
    iAnimChild = NULL;
    
    OrphanAll(iSpriteChild);
    iSpriteChild = NULL;
    
    OrphanAll(iTextCursor);
    iTextCursor = NULL;
    
    if ( iModel )
        {    
        iModel->RemoveNode( iId );
        }
    }

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CAlfNodeVisual::CAlfNodeVisual() 
    {
    iWindow =  NULL;
    };

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
void CAlfNodeVisual::ConstructL( CAlfHierarchyModel* aModel, RMemReadStream* aStream )
	{
	CAlfNode::ConstructL( aModel, aStream );
    aStream->ReadL( (TUint8*)&iOrigin, sizeof(TPoint));
    aStream->ReadL( (TUint8*)&iSize, sizeof(TSize));
	}

// ---------------------------------------------------------------------------
// ReadEndMarkerL
// ---------------------------------------------------------------------------
//
void CAlfNodeVisual::ReadEndMarkerL( RMemReadStream& aStream )
    {
    TUint8 marker = aStream.ReadInt8L();
    ASSERT( marker == EAlfCommandEndMarker ) ; // endmarker
    }
 
// ---------------------------------------------------------------------------
// UpdateOrdinalPosition
// ---------------------------------------------------------------------------
//
void CAlfNode::UpdateOrdinalPosition()
    {
    TInt ordinal = OrdinalPosition();
    if ( ordinal >= 0 )
        {
        if ( iWindow )
            {
            iWindow->Move( ordinal );
            }
        else
            {
            //!!!! There is no window???
            // position must be updated, because it not necessary the first drawn.
            TInt offset;
            TAlfWindowAttributes* windowAttributes = (TAlfWindowAttributes*)CreateWindowAttributes(offset, sizeof(TAlfWindowAttributes));
            windowAttributes->iOrdinalPosition = ordinal; 
            windowAttributes->iWindowNodeType = iType;
            windowAttributes->iScreenNumber = iScreenNumber;
            // for updating window group ordinals
            iModel->Server().Bridge()->AddData( EAlfDSReorder, 
                    iGroupId, 
                    iId, 
                    (TAny*)offset );
            }
        }
    AMT_MAP_NODE_SET_ORDINAL_POSITION();
    }

// ---------------------------------------------------------------------------
// UpdateChildrenOrdinalPositions
// ---------------------------------------------------------------------------
//
void CAlfNode::UpdateChildrenOrdinalPositions(CAlfNode* aNode)
    {
    CAlfNode* node = (CAlfNode*)aNode->iSpriteChild;
    while( node )
        {
        node->UpdateOrdinalPosition();
        node = node->iSibling;
        }
    node = (CAlfNode*)aNode->iAnimChild;
    while( node )
        {
        node->UpdateOrdinalPosition();
        node = node->iSibling;
        }
    if ( aNode->iTextCursor )
        {
        (aNode->iTextCursor)->UpdateOrdinalPosition();
        }
    node = (CAlfNode*)aNode->iChild;
    while ( node )        
        {
        node->UpdateChildrenOrdinalPositions(node);
        node->UpdateOrdinalPosition();
        node = node->iSibling;
        }

    }

// ---------------------------------------------------------------------------
// OrphanAll
// ---------------------------------------------------------------------------
//
void CAlfNode::OrphanAll(CAlfNode* aNode)
    {
    while (aNode)
        {
        aNode = aNode->OrphanMe();
        }
    }

// ---------------------------------------------------------------------------
// CommitCommandsL
// ---------------------------------------------------------------------------
//
void CAlfNodeVisual::CommitCommands( TInt aCurrentPos, TInt aFrameSize, TBool aPartial, TBool aLastPart, TInt aChunkInUse  )
    {
    // __ALFLOGSTRING1("CAlfNodeVisual::CommitCommandsL, Id/Group: %d/%d Type: %d, Pos: %d, Size: %d, last part %d"), iId, iGroupId, iType, aCurrentPos, aFrameSize, aLastPart );
    if ( !iWindow )
        {
        __ALFLOGSTRING("CAlfHierarchyModel::CommitCommandsL Discarding window commands");
        return;
        }

    if ( !aFrameSize )
        {
        __ALFLOGSTRING3("CAlfHierarchyModel::CommitCommandsL empty frame Pos: %d, Size: %d, last part %d ", aCurrentPos, aFrameSize, aLastPart );
        return;
        }
    
    TBool emptyBuffer = EFalse;
    
    TInt nodeFlags = 0;

    // By default nodes are always opaque, unless they are sprites (or anims?) or
    // transparency has been enabled in the window.    
    if (iType != MWsWindowTreeNode::EWinTreeNodeSprite && 
        iType != MWsWindowTreeNode::EWinTreeNodeAnim &&
       iType != MWsWindowTreeNode::EWinTreeNodeStandardTextCursor &&
        !iAlphaChannelTransparencyEnabled)
        {
        nodeFlags |= EAlfWinNodeFlagOpaque;         
        }
        
    if ( iType == MWsWindowTreeNode::EWinTreeNodeStandardTextCursor )
        {
        emptyBuffer = ETrue;
        }
       
    iWindow->PostPartialBuffer( iModel->ChunkBase(aChunkInUse) + aCurrentPos, aFrameSize, aPartial, aLastPart , emptyBuffer, nodeFlags );
    }

// ---------------------------------------------------------------------------
// DrawWindowFrameL
// ---------------------------------------------------------------------------
void CAlfNodeVisual::DrawWindowFrameL( RMemReadStream& aStream )
    {
    TInt32 chunkInUse = aStream.ReadInt32L();
    TInt nextFramePos = aStream.ReadInt32L();
       
    // jump to the next frame
    if ( nextFramePos == 12345678 )
        {
        __ALFLOGSTRING("CAlfHierarchyModel::PostBufferL, Address of the frame has not been initialized!");
        }
    // read frame flags. For offscreen content, we'll add the complete packets 
     // as partial, if they have not been implicitly flushed 
     
     TUint8 command = aStream.ReadInt8L();
     __ASSERT_DEBUG( command == EAlfFrameFlags, USER_INVARIANT() );
     TInt32 frameFlags = aStream.ReadInt32L();

    TUint8 padding = aStream.ReadInt8L();
    ReadEndMarkerL( aStream );
    while( padding--)
        {
        aStream.ReadInt8L();
        }
        
    TInt currentPos = aStream.Source()->TellL( MStreamBuf::ERead ).Offset();
    aStream.Source()->SeekL( MStreamBuf::ERead, TStreamPos(nextFramePos));
    TSgcCanvasCommands packetState = (TSgcCanvasCommands)aStream.ReadInt8L();
    TInt frameSize = nextFramePos - currentPos;
#ifdef _DEBUG
    ASSERT( currentPos % 8 == 0 );
#endif
    
    //__ALFLOGSTRING1("CAlfHierarchyModel::PostBufferL endMarker: %d partial: %d"), endMarker, iReadingPartialBuffer );
    switch( packetState )
        {
        case EAlfPacketReady:
            {
            if ( frameFlags & EAlfTransparentContent )
                {
                if ( frameFlags & EAlfTransparentContentFlush  )
                    {
                    CommitCommands( currentPos, frameSize, !iReadingPartialBuffer /*partial*/, ETrue /*lastpart*/, chunkInUse );
                    }
                else
                    {
                    CommitCommands( currentPos, frameSize, ETrue /*partial*/, EFalse/*not lastpart*/, chunkInUse );
                    }
                }
            else
                {
                CommitCommands( currentPos, frameSize, iReadingPartialBuffer, ETrue, chunkInUse );
                }
            iReadingPartialBuffer = EFalse;
                            
            break;
            }
        case EAlfPacketNotReady:
            {
            __ASSERT_DEBUG( nextFramePos % 8 == 0, USER_INVARIANT() );
            __ASSERT_DEBUG( frameSize % 8 == 0, USER_INVARIANT() );
            iReadingPartialBuffer = ETrue;
            CommitCommands( currentPos, frameSize, iReadingPartialBuffer, EFalse, chunkInUse );
            break;
            }
        default:
            {
            __ALFLOGSTRING1("CAlfHierarchyModel::PostBufferL endMarker: %d ", packetState);
            break;
            }
        }
    }


// ---------------------------------------------------------------------------
// FlagChanged
// ---------------------------------------------------------------------------
//
void CAlfNodeVisual::FlagChanged( MWsWindowTreeObserver::TFlags aFlag, TBool aNewValue )
    {
    switch( aFlag )
        {
        case MWsWindowTreeObserver::EVisible:
            {
            iVisible = aNewValue;
            if ( iWindow )
            	{
            	if ( iVisible && iNodeActivated )
            		{
            		iWindow->SetActive( ETrue );
            		}
            	else
            		{
                    iWindow->SetActive( EFalse );
            		}
            	
            	iWindow->IncludeToVisibilityCalculation( iVisible && iNodeActivated );
            	}
            break;
            }
        case MWsWindowTreeObserver::ENonFading:
            {
            iNonFading = aNewValue;
            SetFaded(iFadeState);
            break;
            }
        case MWsWindowTreeObserver::EAlphaChannelTransparencyEnabled:
            {
            iAlphaChannelTransparencyEnabled = aNewValue;
            if ( iWindow )
                {
                iWindow->SetTransparencyAlphaChannel( aNewValue );
                }
            break;
            }
        }
    if ( aFlag == MWsWindowTreeObserver::ENonFading && HasChildren() )
        {
        UpdateChildrenFlags( aFlag, iFadeState );
        }
    }

// ---------------------------------------------------------------------------
// UpdateChildrenFlags
// ---------------------------------------------------------------------------
//
void CAlfNodeVisual::UpdateChildrenFlags( MWsWindowTreeObserver::TFlags aFlag, TBool aNewValue )
    {
    CAlfNode* node = (CAlfNode*)iSpriteChild;
    while( node )
        {
        node->FlagChanged( aFlag, aNewValue );
        node = node->iSibling;
        }
    node = (CAlfNode*)iAnimChild;
    while( node )
        {
        node->FlagChanged( aFlag, aNewValue );
        node = node->iSibling;
        }
    if ( iTextCursor )
        {
        iTextCursor->FlagChanged( aFlag, aNewValue );
        }
    node = (CAlfNode*)iChild;
    while ( node )
        
        {
        node->FlagChanged( aFlag, aNewValue ); // causes recursion on children
        node = node->iSibling;
        }
    }

// ---------------------------------------------------------------------------
// SetExtent
// ---------------------------------------------------------------------------
//
void CAlfNodeVisual::SetExtent( TRect& aRect )
    {
	// aRect is in screen coordinates whereas iOrigin and iSize are relative.
    if ( iSize != aRect.Size() )
    	{
    	iSize = aRect.Size();
    	if ( iWindow) 
    		{
    		iWindow->SetSize(iSize,0);
    		}
    	}
  	// Recursively, TODO, check this with flash example
   	TPoint parentPosition; 
   	CAlfNodeVisual* parent = (CAlfNodeVisual*)iParent;
   	while(parent)
   	    {
        if(parent->Type() != MWsWindowTreeNode::EWinTreeNodeGroup)
            { // parent 
            parentPosition += parent->iOrigin;
            parent = (CAlfNodeVisual*)parent->iParent;
            }
        else
            {
            break;
            }
                
    	}
            
    TPoint newOrigin = aRect.iTl - parentPosition;
    if ( iOrigin != newOrigin )
    	{            
    	iOrigin = newOrigin;
    	if (iWindow)
    		{
    		iWindow->SetPos(iOrigin, 0 );
    		/*if (HasChildren())
    		    {
    		    UpdateChildrenExtents(posDelta);
    		    }*/
    		// __ALFLOGSTRING1("CAlfNodeVisual, SetPos %d,%d, iType %d"), iOrigin.iX, iOrigin.iY, iType );
    		}
    	}
    // __ALFLOGSTRING1("CAlfNodeVisual, SetExtent iSize: %d,%d ; iPosition %d,%d "), iSize.iWidth, iSize.iHeight, iOrigin.iX, iOrigin.iY ); 
    }

void CAlfNodeVisual::UpdateParentPosition(TPoint aPosDelta)
    {
    // iParentPosition = aRect.iTl;
    // iParentSize = aRect.Size();
    
    // Todo, size change?
    if (iWindow)
        {
        iOrigin += aPosDelta;
        iWindow->SetPos( iOrigin, 0 );
        UpdateChildrenExtents(aPosDelta);
        // __ALFLOGSTRING1("CAlfNodeVisual, SetPos %d,%d, iType %d"), iOrigin.iX, iOrigin.iY, iType );
        }

    }

void CAlfNodeVisual::UpdateChildrenExtents(TPoint aPosDelta)
    {
        CAlfNode* node = (CAlfNode*)iSpriteChild;
        while( node )
            {
            ((CAlfNodeVisual*)node)->UpdateParentPosition(aPosDelta);
            node = node->iSibling;
            }
        node = (CAlfNodeVisual*)iAnimChild;
        while( node )
            {
            ((CAlfNodeVisual*)node)->UpdateParentPosition(aPosDelta);
            node = node->iSibling;
            }
        if ( iTextCursor )
            {
            ((CAlfNodeVisual*)iTextCursor)->UpdateParentPosition(aPosDelta);
            }
        node = (CAlfNodeVisual*)iChild;
        while ( node )        
            {
            ((CAlfNodeVisual*)node)->UpdateChildrenExtents(aPosDelta);
            ((CAlfNodeVisual*)node)->UpdateParentPosition(aPosDelta);
                        
            node = node->iSibling;
            }
        }

// ---------------------------------------------------------------------------
// ActivateNodeL
// ---------------------------------------------------------------------------
//
void CAlfNodeVisual::ActivateNode()
     { 
     if ( !iNodeActivated)
         {
         iNodeActivated = ETrue;
        
         if ( iWindow )
            {
        	if ( iVisible )
        		{
        		iWindow->SetActive( ETrue );
        		}
        	
        	iWindow->IncludeToVisibilityCalculation( iVisible && iNodeActivated );
            }
         }
     else
         {
         __ALFLOGSTRING("CAlfNodeVisual::ActivateNodeL - Node ALREADY active! ");
         }
     }


// ---------------------------------------------------------------------------
// CreateWindowL
// ---------------------------------------------------------------------------
void CAlfNodeVisual::CreateWindowL(TInt aWindowGroupHandle, TInt aWindowHandle, TInt aParentWindowHandle)
    {
    TWindowIdentifier windowIdentifier( iGroupId, iId );
    TWindowIdentifier clientSideIdentifier( aWindowGroupHandle, aWindowHandle );
    TWindowIdentifier parentIdentifier( 0, aParentWindowHandle ); // Group id not used, set to zero.
    
    TAlfWServInfo info;     
    info.iSize = iSize;
    info.iPosition = iOrigin;    
    info.iRefId = windowIdentifier; // Node ids   
    info.iClientSideId = clientSideIdentifier; // Client handles
    info.iNodeType = iType;
    info.iScreenNumber = iScreenNumber;    
    info.iParentRefId = parentIdentifier;
#ifdef ALF_DEBUG_TRACK_DRAWING    
    info.iTrackWindow = iTrackNode;
#endif
    iWindow = iModel->Server().WindowMgr()->CreateNewWindowL( iId, info );
    iWindow->CommitGc();
    }

// ---------------------------------------------------------------------------
// destructor
// ---------------------------------------------------------------------------
//
CAlfNodeVisual::~CAlfNodeVisual()
    {
    if ( iWindow && iModel )
        {
        iModel->Server().WindowMgr()->DestroyWindow( *iWindow );
		iWindow = NULL; // make sure there is no double deletion
        }
    }

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CAlfNodeWindow::CAlfNodeWindow()
    {
    iType = MWsWindowTreeNode::EWinTreeNodeClient;
    };

CAlfNodeWindow::~CAlfNodeWindow()
    {
    RemoveDependencies( iParent ? iParent->iChild : NULL );
    }
// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CAlfNodeWindow::ConstructL( CAlfHierarchyModel* aModel, RMemReadStream* aStream  )
	{
	CAlfNodeVisual::ConstructL( aModel, aStream );
	aStream->ReadL( (TUint8*)&iNodeWindowConstructionStruct, sizeof( TNodeWindowConstructionStruct ) );
    iOrdinalPriority = iNodeWindowConstructionStruct.iOrdinalPriority;
    ResolveParent( iNodeWindowConstructionStruct.iParentId, iId );
    CreateWindowL( iNodeWindowConstructionStruct.iWindowGroupHandle, iNodeWindowConstructionStruct.iWindowHandle, iNodeWindowConstructionStruct.iParentId );
    UpdateOrdinalPosition();
    
    AMT_MAP_STREAMER_NODE_WINDOW_CONSTRUCT();
	}

// ---------------------------------------------------------------------------
// MoveToWindowGroup
// ---------------------------------------------------------------------------
//
void CAlfNodeWindow::MoveToWindowGroup( TUint32 aNewGroupId )
	{
	__ALFLOGSTRING3("CAlfNodeWindow::MoveToWindowGroup moving nodeID 0x%x from group 0x%x to 0x%x",iId, iParent->iId, aNewGroupId );
	CAlfNodeGroup* newGroup = (CAlfNodeGroup*)iModel->FindNode( aNewGroupId );
	if ( newGroup )
		{
		// collect my children in my old group (inclucing me)
		RPointerArray<CAlfNode> myChildNodes;
		GetAllChildrenInGroup( myChildNodes, iId );
			    
		// release old relationships
		CAlfNode* previous = FindPreviousChild();
		if ( !previous )
			{
			iParent->iChild = iSibling; // I was the first and possibly only child
			}
		else
			{
			previous->iSibling = iSibling; // there was the a previous child. update the link to my next sibling (which might be NULL)
			}

		// yippii, new parent, 		
		// add me as the first child of the new group
		iParent = newGroup;
		iSibling = NULL;
		SetFirstChild();

		TUint32 screenNumber = iScreenNumber;

		TInt i = myChildNodes.Count();
		while( --i >=0 ) // update groupid and send new location to appui 
		    {
		    iModel->Server().Bridge()->AddData( EAlfDSMoveWindowToNewGroup, 
		            myChildNodes[i]->iId, 
		            screenNumber, 
		            (TAny*)aNewGroupId );
			myChildNodes[i]->iGroupId = aNewGroupId;
			}
	  
		myChildNodes.Close();
		}
	else
		{
		__ALFLOGSTRING("CAlfNodeWindow::MoveToWindowGroupL, new group does not exists");
		USER_INVARIANT();
		}
	}

// ---------------------------------------------------------------------------
// SetWindowAreaL
// ---------------------------------------------------------------------------
//
void CAlfNodeWindow::SetWindowAreaL( RMemReadStream* aStream )
    {
    TPoint pos;
    RRegion region;
    CleanupClosePushL( region );
    
    pos.iX = aStream->ReadInt32L();
    pos.iY = aStream->ReadInt32L();
    
    TInt count = aStream->ReadInt32L();
    for ( TInt i = 0; i < count; ++i )
        {
        TRect r;
        aStream->ReadL((TUint8*)&r.iTl.iX, 4 * sizeof(TInt32 ));
        region.AddRect(r);
        }
    
    if ( region.CheckError() )
        {
        // Fallback to non-shape version
        region.Clear();
        }
    
    iWindow->SetWindowArea( pos, region );
    
    CleanupStack::PopAndDestroy();
    }

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CAlfNodeWindow* CAlfNodeWindow::NewL( CAlfHierarchyModel* aModel, RMemReadStream* aStream, TInt aScreenNumber  )
    {
    CAlfNodeWindow* self = new(ELeave)CAlfNodeWindow();
    CleanupStack::PushL( self );
    self->iScreenNumber = aScreenNumber;
    self->ConstructL( aModel, aStream ); 
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Construct
// ---------------------------------------------------------------------------
//
CAlfNodeRoot::CAlfNodeRoot( )
    {
    iType = MWsWindowTreeNode::EWinTreeNodeRoot;
    }

// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CAlfNodeRoot::ConstructL( CAlfHierarchyModel* aModel, RMemReadStream* aStream  )
	{
	CAlfNodeVisual::ConstructL( aModel, aStream );
	
	// TODO: 
	ResolveParent( 0, iId );
	CreateWindowL(0,0,0); // Currently renderstage does not deliver ws/wg handles
	}

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CAlfNodeRoot* CAlfNodeRoot::NewL( CAlfHierarchyModel* aModel, RMemReadStream* aStream, TInt aScreenNumber  )
    {
    CAlfNodeRoot* self = new(ELeave)CAlfNodeRoot();
    CleanupStack::PushL( self );
    self->iScreenNumber = aScreenNumber;
    self->ConstructL( aModel, aStream ); 
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// destructor
// ---------------------------------------------------------------------------
//
CAlfNodeRoot::~CAlfNodeRoot( )
    {
    // TODO: destroy all children nodes?
    RemoveDependencies( NULL );
    }

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CAlfNodeGroup::CAlfNodeGroup()
    {
    iType = MWsWindowTreeNode::EWinTreeNodeGroup;
    }

// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CAlfNodeGroup::ConstructL( CAlfHierarchyModel* aModel, RMemReadStream* aStream  )
	{
	CAlfNode::ConstructL( aModel, aStream );
    TInt32 clientHandle = aStream->ReadUint32L();
    iSecureId = aStream->ReadUint32L();
    TInt32 parentId = aStream->ReadUint32L();
    
    ResolveParent( parentId, iId );
    
    TInt offset;
    TAlfWindowAttributes* windowAttributes = (TAlfWindowAttributes*)CreateWindowAttributes(offset, sizeof(TAlfWindowAttributes));
    windowAttributes->iWindowNodeType = iType; 
    windowAttributes->iClientHandle = clientHandle;
    windowAttributes->iScreenNumber = iScreenNumber;
    windowAttributes->iSecureId = iSecureId;
    windowAttributes->iParentNodeId = parentId;
	
    iModel->Server().Bridge()->AddData( EAlfDSNewWindow, 
            iGroupId, 
            iId, 
            (TAny*)offset );
    
    AMT_MAP_STREAMER_NODE_GROUP_CONSTRUCT();
	}

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CAlfNodeGroup* CAlfNodeGroup::NewL( CAlfHierarchyModel* aModel, RMemReadStream* aStream , TInt aScreenNumber )
    {
    CAlfNodeGroup* self = new(ELeave)CAlfNodeGroup();
    CleanupStack::PushL( self );
    self->iScreenNumber = aScreenNumber;
    self->ConstructL( aModel, aStream ); 
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// WindowGroupChainedL
// ---------------------------------------------------------------------------
//
void CAlfNodeGroup::WindowGroupChained( TUint32 aChainedGroup )
	{
	iChainedTo = (CAlfNodeGroup*)iModel->FindNode( aChainedGroup );
	if ( iChainedTo )
		{
		iChainedTo->iChainedFrom = this;
		}
	else
		{
		USER_INVARIANT();
		}
	
    if (iModel)
        {
        iModel->Server().Bridge()->AddData( EAlfDSGroupChained, 
                iId, 
                aChainedGroup, 
                (TAny*)iScreenNumber
                );                
        }
	}

// ---------------------------------------------------------------------------
// GroupChainBrokenAfter
// ---------------------------------------------------------------------------
//
void CAlfNodeGroup::GroupChainBrokenAfter(  )
	{
	if ( iChainedTo )
		{
		iChainedTo->iChainedFrom = NULL;
		iChainedTo = NULL;
		}
	if (iModel)
		{
		iModel->Server().Bridge()->AddData( EAlfDSGroupChainBroken, 
				iId, 
				0, 
				(TAny*)iScreenNumber );                
		}

	}


// ---------------------------------------------------------------------------
// destructor
// ---------------------------------------------------------------------------
//
CAlfNodeGroup::~CAlfNodeGroup()
    {
    if ( iChainedTo )
    	{
    	GroupChainBrokenAfter();
    	}
    if ( iChainedFrom )
    	{
    	iChainedFrom->iChainedTo = NULL;
    	}
    
    if (iModel)
        {
        TInt offset;
        TAlfWindowAttributes* windowAttributes = (TAlfWindowAttributes*)CreateWindowAttributes(offset, sizeof(TAlfWindowAttributes));
        windowAttributes->iWindowNodeType = iType; 
        windowAttributes->iScreenNumber = iScreenNumber; 

        iModel->Server().Bridge()->AddData( EAlfDSDestroyWindow, 
                iGroupId, 
                iId, 
                (TAny*)offset );
        iWindow = 0; // just in case
        }
    RemoveDependencies( iParent->iChild );
    };

// ---------------------------------------------------------------------------
// constructor
// ---------------------------------------------------------------------------
//
CAlfNodeAnim::CAlfNodeAnim( )
    {
    iType = MWsWindowTreeNode::EWinTreeNodeAnim;
     }

CAlfNodeAnim::~CAlfNodeAnim( )
    {
    if ( iParent )
        {
        RemoveDependencies( iParent->iAnimChild );
        }
    }

// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CAlfNodeAnim::ConstructL( CAlfHierarchyModel* aModel, RMemReadStream* aStream  )
	{
	CAlfNodeVisual::ConstructL( aModel, aStream );
	TInt32 parentId = aStream->ReadUint32L();

	// TODO: 
	ResolveParent( parentId, iId );
	CreateWindowL(0,0,iParent->iId); // Currently renderstage does not deliver ws/wg handles
	UpdateOrdinalPosition();
	}

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CAlfNodeAnim* CAlfNodeAnim::NewL( CAlfHierarchyModel* aModel, RMemReadStream* aStream, TInt aScreenNumber  )
    {
    CAlfNodeAnim* self = new(ELeave)CAlfNodeAnim();
    CleanupStack::PushL( self );
    self->iScreenNumber = aScreenNumber;
    self->ConstructL( aModel, aStream ); 
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// constructor
// ---------------------------------------------------------------------------
//
CAlfNodeSprite::CAlfNodeSprite( )
    {
    iType = MWsWindowTreeNode::EWinTreeNodeSprite;
    }

CAlfNodeSprite::~CAlfNodeSprite( )
    {
    if ( iParent )
        {
        RemoveDependencies( iParent->iSpriteChild );
        }
    }
// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CAlfNodeSprite::ConstructL( CAlfHierarchyModel* aModel, RMemReadStream* aStream  )
	{
	CAlfNodeVisual::ConstructL( aModel, aStream );
	TInt32 parentId = aStream->ReadUint32L();

	// TODO: 
	ResolveParent( parentId, iId );
	CreateWindowL(0,0,iParent->iId); // Currently renderstage does not deliver ws/wg handles
	// Sprite may not be first in its group
	UpdateOrdinalPosition();
	}

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CAlfNodeSprite* CAlfNodeSprite::NewL( CAlfHierarchyModel* aModel, RMemReadStream* aStream, TInt aScreenNumber  )
    {
    CAlfNodeSprite* self = new(ELeave)CAlfNodeSprite();
    CleanupStack::PushL( self );
    self->iScreenNumber = aScreenNumber;
    self->ConstructL( aModel, aStream ); 
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// constructor
// ---------------------------------------------------------------------------
//
CAlfNodeTextCursor::CAlfNodeTextCursor( )
    {
    iType = MWsWindowTreeNode::EWinTreeNodeStandardTextCursor;
    }

// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CAlfNodeTextCursor::ConstructL( CAlfHierarchyModel* aModel, RMemReadStream* aStream  )
    {
    CAlfNode::ConstructL( aModel, aStream );
    iCursorType = aStream->ReadInt32L();
    aStream->ReadL( (TUint8*)&iRect, sizeof(TRect));
    aStream->ReadL( (TUint8*)&iClipRect, sizeof(TRect));
    iSize = iRect.Size();
    iOrigin = iRect.iTl;
    
    iFlags = aStream->ReadInt32L();
    iColor = aStream->ReadInt32L();
    iFlashInterval = aStream->ReadInt32L(); 
    TInt32 parentId = aStream->ReadUint32L();
    
    // TODO: 
    ResolveParent( parentId, iId );
    
    CreateWindowL(0,0,iParent->iId); // Currently renderstage does not deliver ws/wg handles
    TInt offset;
    // pass rest of the cursor data
    TAlfCursorDataBufferAttributes* attributes = (TAlfCursorDataBufferAttributes*)CreateWindowAttributes(offset, sizeof(TAlfCursorDataBufferAttributes));
    attributes->iColor = iColor;
    attributes->iFlags = iFlags;
    attributes->iFlashInterval = iFlashInterval;
    attributes->iScreenNumber = iScreenNumber;
    
    aModel->Server().Bridge()->AddData( EAlfDSSetCursorData, 
            iGroupId, 
            iId, 
            (TAny*)offset );
    
    UpdateOrdinalPosition();
    
    AMT_MAP_STREAMER_TEXT_CURSOR_CONSTRUCT();
    }

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CAlfNodeTextCursor* CAlfNodeTextCursor::NewL( CAlfHierarchyModel* aModel, RMemReadStream* aStream, TInt aScreenNumber  )
    {
    CAlfNodeTextCursor* self = new(ELeave)CAlfNodeTextCursor();
    CleanupStack::PushL( self );
    self->iScreenNumber = aScreenNumber;
    self->ConstructL( aModel, aStream ); 
    CleanupStack::Pop( self );
    return self;
    }

void CAlfNodeTextCursor::AttributeChangedL( RMemReadStream* aStream )
    {
    AMT_MAP_BRIDGE_ADD_TEXT_CURSOR();
    
    // TODO: PASS FORWARD!!!
    TInt attribute = aStream->ReadInt32L();
    switch ( attribute )
        {
        case MWsWindowTreeObserver::ECursorType:
            {
            iCursorType = aStream->ReadInt32L();
            break;
            }
        case MWsWindowTreeObserver::ECursorClipRect:
            {
            aStream->ReadL( (TUint8*)&iClipRect, sizeof(TRect));
            break;
            }

        case MWsWindowTreeObserver::ECursorFlags:
            {
            iFlags = aStream->ReadInt32L();
            break;
            }

        case MWsWindowTreeObserver::ECursorColor:
            {
            iColor = aStream->ReadInt32L();
            break;
            }
        }
    TInt offset;
    TAlfCursorDataBufferAttributes* attributes = (TAlfCursorDataBufferAttributes*)CreateWindowAttributes(offset, sizeof(TAlfCursorDataBufferAttributes));
       attributes->iColor = iColor;
       attributes->iFlags = iFlags;
       attributes->iFlashInterval = iFlashInterval;
       attributes->iScreenNumber = iScreenNumber;
       __ALFLOGSTRING("Forwarding cursor data");
       iModel->Server().Bridge()->AddData( EAlfDSSetCursorData, 
               iGroupId, 
               iId, 
               (TAny*)offset );
       
   AMT_MAP_STREAMER_TEXT_CURSOR_CHANGE();
    }
CAlfNodeTextCursor::~CAlfNodeTextCursor( )
    {
    // text cursor is the only text cursor in its parent. No need to check siblings.
    if(iParent)
        {
        iParent->iTextCursor = NULL;
        }
    // RemoveNode is called instead of RemoveDependeciesL, because this node has no depencies.
    if(iModel)
        {
        iModel->RemoveNode( iId );
        }
    }

#ifdef __WINS__
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CAlfNode::Debug_CheckSiblingOrder(const TDesC16& aContext)
   {
   if (!iParent || Type() != MWsWindowTreeNode::EWinTreeNodeClient)
       {
       return;
       }
   
   TBool foundMyself = EFalse;
   TInt iterNodePosition = 0; 
   
   // Search for myselk in iParent sibling list
   for(CAlfNode* iterNode = iParent->iChild; iterNode != NULL; iterNode = iterNode->iSibling, iterNodePosition++)
       {
       if (iterNode == this) 
           {
           foundMyself = ETrue;
           }  
       }
   
   if(!foundMyself)
       {
       // Did not find myself from the parent sibling list ==> error!
       __ALFLOGSTRING2("CAlfNode::Test_CheckSiblingOrder(). Fatal error! Node tree is corrupted.  at %S: iId=0x%x", &aContext, iId );                    
       __ALFLOGSTRING3("CAlfNode::Test_CheckSiblingOrder(). iterNodePosition=%d, iChild=0x%x, iSibling=0x%x", iterNodePosition, iChild, iSibling );                    
       __ALFLOGSTRING3("CAlfNode::Test_CheckSiblingOrder(). iSpriteChild=0x%x, iAnimChild=0x%x, iTextCursor=0x%x", iSpriteChild, iAnimChild, iTextCursor );                    
       __ALFLOGSTRING4("CAlfNode::Test_CheckSiblingOrder(). This node=0x%x, type = %d, parent node=0x%x, parent type=%d", this, Type(), iParent, iParent->Type());
       // Print sibling list
       iterNodePosition=0;     
       __ALFLOGSTRING("CAlfNode::Test_CheckSiblingOrder(). SIBLINGS:");
       for(CAlfNode* iterNode2 = iParent->iChild; iterNode2!=NULL; iterNode2 = iterNode2->iSibling, iterNodePosition++)
           {
           __ALFLOGSTRING4("CAlfNode::Test_CheckSiblingOrder(). *** iterNodePosition=%d, id=%d, this=0x%x, iChild=0x%x", iterNodePosition, iterNode2->iId, iterNode2, iterNode2->iChild );                    
           __ALFLOGSTRING3("CAlfNode::Test_CheckSiblingOrder(). iSpriteChild=0x%x, iAnimChild=0x%x, iTextCursor=0x%x", iterNode2->iSpriteChild, iterNode2->iAnimChild, iterNode2->iTextCursor );                    
           }                 
       
       __ASSERT_ALWAYS(EFalse, USER_INVARIANT());   
       }
   }
#endif

// end of file

