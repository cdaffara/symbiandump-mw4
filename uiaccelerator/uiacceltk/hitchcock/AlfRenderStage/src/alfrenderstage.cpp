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
* Description:   AlfRenderStage provides a way to redirect avkon drawing to a graphics accelerator
*
*/

#include "alfrenderstage.h"

#include <fbs.h>
#include <bitstd.h>
#include <bitdev.h>
#include <graphics/wsgraphicdrawer.h>
#include <graphics/wsgraphicscontext.h>

#include <alfdecoderserverclient.h>
#include <huiwscanvascommands.h>
#include <uiacceltk/HuiUtil.h>
#include <alf/alfhintplugin.h>
#include <alf/alfcompositionclient.h>

#include "alflogger.h"
#include "alfrsgc.h"
#include "alfrssendbuffer.h"
#include "alfasynchgoomsession.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CAlfRenderStage* CAlfRenderStage::NewL(MWsGraphicDrawerEnvironment* aEnv,MWsScreen* aScreen, TInt aScreenNumber, CWsRenderStage* aNext)
	{
	CAlfRenderStage * stage = new(ELeave) CAlfRenderStage(aNext);
	CleanupStack::PushL(stage);
	stage->ConstructL(aEnv, aScreen, aScreenNumber);
	CleanupStack::Pop(stage);
	return stage;
	}

// ---------------------------------------------------------------------------
// C++ constructor
// ---------------------------------------------------------------------------
//
CAlfRenderStage::CAlfRenderStage(CWsRenderStage* aNext) : iNormalOrientation(ETrue), iNext(aNext)
	{
	}

// ---------------------------------------------------------------------------
// C++ destructor
// ---------------------------------------------------------------------------
//
CAlfRenderStage::~CAlfRenderStage()
	{
	iWindowIds.Reset();
	iEnv->UnregisterWsEventHandler(this);
	iScreen = NULL;
	delete iAlfSendBuffer;
	delete iWsGraphicsContext;
	delete iGoomSession;
	
	AMT_FREE_TLS();

    // Used just as a temporary holding place, do not delete!
	iWindowDrawingNode = NULL;	
	}

// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CAlfRenderStage::ConstructL(MWsGraphicDrawerEnvironment* aEnv, MWsScreen* aScreen, TInt aScreenNumber)
	{
	iEnv = aEnv;
	iScreen = aScreen;
	iScreenNumber = aScreenNumber;
	
	__ALFLOGSTRING1("CAlfRenderStage: About to baseconstruct. iScreenNumber=%d", iScreenNumber);
	CWsRenderStage::BaseConstructL();
    CWsRenderStage::SetNext(iNext);
	
	__ALFLOGSTRING("CAlfRenderStage: About to initialize send buffer");
    iAlfSendBuffer = CAlfRsSendBuffer::NewL( *this, aScreenNumber );
	
	iEnv->RegisterWsEventHandler( this, TWservCrEvent::EScreenOrientationChanged  );
	iWsGraphicsContext = CAlfGraphicsContext::NewL( *iAlfSendBuffer );

    // Initialize segment redraw helper attributes to default values
    iWindowDrawingNode = NULL;
    iSegmentRedrawActive = EFalse;
    
    // Initialise system
    InitL();

    // set up the link for composition rs
    MAlfCompositionController* compcntrl = ( MAlfCompositionController*)ResolveObjectInterface(KAlfCompositionControllerIfUid);
    if (compcntrl)
      {
      compcntrl->AlfBridgeCallback(MAlfBridge::ESetWindowTreeObserver,(MAlfCompositionAgnosticWindowTreeObserver*)this);  
      }

    AMT_SET_TLS();
    
    __ALFLOGSTRING("CAlfRenderStage: ready to rock");
    }

// ---------------------------------------------------------------------------
// InitL
// ---------------------------------------------------------------------------
//
void CAlfRenderStage::InitL()
    {
    }


// ---------------------------------------------------------------------------
// ResolveObjectInterface
// ---------------------------------------------------------------------------
//
TAny* CAlfRenderStage::ResolveObjectInterface(TUint aTypeId)
	{
	switch(aTypeId)
		{
    	case MWsGraphicsContext::EWsObjectInterfaceId:
    		return static_cast<MWsGraphicsContext*>(iWsGraphicsContext);
    	case MWsTextCursor::EWsObjectInterfaceId:
    	    return static_cast<MWsTextCursor*>(this);
    	case KAlfCompositionAgnosticWindowTreeObserverInterfaceId:
    	    return static_cast<MAlfCompositionAgnosticWindowTreeObserver*>(this);
    	case MAlfHintObserver::EWsObjectInterfaceId:
    	    return static_cast<MAlfHintObserver*>(this);
			
    	case MWsDrawAnnotationObserver::EWsObjectInterfaceId:
    	    return static_cast<MWsDrawAnnotationObserver*>(this);
        case KAlfSynchronizationInterfaceUid:
            return static_cast<MAlfSynchronizationInterface*>(this);
    	default:
    	    return CWsRenderStage::ResolveObjectInterface(aTypeId);
		}
	}

