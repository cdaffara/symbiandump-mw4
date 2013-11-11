/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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

#include <alfdecoderserverclient.h>
#include <alfstreamerconsts.h>

#include <uiacceltk/HuiUtil.h>
#include <coemain.h>
#include <alf/alfcompositionclient.h>
#include "graphics/surfacemanager.h"
#include "graphics/surface.h"
#include "graphics/surfaceupdateclient.h"
#include "graphics/suerror.h" // KAllScreens

#include "graphics/surfaceconfiguration.h" // TSurfaceConfiguration

#include "alflogger.h"

// ---------------------------------------------------------------------------
// CAlfCompositionClientBaseData::NewL
// ---------------------------------------------------------------------------
//
CAlfCompositionClientBase::CAlfCompositionClientBaseData* 
    CAlfCompositionClientBase::CAlfCompositionClientBaseData::NewL(
            TInt aBufferSize, 
            RAlfBridgerClient* aClient)
    {
    CAlfCompositionClientBaseData* me = new (ELeave) CAlfCompositionClientBaseData();
    CleanupStack::PushL(me);
    me->ConstructL(aBufferSize, aClient);
    CleanupStack::Pop();
    return me;
    }

// ---------------------------------------------------------------------------
// CAlfCompositionClientBaseData::ConstructL
// ---------------------------------------------------------------------------
//
void CAlfCompositionClientBase::CAlfCompositionClientBaseData::ConstructL(TInt /*aBufferSize*/, RAlfBridgerClient* aClient)
    {
    if (!aClient)
        {
        iClient = new (ELeave) RAlfBridgerClient();     
        iOwnsClient = ETrue; 
        User::LeaveIfError(iClient->Connect());
        }
    else
        {
        iClient = aClient;
        }
    }

// ---------------------------------------------------------------------------
// ~CAlfCompositionClientBaseData
// ---------------------------------------------------------------------------
//
CAlfCompositionClientBase::CAlfCompositionClientBaseData::~CAlfCompositionClientBaseData()
    {
    if (iOwnsClient && iClient)
        {
        iClient->Close();
        delete iClient;    
        }
    if (iDeleted)
        {
        *iDeleted = ETrue;
        }    
    }    
// ---------------------------------------------------------------------------
// RequestEventL
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfCompositionClientBase::RequestEventL(TInt aBufferSize, RAlfBridgerClient* aClient)
    {
    if (!IsActive())
        {    
        if (!iData)
            {    
            iData = CAlfCompositionClientBaseData::NewL(aBufferSize, aClient);
            }
        SetActive();
        iData->iClient->SendAsynchronous(KAlfCompOpRequestEvent,TIpcArgs(&iData->iBuffer), iStatus);
        }    
    }            

// ---------------------------------------------------------------------------
// SendEvent
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CAlfCompositionClientBase::SendEvent(TInt aOp, const TAny* aEventData, TInt aEventSize)
    {
    // Todo, should support batching / asynch    
    if (!iData)
        {    
        TRAPD(err, iData = CAlfCompositionClientBaseData::NewL(aEventSize, 0)) // basically zero buffer would do 
        if (err)
            {
            return err;
            }    
        }

    TPtrC8 ptr(reinterpret_cast<const TUint8*>(aEventData), aEventSize);

    return iData->iClient->SendSynch(aOp, ptr); 
    }            

// ---------------------------------------------------------------------------
// HandleEventL
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfCompositionClientBase::HandleEventL(TInt /*aEventType*/, TAny* /*aEventData*/)
    {
    }

// ---------------------------------------------------------------------------
// NullBoolPtr
// ---------------------------------------------------------------------------
//
void NullBoolPtr(TAny* ptr)
    {
    TBool** boolptr = static_cast<TBool**>(ptr);
    *boolptr = 0;    
    }
 
// ---------------------------------------------------------------------------
// RunL
// ---------------------------------------------------------------------------
//
void CAlfCompositionClientBase::RunL()
    {
    TInt status = iStatus.Int();
    User::LeaveIfError(status);
    TBool deleted = EFalse;
    iData->iDeleted = &deleted; 
    CleanupStack::PushL(TCleanupItem(NullBoolPtr, &iData->iDeleted));
    HandleEventL(status, (void*)iData->iBuffer.Ptr());
    CleanupStack::PopAndDestroy();
    if (!deleted) // just small precaution if client deletes itself on callback
        {
        RequestEventL(sizeof(TInt));
        }
    } 

// ---------------------------------------------------------------------------
// DoCancel
// ---------------------------------------------------------------------------
//    
void CAlfCompositionClientBase::DoCancel()
    {
    if (iData)
        {
        iData->iClient->SendSynch(KAlfCompOpCancelEventRequest, TIpcArgs(Handle()) );
        }
    }    

// ---------------------------------------------------------------------------
// RunError
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CAlfCompositionClientBase::RunError(TInt aError)
    {
    RDebug::Print(_L("CAlfCompositionClientBase::RunError( %d )"),aError);
    if ( aError == KErrServerTerminated )
        {
        RDebug::Print(_L("CAlfCompositionClientBase::RunError - ALF server has died. Everything is lost. Halting."));
        USER_INVARIANT();
        }
    return KErrNone;
    }    

// ---------------------------------------------------------------------------
// ~CAlfCompositionClientBase
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfCompositionClientBase::~CAlfCompositionClientBase()
    {
    Cancel();
    delete iData;
    iData = NULL;
    }

// ---------------------------------------------------------------------------
// SetHandleL
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfCompositionClientBase::SetHandleL(TInt aHandle)
    {
    User::LeaveIfError(aHandle);
    if (!iData)
        {    
        iData = CAlfCompositionClientBaseData::NewL(sizeof(TInt),0);// basically zero buffer would do 
        }
    iData->iHandle = aHandle;
    }

