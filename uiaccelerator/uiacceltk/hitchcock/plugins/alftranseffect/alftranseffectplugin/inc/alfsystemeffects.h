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
* Description:   Alf system effects
*
*/


#ifndef ALFSYSTEMEFFECTS_H
#define ALFSYSTEMEFFECTS_H

#include <e32base.h>
#include <alf/AlfTransEffectPlugin.h>
#include <akntranseffect.h>
#include "alftfxplugincommon.hrh"

// CONSTANTS
const TUid KAlfTfxSystemEffectsImpUid = { KAlfTfxSystemEffectsImpUidValue };

// CLASS DECLARATION

/**
 *  System effects class for ALF server plugin
 *
 *  @since S60 v3.2
 */
class CAlfSystemEffects:	public CBase, public MAlfTransEffectPlugin
	{
	public: // Construction and destruction

    	/**
     	* Create a new instance of the interface
     	* 
     	* @return pointer to the instance
     	*/
			static MAlfTransEffectPlugin* NewExtensionL();

    	/**
     	* Destructor
     	*/
			virtual ~CAlfSystemEffects();

	private: // From MAlfTransEffectPlugin

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
	    	
private: // Construction and destruction

    	/**
     	* C++ contructor
     	*/
			CAlfSystemEffects();

    	/**
     	* Second phase constructor
     	*/
			void ConstructL();
	};

#endif // ALFSYSTEMEFFECTS_H
