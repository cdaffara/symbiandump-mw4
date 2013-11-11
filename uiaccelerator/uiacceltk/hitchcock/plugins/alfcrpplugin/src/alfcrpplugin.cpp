/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Alf CRP ECOM plugin
*
*/


#include <bldvariant.hrh>

#include "alfcrpplugin.h"
#include <ecom/implementationproxy.h>
#include <s32mem.h> //RDesReadStream

#include "alfsynchronizer.h"
#include "alfrenderstageutils.h"

// CONSTANTS
const TInt KImpId( 0x2002C358 );
const TInt KAlfCrpSynchronize = 0;

// --------------------------------------------------------------------------
// CAlfCrpPlugin::CreateL
// --------------------------------------------------------------------------
//
CWsGraphicDrawer* CAlfCrpPlugin::CreateL()
    {
    CAlfCrpPlugin* crp = new (ELeave) CAlfCrpPlugin();
    return crp;
    }

// --------------------------------------------------------------------------
// CAlfCrpPlugin::~CAlfCrpPlugin
// --------------------------------------------------------------------------
//    
CAlfCrpPlugin::~CAlfCrpPlugin()    
    {
    }

// --------------------------------------------------------------------------
// CAlfCrpPlugin::DoDraw
// --------------------------------------------------------------------------
//
void CAlfCrpPlugin::DoDraw( MWsGc& /*aGc*/, const TRect& /*aRect*/, 
    const TDesC8& /*aData*/) const
    {
    // Draws nothing
    }

// --------------------------------------------------------------------------
// CAlfCrpPlugin::HandleMessage
// --------------------------------------------------------------------------
//    
void CAlfCrpPlugin::HandleMessage( const TDesC8& aData )
    {
    TRAP_IGNORE( DoHandleMessageL( aData ) );
    }
    
// --------------------------------------------------------------------------
// CAlfCrpPlugin::DoHandleMessageL
// --------------------------------------------------------------------------
//    
void CAlfCrpPlugin::DoHandleMessageL( const TDesC8& aData )
    {  
    RDesReadStream in( aData );
    switch( in.ReadInt32L() )
        {
        case KAlfCrpSynchronize:
            {
            MAlfSynchronizationInterface* synchronizer = NULL;            
            if ( Env().ScreenCount() )
                {
                MWsScreen* screen = Env().Screen(0);
                if ( screen )
                    {
                    synchronizer = 
                        (MAlfSynchronizationInterface*)screen->ResolveObjectInterface(
                            KAlfSynchronizationInterfaceUid);
                    }
                }
            
            if ( synchronizer )
                {
                _LIT_SECURITY_POLICY_S0(KAlfSynchronizerPolicy, 0x10003B20);
                RProperty::Define( KAlfPSUidSynchronizer, KAlfPSKeySynchronizer, RProperty::EInt, KAlfSynchronizerPolicy, KAlfSynchronizerPolicy );
                RProperty::Set( KAlfPSUidSynchronizer, KAlfPSKeySynchronizer, 0 );
                    
                CAlfSynchronizer* synch = CAlfSynchronizer::NewL();
                CleanupStack::PushL( synch );
                TInt syncId = 0;
                synchronizer->Synchronize(syncId);
                synch->Start( syncId );
                            
                CleanupStack::PopAndDestroy( synch );
                }
            }
            break;
        default:
            break;
        }
    }

// --------------------------------------------------------------------------
// CAlfCrpPlugin::ConstructL
// --------------------------------------------------------------------------
//    
void CAlfCrpPlugin::ConstructL( MWsGraphicDrawerEnvironment& aEnv,
    const TGraphicDrawerId& aId, MWsClient& aOwner, const TDesC8& /*aData*/ )
    {
    BaseConstructL( aEnv, aId, aOwner );

    ShareGlobally();
    }

// --------------------------------------------------------------------------
// KImplementationTable
// --------------------------------------------------------------------------
//    
LOCAL_C const TImplementationProxy KImplementationTable[] = 
    {
    IMPLEMENTATION_PROXY_ENTRY(KImpId, CAlfCrpPlugin::CreateL)
    };

// --------------------------------------------------------------------------
// ImplementationGroupProxy
// --------------------------------------------------------------------------
//    
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = (sizeof(KImplementationTable) / sizeof(TImplementationProxy));
    return KImplementationTable;
    }

// End of file
