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




// INCLUDE FILES
#include <e32base.h>
#include <bitdev.h>
#include <implementationproxy.h>

#include "alftfxplugincommon.hrh"
#include "alfsystemeffects.h"
#include "alf/alfsrveffectenv.h"
#include <alflogger.h>

#include "uiacceltk/HuiDisplay.h"
#include "uiacceltk/HuiLayout.h"
#include "uiacceltk/HuiControlGroup.h"
#include "uiacceltk/HuiControl.h"
#include "uiacceltk/HuiRoster.h"
#include "uiacceltk/HuiCommand.h"
#include "uiacceltk/HuiEnv.h"
#include "uiacceltk/HuiTransformation.h"

// Constants
const TInt KAlfDefaultFocusGainedEffectDuration = 500;
const TInt KAlfDefaultFocusLostEffectDuration = 500;

// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CAlfSystemEffects::CAlfSystemEffects
// C++ constuctor.
// -----------------------------------------------------------------------------
//
CAlfSystemEffects::CAlfSystemEffects( ) 
    
    {
    //LOGTEXT(_L("CAlfSystemEffects::CAlfSystemEffects()"));
    }


// -----------------------------------------------------------------------------
// CAlfSystemEffects::NewExtensionL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
MAlfTransEffectPlugin* CAlfSystemEffects::NewExtensionL()
    {
    __ALFFXLOGSTRING("CAlfSystemEffects::NewExtensionL >>");
    //LOGTEXT(_L("CAlfSystemEffects::NewExtensionL()"));
    CAlfSystemEffects* self = new(ELeave) CAlfSystemEffects();
    CleanupStack::PushL( self ); 
    self->ConstructL();
    CleanupStack::Pop( self );
    __ALFFXLOGSTRING("CAlfSystemEffects::NewExtensionL <<");
    return ( self );
    }


// -----------------------------------------------------------------------------
// CAlfSystemEffects::ConstructL
// Symbian 2nd phase constructor.
// -----------------------------------------------------------------------------
//
void CAlfSystemEffects::ConstructL()
    {
    //LOGTEXT(_L("CAlfSystemEffects::ConstructL()"));
    }


// -----------------------------------------------------------------------------
// CAlfSystemEffects::~CAlfSystemEffects
// C++ destructor.
// -----------------------------------------------------------------------------
//
CAlfSystemEffects::~CAlfSystemEffects()
    {
    __ALFFXLOGSTRING("CAlfSystemEffects::~CAlfSystemEffects");
    //LOGTEXT(_L("CAlfSystemEffects::~CAlfSystemEffects()"));
    }


// -----------------------------------------------------------------------------
// CAlfSystemEffects::GetSupportedEffects
// -----------------------------------------------------------------------------
//
TInt CAlfSystemEffects::GetSupportedEffects( RArray<TInt>& aSupportedEffectIds )
	{ 
	__ALFFXLOGSTRING("CAlfSystemEffects::GetSupportedEffects >>");
	// Add effect ids which are supported by this plugin
	aSupportedEffectIds.Append( MAlfTransEffectPlugin::EContextFocusGained ); 
	aSupportedEffectIds.Append( MAlfTransEffectPlugin::EContextFocusLost ); 
	aSupportedEffectIds.Append( MAlfTransEffectPlugin::EContextEmbeddedStart ); 
	aSupportedEffectIds.Append( MAlfTransEffectPlugin::EContextEmbeddedExit ); 
	__ALFFXLOGSTRING("CAlfSystemEffects::GetSupportedEffects - return KErrnone<<");	
	return KErrNone; 
	};
	

// -----------------------------------------------------------------------------
// CAlfSystemEffects::StartPhase
// -----------------------------------------------------------------------------
//
TInt CAlfSystemEffects::StartPhase( TInt aPhaseId, TInt aEffectId, 
    CAlfSrvEffectEnv& aEffectEnv, const TDesC8* aParams ) 
	{ 
	__ALFFXLOGSTRING("CAlfSystemEffects::StartPhase >>");
	TRAP_IGNORE( DoStartPhaseL( aPhaseId, aEffectId, aEffectEnv, aParams ));
	__ALFFXLOGSTRING("CAlfSystemEffects::StartPhase - return KErrNone <<");
	return KErrNone;
	}
	
	
