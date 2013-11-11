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
* Description:   Application Sever
*
*/



#include <coemain.h>
#include <coedef.h>
#include <apgwgnam.h>
#include <eiksvdef.h>
#include <ecom.h>
#include <uiacceltk/HuiUtil.h>
#include <f32file.h>
#include <TfxSrvEcomIds.h>
#include <mtransitionserver.h>
#include "alf/alfappserver.h"
#include "alf/alfappui.h"
#include "alf/alfconstants.h"
#include "alfappsrvsession.h"
#include "alflogger.h"
#include "alfsrvsubsessionbase.h"
#include "alf/alfextensionfactory.h"
#include "alfstaticfactory.h"
#include "alfsrvtranseffect.h"
#include "alf/AlfTransEffectPlugin.h"
#include <uiacceltk/HuiDisplay.h> //@todo::
#include "alfclientserver.h"
#include "alfsrvtexturemanager.h"
#include "alfsrvscreenbuffermanager.h"

// ----------------------------

/**
 * Common command batch buffer size. This must always be larger than command 
 * header size. In practice, this should be always larger than 128.
 */
const TInt KAlfCommonCommandBatchBufferSize = 
    KAlfClientServerDefaultCommandBatchBufferSize;

// a kind of cleanup item for ecom info array
NONSHARABLE_CLASS(CCleanupEcomArray):public CBase
    {
    public:
    ~CCleanupEcomArray()
        {
        iArray.ResetAndDestroy();
        iArray.Close();
        }
    RImplInfoPtrArray iArray;
    };


// ECom notify helper.
NONSHARABLE_CLASS(CAlfEcomUtil): public CActive
    {
    public:    
    
    CAlfEcomUtil(CAlfAppServer& aServer)
    :CActive(EActivePriorityDefault), iOwner(aServer)
	    {
	    CActiveScheduler::Add(this);
	    }

    void ConstructL()
	    {
	    iEComSession = &REComSession::OpenL();
    	}

    ~CAlfEcomUtil()
	    {
	    Cancel();
	    if (iEComSession)
	        {
	        iEComSession->Close();
	        }
	    iEComSession = NULL;
	    }

    static CAlfEcomUtil* NewL(CAlfAppServer& aServer)
	    {
	    CAlfEcomUtil* self = new (ELeave) CAlfEcomUtil(aServer);
	    CleanupStack::PushL(self);
	    self->ConstructL();
	    CleanupStack::Pop(self);
	    return self;
	    }

    void Start()
	    {
	    if (!IsActive())
	        {
    	    iEComSession->NotifyOnChange(iStatus);
	        SetActive();
	        }
	    }

    void RunL()
	    {
	    // if there was an error, just breathe a moment, i.e. don't activate new request immediately
        if (iStatus.Int() == KErrNone )
            {
    	    //Check wether plugin was installed or removed
     	    TRAP_IGNORE(iOwner.CheckForEcomPluginInstallUninstallL())
    	    Start();
            }
        __ALFLOGSTRING1( "CAlfEcomUtil::RunL: status %d", iStatus.Int() )
	    }

    void DoCancel()
	    {
	    iEComSession->CancelNotifyOnChange(iStatus);
	    }
	
	private:    
        CAlfAppServer& iOwner;
        REComSession* iEComSession;
    };
    

// ======== MEMBER FUNCTIONS ========


EXPORT_C CAlfAppServer* CAlfAppServer::NewAppServerL()
    {
    return new (ELeave) CAlfAppServer();
    }


// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CAlfAppServer::CAlfAppServer():
      iLastActiveClientWg(KErrNotFound)
    {
    CCoeEnv* coe = CCoeEnv::Static();
    if (coe)
        {
        iWs = &coe->WsSession();
        }
    }

