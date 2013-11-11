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
* Description:   Transition effect framework plugin interface
*
*/




#ifndef M_ALFTRANSEFFECTPLUGIN_H
#define M_ALFTRANSEFFECTPLUGIN_H

#include <ecom/ecom.h>
#include <implementationproxy.h>
#include <avkondomainpskeys.h>
#include <alf/AlfTransEffectPlugin.hrh>
#include "alfuids.h"

class CAlfSrvEffectEnv;

// CONSTANTS

// Common UID that identifies all effect plugins
const TUid KAlfTransEffectPluginInterfaceUid = {KAlfTransEffectPluginInterfaceUidValue};

const TUid KAlfGfxPluginInterfaceUId = {KAlfGfxPluginInterfaceUIdValue};

// Common string that identifies all effect plugins
_LIT8( KAlfTransEffectPluginTypeStringDesc, KAlfTransEffectPluginTypeString);

// We define the PubSub keys here for testing purposes.
// If the system works as intended, we need to create a separate file for the pubsub keys

const TUid KPSAlfDomain = { KAlfAppServerInterfaceUid3 };

/**
 * Indicates the status of the transition
 *
 * 0 transition ongoing
 * 1 transition finished - reserved windows can be released.
 */
const TUint32 KAlfTransitionStatus = 0x00000001; 


/**
 * This interface class is used for a callbacks from the effect plugins.
 *
 */
class MAlfTransEffectObserver
    {
	public:
    	virtual TInt AlfTransEffectCallback(TInt aEvent, TInt aState = 0, const TDesC8* aParams = NULL) = 0;
	};
	
class MAlfGfxEffectPlugin
    {
    // Currently this class only contains the enumeration of the operations that are supported
    // No actual interface at the moment.
    // All commands are routed via HandleMessageL function
    public:
    
// New enums are added when needed, this is just a list of those that
// are currently rerouted in new gfxtransenginetfx

// In order to route also the registration commands from tfxsrvplugin that Akn skin server
// starts, more operations are added.

// The whole message structure is a memory stream to handle varying parameters
// The operation code defines what parameters will be present.

// It is also possible to get back data, the return data must be a memory stream
// that has length and some type indicator in the beginning.
// Different operations may return different kind of data.
// For example EBeginFullScreen will return error code KErrAbort if full screen effects are disabled.

        enum TOp
	        {
	        ENoOp,
	        // full screen effect operations
	        EBeginFullscreen,
	        EEndFullscreen,
	        EAbortFullscreen,
	        // control effects
	        EBeginComponentTransition,
	        EAbortComponentTransition,
	        // full screen transition server operations
	        ERegisterFullScreenEffectFile,
	        EUnregisterFullScreenEffectFile,
	        EBlockFullScreenUid,
	        EUnregisterAllEffects,
	        // operations coming via CAlfTransitionServerClient
	        ETfxServerOpAddFullscreenKML,
	        ETfxServerOpRemoveFullscreenKML,
	        ETfxServerOpListBoxRegisterKML,
	        ETfxServerOpListBoxUnregisterKML,
	        ETfxServerOpListBoxSetFrameTime,
	        ETfxServerOpListBoxSetMinFrameTime,
	        ETfxServerOpControlSetFrameTime,
	        ETfxServerOpControlSetMinFrameTime,
	        ETfxServerOpBlockFullscreenUid,
	        ETfxServerOpControlPolicyUpdate,
	        ETfxServerOpControlPolicyRemove,
	        ETfxServerOpRegisterControlKml,
	        ETfxServerOpRegisterControlKmlEx,
	        ETfxServerOpRemoveAllKml,
	        ETfxServerOpRemoveControlKml,
	        ETfxServerOpBeginGroup,
	        ETfxServerOpEndGroup,
	        ETfxServerOpSetWantedTime,
	        ETfxServerOpSetMinTime,
	        ETfxServerOpAddFullscreenKMLEx,
	        ETfxServerVerifyConnection,
	        ETfxServerOpFreeRam,
	        ETfxServerOpMemoryGood,
	        // policy update requests
	        ETfxServerOpControlPolicyRequest,
	        ETfxServerOpControlPolicyGet,
	        ETfxServerOpControlCancelPolicyUpdate
	        };
     
    };

/**
 * This interface class is implmented by plugins for Alf transition effect framework 
 */
class MAlfTransEffectPlugin
	{
	public:

    	/**
     	 * Default phase numbers for the effects
     	 */
		enum TPhase
			{
			EPhaseNone,
			EFirstPhase,
			ESecondPhase,
			};

	    /**
	     * System effect contexts 
	     */
		enum TContext
			{
			EContextNone,
			EContextFocusGained,
			EContextFocusLost,
			EContextStart,
			EContextExit,
			EContextEmbeddedStart,
			EContextEmbeddedExit
			};
			
        /**
         * Types of data that may be returned by CAlfGfxEffects::HandleMessageL
         * The caller 
         */		
	    enum TReturnParameterTypes
	        {
	        };
    
	public:
	
    	/**
         * Creates the plugin implementation.
     	 *
     	 * @param aImplementationUid UID of the effect plugin implementation.
     	 * @return Plugin implementation.
     	 */
 		static MAlfTransEffectPlugin* NewExtensionL( TUid aImplementationUid );

        /**
        * Virtual destructor. 
        * @since 2.8
        */
		virtual ~MAlfTransEffectPlugin() { REComSession::DestroyedImplementation (iDtor_ID_Key); };
		
    	/**
         * Get all effect IDs that are supported.
     	 *
     	 * @param aSupportedEffectIds Array of supported effect IDs.
     	 * @return System error code.
     	 */
    	virtual TInt GetSupportedEffects( RArray<TInt>& aSupportedEffectIds ) = 0;
    
    	/**
         * Starts a new phase in the effect.
     	 *
     	 * @param aPhaseId Phase that is started.
      	 * @param aEffectId Effect ID.
     	 * @param aEffectEnv Effect environment for this session.
     	 * @param aParams Other parameters.
     	 * @return System error code.
     	 */
    	virtual TInt StartPhase( TInt aPhaseId, TInt aEffectId, CAlfSrvEffectEnv& aEffectEnv, const TDesC8* aParams = NULL ) = 0;
    
    	/**
     	 * Not yet implemented
     	 */
    	virtual TInt Abort() = 0;
    

    	/**
     	 * Not yet implemented
     	 */
    	virtual TInt AddEventObserver( MAlfTransEffectObserver* aObserver, TInt aEvents, const TDesC8* aParams = NULL ) = 0;
    
    	/**
     	 * Not yet implemented
     	 */
   		virtual TInt RemoveEventObserver( MAlfTransEffectObserver* aObserver, TInt aEvents ) = 0;
    
    	/**
     	 * Not yet implemented
     	 */
   		virtual TInt GetEventState( TInt aEvent, TInt* aState, TDes8* aParams = NULL ) = 0;
    

	public:

		// The ID used by ECom for destroying this dll
		TUid	iDtor_ID_Key;
	};


#endif // M_ALFTRANSEFFECTPLUGIN_H