// ---------------------------------------------------------------------------
// Begin
// ---------------------------------------------------------------------------
//
void CAlfRenderStage::Begin(const TRegion* aRegion )
	{
	iDrawingActive = ETrue;
	__ALFLOGSTRING1("CAlfRenderStage::Begin.  iScreenNumber=%d", iScreenNumber);
	TRAPD( err, iAlfSendBuffer->PrepareBufferL() );
	if ( err )
	    {
	    __ALFLOGSTRING1("CAlfRenderStage::Begin, Leave occured %d", err );
	    }
		
	iUpdateRegion = aRegion;
	iNext->Begin(aRegion);
	}

// ---------------------------------------------------------------------------
// Synchronize
// ---------------------------------------------------------------------------
//
TInt CAlfRenderStage::Synchronize(TInt& aId)
    {
    if ( iAlfSendBuffer )
        {
        ++iSyncId;
        aId = iSyncId;
        iAlfSendBuffer->Synchronize(iSyncId);
        return KErrNone;
        }
    return KErrGeneral;
    }

// ---------------------------------------------------------------------------
// End
// ---------------------------------------------------------------------------
//
void CAlfRenderStage::End( TRequestStatus* aFinished ) 
	{
	iDrawingActive = EFalse;
	TInt err(KErrNone);
	__ALFLOGSTRING2("CAlfRenderStage::End. iScreenNumber=%d, aFinished=%d", iScreenNumber,aFinished);
	if (iAlfSendBuffer->FrameContainsDataL() ) 
	    {
    // set up the link for composition rs
        MAlfCompositionController* compcntrl = ( MAlfCompositionController*)ResolveObjectInterface(KAlfCompositionControllerIfUid);
        if (compcntrl)
            {
            compcntrl->AlfBridgeCallback(MAlfBridge::EEndCallbackInitiated,0);  
            }
	    TRAP( err, iAlfSendBuffer->SendL( aFinished ));
	    }
	else
	    {
	    __ALFLOGSTRING("CAlfRenderStage::End - Empty Frame");
	    // frame was empty for some reason. Complete request.
	    EndCallBack( aFinished );
	    }
	if ( err )
	    {
	    __ALFLOGSTRING("CAlfRenderStage::End, Leave occured!!");
	    }
	}

// ---------------------------------------------------------------------------
// EndCallBack
// ---------------------------------------------------------------------------
//
void CAlfRenderStage::EndCallBack( TRequestStatus* aFinished )
    {   
    __ALFLOGSTRING("CAlfRenderStage::EndCallBack 1");
	iNext->End(aFinished);
    __ALFLOGSTRING("CAlfRenderStage::EndCallBack 2");
    }

// ---------------------------------------------------------------------------
// ScreenNumber
// ---------------------------------------------------------------------------
//
TInt CAlfRenderStage::ScreenNumber() const
	{
    __ALFLOGSTRING1("CAlfRenderStage::ScreenNumber. screen=%d", iScreenNumber);
	return iScreenNumber;
	}
// ---------------------------------------------------------------------------
// SizeInPixels
// ---------------------------------------------------------------------------
//
/*TSize CAlfRenderStage::SizeInPixels() const
	{
    __ALFLOGSTRING2( "CAlfRenderStage::SizeInPixels() iNormalOrientation=%d, iScreenNumber=%d ",iNormalOrientation, iScreenNumber  );
    
	TSize res = iDisplayPolicy->SizeInPixels();
	
    if( !iNormalOrientation )
        {
        res = TSize( res.iHeight, res.iWidth ); 
        }
    
    __ALFLOGSTRING2( "CAlfRenderStage::SizeInPixels() %d x %d ", res.iWidth, res.iHeight );
	return res;
	}
*/
void CAlfRenderStage::DrawTextCursor(const TTextCursorInfo& aTextCursorInfo)
    {
    TRegionFix<1> fullWindowRegion;
    	const TRegion *clippingRegion= &aTextCursorInfo.iRegion;
    	if (aTextCursorInfo.iRegion.CheckError())
    		{
    		fullWindowRegion.AddRect(aTextCursorInfo.iWindow->AbsRect());
    		clippingRegion = &fullWindowRegion;
    		}

    	if (clippingRegion->IsEmpty())
    		{
    		return;
    		}

    	iWsGraphicsContext->SetDrawMode(MWsGraphicsContext::EDrawModePEN);

    	switch (aTextCursorInfo.iTextCursorType)
    		{
    		case TTextCursor::ETypeRectangle:
    			{
    			iWsGraphicsContext->SetBrushStyle(MWsGraphicsContext::ESolidBrush);
    			iWsGraphicsContext->SetPenStyle(MWsGraphicsContext::ENullPen);
    			iWsGraphicsContext->SetBrushColor(KRgbBlack); /* we are ignoring aTextCursorInfo.iTextCursorColor */
    			}
    			break;
    		case TTextCursor::ETypeHollowRectangle:
    			{
    			iWsGraphicsContext->SetBrushStyle(MWsGraphicsContext::ENullBrush);
    			iWsGraphicsContext->SetPenStyle(MWsGraphicsContext::ESolidPen);
    			iWsGraphicsContext->SetPenColor(KRgbBlack); /* we are ignoring aTextCursorInfo.iTextCursorColor */
    			}
    			break;
    		default:
    		    {
    		    break;
    		    }
    		}
    	iWsGraphicsContext->SetClippingRegion(*clippingRegion);
    	//
    	// During Sprite drawing, the GC gets reset.  Possibly other code could
    	// have done this also.  So make sure we setup the origin so that window-relative
    	// co-ordinates work as expected; iCursorRect is in window co-ordinates.
    	//
    	iWsGraphicsContext->SetOrigin(aTextCursorInfo.iWindow->Origin());
    	iWsGraphicsContext->DrawRect(aTextCursorInfo.iCursorRect);    
    }

