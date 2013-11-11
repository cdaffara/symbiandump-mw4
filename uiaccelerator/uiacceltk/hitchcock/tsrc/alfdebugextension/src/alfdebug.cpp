/*
* Copyright (c)  Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  ?Description
*
*/


#include "alfdebugimpl.h"
#include "alfdebugextensionconstants.h"
#include "alfdebuguid.h"
#include <alf/alfgencomponent.h>
#include <alf/alfenv.h>


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
CAlfDebugExtensionImpl::CAlfDebugExtensionImpl()
    {
    }


// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
void CAlfDebugExtensionImpl::ConstructL( CAlfEnv& aEnv )
    {
    iEnv = &aEnv;
    
    TRAP_IGNORE( 
      iComms =
        CAlfGenComponent::NewL( 
            aEnv,
            EAlfDebugExtensionCreateDebug,
            KAlfDebugExtensionImplementationId,
            KNullDesC8 ) );
    }


// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
CAlfDebugExtensionImpl* CAlfDebugExtensionImpl::NewL( CAlfEnv& aEnv )
    {
    CAlfDebugExtensionImpl* self = new( ELeave ) CAlfDebugExtensionImpl;
    CleanupStack::PushL( self );
    self->ConstructL(aEnv);
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
CAlfDebugExtensionImpl::~CAlfDebugExtensionImpl()
    {
    delete iComms;
    }

// ---------------------------------------------------------------------------
// ?implementation_description
// ---------------------------------------------------------------------------
//
void CAlfDebugExtensionImpl::SetTimeFactor(TReal32 aTimeFactor)
    {
    TPckgC<TReal32> inParams(aTimeFactor);
    if ( iComms )
        {
        iComms->DoCmdNoReply( EAlfDebugCmdSetTimeFactor, inParams );
        }
    return;
    }

// ---------------------------------------------------------------------------
// ?implementation_description
// ---------------------------------------------------------------------------
//    
TReal32 CAlfDebugExtensionImpl::TimeFactor()
    {
    TReal32 result(1.f);
    if ( iComms )
        {
        TPckg<TReal32> outParams(result);
        iComms->DoSynchronousCmd( EAlfDebugCmdGetTimeFactor, KNullDesC8(), outParams );
        }
    return result;
    }

// ---------------------------------------------------------------------------
// ?implementation_description
// ---------------------------------------------------------------------------
//    
TUint CAlfDebugExtensionImpl::FrameCount()
    {
    TUint result(0);
    if ( iComms )
        {
        TPckg<TUint> outParams(result);
        iComms->DoSynchronousCmd( EAlfDebugCmdGetFrameCount, KNullDesC8(), outParams );
        }
    return result;
    }

// ---------------------------------------------------------------------------
// ?implementation_description
// ---------------------------------------------------------------------------
//    
TReal32 CAlfDebugExtensionImpl::FrameRate()
    {
    TReal32 result(0.f);
    if ( iComms )
        {
        TPckg<TReal32> outParams(result);
        iComms->DoSynchronousCmd( EAlfDebugCmdGetFrameRate, KNullDesC8(), outParams );
        }
    return result;
    }

// ---------------------------------------------------------------------------
// ?implementation_description
// ---------------------------------------------------------------------------
//     
void CAlfDebugExtensionImpl::ShowFrameRate( TInt aInterval )
    {
    TPckgC<TInt> inParams(aInterval);
    if ( iComms )
        {
        iComms->DoCmdNoReply( EAlfDebugCmdShowFrameRate, inParams );
        }
    return;
    }
    
void CAlfDebugExtensionImpl::ShowServerHeapUsage( TBool aShow )
    {
    TPckgC<TBool> inParams(aShow);
    if ( iComms )
        {
        iComms->DoCmdNoReply( EAlfDebugCmdShowServerHeap, inParams );
        }
    return;
    }
    

// ---------------------------------------------------------------------------
// Get measurements from the server side.
// ---------------------------------------------------------------------------
//     
TInt CAlfDebugExtensionImpl::GetMeasurements( 
        TAlfDebugServerMeasurements& aResult )
    {
    TInt err = KErrServerTerminated;
    
    if ( iComms )
        {
        TPckg<TAlfDebugServerMeasurements> outParams( aResult );
        err = iComms->DoSynchronousCmd( 
            EAlfDebugCmdMeasure, 
            KNullDesC8(), 
            outParams );
        }
        
    return err;
    }
    
CAlfDebugFactoryImpl::~CAlfDebugFactoryImpl()
    {
    delete iOwnEnv;
    }
    
CAlfDebugExtension* CAlfDebugFactoryImpl::CreateDebugExtensionL( CAlfEnv* aEnv )
    {
    CAlfEnv* usedEnv = aEnv;
    
    if ( !usedEnv )
        {
        // check static
        usedEnv = CAlfEnv::Static();
        if ( !usedEnv )
            {
            // create own
            iOwnEnv = CAlfEnv::NewL();
            usedEnv = iOwnEnv;
            }
        }
    
    return CAlfDebugExtensionImpl::NewL( *usedEnv );
    }


EXPORT_C CAlfDebugFactory* CreateDebugExtensionFactoryL()
    {
    return new (ELeave) CAlfDebugFactoryImpl;
    }