// ---------------------------------------------------------------------------
// Handle
// ---------------------------------------------------------------------------
//
TInt CAlfCompositionClientBase::Handle() const
    {
    return iData?iData->iHandle:0;
    }

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfCompositionCntrlClient* CAlfCompositionCntrlClient::NewL(RAlfBridgerClient* aClient, MAlfCompositionController* aController)
    {
    CAlfCompositionCntrlClient* me = new (ELeave) CAlfCompositionCntrlClient(aController);    
    me->RequestEventL(60, aClient); // magic, extent with options
    me->SendEvent(KAlfCompositionWServReady,0,0);
    return me;
    }
     
// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfCompositionCntrlClient::~CAlfCompositionCntrlClient()
    {
    iHostBindingsHash.Close();
    iHostPermittedOpsHash.Close();
    }

// ---------------------------------------------------------------------------
// HandleEventL
// ---------------------------------------------------------------------------
//
void CAlfCompositionCntrlClient::HandleEventL(TInt aEventType, TAny* aEventData)
    {
    __ASSERT_DEBUG(iController, USER_INVARIANT());
    
    if (aEventType == KAlfCompositionLowOnGraphicsMemory && iController)
        {
        iController->LowOnGraphicsMemory();    
        return;
        }
    
    if (aEventType == KAlfCompositionGoodOnGraphicsMemory && iController)
        {
        iController->GraphicsMemoryGood();
        return;
        }    
    
    if (aEventType == KAlfCompositionLayoutSwitchComplete && iController)
        {
        iController->AlfBridgeCallback(KAlfCompositionLayoutSwitchComplete, NULL);
        }
    
    TInt* ptr = static_cast<TInt*>(aEventData);
    TInt target;
    
    // find out if target is a host (is binded to some other target)
    TInt bindedTarget = 0;
    TInt* tempbind = iHostBindingsHash.Find(*ptr);
    if(tempbind)
        bindedTarget = *tempbind;
    TInt32 permittedOps = 0;
    if(bindedTarget)
        {
        target = bindedTarget;
        TInt32* tempOps = iHostPermittedOpsHash.Find(*ptr);
        if(tempOps)
            permittedOps = *tempOps; 
        }
    else
        {
        target = *ptr;
        }
        
    // SetTargetL is always sent before the actual command.
    iController->SetTargetL(target); // When this is composition
    
    switch (aEventType)
        {
        case KAlfCompOpCreateToken:
            {
            break;
            }
        case KAlfCompOpBindSourceToToken:
            {
#ifdef _ALF_LOGGING
            RDebug::Print(_L("ptr0: %d, Target %d, Flags %d, combinedtarget"), ptr[0], ptr[1], ptr[2]);
#endif // #ifdef _ALF_LOGGING
            if( ptr[1] != 0) // add binding information for new host to given target with permitted operations
                {
                iHostBindingsHash.Insert(*ptr, ptr[1]);
                iHostPermittedOpsHash.Insert(*ptr, ptr[2]);
                }
            else // remove old host bindings when the host is being deleted.
                {
                iHostBindingsHash.Remove(*ptr);
                iHostPermittedOpsHash.Remove(*ptr);
                }
            break;
            }
        case KAlfCompOpCreateSource:    
            {
            iController->CreateTargetL(target, ptr[2], ptr[3]); 
            break;
            }    
        case KAlfCompOpEnableAlpha:
            {
            if ( !bindedTarget || permittedOps & CAlfCompositionHost::EAlfAllowChangeAlpha)
                {
                iController->EnableAlpha(ptr[1]);
                }
            break;
            }    
        case KAlfCompOpSetOpacity:
            {
            if ( !bindedTarget || permittedOps & CAlfCompositionHost::EAlfAllowChangeOpacity)
                {
                TReal32* opacity = /*static_cast<*/(TReal32*)(++ptr);
                iController->SetOpacity(*opacity);
                }
            break;
            }    
        case KAlfCompOpSetRotation:
            {
            if ( !bindedTarget || permittedOps & CAlfCompositionHost::EAlfAllowChangeRotation)
                {      
                iController->SetRotation(ptr[1]);
                }
            break;
            }     
        case KAlfCompOpSetZOrder:
            {
            TInt newToken = 0;
            if( ptr[3] != KErrNotFound)
                {
                newToken = iController->SetOrder(target,ptr[1],ptr[2],ETrue); //  top, below, combineTargets             
                }
            else
                {
                newToken = iController->SetOrder(target,ptr[1],ptr[2],EFalse); //  top, below, combineTargets             
                }
            TInt array[] = {ptr[0], newToken, ptr[3]}; // caller session, newToken, secretKey
            SendEvent(KAlfCompositionTargetCreated, array, sizeof(array));
            break;
            }    

        case KAlfCompOpSetExtent:
            {
            if ( !bindedTarget || permittedOps & CAlfCompositionHost::EAlfAllowChangeExtent)
                {
                TRect rect(ptr[1],ptr[2],ptr[3],ptr[4]);
                TSurfaceId surfaceId;
                surfaceId.iInternal[0] = ptr[6];
                surfaceId.iInternal[1] = ptr[7];
                surfaceId.iInternal[2] = ptr[8];
                surfaceId.iInternal[3] = ptr[9];
                iController->SetExtentSurfaceId(surfaceId);
                iController->SetExtent(rect, ptr[5]); // rect, screen
                }
            break;
            }
        case KAlfCompOpSetSRect:
            {
            if ( !bindedTarget || permittedOps & CAlfCompositionHost::EAlfAllowChangeExtent)
                {
                TRect rect(ptr[1],ptr[2],ptr[3],ptr[4]);
                iController->SetSourceRect(rect);
                }
            break;
            }
        case KAlfComOpSetBackgroundAnim:
            {
            if ( !bindedTarget || permittedOps & CAlfCompositionHost::EAlfAllowChangeBackgroundAnim)
                {
                iController->SetIsBackgroundAnim(ptr[1]);
                }
            break;
            }
        case KAlfCompOpSessionClosed:
            {
            iController->DeleteTarget(target);
            break;
            }
        
        case KAlfCompositionTargetVisible:
            {
            iController->HideTarget(target, EFalse);
            break;
            }    

        case KAlfCompositionTargetHidden:
            {
            iController->HideTarget(target, ETrue);
            break;
            }    
        default:
            break;
        }
    }    

