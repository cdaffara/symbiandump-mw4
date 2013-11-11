/*
* Copyright (c) 2008-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   window server plugin, used by transition server to control
*                framebuffers.
*
*/



#ifndef __ALFSERVERDRAWER_H__
#define __ALFSERVERDRAWER_H__

#include <e32std.h>
#include <w32std.h>
#include <e32property.h>

#include "alfdrawerengine.h"

class CWsServerDrawerController;
class CIdle;
class RWsSession;
class CAlfWindowManager;
class CAlfWindow;
class CPolicyHandler;
class CAlfFinishTimer;
class CPSObserver;

/**
 *  crp used by the server to control framebuffers, and used as proxy for
 *  fullscreen messages.
 *
 *  @since S60 3.2
 */
 NONSHARABLE_CLASS(CAlfServerDrawer): public CBase, public MAlfDrawerEngine 
	{
public:

	static CAlfServerDrawer* NewL( CAlfWindowManager* aWindowManager,
	    CPolicyHandler& aPolicyHandler );

    enum TScreenModeChangedStates
        {
        EScreenModeChangedIdle,
        EScreenModeBeginHasBeenCalled,
        EScreenModeChangedWaitingForScreenUpdated
        };

    ~CAlfServerDrawer();

	TInt BeginControlTransition( TUint aAction, TUid aControlUid,
	    TInt aWindowGroup, TInt aWindowHandle );
	void EndControlTransition( TInt aHandle );
	void UnredirectEvent( TInt aHandle );

    void PrepareFullscreen();
    // parameters are analyzed by CAlfGfxEffects
    
    void BeginFullscreen( TInt aAction, const TRect& aEffectRect, 
						TInt aType, const TUid aUid1, const TUid aUid2, TInt aData,
						TSecureId aSid1, TInt aWg1, TSecureId aSid2, TInt aWg2);
    void EndFullscreen(TBool aTimeout = EFalse);
    void CancelFullscreen();
    void AbortFullscreen();
    void RegisterAlfFullScreenEffect( TInt aAction, const TDesC& aResourceDir,
            const TDesC& aFileName);
    void UnregisterAlfFullScreenEffect( TInt aAction );
    void UnregisterAllAlfFullScreenEffect();
    void BeginSyncronizedGroup(TInt aGroupId);
    void EndSyncronizedGroup(TInt aGroupId, TBool aForced = EFalse);
    
    /**
    * From MAlfDrawerEngine
    */
	
	TUid& FromUid();
	TSecureId& FromSid();
	TInt FromWg();
	TUid& ToUid();
	TSecureId& ToSid();
	TInt ToWg();
	TUint& Action();
	TInt& Flags();

	void IncreaseControlHandle();
	TInt CurrentControlHandle();
	
	TInt CurrentFullScreenHandle();
	
	void StartEndChecker();
	void CancelEndChecker();

	TInt SendBeginFullscreen(TInt aHandle, TUint aAction, const TUid& aUid, 
							 const TUid& aPreviousUid, const TRect& aEffectRect);
	/**
	 *  SendEndFullscreen
	 *  
	 *  @return    1, If longer app start effect was triggered. Otherwise returns 0.
	 */
	TInt SendEndFullscreen(TBool aTimeout = EFalse);
	TInt SendAbortFullscreen();
	TInt SendAbortControlTransition();
	void SendBeginFullscreen();
    void DoSendBeginFullscreenL();
	TInt SendBeginControlTransition();
	TInt SendFinishControlTransition();
	TInt DoSendFinishControlTransition(TInt aHandle);
	
    TBool EffectFinishedFullScreen() const;
    
	static TInt FailedFullscreen(TAny* aArg);

    TBool ScreenModeChanged() const;
    
    
private:
	CAlfServerDrawer( CAlfWindowManager* aWindowManager, CPolicyHandler& aPolicyHandler );
	void ConstructL();
	static TInt AsyncConstruct(TAny* aThis);

	TAny* ResolveObjectInterface(TUint aTypeId);
	
private:
    static TInt ScreenModeChangedTimeoutL( TAny* aServerDrawer );
    void HandleScreenModeChangedTimeoutL();
    
    static TInt FullScreenTimeout( TAny* aServerDrawer );
    void HandleFullScreenTimeout();
    
    static TInt FinishFullScreenTimeout( TAny* aServerDrawer );
    void HandleFinishFullScreenTimeout();

    static TInt ControlTimeout( TAny* aServerDrawer );
    void HandleControlTimeout();
    
    static TInt FullScreenTransitionFinished( TAny* aServerDrawer );
    void HandleFullScreenTransitionFinished(); 

    static TInt ControlTransitionFinished( TAny* aServerDrawer );
    void HandleControlTransitionFinished();
    
    TBool FileExistsL( TPtrC& aResourceDir, TPtrC& aFileName );
        
private: // data

public:
	CWsServerDrawerController* iControlController;
	CWsServerDrawerController* iFullScreenController;
	    
	CAlfWindowManager* iWindowManager;
    CPolicyHandler& iPolicyHandler;
	TBool iInScreenModeChange;

    TInt iCurrentControlHandle;
    TInt iCurrentFullScreenHandle;

    TUint iAction;
    TUint iOldAction;
    TInt iFlags;
    TRect iEffectRect;
    TRect iControlRect; // demarcation rectangle for control effects
    TUid iFromUid;
    TUid iToUid;
    TSecureId iFromSid;
    TSecureId iToSid;
    TInt iFromWg;
    TInt iToWg;
    TBool iWaitingForRootWgId;
    TInt iFromScreen;
    TInt iToScreen;
    
    TUid iParentUid;
    TInt iFocusWg;
    
    // Component transition data
    TUint iControlAction;
    TUid iControlUid;
    TInt iControlWindowGroup;
    TInt iControlHandle;
    
    TInt iType; // parameter type for GFX parameters
    TScreenModeChangedStates iScrModeChangedState;
    CAlfFinishTimer* iFullScreenTimeout;
    CAlfFinishTimer* iControlTimeout;
    
    CAlfFinishTimer* iFinishFullScreen;
    
    // This variable tells if the full screen transition was finished
    // by a signal from Effect Engine (via AlfBridge)
    // or if timeout finished the transaction.
    TBool iFullScreenFinished;
    
    CPSObserver* iFullScreenTransitionEndObserver;
    CPSObserver* iControlTransitionEndObserver;
    
    RProperty iProperty;
    
    RFs iFs;
    TBool iFullScreenFxSent;
    TBool iFullScreenEndSent;

    RArray<TInt> iActiveControlFx;
    };


#endif //__ALFSERVERDRAWER_H__
