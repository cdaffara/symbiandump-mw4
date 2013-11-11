/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Is a serialized version of CFeed suitable for transfer between
*                client and server.
*
*/


#include "PackedAttributes.h"
#include "FeedAttributes.h"
#include "PackedFeed.h"
#include "Logger.h"


const TInt KTokenArrayIncrementSize = 1000;
const TInt KStringTableIncrementSize = 10240;


// -----------------------------------------------------------------------------
// CPackedFeed::NewL
//
// Two-phased constructor.  Used on both the client and server.
// -----------------------------------------------------------------------------
//
CPackedFeed* CPackedFeed::NewL()
    {
    CPackedFeed* self = new (ELeave) CPackedFeed(KTokenArrayIncrementSize, 
            KStringTableIncrementSize);

    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

        
// -----------------------------------------------------------------------------
// CPackedFeed::CPackedFeed
//
// C++ default constructor can NOT contain any code, that
// might leave.  Used on both the client and server.
// -----------------------------------------------------------------------------
//
CPackedFeed::CPackedFeed(TInt aTokenArrayInc, TInt aStringTableInc):
        CPacked(aTokenArrayInc, aStringTableInc), iLeakTracker(CLeakTracker::EPackedFeed)
    {
    }
        

// -----------------------------------------------------------------------------
// CPackedFeed::ConstructL
//
// Symbian 2nd phase constructor can leave.  Used on both the client and server.
// -----------------------------------------------------------------------------
//
void CPackedFeed::ConstructL()
    {
    BaseConstructL();
    }
        

// -----------------------------------------------------------------------------
// CPackedFeed::~CPackedFeed
//
// Deconstructor.  Used on both the client and server.
// -----------------------------------------------------------------------------
//
CPackedFeed::~CPackedFeed()
    {
    }

        
// -----------------------------------------------------------------------------
// CPackedFeed::FeedBeginsL
//
// Insert the begining of a feed.   Used only on the server.
// -----------------------------------------------------------------------------
//
void CPackedFeed::FeedBeginsL()
    {
    InsertTokenL(EFeedTokenFeedBegin);
    }


// -----------------------------------------------------------------------------
// CPackedFeed::FeedEndsL
//
// Insert the ending of a feed.    Used only on the server.
// -----------------------------------------------------------------------------
//
void CPackedFeed::FeedEndsL()
    {
    InsertTokenL(EFeedTokenFeedEnd);
    
    // Done packing the feed so wrap a descriptor around iTokenArray.
    iTokenArrayAsDes.Set((const TUint8*) iTokenArray, iTokenArrayLength * sizeof(TUint));
    }


// -----------------------------------------------------------------------------
// CPackedFeed::ItemBeginsL
//
// Insert the begining of a item.   Used only on the server.
// -----------------------------------------------------------------------------
//
void CPackedFeed::ItemBeginsL()
    {
    InsertTokenL(EFeedTokenItemBegin);
    }


// -----------------------------------------------------------------------------
// CPackedFeed::ItemEndsL
//
// Insert the ending of a item.   Used only on the server.
// -----------------------------------------------------------------------------
//
void CPackedFeed::ItemEndsL()
    {
    InsertTokenL(EFeedTokenItemEnd);
    }


// -----------------------------------------------------------------------------
// CPackedFeed::EnclosureBeginsL
//
// Insert the begining of a enclosure.   Used only on the server.
// -----------------------------------------------------------------------------
//
void CPackedFeed::EnclosureBeginsL()
    {
    InsertTokenL(EFeedTokenEnclosureBegin);
    }


// -----------------------------------------------------------------------------
// CPackedFeed::EnclosureEndsL
//
// Insert the ending of a enclosure.   Used only on the server.
// -----------------------------------------------------------------------------
//
void CPackedFeed::EnclosureEndsL()
    {
    InsertTokenL(EFeedTokenEnclosureEnd);
    }


// -----------------------------------------------------------------------------
// CPackedFeed::OtherTitleL
//
// Insert an unimportant token.  The client can use this information for feed 
// validation. Used only on the server.
// -----------------------------------------------------------------------------
//
void CPackedFeed::OtherTitleL()
    {
    InsertTokenL(EFeedTokenOtherTitle);
    }


// -----------------------------------------------------------------------------
// CPackedFeed::OtherDescriptionL
//
// Insert an unimportant token.  The client can use this information for feed 
// validation. Used only on the server.
// -----------------------------------------------------------------------------
//
void CPackedFeed::OtherDescriptionL()
    {
    InsertTokenL(EFeedTokenOtherDescription);
    }


// -----------------------------------------------------------------------------
// CPackedFeed::OtherLinkL
//
// Insert an unimportant token.  The client can use this information for feed 
// validation. Used only on the server.
// -----------------------------------------------------------------------------
//
void CPackedFeed::OtherLinkL()
    {
    InsertTokenL(EFeedTokenOtherLink);
    }



