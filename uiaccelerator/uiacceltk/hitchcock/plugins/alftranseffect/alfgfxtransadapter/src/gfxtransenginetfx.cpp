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
* Description:   Adapter to the GfxTransEffect client. This is the entry point into
*                the Nokia specific parts of the KML transition sollution, from the
*                GfxTransEffect client. 
*
*/



#include <s32mem.h> //RMemReadStream
#include <coecntrl.h>
#include <ecom.h>

#include <akntranseffect.h>
#include <akntransitionutils.h>
#include <avkondomainpskeys.h>
#include <centralrepository.h>
#include <aknlistboxtfxinternal.h> // list box types
#include <eikenv.h>
#include <eikappui.h>
#include <eikapp.h>
#include <eiksrvs.h>

#include "gfxtransenginetfx.h"
#include "e32property.h"
//#include "wstranscontrolgraphic.h"
//#include "wsadaptergraphic.h"

//#include "tfxserverdef.h"
//#include "subwocon.h"

#include <gfxtranseffect/gfxtranseffect.h>

#include <alfdecoderserverclient.h>
#include <alf/AlfTransEffectPlugin.h>
#include <alflogger.h>

//maximum number of ongoing component effects
//No restrictions at the moment. Anything goes.
//const TInt KMaxOngoing = 10;

enum TNoteSequenceState
    {
    ENoSequence = 0,
    EFirst = 1,
    EMid = 2,
    ELast = 3
    };

// ======== LOCAL CLASS ========

NONSHARABLE_CLASS(CTransControlDrawerInfo) : public CBase
	{
	CTransControlDrawerInfo(TInt aHandle);
public:
	static CTransControlDrawerInfo* NewL(TInt aHandle, TRect& aRect);
	~CTransControlDrawerInfo();
	inline TInt Handle();
	inline CWsTransControlGraphic* Drawer();
	void DeleteDrawer();
	void SetHasWOParticipants(TBool aHasWOParticipant);
	TBool HasWOParticipants();
private:
	TInt iHandle;
	TBool iHasWOParticipant;
//	CWsTransControlGraphic* iDrawer;
	};

CTransControlDrawerInfo::CTransControlDrawerInfo(TInt aHandle) :
	iHandle(aHandle)
	{
	}

CTransControlDrawerInfo* CTransControlDrawerInfo::NewL(TInt aHandle, TRect& /*aRect*/)
	{
	RThread t; //To get id of current thread to use as client handle.
	CTransControlDrawerInfo* self = new (ELeave) CTransControlDrawerInfo(aHandle);
	CleanupStack::PushL(self);
//	self->iDrawer = CWsTransControlGraphic::NewL(aRect, aHandle, t.Id());
	CleanupStack::Pop(self);
	return self;
	}

void CTransControlDrawerInfo::SetHasWOParticipants(TBool aHasWOParticipant)
	{
	iHasWOParticipant = aHasWOParticipant;
	}

TBool CTransControlDrawerInfo::HasWOParticipants()
	{
	return iHasWOParticipant;
	}

CTransControlDrawerInfo::~CTransControlDrawerInfo()
	{
//	delete iDrawer;
	}

void CTransControlDrawerInfo::DeleteDrawer()
	{
//	delete iDrawer;
//	iDrawer = NULL;
	}

//Inline functions kept here, since they are only used within this file

inline TInt CTransControlDrawerInfo::Handle()
	{
	return iHandle;
	}

/*
inline CWsTransControlGraphic* CTransControlDrawerInfo::Drawer()
	{
	return iDrawer;
	}
*/	

/**
 * Helper class, handles requests to the transition plugin
 */
NONSHARABLE_CLASS(CAlfTransitionRequest): public CActive
{
public:
	enum TType 
		{
		ETransFinish,
		EPolicyUpdate
		};

	CAlfTransitionRequest(CGfxTransAdapterTfx* aParent, MTfxServerObserver* aOwner, TType aType);
	~CAlfTransitionRequest();		

	void IssueReq();
	inline TType Type();
	TPtr8** InBuf() { return &iInPtr; }
	TPtr8** OutBuf() { return &iOutPtr; }
	
	void ResetBufs()
		{
		delete iInPtr;
    	iInPtr = 0;
		delete iOutPtr;
    	iOutPtr = 0;	
		}
		
private:
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);

	MTfxServerObserver* iOwner;
	CGfxTransAdapterTfx* iParent;
	TType iType;
	TPtr8* iInPtr;
	TPtr8* iOutPtr;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CAlfTransitionRequest::CAlfTransitionRequest(CGfxTransAdapterTfx* aParent, MTfxServerObserver* aOwner, TType aType) : 
	CActive(CActive::EPriorityHigh), 
	iOwner(aOwner),
	iParent(aParent),
	iType(aType)
	{
	CActiveScheduler::Add(this);
	}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CAlfTransitionRequest::~CAlfTransitionRequest() 
	{
  	__ALFFXLOGSTRING("CAlfTransitionRequest - cancelling and deleting policy request" );
	Cancel();
	ResetBufs();
	}
	
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CAlfTransitionRequest::RunL()
	{
	ResetBufs();

	if ( iStatus.Int() == KErrCancel )
	    {
	    // If the request was cancelled, we are done
    	__ALFFXLOGSTRING("CAlfTransitionRequest: Policy request cancelled" );
	    return;
	    }
	
	switch( iType ) 
		{
/*		
// not supported at the time being - all transaction finishing is handled in alf
		case ETransFinish:
			iOwner->TransitionFinished(iStatus.Int());
			__ALFFXLOGSTRING1("Client::CActive.iOwner->TransitionFinished status = %d", iStatus.Int());
			break;
*/			
		case EPolicyUpdate:
			{
#ifdef _DEBUG
			RThread me;
			TInt ap = me.Id();
			me.Close();
			__ALFFXLOGSTRING2("CAlfTransitionRequest: Policy received. Cli: %d, status %d",ap,iStatus.Int());
#endif
            User::LeaveIfError( iStatus.Int() );
            iParent->GetNewPoliciesL( iStatus.Int() );
            User::LeaveIfError( iParent->RequestPolicyUpdates() );
			}
			break;
		default:
			break;
		}
	}
	
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TInt CAlfTransitionRequest::RunError(TInt aError)
	{
	//The idea here is that on failure (wich should be OOM), we clear all our policies
	//clientside.
	//We must also inform server of this, so it can resend all policies the next time we request.
	//Next time a transition is tried, policies will be re-requested.
#ifdef _DEBUG
	RThread me;
	TInt meid = me.Id();
	TInt tmp = aError; //remove warning when debug prints and turned off
	aError = tmp;
	__ALFFXLOGSTRING2("--->> Policy error <<--. Cli: %d, status: %d",meid,aError);
	me.Close();
#endif

	iOwner->ClearAllPolicies(); //clear all policies clientside
	iParent->CancelPolicyUpdates(); //cancel requests on server.

	return 0;
	}
	
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CAlfTransitionRequest::DoCancel()
	{
    //nothing to do here. requests are cancelled elsewhere.
	}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CAlfTransitionRequest::IssueReq() 
	{
	SetActive();
	}

