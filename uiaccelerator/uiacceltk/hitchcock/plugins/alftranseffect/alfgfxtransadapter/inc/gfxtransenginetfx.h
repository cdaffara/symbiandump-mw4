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



#ifndef _GFXTRANSENGINESTUB_H_
#define _GFXTRANSENGINESTUB_H_

#include <e32base.h>

#include <gfxtranseffect/gfxtransadapter.h>
#include <alfdecoderserverclient.h>
#include <alf/AlfTransEffectPlugin.h>
#include <alftfxserverobserver.h>

class CWsTransControlGraphic;
class CWsAdapterGraphic;
//class CTransControlDrawerInfo;
class CSubWOControl;
class CAlfTransitionRequest;

/**
 *  gfxtranseffect engine adapter
 *
 *  This class is loaded by the GfxTransEffect client and used by it to request 
 *  transition effects.
 *
 *  @since S60 3.2
 */
 NONSHARABLE_CLASS(CGfxTransAdapterTfx) : public CBase, 
										 public MGfxTransAdapter,
										 public MTfxServerObserver
	{
	friend class MGfxTransAdapter;
private:

	/**
	 * constructor
     * 
     * @since S60 3.2
     *
     * @param aClient the GfxTrans client side.
    */  
	CGfxTransAdapterTfx(MGfxTransClient* aClient);

	/**
	 * default destructor
     * 
     * @since S60 3.2
     *
    */  
	~CGfxTransAdapterTfx();
public:

	/**
	 * 2nd phase constructor
     * 
     * @since S60 3.2
     *
    */  
	void ConstructL();

	/**
	 * From MGfxTransEngine
	 * 
	 * Notifies the adaptor of the different states its in while collecting data for a transition.
     * 
     * @since S60 3.2
     *
     * @param aState the state the client is currently in
     * @param aKey the control that is now processed
     * @param aHandle the handle for this transition.
     * @return KErrNone if successful, any other error code if the client should abort.
    */  
	TInt HandleClientState(TClientState aState, const CCoeControl* aKey, TInt aHandle);

	/**
	 * From MGfxTransEngine
     *
     * Tests if adapter is active (trying to make it active if needed).
     * @since S60 3.2
     *
     * @return ETrue if active, otherwise EFalse.
    */  
	TBool IsActive();

	/**
	 * From MGfxTransEngine
     *
     * Called by client to start the specific transition
     * @since S60 3.2
     *
     * @param aHandle the handle of the transition to start.
    */  
	void StartTransition(TInt aHandle);

	/**
	 * From MGfxTransEngine
     *
     * Starts the policy-request system between client and server.
     *
     * @since S60 3.2
     *
     * @param aCount. Reference where the number of available policies will be returned.
     * @return Pointer to a TControlPolicy array or NULL.
    */  
	TControlPolicy* GetTransitionPolicies(TInt &aCount);

	/**
	 * From MGfxTransEngine
     * 
     * Starts a fullscreen effect
     * 
     * @since S60 3.2
     *
     * @param aAction the fullscreen transition action number
     * @param aEffectArea an optional rectangle the fs effect can use
    */  
	void BeginFullScreen(TUint aAction, const TRect &aEffectArea);

	/**
	 * From MGfxTransEngine
     * 
     * Starts a fullscreen effect, extended version
     *
     * @since S60 3.2
     *
     * @param aAction the fullscreen transition action number
     * @param aEffectArea an optional rectangle the fs effect can use
     * @param aType an integer identifying the format of aParams
     * @param aParams a buffer to be interpreted by the TfxServer containing more information
    */  
	void BeginFullScreen(TUint aAction, const TRect &aEffectArea, 
						 TUint aType, const TDesC8& aParams);

	/**
	 * From MGfxTransEngine
	 *
	 * Sends an EndFullscreen notification
     * 
     * @since S60 3.2
     *
    */  
	void EndFullScreen();

	/**
	 * From MGfxTransEngine
	 *
	 * Tells TfxServer to abort the current fullscreen effect, if any is ongoing.
     * 
     * @since S60 3.2
     *
    */  
	void AbortFullScreen();

	/**
	 * From MGfxTransEngine
	 *
	 * Handles updates of participants, during the transition
     * 
     * @since S60 3.2
    */  
	void HandleParticpantUpdate(TInt aHandle, 
								const CParticipantData* aParticipant, 
								RWsGraphicMsgBuf* aCommandBuffer, 
								const TRect& aDrawRect, 
								const TRect& aBoundingRect);
								
	/**
	 * From MGfxTransEngine
	 *
	 * Not used.
     * 
     * @since S60 3.2
    */  
	void NotifyExternalState(TInt aState, const TDesC8* aArg);

	/**
	 * From MTfxServerObserver
	 *
     * Called when a policy update request has completed.
     * @param aPolicy a new control policy
    */  
	void PolicyUpdate(TControlPolicy* aPolicy);
	
	/**
	 * From MTfxServerObserver
	 *
     * Called when all policies needs to be cleared.
    */  
	void ClearAllPolicies();

	/**
	 * From MTfxServerObserver
	 *
     * called when a transitionfinished notification request has completed
     * @param aHandle the handle of the transition that finished.
    */  
//	void TransitionFinished(TInt aHandle );

    /*
    *
    */
    void GetNewPoliciesL( TUint aCount );
    
    /*
    *
    */
    void CancelPolicyUpdates();
    
    void SendGroupCommand(TInt aGroupId, TBool aGroupFinished);
    /**
    * Send the policy update request to effect plugin
    */
    TInt RequestPolicyUpdates();

private:

	/**
     * Starts the transition
     * 
     * @since S60 3.2
     *
     * @param aHandle handle of the transition to start
     * @param aTransData the transition specification collected by the client
    */  
	void DoStartTransitionL(TInt aHandle, const CTransitionData* aTransData);

    /*
     * Generate transition event
     */
	void GenerateComponentTransitionEventL(const CTransitionData* aTransData, TInt aOp, TInt aHandle = KErrNotFound);
	
	/*
	 * Connect to server and load plugin
	 */
	void ConnectToServer();
	
	/**
	 * Finds a component transition handler crp
     * 
     * @since S60 3.2
     *
     * @param aHandle the transition handle to find the crp for.
     * @return a pointer to the crp
    */  
//    CWsTransControlGraphic* FindDrawer(TInt aHandle);

    /**
    * Finds a component transition
     *
     * @since S60 3.2
     *
     * @param aHandle the transition handle to find the control for.
     * @return a pointer to the CTransControlDrawerInfo
    */
//    CTransControlDrawerInfo* FindControl(TInt aHandle);
    void SubConEnd(const CCoeControl* aControl);
    void SubConBegin(const CCoeControl* aControl);
    void SubConAbort( const CCoeControl* aControl );
    
    /*
     * Finds the secure ID for the given application UID.
     * 
     * @param aAppUid Application UID
     * @return Secure ID.
     */
    TSecureId SecureIdFromAppUid( TUid aAppUid );
    
    /*
     * Finds the Windoe group ID for the given application UID.
     * 
     * @param aAppUid Application UID
     * @return Window group ID. Returns 0 if not found.
     */
    TInt32 WindowGroupIdFromAppUid( TUid aAppUid );


private: // data
    /**
     * pointer to the client side
     * Not owned.
     */
	MGfxTransClient* iClient;
	
	/**
	 * A connection to the tfx server
	 */
	//RTfxServer iTfxServer;
	
	RAlfTfxClient iTfxServer;
	
	// We need plugin implementation id to send to RAlfTfxClient,
	// we need to do the resolution from interface uid to implementation uid ourselves,
	// alf does not do any resolution
	TUid iPluginImplementation;
	TBool iConnected;
	TBool iHasPlugin;
	
    // The value in central repository to keep track if effects are allowed
    TInt iEffectValue;

	/**
	 * Each currently active transition has an entry in this array.
	 * It owns the pointers in it.
	 */
//	RPointerArray<CTransControlDrawerInfo> iControlInfo;

    TInt iCurrHandle;

    struct TControlEffect
        {
        TInt iHandle;
        TInt iWindowGroup;
        TInt iWindowHandle;
        };
    RArray<TControlEffect> iControlEffects;
    
//	TBool iIsWO;

	TThreadId iClientId;

	HBufC8* iTransferBuffer;
	HBufC8* iReturnBuffer;
	HBufC8* iAsyncTransferBuffer;
	HBufC8* iAsyncReturnBuffer;

	CAlfTransitionRequest* iPolicyReq;

//	CCoeEnv* iCoeEnv;
	///
//	TUint iActiveAction;
//	TUid  iActiveUid;
//	TInt iSubCons;
//	TInt iCurrZ;
	const CCoeControl* iMainControl;
//	RPointerArray<CSubWOControl> iSubWOControls;
//    RPointerArray<const CCoeControl> iIgnoredWOChildControls;

//    TInt iSequence;
	
	struct TUidMapping
	    {
	    TUidMapping() : iAppUid(0), iSecureId(0), iWindowGroupId(0) {}
	    TInt32 iAppUid;
	    TInt32 iSecureId;
	    TInt32 iWindowGroupId;
	    };
	
	TUidMapping iCachedUidMapping; // cache one - this is covering most of the cases
	
    };

#endif
