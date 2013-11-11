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
* Description:   Environment
*
*/



#include "alf/alfsrveffectenv.h"
#include "alfappsrvsession.h"
#include "alfsrvcontrolgroupsubsession.h"
#include "alfsrvdisplaysubsession.h"
#include <uiacceltk/HuiControlGroup.h>
#include <uiacceltk/HuiEnv.h>

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructors
// ---------------------------------------------------------------------------
//
CAlfSrvEffectEnv::CAlfSrvEffectEnv(CAlfAppSrvSession& aSession)
    : iSession(&aSession)
    {
    }


void CAlfSrvEffectEnv::ConstructL()
    {
    }


CAlfSrvEffectEnv* CAlfSrvEffectEnv::NewL(CAlfAppSrvSession& aSession)
    {
    CAlfSrvEffectEnv* self = CAlfSrvEffectEnv::NewLC(aSession);
    CleanupStack::Pop( self );
    return self;
    }


CAlfSrvEffectEnv* CAlfSrvEffectEnv::NewLC(CAlfAppSrvSession& aSession)
    {
    CAlfSrvEffectEnv* self = new( ELeave ) CAlfSrvEffectEnv (aSession);
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CAlfSrvEffectEnv::~CAlfSrvEffectEnv()
    {
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CAlfSrvEffectEnv::ControlGroupCountL()
    {
    TInt count = 0;
    RPointerArray<CAlfSrvSubSessionBase> groups;
    CleanupClosePushL( groups );
    iSession->GetSubsessionsByTypeL( groups, EHuiObjectTypeControlGroup );
    count = groups.Count();
    CleanupStack::PopAndDestroy( &groups ); // groups.Close()        
    return count;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CAlfSrvEffectEnv::VisibleControlGroupCountL()
    {
    TInt count = 0;
    RPointerArray<CAlfSrvSubSessionBase> groups;
    CleanupClosePushL( groups );
    iSession->GetSubsessionsByTypeL( groups, EHuiObjectTypeControlGroup );
    for ( TInt g = 0 ; g < groups.Count() ; g++ )
        {
        // Check if the control group is shown (set by the application)
        CAlfSrvControlGroupSubSession* controlGroupSubSession = 
                static_cast<CAlfSrvControlGroupSubSession*>( groups[g] );        
        if ( controlGroupSubSession->IsShown() )
            {
            count++;
            }
        }
    CleanupStack::PopAndDestroy( &groups ); // groups.Close()        
    return count;
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C CHuiControlGroup* CAlfSrvEffectEnv::ControlGroupL( TInt aIndex )
    {
    CHuiControlGroup* retVal = NULL;
    RPointerArray<CAlfSrvSubSessionBase> groups;
    CleanupClosePushL( groups );
    iSession->GetSubsessionsByTypeL( groups, EHuiObjectTypeControlGroup );

    // Sanity check for index
    if (aIndex < groups.Count() && aIndex >= 0)
        {
        CAlfSrvControlGroupSubSession* controlGroupSubSession = 
            static_cast<CAlfSrvControlGroupSubSession*>( groups[aIndex] );
            
        retVal = &controlGroupSubSession->ControlGroup();            
        }
        
    CleanupStack::PopAndDestroy( &groups ); // groups.Close()        
    return retVal;        
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C CHuiControlGroup* CAlfSrvEffectEnv::VisibleControlGroupL( TInt aIndex )
    {
    CHuiControlGroup* retVal = NULL;
    TInt count = 0;
    RPointerArray<CAlfSrvSubSessionBase> groups;
    CleanupClosePushL( groups );
    iSession->GetSubsessionsByTypeL( groups, EHuiObjectTypeControlGroup );
    
    iSession->ReOrderControlGroupSessionsL( groups );
    
    for ( TInt g = 0 ; g < groups.Count() && !retVal ; g++ )
        {
        // Check if the control group is shown (set by the application)
        CAlfSrvControlGroupSubSession* controlGroupSubSession = 
                static_cast<CAlfSrvControlGroupSubSession*>( groups[g] );        
        if ( controlGroupSubSession->IsShown() )
            {
            if ( count == aIndex )
                {
                retVal = &controlGroupSubSession->ControlGroup();  
                }
            else
                {
                count++;
                }
            }
        }
    CleanupStack::PopAndDestroy( &groups ); // groups.Close()        
    return retVal;       
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CAlfSrvEffectEnv::DisplayCountL()
    {
    TInt count = 0;
    RPointerArray<CAlfSrvSubSessionBase> displays;
    CleanupClosePushL( displays );
    iSession->GetSubsessionsByTypeL( displays, EHuiObjectTypeDisplay );
    count = displays.Count() ; 
    CleanupStack::PopAndDestroy( &displays ); // displays.Close()
    return count;
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C CHuiDisplay* CAlfSrvEffectEnv::DisplayL(TInt aIndex)
    {
    // update display attributes for each display
    CHuiDisplay* display = NULL;
    RPointerArray<CAlfSrvSubSessionBase> displays;
    CleanupClosePushL( displays );
    iSession->GetSubsessionsByTypeL( displays, EHuiObjectTypeDisplay );

    // Sanity check for index
    if ( aIndex < displays.Count() && aIndex >= 0 )
        {
        CAlfSrvDisplaySubSession* displaySubSession = 
                static_cast<CAlfSrvDisplaySubSession*>( displays[aIndex] );     
        display = &displaySubSession->Display();
        }
    CleanupStack::PopAndDestroy( &displays ); // displays.Close()
    return display;
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C CHuiEnv* CAlfSrvEffectEnv::SharedEnv()
    {
    return iSession->SharedHuiEnv();    
    }