// ---------------------------------------------------------------------------
// DoHandleEvent
// ---------------------------------------------------------------------------
//
void CAlfRenderStage::DoHandleEvent(const TWservCrEvent& aEvent)
    {
    TUint eventType = aEvent.Type();
    switch( eventType )
        {
    	case TWservCrEvent::EDeviceOrientationChanged:
    		{
    		__ALFLOGSTRING("CAlfRenderStage::DoHandleEvent EDeviceOrientationChanged");
    		break;
    		}
        default:
            // no other events have been registered
            break;
        }
    }

// ---------------------------------------------------------------------------
// WsGraphicsContext
// ---------------------------------------------------------------------------
//
CAlfGraphicsContext* CAlfRenderStage::WsGraphicsContext()
    {
    return iWsGraphicsContext;
    }

// ---------------------------------------------------------------------------
// AlfSendBuffer
// ---------------------------------------------------------------------------
//
CAlfRsSendBuffer* CAlfRenderStage::AlfSendBuffer()
    {
    return iAlfSendBuffer;
    }


// ---------------------------------------------------------------------------
// DoDrawTextCursor
// ---------------------------------------------------------------------------
//
void CAlfRenderStage::DoDrawTextCursor( 
    const MWsWindowTreeNode& aWindowTreeNode, 
    const TRect& aExtent, 
    const TRect& aClipRect,
    const TRect& aCursorRect )
    {
    // It might be possible to reduce amount of drawing. 
    
    TRegionFix<1> drawRegion;
    drawRegion.AddRect( aExtent );
    TRegionFix<1> clipRegion;
    clipRegion.AddRect( aClipRect );

    TRgb penColor = TRgb(0x000000);
    TRgb brushColor = TRgb(0x000000);
    TRgb dotColor =  TRgb(0xFFFFFF);
    
    WindowRedrawStart( aWindowTreeNode, drawRegion );
    iWsGraphicsContext->Reset();
    iWsGraphicsContext->SetDrawMode( MWsGraphicsContext::EDrawModePEN );
    iWsGraphicsContext->SetBrushStyle( MWsGraphicsContext::ESolidBrush );
    iWsGraphicsContext->SetPenStyle( MWsGraphicsContext::ESolidPen );
    iWsGraphicsContext->SetBrushColor( brushColor ); 
    iWsGraphicsContext->SetPenColor( penColor );
    iWsGraphicsContext->SetClippingRegion( clipRegion );    
    iWsGraphicsContext->DrawRect( aCursorRect );    

    // Draw pattern to cursor so that it is visible in any color backgrounds.
    iWsGraphicsContext->SetPenColor( dotColor );    
    TPoint start = aCursorRect.iTl;
    TPoint end = TPoint(aCursorRect.iTl.iX, aCursorRect.iBr.iY);
    
    for (TInt i=0; i<aCursorRect.Width();i++)
        {    
        TPoint point = start;
        for (TInt j=0; j<aCursorRect.Height();j++)
            {
            if ((i % 2))
                {
                if (j % 2)
                    iWsGraphicsContext->Plot(point);            
                }
            else
                {
                if (!(j % 2))
                    iWsGraphicsContext->Plot(point);                        
                }                
            point.iY++;
            }
        start.iX++;
        }
        
    WindowRedrawEnd( aWindowTreeNode );
    }

