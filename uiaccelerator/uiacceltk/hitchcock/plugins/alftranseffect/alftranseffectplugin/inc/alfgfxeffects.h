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
* Description:   Alf system effects
*
*/




#ifndef ALFGFXEFFECTS_H
#define ALFGFXEFFECTS_H

#include <e32base.h>
#include <alf/AlfTransEffectPlugin.h>
#include "alftfxplugincommon.hrh"
#include "../../../../ServerCore/Inc/alfwindowfxplugin.h"
#include "wsserverdrawercontroller.h"
#include "alfloggingconfiguration.h"

// CONSTANTS
const TUid KAlfGfxImplUid = { KAlfGfxImplUidValue };

class CAlfServerDrawer;
class CRepositoryHandler;
class CPolicyHandler;

// CLASS DECLARATION

/**
 *  System effects class for ALF server plugin
 *
 *  @since
 */
NONSHARABLE_CLASS( CAlfGfxEffects ):   public CAlfWindowFxPlugin, MEndChecker /*, public MAlfTransEffectPlugin */
	{
	public: // Construction and destruction

    	/**
     	* Create a new instance of the interface
     	* 
     	* @return pointer to the instance
     	*/
		static CAlfWindowFxPlugin* NewFxPluginL();

        /** 
        * 2nd phase constructor. Called by framework to complete construction of a plugin. 
        * Must have base call from deriving class to complete construction
        */
        void ConstructL(const CAlfWindowManager& aMgr, TInt aAlfBufferFormat);

    	/**
     	* Destructor
     	*/
		~CAlfGfxEffects();
		
		// From CAlfWindowManager - implemented
		
        /** Synch message from client */    
        void HandleMessageL(const TDesC8& aMessage, TPtr8& aResponse);

        /** Asynch message from client */    
        void HandleMessageL(const TDesC8& aMessage, const RMessage2& aClientMessage);

        /** Cancel asynch message(s) from client */    
        void CancelMessage(TInt aMessageId);
        
        /** release message(s) from client */  
        void ClientAboutToExit(TThreadId aClientId);

		/**
		* From CAlfWindowManager - not implemented
		*/
        void PrepareFrameL(TUint aEstimatedFrameInterval);

        void WindowUpdated(TInt aWindowId, TInt aEventType);
	
	public:
        
        void EndExpired();
        
	private: // From MAlfTransEffectPlugin
	    // - not supported   

    	TInt GetSupportedEffects( RArray<TInt>& aSupportedEffectIds );
    	TInt StartPhase( TInt aPhaseId, TInt aEffectId, CAlfSrvEffectEnv& aEffectEnv, const TDesC8* aParams );
    	TInt Abort() { return KErrNone; };
    	TInt AddEventObserver( MAlfTransEffectObserver* /*aObserver*/, TInt /*aEvents*/, const TDesC8* /*aParams*/ ) { return KErrNone; };
   		TInt RemoveEventObserver( MAlfTransEffectObserver* /*aObserver*/, TInt /*aEvents*/ ) { return KErrNone; };
  		TInt GetEventState( TInt /*aEvent*/, TInt* /*aState*/, TDes8* /*aParams*/ ) { return KErrNone; };
    
	private: // New
    	/**
     	* Leaving version of StartPhase().
     	*/
	    TInt DoStartPhaseL( TInt aPhaseId, TInt aEffectId, CAlfSrvEffectEnv& aEffectEnv, const TDesC8* aParams );
	    
	    void ShowVisibleGroupsAndMakeNonTransparentL(CAlfSrvEffectEnv& aEffectEnv);
	    void HideGroupsAndMakeTransparentL(CAlfSrvEffectEnv& aEffectEnv);
	    
	    // checks that resource directory is on z drive
	    TInt VerifyResourceLocation(const TDesC& aResourceDir);

	    	
private: // Construction and destruction

    	/**
     	* C++ contructor
     	*/
		CAlfGfxEffects();
#ifdef _ALF_FXLOGGING		
		void PrintRequestInfo(TInt aOperation, TInt aAction );
#endif
		
private: // data

    CAlfServerDrawer* iEngine;
    CRepositoryHandler* iSysDisable;
    CPolicyHandler* iPolicyHandler;
        
    /**
     * The wanted time (in milliseconds) for a frame in listbox animations.
     */
    TInt iListBoxFrameTime;

    /**
     * The minimum time (in milliseconds) between frames in listbox animations.
     */
    TInt iListBoxMinFrameTime;

    /**
     * The wanted time (in milliseconds) for a frame in control animations.
     */
	TInt iControlFrameTime;
	
	/**
     * The minimum time (in milliseconds) between frames in control animatins.
     */
	TInt iControlMinFrameTime;
	
	TInt iTransitionTime;
	TInt iTransitionMinTime;
	
	RArray<TInt> iActiveSyncronizedGroups;
	
	CEndCheck* iSyncronizedGroupDefitionEndChecker;
	
	};

#endif // ALFGFXEFFECTS_H