// -----------------------------------------------------------------------------
// CAlfSystemEffects::DoStartPhaseL
// -----------------------------------------------------------------------------
//
TInt CAlfSystemEffects::DoStartPhaseL( TInt /*aPhaseId*/, TInt aEffectId, 
    CAlfSrvEffectEnv& aEffectEnv, const TDesC8* /*aParams*/ ) 
	{ 
	__ALFFXLOGSTRING1("CAlfSystemEffects::DoStartPhaseL - aEffectId: %d ", aEffectId );
	
	// Start effect here!
    switch (aEffectId)
        {
        case MAlfTransEffectPlugin::EContextFocusGained:
            {
            ShowVisibleGroupsAndMakeNonTransparentL( aEffectEnv );                           				
            break;
            }
            
        case MAlfTransEffectPlugin::EContextFocusLost:
            {
            HideGroupsAndMakeTransparentL( aEffectEnv );          
            break;
            }
            
        case MAlfTransEffectPlugin::EContextEmbeddedStart:
            {
            // do a normal show
            ShowVisibleGroupsAndMakeNonTransparentL( aEffectEnv );
            
            // in addition, do a expand effect for the visible groups
            TInt controlGroupCount = aEffectEnv.VisibleControlGroupCountL();
            for ( TInt g = 0 ; g < controlGroupCount ; g++ )
                {
                CHuiControlGroup* group = aEffectEnv.VisibleControlGroupL(g);
		        if (group)
                    {
                    // Perhaps we should show controlgroup in other displays too...?
                    CHuiDisplay* defaultDisplay = aEffectEnv.DisplayL(0);                
                    if (defaultDisplay)
                        {
                        CHuiLayout* hostContainer = group->Control(0).ContainerLayout( NULL );          
                       
                        // Cancel the previous 'disable transformation' commands.
                        aEffectEnv.SharedEnv()->CancelCommands( hostContainer, EHuiOpDisableTransformation );
                       
                        hostContainer->EnableTransformationL(); 
                        CHuiTransformation& xf = hostContainer->Transformation(); 
                        
                        THuiTimedValue scale(0);
                        scale.Set(1, KAlfDefaultFocusGainedEffectDuration);

                        // Clear previously created transformation steps.
                        xf.LoadIdentity();

                        xf.Scale(scale,scale);
                        
                        // Disable transformations after effect
                        THuiVisualCommand disableTransformationCommand( *hostContainer, EHuiOpDisableTransformation );
                        aEffectEnv.SharedEnv()->Send( disableTransformationCommand, KAlfDefaultFocusGainedEffectDuration );
                        }
                    }
                }            
            break;
            }
        case MAlfTransEffectPlugin::EContextEmbeddedExit:
            {
            // do normal hide
            HideGroupsAndMakeTransparentL( aEffectEnv );
            
            // in addition, do a shrink effect for the visible groups
            TInt controlGroupCount = aEffectEnv.VisibleControlGroupCountL();
            for ( TInt g = 0 ; g < controlGroupCount ; g++ )
                {
                CHuiControlGroup* group = aEffectEnv.VisibleControlGroupL(g);
		        if (group)
                    {
                    // Perhaps we should show controlgroup in other displays too...?
                    CHuiDisplay* defaultDisplay = aEffectEnv.DisplayL(0);                
                    if (defaultDisplay)
                        {
                        // Do a little pop out effect
                        CHuiLayout* hostContainer = group->Control(0).ContainerLayout( NULL );
                               
                        // Cancel the previous 'disable transformation' commands.
                        aEffectEnv.SharedEnv()->CancelCommands( hostContainer, EHuiOpDisableTransformation );                

                        hostContainer->EnableTransformationL(); 
                        CHuiTransformation& xf = hostContainer->Transformation(); 
                        
                        THuiTimedValue scale(1);
                        scale.Set(0, KAlfDefaultFocusLostEffectDuration);

                        // Clear previously created transformation steps.
                        xf.LoadIdentity();

                        xf.Scale(scale,scale);
                        
                        // Disable transformations after effect
                        THuiVisualCommand disableTransformationCommand( *hostContainer, EHuiOpDisableTransformation );
                        aEffectEnv.SharedEnv()->Send( disableTransformationCommand, KAlfDefaultFocusLostEffectDuration );
                        }
                    }
                }            
            break;
            }
       default:
            {
            
            break;
            }        
        }
	__ALFFXLOGSTRING("CAlfSystemEffects::DoStartPhaseL - return KErrNone <<");
	return KErrNone; 
	};
    
