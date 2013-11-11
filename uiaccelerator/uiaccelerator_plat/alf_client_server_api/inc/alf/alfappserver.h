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
* Description:   Application server
*
*/



#ifndef C_ALFAPPSERVER_H
#define C_ALFAPPSERVER_H

#include <AknServerApp.h>
#include <w32std.h>
#include <alf/alfmetricsinterface.h>    
#include <alf/alfappui.h>
#include <alf/alfextensionfactory.h>   
#include <alf/alfconstants.h> 
#include <uiacceltk/HuiCommand.h>
#include <uiacceltk/HuiEnv.h> 
#include <uiacceltk/HuiDisplay.h> 

class CApaAppServiceBase;
class CAlfAppSrvSessionBase;
class CAlfEcomUtil; 
class MWindowVisibilityObserver;
class CAlfSrvSubSessionBase;
class CAlfSrvTransEffect;
class CAlfAppSrvSession;
class CAlfSrvTextureManager;
class MTransitionServer2;
class CAlfSrvScreenBufferManager;

/**
 *  Application server class.
 *
 *  The server class is responsible of managing the sessions/services.
 *
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS(CAlfAppServer) : public CAknAppServer
    {   
    
public:

    // Window group position compared to the client wg (=parent)
    enum TAlfWGPostion
        {
        EBehindOfParent,
        EOnTopOfParent,
        EAbsoluteBackground,
        EAlfWindowSize
        };


    /**
     * Static constructor.
     */
    IMPORT_C static CAlfAppServer* NewAppServerL();

    /**
     * Destructor.
     */
    ~CAlfAppServer();
    
    /**
     * Client is about to exit, free all resources for the specific client
     *
     * @param aClientId Unique identifier of client, usually address of client's 
     *                  instance pointer
     */ //Todo: as this is called from base class inside library, no need to export ?
    IMPORT_C void HandleClientExit(TInt aClientId);
    
    /**
     * Moves the server window group relative to the clinet (parent) wg.
     *
     * @param aWindowGroup Server window group
     * @param aParentIdentifier Client side window group
     * @param aPosition Server's wg position compared to the client's wg.
     */
    IMPORT_C void AdjustWindowGroupPositionL(
        RWindowGroup& aWindowGroup, 
        TInt aParentIdentifier, 
        TAlfWGPostion aPosition );
        
    /**
     * Get server's window server session.
     *
     * @return Window server session reference.
     */
    IMPORT_C RWsSession& WsSession() const;

    /**
     * Get metrics interface
     *
     * @return Metrics API. NULL if not set. Ownership not transferred.
     */
    IMPORT_C MAlfMetricsInterface* MetricsInterface();
    IMPORT_C const MAlfMetricsInterface* MetricsInterface() const;

    /**
     * Set metrics interface
     *
     * @param aMetricsInterface Metrics API. Ownership not transferred.
     */
    IMPORT_C void SetMetricsInterface(MAlfMetricsInterface* aMetricsInterface);

    /**
     * Sets AppUi pointer.
     *
     * @param aAppUi AppUi pointer. Ownership not transferred.
     */
    void SetAppUi(CAlfAppUi* aAppUi);
    
    /**
     * Get AppUi pointer
     *
     * @return AppUi pointer. NULL if not set. Ownership not transferred.
     */
    IMPORT_C CAlfAppUi* AppUi();
        
    /**
     * Called when the focused window group is changed
     *
     * @param aSession Pointer to focused session
     * @param aWgId Window group identifier of new session, needed only when embedded apps transition takes place
     */
    void FocusedWindowGroupChangedL( CAlfAppSrvSessionBase* aSession, TInt aWgId = KErrNotFound );
    
    /**
     * Called when window visibility changes.
     *
     * @param aVisibilityFlags Visibility flags - see TWsVisibilityChangedEvent
     * @param aDestination Associated window-owning control.
     */
    void WindowVisiblityChangedL( TUint aVisibilityFlags, CCoeControl* aDestination );

    /**
     * Checks ECom plug-ins.
     */
    void CheckForEcomPluginInstallUninstallL();

    /**
     * For setting observers which will receive notify when window visibility changes.
     *
     * @param aObserver observer implementing MWindowVisibilityObserver interface
     * @param aClientId Identifier of a session so observer can be removed on queue when the session is terminated
     */
    IMPORT_C void SetWindowChangeObserverL(MWindowVisibilityObserver* aObserver, TInt aClientId); 

    /**
     * Returns a new container for sub sessions.
     *
     * @return Container. Ownership not transferred.
     */
    CObjectCon* NewContainerL();

    /**
     * Releases container created by NewContainerL back to server.
     * @param aContainer container to be removed.
     */
    void ReleaseContainer(CObjectCon& aContainer);
    
