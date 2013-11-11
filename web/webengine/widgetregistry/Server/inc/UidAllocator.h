/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Class which allocates random UIDs from a given range
*
*/


#ifndef UIDALLOCATOR_H
#define UIDALLOCATOR_H

// INCLUDE FILES
#include "WidgetRegistryConstants.h"

/**
 * This class allows clients to allocate a UID from a given range.
 * @internalComponent
 */
class TUidAllocator
    {
public:
    /**
    * Allocates a single UID and returns it or leaves with KErrNotFound.
    */
    TInt AllocateL( const RUidArray& aUsedUids, TInt aDriveLetter );

    static TBool IsWidget( TUid aUid );
    };
#endif