// ---------------------------------------------------------------------------
// NodeCreated
// ---------------------------------------------------------------------------
//
void CAlfRenderStage::NodeCreated(const MWsWindowTreeNode& aWindowTreeNode, MWsWindowTreeNode const* aParent)
    {
    __ALFLOGSTRING("CAlfRenderStage::NodeCreated");
    // You need to enable ALF_DEBUG_TRACK_DRAWING in alfappservercore.mmp
    // and change this value to 1, if you want to track this node in debugger.
   
    iWindowIds.AppendL( (TInt) &aWindowTreeNode );
    TInt nodeType = aWindowTreeNode.NodeType();
    switch (nodeType)
        {
        case MWsWindowTreeNode::EWinTreeNodeClient:
            {
            const MWsWindow* window = aWindowTreeNode.Window();
            TInt ordinalPriority = window->OrdinalPriority();
            TInt windowId = window->Handle();
            TInt windowGroupId = KErrNotFound;
            
            const MWsWindowTreeNode* parentWindowGroup = aParent;
            while (parentWindowGroup)
                {
                if (parentWindowGroup->NodeType() == MWsWindowTreeNode::EWinTreeNodeGroup)
                    {
                    windowGroupId = parentWindowGroup->WindowGroup()->Identifier();
                    break;
                    }
                parentWindowGroup = parentWindowGroup->ParentNode();
                }
            TSize size = window->Size();
            TPoint origin = window->Origin();
            //TInt parent = GetIdentifierL(aParent );
    	    iAlfSendBuffer->WriteIntsL( EAlfNodeCreated,
    	        10,
    	        nodeType,
    	        &aWindowTreeNode,
    	        origin.iX,
    	        origin.iY,
    	        size.iWidth,
    	        size.iHeight,
    	        ordinalPriority,
    	        (TInt)aParent,
    	        windowGroupId,
    	        windowId
    	        );
            break;
            }
        case MWsWindowTreeNode::EWinTreeNodeRoot:
            {
            TSize size = TSize( 360, 640);
            TPoint origin = TPoint( 0, 0 );
            //TInt parent = GetIdentifierL(aParent );
            iAlfSendBuffer->WriteIntsL( EAlfNodeCreated,
                6,
                nodeType,
                &aWindowTreeNode,
                origin.iX,
                origin.iY,
                size.iWidth,
                size.iHeight
                );
            break;
            }
        case MWsWindowTreeNode::EWinTreeNodeGroup:
            {
            const MWsWindowGroup* windowGroup = aWindowTreeNode.WindowGroup();
            ASSERT(windowGroup);
			TInt identifier = windowGroup->Identifier();
			TInt secureId = windowGroup->Client()->SecureId();

#ifndef __WINS__  
            if( iScreenNumber == 0 )
		        {          
                if ( secureId != 0x10207218 && // Capserver / AKA goom
                     secureId != 0x10204c27 && // Policy server
                     secureId != 0x2000f85a )  // IAD application updater
                    {
                    // Todo: Must actually check whether the configuration uses goom
                    // would create drastic performance hit in a system that does not need
                    // (and construct) goom
                    if ( !iGoomSession )
	                    {
	                    iGoomSession = new CAlfAsynchGoomSession;
	                    }
                    if ( iGoomSession )
	                    {
	                    iGoomSession->AppAboutToStart(TUid::Uid(secureId));
	                    }
			        }
                }
#endif
			
            iAlfSendBuffer->WriteIntsL( EAlfNodeCreated,
    	        5,
    	        nodeType,
    	        &aWindowTreeNode,
    	        identifier,
    	        secureId,
    	        (TInt)aParent
    	        );
            break;
            }
        case MWsWindowTreeNode::EWinTreeNodeSprite:
            {
            __ALFLOGSTRING1("CAlfRenderStage::NodeCreated node Id: %d", &aWindowTreeNode );
            TRect spriteRect = aWindowTreeNode.Sprite()->Rect();
            // TODO, MCL 09+
            // TSpriteType spriteType = aWindowTreeNode.Sprite()->SpriteType();
            iAlfSendBuffer->WriteIntsL( EAlfNodeCreated,
                7,
                nodeType,
                &aWindowTreeNode,
                spriteRect.iTl.iX,
                spriteRect.iTl.iY,
                spriteRect.iBr.iX,
                spriteRect.iBr.iY,
                // sprite type 
                (TInt)aParent
                );
            break;
            }
        case MWsWindowTreeNode::EWinTreeNodeAnim:
            {
            TSize size = TSize( 0, 0 );
            TPoint origin = TPoint( 0, 0 );
            iAlfSendBuffer->WriteIntsL( EAlfNodeCreated,
                    7,
                    nodeType,
                    &aWindowTreeNode,
                    origin.iX,
                    origin.iY,
                    size.iWidth,
                    size.iHeight,
                    (TInt)aParent
            );
            break;
            }
        case MWsWindowTreeNode::EWinTreeNodeStandardTextCursor:
            {
            // TODO
            // Use interface MWsStandardTextCursors for Type, Rect, cliptRect, flags, color and flashinterval
            const MWsStandardTextCursor* cursor = aWindowTreeNode.StandardTextCursor();
            
            iAlfSendBuffer->WriteIntsL( EAlfNodeCreated,
                    15,
                    nodeType,
                    &aWindowTreeNode,
                    cursor->Type(),
                    cursor->Rect().iTl.iX,
                    cursor->Rect().iTl.iY,
                    cursor->Rect().iBr.iX,
                    cursor->Rect().iBr.iY,
                    cursor->ClipRect().iTl.iX,
                    cursor->ClipRect().iTl.iY,
                    cursor->ClipRect().iBr.iX,
                    cursor->ClipRect().iBr.iY,
                    cursor->Flags(),
                    cursor->Color().Internal(),
                    cursor->FlashInterval().Int(),
                    (TInt)aParent );
            
            
            // Initial command buffer for the cursor
            DoDrawTextCursor( aWindowTreeNode, cursor->Rect(), cursor->ClipRect(), cursor->Rect() );
            break;
            }
        default:
            {
            __ALFLOGSTRING1("CAlfRenderStage:: EAlfNodeCreated, unknown node type: %i",  nodeType);
            break;
            }
        }
#ifdef ALF_DEBUG_TRACK_DRAWING
    TInt trackThisNode = 0;
    if ( trackThisNode )
        {
        iAlfSendBuffer->WriteIntsL( EAlfDebugTrackNode, 2, (TInt32)&aWindowTreeNode, 1 );
        }
#endif    
	iAlfSendBuffer->CommitL();
	
    AMT_MAP_RENDER_STAGE_NODE_CREATED();
    }

