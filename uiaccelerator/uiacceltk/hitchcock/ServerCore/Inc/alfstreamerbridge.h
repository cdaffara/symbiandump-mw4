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
* Description:   Interthread synchronization object
*
*/


#ifndef __ALF_STREAMER_BRIDGE__
#define __ALF_STREAMER_BRIDGE__

#include <e32std.h>
#include <e32base.h>
#include <fbs.h>
#include "alfbridgecommands.h"
#include "uiacceltk/HuiEnv.h"
#include "alfstreamerconsts.h"


class CAlfStreamerServer;
class CAlfCommandDebug;

const TInt KInitialVariableBufferSize = 2000;

// basic signalling for frame flow control
const TInt KRELEASEWINDOWSERVER = 0;
const TInt KRELEASEDBEFOREQUEUE = 1;


//TODO THESE SHOULD USE MWsWindowTreeNode::TType INSTEAD
enum TAlfWindowNodeType
    {
	EAlfWinTreeNodeClient = 0,	
	EAlfWinTreeNodeRoot = 1,		
	EAlfWinTreeNodeGroup = 2,		
	EAlfWinTreeNodeAnim = 16,		
	EAlfWinTreeNodeSprite = 17,
	EAlfWinTreeNodeTextCursor = 18,
	EAlfWinTreeNodeFloatingSprite = 19 // TODO SOLVE THIS. THIS IS VERY MISLEADING ARRANGAMENT!
	};

enum TAlfWindowNodeFlags
    {
    EAlfWinNodeFlagOpaque = 0x1            
    };

class TAlfWindowAttributes
    {
public:
    TAlfWindowAttributes() :    
        iWindowNodeType(0),
        iPosition(0,0),
        iSize(0,0),
        iOrdinalPosition(0),
        iOpacity(1.f),
        iRotation(0.f),
        iTransitionTime(0),
        iActive(ETrue),
        iSurfaceExtent(0,0,0,0),
        iLayerUsesAlphaFlagEnabled(EFalse),
        iClientHandle(0),
        iClientGroupHandle(0),
        iWindowNodeFlags(0),
        iScreenNumber(0),
        iSecureId(0),
        iParentNodeId(0)
#ifdef ALF_DEBUG_TRACK_DRAWING
        ,iTrackWindow(EFalse)
#endif
            {
            }
    
    TInt    iWindowNodeType; // MWsWindowTreeNode::TType
    TPoint  iPosition;        
    TSize   iSize;
    TInt    iOrdinalPosition;
    TReal32 iOpacity;
    TReal32 iRotation;
    TUint   iTransitionTime;
    TBool   iActive;
    TRect   iSurfaceExtent;
    TBool   iLayerUsesAlphaFlagEnabled;
    TInt    iClientHandle;
    TInt    iClientGroupHandle;
    TInt    iWindowNodeFlags;
    TInt    iScreenNumber;
    TInt    iBitmapHandle;
    TInt32  iSecureId;
    TInt    iParentNodeId;
#ifdef ALF_DEBUG_TRACK_DRAWING	
    TBool   iTrackWindow;
#endif	
    // TODO: We should refactor this to contain unions or 
    // different strcuts for different commands, 
    // every command does not use everything
    
    };

class TAlfCursorDataBufferAttributes
    {
public:
    TInt iColor;
    TInt iFlashInterval;
    TInt iFlags;
    TInt iScreenNumber;
    };


class TAlfWindowCommandBufferAttributes
    {

public:
    enum TAlfBufferStatus 
    {
		EPartComplete,
		ENotComplete,
		ELastPart
    }; 

    TAlfWindowCommandBufferAttributes() :    
        iBufferLength(0),
        iBuffer(NULL),
        iEmptyThisBuffer(ETrue),
        iPartStatus(EPartComplete),
        iWindowNodeFlags(0)        

            {
            }
    TInt    iBufferLength;
    TAny*   iBuffer;
    TBool   iEmptyThisBuffer;
    TAlfBufferStatus   iPartStatus;
    TInt    iWindowNodeFlags;
    TInt    iScreenNumber;
    };

class MAlfStreamerListener
    {
    public:
    virtual void HandleCallback(TInt aStatus) = 0;
    };

