/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*      Implementation of class RFeatMgr.   
*      
*
*/


// INCLUDE FILES

#include <featmgr.h>
#include "RFeatMgr.h"
#include "CodPanic.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// RFeatMgr::RFeatMgr()
// ---------------------------------------------------------
//
RFeatMgr::RFeatMgr(): iOpen( EFalse )
    {
    }

// ---------------------------------------------------------
// RFeatMgr::Close()
// ---------------------------------------------------------
//
void RFeatMgr::Close()
    {
    if ( iOpen )
        {
        FeatureManager::UnInitializeLib();
        iOpen = EFalse;
        }
    }

// ---------------------------------------------------------
// RFeatMgr::OpenL()
// ---------------------------------------------------------
//
void RFeatMgr::OpenL()
    {
    __ASSERT_DEBUG( !iOpen, CodPanic( ECodInternal ) );
    FeatureManager::InitializeLibL();
    iOpen = ETrue;
    }

// ---------------------------------------------------------
// RFeatMgr::FeatureSupported()
// ---------------------------------------------------------
//
TBool RFeatMgr::FeatureSupported( TInt aFeature )
    {
    __ASSERT_DEBUG( iOpen, CodPanic( ECodInternal ) );
    return FeatureManager::FeatureSupported( aFeature );
    }