// ---------------------------------------------------------------------------
// NodeReleased
// ---------------------------------------------------------------------------
//
void CAlfRenderStage::NodeReleased(const MWsWindowTreeNode& aWindowTreeNode)
    {
    __ALFLOGSTRING("CAlfRenderStage::NodeReleased");
    TInt nodeType = aWindowTreeNode.NodeType();
 
#ifndef __WINS__     
	if ( iScreenNumber == 0 )
        {
        if ( iGoomSession && nodeType == MWsWindowTreeNode::EWinTreeNodeGroup )
            {
            const MWsWindowGroup* windowGroup = aWindowTreeNode.WindowGroup();
            TInt secureId = windowGroup->Client()->SecureId();
        
            iGoomSession->AppClosed( TUid::Uid( secureId ) );
            }
	    }
#endif        
    
    iAlfSendBuffer->WriteIntsL( EAlfNodeReleased,
        2,
        nodeType,
        &aWindowTreeNode
        );
	iAlfSendBuffer->CommitL();
	TInt index = iWindowIds.Find( (TInt)&aWindowTreeNode );
	if ( index != KErrNotFound )
	    {
        iWindowIds.Remove( index );
	    }
	else
	    {
	    __ALFLOGSTRING("CAlfRenderStage::NodeReleased - WARNING: Node not found!!");
	    }

	AMT_MAP_RENDER_STAGE_NODE_RELEASED();
    }

// ---------------------------------------------------------------------------
// NodeActivated
// ---------------------------------------------------------------------------
//
void CAlfRenderStage::NodeActivated(const MWsWindowTreeNode& aWindowTreeNode)
    {
    __ALFLOGSTRING("CAlfRenderStage::NodeActivated >>");
    TInt nodeType = aWindowTreeNode.NodeType();
    iAlfSendBuffer->WriteIntsL( EAlfNodeActivated,
        2,
        nodeType,
        &aWindowTreeNode );

	iAlfSendBuffer->CommitL();
    __ALFLOGSTRING("CAlfRenderStage::NodeActivated <<");
   
    AMT_MAP_RENDER_STAGE_NODE_ACTIVATED();
    }

// ---------------------------------------------------------------------------
// NodeExtentChanged
// ---------------------------------------------------------------------------
//
void CAlfRenderStage::NodeExtentChanged(const MWsWindowTreeNode& aWindowTreeNode, const TRect& aRect)
    {
     __ALFLOGSTRING("CAlfRenderStage::NodeExtentChanged");
    if ( aWindowTreeNode.NodeType() == MWsWindowTreeNode::EWinTreeNodeStandardTextCursor )
        {           
        const MWsStandardTextCursor* cursor = aWindowTreeNode.StandardTextCursor();
        TRect rect(cursor->Rect());
        iAlfSendBuffer->WriteIntsL( EAlfNodeExtentChanged,
            5,
            rect.iTl.iX,
            rect.iTl.iY,
            rect.iBr.iX,
            rect.iBr.iY,
            &aWindowTreeNode
            );    

        DoDrawTextCursor( aWindowTreeNode, cursor->Rect(), cursor->ClipRect(), cursor->Rect() );          
        }
    else
        {
        iAlfSendBuffer->WriteIntsL( EAlfNodeExtentChanged,
            5,
            aRect.iTl.iX,
            aRect.iTl.iY,
            aRect.iBr.iX,
            aRect.iBr.iY,
            &aWindowTreeNode
            );    
        }
	iAlfSendBuffer->CommitL();

	AMT_MAP_RENDER_STAGE_NODE_EXTENT_CHANGED();
    }

// ---------------------------------------------------------------------------
// SiblingOrderChanged
// ---------------------------------------------------------------------------
//
void CAlfRenderStage::SiblingOrderChanged(const MWsWindowTreeNode& aWindowTreeNode, TInt aNewPos)
    {
    __ALFLOGSTRING("CAlfRenderStage::SiblingOrderChanged");
    TInt nodeType = aWindowTreeNode.NodeType();
    iAlfSendBuffer->WriteIntsL( EAlfNodeSiblingOrderChanged,
        2,
        aNewPos,
        &aWindowTreeNode );

    iAlfSendBuffer->CommitL();
    }

// ---------------------------------------------------------------------------
// FlagChanged
// ---------------------------------------------------------------------------
//
void CAlfRenderStage::FlagChanged(const MWsWindowTreeNode& aWindowTreeNode, TInt aFlag, TBool aNewValue)
    {
    __ALFLOGSTRING("CAlfRenderStage::FlagChanged");
    iAlfSendBuffer->WriteIntsL(
            EAlfNodeFlagChanged,
            3,
            aFlag,
            aNewValue,
            &aWindowTreeNode   );
	iAlfSendBuffer->CommitL();

	AMT_MAP_RENDER_STAGE_FLAG_CHANGED();
    }