// from base class CAknAppServer

    /**
     * From CAknAppServer
     * Creates session/service based on the given service type.
     * This is called by the framework when a client opens session with the 
     * server.
     *
     * @param aServiceType Service type, which identifies the service.
     * @return New service/session instance. Ownership transferred.
     */
    CApaAppServiceBase* CreateServiceL( TUid aServiceType ) const;
    
    /**
     * From CAknAppServer
     * Called when all client have exited.
     */
    void HandleAllClientsClosed();
        
public: // internals

    /**
     * Used by callback for asynchronous ecom implementation removal
     */   
    void DoSynchEcomImplementations();

    MAlfExtension* CreateExtensionL(TInt aImplementationUid, TInt aImplementationId, const RMessage2 & aMessage );
    
    void DestroyedObject(TInt aFactoryUid);

    void CreateTransitionEffectsL();
    
    CAlfSrvTransEffect* TransitionEffects();
    
	TInt CreateTfxServerPlugin();
 
 	MTransitionServer2* TfxServer();
    
    /**
     * Sends an event to all clients. This method can be used for general purpose server to client
     * notifications e.g. when server receives resource changed event or ws-events or
     * something else as long as the clientside knows how to handle the event.  
     */
    void TriggerSystemEvent(TInt aEvent);
    
    void StartBackgroundTimerL(TInt aClientWg);
    
    void CancelBackgroundTimer();
    
    /**
     * Returns maximum length of common command batch buffer.
     * @return maximum length of common command batch buffer.
     */
    TInt CommonCommandBatchBufferMaxLength() const;

    /**
     * Gets common command batch buffer (unless it is already in use).
     * Client must call ReleaseCommonCommandBatchBuffer to release buffer for 
     * global use.
     * @param aBuffer pointer descriptor which will point to common buffer.
     * @return ETrue if common command batch buffer was given to client, EFalse otherwise.
     */
    TBool AcquireCommonCommandBatchBuffer( TPtr8& aBuffer );

    /**
     * Releases common command batch buffer for global use.
     */
    void ReleaseCommonCommandBatchBuffer();
   
    /**
     * Returns reference to texture manager.
     * @return reference to texture manager.
     */
    CAlfSrvTextureManager& TextureManager();
    
    /**
     * Returns reference to screen buffer manager.
     * @return reference to screen buffer manager.
     */
    CAlfSrvScreenBufferManager& ScreenBufferManager();

    /**
     * Goes through all the sessions and checks if one of them has the keyboard focus
     */
    TBool AlfClientHasFocus();
    
    /**
     * This is typically called by the session when it receives EAlfNotifyAppVisibility event that
     * it's windowgroup is getting hidden.
     * 
     * The session can query if there is some other alf application's window group above it 
     * and set that other session as a new activesession  
     */
    CAlfAppSrvSessionBase* UpMostClientAboveWg( TInt aWgId );

    
    CAlfAppSrvSessionBase* SrvSessionForControlGroup(CHuiControlGroup& aGroup);
    
    TInt GetLastActiveClient();
    
    
public:
    
    CAlfAppSrvSessionBase* iOldSession;

    /**
     * Second phase constructor. Called by framework.
     * @param aFixedServerName server name.
     */
    void ConstructL( const TDesC& aFixedServerName );
    
