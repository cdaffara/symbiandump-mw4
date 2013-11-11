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





#include "alflogger.h"
#include "alf/alfappserver.h"
#include "alf/alfappui.h"
#include "alf/alfconstants.h"
#include "alfsrvdisplaysubsession.h"
#include "alfsrvcontrolgroupsubsession.h"
#include "alfsrvsettingshandler.h"

#include <coemain.h>
#include <mm/mmcleanup.h>
#include <pslninternalcrkeys.h>
#include <centralrepository.h>
#include <akntranseffect.h> 

#include <uiacceltk/HuiControl.h>
#include <uiacceltk/HuiControlGroup.h>
#include <uiacceltk/HuiDisplay.h>
#include <uiacceltk/HuiDisplayCoeControl.h>
#include <uiacceltk/HuiEnv.h>
#include <uiacceltk/HuiTextVisual.h>
#include <uiacceltk/HuiTransformation.h>
#include <uiacceltk/HuiBorderBrush.h>
#include <uiacceltk/HuiImageVisual.h>
#include <uiacceltk/HuiGridLayout.h>
#include <uiacceltk/HuiTextureProcessor.h>
#include <uiacceltk/HuiSegmentedTexture.h>


#include "alfsrvtranseffect.h"


// ============================ LOCAL CLASS ===============================

/**
 * This helper class implements listener for any CenRep key. 
 *
 */
NONSHARABLE_CLASS( CAlfCenRep ) : public CActive
    {
public:  // New  
    static CAlfCenRep* NewL( TInt aPriority, const TUid& aRepository, TUint32 aKey );
    ~CAlfCenRep();
    
    /**
     * Gets the current stored value of the key.
     */
    TInt GetValue( TInt& aValue );
    
private: // From CActive
    void RunL();
    void DoCancel();
    
private: // New
    CAlfCenRep( TInt aPriority, const TUid& aRepository, TInt aKey );
    void ConstructL();

private: // Data
	CRepository *iCenRep;
    TUid iRepository;
    TInt iKey;
    TInt iValue;
    TInt iError;
    };