// ---------------------------------------------------------------------------
// AttributeChanged
// ---------------------------------------------------------------------------
//
void CAlfRenderStage::AttributeChanged(const MWsWindowTreeNode& aWindowTreeNode, TInt aAttribute)
    {
    if ( aWindowTreeNode.NodeType() == MWsWindowTreeNode::EWinTreeNodeClient &&
         aWindowTreeNode.Window() != NULL )
        {
        TInt32 nodeId = (TInt32)&aWindowTreeNode;
        
        switch ( aAttribute )
            {
        case MWsWindowTreeObserver::EWindowShape:
            {
            TPoint origin( aWindowTreeNode.Window()->AbsRect().iTl );
            
            if ( aWindowTreeNode.Window()->WindowArea().Count() )
                {
                iAlfSendBuffer->WriteRegionIntsL( 
                    EAlfNodeSetWindowArea, 
                    aWindowTreeNode.Window()->WindowArea(), 
                    3, 
                    nodeId, 
                    origin.iX,
                    origin.iY );
                }
            else
                {
                iAlfSendBuffer->WriteIntsL( EAlfNodeSetWindowArea, 4, nodeId, 0, 0, 0 ); // no region
                }
            
            iAlfSendBuffer->CommitL();
            }
            break;
            
        default:
            break;
            }
        }
        
    if ( aWindowTreeNode.NodeType() == MWsWindowTreeNode::EWinTreeNodeStandardTextCursor )
        {
        TInt32 nodeId = (TInt32)&aWindowTreeNode;
        switch( aAttribute )
            {
            case ECursorType:
                {
                iAlfSendBuffer->WriteIntsL(
                        EAlfNodeAttributeChanged,
                        3,
                        nodeId,
                        aAttribute,
                        aWindowTreeNode.StandardTextCursor()->Type() );
                break;
                }
            case ECursorClipRect:
                {
                const TRect rect = aWindowTreeNode.StandardTextCursor()->ClipRect();
                iAlfSendBuffer->WriteIntsL(
                        EAlfNodeAttributeChanged,
                        6,
                        nodeId,
                        aAttribute,
                        rect.iTl.iX,
                        rect.iTl.iY,
                        rect.iBr.iX,
                        rect.iBr.iY );

                const MWsStandardTextCursor* cursor = aWindowTreeNode.StandardTextCursor();       
                DoDrawTextCursor( aWindowTreeNode, cursor->Rect(), cursor->ClipRect(), cursor->Rect() );
                break;
                }
            case ECursorFlags:
                {
                iAlfSendBuffer->WriteIntsL(
                        EAlfNodeAttributeChanged,
                        3,
                        nodeId,
                        aAttribute,
                        aWindowTreeNode.StandardTextCursor()->Flags() );
                break;
                }
            case ECursorColor:
                {
                iAlfSendBuffer->WriteIntsL(
                        EAlfNodeAttributeChanged,
                        3,
                        nodeId,
                        aAttribute,
                        aWindowTreeNode.StandardTextCursor()->Color().Internal() );
                break;
                }
            }
        iAlfSendBuffer->CommitL();
        }
    
    AMT_MAP_RENDER_STAGE_TEXT_CURSOR_CHANGE();
    }

// ---------------------------------------------------------------------------
// FadeCountChanged
// ---------------------------------------------------------------------------
//
void CAlfRenderStage::FadeCountChanged(const MWsWindowTreeNode& aWindowTreeNode, TInt aFadeCount)
    {
    iAlfSendBuffer->WriteIntsL( EAlfNodeFadeCountChanged, 2, (TInt32)&aWindowTreeNode, aFadeCount );
	iAlfSendBuffer->CommitL();
    }

// ---------------------------------------------------------------------------
// TransparentRegionChanged
// ---------------------------------------------------------------------------
//
void CAlfRenderStage::TransparentRegionChanged(const MWsWindowTreeNode& /*aWindowTreeNode*/, const TRegion& /*aNewTransparentRegion*/, const TRegion* /*aNewOpaqueRegion*/)
    {
    // @todo
	// NULL region received from wserv. causes crash. quickfix is to ignore this as the
	// output is ignored in anycase in alfhierarchymodel.cpp
    /*
    iAlfSendBuffer->WriteIntsL( EAlfNodeTransparentRegionChanged, 1, (TUint32)&aWindowTreeNode );
    iAlfSendBuffer->WriteRegionL( EAlfNodeTransparentRegionChanged, aNewTransparentRegion );
    iAlfSendBuffer->WriteRegionL( EAlfNodeTransparentRegionChanged, *aNewOpaqueRegion );
    iAlfSendBuffer->CommitL();
    */
    }


// ---------------------------------------------------------------------------
// MovedToWindowGroup
// ---------------------------------------------------------------------------
//
void CAlfRenderStage::MovedToWindowGroup(const MWsWindowTreeNode& aWindowTreeNode, const MWsWindowTreeNode& aNewWindowGroupNode)
    {
    iAlfSendBuffer->WriteIntsL( EAlfNodeMovedToWindowGroup, 2, (TUint32)&aWindowTreeNode, (TUint32)&aNewWindowGroupNode );
    iAlfSendBuffer->CommitL();
    }