// ---------------------------------------------------------------------------
// We keep it here, even though its inline, since it is never used outside this
// file anyway.
// The idea with .inl files is to keep the header file clean.
// ---------------------------------------------------------------------------
//
inline CAlfTransitionRequest::TType CAlfTransitionRequest::Type()
	{
	return iType;
	}


// ======== MEMBER FUNCTIONS ========

CGfxTransAdapterTfx::CGfxTransAdapterTfx(MGfxTransClient* aClient) :
	iClient(aClient),
	iConnected(EFalse),
	iHasPlugin(EFalse)
	{
	}
	
CGfxTransAdapterTfx::~CGfxTransAdapterTfx()
	{
	
    
    iControlEffects.Close();
	__ALFFXLOGSTRING("CGfxTransAdapterTfx for HWA transitionn effects, destructor ");
//	iIgnoredWOChildControls.Close();
	//iControlInfo.ResetAndDestroy();
	if ( iConnected )
	    {
	    if ( iHasPlugin )
	        {
        	__ALFFXLOGSTRING("CGfxTransAdapterTfx, canceling policy updates in destructor  ");
	        CancelPolicyUpdates();
	        // should we wait here until the cancellation has returned?
        	__ALFFXLOGSTRING("CGfxTransAdapterTfx, unloading plugin  ");
	        iTfxServer.UnloadTfxPlugin( iPluginImplementation );
	        }
	    }
	delete iPolicyReq;    
  	iTfxServer.Close();
  	iConnected = EFalse;
  	iHasPlugin = EFalse;
	delete iTransferBuffer;
	delete iReturnBuffer;
	delete iAsyncTransferBuffer;
	delete iAsyncReturnBuffer;
	}
	
void CGfxTransAdapterTfx::ConstructL()
	{
	
	__ALFFXLOGSTRING("CGfxTransAdapterTfx for HWA transitionn effects, ConstructL");
	
	RThread me;
	iClientId = me.Id();	
    me.Close();

    iPluginImplementation = TUid::Null();
    ConnectToServer();
    
    // We need to save various 32 bit integers. The exact number depends on the function.
    // We reserve a bit extra here, should check if the size is big enough for all cases.
    iTransferBuffer = HBufC8::NewL( 25 * sizeof(TInt32) );
    // If we get return values, we'll need a bigger buffer
    iReturnBuffer = HBufC8::NewL( sizeof( TInt ) );
        
    // We need to save various 32 bit integers. The exact number depends on the function.
    // We reserve a bit extra here, should check if the size is big enough for all cases.
    iAsyncTransferBuffer = HBufC8::NewL( 25 * sizeof(TInt32) );
    // If we get return values, we'll need a bigger buffer
    iAsyncReturnBuffer = HBufC8::NewL( sizeof( TInt ) );
        
	iPolicyReq = new( ELeave )CAlfTransitionRequest(this,
        this,
		CAlfTransitionRequest::EPolicyUpdate);
		
	}
	
void CGfxTransAdapterTfx::ConnectToServer()
    {
    // We need to resolve the plugin id for starters, 
    // use default parameters, we take the first one we find.

    TInt err = KErrNone;
    
    if ( !iConnected )
        {
        err = iTfxServer.Open();
        if ( err == KErrNone )
            {
            iConnected = ETrue;
            }
        else
            {
            // could not connect to server - cannot continue.
            return;
            }
        }
       
    if ( iHasPlugin )
        {
        // already connected
        return;
        }

    if ( iPluginImplementation == TUid::Null())
        {
        RImplInfoPtrArray pluginArray;
        TRAP( err, REComSession::ListImplementationsL( KAlfGfxPluginInterfaceUId, pluginArray ) );

	    if ( pluginArray.Count() > 0 )
	        {
	        iPluginImplementation = pluginArray[0]->ImplementationUid();
	        }
        // Clear the data in plugin array, it is not going to be used any more.    
        TInt i = 0;    
        for ( i = pluginArray.Count() - 1; i >= 0; i-- )
            {
            // destroy 
            delete pluginArray[i];
            pluginArray.Remove( i );
            }
        pluginArray.Close();
        }
		
    if ( err == KErrNone && iPluginImplementation != TUid::Null() )
        {
        err = iTfxServer.LoadTfxPlugin( iPluginImplementation );
        if ( err == KErrNone || err == KErrAlreadyExists )
            {
            // We have successfully loaded the plugin and saved
            // the implementation uid to be used later.
            iHasPlugin = ETrue;
        	__ALFFXLOGSTRING("HWA TransEffect Plugin loaded");
            }
        }
     
    }
	

// ---------------------------------------------------------------------------
// tests if adapter is active (trying to make it active if needed)
// ---------------------------------------------------------------------------
//
TBool CGfxTransAdapterTfx::IsActive()
	{
	TInt ap = iClientId.Id();
	__ALFFXLOGSTRING1("IsActive() cli: %d",ap);
	
	TInt err = KErrNone;
	if( !iHasPlugin ) 
		{
		ConnectToServer();
		if (!iHasPlugin )
		    {
    		return EFalse;
		    }
		}
		
	if( err == KErrNone )
		{
		//Make sure policy request handling is on.
		err = RequestPolicyUpdates();
		}
	else
		{
		//Failed to connect, remove all policies from client.
		iClient->ClearAllPolicies();
		}
		
	return (err == KErrNone);
	}