// ============================ LOCAL CLASS MEMBER FUNCTIONS ================


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CAlfCenRep::CAlfCenRep( TInt aPriority, const TUid& aRepository, TInt aKey ) :
    CActive( aPriority ),
    iRepository( aRepository ),
    iKey( aKey )
    {
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CAlfCenRep::ConstructL()
    {
    iCenRep = CRepository::NewL( iRepository );
    CActiveScheduler::Add( this );
    RunL();
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CAlfCenRep* CAlfCenRep::NewL( TInt aPriority, const TUid& aRepository, TUint32 aKey )
    {
    CAlfCenRep* self = new (ELeave) CAlfCenRep( aPriority, aRepository, aKey );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CAlfCenRep::~CAlfCenRep()
    {
    Cancel();
    delete iCenRep;
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CAlfCenRep::RunL()
    {
    TInt value;
    iError = iCenRep->Get( iKey, value );
    if( iError == KErrNone )
    	{
    	iValue = value;
    	}
    iError = iCenRep->NotifyRequest( iKey, iStatus );
    
    if( iError == KErrNone )
    	{
    	SetActive();
    	}
    }
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TInt CAlfCenRep::GetValue( TInt& aValue )
	{
	if(iError != KErrNone) //if we had an error eariler
		{
		TInt value;
		iError = iCenRep->Get( iKey, value ); //try to get the value
		if(iError == KErrNone)
			{
			iValue = value;
			iError = iCenRep->NotifyRequest( iKey, iStatus ); //try to start request
			if(iError == KErrNone)
				{
				SetActive();
				}
			}
		}
	aValue = iValue;
	return iError;
	}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CAlfCenRep::DoCancel()
    {
    iCenRep->NotifyCancel( iKey );
    }



// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructors
// ---------------------------------------------------------------------------
//
CAlfSrvTransEffect::CAlfSrvTransEffect()
    {
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CAlfSrvTransEffect* CAlfSrvTransEffect::NewL()
    {
    CAlfSrvTransEffect* me = new (ELeave) CAlfSrvTransEffect();
    CleanupStack::PushL(me);
    me->ConstructL();
    CleanupStack::Pop();
    return me;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CAlfSrvTransEffect::ConstructL()
    {    
    // Load transition effect plugins
	PopulateEffectArrayL();
    
    if (iEffectPlugins.Count() > 0)
    	{
    	iIsEnabled = ETrue;
    	}
        
    // Start listening Avkon effect system state 
    iAvkonTfxStateListener = CAlfCenRep::NewL( 
    	EPriorityHigh, KCRUidThemes, KThemesTransitionEffects );   
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CAlfSrvTransEffect::~CAlfSrvTransEffect()
    {
    delete iAvkonTfxStateListener;
    
	iEffectPlugins.ResetAndDestroy();
	
	iEffectMap.Close();
    
   	REComSession::FinalClose(); 
    }


// -----------------------------------------------------------------------------
// Get a list of plugin implementations via ECom, construct the plugins and 
// add them to the array.
// -----------------------------------------------------------------------------
//
void CAlfSrvTransEffect::PopulateEffectArrayL()
	{
	// Get a list of all plugins from ECom
	TEComResolverParams resolverParams;
	resolverParams.SetDataType ( KAlfTransEffectPluginTypeStringDesc );
	resolverParams.SetWildcardMatch ( EFalse );
	RImplInfoPtrArray implInfoArray;
	REComSession::ListImplementationsL( KAlfTransEffectPluginInterfaceUid, 
		resolverParams, implInfoArray );
	CleanupResetAndDestroyPushL( implInfoArray ); // Note: intentionally called only after ListImplementationsL()
    
	__ALFLOGSTRING1("ALF: CAlfSrvTransEffect::PopulateExtensionArrayL(): extension count: %d",
		implInfoArray.Count())
	
	// Create each plugin via ECom and add it to the list 
	for ( TInt i = 0; i < implInfoArray.Count(); i++ )
		{		
		MAlfTransEffectPlugin* plugin = NULL;
		
		// Trap the construction as if one plugin fails to construct it 
		// should not stop others from constructing
		TRAPD( error, plugin = reinterpret_cast <MAlfTransEffectPlugin*> ( REComSession::CreateImplementationL( 
			implInfoArray[i]->ImplementationUid(), _FOFF (MAlfTransEffectPlugin, iDtor_ID_Key) ) ) );
	
		TInt countAddedEffects = 0;
		if ( !error && ( plugin != NULL ) )
			{
			RArray<TInt> effectIds;
			CleanupClosePushL( effectIds );
			
			plugin->GetSupportedEffects( effectIds );
			TInt count = effectIds.Count();
			if ( count > 0 )
				{
				for ( TInt ii = 0; ii < count; ii++ )
					{
					TInt mapError = iEffectMap.InsertInSignedKeyOrder( CAlfSrvTransEffect::TEffectMapItem( effectIds[ii], plugin ) );
					if ( mapError == KErrNone ) 
						{
						countAddedEffects++;
						}
					else // You cannot insert duplicate effects (with same effect ID)
						{
						__ALFLOGSTRING3("Alf: CAlfSrvTransEffect::PopulateExtensionArrayL. Cannot insert effect id to the map array. Imp.uid=%d, effect id=%d, error=%d", 						
							implInfoArray[i]->ImplementationUid().iUid, effectIds[ii], error)
						}
					}
				}
			CleanupStack::PopAndDestroy( &effectIds );
			}
		
		// Add plugin to the array if it has at least one effect that has been
		// successfully registered.	
		if ( !error && ( plugin != NULL ) && ( countAddedEffects > 0 ) )
			{
			iEffectPlugins.AppendL( plugin );
			}
		else
			{
			// If the plugin is failing we skip that.
			__ALFLOGSTRING3("Alf: CAlfSrvTransEffect::PopulateExtensionArrayL. Cannot add plugin, uid=%d, error=%d, plugin=%d", 
				implInfoArray[i]->ImplementationUid().iUid, error, plugin )
			__ALFLOGSTRING1("                                                  countAddedEffects=%d", 
				countAddedEffects )
			delete( plugin );
			}		
		} // for

	CleanupStack::PopAndDestroy( &implInfoArray );
	}


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool CAlfSrvTransEffect::IsEnabled()
	{
	TInt value;
	iAvkonTfxStateListener->GetValue( value );
	TBool isAvkonEffectsEnabled = ( value != KMaxTInt );
	return( iIsEnabled &&  !isAvkonEffectsEnabled );
	}    
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CAlfSrvTransEffect::StartPhase( 
    TInt aPhaseId, TInt aContext, 
    CAlfSrvEffectEnv& aEffectEnv, 
    TInt /*aUidValue*/, 
    TInt /*aKey*/, 
    TInt /*aUidValue2*/, 
    TInt /*aKey2*/, 
    const TDesC8* aParams)
	{
	if ( IsEnabled() )
		{		
		// Find the plugin which implements the effect related to this context and call it
		MAlfTransEffectPlugin* plugin = NULL;
		if ( FindPlugin( aContext,  &plugin ) == KErrNone )
			{
			// Update state
			iCurrContext = aContext;
			iCurrPhase = aPhaseId;
			
			// Call plugin
			ASSERT( plugin != NULL );
			plugin->StartPhase( aPhaseId, aContext, aEffectEnv, aParams );
			}
		}
		
	return KErrNone;	
	}    
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CAlfSrvTransEffect::Abort( TInt /*aContext*/, TInt /*aUidValue*/, TInt /*aKey*/ )
	{
	return KErrNone;
	}    
    
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CAlfSrvTransEffect::SetProperty( TInt /*aProperty*/, TInt /*aValue*/, TInt /*aUidValue*/, TInt /*aKey*/ )
	{
	return KErrNone;
	}    
    
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CAlfSrvTransEffect::GetProperty( TInt /*aProperty*/, TInt& /*aValue*/, TInt /*aUidValue*/, TInt /*aKey*/ )
	{
	return KErrNone;
	}    
    
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CAlfSrvTransEffect::AddEventObserver( MAlfTransEffectObserver* /*aObserver*/, TInt /*aEvents*/, const TDesC8* /*aParams*/ )
	{
	return KErrNone;
	}    
    
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CAlfSrvTransEffect::RemoveEventObserver( MAlfTransEffectObserver* /*aObserver*/, TInt /*aEvents*/ )
	{
	return KErrNone;
	}    
   
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CAlfSrvTransEffect::GetEventState( TInt /*aEvent*/, TInt* /*aState*/, TDes8* /*aParams*/ )
	{
	return KErrNotReady;
	}    
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CAlfSrvTransEffect::FindPlugin( TInt aContext, MAlfTransEffectPlugin** aPlugin ) 
	{
	// Note: Now the context id is the same as effect id, but this can change in the future
	CAlfSrvTransEffect::TEffectMapItem item( aContext, NULL );
	TInt index;
	if ( iEffectMap.FindInSignedKeyOrder( item, index ) == KErrNone )
		{
		*aPlugin = iEffectMap[index].iPluginPtr;
		return KErrNone;
		}
	else
		{
		return KErrNotFound;
		}
	}    
    
    

// End of file    