// ---------------------------------------------------------------------------
// WindowGroupChained
// ---------------------------------------------------------------------------
//
void CAlfRenderStage::WindowGroupChained(const MWsWindowTreeNode& aParent, const MWsWindowTreeNode& aChild)
    {
    __ALFLOGSTRING("CAlfRenderStage::WindowGroupChained");
    iAlfSendBuffer->WriteIntsL( EAlfNodeWindowGroupChained, 2, (TUint32)&aParent, (TUint32)&aChild );
	iAlfSendBuffer->CommitL();
    }

// ---------------------------------------------------------------------------
// WindowGroupChainBrokenAfter
// ---------------------------------------------------------------------------
//
void CAlfRenderStage::WindowGroupChainBrokenAfter(const MWsWindowTreeNode& aWindowGroupNode)
    {
    iAlfSendBuffer->WriteIntsL( EAlfNodeWindowGroupChainBrokenAfter, 1, (TUint32)&aWindowGroupNode );
    iAlfSendBuffer->CommitL();
    }


// ---------------------------------------------------------------------------
// WindowRedrawStart
// ---------------------------------------------------------------------------
//
void CAlfRenderStage::WindowRedrawStart( const MWsWindowTreeNode& aWindowTreeNode, const TRegion& aRegion )
    {
    __ALFLOGSTRING("AlfRenderStage::WindowRedrawStart");
    iWsGraphicsContext->ResetDrawCommandCount();
    // This is being saved since SegmentRedraw stuff will call WindowRedraw methods 
    iWindowDrawingNode = &aWindowTreeNode;

#ifdef ALF_DEBUG_TRACK_DRAWING
    TInt trackThisNode = 0;
    if ( trackThisNode )
        {
        iAlfSendBuffer->WriteIntsL( EAlfDebugTrackNode, 2, (TInt32)&aWindowTreeNode, 1 );
        }
#endif   
    
    TInt shapeRectCount = 0;
    
    if ( aWindowTreeNode.NodeType() == MWsWindowTreeNode::EWinTreeNodeClient )
        {
        const TRegion& windowArea = aWindowTreeNode.Window()->WindowArea();
        if ( !(windowArea.Count() == 1 && windowArea.BoundingRect() == aWindowTreeNode.Window()->AbsRect())
                && windowArea.Count() > 0)
            {
            /*
            TRect rect = aWindowTreeNode.Window()->AbsRect();
            RDebug::Print(_L("%d %d %d %d"), rect.iTl.iX, rect.iTl.iY, rect.iBr.iX, rect.iBr.iY );
            
            for ( TInt i= 0; i < windowArea.Count();i++)
                {
                TRect rect = windowArea[i]; 
                RDebug::Print(_L("CAlfRenderStage::WindowRedrawStart - Shape") );
                RDebug::Print(_L("%d %d %d %d"), rect.iTl.iX, rect.iTl.iY, rect.iBr.iX, rect.iBr.iY );
                }
            */
            
            shapeRectCount = windowArea.Count();
            __ALFLOGSTRING1("CAlfRenderStage::WindowRedrawStart - some shape, %d rects", shapeRectCount );
            }
        }
    // Write window header
    iAlfSendBuffer->WriteWindowDataL( (TUint32)&aWindowTreeNode, aRegion.Count(), shapeRectCount );
    
    iAlfSendBuffer->WriteRegionL( EAlfSetUpdateRegion, aRegion );

    if ( shapeRectCount )
        {
        // Write Shape region only if defined
        iAlfSendBuffer->WriteRegionL( EAlfSetShapeRegion, aWindowTreeNode.Window()->WindowArea() );
        }
    }
 
// ---------------------------------------------------------------------------
// WindowRedrawEnd
// ---------------------------------------------------------------------------
//
void CAlfRenderStage::WindowRedrawEnd( const MWsWindowTreeNode& /*aWindowTreeNode*/ )
    {
    __ALFLOGSTRING("AlfRenderStage::WindowRedrawEnd");
    iSegmentRedrawActive = EFalse;
    iAlfSendBuffer->EndFrameL();
    }

// ---------------------------------------------------------------------------
// WindowAnimRedrawStart
// ---------------------------------------------------------------------------
//
void CAlfRenderStage::WindowAnimRedrawStart( const MWsWindowTreeNode& aWindowTreeNode, const TRegion& aRegion )
    {
    //_ALFLOGSTRING("AlfRenderStage::WindowAnimRedrawStart");
    WindowRedrawStart( aWindowTreeNode, aRegion );
    }

// ---------------------------------------------------------------------------
// WindowAnimRedrawEnd
// ---------------------------------------------------------------------------
//
void CAlfRenderStage::WindowAnimRedrawEnd( const MWsWindowTreeNode& aWindowTreeNode )
    {
    //_ALFLOGSTRING("AlfRenderStage::WindowAnimRedrawEnd");
    WindowRedrawEnd( aWindowTreeNode );    
    }

