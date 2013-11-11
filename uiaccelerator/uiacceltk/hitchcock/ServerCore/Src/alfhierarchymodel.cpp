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



#include <graphics/wsgraphicscontext.h>
#include <graphics/wsgraphicdrawerinterface.h>
#include <bitstd.h>
#include <fbs.h>
#include <gdi.h>
#include <e32math.h>
#include <e32hashtab.h>
#include <uiacceltk/HuiUtil.h>

#include "alfhierarchymodel.h"
#include "alfstreamerconsts.h"
#include "alfstreamerserver.h"
#include "alfwindowmanager.h"
#include "alfstreamerbridge.h"
#include "alfwindow.h"
#include "alflogger.h"
#ifdef ALF_DEBUG_TRACK_DRAWING 
#include "alfcommanddebug.h"
#endif
#include "alfnodes.h"
#include "huiwscanvascommands.h"

#include "mwsgraphicscontexttobitgdimappings.h"

const TInt KChunkCommitBatchSize = 10000;
const TInt KChunkMaxSize = 500000;
const TInt KFrameOffsetTemplate = 12345678;

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CAlfHierarchyModel* CAlfHierarchyModel::NewL(CAlfStreamerServer& aServer)
    {
    CAlfHierarchyModel* self = new(ELeave)CAlfHierarchyModel(aServer);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CAlfHierarchyModel::ConstructL()
    {
    iWindowPos = TPoint(0, 0 );
    iSema.CreateLocal();
    if (iServer.Bridge())
        {
        iServer.Bridge()->SetBatchObserver(this);
        }                
    #ifdef ALF_DEBUG_TRACK_DRAWING 
    iCommandDebugger = CAlfCommandDebug::NewL();
    #endif

    #ifdef USE_MODULE_TEST_HOOKS_FOR_ALF
    // Initiliaze global data in TLS and Open global module testing chunk and mutex
    User::LeaveIfError(Dll::SetTls(new(ELeave) CAlfModuleTestDataControl()));
    User::LeaveIfError(AMT_CONTROL()->OpenGlobalObjects());
    #endif
    
	}

// ---------------------------------------------------------------------------
// AppendScreenL
// ---------------------------------------------------------------------------
//
void CAlfHierarchyModel::AppendScreen( TInt aScreenNumber )
    {
    __ALFLOGSTRING1("CAlfHierarchyModel::AppendScreenL( %d )", aScreenNumber );
    if ( aScreenNumber > 0 ) // first display comes with the package. only the following are created when requested
        {
        iServer.Bridge()->AddData( EAlfDSCreateNewDisplay, aScreenNumber, 0, NULL );
        }
    }

// ---------------------------------------------------------------------------
// RemoveScreenL
// ---------------------------------------------------------------------------
//
void CAlfHierarchyModel::RemoveScreen( TInt aScreenNumber )
    {
    if ( aScreenNumber < 1 ) 
		{
		// The main display cannot be removed.
		return;
		}
	
    __ALFLOGSTRING1("CAlfHierarchyModel::RemoveDisplayL( %d )", aScreenNumber );
    // NOTE & TODO: This works fine, if we are removing the last added display. If we happen to remove display in the middle,
    // then referring to displays by their indeces would be a major problem
    // TODO: Who destroyes the nodes on this display? wserv?
    iServer.Bridge()->AddData( EAlfDSDestroyDisplay, aScreenNumber, 0, NULL );
    }

// ---------------------------------------------------------------------------
// destructor
// ---------------------------------------------------------------------------
//
CAlfHierarchyModel::~CAlfHierarchyModel()
    {
    // we do not complete pending messages 
	iPendingAlfWindowDataMessages.Close(); 
	iNodeHashArray.Close();
    if (iServer.Bridge())
        {
        iServer.Bridge()->SetBatchObserver(0);
        }
    delete iPeriodic;
    iSema.Close();
    if(iStream)
        {
        iStream->Close();
        delete iStream;
        iStream = NULL;
        }
    iChunk.Close();
#ifdef ALF_DEBUG_TRACK_DRAWING 
    delete iCommandDebugger;
#endif
    iUpdateRegion.Close();    
#ifdef USE_MODULE_TEST_HOOKS_FOR_ALF
    delete AMT_CONTROL();
    Dll::FreeTls();
#endif
    }

// ---------------------------------------------------------------------------
// ReleaseSemaphor
// ---------------------------------------------------------------------------
//
void ReleaseSemaphor(TAny* aAlfHierarchyModel)
    {
    CAlfHierarchyModel* me = (CAlfHierarchyModel*)aAlfHierarchyModel;
    me->DoReleaseSemaphor();
    }

// ---------------------------------------------------------------------------
// DoReleaseSemaphor
// ---------------------------------------------------------------------------
//
void CAlfHierarchyModel::DoReleaseSemaphor()
    {
    iSema.Signal();
    }

// ---------------------------------------------------------------------------
// RequestPacketEndCallback
// 
// This is used to notify when certain offset in the chunk is freed. To avoid 
// excessive notifications (and callbacks), skip some of the notifications.
// ---------------------------------------------------------------------------
//
void CAlfHierarchyModel::RequestPacketEndCallback( TInt aOffset )
    {
    if ( aOffset > iPreviousCallbackOffset + KChunkCommitBatchSize || aOffset < iPreviousCallbackOffset )
        {
        iPreviousCallbackOffset = aOffset;
        iServer.Bridge()->RequestCommandReadNotification( aOffset );
        }
    }

// ---------------------------------------------------------------------------
// RequestFrameEndCallback
// ---------------------------------------------------------------------------
//
void CAlfHierarchyModel::RequestFrameEndCallback(TBool aCompositionModified)
    {
    __ALFLOGSTRING1( "CAlfHierarchyModel::RequestFrameEndCallback, composition modified: %d", aCompositionModified);
    iServer.Bridge()->StartNewBlock(aCompositionModified);
    }
// ---------------------------------------------------------------------------
// HandleMessageL
// ---------------------------------------------------------------------------
//
void CAlfHierarchyModel::HandleMessageL( const RMessage2& aMessage )
    {
    iSema.Wait();
    CleanupStack::PushL(TCleanupItem(ReleaseSemaphor, this));
    TBool doComplete= EFalse;
    // __ALFLOGSTRING1( "CAlfHierarchyModel::HandleMessageL %d", aMessage.Function() );
    switch (aMessage.Function())
        {
        case EAlfBridgerRequestDataBlock:
            {
            ShareChunkL( aMessage );
            return;
            }
        case EDsNotifyNativeWindowData:
            {
            User::Leave(KErrNotSupported);
            return;
            }
        case EAlfBridgerAsyncronousData:
            {
            if ( iChunk.Handle() )
                {
                ExecuteCommandsL(aMessage.Int1());
                if ( iBatchAlreadyCommited )
                    {
                    // This may happen, if alfstreamerbridge happens to process the commands (containing the EAlfCommitBatch) 
                    //  before this asyncronous request is received.
                    __ALFLOGSTRING("Batch already completed. Complete immediately");
                    aMessage.Complete( EAlfBridgerAsyncronousData );
                    iBatchAlreadyCommited = EFalse;
                    }
                else
                    {
                    SetSynchMessage(&aMessage);
                    }
                }
            break;
            }
        case EAlfBridgerBlindSend:
            {
            ExecuteCommandsL();
            CleanupStack::PopAndDestroy(); // releases iSema
            aMessage.Complete( EAlfBridgerBlindSend );
            return;
            }
        case EAlfBridgerSendChunk:
            {
            OpenChunkL( aMessage );
			aMessage.Complete( EAlfBridgerSendChunk );
            return;
            }
        case EAlfSynchronize:
            {
            iServer.Bridge()->AddData( EAlfDSSynchronize, aMessage.Int0() );
            aMessage.Complete( KErrNone );
            }
            break;
            
        default:
            {
            doComplete= ETrue;
            __ALFLOGSTRING("CAlfHierarchyModel::HandleMessageL, default case reached.");
            break;
            }
        }
    if (doComplete && !aMessage.IsNull())
        {
        aMessage.Complete(KErrNotSupported);
        }
           
    CleanupStack::PopAndDestroy(); // releases iSema
    }

// ---------------------------------------------------------------------------
// ShareChunkL
// ---------------------------------------------------------------------------
//
void CAlfHierarchyModel::ShareChunkL( const RMessage2& aMessage )
    {
    if ( !iChunk.Handle() )
        {
        // For security reasons, alfserver owns the chunk. Because it is nameless, it cannot be accessed
        // unless a handle to the chunk is given by alfserver
        //
        // TODO:CHECK what if somebody else wants to send EAlfBridgerRequestDataBlock. Currently we are giving the chunk
        // to anybody who connects to the server.
        iChunk.CreateDisconnectedGlobal( KNullDesC, 0, KChunkMaxSize, KChunkMaxSize ); //CreateDisconnectedGlobal
        iChunkHeader  = reinterpret_cast<TChunkHeader*>(iChunk.Base());
        memset( iChunkHeader, 0, sizeof( TChunkHeader ) );
        }
    TInt screenNumber = aMessage.Int1();
    TPckg<TInt> pkgLength( KChunkMaxSize );
    AppendScreen( screenNumber );
    aMessage.WriteL(0, pkgLength );
    aMessage.Complete( iChunk );
    if ( iStream )
        {
        iStream->Close();
        }
    else
        {
        iStream = new(ELeave)RMemReadStream;
        }
    iStream->Open( iChunk.Base() + sizeof( TChunkHeader), KChunkMaxSize );
    
    CleanupStack::PopAndDestroy(); // releases iSema
}

// ---------------------------------------------------------------------------
// ShareChunkL
// ---------------------------------------------------------------------------
//
void CAlfHierarchyModel::OpenChunkL( const RMessage2& aMessage )
    {
    TInt size = aMessage.Int0();
    TInt32 handle = aMessage.Int1();
    TBool openForReading = aMessage.Int2();
    
    iCacheChunks.Insert( handle, RChunk() );
    iCacheChunks.Find( handle )->SetHandle( aMessage.Int1() );
    iCacheChunks.Find( handle )->Open( aMessage, 1, EFalse, EOwnerProcess );
    
    if ( openForReading ) 
        {
        if (iStream)
            {
            iStream->Close();
            }
        else
            {
            iStream = new(ELeave)RMemReadStream;
            }
        
        TUint8*  base = iCacheChunks.Find( handle )->Base();
        iStream->Open( base + sizeof( TChunkHeader), size );

        __ALFLOGSTRING1("Chunk in use %d", iChunkInUse );
        iChunkInUse = handle;    
        iChunkHeader  = reinterpret_cast<TChunkHeader*>(base);
        }
    CleanupStack::PopAndDestroy(); // releases iSema
  
}

// ---------------------------------------------------------------------------
// CloseChunkL
// ---------------------------------------------------------------------------
//
void CAlfHierarchyModel::CloseChunk( TInt32 aChunkHandle )
    {
    __ALFLOGSTRING1("CAlfHierarchyModel::CloseChunkL: closing %d", aChunkHandle );
    RChunk* chunk = iCacheChunks.Find( aChunkHandle );
    iCacheChunks.Remove( aChunkHandle );
    if ( chunk )
        {
        chunk->Close();
        }
    else
        {
        __ALFLOGSTRING1( "CAlfHierarchyModel::CloseChunkL - Warning: chunk %d not found!", aChunkHandle );
        }
    }

// ---------------------------------------------------------------------------
// ReadEndMarkerL
// ---------------------------------------------------------------------------
//
void CAlfHierarchyModel::ReadEndMarkerL()
    {
    TUint8 marker = iStream->ReadInt8L();
    ASSERT( marker == EAlfCommandEndMarker ) ; // endmarker
    }
    
// ---------------------------------------------------------------------------
// ReadRectL
// ---------------------------------------------------------------------------
//
void CAlfHierarchyModel::ReadRectL( TRect& aRect, RMemReadStream* aStream )
    {
    aStream->ReadL( (TUint8*)&(aRect.iTl.iX), sizeof( TRect) );
    }

// ---------------------------------------------------------------------------
// ReadRegionL
// ---------------------------------------------------------------------------
//
void CAlfHierarchyModel::ReadRegionL( RMemReadStream* aStream, RRegion& aRegion, TPoint aWindowPos )
    {
    aRegion.Clear();
    TRect rect;
    TInt count = aStream->ReadInt32L();
    for (TInt i = 0 ; i < count ; i++ )
        {
        ReadRectL( rect, aStream );
        rect.Move(-aWindowPos);
        aRegion.AddRect( rect );
        }
    }
	

// ---------------------------------------------------------------------------
// ExecuteCommandsL
// ---------------------------------------------------------------------------
//
void CAlfHierarchyModel::ExecuteCommandsL( TBool aCompositionModified )
    {
    if ( !iStream )
        {
        return;
        }
    TInt offset = iStream->Source()->TellL( MStreamBuf::ERead ).Offset();


#ifdef _ALF_LOGGING
    TInt commandCount(0);
    TSgcCanvasCommands previousCommand;
#endif
    TSgcCanvasCommands command = EAlfCommandNotInitialized;
    TTime beginTime;
    beginTime.HomeTime();
    
    __ALFLOGSTRING1( "CAlfHierarchyModel::ExecuteCommandsL - Committed write offset: %d", iChunkHeader->iCommittedWriteOffset );
    while( iChunkHeader->iCommittedWriteOffset != offset )
        {
#ifdef _ALF_LOGGING
        previousCommand = command;
#endif
        TRAPD( err,
                command = (TSgcCanvasCommands)iStream->ReadUint8L();
                iScreenNumber = iStream->ReadUint8L();
            );
        if ( err != KErrNone )
	          {
#ifdef _ALF_LOGGING
	          __ALFLOGSTRING3("CAlfHierarchyModel::ExecuteCommandsL - Cmd: %d, Previous Cmd: %d, Count: %d..", command, previousCommand, commandCount );
#endif	          
	          __ALFLOGSTRING2("..CAlfHierarchyModel::ExecuteCommandsL - Offset: %d, LEAVE WITH ERROR: %d", offset, err  );
              }
        
#ifdef ALF_DEBUG_TRACK_DRAWING 
        commandCount++;
        iUsedCommands[command]++;

        iCommandDebugger->SetDescription( command );
        __ALFLOGSTRING3("CAlfHierarchyModel::ExecuteCommandsL (%d) %S offset: %d,", command, &iCommandDebugger->Text(), offset );
#endif        
        switch ( command )
            {
        	case EAlfSetWindowId:
                {
                TUint32 nodeId = (TUint32)iStream->ReadUint32L();
                CAlfNodeVisual* node = (CAlfNodeVisual*)FindNode( nodeId );

                if ( node )
                    {
#ifdef ALF_DEBUG_TRACK_DRAWING					
                    if (node->iId == iSearchNode || node->Tracking() )
                        {
                        RDebug::Print( _L( "CAlfHierarchyModel::ExecuteCommandsL - Tracking"));
                        }
#endif						
#ifdef ALF_DEBUG_PRINT_NODE_INFO
                    _LIT( KText, "Drawing");
                    // node->PrintInfo(0, node, TPtrC(KText) , iSearchNode);
#endif

                    node->DrawWindowFrameL( *iStream );
                    }
                else
                    {
                    ProcessUnknownNodeDrawingL( *iStream );
                    }
                break;
                }
            case EAlfNodeCreated:
                {
                DoNodeCreatedL();
                break;
                }
            case EAlfNodeReleased:
                {
                DoNodeReleasedL();
                break;
                }
            case EAlfNodeActivated:
                {
                DoNodeActivatedL();
                break;
                }
            case EAlfNodeExtentChanged:
                {
                DoNodeExtentChangedL();
                break;
                }
            case EAlfNodeSiblingOrderChanged:
                {
                DoNodeSiblingOrderChangedL();
                break;
                }
            case EAlfNodeFlagChanged:
                {
                DoNodeFlagChangedL();
                break;
                }
            case EAlfNodeFadeCountChanged:
                {
                DoNodeFadeCountChangedL();
                break;
                }                
            case EAlfNodeFadeAllChildren:
                {
                DoNodeFadeAllChildrenL();
                break;
                }                
            case EAlfNodeTransparentRegionChanged:
                {
                DoNodeTransparentRegionChangedL();
                break;
                }
            case EAlfNodeLayerAdded:
                {
                DoNodeLayerAddedL();
                break;
                }
            case EAlfNodeLayerExtentChanged:
                 {
                 DoNodeLayerExtentChangedL();
                 break;
                 }               
            case EAlfNodeLayerUsesAlphaFlagChanged:
                 {
                 DoNodeLayerUsesAlphaFlagChangedL();
                 break;
                 }               
            case EAlfNodeMovedToWindowGroup:
                {
                DoNodeMovedToWindowGroupL();
                break;
                }
            case EAlfNodeWindowGroupChained:
                {
                DoNodeWindowGroupChainedL();
                break;
                }
            case EAlfNodeWindowGroupChainBrokenAfter:
                {
                DoNodeWindowGroupChainBrokenAfterL();
                break;
                }
            case EAlfWrap:
                {
                ReadEndMarkerL(); // futile, but just in case somet
                iStream->Source()->SeekL( MStreamBuf::ERead, TStreamPos(0));
                iChunkHeader->iReadOffset = 0;
                break;
                }
            case EAlfNodeAttributeChanged: // currently only textcursor may receive these events
                {
                DoNodeAttributeChangedL();
                break;
                }
            
            case EAlfNodeSetWindowArea:
                {
                DoNodeSetWindowAreaL();
                }
                break;
                
            case EAlfCommitBatch:
                {
                RequestFrameEndCallback(aCompositionModified);
                break;
                }
#ifdef ALF_DEBUG_TRACK_DRAWING              
            case EAlfDebugTrackNode:
                {
                DoNodeDebugTrackL();
                break;
                }
#endif                
            case EAlfDestroyChunk:
                { 
                TInt chunkId = iStream->ReadInt32L();
                iServer.Bridge()->AddData( EAlfReleaseTemporaryChunk, chunkId );
                break;
                }
            case EAlfJumpToAnotherChunk:
                {
                RequestPacketEndCallback( offset );
                TInt32 readChunkId = iStream->ReadInt32L();
                TInt size = iStream->ReadInt32L();
                ReadEndMarkerL();
                // __ALFLOGSTRING1("Next chunk: %d, size: %d, previous chunk: %d"), readChunkId, size, iChunkInUse );
       
                if ( readChunkId == 0 )
                    {
                    iChunkHeader  = reinterpret_cast<TChunkHeader*>(iChunk.Base());
                    iStream->Close();
                    iStream->Open( iChunk.Base() + sizeof( TChunkHeader), KChunkMaxSize );
                    iStream->Source()->SeekL( MStreamBuf::ERead, TStreamPos(0)); 
                    }
                else
                    {
                    iStream->Close();
                    iStream->Open( iCacheChunks.Find( readChunkId )->Base() + sizeof( TChunkHeader), size );
                    iChunkHeader  = reinterpret_cast<TChunkHeader*>(iCacheChunks.Find( readChunkId )->Base());
                    }
                iChunkInUse = readChunkId;
                
                break;
                }
            default:
                {
                __ALFLOGSTRING("CAlfHierarchyModel::ExecuteCommandsL, off track");
#ifdef _ALF_LOGGING
                __ALFLOGSTRING3("CAlfHierarchyModel::ExecuteCommandsL, off track - Cmd: %d, Previous Cmd: %d, Count: %d..", command, previousCommand, commandCount );
#endif                
                __ALFLOGSTRING1("..CAlfHierarchyModel::ExecuteCommandsL, unrecoverable error : HALTING at offset: %d", offset );
                USER_INVARIANT();  
                break;
                }
            }
        ReadEndMarkerL();
        offset = iStream->Source()->TellL( MStreamBuf::ERead ).Offset();
        RequestPacketEndCallback( offset );
        //__ALFLOGSTRING1("end offset %d  offset %d, command %d"), aEndOffset, offset, command );
        };
    iFrame++;
    }

// ---------------------------------------------------------------------------
// BridgerCallback
// This is called by CAlfStreamBridge when EAlfCommitBatch is processed.
//
// Note: This call comes from another thread, Alf server thread. Beware that
//       Alf streamer/decoder server thread might access the class data
//       concurrently!
// ---------------------------------------------------------------------------
//
void CAlfHierarchyModel::BridgerCallback( TInt aOp, TInt aInt )
    {
    // hackish, just do the panic release for window server
    // Trying to mimize changes on existing logic
    if (aOp == KRELEASEWINDOWSERVER)
        {
        __ALFLOGSTRING1("CAlfHierarchyModel::BridgerCallback, iMessage is null: %d", iMessage.IsNull());

        if (!iMessage.IsNull()) 
            {
            __ALFLOGSTRING("CAlfHierarchyModel::BridgerCallback, emergency releasing of window server");
            iMessage.Complete(EAlfBridgerAsyncronousData);
            iDidForcedComplete = ETrue;
            }
        else if (aInt == KRELEASEDBEFOREQUEUE)
            {
            iBatchAlreadyCommited = ETrue;   
            }    
                  
        return;
        }        
        
    switch( aOp )
        {
        case EAlfDSGetAlfNativeWindowData:
            {                        
    		iSema.Wait();
    		// Complete the pending message
            RMessage2* rsMessage = (RMessage2*)aInt;
    		for (TInt i = 0; i < iPendingAlfWindowDataMessages.Count(); i++)
    			{
    			if (&(iPendingAlfWindowDataMessages[i]) == rsMessage)
    				{
    				// Complete the message to RS
    				rsMessage->Complete(KErrNone);
    				iPendingAlfWindowDataMessages.Remove(i);
    				break;
    				}
    			}
		    iSema.Signal();
            break;
            }    

        case EAlfRequestCommandReadNotification:
            {
    		if (iChunkHeader)
    			{
    			iChunkHeader->iReadOffset = aInt; // last read offset
    			}
                    
            // TODO: If you can absolute guaranteen to count the available space correctly, then this can also complete iSpaceNotificationMessage
            break;
            }
        case EAlfRequestCommitBatch:
            {
            __ALFLOGSTRING1("CAlfHierarchyModel::BridgerCallback, iMessage.IsNull %d", iMessage.IsNull());
            if ( iMessage.IsNull())
                {
                if (!iDidForcedComplete)
                    {
                    __ALFLOGSTRING("request for this message has not arrived. complete on arrival");
                    iBatchAlreadyCommited = ETrue;
                    }
                }
            else
                {
                iBatchAlreadyCommited = EFalse;
                iMessage.Complete(EAlfBridgerAsyncronousData);
                }
            iDidForcedComplete = EFalse;
            break;
            }
        case EAlfReleaseTemporaryChunk:
            {
            CloseChunk( aInt );
            break;
            }
        default:
            {
            USER_INVARIANT();                
            }    
        }
    }

// ---------------------------------------------------------------------------
// DoNodeCreatedL
// ---------------------------------------------------------------------------
//
void CAlfHierarchyModel::DoNodeCreatedL()
    {
#ifdef ALF_DEBUG_TRACK_DRAWING 
    TBool trackThisNode(EFalse);
#endif
    CAlfNode* node = NULL;    
    // Nodes add themself to iNodeStructArray, which "owns" the nodes. Nodes are created and destroyed only if requested by the Wserv
    MWsWindowTreeNode::TType nodeType = (MWsWindowTreeNode::TType)iStream->ReadInt32L();
    switch (nodeType)
        {
        case MWsWindowTreeNode::EWinTreeNodeClient:
            {
            node = CAlfNodeWindow::NewL( this, iStream, iScreenNumber );
            
            #ifdef __WINS__
            if (++iDebug_CheckNodeTableItegrityCounter > 100) // Do not check too often as is can be slow
                {
                Debug_CheckNodeTableItegrity(_L("CAlfHierarchyModel::DoNodeCreatedL"));
                }            
            #endif
            
            break;
            }
        case MWsWindowTreeNode::EWinTreeNodeRoot:
            {
            node = CAlfNodeRoot::NewL( this ,iStream, iScreenNumber );
            iRootNode = (CAlfNodeRoot*)node;
            break;
            }
        case MWsWindowTreeNode::EWinTreeNodeGroup:
            {
            node = CAlfNodeGroup::NewL( this, iStream, iScreenNumber );
            break;
            }
        case MWsWindowTreeNode::EWinTreeNodeAnim:
            {
            node = CAlfNodeAnim::NewL(this, iStream, iScreenNumber );
            break;
            }
        case MWsWindowTreeNode::EWinTreeNodeSprite:
            {
            node = CAlfNodeSprite::NewL( this, iStream, iScreenNumber );
            break;
            }
        case MWsWindowTreeNode::EWinTreeNodeStandardTextCursor:
            {
            node = CAlfNodeTextCursor::NewL( this, iStream, iScreenNumber );
            break;
            }
        default:
            {
            __ALFLOGSTRING1("CAlfHierarchyModel::DoNodeCreatedL, unknown node type: %i", nodeType);
            break;
            }
        }
    // Note to Debuggers
    // You can track certain type of nodes and their draw buffers by changing the trackThisNode value to 1. 
    // Then enable breakpoints in 
    // some places marked in code using the ALF_DEBUG_TRACK_DRAWING and HUI_DEBUG_TRACK_DRAWING . The macro 
    // must be enabled in 
    //   alfappservercore.mmp 
    //   coretoolkit.mmp and
    //   alfrenderstageplugin.mmp
    
#ifdef ALF_DEBUG_TRACK_DRAWING 
    if ( trackThisNode )
        {
        node->SetTracking( trackThisNode );
        }
#endif
    
#ifdef ALF_DEBUG_PRINT_NODE_INFO    
    if ( node )
        {
        RDebug::Print(_L("DoNodeCreatedL"));
        iRootNode->iLogging = 1;
        CAlfNode::PrintNodeTree(iRootNode, node->iId);
        iRootNode->iLogging = 0;
        //CAlfNode::PrintInfo( 0, (CAlfNodeVisual*)node, TPtrC(KText) , iSearchNode);
        }
#endif

    AMT_MAP_NODE_CREATED();
    }

// ---------------------------------------------------------------------------
// DoNodeReleasedL
// ---------------------------------------------------------------------------
//
void CAlfHierarchyModel::DoNodeReleasedL()
    {
    MWsWindowTreeNode::TType nodeType = (MWsWindowTreeNode::TType)iStream->ReadInt32L();
    TUint32 nodeId = (TUint32)iStream->ReadUint32L();
    CAlfNode* node = FindNode( nodeId );

#ifdef ALF_DEBUG_PRINT_NODE_INFO    
    if ( node )
        {
        //_LIT(KText,"DoNodeReleasedL");
        CAlfNode::PrintNodeTree(iRootNode, node->iId);
                
//        CAlfNode::PrintInfo( 0, (CAlfNodeVisual*)node, TPtrC(KText) , iSearchNode);
        }
#endif    
    if ( node )
        {
        delete node;
        }
    else
        {
        USER_INVARIANT();
        }

    AMT_MAP_NODE_RELEASED();
    }

// ---------------------------------------------------------------------------
// DoNodeActivatedL
// ---------------------------------------------------------------------------
//
void CAlfHierarchyModel::DoNodeActivatedL()
    {
    TInt nodeType = iStream->ReadInt32L();
    TUint32 nodeId = (TUint32)iStream->ReadUint32L();
    CAlfNode* node = FindNode( nodeId );
    if ( node )
        {
        node->ActivateNode();
        }
    else
        {
        USER_INVARIANT();
        }

    AMT_MAP_NODE_ACTIVATED();
    }

// ---------------------------------------------------------------------------
// DoNodeExtentChangedL
// ---------------------------------------------------------------------------
//
void CAlfHierarchyModel::DoNodeExtentChangedL()
    {
    TRect rect;
    ReadRectL( rect, iStream);
    TUint32 nodeId = (TUint32)iStream->ReadUint32L();

    CAlfNodeVisual* node = (CAlfNodeVisual*)FindNode( nodeId );
    if ( node )
       {
       node->SetExtent( rect );
       }
    else
        {
        USER_INVARIANT();
        }
#ifdef ALF_DEBUG_PRINT_NODE_INFO    
    if ( node )
        {
        RDebug::Print(_L("Extent changed"));
        CAlfNode::PrintNodeTree(iRootNode, node->iId);
        //CAlfNode::PrintInfo( 0, (CAlfNodeVisual*)node, TPtrC(KText), iSearchNode);
        }
#endif

    AMT_MAP_NODE_EXTENT_CHANGED();
    }

// ---------------------------------------------------------------------------
// DoNodeSiblingOrderChangedL
// ---------------------------------------------------------------------------
//
void CAlfHierarchyModel::DoNodeSiblingOrderChangedL()
    {
    TInt newPos = iStream->ReadInt32L();
    TUint32 nodeId = (TUint32)iStream->ReadUint32L();

    CAlfNode* node = FindNode( nodeId );
    if ( node )
        {
        AMT_SET_VALUE(iAST_Temp1, node->OrdinalPosition());
        
        node->SiblingOrderChanged( newPos );
        
        AMT_SET_VALUE(iOrdinalChange, node->OrdinalPosition() - AMT_DATA()->iAST_Temp1);
        }
    else
        {
#ifdef _DEBUG
        RDebug::Print( _L("Missing a node %d"), nodeId );
#endif
        // USER_INVARIANT();
        }
    
#ifdef ALF_DEBUG_PRINT_NODE_INFO    
    if ( node )
        {
        RDebug::Print(_L("Sibling order changed"));
        CAlfNode::PrintNodeTree(iRootNode, node->iId);
                
        // CAlfNode::PrintInfo( 0, (CAlfNodeVisual*)node, TPtrC(KText), iSearchNode );
        }
#endif
    }

// ---------------------------------------------------------------------------
// DoNodeFlagChangedL
// ---------------------------------------------------------------------------
//
void CAlfHierarchyModel::DoNodeFlagChangedL()
    {
    MWsWindowTreeObserver::TFlags flag = (MWsWindowTreeObserver::TFlags)iStream->ReadUint32L();
    TBool newValue = (TBool)iStream->ReadInt32L();
    TUint32 nodeId = (TUint32)iStream->ReadUint32L();

    CAlfNode* node = FindNode( nodeId );
    if ( node )
        {
        node->FlagChanged( flag, newValue );
        
        }
    else
        {
        USER_INVARIANT();
        }

    AMT_MAP_NODE_FLAG_CHANGED();
    }

// ---------------------------------------------------------------------------
// DoNodeFadeAllChildrenL
// ---------------------------------------------------------------------------
//
void CAlfHierarchyModel::DoNodeFadeAllChildrenL()
    {
    TUint32 nodeId = (TUint32)iStream->ReadUint32L();
    TBool faded = (TBool)iStream->ReadInt32L();    

    CAlfNode* node = FindNode( nodeId );
    if ( node )
        {
        node->FadeAllChildren( faded );
        }
    else
        {
        USER_INVARIANT();
        }
    }

// ---------------------------------------------------------------------------
// DoNodeFadeCountChangedL
// ---------------------------------------------------------------------------
//
void CAlfHierarchyModel::DoNodeFadeCountChangedL()
    {
    TUint32 nodeId = (TUint32)iStream->ReadUint32L();
    TInt fadeCount = (TInt)iStream->ReadUint32L();

    CAlfNode* node = FindNode( nodeId );
    if ( node )
        {
        node->FadeCountChanged( fadeCount );
        }
    else
        {
        USER_INVARIANT();
        }
    }

// ---------------------------------------------------------------------------
// DoNodeTransparentRegionChangedL
// ---------------------------------------------------------------------------
//
void CAlfHierarchyModel::DoNodeTransparentRegionChangedL()
    {
    TUint32 nodeId = (TUint32)iStream->ReadUint32L();
    iStream->ReadInt8L();
    RRegion newTransparentRegion;
    RRegion newOpaqueRegion;
    ReadRegionL( iStream, newTransparentRegion );
    iStream->ReadInt8L();
    ReadRegionL( iStream, newOpaqueRegion );
    // TODO: Implement actions
    newTransparentRegion.Close();
    newOpaqueRegion.Close();
    }

// ---------------------------------------------------------------------------
// DoNodeLayerAddedL
// ---------------------------------------------------------------------------
//
void CAlfHierarchyModel::DoNodeLayerAddedL()
    {
    DoNodeLayerExtentChangedL();
    }

// ---------------------------------------------------------------------------
// DoNodeLayerExtentChangedL
// ---------------------------------------------------------------------------
//
void CAlfHierarchyModel::DoNodeLayerExtentChangedL()
    {   
    // Set composition surface extent 
    TUint32 nodeId = (TUint32)iStream->ReadUint32L();
    TRect extent = TRect(0,0,0,0);
    ReadRectL(extent, iStream); 
    TBool isDSA = (TUint32)iStream->ReadUint32L();
    CAlfNodeVisual* node = (CAlfNodeVisual*)FindNode( nodeId );
    if ( node && node->Window() )
        {
        // SetSurfaceExtent is not supported for image visual
        node->Window()->SetSurfaceExtent( extent );
        if (isDSA)
            {
            node->Window()->SetLayerUsesAplhaFlag(KWindowIsDSAHost);
            }
        if (extent.IsEmpty())
            {
            iServer.ReleasePermissionTokenL(node->iId);
            }
        else
            {
            iServer.CreatePermissionTokenL(node->iId, 
                                           node->Window()->WsInfo().iClientSideId.iWindowIdentifer,
                                           node->Window()->WsInfo().iClientSideId.iWindowGroupId );
            }    
        
        }
    else if( node ) // this would mean that node has being orphaneded but not yet deleted
        {
        __ALFLOGSTRING1("CAlfHierarchyModel::DoNodeLayerExtentChangedL node found but window %d was destroyed", nodeId);
        }
    }

// ---------------------------------------------------------------------------
// DoNodeLayerUsesAlphaFlagChangedL
// ---------------------------------------------------------------------------
//
void CAlfHierarchyModel::DoNodeLayerUsesAlphaFlagChangedL()
    {   
    // Set composition surface extent 
    TUint32 nodeId = (TUint32)iStream->ReadUint32L();
    TBool enabled = (TBool)iStream->ReadInt32L();
    CAlfNodeVisual* node = (CAlfNodeVisual*)FindNode( nodeId );
    if ( node && node->Window() )
        {
        // SetLayerUsesAplhaFlag is not supported for image visual
        node->Window()->SetLayerUsesAplhaFlag( enabled );
#ifdef SYMBIAN_GRAPHICS_WSERV_QT_EFFECTS        
        node->FlagChanged(MWsWindowTreeObserver::EAlphaChannelTransparencyEnabled, enabled);
#endif // #ifdef SYMBIAN_GRAPHICS_WSERV_QT_EFFECTS        
        }
    else if( node ) // this would mean that node has being orphaneded but not yet deleted
        {
        __ALFLOGSTRING1("CAlfHierarchyModel::DoNodeLayerUsesAlphaFlagChangedL node found but window %d was destroyed", nodeId);
        }
    }


// ---------------------------------------------------------------------------
// DoNodeMovedToWindowGroupL
// ---------------------------------------------------------------------------
//
void CAlfHierarchyModel::DoNodeMovedToWindowGroupL()
    {
    TUint32 nodeId = (TUint32)iStream->ReadUint32L();
    TUint32 newWindowgroupNode = (TUint32)iStream->ReadUint32L();
    CAlfNodeWindow* node = (CAlfNodeWindow*)FindNode( nodeId );
    if ( node )
        {
        node->MoveToWindowGroup( newWindowgroupNode );
		} 
    }

// ---------------------------------------------------------------------------
// DoNodeWindowGroupChainedL
// ---------------------------------------------------------------------------
//
void CAlfHierarchyModel::DoNodeWindowGroupChainedL()
    {
    TUint32 parentNode = (TUint32)iStream->ReadUint32L();
    TUint32 childNode = (TUint32)iStream->ReadUint32L();
    CAlfNodeGroup* node = (CAlfNodeGroup*)FindNode( parentNode );
    if ( node )
    	{
    	node->WindowGroupChained( childNode );
    	}
    }

// ---------------------------------------------------------------------------
// DoNodeWindowGroupChainBrokenAfterL
// ---------------------------------------------------------------------------
//
void CAlfHierarchyModel::DoNodeWindowGroupChainBrokenAfterL()
    {
    TUint32 nodeId = (TUint32)iStream->ReadUint32L();
    CAlfNodeGroup* node = (CAlfNodeGroup*)FindNode( nodeId );
    if ( node )
    	{
    	node->GroupChainBrokenAfter();
    	}
    }

// ---------------------------------------------------------------------------
// DoNodeAttributeChangedL
// ---------------------------------------------------------------------------
//
void CAlfHierarchyModel::DoNodeAttributeChangedL()
    {
    TUint32 nodeId = (TUint32)iStream->ReadUint32L();
    CAlfNodeTextCursor* node = (CAlfNodeTextCursor*)FindNode( nodeId);
    if ( node && node->Type() == MWsWindowTreeNode::EWinTreeNodeStandardTextCursor )
        {
        node->AttributeChangedL( iStream );

#ifdef ALF_DEBUG_PRINT_NODE_INFO    
    if ( node )
        {
        //_LIT(KText,"Attribute changed");
        //CAlfNode::PrintInfo( 0, (CAlfNodeVisual*)node, TPtrC(KText) , iSearchNode);
        //CAlfNode::PrintNodeTree(iRootNode, KText, node->iId);
                
        }
#endif  
        }
    else
        {
        USER_INVARIANT(); // attribute change for unexpected node type. new code needed!
        }

    AMT_INC_COUNTER_IF(node, iTotalNodeAttributeChangedCount );
    }
    
// ---------------------------------------------------------------------------
// DoNodeAttributeChangedL
// ---------------------------------------------------------------------------
//
void CAlfHierarchyModel::DoNodeSetWindowAreaL()
    {
    TUint32 nodeId = (TUint32)iStream->ReadUint32L();
    CAlfNode* node = FindNode(nodeId);
    if ( node && node->Type() == MWsWindowTreeNode::EWinTreeNodeClient )
        {
        CAlfNodeWindow* w = static_cast<CAlfNodeWindow*>(node);
        w->SetWindowAreaL( iStream );
        }
    else
        {
        USER_INVARIANT(); // set window area change for unexpected node type. new code needed!
        }
    }

// ---------------------------------------------------------------------------
// DoNodeWindowGroupChainBrokenAfterL
// ---------------------------------------------------------------------------
//
#ifdef ALF_DEBUG_TRACK_DRAWING
void CAlfHierarchyModel::DoNodeDebugTrackL()
    {
    TUint32 nodeId = (TUint32)iStream->ReadUint32L();
    TBool trackingValue = iStream->ReadUint32L();
    CAlfNodeGroup* node = (CAlfNodeGroup*)FindNode( nodeId );
    if ( node )
        {
        node->SetTracking( trackingValue );
        }
    }
#endif
// ---------------------------------------------------------------------------
// FindNode
// ---------------------------------------------------------------------------
//
CAlfNode* CAlfHierarchyModel::FindNode( TUint32 aWindowId )
    {
    if ( iPreviouslySearchedNode && iPreviouslySearchedNode->iId == aWindowId )
        {
        return iPreviouslySearchedNode;
        }
    CNodeHashStruct* nodeHashStruct = iNodeHashArray.Find( aWindowId );
    if (nodeHashStruct)
        {
        iPreviouslySearchedNode = nodeHashStruct->iNode;
        return iPreviouslySearchedNode;
        }
    return NULL;
    }

// ---------------------------------------------------------------------------
// InsertNode
// ---------------------------------------------------------------------------
//
TInt CAlfHierarchyModel::InsertNode( TUint32 aWindowId, CAlfNode* aNode  )
    {
    return iNodeHashArray.Insert( aWindowId, CNodeHashStruct( aNode ));
    }

// ---------------------------------------------------------------------------
// RemoveNode
// ---------------------------------------------------------------------------
//
TBool CAlfHierarchyModel::RemoveNode( TUint32 aWindowId )
	{
	iPreviouslySearchedNode = NULL;
	return iNodeHashArray.Remove( aWindowId );
	}

// ---------------------------------------------------------------------------
// RemoveAllNodes
// ---------------------------------------------------------------------------
//
void CAlfHierarchyModel::RemoveAllNodes( )
    {
    // TODO: Is this useless? wserv will remove its nodes anyway, if screen is destroyed
    }

// ---------------------------------------------------------------------------
// ProcessUnknownNodeDrawingL
// ---------------------------------------------------------------------------
//
void CAlfHierarchyModel::ProcessUnknownNodeDrawingL( RMemReadStream& aStream )
    {
    TInt32 chunkInUse = aStream.ReadInt32L();
    TInt nextFramePos = aStream.ReadInt32L();
       
    // jump to the next frame
    if ( nextFramePos == KFrameOffsetTemplate )
        {
        __ALFLOGSTRING("CAlfHierarchyModel::PostBufferL, Address of the frame has not been initialized!");
        } 
    TUint8 padding = aStream.ReadInt8L();
    aStream.ReadInt8L();
    while( padding--)
        {
        aStream.ReadInt8L();
        }
    TInt currentPos = aStream.Source()->TellL( MStreamBuf::ERead ).Offset();
    aStream.Source()->SeekL( MStreamBuf::ERead, TStreamPos(nextFramePos));
    TSgcCanvasCommands packetState = (TSgcCanvasCommands)aStream.ReadInt8L();
    switch( packetState )
        {
        case EAlfPacketReady: // fall through
        case EAlfPacketNotReady:
        default:
            {
            __ALFLOGSTRING1("CAlfHierarchyModel::PostBufferL endMarker: %d ", packetState);
            break;
            }
        }
    }


#ifdef __WINS__
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CAlfHierarchyModel::Debug_CheckNodeTableItegrity(const TDesC16& aContext)
    {  
    iDebug_CheckNodeTableItegrityCounter = 0;
    
    // Loop through the all items     
    TInt count = 0;
    THashMapIter <TUint32,CNodeHashStruct> ptrHashSetIter(iNodeHashArray);
    for ( ; ; )        
        {        
        const CNodeHashStruct* resNext = ptrHashSetIter.NextValue();        
        if (!resNext)            
            {             
            break;            
            }  
        count++;
        if (resNext->iNode->Type() == MWsWindowTreeNode::EWinTreeNodeClient)
            {
            resNext->iNode->Debug_CheckSiblingOrder(aContext);
            }
        }
    }
#endif



