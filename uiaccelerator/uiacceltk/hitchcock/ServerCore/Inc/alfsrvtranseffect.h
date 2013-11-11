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
* Description:   Transition effect framework
*
*/




#ifndef C_ALFTRANSEFFECT_H
#define C_ALFTRANSEFFECT_H

#include <uiacceltk/HuiEnv.h>
#include <uiacceltk/huigifanimationtexture.h>

#include "alf/AlfTransEffectPlugin.h"

class CAlfCenRep;
 
/**
 * This class implements an effect framework that can be used to integrate 
 * effects to Alf server. The class will call ECom plugins for actual effect
 * implementations.
 *
 */
NONSHARABLE_CLASS( CAlfSrvTransEffect ) : public CBase
    {
public:
    
    /**
     * Destructor
     */
    ~CAlfSrvTransEffect();

    /**
     * Constructor
     */
    static CAlfSrvTransEffect* NewL();
    
    /**
     * Returns the state of the effect system.
     *
     * @return Effect system boolean state.
     */
    TBool CAlfSrvTransEffect::IsEnabled();
    
    /**
     * Starts a new phase in the effect.
     *
     * @param aPhaseId Phase that is started.
     * @param aContext Effect context.
     * @param aEffectEnv Effect environment for this session.
     * @param aUidValue UID which effect is related to (e.g. current application UID).
     * @param aKey Key which effect is related to (e.g. current application window group).
     * @param aUidValue2 UID which effect is related to (e.g. previous application UID).
     * @param aKey2 Key which effect is related to (e.g. previous application window group).
     * @param aParams Other parameters.
     * @return System error code.
     */
    TInt StartPhase( TInt aPhaseId, 
    				 TInt aContext, 
    				 CAlfSrvEffectEnv& aEffectEnv, 
    				 TInt aUidValue = 0, 
    				 TInt aKey = 0, 
    				 TInt aUidValue2 = 0, 
    				 TInt aKey2 = 0, 
    				 const TDesC8* aParams = NULL);
    
    /**
     * Returns an available control group based on its index. Zero as a parameter means "all".
     *
     * Not yet implemnted
     *
     * @param aContext Effect context that will be aborted. 
     * @param aUidValue UID which effect is related to (e.g. current application UID).
     * @param aKey Key which effect is related to (e.g. current application window group).
     * @return System error code.
     */
    TInt Abort( TInt aContext = 0, 
    			TInt aUidValue = 0, 
    			TInt aKey = 0 );
    
    /**
     * Not yet implemented
     */
    TInt SetProperty( TInt aProperty, TInt aValue, TInt aUidValue = 0, TInt aKey = 0 );
    
    /**
     * Not yet implemented
     */
    TInt GetProperty( TInt aProperty, TInt& aValue, TInt aUidValue = 0, TInt aKey = 0 );
    
    /**
     * Not yet implemented
     */
    TInt AddEventObserver( 
            MAlfTransEffectObserver* aObserver, 
            TInt aEvents, 
            const TDesC8* aParams = NULL );
    
    /**
     * Not yet implemented
     */
   	TInt RemoveEventObserver( MAlfTransEffectObserver* aObserver, TInt aEvents );
    
    /**
     * Not yet implemented
     */
   	TInt GetEventState( TInt aEvent, TInt* aState, TDes8* aParams = NULL );
    
private:

    /**
     * Not yet implemented
     */
	struct TEffectMapItem
		{
		TEffectMapItem( TInt aEffectId, MAlfTransEffectPlugin* aPluginPtr) 
			:iEffectId( aEffectId ), iPluginPtr( aPluginPtr )
			{};
		TInt iEffectId;
		MAlfTransEffectPlugin* iPluginPtr;
		};

private:

    /**
     * Constructor
     */
    CAlfSrvTransEffect();

    /**
     * Second phase constructor.
     */
    void ConstructL();
    
    /**
     * Fills the effect plugins and effect map arrays.
     */
    void PopulateEffectArrayL();
    
    /**
     * Find the correct effect implementations for the context.
     */
    TInt FindPlugin( TInt aContext, MAlfTransEffectPlugin** aPlugIn );


 private: // Data
 
	// All effect plugins
	RPointerArray<MAlfTransEffectPlugin> iEffectPlugins;
	
	// Map for linking contexts to the effect implementations. 
	RArray<CAlfSrvTransEffect::TEffectMapItem> iEffectMap;
	
	// Listener for Avkon effect state
	CAlfCenRep* iAvkonTfxStateListener;
	
	// Whether the effect system is enabled
	TBool iIsEnabled;
	
	// Current context
	TInt iCurrContext;
	
	// Current phase
	TInt iCurrPhase;
  	};

#endif // C_ALFTRANSEFFECT_H