// ---------------------------------------------------------------------------
// SpriteRedrawStart
// ---------------------------------------------------------------------------
//
void CAlfRenderStage::SpriteRedrawStart( const MWsWindowTreeNode& aWindowTreeNode, const TRegion& aRegion )
    {
    WindowRedrawStart( aWindowTreeNode, aRegion );
    iSpriteRedraw = &aWindowTreeNode;
    iSpriteRedrawFlash = ESpriteFlashUndefined;
    }

// ---------------------------------------------------------------------------
// SpriteRedrawEnd
// ---------------------------------------------------------------------------
//
void CAlfRenderStage::SpriteRedrawEnd( const MWsWindowTreeNode& aWindowTreeNode )
    {
    WindowRedrawEnd( aWindowTreeNode );
    const MWsWindowTreeNode* spriteRedraw = iSpriteRedraw;
    iSpriteRedraw = NULL;
    
    if ( spriteRedraw && iSpriteRedrawFlash != ESpriteFlashUndefined )
        {
        SpriteFlash( *spriteRedraw, iSpriteRedrawFlash == ESpriteFlashOn );
        iSpriteRedrawFlash = ESpriteFlashUndefined;
        }
    }

// ---------------------------------------------------------------------------
// SpriteFlash
// ---------------------------------------------------------------------------
//
void CAlfRenderStage::SpriteFlash( const MWsWindowTreeNode& aWindowTreeNode, TBool aFlashOn )
    {
    __ALFLOGSTRING1("CAlfRenderStage::SpriteFlash %d", aFlashOn );
    if ( !iSpriteRedraw )
        {
        FlagChanged( aWindowTreeNode, MWsWindowTreeObserver::EVisible, aFlashOn );
        }
    else
        {
        iSpriteRedrawFlash = aFlashOn ? ESpriteFlashOn : ESpriteFlashOff;
        }
    }

// ---------------------------------------------------------------------------
// SegmentRedrawStart
// ---------------------------------------------------------------------------
//
void CAlfRenderStage::SegmentRedrawStart( const TRegion& aRegion)
    {
    // If this is the first (and only ) time SegmentRedraw is being called within WindowRedrawStart
    // and WindowRedrawEnd, nothing needs to be done.
    //
    // Otherwise, if there's multiple segment redraws between WindowRedrawStart/End, we will separate
    // each segment to its own buffer.
    if (iSegmentRedrawActive)
        {
        WindowRedrawEnd(*iWindowDrawingNode);
        WindowRedrawStart(*iWindowDrawingNode, aRegion);
        }
    iSegmentRedrawActive = ETrue;
    }

// ---------------------------------------------------------------------------
// SegmentRedrawEnd
// ---------------------------------------------------------------------------
//
void CAlfRenderStage::SegmentRedrawEnd(  )
    {
    }

// ---------------------------------------------------------------------------
// FadeAllChildren
// ---------------------------------------------------------------------------
//
void CAlfRenderStage::FadeAllChildren(const MWsWindowTreeNode& aWindowTreeNode, TBool aFaded)
    {
    TUint32 nodeId = (TUint32)&aWindowTreeNode;
    iAlfSendBuffer->WriteIntsL( EAlfNodeFadeAllChildren, 2, nodeId, aFaded );                      
    iAlfSendBuffer->CommitL();
    }

// ---------------------------------------------------------------------------
// InsertTagL
// ---------------------------------------------------------------------------
//
void CAlfRenderStage::InsertTagL( TInt aTag, TInt32 /*aParameter*/ )
    {
    // Must be between WindowRedrawStart and WindowRedrawEnd calls
    
    switch( aTag )
        {
        case CAlfHintGraphic::EAlfOffscreenContentBegin:
            {
            iAlfSendBuffer->SetFlag( EAlfTransparentContent );
            iAlfSendBuffer->WriteIntL( EAlfContentTag, aTag );
            break;
            }
        case CAlfHintGraphic::EAlfOffscreenContentEnd:
            {
            iAlfSendBuffer->WriteIntL( EAlfContentTag, aTag );
            break;
            }
        case  CAlfHintGraphic::EAlfOffscreenContentFlush:
            {
            iAlfSendBuffer->SetFlag( EAlfTransparentContentFlush );
            iAlfSendBuffer->SetFlag( EAlfTransparentContent );
            iAlfSendBuffer->WriteIntL( EAlfContentTag, aTag );
            break;
            }
        case CAlfHintGraphic::EAlfSetWindowId:
            {
            break;
            }
        case CAlfHintGraphic::EAlfEndSubWindow:
            {
#ifdef RD_SUBWINDOW_EFFECTS            
            iAlfSendBuffer->EndMarkerL();
#endif
            break;
            }
            
        default:
            {
            break;
            }
        }
    }

void CAlfRenderStage::InsertTagL( TInt aTag, TRect aBoundingRectangle, TInt aLayerId )
    {
    switch(aTag)
        {
        case CAlfHintGraphic::EAlfBeginSubWindow:
            {
#ifdef RD_SUBWINDOW_EFFECTS             
            iAlfSendBuffer->StartMarkerL(aBoundingRectangle, aLayerId);
#endif
            break;
            }
        }
    
    }


// end of file    