private:

    // private default constructor
    CAlfAppServer();    

    /**
     * Checks if implementation exists.
     */
    TBool ImplementationExists(TUid aServiceType) const;
    
    /**
     * Loads extension.
     */
    CAlfAppSrvSessionBase* LoadServiceExtensionL(TUid aServiceType);
    
    /**
     * Creates new session implementation.
     */
    CAlfAppSrvSessionBase* NewImplementationL(TUid aServiceType);
        
    /**
     * Initiate callback for asynchronous ecom implementation removal
     */
    void SyncEcomImplementations(TInt aClientId); 
    
    /**
     * Rnd: logs the window groups
     */
    void LogWindowGroupsL() const;
    
    /**
    * Ecom factory has been removed, command sessions to close assosiated objects
    */
    void NotifySessionsAboutDestroyedImplementation(TInt aFactoryUid);

private:

    // just to allow accessing Cone's session without CCoeStatic
    RWsSession* iWs; 
    
    // backdrop window group identifier, cahcing this value allows us find status pane window group much faster
    // should be initialized in leaving constructor once that is implemented 
    TInt iBackDropWgIdentifier; 

    // Metrics API. Not owned.
    MAlfMetricsInterface* iMetricsInterface;
    
    // AppUi pointer. Not owned.
    CAlfAppUi* iAppUi;
    
    // Window group id of the client
    // This is updated when the client becomes focused.
    TInt iLastActiveClientWg;
    
    // ECom observer.
    CAlfEcomUtil* iEcomWatcher;
    
    struct TWindowChangeObserver
        {
        TWindowChangeObserver(MWindowVisibilityObserver* aObserver, TInt aClientId):iId(aClientId), iPtr(aObserver){}
        TInt iId;
        MWindowVisibilityObserver* iPtr;
        };
    
    // list of sessions which are interested about window visibility changes
    RArray<TWindowChangeObserver> iWindowChangeObservers;
    
    struct TServiceExtension
        {
        TServiceExtension():iImpl(0){}
        operator==(const TInt aId)
            {
            return TInt(iImpl)==aId;
            }
        TUid iDestructorUid;
        TUid iServiceUid;
        CAlfAppSrvSessionBase* iImpl;
        };
    
    // list of ecom based sessions 
    RArray<TServiceExtension> iCustomSessions;
    
    // list of ecom based sessions that have been already destructed, 
    // but the ECom Fw has not been notified yet
    RArray<TInt> iRemovedArray;
    
    // Async callback for ecom callbacks
    CAsyncCallBack* iAsynchOneShot;
    
    // For sub session objects. Own.
    CObjectConIx* iObjectConIx;

    
    struct TAlfElementFactory
        {
        TAlfElementFactory():iImpl(0), iAccessCount(0){}
        operator==(const TInt aId)
            {
            return iUid.iUid==aId;
            }

        TUid iDestructorUid;
        TUid iUid;
        MAlfExtensionFactory* iImpl;
        TInt iAccessCount;
        };
    
    RArray<TAlfElementFactory> iFactories;
        
    TInt iTransitionEffect;

    // Transition effects framework
    CAlfSrvTransEffect* iTransitionEffects;

	// Tfx Server API (ECom plugin)
    MTransitionServer2* iTfxServer;

	// Tfx Server API destructor uid
    TUid iTfxServerEComDestructorUID;

    CPeriodic* iBackgroundTimer;
    
    /**
     * Command batch buffer. This buffer is shared between all sessions.
     * Owned.
     */
    HBufC8* iCommonCommandBatchBuffer;

    /**
     * Boolean variable indicating if common command batch buffer is in use.
     */
    TBool iCommonCommandBatchBufferInUse;
    
    /**
     * Server side texture manager.
     * Owned.
     */
    CAlfSrvTextureManager* iTextureManager;
    
    /**
     * Server side screen buffer manager.
     * Owned.
     */
    CAlfSrvScreenBufferManager* iScreenBufferManager;
    
    };

#endif // C_ALFAPPSERVER_H