// ---------------------------------------------------------------------------
// Handles the different states of the client.
// ---------------------------------------------------------------------------
//
TInt CGfxTransAdapterTfx::HandleClientState( TClientState aState, const CCoeControl* aKey, TInt aHandle )
	{
	if( !iHasPlugin )
	    {
	    return KErrNone;
	    }
	 
	TInt err = KErrNone;    
	const CTransitionData* transdata;
	err = iClient->GetTransitionData( aHandle, transdata );

	switch(aState)
		{
		case EFallback:
	    case EAbort:
	        for(TInt i = 0 ; i < iControlEffects.Count(); i++ )
	            {
	            // clear ongoing effect for this handle
	            if( iControlEffects[i].iHandle == aHandle)
	                {
	                iControlEffects.Remove(i);
	                i--;
	                }
	            }
		    break;
		case EPreBeginCapture:
		    break;
		case EPostBeginCapture:
			// Send control effect request to ALf. This is done immediately after call of 
			// GfxTransEffect::Begin(). This makes it possible (NOT QUARANTEENED)
			// that effect request arrives to Alf before possible visiblity changes are made to 
			// the control.
		    if (aKey && aKey->DrawableWindow() && err == KErrNone)
		        {
                TControlEffect newEffect;
                newEffect.iHandle = aHandle;
                newEffect.iWindowGroup = aKey->DrawableWindow()->WindowGroupId();
                newEffect.iWindowHandle =aKey->DrawableWindow()->ClientHandle(); 
                iControlEffects.Append(newEffect);
		        }
		    break;
		case EPreEndCapture:	
		    break;
		case EPostEndCapture:
            if (aKey && err == KErrNone)
                {
                TRAP( err, GenerateComponentTransitionEventL( transdata, MAlfGfxEffectPlugin::EBeginComponentTransition, aHandle ) );
                }
		    break;
		case EGlobalAbort:
		    // abort component transition, no handle.
		    for(TInt i = 0 ; i < iControlEffects.Count(); i++ )
                {
                // clear all on going effects
                iControlEffects.Remove(i);
                i--;
                }
          // TODO: send abort to server
		   // TRAP( err, GenerateComponentTransitionEventL( transdata, MAlfGfxEffectPlugin::EAbortComponentTransition) );
		    
	        break;
		case EBeginGroup:
			{
            __ALFFXLOGSTRING1("-- BeginGroup: New transition group for groupid: %d)",aHandle);
            SendGroupCommand(aHandle, EFalse);
		    break;
			}
		case EEndGroup:
			{
            __ALFFXLOGSTRING1("-- EndGroup: closing transition group: %d)",aHandle);
            SendGroupCommand(aHandle, ETrue);
		    break;
			}
		default:
			break;
		}
	return err;
	}

// ---------------------------------------------------------------------------
// Called by client when a participant of a transition updates in mid transition.
// ---------------------------------------------------------------------------
//
void CGfxTransAdapterTfx::HandleParticpantUpdate(TInt /*aHandle*/, const CParticipantData* /*aParticipant*/, 
												RWsGraphicMsgBuf* /*aCommandBuffer*/, 
												const TRect& /*aDrawRect*/, const TRect& /*aBoundingRect*/)
	{
/*	
	if(aCommandBuffer->Count() == 0)
	    {
	    return;
	    }
	
	const CTransitionData* transdata;
	TInt err = iClient->GetTransitionData(aHandle,transdata);
	if(err != KErrNone) 
		{
		return;
		}
		
	CTransControlDrawerInfo* drawinfo = FindControl(aHandle);
	if(drawinfo && drawinfo->HasWOParticipants()) 
		{
		return;
		}
		
		
	TInt participantid = -1;
	if(aParticipant == transdata->iBaseControl) 
		{
		participantid = 0;
		}
	else
		{
		TBool found = EFalse;
		for(TInt i = transdata->iSubControls.Count() - 1; (i >= 0) && !found; i--) 
			{
			if(transdata->iSubControls[i] == aParticipant) 
				{
				participantid = i + 1;
				found = ETrue;
				}
			}
		}
	
	if(participantid >= 0) 
		{
		//Find control drawer for this handle, send updated participant to it.
		CWsTransControlGraphic* drawer = FindDrawer(aHandle);
		if(drawer) 
			{
			RThread t;
			__ALFFXLOGSTRING2("Sending participant update for client:%d, handle:%d",t.Id().Id(),aHandle);
			drawer->SendParticipantUpdate(participantid, aCommandBuffer, aDrawRect, aBoundingRect);			
			}
		}
*/		
	}


