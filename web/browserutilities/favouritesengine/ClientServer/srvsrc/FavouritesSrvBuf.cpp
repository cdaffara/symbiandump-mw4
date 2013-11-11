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
*      Implementation of class HFavouritesSrvBuf
*      
*
*/


// INCLUDE FILES

#include "FavouritesSrvBuf.h"

// CONSTANTS

/// Buffer granularity.
LOCAL_D const TInt KFavouritesSrvBufGranularity = 512;

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// HFavouritesSrvBuf::NewL()
// ---------------------------------------------------------
//
HFavouritesSrvBuf* HFavouritesSrvBuf::NewL()
    {
    HFavouritesSrvBuf* buf = NewLC();
    CleanupStack::Pop();    // buf
    return buf;
    }

// ---------------------------------------------------------
// HFavouritesSrvBuf::NewLC()
// ---------------------------------------------------------
//
HFavouritesSrvBuf* HFavouritesSrvBuf::NewLC()
    {
    HFavouritesSrvBuf* buf = new (ELeave) HFavouritesSrvBuf;
    buf->PushL();
    buf->ConstructL();
    return buf;
    }

// ---------------------------------------------------------
// HFavouritesSrvBuf::ConstructL()
// ---------------------------------------------------------
//
void HFavouritesSrvBuf::ConstructL()
    {
    iBuf = CBufSeg::NewL( KFavouritesSrvBufGranularity );
    Set( *iBuf, 0, ERead | EWrite );
    }

// ---------------------------------------------------------
// HFavouritesSrvBuf::DoRelease()
// ---------------------------------------------------------
//
void HFavouritesSrvBuf::DoRelease()
    {
    delete this;
    }

//  End of File  
