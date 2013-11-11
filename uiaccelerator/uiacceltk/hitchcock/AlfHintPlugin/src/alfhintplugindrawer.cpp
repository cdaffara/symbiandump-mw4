/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*
*/

#include <ecom/implementationproxy.h>
#include <s32mem.h> // for RDesReadStream
#include "alfhintplugindrawer.h"
#include <alfhintapi.h>

LOCAL_C const TImplementationProxy KImplementationTable[] = 
    {
    // TODO: CHANGE ID
    IMPLEMENTATION_PROXY_ENTRY(CAlfHintGraphicDrawer::EImplUid, CAlfHintGraphicDrawer::CreateL),
    };

// ---------------------------------------------------------------------------
// ImplementationGroupProxy
// ---------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = (sizeof(KImplementationTable) / sizeof(TImplementationProxy));
    return KImplementationTable;
    }

// ---------------------------------------------------------------------------
// CreateL
// ---------------------------------------------------------------------------
//
CAlfHintGraphicDrawer* CAlfHintGraphicDrawer::CreateL()
    {
    return new(ELeave) CAlfHintGraphicDrawer;
    }

// ---------------------------------------------------------------------------
// CAlfHintGraphicDrawer
// ---------------------------------------------------------------------------
//
CAlfHintGraphicDrawer::CAlfHintGraphicDrawer()
    {
    }

// ---------------------------------------------------------------------------
// ~CAlfHintGraphicDrawer
// ---------------------------------------------------------------------------
//
CAlfHintGraphicDrawer::~CAlfHintGraphicDrawer()
    {
    }

// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CAlfHintGraphicDrawer::ConstructL(MWsGraphicDrawerEnvironment& aEnv, const TGraphicDrawerId& aId, 
                                MWsClient& aOwner, const TDesC8& aData)
    {
    iEnv = &aEnv;
    BaseConstructL(aEnv, aId, aOwner);
    iHintApi = static_cast<MAlfHintObserver*>(aEnv.Screen(0)->ResolveObjectInterface( KAlfHintObjectInterface ) );
    /*RDesReadStream in(aData);
    TInt windowHandle = in.ReadInt32L();
*/
    User::LeaveIfError(ShareGlobally());
    }
    
// ---------------------------------------------------------------------------
// ResolveObjectInterface
// ---------------------------------------------------------------------------
//
TAny* CAlfHintGraphicDrawer::ResolveObjectInterface(TUint aTypeId)
    {
    switch(aTypeId)
        {
        default:
            return CWsGraphicDrawer::ResolveObjectInterface(aTypeId);
        }
    }   

// ---------------------------------------------------------------------------
// DoDraw
// ---------------------------------------------------------------------------
//
void CAlfHintGraphicDrawer::DoDraw(MWsGc& aGc, const TRect& aRect, const TDesC8& aData) const
    {
    // read data
    TInt err = 0;
    TInt tag = KErrNotFound;
    TRect boundingRectangle(0,0,360,240);
    TInt layerId = 0;
    TInt extraParameters = 0;
    TRAP( err, 
    RDesReadStream in(aData);
    tag = in.ReadInt8L();
    extraParameters = in.ReadInt8L();
    in.ReadL((TUint8*)&boundingRectangle,sizeof(TRect));
    layerId = in.ReadInt32L();
    );
    
    if (iHintApi) 
        {
        if (!extraParameters)
            {
            iHintApi->InsertTagL( tag );
            }
        else
            {
            iHintApi->InsertTagL( tag, boundingRectangle, layerId );
            }
        }
    }

// ---------------------------------------------------------------------------
// HandleMessage
// ---------------------------------------------------------------------------
//
void CAlfHintGraphicDrawer::HandleMessage(const TDesC8& aData)
    {
    }
    
