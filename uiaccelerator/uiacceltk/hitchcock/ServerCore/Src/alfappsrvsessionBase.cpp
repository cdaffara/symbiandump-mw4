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
* Description:   Server session
*
*/


#include <uiacceltk/HuiEnv.h>
#include <uiacceltk/HuiRoster.h>
#include <uiacceltk/HuiDisplay.h>
#include <uiacceltk/HuiEvent.h>
#include <uiacceltk/HuiControl.h>
#include <uiacceltk/HuiControlGroup.h>
#include <uiacceltk/HuiUtil.h>

#include "alf/alfappsrvsessionbase.h"
#include "alf/alfappserver.h"
#include "alf/alfappui.h"
#include "alf/alfconstants.h"
#include "alfloggingconfiguration.h"
#include "alflogger.h"
#include "alfsrvdisplaysubsession.h"
#include "alfsrvcontrolgroupsubsession.h"
#include "alfsrvtransformationsubsession.h"
#include "alf/alfextensionfactory.h"
#include "alf/alfversion.h"
#include "alfsrvsettingshandler.h"
#include "alfstreamerconsts.h"
#include "alfdecoderserverclient.h"

const TUint KAlfMaxCpuUsageDuringPointerEvent = 35; // Percentage

struct TAlfHandleAndImplUid
    {
    TAlfHandleAndImplUid(TInt aHandle, TInt aUid):iHandle(aHandle),iUid(aUid){}
    static TBool Compare(const TInt* aKey, const TAlfHandleAndImplUid& aEntry)
        {
        return (*aKey==aEntry.iHandle);
        }
    TInt iHandle;
    TInt iUid;
    };

struct TAlfTouchEventS: public TAlfTouchEvent
    {
    TAlfTouchEventS(const THuiEvent& aEvent) 
        {
        iEvent = aEvent.iPointerEvent;
        Mem::FillZ(iControls, sizeof(iControls));
        Mem::FillZ(iVisuals, sizeof(iVisuals));
        }
        
    void AppendCtrl(TInt aControlHandle, TInt aVisualHandle)
        {
        for (TInt ii = 0; ii < KAlfTouchEventControlCount; ii++)
            {
            if (iControls[ii] == 0)
                {
                iControls[ii] = aControlHandle;
                iVisuals[ii] = aVisualHandle;
                break;
                }
            else if ( ii == KAlfTouchEventControlCount-1 )
                {
                __ALFLOGSTRING( "TAlfTouchEventS::AppendCtrl: Overflow" )
                }
            else
                {
                // For PC-lint
                }
            }
        }
    };

NONSHARABLE_CLASS(CAlfAppSrvSessionBase::TPrivateData)
    {
    public:
    enum TAlfPtrEventState 
        {
        ENotInitialized,
        EStartNewEvent,
        EUpdateCtrls,
        ENotFocused
        };
    
    public:
    TPrivateData():iSharedEnv(0),iContainer(0),iObjectIx(0) {}
    CHuiEnv* iSharedEnv;
    TInt iClientWindowGroupId;
    CObjectCon* iContainer;     // Not own
    CObjectIx* iObjectIx;       // Own
    RArray<TAlfHandleAndImplUid> iHandles;    
    RPointerArray<CAlfClientMessage> iMessages;
    RArray<TAlfTouchEventS>       iHuiEvents; // now only pointer events are supported
    TAlfPtrEventState iPtrEventState;
    RMessagePtr2 iPointerEvent;
    RMessagePtr2 iSystemEvent;
    TInt iParentId;
    TBool iActive;
    TUint iSecureId;
    }; 

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Cosntructor
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfAppSrvSessionBase::CAlfAppSrvSessionBase(const CAlfAppServer* aServer)
    :iHost(const_cast<CAlfAppServer*>(aServer))
    {
    }