// ---------------------------------------------------------------------------
// Second phase constructor
// ---------------------------------------------------------------------------
//
void CAlfAppServer::ConstructL( const TDesC& aFixedServerName )
    {
    CAknAppServer::ConstructL( aFixedServerName );

    iCommonCommandBatchBuffer = HBufC8::NewL( KAlfCommonCommandBatchBufferSize );
    iTextureManager = CAlfSrvTextureManager::NewL();
    iScreenBufferManager = CAlfSrvScreenBufferManager::NewL(*this);
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CAlfAppServer::~CAlfAppServer()
    {
	#ifdef TFXSERVER_API_V2
    if (iTfxServer)
    	{
        iTfxServer->Disconnect();
    	delete iTfxServer;
    	iTfxServer = NULL;
    	REComSession::DestroyedImplementation( iTfxServerEComDestructorUID );
    	iTfxServerEComDestructorUID.iUid = 0;	
    	}
	#endif

    delete iBackgroundTimer;
    delete iTransitionEffects;
    
    delete iEcomWatcher;
    
    iWindowChangeObservers.Close();
    
    if (iAsynchOneShot)
        {
        iAsynchOneShot->Cancel();
        }
    
    delete iAsynchOneShot;
    iRemovedArray.Close();
    
    delete iObjectConIx;

    for (TInt ii = iCustomSessions.Count()-1; ii >= 0; ii--)
        {
        delete iCustomSessions[ii].iImpl;
        REComSession::DestroyedImplementation(
                iCustomSessions[ii].iDestructorUid);
        }
        
    iCustomSessions.Close();
    
    // Todo: check remaining implementations
    for (TInt ii = iFactories.Count()-1; ii >= 0; ii--)
        {
        iFactories[ii].iImpl->Release();
        if (iFactories[ii].iDestructorUid.iUid)
            {
            REComSession::DestroyedImplementation(
                iFactories[ii].iDestructorUid);
            }
        }

    iFactories.Close();
        
    iMetricsInterface = NULL;
    iAppUi= NULL;

    delete iCommonCommandBatchBuffer;
	delete iScreenBufferManager;
    delete iTextureManager;
    }

// ---------------------------------------------------------------------------
// Called when client application exist.
// All allocated resources for that application should be freed.
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfAppServer::HandleClientExit(TInt aClientId)
    {
	if (MetricsInterface())
	    {
		MetricsInterface()->SessionTerminated(aClientId);
		}
  
    SyncEcomImplementations(aClientId);  
    
    CAlfAppSrvSessionBase* session = reinterpret_cast<CAlfAppSrvSessionBase*>(aClientId);
    
    ScreenBufferManager().RemoveScreenBufferObservers(session);
    
    if (session == iOldSession)
        {
        iOldSession = 0;
        }
    
    if (iAppUi)
        {
        iAppUi->FreeResourcesForClient(aClientId);
        }
    else 
        {
        return;
        }
        
    for (TInt i = iWindowChangeObservers.Count()-1; i >= 0 ; i--)
        {
        if (iWindowChangeObservers[i].iId == aClientId)
            {
            iWindowChangeObservers.Remove(i);
            }
        }
        
    // todo, is it true that we have no other way to check the count than using iterator?
    TInt count = 0;
    iSessionIter.SetToFirst();
    for(;iSessionIter;iSessionIter++)
        {
        count++;
        }
    
    // if this was the last client, it is ok to bail out here    
    if (count == 1)
        {
        AppUi()->UpdateActiveSession( 0 );
        AdjustWindowGroupPositionL(*CHuiStatic::RootWin(0),0,EAbsoluteBackground);
        return;
        }

    // Set wg position based in the topmost client app
    TInt parentWg = KErrNotFound;    
    CAlfAppSrvSessionBase* dying = reinterpret_cast<CAlfAppSrvSessionBase*>(aClientId);

    if ( dying->ClientWindowGroup() == KErrNotFound )
        {
        // debugging trash
        // just bail out w/o changing the order of window groups
        return;       
        }
    
    TBool lastActiveDying = EFalse;
    if ( dying->ClientWindowGroup() == iLastActiveClientWg )
        {
        lastActiveDying = ETrue;
        iLastActiveClientWg = KErrNotFound;
        parentWg = dying->ParentWindowGroupId();
        }

    CAlfAppSrvSessionBase* newFocusSession = 0;

    // if parent of embedded app found, check if it ALF app and set it to focus
    if (parentWg != KErrNotFound)
        {
        iSessionIter.SetToFirst();
        while (iSessionIter)
            {
            CAlfAppSrvSessionBase* session = reinterpret_cast<CAlfAppSrvSessionBase*>(iSessionIter++);
            if ( session && session->ClientWindowGroup() == parentWg)
                {
                newFocusSession = session;
                break;
                }
            }
        }
        
    if (CCoeEnv::Static() && !newFocusSession) // else find topmost app and move us behind that
        {
        parentWg = KErrNotFound; // just in case app had parent but that was not ALF app
        CArrayFixFlat<TInt>* wgs = new CArrayFixFlat<TInt>(4);
        if ( wgs )
            {
            iWs->WindowGroupList(wgs);
            for ( TInt ordinal = 0 ; ordinal < wgs->Count(); ordinal++ )
                {
                iSessionIter.SetToFirst();
                if( newFocusSession ) // session found from previous ordinal position.
                    {
                    break;
                    }
                while (iSessionIter)
                    {
                    CAlfAppSrvSessionBase* session = reinterpret_cast<CAlfAppSrvSessionBase*>(iSessionIter++);
                    if ( session && 
                         session->ClientWindowGroup() == wgs->At( ordinal ) &&
                         (TInt)session != aClientId )
                        {
                        newFocusSession = session;
                        break;
                        }
                    }
                }
            }
        delete wgs;
        }
    if( lastActiveDying || newFocusSession )
        {
        TRAP_IGNORE(FocusedWindowGroupChangedL(newFocusSession, parentWg));
        }
    }


// ---------------------------------------------------------------------------
// Static utility used in callback
// ---------------------------------------------------------------------------
// 
TInt SynchEcomImplementations(TAny* aThis)
    {
    ((CAlfAppServer*)aThis)->DoSynchEcomImplementations();
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// Check if the exiting client is using ecom based session
// if so, set asynchronous callback for notifying Ecom FW about the change
// ---------------------------------------------------------------------------
// 
void CAlfAppServer::SyncEcomImplementations(TInt aClientId)  
    {
    TBool isEcom(EFalse);
    for(TInt ii = iCustomSessions.Count()-1; ii >= 0 ; ii--)
        {
        if (iCustomSessions[ii] == aClientId)
            {
            isEcom = ETrue;
            iCustomSessions[ii].iImpl = 0; // session is about to terminate, null the pointer
            break;
            }
        }
    if (isEcom)
        {
        iRemovedArray.Append(aClientId);
        if (iAsynchOneShot && !iAsynchOneShot->IsActive())
            {
            iAsynchOneShot->CallBack();
            }
        
        }
    }

// ---------------------------------------------------------------------------
// Check and notify Ecom FW about removed implementations caused by client exit
// Called asynchronously after a client usin ecom based session has exited.
// ---------------------------------------------------------------------------
// 
void CAlfAppServer::DoSynchEcomImplementations()
    {
    // check if the session was ECOM based and notify FW if so
    for (TInt i = iRemovedArray.Count()-1; i >= 0; i--)
        {
        for(TInt ii = iCustomSessions.Count()-1; ii >= 0 ; ii--)
            {
            if (iCustomSessions[ii] == iRemovedArray[i])
                {
                REComSession::DestroyedImplementation(
                    iCustomSessions[ii].iDestructorUid);            
                iCustomSessions.Remove(ii);
                }
            }
        
        }
    iRemovedArray.Reset();
    User::Heap().Compress();
    }
    
// ---------------------------------------------------------------------------
// From class CAknAppServer.
// Called when all clients have existed.
// ---------------------------------------------------------------------------
// 
void CAlfAppServer::HandleAllClientsClosed()
    {
    if (iAppUi)
        iAppUi->AllClientsClosed();
    }
        
// ---------------------------------------------------------------------------
// From class CAknAppServer.
// Allocates the session instance
// ---------------------------------------------------------------------------
//
CApaAppServiceBase* CAlfAppServer::CreateServiceL( TUid aServiceType ) const
    {
    CApaAppServiceBase* session = 0;
    if (aServiceType == KAlfServiceUid)   
		{
		session = CAlfAppSrvSession::NewL(this);
		}
    else
	    { // nasty const cast
    	session = ((CAlfAppServer*)this)->LoadServiceExtensionL(aServiceType);		    
	    }
	
	if(session)	
	    {
		if (MetricsInterface())
		    {
		    const_cast<MAlfMetricsInterface*>(MetricsInterface())->SessionCreated(reinterpret_cast<TInt>(session));
		    }
		    
	    return session;
	    }
	else
		{
		return CAknAppServer::CreateServiceL(aServiceType);	
		}
    }
    
// ---------------------------------------------------------------------------
// Adjusts server's wg according to client's wg.
// ---------------------------------------------------------------------------
// 
EXPORT_C void CAlfAppServer::AdjustWindowGroupPositionL(
    RWindowGroup& aWindowGroup, 
    TInt aParentIdentifier, 
    TAlfWGPostion aPosition)
    {
    if (!iAppUi )
        {
        return;  
        }

    // Update the member variable based on the WG the server follows.
    if (aPosition != EAbsoluteBackground)
        {
        iLastActiveClientWg = aParentIdentifier;
        }
        
    if (!CCoeEnv::Static())
        {
        iAppUi->AdjustWindowGroupPositionL(
                    aParentIdentifier, 
                    aPosition);
        return;
        }
        
#ifdef _ALF_LOGGING
    LogWindowGroupsL();
    switch( aPosition )
        {
        case EBehindOfParent:
            __ALFLOGSTRING1( "CAlfAppServer::AdjWinGrpPosL EBehindOfParent parent id %d", aParentIdentifier )
            break;
        case EOnTopOfParent:
            __ALFLOGSTRING1( "CAlfAppServer::AdjWinGrpPosL EOnTopOfParent parent id %d", aParentIdentifier )
            break;
        case EAbsoluteBackground:
            __ALFLOGSTRING1( "CAlfAppServer::AdjWinGrpPosL EAbsoluteBackground parent id %d", aParentIdentifier )
            break;
        default:
            __ALFLOGSTRING2( "CAlfAppServer::AdjWinGrpPosL UNKNOWN parent id %d pos %d", aParentIdentifier, aPosition )
            break;
        }
#endif
    
    if (aPosition == EAbsoluteBackground) // just for convenience
        {
        aWindowGroup.SetOrdinalPosition(0,ECoeWinPriorityNeverAtFront);
        LogWindowGroupsL();
        return;
        }
            
    TInt parentPriority = iWs->GetWindowGroupOrdinalPriority(aParentIdentifier);

    // perhaps we should maintain wg-list elsewhere
    CArrayFixFlat<TInt>* wgs = new (ELeave) CArrayFixFlat<TInt>(1); 
    CleanupStack::PushL(wgs);
    iWs->WindowGroupList(parentPriority,wgs);
    
    TInt pos = KErrNotFound;
    TInt movingWgOldPos = KErrNotFound;
    TInt wgCount = wgs->Count();
    for (TInt i = 0; i < wgCount; i++)
        {
        if (aParentIdentifier == wgs->At(i))
            {
            if ( aPosition == EOnTopOfParent )
                {
                pos = i;
                }
            else
                {
                pos = i+1;
                }
            }
        if ( aWindowGroup.WindowGroupId() == wgs->At(i))
            {
            movingWgOldPos = i;
            }
           
        if ( pos != KErrNotFound && movingWgOldPos != KErrNotFound )
            {
            // Both found already.
            break;
            }
        }
    
    // If the moving window group has already been before the parent
    // we need to adjust the new position
    if ( movingWgOldPos < pos && movingWgOldPos != KErrNotFound )
        {
        pos--;
        }


    User::LeaveIfError(pos); // parenty not found, leave
    
    // if iBackDropWgIdentifier has been set, status pane is partly drawn to 
    // back drop window group
    if (aPosition == EBehindOfParent && 
        iBackDropWgIdentifier 
        && parentPriority == ECoeWinPriorityNormal ) // only apps with normal priority should use status pane
        {
        for (TInt ii = pos; ii < wgCount; ii++)
            {
            if (wgs->At(ii) == iBackDropWgIdentifier)
                {
                pos = ii+1; // right behind status pane
                break;
                }
            }    
        
        }
    CleanupStack::PopAndDestroy(wgs);
    
    aWindowGroup.SetOrdinalPosition(pos, parentPriority);
    
    LogWindowGroupsL();
    }
   
// ---------------------------------------------------------------------------
// Returns server's window server session.
// ---------------------------------------------------------------------------
// 
EXPORT_C RWsSession& CAlfAppServer::WsSession() const
    {
    return *iWs;
    }

// ---------------------------------------------------------------------------
// Returns the metrics interface.
// ---------------------------------------------------------------------------
// 
EXPORT_C MAlfMetricsInterface* CAlfAppServer::MetricsInterface()
    {
    return iMetricsInterface;
    }
    
// ---------------------------------------------------------------------------
// Returns the metrics interface.
// ---------------------------------------------------------------------------
// 
EXPORT_C const MAlfMetricsInterface* CAlfAppServer::MetricsInterface() const
    {
    return iMetricsInterface;
    }

// ---------------------------------------------------------------------------
// Sets the metrics interface.
// ---------------------------------------------------------------------------
// 
EXPORT_C void CAlfAppServer::SetMetricsInterface(
    MAlfMetricsInterface* aMetricsInterface)
    {
    iMetricsInterface = aMetricsInterface;
    }

// ---------------------------------------------------------------------------
// Sets the AppUi.
// ---------------------------------------------------------------------------
// 
void CAlfAppServer::SetAppUi(CAlfAppUi* aAppUi)
    {
    iAppUi = aAppUi;
    iWs = &CHuiStatic::WsSession();
    }
    
// ---------------------------------------------------------------------------
// Returns the AppUi pointer.
// ---------------------------------------------------------------------------
// 
EXPORT_C CAlfAppUi* CAlfAppServer::AppUi()
    {
    __ASSERT_DEBUG(iAppUi, USER_INVARIANT());
    return iAppUi;
    }
   
// ---------------------------------------------------------------------------
// Called when the focused window group changes.
// ---------------------------------------------------------------------------
//     
void CAlfAppServer::FocusedWindowGroupChangedL( CAlfAppSrvSessionBase* aSession, TInt aWgId)
    {
    iOldSession = NULL;
    CancelBackgroundTimer();

    // Find new and old focused clients
    CAlfAppSrvSessionBase* newFocusedSession = aSession;
    CAlfAppSrvSessionBase* oldFocusedSession = NULL;
    
    iSessionIter.SetToFirst();
    while (iSessionIter)
        {
        CAlfAppSrvSessionBase* session =
            reinterpret_cast<CAlfAppSrvSessionBase*>(iSessionIter++);

        if (  session && 
              !oldFocusedSession && 
              iLastActiveClientWg != KErrNotFound &&
              session->ClientWindowGroup() == iLastActiveClientWg )
            {
            oldFocusedSession = session;
            }
            
        // If found, stop the search. 
        if ( oldFocusedSession || iLastActiveClientWg == KErrNotFound )
            {
            break;
            }
        // Otherwise, keep going...
        }
        
    if ( !newFocusedSession )
        {
        // Another application got the focus
        // let the server follow the previous app wg
        if ( oldFocusedSession )
            {
            oldFocusedSession->ResetRootlayerTransformationsL();
            oldFocusedSession->FocusLostL( EFalse );
            AdjustWindowGroupPositionL(
                        *CHuiStatic::RootWin(),
                        oldFocusedSession->ClientWindowGroup(), 
                        oldFocusedSession->PreferredWindowGroupPosition());        
            }
        else // this will be the case when there is only direct client connections left
            {
            AdjustWindowGroupPositionL(*CHuiStatic::RootWin(0),0,EAbsoluteBackground);
            }
        AppUi()->UpdateActiveSession( 0 );
        AppUi()->SetContainerNonFading( EFalse );
        }
    else
        {
        // One of our clients got the focus
        AdjustWindowGroupPositionL( 
                *CHuiStatic::RootWin(), 
                newFocusedSession->ClientWindowGroup(), 
                newFocusedSession->PreferredWindowGroupPosition() );
#ifdef SYMBIAN_BUILD_GCE    
        newFocusedSession->ActivateContainerLayoutL(ETrue);
#endif // #ifdef SYMBIAN_BUILD_GCE    

        TBool didFadeOutTransition = EFalse; // default, just make sure that visual trees are restored
        if ( oldFocusedSession && newFocusedSession != oldFocusedSession) // switching between apps
            {
            if ((oldFocusedSession->ClientWindowGroup() == newFocusedSession->ParentWindowGroupId())
                || (newFocusedSession->ClientWindowGroup() == oldFocusedSession->ParentWindowGroupId() ))
                { // swithching into embedded app
                didFadeOutTransition = oldFocusedSession->FocusLostL( MAlfTransEffectPlugin::EContextEmbeddedExit );
                if (didFadeOutTransition)
                    {
                    didFadeOutTransition = MAlfTransEffectPlugin::EContextEmbeddedStart;
                    }
                }
            else // swithching between separate apps
                {
                didFadeOutTransition = oldFocusedSession->FocusLostL( MAlfTransEffectPlugin::EContextFocusLost );
                }    
            }
        else if (!oldFocusedSession ) // New app is being launched
            {                         // or previous app destroyed  
            if (aWgId == KErrNotFound)
                {
                didFadeOutTransition = MAlfTransEffectPlugin::EContextFocusGained; // enable default effect
                }
            else
                {
                didFadeOutTransition = MAlfTransEffectPlugin::EContextEmbeddedStart; // enable default effect
                }
            }
        else
            {
            // Fot PC-lint
            }
        newFocusedSession->FocusGainedL( didFadeOutTransition );
        }
#ifdef SYMBIAN_BUILD_GCE
/*
#ifdef HUI_FX
*/
    // Should test if transition is running
    // If transitions are disabled, no delay is needed
    if ( oldFocusedSession && oldFocusedSession != newFocusedSession )
        {
        if ( !newFocusedSession )
            {
            iOldSession = oldFocusedSession;
            StartBackgroundTimerL(iLastActiveClientWg);
            }
        else
            {
            oldFocusedSession->ActivateContainerLayoutL(EFalse);
            }
        }
/*        
#else  
    if (oldFocusedSession && oldFocusedSession != newFocusedSession )
        oldFocusedSession->ActivateContainerLayoutL(EFalse);
#endif    
*/
#endif // #ifdef SYMBIAN_BUILD_GCE    
    }


// ---------------------------------------------------------------------------
// Notify window visibility observers about change
// ---------------------------------------------------------------------------
// 
void CAlfAppServer::WindowVisiblityChangedL( 
    TUint aVisibilityFlags, 
    CCoeControl* aDestination )
    {
    for (TInt i = 0; i < iWindowChangeObservers.Count(); i++)
        {
        if (iWindowChangeObservers[i].iPtr->CoeControl() == aDestination)
            {
            if ( aVisibilityFlags & 
                 TWsVisibilityChangedEvent::EPartiallyVisible )
                {
                iWindowChangeObservers[i].iPtr->WindowPartiallyVisible();
                }
            else if ( aVisibilityFlags & 
                      TWsVisibilityChangedEvent::EFullyVisible )
                {
                iWindowChangeObservers[i].iPtr->WindowFullyVisible();
                }
            else if ( aVisibilityFlags & 
                      TWsVisibilityChangedEvent::ENotVisible )
                {
                iWindowChangeObservers[i].iPtr->WindowNotVisible();
                }
            else
                {
                // For PC-lint
                }
            
            break;
            }
        }
    }
    
// ---------------------------------------------------------------------------
// Loads a session extension.
// ---------------------------------------------------------------------------
// 
CAlfAppSrvSessionBase* CAlfAppServer::LoadServiceExtensionL(TUid aServiceType)
    {
    CAlfAppSrvSessionBase* newSession = 0;
    // check if lib already loaded
    if (ImplementationExists(aServiceType)) // no need to scan
        {
        newSession = NewImplementationL(aServiceType);       
        }
    else
        {
        // perform plugin scan
        CCleanupEcomArray* ecomInfoArray = new (ELeave) CCleanupEcomArray;
        CleanupStack::PushL(ecomInfoArray);
        REComSession::ListImplementationsL(TUid::Uid(K_ALF_SERVICE_INTERFACE), ecomInfoArray->iArray);
        // check if plugin with implementation uid matches 
        TInt count = ecomInfoArray->iArray.Count();
        for (TInt ii = count-1; ii >= 0; ii-- )
            {
            if ((ecomInfoArray->iArray[ii])->ImplementationUid() 
                    == aServiceType)
                {
                newSession = NewImplementationL(aServiceType);       
                break;
                }
            }
        CleanupStack::PopAndDestroy();    
        }

    if (newSession && !iAsynchOneShot)
        { // instantiate remove callback here if it does not exist yet
        iAsynchOneShot = 
            new (ELeave) CAsyncCallBack(
                TCallBack(SynchEcomImplementations, this), 
                EActivePriorityDefault);
        }

    
    return newSession;
    }

// ---------------------------------------------------------------------------
// Checks if implementation of this type has been loaded already, 
// no need to perform scan if this is the case 
// ---------------------------------------------------------------------------
// 
TBool CAlfAppServer::ImplementationExists(TUid aServiceType) const
    {
    for(TInt ii = iCustomSessions.Count()-1; ii >= 0; ii--)
        {
        if (aServiceType==iCustomSessions[ii].iServiceUid)
            {
            return ETrue;
            }
        }

    return EFalse;
    }

// ---------------------------------------------------------------------------
// Loads and instantiates actual implementation using ECOM 
// ---------------------------------------------------------------------------
// 
CAlfAppSrvSessionBase* CAlfAppServer::NewImplementationL(TUid aServiceType)
    {
    TServiceExtension ext;
    ext.iServiceUid = aServiceType;
    ext.iImpl = 
        (CAlfAppSrvSessionBase*)REComSession::CreateImplementationL(
            aServiceType, 
            ext.iDestructorUid, 
            this);
    iCustomSessions.AppendL(ext);

    // set notify for changes in plugins (if it does not exist already) 
    if (!iEcomWatcher)
        {
        iEcomWatcher = CAlfEcomUtil::NewL(*this);
        iEcomWatcher->Start();
        }
  
    return ext.iImpl;
    }

// ---------------------------------------------------------------------------
// Something has changed in ECom, check if we are affected
// ---------------------------------------------------------------------------
// 
void CAlfAppServer::CheckForEcomPluginInstallUninstallL()
    {
    // cross-check services loaded
    TInt csCount = iCustomSessions.Count();
    if (csCount)
        {
        CCleanupEcomArray* ecomInfoArray = new (ELeave) CCleanupEcomArray;
        CleanupStack::PushL(ecomInfoArray);
        REComSession::ListImplementationsL(TUid::Uid(K_ALF_SERVICE_INTERFACE), 
                                            ecomInfoArray->iArray);
        // check if plugin with implementation uid matches 
        TInt count = ecomInfoArray->iArray.Count();
        while (csCount > 0)
            {
            csCount--;
            TUid csUid = iCustomSessions[csCount].iServiceUid;
            TBool found(EFalse);
            for (TInt ii = count-1; ii >= 0; ii-- )
                {
                if ((ecomInfoArray->iArray[ii])->ImplementationUid() 
                    == csUid)
                    {
                    found = ETrue;       
                    break;
                    }
                }
            if (!found)
                { // notifies ECom about destroyed implementation
                delete iCustomSessions[csCount].iImpl;
                }
            }
            
        CleanupStack::PopAndDestroy();            
        }

    TInt factoryCount = iFactories.Count();
    if (factoryCount)
        {
        CCleanupEcomArray* ecomInfoArray = new (ELeave) CCleanupEcomArray;
        CleanupStack::PushL(ecomInfoArray);
        REComSession::ListImplementationsL(TUid::Uid( K_ALF_FACTORY_INTERFACE), 
                                            ecomInfoArray->iArray);
        // check if plugin with implementation uid matches 
        TInt count = ecomInfoArray->iArray.Count();
        while (factoryCount > 0 )
            {
            factoryCount--;
            TUid fUid = iFactories[factoryCount].iUid;
            if (fUid == KNullUid) // fixed factory
                {
                continue;                
                }
                
            TBool found(EFalse);
            for (TInt ii = count-1; ii >= 0; ii-- )
                {
                if ((ecomInfoArray->iArray[ii])->ImplementationUid() 
                    == fUid)
                    {
                    found = ETrue;       
                    break;
                    }
                }
            if (!found)
                { // notifies ECom about destroyed implementation
                NotifySessionsAboutDestroyedImplementation(iFactories[factoryCount].iUid.iUid);
                iFactories[factoryCount].iImpl->Release();
                }
            }
            
        CleanupStack::PopAndDestroy();            
        }

    if (iCustomSessions.Count() == 0 && 
        (iFactories.Count() == 0 || (iFactories.Count() == 1 && iFactories[0].iUid == KNullUid) ) )
        { // no active extensions -> cancel notify
        delete iEcomWatcher;
        iEcomWatcher = 0;    
        }
    
    }

// ---------------------------------------------------------------------------
// Appends new window visibility observer
// ---------------------------------------------------------------------------
// 
EXPORT_C void CAlfAppServer::SetWindowChangeObserverL(
    MWindowVisibilityObserver* aObserver, TInt aClientId) 
    {
    iWindowChangeObservers.AppendL(TWindowChangeObserver(aObserver,aClientId));
    }
    
TBool IsNewStatusPaneArchL()
    {
    _LIT(KS60ProductIDFile, "Series60v*.sis");
    _LIT(KROMInstallDir, "z:\\system\\install\\");

    TFindFile ff( CHuiStatic::FsSession());
    CDir* result;
    User::LeaveIfError( ff.FindWildByDir( KS60ProductIDFile, KROMInstallDir, result ) );
    CleanupStack::PushL( result );
    User::LeaveIfError( result->Sort( ESortByName|EDescending ) );

    TInt major = (*result)[0].iName[9] - '0';
    TInt minor = (*result)[0].iName[11] - '0';

    CleanupStack::PopAndDestroy(); // result
    return ((major>3) || (major == 3 && minor >= 2));
    }

void ResolveBdWgIdL(TInt& aId, RWsSession& aWs, CAlfAppUi& /*aAppUi*/)
    {
    if (!IsNewStatusPaneArchL())
        {
        CArrayFixFlat<TInt>* wgs = new (ELeave) CArrayFixFlat<TInt>(1); 
        CleanupStack::PushL(wgs);
        aWs.WindowGroupList(ECoeWinPriorityNormal,wgs);
        TInt wgCount = wgs->Count();

        CApaWindowGroupName* wgName = CApaWindowGroupName::NewLC(aWs); 
        HBufC* buf = HBufC::NewL(CApaWindowGroupName::EMaxLength);
        wgName->SetWindowGroupName(buf);

        for ( TInt spPos = 0; spPos < wgCount; spPos++)
            {
            TPtr des = buf->Des();
            TInt err = aWs.GetWindowGroupNameFromIdentifier(wgs->At(spPos), des);
            if (!err)
                {
                if ( wgName->Caption().Compare(EIKON_SERVER_BACKDROP_WINDOW_GROUP_NAME) == 0)
                    {
                    aId = wgs->At(spPos);
                    break;
                    }
                }
            }
        CleanupStack::PopAndDestroy(2); // wgName, wgs
        }
    }

// ---------------------------------------------------------------------------
// Creates container for session in which it holds the sub sessions.
// ---------------------------------------------------------------------------
// 
CObjectCon* CAlfAppServer::NewContainerL()
    {
    if ( !iObjectConIx )
        {
        iObjectConIx = CObjectConIx::NewL();
        // we don't have ConstructL, so this would be suitable place to do one-shot-only construction 
        if(CCoeEnv::Static())
            {
            ResolveBdWgIdL(iBackDropWgIdentifier, *iWs, *iAppUi);
            }
        }
        
    return iObjectConIx->CreateL();
    }

// ---------------------------------------------------------------------------
// Releases container back to server.
// ---------------------------------------------------------------------------
// 
void CAlfAppServer::ReleaseContainer(CObjectCon& aContainer)
    {
    if ( iObjectConIx )
        {
        iObjectConIx->Remove(&aContainer);
        }
    }

// ======== RnD FUNCTIONS ========

// ---------------------------------------------------------------------------
// Logs window groups
// ---------------------------------------------------------------------------
// 
void CAlfAppServer::LogWindowGroupsL() const
    {
#ifdef _ALF_LOGGING
    // uncomment block if you want to log the window group order.
    /*
    CArrayFixFlat<TInt>* wgs = new (ELeave) CArrayFixFlat<TInt>(4); 
    CleanupStack::PushL(wgs);
    iWs->WindowGroupList(wgs);
    const TInt focusedWg = iWs->GetFocusWindowGroup();
    
    for ( TInt i = 0; i < wgs->Count() ; i++ )
        {
        TThreadId threadId;
        iWs->GetWindowGroupClientThreadId(wgs->At(i), threadId);
        RThread thread;
        if ( thread.Open(threadId) == KErrNone )
            {
            TName name = thread.Name();
            TPtrC ptr(name);
            if ( wgs->At(i) == focusedWg )
                {
                __ALFLOGSTRING3( " [%d] wg:%d %S FOCUSED",i,wgs->At(i), &ptr );
                }
            else
                {
                __ALFLOGSTRING3( " [%d] wg:%d %S",i,wgs->At(i), &ptr );
                }
            }
        }
    
    CleanupStack::PopAndDestroy( wgs );
    */
#endif
    }

MAlfExtension* CAlfAppServer::CreateExtensionL(TInt aImplementationUid, TInt aImplementationId, const RMessage2 & aMessage )
    {   
    TAlfElementFactory* factory = 0;

    if (iFactories.Count() == 0)
        {
        TAlfElementFactory ef;
        ef.iUid = KNullUid;
        ef.iImpl = new (ELeave) TAlfFixedFactory;
        ef.iDestructorUid = KNullUid;
        iFactories.AppendL(ef);
        }

    // Check if factory already loaded
    for(TInt ii = iFactories.Count()-1; ii>=0;ii--)
        {
        if (iFactories[ii].iUid == TUid::Uid(aImplementationUid))
            {
            factory = &iFactories[ii];
            break;
            }
        }

    if (!factory)
        {        
        // Todo should actually perform interface based scan and check that possible candidate (if any)
        // implements correct interface also
        // K_ALF_FACTORY_INTERFACE
        
        TAlfElementFactory ef;
        ef.iUid = TUid::Uid(aImplementationUid);
        ef.iImpl = (MAlfExtensionFactory*)REComSession::CreateImplementationL(
            ef.iUid, 
            ef.iDestructorUid);
        User::LeaveIfNull(ef.iImpl);
        iFactories.AppendL(ef);
        factory = &iFactories[iFactories.Count()-1];

        if (!iEcomWatcher)
            {
            iEcomWatcher = CAlfEcomUtil::NewL(*this);
            iEcomWatcher->Start();
            }
        }

    HBufC8* inBuf = 0;   
    TInt paramsLenght = aMessage.GetDesLength(1);
    if (paramsLenght > 0)
        {
        inBuf = HBufC8::NewLC(paramsLenght);        
        TPtr8 ptr = inBuf->Des();
        aMessage.ReadL(1,ptr);
        }

    // Create implementation    
    MAlfExtension* object = 0;
    if (inBuf)
        {
        object = factory->iImpl->CreateExtensionL(aImplementationId, inBuf->Des(), *((CAlfAppSrvSessionBase*)aMessage.Session()));
        CleanupStack::PopAndDestroy(inBuf);
        }
    else
        {
        object = factory->iImpl->CreateExtensionL(aImplementationId, KNullDesC8, *((CAlfAppSrvSessionBase*)aMessage.Session()));       
        }    
    
    factory->iAccessCount++;

    if (!object) // actually factory should leave
        {
        User::Leave(KErrNotSupported);
        } 

    return object;
    }
        

void CAlfAppServer::DestroyedObject(TInt aFactoryUid)
    {
    if (!aFactoryUid)
        {
        return;        
        }
 
    TInt ii = -1;
    for ( ii = iFactories.Count()-1; ii >= 0; ii--)
        {
        if (iFactories[ii] == aFactoryUid)
            {
            iFactories[ii].iAccessCount--;
            if (iFactories[ii].iAccessCount == 0)
                {
                iFactories[ii].iImpl->Release();
                if (iFactories[ii].iDestructorUid.iUid)
                    {
                    REComSession::DestroyedImplementation(
                            iFactories[ii].iDestructorUid);
                    }
                iFactories.Remove(ii);
                User::Heap().Compress();
                }
            break;
            }
        }
    }

void CAlfAppServer::NotifySessionsAboutDestroyedImplementation(TInt aFactoryUid)
    {
    iSessionIter.SetToFirst();
    while (iSessionIter)
        {
        CAlfAppSrvSessionBase* session =
            reinterpret_cast<CAlfAppSrvSessionBase*>(iSessionIter++);
        
        session->FactoryDestroyed(aFactoryUid);
        }
    }


// ---------------------------------------------------------------------------
// Create a transition effect instance for the server if it does not exist
// ---------------------------------------------------------------------------
// 
void CAlfAppServer::CreateTransitionEffectsL()
	{
	if ( iTransitionEffects == NULL )
		{
		iTransitionEffects = CAlfSrvTransEffect::NewL();
		}
	}


// ---------------------------------------------------------------------------
// Return a pointer to the transition effects instance 
// ---------------------------------------------------------------------------
// 
CAlfSrvTransEffect* CAlfAppServer::TransitionEffects()
	{
	return( iTransitionEffects );
	}

// -----------------------------------------------------------------------------
// Create Tfx Server client plugin and make a connection to Tfx Server
// -----------------------------------------------------------------------------
//
TInt CAlfAppServer::CreateTfxServerPlugin()
    {
    TInt err = KErrNone;
    
	#ifdef TFXSERVER_API_V2
    if (iTfxServer == NULL)
        {
        TUid implementationUidSrv = { KTfxSrv2EcomImpl };
        TRAP(err, iTfxServer = (MTransitionServer2*)
            REComSession::CreateImplementationL( implementationUidSrv, iTfxServerEComDestructorUID,
            	NULL ));
        }
   	if (!err && (iTfxServer != NULL))
		{
        if (!iTfxServer->IsConnected())
    		{
            err = iTfxServer->Connect();
            }	
		}
	#endif
		
	return err;	
    }
    
    
// -----------------------------------------------------------------------------
// Return Tfx Server client pointer if possible
// -----------------------------------------------------------------------------
//
MTransitionServer2* CAlfAppServer::TfxServer()
	{
	TInt err = KErrNone;
	#ifdef TFXSERVER_API_V2
    if (iTfxServer && !iTfxServer->IsConnected())
  		{
        err = iTfxServer->Connect();
        }	
	#endif
	return (err ? NULL : iTfxServer);
	}
	

// ---------------------------------------------------------------------------
// Triggers sending a system event to clients
// ---------------------------------------------------------------------------
// 
void CAlfAppServer::TriggerSystemEvent(TInt aEvent)
    {
    iSessionIter.SetToFirst();
    while (iSessionIter)
        {
        CAlfAppSrvSessionBase* session =
            reinterpret_cast<CAlfAppSrvSessionBase*>(iSessionIter++);
        
        session->TriggerSystemEvent(aEvent);
        }        
    }
 
TBool CrossCheckForeground(TAny* aThis)
    {
    CAlfAppServer* me = (CAlfAppServer*)aThis;
    
//#ifdef HUI_FX
    if ( me->iOldSession )
        {
        TRAP_IGNORE( me->iOldSession->ActivateContainerLayoutL(EFalse); 
        me->iOldSession = NULL;
        me->CancelBackgroundTimer(); );
        return EFalse;
        }
//#endif    
    TRAP_IGNORE(me->FocusedWindowGroupChangedL(0)) // cancels the timer
    return EFalse; 
    }

void CAlfAppServer::StartBackgroundTimerL(TInt aClientWg)
    {
    if (aClientWg != iLastActiveClientWg)
        {
        CancelBackgroundTimer();
        return;
        }
        
    if (!iBackgroundTimer)
        {
        iBackgroundTimer = CPeriodic::NewL(EPriorityNormal);
        }
    CancelBackgroundTimer(); // reset the time in case of active timers
/*    
#ifdef HUI_FX    
*/
    if ( iOldSession )
        {
        // longer delay to allow the application to start properly
        iBackgroundTimer->Start(1500000,0,TCallBack(CrossCheckForeground, this));
        }
    else
        {
        iBackgroundTimer->Start(20000,0,TCallBack(CrossCheckForeground, this));
        }
/*        
#else    
    iBackgroundTimer->Start(20000,0,TCallBack(CrossCheckForeground, this));
#endif
*/    
    }
    
void CAlfAppServer::CancelBackgroundTimer()
    {
    if(iBackgroundTimer)
        {
        iBackgroundTimer->Cancel();
        }
    }
    
TInt CAlfAppServer::CommonCommandBatchBufferMaxLength() const
    {
    return iCommonCommandBatchBuffer->Des().MaxLength();
    }

TBool CAlfAppServer::AcquireCommonCommandBatchBuffer( TPtr8& aBuffer )
    {
    TBool result = EFalse;
    
    if ( !iCommonCommandBatchBufferInUse )
        {
        // In practice, iCommonCommandBatchBufferInUse might be true only
        // if MAlfExtension::HandleCmdL starts active scheduler.
        // (but then ServiceL should not be called again)

        result = ETrue;
        iCommonCommandBatchBufferInUse = ETrue;
        aBuffer.Set( iCommonCommandBatchBuffer->Des() );
        aBuffer.Zero();
        }

    return result;
    }

void CAlfAppServer::ReleaseCommonCommandBatchBuffer()
    {
    iCommonCommandBatchBufferInUse = EFalse;
    }
    
CAlfSrvTextureManager& CAlfAppServer::TextureManager()
    {
    return *iTextureManager;
    }
    
CAlfSrvScreenBufferManager& CAlfAppServer::ScreenBufferManager()
    {
    return *iScreenBufferManager;
    }

TBool CAlfAppServer::AlfClientHasFocus()
    {
    TBool result = EFalse;
    TInt focusWindowGroupId = WsSession().GetFocusWindowGroup();

    iSessionIter.SetToFirst();
    while (iSessionIter)
      {
      CAlfAppSrvSessionBase* session = reinterpret_cast<CAlfAppSrvSessionBase*>(iSessionIter++);
      if ( session && session->ClientWindowGroup() == focusWindowGroupId )
          {
          result = ETrue;
          break;
          }
      }
    return result;
    }

CAlfAppSrvSessionBase* CAlfAppServer::UpMostClientAboveWg( TInt aWgId )
    {
    if (!iAppUi && !CCoeEnv::Static())
        return 0;
            
    CAlfAppSrvSessionBase* sessionAboveGivenWgId = NULL;
     
    CArrayFixFlat<TInt>* wgs = new CArrayFixFlat<TInt>(4);
    if ( wgs )
        {
        iWs->WindowGroupList( 0, wgs);
        TInt topmostWgId = wgs->At( 0 );
         for ( TInt ordinal = 0 ; ordinal < wgs->Count(); ordinal++ )
            {
            iSessionIter.SetToFirst();
            if( sessionAboveGivenWgId || aWgId == wgs->At( ordinal ) ) 
               {
                // either above client app's wg was found or there wasn't any client app's wg above given aWgId
                break;
                }
            while (iSessionIter)
                {
                CAlfAppSrvSessionBase* session = reinterpret_cast<CAlfAppSrvSessionBase*>(iSessionIter++);
                if ( session && 
                     session->ClientWindowGroup() == topmostWgId )
                    {
                    sessionAboveGivenWgId = session;
                    break;
                    }
                 }
            }
        }
    delete wgs;
    return sessionAboveGivenWgId;
    }

CAlfAppSrvSessionBase* CAlfAppServer::SrvSessionForControlGroup(CHuiControlGroup& aGroup)
    {
    CAlfAppSrvSessionBase* result = NULL;
    iSessionIter.SetToFirst();
    while (iSessionIter)
      {
      CAlfAppSrvSessionBase* session = reinterpret_cast<CAlfAppSrvSessionBase*>(iSessionIter++);
      if ( session->GetHandleFromInterface(EHuiObjectTypeControlGroup, &aGroup) != KErrNotFound )
          {
          result = session;
          break;
          }
      }
    return result;
    }

TInt CAlfAppServer::GetLastActiveClient()
    {
    return iLastActiveClientWg;
    }