// ---------------------------------------------------------------------------
// returns an array of supported control transitions
// ---------------------------------------------------------------------------
//
TControlPolicy* CGfxTransAdapterTfx::GetTransitionPolicies(TInt &aCount)
	{
	if( !iHasPlugin )
		{
		ConnectToServer();
		if ( !iHasPlugin )
		    {
    		return NULL;
		    }
		}
	//Start requesting policies
	RequestPolicyUpdates();

	//No policies at this point, all will be pushed.
	aCount = 0;
	return NULL;
	}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CGfxTransAdapterTfx::SubConBegin( const CCoeControl* /*aControl*/ )
    {
/*    
    if ( aControl )
        {
        for ( TInt i = 0; i < aControl->CountComponentControls(); i++ )
            {
            const CCoeControl* subcon = aControl->ComponentControl( i );
            if ( subcon && subcon->OwnsWindow() && subcon->Size() != TSize( 0, 0 )  && !GfxTransEffect::IsRegistered(subcon))
                {
                if ( iIgnoredWOChildControls.Find( subcon ) == KErrNotFound )
                    {
                    iSubCons++;
                    GfxTransEffect::Register( subcon, iActiveUid );
                    GfxTransEffect::Begin( subcon, iActiveAction );
                    }
                }
            iCurrZ++;
            SubConBegin( subcon );
            iCurrZ--;
            }
        }
*/        
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CGfxTransAdapterTfx::SubConEnd(const CCoeControl* /*aControl*/)
    {
/*    
    if ( aControl )
        {
        for ( TInt i = aControl->CountComponentControls() - 1; i >= 0 ; i-- )
            {
            CCoeControl* subcon = aControl->ComponentControl(i);
            SubConEnd( subcon );
            if ( subcon && subcon->OwnsWindow() &&
                 GfxTransEffect::IsRegistered( subcon ) )
                {
                if (subcon->IsVisible())
                    {
                    GfxTransEffect::SetBeginDemarcation(subcon, TRect(0, 0, 0, 0));
                    }
                else
                    {
                    GfxTransEffect::SetEndDemarcation(subcon, TRect(0, 0, 0, 0));
                    }
                GfxTransEffect::Invalidate(subcon);
                GfxTransEffect::End(subcon);
                if(subcon->IsVisible())
                    {
                    // make subcon temporarily invisible to avoid redrawing when deregistering
                    // redrawing will go to the remotegc of the main control but is bad for performance
                    // and causes Begin/End Redraw on the window which can cause flicker.
                    subcon->MakeVisible(EFalse);
                    GfxTransEffect::Deregister(subcon);
                    subcon->MakeVisible(ETrue);
                    }
                else 
                    {
                    GfxTransEffect::Deregister(subcon);
                    }
                iSubCons--;
                }
            }
        }
*/        
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CGfxTransAdapterTfx::SubConAbort( const CCoeControl* /*aControl*/ )
    {
/*    
    if ( aControl )
        {
        for ( TInt i = aControl->CountComponentControls() - 1; i >= 0; --i )
            {
            CCoeControl* subcon = aControl->ComponentControl( i );
            SubConAbort( subcon );
            if ( subcon && subcon->OwnsWindow() &&
                 GfxTransEffect::IsRegistered( subcon ) )
                {
                GfxTransEffect::Abort( subcon );
                }
            }
        }
*/        
    }

// ---------------------------------------------------------------------------
// calls to GfxTransEffect::NotifyExternalState comes through here.
// ---------------------------------------------------------------------------
//
void CGfxTransAdapterTfx::NotifyExternalState( TInt aState, const TDesC8* aArg )
	{
	switch(aState)
		{
		case ECaptureComponentsBegin:
        	if( !IsActive() )
                {
                return; //dont do anything if server is not running
                }
            break;                
		case ECaptureComponentsEnd:
        	if( !IsActive() )
                {
                return; //dont do anything if server is not running
                }
            break;                
        case ECaptureComponentsAbort:
        	if( !IsActive() )
                {
                return; //dont do anything if server is not running
                }
//			iTfxServer.AbortControlTransition();
			return;
		case ENotifyGlobalAbort:
//			iTfxServer.AbortControlTransition();
			return;
        case EBeginPopupSequence:
	    case ELastPopupInSequence:
	    case EEndPopupSequence:
		case EInternalHandleSequence:
		    break;
		case EGetRegistrationType:
		    {
		    // Not supported
	        TUid* t = (TUid*)(aArg);
	        *t = KNullUid;
	        break;
		    }
        case EAddIgnoreWOChildComponent:
        case ERemoveIgnoreWOChildComponent:
        default:
            //ignored
            break;
        }
    }

// ---------------------------------------------------------------------------
// called by client to start the specific transition
// ---------------------------------------------------------------------------
//
void CGfxTransAdapterTfx::StartTransition(TInt aHandle)
	{
	if( !iHasPlugin )
		{
		iClient->TransitionFinished( aHandle );
		return;
		}
		
	const CTransitionData* transdata;
	TInt err = iClient->GetTransitionData( aHandle, transdata );

	if( err == KErrNone ) 
		{
       __ALFFXLOGSTRING( "CGfxTransAdapterTfx::StartTransition called for disappear action. Do nothing." );
		}
		
    // always finish the caller		
    iClient->TransitionFinished( aHandle );
	}

// ---------------------------------------------------------------------------
// GfxTransEffect::BeginFullscreen gets passed here.
// ---------------------------------------------------------------------------
//
void CGfxTransAdapterTfx::BeginFullScreen(TUint /*aAction*/, const TRect& /*aEffectArea*/)
	{
	//This function will not be used.
	}

// ---------------------------------------------------------------------------
// GfxTransEffect::SecureIdFromAppUid
// ---------------------------------------------------------------------------
//
TSecureId CGfxTransAdapterTfx::SecureIdFromAppUid( TUid aAppUid )
    {
    TSecureId result(aAppUid);
    if ( aAppUid == TUid::Null() )
        {
        iCachedUidMapping.iWindowGroupId = -1;
        return result;
        }
    
    __ALFFXLOGSTRING1("CGfxTransAdapterTfx::SecureIdFromAppUid Searching SID&WGID for aAppUid: 0x%x", aAppUid.iUid );

    // first check the cache
    if ( iCachedUidMapping.iAppUid == aAppUid.iUid && iCachedUidMapping.iSecureId != 0 )
        {
        __ALFFXLOGSTRING2( "CGfxTransAdapterTfx::SecureIdFromAppUid using cached SID 0x%x and WGID: %d", 
                iCachedUidMapping.iSecureId,
                iCachedUidMapping.iWindowGroupId );
        return TSecureId(iCachedUidMapping.iSecureId);
        }

    TInt windowGroupId = -1;
    RProcess thisProcess;
    TUidType uidType = thisProcess.Type();
    
    if ( uidType.IsValid() )
        {
        bool thisApplication = false;
        if ( uidType[2] == aAppUid ) // 0 = UID1, 1 = UID2, 2 = UID3
            { 
            // this application
            result = thisProcess.SecureId();
            thisApplication = ETrue;
            __ALFFXLOGSTRING2("CGfxTransAdapterTfx::SecureIdFromAppUid Own SID 0x%x in thread %S", 
                    result.iId, 
                    &RThread().Name() );
            }
        
        CCoeEnv* env = CCoeEnv::Static();
        RWsSession localSession;
        RWsSession* usedWsSession = NULL;
        if ( env ) 
            {
            usedWsSession = &env->WsSession();
            }
        else 
            {
            if ( localSession.Connect() == KErrNone ) 
                {
                usedWsSession = &localSession;
                }
            }
        
        // check if the AppArcServer has already information about this Application
        if ( usedWsSession  ) 
            {
            TApaTaskList taskList( *usedWsSession ); 
            const TApaTask task = taskList.FindApp( aAppUid );
            const TThreadId otherThreadId = task.ThreadId();
            
            if ( thisApplication ) 
                {
                // if security ID of the other thread matched, we take the WG ID from AppArcServer
                RThread otherThread;
                if ( otherThread.Open( otherThreadId ) == KErrNone ) 
                    {
                    __ALFFXLOGSTRING1("CGfxTransAdapterTfx::SecureIdFromAppUid Exising TApaTask found using thread %S", &otherThread.Name() );
                    if ( otherThread.SecureId() == result )
                        {
                        windowGroupId = task.WgId();
                        __ALFFXLOGSTRING1("CGfxTransAdapterTfx::SecureIdFromAppUid SID match -> WGID : &d found from TApaTask", windowGroupId );
                        }
                    else 
                        {
                        __ALFFXLOGSTRING2("CGfxTransAdapterTfx::SecureIdFromAppUid otherSID 0x%x otherWGID : &d not matching in TApaTask", otherThread.SecureId().iId , task.WgId() );
                        }
                    }
                otherThread.Close();

                if ( windowGroupId == -1 && env )
                    {
                    // take the root WG ID
                    windowGroupId = env->RootWin().Identifier();
                    __ALFFXLOGSTRING1("CGfxTransAdapterTfx::SecureIdFromAppUid root WGID %d used", windowGroupId );
                    }
                }
            else  
                {
                RThread otherThread;
                if ( otherThread.Open( otherThreadId ) == KErrNone )
                    {
                    result = otherThread.SecureId();
                    windowGroupId = task.WgId();
                    __ALFFXLOGSTRING3("CGfxTransAdapterTfx::SecureIdFromAppUid Taking SID 0x%x WGID %d from thread %S via TApaTask", result.iId, windowGroupId, &otherThread.Name() );
                    }
                otherThread.Close();
                }
            }

        localSession.Close();
        }
    thisProcess.Close();
    
    if ( windowGroupId != -1 )
        {
        // update cache
        iCachedUidMapping.iAppUid = aAppUid.iUid;
        iCachedUidMapping.iSecureId = result;
        iCachedUidMapping.iWindowGroupId = windowGroupId;
        }

    return result;
    }

// ---------------------------------------------------------------------------
// GfxTransEffect::WindowGroupIdFromAppUid
// ---------------------------------------------------------------------------
//
TInt32 CGfxTransAdapterTfx::WindowGroupIdFromAppUid( TUid aAppUid )
    {
    TInt32 result = 0; 
    if ( iCachedUidMapping.iAppUid == aAppUid.iUid ) 
        {
        result = iCachedUidMapping.iWindowGroupId;
        }
    iCachedUidMapping.iSecureId = 0;
    return result;
    }


// ---------------------------------------------------------------------------
// GfxTransEffect::BeginFullscreen gets passed here.
// ---------------------------------------------------------------------------
//
void CGfxTransAdapterTfx::BeginFullScreen(TUint aAction, const TRect& aEffectArea, 
										 TUint aType, const TDesC8& aParams)
	{
    __ALFFXLOGSTRING1( "CGfxTransAdapterTfx::BeginFullScreen - %S >>", &RThread().Name());
    __ALFFXLOGSTRING1( "CGfxTransAdapterTfx::BeginFullScreen - iHasPlugin %d >>", iHasPlugin);
	if( !iHasPlugin )
		{
		ConnectToServer();
		if ( !iHasPlugin )
		    {
    		return;
		    }
		}
        
    TInt op = MAlfGfxEffectPlugin::EBeginFullscreen;
    
	TPtr8 inPtr = iTransferBuffer->Des();
	inPtr.Zero();
	TPtr8 outPtr = iReturnBuffer->Des();
	outPtr.Zero();
	RDesWriteStream inBuf( inPtr );
	
    // We won't leave here because our buffer is long enough
    // The buffer has been allocated in the constructor
    // If you add parameters, check the length of the buffer
    TRAP_IGNORE(
        {
        inBuf.WriteInt32L( op );
        inBuf.WriteUint32L( aAction );
        inBuf.WriteInt32L( aEffectArea.iTl.iX );
        inBuf.WriteInt32L( aEffectArea.iTl.iY );
        inBuf.WriteInt32L( aEffectArea.iBr.iX );
        inBuf.WriteInt32L( aEffectArea.iBr.iY );
        inBuf.WriteInt32L( aType );
	
        AknTransEffect::TParameter tmp;
        TPckgC<AknTransEffect::TParameter> params( tmp );
    	params.Set( aParams );
    	if ( aType == AknTransEffect::EParameterType )
    		{
            __ALFFXLOGSTRING3("CGfxTransAdapterTfx::BeginFullScreen - EParameterType, iNext.iUid: 0x%x, iPrev.iUid: 0x%x, iFlags: %d", params().iNext.iUid, params().iPrev.iUid, params().iFlags );
            inBuf.WriteInt32L( params().iNext.iUid );
            inBuf.WriteInt32L( params().iPrev.iUid );
            inBuf.WriteInt32L( params().iFlags );
            inBuf.WriteInt32L( SecureIdFromAppUid(params().iNext).iId );
            inBuf.WriteInt32L( WindowGroupIdFromAppUid(params().iNext) );
            inBuf.WriteInt32L( SecureIdFromAppUid(params().iPrev) );
            inBuf.WriteInt32L( WindowGroupIdFromAppUid(params().iPrev) );
    		}    
    	else if ( aType == AknTransEffect::EParameterAvkonInternal )
    	    {
       		const TUint* data = reinterpret_cast<const TUint*>( aParams.Ptr() );
            inBuf.WriteUint32L( data[0] );
            inBuf.WriteUint32L( data[1] );
            inBuf.WriteUint32L( data[2] );
            __ALFFXLOGSTRING3("CGfxTransAdapterTfx::BeginFullScreen - EParameterAvkonInternal, data[0-2] = [%d, %d, %d]", data[0], data[1], data[2]);
    	    }
    	else
    	    {
            inBuf.WriteUint32L( 0 );
            inBuf.WriteUint32L( 0 );
            inBuf.WriteUint32L( 0 );
    	    }

        inBuf.CommitL();
        });
        
    inBuf.Release();
    inBuf.Close();


    // we ignore any returned error as we are just doing our best...
    iTfxServer.SendSynchronousData( iPluginImplementation, inPtr, outPtr );

    // clear out used data    
	inPtr.Zero();
	outPtr.Zero();
	__ALFFXLOGSTRING("CGfxTransAdapterTfx::BeginFullScreen <<");
	}

// ---------------------------------------------------------------------------
// GfxTransEffect::EndFullscreen gets passed here.
// ---------------------------------------------------------------------------
//
void CGfxTransAdapterTfx::EndFullScreen()
	{
    __ALFFXLOGSTRING1( "CGfxTransAdapterTfx::EndFullScreen() - %S >>", &RThread().Name());
	if( !iHasPlugin )
		{
  		return;
		}
	
	TPtr8 inPtr = iTransferBuffer->Des();
	inPtr.Zero();
	TPtr8 outPtr = iReturnBuffer->Des();
	outPtr.Zero();
	RDesWriteStream inBuf( inPtr );
    TInt op = MAlfGfxEffectPlugin::EEndFullscreen;
    // We won't leave here because our buffer is long enough
    TRAP_IGNORE(
        {
    	// no other parameters used
        inBuf.WriteInt32L( op );
        inBuf.CommitL();
        });
    inBuf.Release();
    inBuf.Close();

    __ALFFXLOGSTRING( "CGfxTransAdapterTfx::EndFullScreen" );
    // we ignore any returned error as we are just doing our best...
    iTfxServer.SendSynchronousData( iPluginImplementation, inPtr, outPtr );
    // clear out used data    
	inPtr.Zero();
	outPtr.Zero();
        
	}

// ---------------------------------------------------------------------------
// GfxTransEffect::AbortFullscreen gets passed here.
// ---------------------------------------------------------------------------
//
void CGfxTransAdapterTfx::AbortFullScreen()
	{
    __ALFFXLOGSTRING1( "CGfxTransAdapterTfx::AbortFullScreen() - %S >>", &RThread().Name());
    if ( !iHasPlugin )
        {
        return;
        }
	
	TPtr8 inPtr = iTransferBuffer->Des();
	inPtr.Zero();
	TPtr8 outPtr = iReturnBuffer->Des();
	outPtr.Zero();
	RDesWriteStream inBuf( inPtr );
    TInt op = MAlfGfxEffectPlugin::EAbortFullscreen;
    // We won't leave here because our buffer is long enough
    TRAP_IGNORE(
        {
	    // no other parameters used
        inBuf.WriteInt32L( op );
        inBuf.CommitL();
        });
    inBuf.Release();
    inBuf.Close();

    // we ignore any returned error as we are just doing our best...
    iTfxServer.SendSynchronousData( iPluginImplementation, inPtr, outPtr );
    // clear out used data    
	inPtr.Zero();
	outPtr.Zero();
	__ALFFXLOGSTRING("CGfxTransAdapterTfx::AbortFullScreen <<");
	}


// ---------------------------------------------------------------------------
// implementation, handling transition startup
// ---------------------------------------------------------------------------
//
void CGfxTransAdapterTfx::DoStartTransitionL( TInt /*aHandle*/, const CTransitionData* aTransData )
	{
    // send the transition request to Alf Plugin
    __ALFFXLOGSTRING("CGfxTransAdapterTfx::DoStartTransitionL >>");
    if ( !aTransData->iKey->DrawableWindow() )
        {
        // If we don't have a window, we cannot do transition
        return;
        }

	TPtr8 inPtr = iTransferBuffer->Des();
	inPtr.Zero();
	TPtr8 outPtr = iReturnBuffer->Des();
	outPtr.Zero();
	RDesWriteStream inBuf( inPtr );
    TInt op = MAlfGfxEffectPlugin::EBeginComponentTransition;
    TInt windowGroup = aTransData->iKey->DrawableWindow()->WindowGroupId();
    TInt windowHandle = aTransData->iKey->DrawableWindow()->ClientHandle(); 
    
    // We don't send all the position information as we will do no screen capture
    // based on position, and the visuals know their own positions anyway.
    // If we find out that we need to specify some starting or ending positions,
    // we can add the information later.
    
    inBuf.WriteInt32L( op );
    inBuf.WriteUint32L( aTransData->iAction );
    inBuf.WriteInt32L( aTransData->iUid.iUid );
    inBuf.WriteInt32L( windowGroup ); // window group id of the control
    inBuf.WriteInt32L( windowHandle ); // server side window handle for the control
    inBuf.WriteInt32L( aTransData->iNLayered );
    // Participant data for the main layer. Other layers not handled separately.
    const CParticipantData* baseControl = aTransData->iBaseControl;        
    inBuf.WriteInt32L( baseControl->iLayerType.iUid ); // uid for this participant
    inBuf.WriteInt32L( baseControl->iFlags ); // hints 
    // Possible component control data missing - we assume handling of the whole control as one
    
    inBuf.CommitL();
    inBuf.Release();
    inBuf.Close();

    __ALFFXLOGSTRING( "CGfxTransAdapterTfx::DoStartTransitionL" );
    iTfxServer.SendSynchronousData( iPluginImplementation, inPtr, outPtr );
    // clear out used data    
	inPtr.Zero();
	outPtr.Zero();
	
	/*
	if ( iSubCons ) //Note: "normal" nesting wont work, but is not used in this platform.
		{
		if(iMainControl) 
			{
			//Collecting window owning sub controls, add to list.
			__ALFFXLOGSTRING2("Adding subcon %x, handle: %d",aTransData->iKey, aHandle);
	        CSubWOControl* subcon = CSubWOControl::NewL( aHandle, *aTransData );
			iSubWOControls.AppendL(subcon);
			return;
			}
		else
			{
			//Subcon Mismatch, abort transition and clean up.
			iClient->TransitionFinished(aHandle);	
			return;
			}
		}

	if(iControlInfo.Count() >= KMaxOngoing)
		{
		__ALFFXLOGSTRING("CGfxTransAdapterTfx::DoStartTransitionL maxongoing");
		iClient->TransitionFinished(aHandle);
		return;
		}
	
	TInt ap = iClientId.Id();
	__ALFFXLOGSTRING2("Adapter: Transition Started: %d, cli: %d",aHandle,ap);
	TRect boundingrect = TRect(aTransData->iBeginWOPos + aTransData->iBaseControl->iBeginRect.iTl, 
							   aTransData->iBeginWOPos + aTransData->iBaseControl->iBeginRect.iBr);
	boundingrect.BoundingRect(TRect(aTransData->iEndWOPos + aTransData->iBaseControl->iEndRect.iTl, 
									aTransData->iEndWOPos + aTransData->iBaseControl->iEndRect.iBr));
	CTransControlDrawerInfo* drawinfo = CTransControlDrawerInfo::NewL(aHandle, boundingrect);
	CleanupStack::PushL(drawinfo);
	User::LeaveIfError(iControlInfo.Append(drawinfo));
	CleanupStack::Pop(drawinfo);
	CWsTransControlGraphic* drawer = drawinfo->Drawer();

	__ALFFXLOGSTRING("Adapter: Sending participant 0");
	drawer->SendParticipant(aTransData->iBaseControl,0);
	TInt count = aTransData->iSubControls.Count(); 	
	for(TInt i = 0; i < count; i++)
		{
		__ALFFXLOGSTRING1("Adapter: Sending participant %d",i+1);
		drawer->SendParticipant(aTransData->iSubControls[i],i + 1);
		}

	TInt wocount = iSubWOControls.Count();
	if(wocount) 
		{
		drawinfo->SetHasWOParticipants(ETrue);
		}
	for(TInt i = wocount - 1; i >= 0; i--)
		{
		drawer->SendWOParticipant(*iSubWOControls[i]);
		}
	for(TInt i = 0; i < wocount; i++)
		{
		iClient->TransitionFinished(iSubWOControls[i]->Handle());
		}
	iSubWOControls.ResetAndDestroy();

	__ALFFXLOGSTRING("Adapter: Sending Transition Data");

	TBool drawerstart = EFalse;
	if(iIsWO && !aTransData->iKey->IsVisible())
		{
		drawerstart = ETrue;
		}

	drawer->SendTransitionData(aTransData, aHandle, drawerstart); 
	drawer->Flush();

    if(iIsWO && !aTransData->iKey->Parent())
        {
        // aTransData->iKey->DrawNow();
        if ( aTransData->iKey->IsVisible() && !aTransData->iKey->IsBackedUp() )
            {
            RWindow* window = ( RWindow* ) aTransData->iKey->DrawableWindow();
            window->BeginRedraw();
            window->EndRedraw();
            }
        }
    else
		{
		const CCoeControl* parent = aTransData->iKey->Parent();
		if(parent)
			{
//			TRect rect;
//			if(iIsWO)
//				{
//				rect = TRect(aTransData->iKey->PositionRelativeToScreen(),aTransData->iKey->Size());
//				rect.Move(-parent->PositionRelativeToScreen());
//				}
//			else
//				{
//				rect = aTransData->iBaseControl->iBeginRect;
//				}
				
			//Drawing problems with listboxes in component if
			//particle systems is used. We disable this rect for now
			//to resolve this. Shouldn't affect performance much.

            if ( !aTransData->iKey->IsVisible() )
                {
                parent->DrawNow();
                }
            }
        }
    ////
	iCoeEnv->WsSession().Flush();

	__ALFFXLOGSTRING("Adapter: Requesting finished event");
	if(KErrNone != iTfxServer.RequestFinishedEvent(aHandle))
		{
		iCurrHandle = 0;
		TransitionFinished(aHandle);
		return;
		}

	if(!drawerstart) 
		{
		if(KErrNone != iTfxServer.SendStartTick(iClientId,aHandle))
			{
			TransitionFinished(aHandle);
			}
		}
	iCurrHandle = 0;
*/	
	__ALFFXLOGSTRING("CGfxTransAdapterTfx::DoStartTransitionL <<");
	}

void CGfxTransAdapterTfx::GenerateComponentTransitionEventL(const CTransitionData* aTransData, TInt aOp, TInt aHandle)
    {

    // We generate a transition call from begin capture for control exit transitions
    TPtr8 inPtr = iTransferBuffer->Des();
    inPtr.Zero();
    TPtr8 outPtr = iReturnBuffer->Des();
    outPtr.Zero();
    RDesWriteStream inBuf( inPtr );
    TInt op = aOp;
    
    TInt windowGroup = KErrNotFound;
    TInt windowHandle = KErrNotFound;
    if( aHandle != KErrNotFound )
        {
        for(TInt i = 0 ; i < iControlEffects.Count(); i++ )
            {
            // we take the last one. to make hopefully clean up any earlier effect that was not finished for some reason.
            if( iControlEffects[i].iHandle == aHandle)
                {
                windowGroup = iControlEffects[i].iWindowGroup;
                windowHandle = iControlEffects[i].iWindowHandle;
                iControlEffects.Remove(i);
                i--;
                }
            }
        }
    if(aHandle != KErrNotFound && (windowGroup == KErrNotFound  || windowHandle == KErrNotFound))
        {
        return;
        }
    
    __ALFFXLOGSTRING1("CGfxTransAdapterTfx::GenerateComponentTransitionEventL - Operation: %d", op );

    __ALFFXLOGSTRING4("CGfxTransAdapterTfx::GenerateComponentTransitionEventL - Action: %d,  Uid: 0x%x, WindowGroup: %d, WindowHandle: %d  >>",
        aTransData->iAction,
        aTransData->iUid.iUid,
        windowGroup,
        windowHandle
        );
    
    inBuf.WriteInt32L( op );
    inBuf.WriteUint32L( aTransData->iAction );
    inBuf.WriteInt32L( aTransData->iUid.iUid );
    inBuf.WriteInt32L( windowGroup ); // window group id of the control
    inBuf.WriteInt32L( windowHandle ); // server side window handle for the control
    inBuf.WriteInt32L( aTransData->iNLayered );
    // Participant data not present in exit
    inBuf.WriteInt32L( 0 );
    inBuf.WriteInt32L( 0 ); // no hints

    inBuf.CommitL();
    inBuf.Release();
    inBuf.Close();

    __ALFFXLOGSTRING( "CGfxTransAdapterTfx::GenerateTransitionL - iTfxServer.SendSynchronousData " );
    iTfxServer.SendSynchronousData( iPluginImplementation, inPtr, outPtr );
    // clear out used data    
    inPtr.Zero();
    outPtr.Zero();
    
    // Don't signal client because client did not request the transition to start
    __ALFFXLOGSTRING("CGfxTransAdapterTfx::GenerateTransitionL <<");
    }	


// ---------------------------------------------------------------------------
// finds a control
// ---------------------------------------------------------------------------
//
//CTransControlDrawerInfo* CGfxTransAdapterTfx::FindControl(TInt /*aHandle*/)
/*	
	{
	for(TInt i = iControlInfo.Count() - 1; i >= 0; i--) 
		{
		if(iControlInfo[i]->Handle() == aHandle)
			{
			return iControlInfo[i];			
			}
		}
	return NULL;
	}
*/		

// ---------------------------------------------------------------------------
// finds a control drawer.
// ---------------------------------------------------------------------------
//
//CWsTransControlGraphic* CGfxTransAdapterTfx::FindDrawer(TInt /*aHandle*/)
/*	
	{
	for(TInt i = iControlInfo.Count() - 1; i >= 0; i--) 
		{
		if(iControlInfo[i]->Handle() == aHandle)
			{
			return iControlInfo[i]->Drawer();			
			}
		}
	return NULL;
	}
*/		

// ---------------------------------------------------------------------------
// callback called by server when a policy updates.
// ---------------------------------------------------------------------------
//
void CGfxTransAdapterTfx::PolicyUpdate(TControlPolicy* aPolicy)
	{
	iClient->TransitionPolicyUpdate(aPolicy);	
	}


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CGfxTransAdapterTfx::ClearAllPolicies()
	{
	iClient->ClearAllPolicies();	
	}
	
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TInt CGfxTransAdapterTfx::RequestPolicyUpdates()
    {
    __ALFFXLOGSTRING("CGfxTransAdapterTfx::RequestPolicyUpdates >>");
	if( iPolicyReq->IsActive() )
		{
		// request already ongoing
		return KErrNone;	
		}
		
    if ( !iHasPlugin )
        {
        return KErrDisconnected;
        }
		
    // send a request to plugin to start sending us policy updates
    // This is an asynchronous request
	iPolicyReq->ResetBufs();	

    *(iPolicyReq->InBuf())= new (ELeave) TPtr8(iAsyncTransferBuffer->Des());
    TPtr8& inPtr = **iPolicyReq->InBuf();
    inPtr.Zero();

    *(iPolicyReq->OutBuf())= new (ELeave) TPtr8(iAsyncReturnBuffer->Des());
    TPtr8& outPtr = **iPolicyReq->OutBuf();

    outPtr.Zero();
	RDesWriteStream inBuf( inPtr );
    TInt op = MAlfGfxEffectPlugin::ETfxServerOpControlPolicyRequest;
    // We won't leave here because our buffer is long enough
    TRAP_IGNORE(
        {
	    // no other parameters used
        inBuf.WriteInt32L( op );
        inBuf.CommitL();
        });
    inBuf.Release();
    inBuf.Close();

    __ALFFXLOGSTRING( "CGfxTransAdapterTfx::RequestPolicyUpdates" );
    iTfxServer.SendAsynchronousData( iPluginImplementation, inPtr, outPtr, iPolicyReq->iStatus );
    iPolicyReq->IssueReq();
    __ALFFXLOGSTRING("CGfxTransAdapterTfx::RequestPolicyUpdates <<");
    return KErrNone;
    
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CGfxTransAdapterTfx::GetNewPoliciesL( TUint aCount )
    {
    __ALFFXLOGSTRING1("CGfxTransAdapterTfx::GetNewPoliciesL %d >>", aCount);
    if ( !iHasPlugin )
        {
        User::Leave( KErrDisconnected );
        }
    // Send request to plugin  

	TInt err = KErrNone;
	
	TPtr8 inPtr = iTransferBuffer->Des();
	inPtr.Zero();
	
	TInt bufferSize = sizeof( TUint32 ) + ( sizeof( TInt32 ) * 2 + sizeof( TUint8 ) ) * aCount ;
	if ( iReturnBuffer->Des().MaxSize() < bufferSize )
	    {
	    delete iReturnBuffer;
	    iReturnBuffer = NULL;
        iReturnBuffer = HBufC8::NewL( bufferSize );
	    }
	    
	TPtr8 outPtr = iReturnBuffer->Des();
	outPtr.Zero();
	RDesWriteStream inBuf( inPtr );
    TInt op = MAlfGfxEffectPlugin::ETfxServerOpControlPolicyGet;
   	RThread me;
   	
    inBuf.WriteInt32L( op );
	TUint64 id = me.Id();
    TUint8* ptr = reinterpret_cast<TUint8*>( &id );
    inBuf.WriteL( ptr, 8 );
    inBuf.WriteInt32L( aCount );
    
    inBuf.CommitL();
    inBuf.Release();
    inBuf.Close();
    me.Close();

    if ( iHasPlugin )
        {
        __ALFFXLOGSTRING( "CGfxTransAdapterTfx::GetNewPoliciesL" );
        err = iTfxServer.SendSynchronousData( iPluginImplementation, inPtr, outPtr );
        }
    // This is a case where we are supposed to get data back        
    if ( err == KErrNone )
        {
        RDesReadStream instream( outPtr );
        CleanupClosePushL( instream );
        TUint count = instream.ReadUint32L();
        while ( count-- > 0 )
            {
            TInt uid = instream.ReadInt32L();
            TUint action = instream.ReadUint32L();
            TPolicy policyId = ( TPolicy ) instream.ReadUint8L();
            TControlPolicy* policy = new ( ELeave ) TControlPolicy();
            policy->iId.iUid = uid;
            policy->iAction = action;
            policy->iPolicy = policyId;
            iClient->TransitionPolicyUpdate( policy );
            }
        CleanupStack::PopAndDestroy(); // instream
        }
        
    // clear out used data    
	inPtr.Zero();
	outPtr.Zero();
	__ALFFXLOGSTRING("CGfxTransAdapterTfx::GetNewPoliciesL <<");
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CGfxTransAdapterTfx::CancelPolicyUpdates()
    {
    __ALFFXLOGSTRING("CGfxTransAdapterTfx::CancelPolicyUpdates >>");
    if ( !iHasPlugin )
        {
        // nothing to cancel
        return;
        }
        
    // send request to plugin
    // If the updates have not been subscribed, there is nothing to cancel either
    TPtr8 inPtr = iTransferBuffer->Des();
    inPtr.Zero();
    TPtr8 outPtr = iReturnBuffer->Des();
    outPtr.Zero();
	RDesWriteStream inBuf( inPtr );
    TInt op = MAlfGfxEffectPlugin::ETfxServerOpControlCancelPolicyUpdate;
   	RThread me;
    // We won't leave here because our buffer is long enough
    TRAP_IGNORE(
        {
	    // no other parameters used
        inBuf.WriteInt32L( op );
    	TUint64 id = me.Id();
	    TUint8* ptr = reinterpret_cast<TUint8*>( &id );
	    inBuf.WriteL( ptr, 8 );
        inBuf.CommitL();
        });
    inBuf.Release();
    inBuf.Close();
    me.Close();

    // not waiting for policies, nothing to cancel
	if( iPolicyReq->IsActive() )
		{
		// The cancel function will complete the request
        __ALFFXLOGSTRING( "CGfxTransAdapterTfx::CancelPolicyUpdates" );
        iTfxServer.SendSynchronousData( iPluginImplementation, inPtr, outPtr );
		}
		
    // clear out used data    
    inPtr.Zero();
    outPtr.Zero();
    __ALFFXLOGSTRING("CGfxTransAdapterTfx::CancelPolicyUpdates <<");
    }

// ------------------------------------------------------------
// SendGroupCommand
//-------------------------------------------------------------
//
void CGfxTransAdapterTfx::SendGroupCommand(TInt aGroupId, TBool aGroupFinished)
    {
    __ALFFXLOGSTRING("CGfxTransAdapterTfx::SendGroupCommand >>");
    if ( !iHasPlugin )
        {
        // nothing to cancel
        return;
        }
        
    // send request to plugin
    // If the updates have not been subscribed, there is nothing to cancel either
    TPtr8 inPtr = iTransferBuffer->Des();
    inPtr.Zero();
    TPtr8 outPtr = iReturnBuffer->Des();
    outPtr.Zero();
	RDesWriteStream inBuf( inPtr );
	
    TInt op;
    if ( aGroupFinished )
    	{
		op = MAlfGfxEffectPlugin::ETfxServerOpEndGroup;
    	}
    else
    	{
		op = MAlfGfxEffectPlugin::ETfxServerOpBeginGroup;
    	}
   	RThread me;
    // We won't leave here because our buffer is long enough
    TRAP_IGNORE(
        {
	    // no other parameters used
        inBuf.WriteInt32L( op );
        inBuf.WriteInt32L( aGroupId );
    	TUint64 id = me.Id();
	    TUint8* ptr = reinterpret_cast<TUint8*>( &id );
	    inBuf.WriteL( ptr, 8 );
        inBuf.CommitL();
        });
    inBuf.Release();
    inBuf.Close();
    me.Close();

    iTfxServer.SendSynchronousData( iPluginImplementation, inPtr, outPtr );
		
    // clear out used data    
    inPtr.Zero();
    outPtr.Zero();
    __ALFFXLOGSTRING("CGfxTransAdapterTfx::SendGroupCommand <<");
    }


// ======== GLOBAL FUNCTIONS ========

// ---------------------------------------------------------------------------
// DLL interface. used by the client to create the adapter
// ---------------------------------------------------------------------------
//
EXPORT_C MGfxTransAdapter* MGfxTransAdapter::CreateL(MGfxTransClient* aClient)
	{
	__ALFFXLOGSTRING("MGfxTransAdapter::CreateL >>");
	CGfxTransAdapterTfx* adapter = new (ELeave) CGfxTransAdapterTfx(aClient);
	CleanupStack::PushL(adapter);
	adapter->ConstructL();
	CleanupStack::Pop(adapter);
	__ALFFXLOGSTRING("MGfxTransAdapter::CreateL <<");
	return adapter;
	}

// ---------------------------------------------------------------------------
// DLL interface. used by the client to destroy the adapter
// ---------------------------------------------------------------------------
//
EXPORT_C void MGfxTransAdapter::Destroy(MGfxTransAdapter* aEngine)
	{
	delete static_cast<CGfxTransAdapterTfx*>(aEngine);
	}