void CAlfSystemEffects::ShowVisibleGroupsAndMakeNonTransparentL(CAlfSrvEffectEnv& aEffectEnv)
    {
    __ALFFXLOGSTRING("CAlfSystemEffects::ShowVisibleGroupsAndMakeNonTransparentL >>");
    // Set opacity of every control group to 1, even if it is not shown because
    // application may want to show it later and it cannot control the
    // opacity.
    TInt controlGroupCount = aEffectEnv.ControlGroupCountL();
    for ( TInt g = 0 ; g < controlGroupCount ; g++ )
        {
        CHuiControlGroup* group = aEffectEnv.ControlGroupL(g);
        if (group)
            {
            // Perhaps we should show controlgroup in other displays too...?
            CHuiDisplay* defaultDisplay = aEffectEnv.DisplayL(0);                
            if (defaultDisplay)
                {
                // Set the root visual opacity to 1 which affects all the visuals in the group
                CHuiLayout* hostContainer = group->Control(0).ContainerLayout( NULL );                                                
                hostContainer->iOpacity.Set(1.f, KAlfDefaultFocusGainedEffectDuration);                                                
                
                // Disable also previous transformations.
                hostContainer->EnableTransformationL(EFalse); 
                }
            }
        }   
        
    // Check how many control groups are shown (set visible by the application)
    
    // todo: the visible control group should be shown in the same order as they were
    // set into the roster originally.
    TInt visibleControlGroupCount = aEffectEnv.VisibleControlGroupCountL();
    for ( TInt g = 0 ; g < visibleControlGroupCount ; g++ )
        {
        CHuiControlGroup* group = aEffectEnv.VisibleControlGroupL(g);
        if (group)
            {
            CHuiDisplay* defaultDisplay = aEffectEnv.DisplayL(0);                
            if (defaultDisplay)
                {
                // Cancel the previous hide commands.
                aEffectEnv.SharedEnv()->CancelCommands( group, EHuiOpHide );
                
                // Show control group ! 
                defaultDisplay->Roster().ShowL(*group);                    
                group->SetAcceptInput(EFalse);
                }
            }
        }  
    __ALFFXLOGSTRING("CAlfSystemEffects::ShowVisibleGroupsAndMakeNonTransparentL <<");
    }
    
void CAlfSystemEffects::HideGroupsAndMakeTransparentL(CAlfSrvEffectEnv& aEffectEnv)
    {
    __ALFFXLOGSTRING("CAlfSystemEffects::HideGroupsAndMakeTransparentL >>");
    TInt controlGroupCount = aEffectEnv.ControlGroupCountL();
    for ( TInt g = 0 ; g < controlGroupCount ; g++ )
        {
        CHuiControlGroup* group = aEffectEnv.ControlGroupL(g);
        if (group)
            {
            // Perhaps we should show controlgroup in other displays too...?
            CHuiDisplay* defaultDisplay = aEffectEnv.DisplayL(0);                
            if (defaultDisplay)
                {
                // Set the root visual opacity to 0 which affects all the visuals in the group
                CHuiLayout* hostContainer = group->Control(0).ContainerLayout( NULL );
                hostContainer->iOpacity.Set(0.f, KAlfDefaultFocusGainedEffectDuration);   
                                                             
				if (defaultDisplay->DisplayType() != CHuiDisplay::EDisplayOffScreenBuffer)
					{
					// Hide the group so that it does not use resources when we dont have focus
                    THuiGroupCommand command( *group, EHuiOpHide, defaultDisplay );
                    aEffectEnv.SharedEnv()->Send(command, KAlfDefaultFocusLostEffectDuration);
					}
                }
            }
        } 
    __ALFFXLOGSTRING("CAlfSystemEffects::HideGroupsAndMakeTransparentL <<");
    }

//  End of File  