// ---------------------------------------------------------------------------
// RunError
// ---------------------------------------------------------------------------
//
TInt CAlfCompositionCntrlClient::RunError(TInt aError)
    {
    CAlfCompositionClientBase::RunError(aError);
    // reactivate
    TRAPD(err, RequestEventL(60)); // magic, extent with options
    return err;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
NONSHARABLE_CLASS(CAlfCompositionSource::CAlfCompositionSourceData):public CBase
    {
    public:
    static CAlfCompositionSourceData* NewL()
        {
        CAlfCompositionSourceData* me = new (ELeave) CAlfCompositionSourceData();
        CleanupStack::PushL(me);
        // jada
        CleanupStack::Pop();
        return me;
        }

    ~CAlfCompositionSourceData()
        {
        if (iDeleted)
            {
            *iDeleted = ETrue;
            }    
        iObservers.Reset();
        }    

    TBool* iDeleted;
    TSurfaceId iSurfaceId;
    RPointerArray<MAlfCompositionObserver> iObservers;
    };

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfCompositionSource* CAlfCompositionSource::NewL(RWindow& aClientWindow)
    {
    CAlfCompositionSource* me = new (ELeave) CAlfCompositionSource();
    CleanupStack::PushL(me);
    me->ConstructL(aClientWindow);
    CleanupStack::Pop(me);
    return me;
    }

// ---------------------------------------------------------------------------
// ~CAlfCompositionSource
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfCompositionSource::~CAlfCompositionSource()
    {
    delete iData;
    iData = NULL;
    }
// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CAlfCompositionSource::ConstructL(TInt aWsHandle, TInt aGroupHandle, TInt aScreenNumber)
    {
    iData = CAlfCompositionSourceData::NewL();
#ifdef _ALF_LOGGING
    RDebug::Print(_L("CAlfCompositionClientBase::ConstructL - %d"), iData );
#endif // #ifdef _ALF_LOGGING

    User::LeaveIfError( SendEvent(KAlfCompositionSourceScreenNumber, &aScreenNumber, sizeof(TInt)));
    
    TInt array[] = { 0, aWsHandle, aGroupHandle}; 
    TInt handle = SendEvent(KAlfCompOpCreateSource, array, sizeof(array));   
    SetHandleL(handle);
    }

void CAlfCompositionSource::ConstructL(RWindow& aClientWindow)
    {
    ConstructL(aClientWindow.ClientHandle(), aClientWindow.WindowGroupId(), aClientWindow.ScreenNumber());
    TSurfaceConfiguration surfaceConfigs;
    User::LeaveIfError(aClientWindow.GetBackgroundSurface(surfaceConfigs));
    surfaceConfigs.GetSurfaceId(iData->iSurfaceId);
    if(iData->iSurfaceId.IsNull())
        {
        __ALFLOGSTRING( "CAlfCompositionSource::NewL - Window does not have background surface set - Leaving");
        User::Leave(KErrNotFound);
        }   
    }

// ---------------------------------------------------------------------------
// CreatePermissionToken
// ---------------------------------------------------------------------------
//  
EXPORT_C TInt CAlfCompositionSource::CreatePermissionToken(TInt aKey, TInt aPermissionFlags)
    {
    TInt array[] = {aKey, aPermissionFlags};    
    return SendEvent(KAlfCompOpCreateToken, array, sizeof(array));
    }    

// ---------------------------------------------------------------------------
// EnableAlpha
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CAlfCompositionSource::EnableAlpha(TBool aEnable)
    {
    return SendEvent(KAlfCompOpEnableAlpha, &aEnable, sizeof(TInt));
    }

// ---------------------------------------------------------------------------
// SetOpacity
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CAlfCompositionSource::SetOpacity(TReal32 aOpacity) __SOFTFP
    {
    return SendEvent(KAlfCompOpSetOpacity, &aOpacity, sizeof(TReal32));
    }

// ---------------------------------------------------------------------------
// SetRotation
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CAlfCompositionSource::SetRotation(TInt aRotationInDegrees)
    {
    return SendEvent(KAlfCompOpSetRotation, &aRotationInDegrees, sizeof(TInt));
    }

// ---------------------------------------------------------------------------
// SetZOrder
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CAlfCompositionSource::SetZOrder(const CAlfCompositionClientBase& aNode, TBool aAbove, TInt aKey)
    {
    TInt array[] = {aNode.Handle(), aAbove, aKey};
    return SendEvent(KAlfCompOpSetZOrder, array, sizeof(array));
    }

// ---------------------------------------------------------------------------
// SetExtent
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CAlfCompositionSource::SetExtent(const TRect& aRect, TInt aScreen)
    {
    TInt array[] = {aRect.iTl.iX,aRect.iTl.iY,aRect.iBr.iX,aRect.iBr.iY, aScreen,
            iData->iSurfaceId.iInternal[0], iData->iSurfaceId.iInternal[1], iData->iSurfaceId.iInternal[2],
            iData->iSurfaceId.iInternal[3]};
    return SendEvent(KAlfCompOpSetExtent, array, sizeof(array));
    }

// ---------------------------------------------------------------------------
// SetSourceRect
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CAlfCompositionSource::SetSourceRect(const TRect& aRect)
    {
    TInt array[] = {aRect.iTl.iX,aRect.iTl.iY,aRect.iBr.iX,aRect.iBr.iY};
    return SendEvent(KAlfCompOpSetSRect, array, sizeof(array));
    }

// ---------------------------------------------------------------------------
// AddCompositionObserverL
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfCompositionSource::AddCompositionObserverL(MAlfCompositionObserver& aObserver, TInt aScreenNumber)
    {
    if (iData->iObservers.Count() == 1 || aScreenNumber != 0)    
        { // TODO: support only single observer for now 
          // because we can't currently handle deletion of an observer
          // during handle event properly (if there were more than one observers present)
        User::Leave(KErrNotSupported);    
        }
        
    TInt index = iData->iObservers.Find(&aObserver);
    if (index == KErrNotFound)
        {
        iData->iObservers.AppendL(&aObserver);
        }
    RequestEventL(sizeof(TInt)); // enable notifications if not active yet
    }

// ---------------------------------------------------------------------------
// RemoveObserver
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfCompositionSource::RemoveObserver(MAlfCompositionObserver& aObserver)
    {
    TInt index = iData->iObservers.Find(&aObserver);
    if (index != KErrNotFound)
        {
        iData->iObservers.Remove(index);
        }
    }

// ---------------------------------------------------------------------------
// SetIsBackgroundAnim
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfCompositionSource::SetIsBackgroundAnim(TBool aIsBg)
    {    
    SendEvent(KAlfComOpSetBackgroundAnim, &aIsBg, sizeof(TBool));
    }

// ---------------------------------------------------------------------------
// EnableKeyboard
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfCompositionSource::EnableKeyboard(TBool aEnable, TInt aScreen)
    {
    TInt array[] = {aEnable, aScreen};
    SendEvent(KAlfCompOpEnableKb, array, sizeof(array));
    }

// ---------------------------------------------------------------------------
// HandleEventL
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfCompositionSource::HandleEventL(TInt aEventType, TAny* aEventData)
    {
    switch (aEventType)
        {        
        case KAlfCompositionFrameReady:
            {
            for (TInt i = 0; i < iData->iObservers.Count(); i++)
                {
                iData->iObservers[i]->FrameReady(*static_cast<TInt*>(aEventData)); // todo multiple screens    
                }
            break;
            }    
        case KAlfCompositionLowOnGraphicsMemory:
            {
            for (TInt i = 0; i < iData->iObservers.Count(); i++)
                {
                iData->iObservers[i]->RunningLowOnGraphicsMemory(); // todo multiple screens    
                }
            break;
            }    
        case KAlfCompositionTargetHidden:
            {
            for (TInt i = 0; i < iData->iObservers.Count(); i++)
                {
                iData->iObservers[i]->CompositionTargetHidden(); // todo multiple screens    
                }
            break;
            }    
        case KAlfCompositionGoodOnGraphicsMemory:
            {
            TInt count = iData->iObservers.Count();
            for (TInt i = 0; i < count; i++)
                {
                iData->iObservers[i]->GraphicsMemoryGood();
                }
            break;
            }     
        case KAlfCompositionTargetVisible:
            {
            for (TInt i = 0; i < iData->iObservers.Count(); i++)
                {
                iData->iObservers[i]->CompositionTargetVisible(); // todo multiple screens    
                }
            break;
            }    

        default:
            break;
        }
    }

// ---------------------------------------------------------------------------
// CAlfCompositionHost::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfCompositionHost* CAlfCompositionHost::NewL(TInt aToken, TInt aKey)
    {
    CAlfCompositionHost* me = new (ELeave) CAlfCompositionHost();
    CleanupStack::PushL(me);        
    me->ConstructL(aToken, aKey);
    CleanupStack::Pop();
    return me;
    }

// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CAlfCompositionHost::ConstructL(TInt aToken, TInt aKey)
    {
    iData = CAlfCompositionSourceData::NewL();
#ifdef _ALF_LOGGING
    RDebug::Print(_L("CAlfCompositionClientBase::ConstructL - %d"), iData );
#endif // #ifdef _ALF_LOGGING

    TInt array[] = { 0, aToken, aKey }; 
    TInt result  = SendEvent(KAlfCompOpBindSourceToToken, array, sizeof(array));   
    User::LeaveIfError(result);
    SetHandleL(result);
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
NONSHARABLE_CLASS( CSurfaceUpdateCallback ) : public CActive
    { 
    public:
 
    public:
        CSurfaceUpdateCallback( CAlfCompositionPixelSource& aPixelSource, TInt aBufferNumber,
                                TInt aPriority): CActive(aPriority), iPixelSource(aPixelSource),
                                iBufferNumber(aBufferNumber) { CActiveScheduler::Add(this); };
        ~CSurfaceUpdateCallback() { Cancel(); };
    public:

        void SetActive() {iStatus = KRequestPending;  CActive::SetActive();};

        void RunL() 
            {
            if( iStatus != KErrCancel )
                {
                SetActive();
                if( !iPixelSource.DrawFrameL(iStatus, iBufferNumber) )
                    {
                    __ALFLOGSTRING( "CSurfaceUpdateCallBack::RunL - DrawFrameL returned EFalse -> Pausing");
                    TRequestStatus* status  = &iStatus;
                    User::RequestComplete(status, KErrCancel);
                    Cancel();
                    }
                }
            };
        void DoCancel() 
            {
            };

    private: // Data
        CAlfCompositionPixelSource& iPixelSource;
        TInt iBufferNumber;
    };

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
NONSHARABLE_CLASS(CAlfCompositionPixelSource::CAlfCompositionPixelSourceData):public CBase
    {
    public:
        
        enum TPixelSourceStatus
            {
            EActive = 0, // drawing loop running
            EPaused = 1, // drawing paused but resources are still reserved
            ESuspended = 2 // drawing suspended and surface related resources freed until activated again
            };

        static CAlfCompositionPixelSourceData* NewL(MAlfBufferProvider& aProvider)
        {
        CAlfCompositionPixelSourceData* me = new (ELeave) CAlfCompositionPixelSourceData(aProvider);
        return me;
        }

    
    CAlfCompositionPixelSourceData(MAlfBufferProvider& aProvider) : 
        iProvider( aProvider), iSourceStatus(ESuspended) {iSurfaceId.CreateNullId();}
    
    ~CAlfCompositionPixelSourceData()
        {
        if(iWindow && iSourceStatus != ESuspended)
            {
            iWindow->RemoveBackgroundSurface(ETrue);
            }
        
        if(iSourceStatus != ESuspended)
            {
            iSurfaceUpdateSession.CancelAllUpdateNotifications();
            }

        iSurfaceUpdateSession.Close();

        if(iSurfaceManager)
            {
            iSurfaceManager->CloseSurface(iSurfaceId);
            iSurfaceManager->Close();
            delete iSurfaceManager;
            }
        
        if(iSurfaceChunk)
            {
            iSurfaceChunk->Close();
            delete iSurfaceChunk;
            }

        if(iSurfaceUpdateWaiter && iSurfaceUpdateWaiter->IsActive())
            {
            iSurfaceUpdateWaiter->Cancel();
            }
        delete iSurfaceUpdateWaiter;
        
        }
    MAlfBufferProvider& iProvider;

    TPixelSourceStatus iSourceStatus;
    
    RSurfaceUpdateSession iSurfaceUpdateSession;
    TSurfaceId iSurfaceId;
    RSurfaceManager* iSurfaceManager;
    RChunk* iSurfaceChunk;

    // Not owned
    RWindow* iWindow;
    
    TUint8* iSurfaceBuffer;

    CSurfaceUpdateCallback* iSurfaceUpdateWaiter;
    
    TInt iCurrentBuffer;
    
    TRect iSurfaceRect;
    
    TInt iWaiterAoPriority;
    
    TTimeStamp iTimeStamp; // not used but is required parameter for NotifyWhenDisplayed
    };



// ---------------------------------------------------------------------------
// CAlfCompositionPixelSource::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfCompositionPixelSource* CAlfCompositionPixelSource::NewL(MAlfBufferProvider& aProvider, RWindow* aWindow)
    {
    CAlfCompositionPixelSource* me = new (ELeave) CAlfCompositionPixelSource();
    CleanupStack::PushL(me);
    me->ConstructL(aProvider, CActive::EPriorityIdle, aWindow);
    CleanupStack::Pop(me);
    return me;    
    }

// ---------------------------------------------------------------------------
// CAlfCompositionPixelSource::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfCompositionPixelSource* CAlfCompositionPixelSource::NewL(MAlfBufferProvider& aProvider, TInt aPriority, RWindow* aWindow)
    {
    CAlfCompositionPixelSource* me = new (ELeave) CAlfCompositionPixelSource();
    CleanupStack::PushL(me);
    me->ConstructL(aProvider, aPriority, aWindow);
    CleanupStack::Pop(me);
    return me;    
    }


// ---------------------------------------------------------------------------
// CAlfCompositionPixelSource::ActivateL
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfCompositionPixelSource::ActivateL()
    {
    if( !iData->iWindow && iData->iSurfaceId.IsNull() )
        {
        User::Leave(KErrNotReady);
        }
    
    if( iData->iSourceStatus == CAlfCompositionPixelSourceData::ESuspended )
        {
        MAlfBufferProvider::TBufferCreationAttributes& creationAttribs = iData->iProvider.BufferAttributes();
        
        iData->iSurfaceRect = TRect(TPoint(0,0), TSize(creationAttribs.iWidth, creationAttribs.iHeight));

        ConstructSurfaceL(creationAttribs);

        iData->iWindow->SetBackgroundSurface(iData->iSurfaceId);

        TInt array[] = { 0, iData->iWindow->ClientHandle(), iData->iWindow->WindowGroupId() }; 
        TInt handle = SendEvent(KAlfCompOpCreateSource, array, sizeof(array));
        CAlfCompositionClientBase::SetHandleL( handle );

        User::LeaveIfError( iData->iSurfaceUpdateSession.Connect() );       
        }
  
    if( iData->iSourceStatus != CAlfCompositionPixelSourceData::EActive )
        {
        if( !iData->iSurfaceUpdateWaiter )
            {
            iData->iSurfaceUpdateWaiter = new (ELeave) CSurfaceUpdateCallback( *this, 0, iData->iWaiterAoPriority );
            }

        iData->iSurfaceUpdateWaiter->SetActive();
        TRequestStatus* status = &iData->iSurfaceUpdateWaiter->iStatus;
        User::RequestComplete(status, KErrNone);
        
        iData->iProvider.OnActivation();
        iData->iSourceStatus = CAlfCompositionPixelSourceData::EActive;
        }
   
    // do nothing if content was already active
    }

// ---------------------------------------------------------------------------
// CAlfCompositionPixelSource::ActivateSyncL
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfCompositionPixelSource::ActivateSyncL()
    {
    if( !iData->iWindow && iData->iSurfaceId.IsNull() )
        {
        User::Leave(KErrNotReady);
        }
    TBool pause = EFalse;
    if( iData->iSourceStatus == CAlfCompositionPixelSourceData::ESuspended )
        {
        MAlfBufferProvider::TBufferCreationAttributes& creationAttribs = iData->iProvider.BufferAttributes();
        
        iData->iSurfaceRect = TRect(TPoint(0,0), TSize(creationAttribs.iWidth, creationAttribs.iHeight));

        ConstructSurfaceL(creationAttribs);

        User::LeaveIfError( iData->iSurfaceUpdateSession.Connect() );     
        
        // update surface buffer before setting surface as background surface
        if( !iData->iSurfaceUpdateWaiter )
            {
            iData->iSurfaceUpdateWaiter = new (ELeave) CSurfaceUpdateCallback( *this, 0, iData->iWaiterAoPriority );
            }

        iData->iSurfaceUpdateWaiter->SetActive();
        pause = !DrawFrameL(iData->iSurfaceUpdateWaiter->iStatus ,0);
        if(pause && iData->iSurfaceUpdateWaiter->IsActive())
            {
            TRequestStatus* status  = &iData->iSurfaceUpdateWaiter->iStatus;
            User::RequestComplete(status, KErrCancel);
            iData->iSurfaceUpdateWaiter->Cancel();
            }
        
        iData->iWindow->SetBackgroundSurface(iData->iSurfaceId);

        TInt array[] = { 0, iData->iWindow->ClientHandle(), iData->iWindow->WindowGroupId() }; 
        TInt handle = SendEvent(KAlfCompOpCreateSource, array, sizeof(array));
        CAlfCompositionClientBase::SetHandleL( handle );
        }
  
    if( iData->iSourceStatus != CAlfCompositionPixelSourceData::EActive && !pause )
        {

        if( !iData->iSurfaceUpdateWaiter )
            {
            iData->iSurfaceUpdateWaiter = new (ELeave) CSurfaceUpdateCallback( *this, 0, iData->iWaiterAoPriority );
            }

        if(!iData->iSurfaceUpdateWaiter->IsActive())
            {
            iData->iSurfaceUpdateWaiter->SetActive();
            TRequestStatus* status = &iData->iSurfaceUpdateWaiter->iStatus;
            User::RequestComplete(status, KErrNone);
            }
        iData->iProvider.OnActivation();
        iData->iSourceStatus = CAlfCompositionPixelSourceData::EActive;
        }
   
    // do nothing if content was already active and running    
    }

// --------------------------------------------------------------------------- 
// CAlfCompositionPixelSource::Suspend
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfCompositionPixelSource::Suspend()
    {
    if(iData->iSourceStatus != CAlfCompositionPixelSourceData::ESuspended)
        {
        iData->iProvider.ContextAboutToSuspend();
    
        TInt error = SendEvent(KAlfCompOpSessionClosed,0,0);
        
        // todo: what if error != KErrNone
        
        FreeSurface();
        
        if(iData->iSurfaceUpdateWaiter && iData->iSurfaceUpdateWaiter->IsActive())
            {
            iData->iSurfaceUpdateWaiter->Cancel();
            }
        
            
        iData->iSourceStatus = CAlfCompositionPixelSourceData::ESuspended;
        }
    }

// ---------------------------------------------------------------------------
// CAlfCompositionPixelSource::CAlfCompositionPixelSource
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfCompositionPixelSource::~CAlfCompositionPixelSource()
    {
    Suspend();
    delete iData;
    iData = NULL;
    }

// ---------------------------------------------------------------------------
// CAlfCompositionPixelSource::SetExtent
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CAlfCompositionPixelSource::SetExtent(const TRect& aRect, TInt aScreen)
    {
    TInt error = KErrNone;
    if(iData->iSurfaceId.IsNull() && !iData->iWindow) 
        {
        //create surface as client did not supply window
        MAlfBufferProvider::TBufferCreationAttributes& creationAttribs = iData->iProvider.BufferAttributes();
        
        iData->iSurfaceRect = TRect(TPoint(0,0), TSize(creationAttribs.iWidth, creationAttribs.iHeight));

        TRAP(error, ConstructSurfaceL(creationAttribs) );

        if( error == KErrNone )
            {
            error = iData->iSurfaceUpdateSession.Connect();       
            iData->iSourceStatus = CAlfCompositionPixelSourceData::EPaused;
            }
        }
    
    if( error == KErrNone )
        {
        TInt array[] = {aRect.iTl.iX,aRect.iTl.iY,aRect.iBr.iX,aRect.iBr.iY, aScreen,
                iData->iSurfaceId.iInternal[0], iData->iSurfaceId.iInternal[1], iData->iSurfaceId.iInternal[2],
                iData->iSurfaceId.iInternal[3]};
        error = SendEvent(KAlfCompOpSetExtent, array, sizeof(array));
        }
    return error;
    }

// ---------------------------------------------------------------------------
// CAlfCompositionPixelSource::ConstructL
// ---------------------------------------------------------------------------
//
void CAlfCompositionPixelSource::ConstructL(MAlfBufferProvider& aProvider, TInt aPriority, RWindow* aWindow)
    {
    iData = CAlfCompositionPixelSourceData::NewL( aProvider );
    iData->iWaiterAoPriority = aPriority;

    TInt screenNumber = KErrNotFound;
    if( aWindow )
        {
        iData->iWindow = aWindow;
        screenNumber = aWindow->ScreenNumber();
        }
    else // let's just initialize baseclass and wait for client to call SetExtent
        {
        iData->iWindow = NULL;
        }
    // construct the base class
    CAlfCompositionSource::ConstructL(0, 0, screenNumber);
    }

// ---------------------------------------------------------------------------
// CAlfCompositionPixelSource::ConstructSurfaceL
// ---------------------------------------------------------------------------
//
void CAlfCompositionPixelSource::ConstructSurfaceL(MAlfBufferProvider::TBufferCreationAttributes& aCreationAttributes)
    {
    TSize size(aCreationAttributes.iWidth, aCreationAttributes.iHeight);

    TUidPixelFormat surfaceFormat = EUidPixelFormatUnknown;
    switch (aCreationAttributes.iFormat)
        {
        case MAlfBufferProvider::ESourceFormatRGB_565:
            surfaceFormat = EUidPixelFormatRGB_565;
            break;
        case MAlfBufferProvider::ESourceFormatXRGB_8888:
            surfaceFormat = EUidPixelFormatXRGB_8888;
            break;
        case MAlfBufferProvider::ESourceFormatARGB_8888:
            surfaceFormat = EUidPixelFormatARGB_8888;
            break;
        case MAlfBufferProvider::ESourceFormatARGB_8888_PRE:
            surfaceFormat = EUidPixelFormatARGB_8888_PRE;
            break;
        default:
            User::Leave( KErrNotSupported );
            break;
        }
    
    TInt err = KErrNone;
    if(!iData->iSurfaceManager)
        {
        iData->iSurfaceManager = new RSurfaceManager();
        User::LeaveIfNull(iData->iSurfaceManager);    
          
        err = iData->iSurfaceManager->Open();
        User::LeaveIfError(err);    
        
        RSurfaceManager::TSurfaceCreationAttributesBuf attributes;
        attributes().iPixelFormat           = surfaceFormat;
        attributes().iSize                  = size;
        attributes().iBuffers               = 1;
        attributes().iStride                = aCreationAttributes.iStride;
        attributes().iAlignment             = aCreationAttributes.iAlignment;
        attributes().iContiguous            = ETrue;
        attributes().iMappable              = ETrue;

        // Create surface
        err = iData->iSurfaceManager->CreateSurface(attributes, iData->iSurfaceId);
        User::LeaveIfError(err);    
        }
    
    if(!iData->iSurfaceChunk)
        {
        // Map to chunk
        iData->iSurfaceChunk = new RChunk();
        User::LeaveIfNull(iData->iSurfaceChunk);    
        err = iData->iSurfaceManager->MapSurface(iData->iSurfaceId, *iData->iSurfaceChunk);
        User::LeaveIfError(err);    
        }

    // Get the info from the surfaceManager
    RSurfaceManager::TInfoBuf info;
    err = iData->iSurfaceManager->SurfaceInfo(iData->iSurfaceId, info);
    User::LeaveIfError(err);        
    RSurfaceManager::TSurfaceInfoV01 surfaceInfo = info();
    aCreationAttributes.iStride = surfaceInfo.iStride;
    
    TInt offset = 0;
    iData->iSurfaceManager->GetBufferOffset(iData->iSurfaceId, 0,  offset);

    // Store pointer to the pixel data
    iData->iSurfaceBuffer = iData->iSurfaceChunk->Base() + offset;
    }

// ---------------------------------------------------------------------------
// CAlfCompositionPixelSource::FreeSurface
// ---------------------------------------------------------------------------
//
void CAlfCompositionPixelSource::FreeSurface()
    {
    if(iData->iWindow &&  iData->iSourceStatus != CAlfCompositionPixelSourceData::ESuspended)
        {
        iData->iWindow->RemoveBackgroundSurface(ETrue);
        iData->iWindow->Session()->Flush();
        }
    
    if(iData->iSourceStatus != CAlfCompositionPixelSourceData::ESuspended)
        {
        iData->iSurfaceUpdateSession.CancelAllUpdateNotifications();
        }
    iData->iSurfaceUpdateSession.Close();
    
    if(iData->iSurfaceManager)
        {
        iData->iSurfaceManager->CloseSurface(iData->iSurfaceId);
        iData->iSurfaceManager->Close();
        delete iData->iSurfaceManager;
        iData->iSurfaceManager = NULL;
        }
    
    iData->iSurfaceId = TSurfaceId::CreateNullId();
    
    if(iData->iSurfaceChunk)
        {
        iData->iSurfaceChunk->Close();
        delete iData->iSurfaceChunk;
        iData->iSurfaceChunk = NULL;
        }
    }

// ---------------------------------------------------------------------------
// CAlfCompositionPixelSource::Buffer
// ---------------------------------------------------------------------------
//
TUint8* CAlfCompositionPixelSource::Buffer(TInt /*aBufferNumber*/)
    {
    return iData->iSurfaceBuffer;
    }

// ---------------------------------------------------------------------------
// CAlfCompositionPixelSource::DrawFrameL
// ---------------------------------------------------------------------------
//
TBool CAlfCompositionPixelSource::DrawFrameL(TRequestStatus& aStatus, TInt aBufferNumber)
    {
    // Region indicates the currently visible region of the surface. The information is not yet available.
    RRegion region;
    region.AddRect(iData->iSurfaceRect);
    TUint8* buffer = Buffer(aBufferNumber);
    TBool draw = iData->iProvider.ProduceNewFrameL(region, buffer);
    if(draw)
        {
        iData->iSurfaceUpdateSession.NotifyWhenDisplayed(aStatus, iData->iTimeStamp);        
    
        iData->iSurfaceUpdateSession.SubmitUpdate(KAllScreens, iData->iSurfaceId, aBufferNumber);
        }
    else
        {
        iData->iSourceStatus = CAlfCompositionPixelSourceData::EPaused;
        }
    region.Close();
    return draw;
    }

// ---------------------------------------------------------------------------
// CAlfCompositionPixelSource::HandleEventL
// ---------------------------------------------------------------------------
//
void CAlfCompositionPixelSource::HandleEventL(TInt aEventType, TAny* aEventData)
    {
    switch (aEventType)
        {        
        case KAlfCompositionFrameReady:
            {
            // do nothing for now
            }    
            break;
        case KAlfCompositionLowOnGraphicsMemory:
        case KAlfCompositionTargetHidden:
            {
            // suspend drawing
            Suspend();
            }    
            break;
        default:
            break;
        }
    // call base class
    CAlfCompositionSource::HandleEventL( aEventType, aEventData );
    }

class CAlfSignalObserver;

class MAlfEffectObserverData
    {
    public:
        virtual void Remove(CAlfSignalObserver* aObserver) = 0;
        virtual RAlfBridgerClient& Client() = 0;
    };

NONSHARABLE_CLASS(CAlfEffectObserver::CAlfEffectObserverData): public CBase, public MAlfEffectObserverData
    {
    public:
    void SubscribeCallbackL(MAlfEffectObserver* aObserver, TInt aHandle, TInt aType = MAlfEffectObserver::EAlfEffectComplete );        
    ~CAlfEffectObserverData();
    void Remove(CAlfSignalObserver* aObserver);
    RAlfBridgerClient& Client();
    
    // data
    RAlfBridgerClient iClient;
    RPointerArray<CAlfSignalObserver> iObservers;
    };


NONSHARABLE_CLASS(CAlfSignalObserver):public CActive
    {
    public:    
    CAlfSignalObserver(CAlfEffectObserver::MAlfEffectObserver* aObserver, MAlfEffectObserverData* aOwner, TInt aHandle, TInt aType):CActive(EPriorityStandard),
        iOwner(aOwner), iObserver(aObserver), iHandle(aHandle), iType(aType), iArgs(aHandle,aType)
        {   
        CActiveScheduler::Add(this);
        SetActive(); 
        }
		
    ~CAlfSignalObserver()
        {
        Cancel();
        }

    private:
    
        void DoCancel()
            {
            if (iOwner)
                {
                iOwner->Client().SendSynch(EAlfCompleteSignal, TIpcArgs(iHandle, iType));
                }
            }
    
	    // just to prohibit cancel outside destructor
        void Cancel()
            {
            CActive::Cancel();
			}	   
	
    void RunL()
        {    
        iObserver->HandleEffectCallback(iType, iHandle, iStatus.Int());
        iOwner->Remove(this);
        }    
    
	public:
        MAlfEffectObserverData* iOwner;
        CAlfEffectObserver::MAlfEffectObserver* iObserver;
        TInt iHandle;        
        TInt iType;
        TIpcArgs iArgs;
    };

void CAlfEffectObserver::CAlfEffectObserverData::SubscribeCallbackL(MAlfEffectObserver* aObserver, TInt aHandle, TInt aType)
    {
    CAlfSignalObserver* obs = new (ELeave) CAlfSignalObserver(aObserver, this, aHandle, aType);
    CleanupStack::PushL(obs);
    User::LeaveIfError(iObservers.Append(obs));
    CleanupStack::Pop();
    iClient.SendAsynchronous(EAlfRequestSignal, obs->iArgs, obs->iStatus);        
    }
        
CAlfEffectObserver::CAlfEffectObserverData::~CAlfEffectObserverData()
    {    
    iObservers.ResetAndDestroy();
    iObservers.Close();
    iClient.Close(); // destroys signals
    }

RAlfBridgerClient& CAlfEffectObserver::CAlfEffectObserverData::Client()
    {
    return iClient;
    }

void CAlfEffectObserver::CAlfEffectObserverData::Remove(CAlfSignalObserver* aObserver)
    {
    TInt index = iObservers.Find(aObserver);
    if (index != KErrNotFound)
        {
        iObservers.Remove(index);
        }
    delete aObserver;
    }    
    
EXPORT_C CAlfEffectObserver* CAlfEffectObserver::NewL()
    {
    CAlfEffectObserver* me = new (ELeave) CAlfEffectObserver();
    CleanupStack::PushL(me);
    me->iData = new (ELeave) CAlfEffectObserverData();
    User::LeaveIfError(me->iData->iClient.Connect());
    CleanupStack::Pop();
    return me;    
    }

EXPORT_C CAlfEffectObserver::~CAlfEffectObserver()
    {
    delete iData; 
    }

EXPORT_C TInt CAlfEffectObserver::ActiveEffectsCount()
    {
    return iData->iClient.EffectsCount();
    }

EXPORT_C void CAlfEffectObserver::SubscribeCallbackL(MAlfEffectObserver* aObserver, TInt aHandle, TInt aType)
    {
    iData->SubscribeCallbackL(aObserver, aHandle, aType ); 
    }

EXPORT_C void CAlfEffectObserver::SetDistractionWindow(const RWindowTreeNode& aWindow, TInt aState)
    {
    if (!&aWindow)
        {
        return;
        }
    iData->iClient.SendSynch(EAlfSetDistractionWindow, TIpcArgs(aWindow.WindowGroupId(), aWindow.ClientHandle(), aState));
    }

CAlfEffectObserver::CAlfEffectObserver()
    {
    }
 
//end of file