EXPORT_C CAlfAppSrvSessionBase::CAlfAppSrvSessionBase()
    :iHost(static_cast<CAlfAppServer*>(CEikonEnv::Static()->AppServer()))
    {
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfAppSrvSessionBase::~CAlfAppSrvSessionBase()
    {
        
    iHost->HandleClientExit(reinterpret_cast<TInt>(this));
    
    if ( iData )
        {
        if (AlfAppUi()->BridgerClient())
            {
            AlfAppUi()->BridgerClient()->SendBlind(EAlfExcludeFromGoomTargets, TIpcArgs(iData->iSecureId,iData->iClientWindowGroupId));
            }
            
        iData->iHandles.Close();
        delete iData->iObjectIx;
    
        // there shouldn't be orphean messages unless client app has been abnormally terminated...
        while( iData->iMessages.Count() )
            {
            iData->iMessages[0]->CompleteMessageD(KErrCancel);
            iData->iMessages.Remove(0);
            }

        iData->iMessages.Close();
        iData->iHuiEvents.Close();
        if (!iData->iPointerEvent.IsNull())
            {
            iData->iPointerEvent.Complete(KErrCancel);
            }
        if (!iData->iSystemEvent.IsNull())
            {
            iData->iSystemEvent.Complete(KErrCancel);
            }
        
        if ( iData->iContainer )
            {
            iHost->ReleaseContainer( *iData->iContainer );
            }
        delete iData;
        }
    iHost = NULL;
    }
  
// ---------------------------------------------------------------------------
// Called when focus gained.
// ---------------------------------------------------------------------------
//  
EXPORT_C void CAlfAppSrvSessionBase::FocusGainedL( TBool /*aDoTransitionEffect*/ )
    {
    // do some default effect if any
    }
  
// ---------------------------------------------------------------------------
// Called when focus lost.
// ---------------------------------------------------------------------------
//    
EXPORT_C TBool CAlfAppSrvSessionBase::FocusLostL( TBool /*aDoTransitionEffect*/ )
    {
    // do some default effect if any
    return EFalse;
    }
    
// ---------------------------------------------------------------------------
// Returns server.
// ---------------------------------------------------------------------------
//  
EXPORT_C CAlfAppServer* CAlfAppSrvSessionBase::AlfServer()
    {
    return iHost;
    }

// ---------------------------------------------------------------------------
// Returns AppUi.
// ---------------------------------------------------------------------------
//  
EXPORT_C CAlfAppUi* CAlfAppSrvSessionBase::AlfAppUi()
    {
    return iHost->AppUi(); // there is always AppUi when it is possible to create clients
    }

// ---------------------------------------------------------------------------
// From class CAknAppServiceBase.
// Called when client requests a service.
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfAppSrvSessionBase::ServiceL(const RMessage2& aMessage)
    {
    iData->iSecureId = aMessage.SecureId(); // one time would be enough    
    if(iHost->MetricsInterface())
        {
        iHost->MetricsInterface()->EnterClientMessageL(reinterpret_cast<TInt>(this), aMessage);
        if (aMessage.IsNull())
            { // metrics interface completed the message, just abort handling of message 
            return;
            }
        }
        
    
#ifdef _ALF_LOGGING
    TRAPD( err, 
        {
        if (!DoSubSessionCmdL(aMessage))    
            {
            DoHandleCommandL(aMessage);
            }
        } );
    if ( err != KErrNone )
        {
        __ALFLOGSTRING2( "CAlfAppSrvSessionBase::ServiceL req %d err %d", aMessage.Function(), err )
        User::Leave( err );
        }
        
#else
    if (!DoSubSessionCmdL(aMessage))    
        {
        DoHandleCommandL(aMessage);
        }
#endif

    if(iHost->MetricsInterface())
        {
        iHost->MetricsInterface()->ExitClientMessage(reinterpret_cast<TInt>(this), aMessage);
        }

    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
TBool CAlfAppSrvSessionBase::DoSubSessionCmdL(const RMessage2& aMessage)
    {
    TBool handled = ETrue;
    TInt function = aMessage.Function();
     // check if we are creating a subsession
    if ( function == EAlfCreateSubSession )
        {
        // Create subsession.
        CAlfSrvSubSessionBase* subSession = CreateSubSessionLC( aMessage );
        
        // Add to container (takes ownership)
        iData->iContainer->AddL( subSession );
        CleanupStack::Pop( subSession );
        
        const TInt handle = iData->iObjectIx->AddL( subSession );
        
        iData->iHandles.AppendL(TAlfHandleAndImplUid(handle,subSession->iFactoryUid));
        
        // Inform client of the handle its been allocated
        TPckg<TInt> handlePckg( handle );
        aMessage.Write(3,handlePckg);
        
        if ( !aMessage.IsNull() )
            {
            // Complete the message
            aMessage.Complete( KErrNone );
            }
        }
        
    else if ( function == EAlfDoSubSessionCmd ||
        function == EAlfDoAsyncSubSessionCmd ||
        function == EAlfSubSCancelAsynchRequest )
        {
        // Find an appropriate object and pass the message to it
        // for processing...
        //
        // we don't allow subsessions close each other, but session that sent the 
        // original request must be used for cancelling  
        //
        // We could use subsession information from message queue if anyone 
        // needs to be able to cancel commands
        const TInt handle = aMessage.Int3();
        
        // Fetch the sub-session by its handle
        CObject* object = iData->iObjectIx->At( handle );
        __ASSERT_ALWAYS( object , USER_INVARIANT() );
         
        CAlfSrvSubSessionBase* subsession = static_cast<CAlfSrvSubSessionBase*>(object);
        if (function != EAlfSubSCancelAsynchRequest)
            {
            subsession->ProcessMessageL( aMessage );
            }
        else
            {
            TInt index = iData->iMessages.Find(aMessage.Int0(), CAlfClientMessage::Compare);
            TInt count = iData->iMessages.Count();
            
            if  (index != KErrNotFound)   
                {
                TInt2 cancelParams(iData->iMessages[index]->iClientId, iData->iMessages[index]->iDecodedOp);
                TPckgC<TInt2> inbuf(cancelParams);
                TBuf8<1> dum;
                MAlfExtension* cmdHandler =  subsession->AsCommandHandler();
                if (cmdHandler)
                    {
                    cmdHandler->HandleCmdL(EAlfSubSCancelAsynchRequest, inbuf, dum);
                    }
                if (count == iData->iMessages.Count()) // message not successfully removed
                    {
#ifdef _ALF_LOGGING
                    RThread thread; 
                    iData->iMessages[index]->iMessage.ClientL(thread);
                    const TName threadName = thread.Name();
                    __ALFLOGSTRING2( "CAlfAppSrvSessionBase::DoSubSessionCmdL: Asynch message from client: %S, op: %d was not cancelled correctly",
                        &threadName, iData->iMessages[index]->iDecodedOp)
                    thread.Close();
#endif // _ALF_LOGGING       
                    
                    iData->iMessages[index]->CompleteMessageD(KErrCancel);
                    iData->iMessages.Remove(index);
                    }                
                }
            aMessage.Complete(KErrNone);
            }
        }

    else if (function == EAlfCloseSubSession)
        {
        CloseSubSession(aMessage);
        }
     else 
        {
        handled = EFalse;
        }    
    
    return handled;
    }

// ---------------------------------------------------------------------------
// Called on contruction.
// ---------------------------------------------------------------------------
// 
EXPORT_C void CAlfAppSrvSessionBase::BaseConstructL()
    {
    // instrument private data etc..
    __ASSERT_ALWAYS(!iData,USER_INVARIANT());
    iData = new (ELeave) TPrivateData();
    iData->iSharedEnv = &AlfAppUi()->HuiEnv();
    iData->iClientWindowGroupId = KErrNotFound;
    
    iData->iContainer = iHost->NewContainerL();
    iData->iObjectIx = CObjectIx::NewL();  
    iData->iParentId = KErrNotFound; 
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
EXPORT_C void CAlfAppSrvSessionBase::ExtensionInterfaceL(TUid /*aInterfaceId*/, TAny*& /*aImplementaion*/)
    {
    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
EXPORT_C CHuiEnv* CAlfAppSrvSessionBase::SharedHuiEnv() const
    {
    if (iData)
        {
        return iData->iSharedEnv;
        }
        
    return 0;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
EXPORT_C void CAlfAppSrvSessionBase::SetClientWindowGroupId(TInt aId)
    {
    __ASSERT_DEBUG(iData, USER_INVARIANT());
    iData->iClientWindowGroupId = aId;
	//Adding this app to goom list - otherwise app like photos will never be killed
    if (AlfAppUi()->BridgerClient())
        {
        AlfAppUi()->BridgerClient()->SendBlind(EAlfVolunteerForGoomTarget, TIpcArgs(iData->iSecureId, iData->iClientWindowGroupId));
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//     
EXPORT_C TRect CAlfAppSrvSessionBase::ClientDrawingArea() const
    {
    return TRect(0,0,0,0);
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
TInt CAlfAppSrvSessionBase::ClientWindowGroup() const
    {
    if (iData)
        {
        return iData->iClientWindowGroupId;
        }
    else
        {
        return KErrNotFound;
        }
    }
  
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//   
CAlfSrvSubSessionBase* CAlfAppSrvSessionBase::CreateSubSessionLC(const RMessage2& aMessage)
    {
    CAlfSrvSubSessionBase* subSession = NULL;
    
    TPckgBuf<TAlfImplementationInformation> buf;
    aMessage.ReadL(0,buf);

    if (buf().iImplementationUid == 0) // built in component requested
        {
        switch (buf().iImplementationId)
            {
            case EAlfDisplaySubSessionOpen:
                {
                TRect rect;
                TPckg<TRect> rectPckg(rect); 
                aMessage.Read(1,rectPckg);

                TAlfDisplayCreateParams params;
                TPckg<TAlfDisplayCreateParams> paramPckg(params); 
                aMessage.Read(2,paramPckg);

                const TInt windowGroupId = params.iWindowGroupId;
                const TInt displayType = params.iDisplayType;
                subSession = CAlfSrvDisplaySubSession::NewLC( *this, rect, windowGroupId, displayType, KHuiUidBackBufferScreen0 );
                break;
                }
            case EAlfDisplaySubSessionOpen2:
                {
                TRect rect;
                TPckg<TRect> rectPckg(rect); 
                aMessage.Read(1,rectPckg);

                TAlfDisplayCreateParams2 params;
                TPckg<TAlfDisplayCreateParams2> paramPckg(params); 
                aMessage.Read(2,paramPckg);

                const TInt windowGroupId = params.iWindowGroupId;
                const TInt displayType = params.iDisplayType;
                const TUid bufferUid = params.iBufferUid;
                subSession = CAlfSrvDisplaySubSession::NewLC( *this, rect, windowGroupId, displayType, bufferUid );
                break;
                }
            case EAlfControlGroupSubSessionOpen:
                {
                const TInt id = aMessage.Int1();
                subSession = CAlfSrvControlGroupSubSession::NewLC( *this, id );
                break;
                }
            
            case EAlfTransformationSubSessionOpen:
                {
                subSession = CAlfSrvTransformationSubSession::NewLC( *this );
                break;
                }
            
            default:
                subSession = CAlfSrvSubSessionBase::NewLC( aMessage, 
                                                   buf().iImplementationUid,
                                                   buf().iImplementationId, 
                                                   *this );
                break;
            }
        }
    else
        {
        // Check what API has been used when app has been compiled before
        // loading extension. If runtime version is no more binary compatible, leave
        TVersion compiletimeVersion = buf().iVersion;
        TVersion runtimeVersion = TVersion(ALF_VERSION_MAJOR, ALF_VERSION_MINOR, ALF_VERSION_BUILD);
        // Major version (AlfClient.dll) has been already checked in the client side at this time.        
        // Minor version (AlfServercore, coretoolkit) is checked here.
        if (compiletimeVersion.iMinor != runtimeVersion.iMinor)
            {
            User::Leave(KErrNotSupported);    
            }
        // For check also build here
        else if (compiletimeVersion.iBuild > runtimeVersion.iBuild)
            {                
            User::Leave(KErrNotSupported);    
            }    
        else
            {                
            subSession = CAlfSrvSubSessionBase::NewLC( aMessage, 
                                                       buf().iImplementationUid,
                                                       buf().iImplementationId,                                                        *this );
            }
        }
         
    return subSession;
    }


// -----------------------------------------------------------------------------
// Remove object from object index
// -----------------------------------------------------------------------------
//
void CAlfAppSrvSessionBase::CloseSubSession( const RMessage2& aMessage )
    {
    TInt handle = aMessage.Int3();
    CloseSubSession(handle);
    
    TInt err = KErrNone;

#ifdef _DEBUG    
    for (TInt ii = iData->iMessages.Count()-1; ii >= 0; ii--)
        {
        if ((iData->iMessages)[ii]->iSubSession == handle)
            {
            iData->iMessages[ii]->CompleteMessageD(KErrCancel);
            iData->iMessages.Remove(ii);
            err = KErrCompletion;
            }
        }
#endif
        
    aMessage.Complete( err );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
void CAlfAppSrvSessionBase::CloseSubSession(TInt aHandle)
    {
    CObject* obj = iData->iObjectIx->At( aHandle );  
    __ASSERT_ALWAYS( obj, USER_INVARIANT() );
    iData->iObjectIx->Remove( aHandle );
    
    TInt index = iData->iHandles.Find(aHandle, TAlfHandleAndImplUid::Compare);
    if (index != KErrNotFound)
        {
        AlfServer()->DestroyedObject( iData->iHandles[index].iUid );
        iData->iHandles.Remove(index);
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
TBool CAlfAppSrvSessionBase::HasSession( TInt aSubSessionHandle ) const
    {
    CObject* object = iData->iObjectIx->At( aSubSessionHandle );
    return object != NULL;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
CAlfSrvSubSessionBase& CAlfAppSrvSessionBase::SubSession( TInt aSubSessionHandle )
    {
    // Fetch the sub-session by its handle
    CObject* object = iData->iObjectIx->At( aSubSessionHandle );
    __ASSERT_ALWAYS( object, USER_INVARIANT() );
    CAlfSrvSubSessionBase* subSession = static_cast<CAlfSrvSubSessionBase*>(object);
    return *subSession;
    }
 
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
TAny* FetchInterface(const THuiInterfaceSupport aType, CAlfSrvSubSessionBase* aSubSession )
	{
	TAny* result = 0;
	switch (aType)
	    {
	    case EHuiObjectTypeVisual:
	        result = aSubSession->AsHuiVisual();
	        break;
	    
	    case EHuiObjectTypeLayout:
	        result = aSubSession->AsHuiLayout();
	        break;
	    
	    case EHuiObjectTypeControl:
	        result = aSubSession->AsHuiControl();
	        break;
	        
	    case EHuiObjectTypeControlGroup:
	        result = aSubSession->AsHuiControlCroup();
	        break;
	        
	    case EHuiObjectTypeDisplay:
	        result = aSubSession->AsHuiDisplay();
	        break;
	    
	    case EHuiObjectTypeBrush:
	        result = aSubSession->AsHuiBrush();
	        break;
	    
	    case EHuiInterfaceMappingFunction:
	        result = aSubSession->AsMappingFunction();        
	        break;
	
	    case EHuiInterfaceVisualOwner:
	        result = aSubSession->AsVisualOwner();        
	        break;
	    case EAlfBrushHandler:
	        result = aSubSession->AsBrushHandler();
	        break;
	    default:
	       {
	       if ( aSubSession->AsCommandHandler() )    
	           {
	           result = aSubSession->AsCommandHandler()->GetInterface(aType);     
	           }
	       break;
	       }
	   }    
	return result;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
EXPORT_C TAny* CAlfAppSrvSessionBase::GetInterfaceL(const THuiInterfaceSupport& aType, TInt aHandle)
    {
    CObject* obj = iData->iObjectIx->At( aHandle );  
    if (!obj)
        {
        User::Leave(KErrArgument);
        }
    
    CAlfSrvSubSessionBase* subSession = static_cast<CAlfSrvSubSessionBase*>(obj);

    TAny* result = FetchInterface(aType, subSession);

    if (!result)
        {
        User::Leave(KErrArgument);
        }
    
    return result;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
EXPORT_C TInt CAlfAppSrvSessionBase::GetHandleFromInterface(const THuiInterfaceSupport& aType, TAny* aInterface)
    {
    TInt handle = KErrNotFound;
    for (TInt ii = iData->iHandles.Count()-1; ii >= 0; ii--)
        {
        const TInt handleitr = iData->iHandles[ii].iHandle;
        CAlfSrvSubSessionBase* subSession = &SubSession(handleitr);

        TAny* result = FetchInterface(aType, subSession);    
        
        if (result == aInterface)
            {
            handle = handleitr;
            break;
            }
        }
    return handle;
    }
 
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
EXPORT_C CAlfLayoutManager* CAlfAppSrvSessionBase::LayoutManager() const
    {
    return NULL;
    }

// ---------------------------------------------------------------------------
// Extension method to implement new virtual like methods w/o breakin binary compatibility
// ---------------------------------------------------------------------------
//     
EXPORT_C void CAlfAppSrvSessionBase::AlfInterfaceProviderExtension(const TUid& /*aExtensionUid*/, TAny** /*aExtensionParams*/)
    {
    
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
EXPORT_C void CAlfAppSrvSessionBase::FactoryDestroyed(TInt aFactoryUid)
    {
    // Todo: We should rather close the whole application...
    
    for (TInt ii = iData->iHandles.Count()-1; ii >= 0; ii--) // destroy in reverse order
        {
        if (iData->iHandles[ii].iUid == aFactoryUid)
            {
            CloseSubSession(iData->iHandles[ii].iHandle);
            }
        }
    }
 
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
CAlfClientMessage* CAlfAppSrvSessionBase::CreateNewMessageHolderL(const RMessage2& aMessage)
    {
    CAlfClientMessage* msg = CAlfClientMessage::NewLC(aMessage.Int3(), aMessage);
    
    TInt lenght1 = aMessage.GetDesLength(1);
    TInt lenght2 = aMessage.GetDesMaxLength(2);
     
    msg->iInBuf = HBufC8::NewL(lenght1);
    TPtr8 inBuf = msg->iInBuf->Des();
    aMessage.ReadL(1,inBuf); 
        
    msg->iOutBuf = HBufC8::NewL(lenght2);
    
    if (aMessage.Function() == EAlfDoAsyncSubSessionCmd)
        {
        TInt2 params(0,0);
        TPckg<TInt2> pbuf(params);
        aMessage.ReadL(0,pbuf);
        msg->SetClientParams(params);
        }
    else
        {
        msg->iDecodedOp = aMessage.Int0();
        }
        
    iData->iMessages.InsertL(msg, 0);
    CleanupStack::Pop(msg);
    return msg;  
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
EXPORT_C TInt CAlfAppSrvSessionBase::HandleCurrentCommanndAsynch()
    {
    // If your code hits following assert, it is beacuse client side meant the command to be synchronous 
    // but the extension implementation wants to turn that asynchronous. 
    // either client side should request the command asynchronously, of server side implementation should complete 
    // the message synchronously    
    ASSERT(iData->iMessages[0]->iClientId);
    
    if (iData->iMessages.Count() && !iData->iMessages[0]->iIsAsync )
        {
        iData->iMessages[0]->iIsAsync = ETrue; 
        }
        
    return iData->iMessages[0]->iClientId;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
EXPORT_C void CAlfAppSrvSessionBase::CompleteCmd(TInt aCommandId, TInt aResult)
    {
    if (iData->iMessages.Count() == 0)
        {
        return; // we could leave instead..
        }
        
    if (aCommandId == 0) // subsession default 
        {
        if (aResult != KErrNone || !iData->iMessages[0]->iIsAsync) 
            {
            iData->iMessages[0]->CompleteMessageD(aResult);
            iData->iMessages.Remove(0);
            }

        return;
        }

    TInt index = iData->iMessages.Find(aCommandId, CAlfClientMessage::Compare);
    
    if  (index != KErrNotFound)   
        {
        iData->iMessages[index]->CompleteMessageD(aResult);
        iData->iMessages.Remove(index);
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
void CAlfAppSrvSessionBase::HandleEventL(TAny* aCtrlPtr, TAny* aVisualPtr, const THuiEvent& aEvent)
    {
    // resolve ctrl client handle
    switch(iData->iPtrEventState)
        {
        case CAlfAppSrvSessionBase::TPrivateData::EStartNewEvent:
            {
            const TAlfTouchEventS touchEvent( aEvent );      
            
            // Purge drag events that come in sequence, because the client cannot handle all in time.    
            if ( touchEvent.iEvent.iType == TPointerEvent::EDrag )
                {
                while ( iData->iHuiEvents.Count() && 
                        iData->iHuiEvents[0].iEvent.iType == TPointerEvent::EDrag )
                    {
                    iData->iHuiEvents.Remove(0);
                    }
                }
            
            iData->iHuiEvents.InsertL(touchEvent,0);            
            iData->iPtrEventState = CAlfAppSrvSessionBase::TPrivateData::EUpdateCtrls;
            } // fall through
        case CAlfAppSrvSessionBase::TPrivateData::EUpdateCtrls:
            {
            TInt controlHandle = GetHandleFromInterface(EHuiObjectTypeControl, aCtrlPtr);
            ASSERT(controlHandle != KErrNotFound); // panic at will
            
            TInt visualHandle = 0;
            if (aVisualPtr)
                {
                visualHandle = GetHandleFromInterface(EHuiObjectTypeVisual, aVisualPtr);
                ASSERT(visualHandle != KErrNotFound);
                }
            iData->iHuiEvents[0].AppendCtrl(controlHandle, visualHandle);
            }
            break;
        default: // don't do anything unless focused and initialized
            break;
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
void CAlfAppSrvSessionBase::StartPointerHandling()
    {
    if (iData->iPtrEventState != CAlfAppSrvSessionBase::TPrivateData::ENotInitialized)
        {
        iData->iPtrEventState = CAlfAppSrvSessionBase::TPrivateData::EStartNewEvent;
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
void CAlfAppSrvSessionBase::FlushPointerHandling()
    {
    if (iData->iPtrEventState != CAlfAppSrvSessionBase::TPrivateData::ENotInitialized)
        {
        iData->iPtrEventState = CAlfAppSrvSessionBase::TPrivateData::ENotFocused;
        if (iData->iHuiEvents.Count() == 1) // manually trigger the event queue if only one item
            {
            TriggerPointerEvent(0);
            }
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
void CAlfAppSrvSessionBase::TriggerPointerEvent(const RMessage2* aMessage)
    {
    if (aMessage)
    	{ 	
     	if (iData->iPointerEvent.IsNull())
        	{
        	iData->iPtrEventState = CAlfAppSrvSessionBase::TPrivateData::ENotFocused; // ensure that state machine gets running 
        	iData->iPointerEvent = *aMessage;
        
        	// Reset cpu time to the correct value
        	TUint maxCpuUsage = AlfAppUi()->SettingsHandler().MaxCpuUsage();
   			SharedHuiEnv()->SetMaxCpuTime(maxCpuUsage);
        	}
        else
        	{
        	// There is already message waiting. Complete this with an error, and proceed with existing message.
        	aMessage->Complete(KErrInUse);
        	}
    	}
    else if (iData->iPointerEvent.IsNull())
        {
    	// Server is calling this internally and client is not ready.
        // Take cpu time from rendering to give more time to event handling, both in server and client.
   		SharedHuiEnv()->SetMaxCpuTime(KAlfMaxCpuUsageDuringPointerEvent);
        }
        
    if (!iData->iPointerEvent.IsNull() 
        && (iData->iHuiEvents.Count() > 1 
            || (iData->iHuiEvents.Count() == 1 && iData->iPtrEventState == CAlfAppSrvSessionBase::TPrivateData::ENotFocused )))
        {
        TPckg<TAlfTouchEventS> buf(iData->iHuiEvents[iData->iHuiEvents.Count()-1]);
        TInt error = iData->iPointerEvent.Write(0,buf);
        if ( error != KErrNone )
            {
            __ALFLOGSTRING1( "CAlfAppSrvSessionBase::TriggerPointerEvent: write error: %d", error )
            }
        iData->iPointerEvent.Complete(error);        
        iData->iHuiEvents.Remove(iData->iHuiEvents.Count()-1);
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
void CAlfAppSrvSessionBase::CancelPointerEvents()
    {
    if (!iData->iPointerEvent.IsNull())
        {
        iData->iPointerEvent.Complete(KErrCancel);
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
void CAlfAppSrvSessionBase::GetSystemEvents(const RMessage2* aMessage)
    {
    __ASSERT_DEBUG(aMessage, USER_INVARIANT());
    if (aMessage)
        {
        if (iData->iSystemEvent.IsNull())
            {
            iData->iSystemEvent = *aMessage;
            }
        else 
            {
            aMessage->Complete(KErrInUse);
            }
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
void CAlfAppSrvSessionBase::TriggerSystemEvent(TInt aEvent)
    {
    if (!iData->iSystemEvent.IsNull()) 
        {
        TPckg<TInt> eventPckg(aEvent);
        TInt error = iData->iSystemEvent.Write(0,eventPckg);
        if ( error != KErrNone )
            {
            __ALFLOGSTRING1( "CAlfAppSrvSessionBase::TriggerSystemEvent: write error: %d", error )
            }
        iData->iSystemEvent.Complete(error);        
        }        
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
void CAlfAppSrvSessionBase::CancelSystemEvents()
    {
    if (!iData->iSystemEvent.IsNull())
        {
        iData->iSystemEvent.Complete(KErrCancel);
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//     
void CAlfAppSrvSessionBase::GetSubsessionsByTypeL( 
    RPointerArray<CAlfSrvSubSessionBase>& aArray, 
    const THuiInterfaceSupport& aType )
    {
    aArray.Reset();
    
    for (TInt ii = iData->iHandles.Count()-1; ii >= 0; ii--)
        {
        const TInt handleitr = iData->iHandles[ii].iHandle;
        CAlfSrvSubSessionBase* subSession = &SubSession(handleitr);

        if ( FetchInterface( aType, subSession ) )
            {
            aArray.AppendL(subSession);
            }
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//     
EXPORT_C CAlfAppServer::TAlfWGPostion CAlfAppSrvSessionBase::PreferredWindowGroupPosition() const
    {
    return CAlfAppServer::EBehindOfParent;
    }

  
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//    
EXPORT_C THuiRefreshMode CAlfAppSrvSessionBase::PreferredRefreshMode() const
    {
    return EHuiRefreshModeAutomatic;
    }

// ---------------------------------------------------------------------------
// Future proofing, just base call for now 
// ---------------------------------------------------------------------------
//     
EXPORT_C void CAlfAppSrvSessionBase::ServiceError(const RMessage2& aMessage,TInt aError)
    {
    CAknAppServiceBase::ServiceError(aMessage,aError);
    }

// ---------------------------------------------------------------------------
// Future proofing, just base call for now 
// ---------------------------------------------------------------------------
//     
EXPORT_C TInt CAlfAppSrvSessionBase::CountResources()
    {
    return CAknAppServiceBase::CountResources();
    }


// ---------------------------------------------------------------------------
// Future proofing, just base call for now 
// ---------------------------------------------------------------------------
//     
EXPORT_C void CAlfAppSrvSessionBase::Disconnect(const RMessage2& aMessage)
    {
    CAknAppServiceBase::Disconnect(aMessage);
    }

// ---------------------------------------------------------------------------
// From CSession2. Future proofing, just base call for now 
// ---------------------------------------------------------------------------
//     
EXPORT_C TInt CAlfAppSrvSessionBase::Extension_(TUint aExtensionId, TAny*& a0, TAny* a1)
    {
    return CAknAppServiceBase::Extension_(aExtensionId,a0,a1);
    }

void CAlfAppSrvSessionBase::SetParentWindowGroupId(TInt aParentId)
    {
    iData->iParentId = aParentId;
    }

TInt CAlfAppSrvSessionBase::ParentWindowGroupId()
    {
    return iData->iParentId;
    }

void CAlfAppSrvSessionBase::ResetRootlayerTransformationsL()
    {
    RPointerArray<CAlfSrvSubSessionBase> groups;
    CleanupClosePushL( groups );
    GetSubsessionsByTypeL( groups, EHuiObjectTypeControlGroup );
    for ( TInt g = 0 ; g < groups.Count() ; g++ )
        {
        CHuiControlGroup* group = groups[g]->AsHuiControlCroup();
        ASSERT( group ); 
        CHuiLayout* hostContainer = group->Control(0).ContainerLayout( NULL );
        if (hostContainer)
            {
            hostContainer->EnableTransformationL(EFalse);
            hostContainer->iOpacity.Set(1.f);
            }
        }
    CleanupStack::PopAndDestroy();
    }

// RnD
void CAlfAppSrvSessionBase::ActivateContainerLayoutL(TBool aActivate)
    {
    if (iData->iActive != aActivate)
        {
        iData->iActive = aActivate;
        RPointerArray<CAlfSrvSubSessionBase> groups;
        CleanupClosePushL( groups );
        GetSubsessionsByTypeL( groups, EHuiObjectTypeControlGroup );
        for ( TInt g = 0 ; g < groups.Count() ; g++ )
            {
            CHuiControlGroup* group = groups[g]->AsHuiControlCroup();
            ASSERT( group ); 
            CHuiLayout* hostContainer = group->Control(0).ContainerLayout( NULL );
            if (hostContainer)
                {
                if (aActivate)
                    {
                    hostContainer->ClearFlag(EHuiVisualFlagInactive);
                    }
                else
                    {
                    hostContainer->SetFlag(EHuiVisualFlagInactive);
                    }
                }
            }
        CleanupStack::PopAndDestroy();
        }
    }
    
TUint CAlfAppSrvSessionBase::SecureId()
    {
    if (iData)
        {
        return iData->iSecureId;
        }
    return 0;
    }            
   
// End of file    