class MAlfBatchObs
    {
    public:
    virtual void BridgerCallback( TInt aOp = 0, TInt aInt = 0) = 0;
    virtual void BridgerCanceled() = 0;
    };

class TAlfBridgerData
    {
    private:

    public:
    TAlfBridgerData()
        {
        ResetEntry();
        }
    
    void Set(TAlfDecoderServerBindings aOp,TInt aI1,TInt aI2,TAny* aPtr)
        {
        iOp=aOp;
        iInt1=aI1;
        iInt2=aI2;
        iPtr=aPtr;        
        }
    
    void ResetEntry()
        {
        Set(KUnInitialized,0,0,0);
        }    
    
    TBool IsEmpty()
        {
        return iOp == KUnInitialized;        
        }
    
    TAlfDecoderServerBindings iOp;
    TInt iInt1;
    TInt iInt2;
    TAny* iPtr;
    };


NONSHARABLE_CLASS(CAlfStreamerBridge): public CActive

    {
    public:
   
    CAlfStreamerBridge(MAlfStreamerListener* aObserver);

    static CAlfStreamerBridge* NewL( MAlfStreamerListener* aObserver );

private:
        
    void ConstructL();

    ~CAlfStreamerBridge();
    
public:
        
    /**
     * SetBatchObserver
     *
     * @since S60 5.0
     * @param aWindow
     */
    void SetBatchObserver(MAlfBatchObs* aBatchObserver);
    
    /**
     * Activate
     *
     * @since S60 5.0
     * @param aWindow
     */    
    void Activate();
         
    /**
     * Trigger
     *
     * @since S60 5.0
     */
    TInt Trigger(TInt aStatus);
  
    /**
     * AddData
     *
     * @since S60 5.0
     * @param aWindow
     */
    TInt AddData( TAlfDecoderServerBindings aOp,TInt aI1 = 0,TInt aI2 = 0,TAny* aPtr =0);
    
    /**
     * GetData
     *
     * @since S60 5.0
     * @param aWindow
     */
    TAlfBridgerData GetData(TInt aIndex);
    
    /**
     * StartNewBlock
     *
     * @since S60 5.0
     */
    void StartNewBlock(TBool aCompositionModified);

    /**
     * StartNewBlock
     *
     * @since S60 5.0
     * @param aLastReadOffset
     */
    void RequestCommandReadNotification( 
            TInt aLastReadOffset, 
            TAlfDecoderServerBindings aCommand = EAlfRequestCommandReadNotification );

    // AppendDataL and GetVarDataL are for keeping the memory allocations inside one thread.    
    IMPORT_C const TAny* AppendVarDataL( TInt aSize, TInt& aIndex );

    // Effect data is not synchronized to window server data
    // It can be safely applied only when refresh is not active in hitchcock scene    
    IMPORT_C const TAny* AppendEffectsDataL( TInt aSize, TInt& aIndex );
    const TAny* GetEffectsDataL( TInt aIndex );

    
    /**
     * GetVarDataL
     *
     * @since S60 5.0
     * @param aIndex Offset to the data struct  (relative to iDataBuf)
     * @return Pointer to the data struct 
     */
    const TAny* GetVarDataL( TInt aIndex );
    
    /* deprecated */
    IMPORT_C TUid FindAppUidForWgId(TInt aWgId);
    IMPORT_C TInt FindWgForAppUid(TUid aAppUid);

    void SetObserver(MAlfStreamerListener* aObserver)
        {
        iObserver = aObserver;
        }        

public: // from CActive
        
    void RunL();
    void DoCancel();
    
private:

    CAlfStreamerServer* iStreamerServer;        
    MAlfStreamerListener* iObserver;
    MAlfBatchObs* iBatchObserver;
    TThreadId iThread;
    
    RArray<TAlfBridgerData> iMessages;
    RArray<TInt> iQueue;
    
    RCriticalSection iQueueSema;
    
    RBuf8 iDataBuf;
    volatile TInt iItemsInBuffer;
    volatile TBool iVarDataAddedButNotPosted;

#ifdef ALF_DEBUG_TRACK_DRAWING    
    CAlfCommandDebug* iCommandDebugger;
#endif

public:
    volatile TAlfNativeWindowData iAlfWindowData;
    volatile TInt iActiveEffectCount;
    };


#endif // __ALF_STREAMER_BRIDGE__
