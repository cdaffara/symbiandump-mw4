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
* Description:   Mapping function handlers
*
*/


#ifndef __ALFHIERARCHYMODEL_H__
#define __ALFHIERARCHYMODEL_H__

#include <s32mem.h>
#include <e32base.h>
#include <e32hashtab.h>
#include <gdi.h>

#define AMT_CONTROL() static_cast<CAlfModuleTestDataControl*>(Dll::Tls())
#include "alfmoduletest.h" 

#include <alfwindowstructs.h>
#include "alfstreamerbridge.h"
#include <uiacceltk/HuiUtil.h>  // USER_INVARIANT

// forward declarations
class CAlfStreamerServer;
class CFbsBitmap;
class TPoint;
class TRect;
class CAlfWindow;
class TRgb;
class CFbsBitGc;
class CAlfNodeRoot;
class CAlfNode;

#include <graphics/wsgraphicdrawer.h>
#include <graphics/wsgraphicscontext.h>

/*
 * Helper class for hash array
 */
NONSHARABLE_CLASS(CNodeHashStruct)
        {
    public:
        CNodeHashStruct( CAlfNode* aNode ) : iNode( aNode ) { };
        
        CAlfNode* iNode;
        };

NONSHARABLE_CLASS(CAlfHierarchyModel) : public CBase, public MAlfBatchObs
    {


public:
    static CAlfHierarchyModel* NewL(CAlfStreamerServer& aServer);
    
    CAlfStreamerServer& Server()
        {
        return iServer;
        }
     
    virtual ~CAlfHierarchyModel();

    void HandleMessageL( const RMessage2& aMessage );
    
    void ShareChunkL( const RMessage2& aMessage );
    
    void CloseChunk( TInt32 aChunkId );
    
    void OpenChunkL( const RMessage2& aMessage );

    // utils
    void DoReleaseSemaphor();
    
    void RequestPacketEndCallback( TInt aOffset );
    
    void RequestFrameEndCallback(TBool aCompositionModified);

    RHashMap<TUint32,CNodeHashStruct> iNodeHashArray;
    
    CAlfNode* FindNode( TUint32 aWindowId );

    TInt InsertNode( TUint32 aWindowId, CAlfNode* aNode );

    TBool RemoveNode( TUint32 aWindowId );
    
    void RemoveAllNodes();
    
private:
    
    CAlfHierarchyModel(CAlfStreamerServer& aServer):iServer(aServer){};

    void AppendScreen( TInt aScreenNumber );
    
    void RemoveScreen( TInt aScreenNumber );
    
    void ConstructL();
    
	void ProcessUnknownNodeDrawingL( RMemReadStream& aStream );
    
protected:

    void ReadEndMarkerL();
        
    
private:
    
    /**
     * ReadRectL
     * Convinience function for reading a TRect from stream
     *
     * @since S60 ?S60_version
     * @param TRect
     */
public:
    void ReadRectL(TRect& aRect, RMemReadStream* aStream );
    
//    static TInt DoSignal( TAny* aObject );
    
private:

    /**
     * ReadRegionL
     * Convinience function for reading a RRegion from stream
     *
     * @since S60 ?S60_version
     * @param Region  
     */
    void ReadRegionL( RMemReadStream* aStream, RRegion& aRegion, TPoint aWindowPos = TPoint(0,0) );
     
    void ExecuteDrawCommandsL( CAlfWindow** aWindow, TAny* aPtr, TInt aSize );
    /**
     * ExecuteCommandsL
     * Internalizes the streamed data.
     *
     * @since S60 ?S60_version
     */
    void ExecuteCommandsL(TBool aCompositionModified = EFalse);
    
    // Commands from window tree oberver at renderstage
    void DoNodeCreatedL();
    void DoNodeReleasedL();
    void DoNodeActivatedL();
    void DoNodeExtentChangedL();
    void DoNodeSiblingOrderChangedL();
    void DoNodeFlagChangedL();    
	void DoNodeFadeCountChangedL();
	void DoNodeTransparentRegionChangedL();
	void DoNodeLayerAddedL();
	void DoNodeLayerExtentChangedL();
	void DoNodeLayerUsesAlphaFlagChangedL();
	void DoNodeMovedToWindowGroupL();
	void DoNodeWindowGroupChainedL();
	void DoNodeWindowGroupChainBrokenAfterL();
	void DoNodeAttributeChangedL();
	void DoNodeSetWindowAreaL();
	void DoNodeDebugTrackL();
	void DoNodeFadeAllChildrenL();
private: // from bridge
    
    virtual void BridgerCallback( TInt aOp, TInt aInt );
    virtual void BridgerCanceled()
        {
        }

    void SetSynchMessage(const RMessage2* aMsg)
        {
        iMessage = *aMsg;
        }
public:
    
    /*
     * Shared chunk handle. Owned by CAlfRenderStage's CAlfSendBuffer
     */

	TUint8* ChunkBase( TUint32 aChunkIndex = 0)
		{
		if ( aChunkIndex == 0 )
		    {
		    return iChunk.Base() +  sizeof( TChunkHeader);
		    }
		else
		    {
		    return iCacheChunks.Find( aChunkIndex )->Base() +  sizeof( TChunkHeader);
		    }
		
		}
    
	   CAlfNodeRoot* iRootNode;
    
    #ifdef __WINS__
	TInt iDebug_CheckNodeTableItegrityCounter;
	void Debug_CheckNodeTableItegrity(const TDesC16& aContext);
    #endif
	
private: // variables

    RChunk iChunk;
    
    RHashMap<TUint32,RChunk> iCacheChunks;
    
    TInt32 iChunkInUse;
        

    CAlfStreamerServer& iServer;

    /*
     * Readstream for the above chunk.
     */
    RMemReadStream* iStream;

    // Todo: debug only
    TInt iUsedCommands[100];

    RRegion iUpdateRegion;
    
    RMessagePtr2 iMessage;
    
    RMessagePtr2 iSpaceNotificationMessage;
    
    TBool iWaitingBridgerToComplete;
    
    RCriticalSection iSema;

    CAlfNode* iPreviouslySearchedNode;
    
    /*
     * For debugging the amount of drawn frames
     */
    TInt iFrame;
    
    TChunkHeader* iChunkHeader;
    
    CFbsBitGc* iGc;
    
    RArray<RMessagePtr2> iPendingAlfWindowDataMessages;
    
    TPoint	iWindowPos;
    
    friend class CAlfNode;
    
    TInt iScreenNumber;
    
    CPeriodic*  iPeriodic;
    
    TInt iSpaceNotificationCounter;
    
    TBool iBatchAlreadyCommited;
    
	TBool iSpaceNotificationMessageOrphean;
	
	TInt iPreviousCallbackOffset;
	
	TInt iSearchNode;
	
#ifdef ALF_DEBUG_TRACK_DRAWING 
	CAlfCommandDebug* iCommandDebugger;
#endif	
    TBool iDidForcedComplete;
    };

#endif
