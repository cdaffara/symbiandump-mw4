/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Create a new UID for the policy.
*
*/



#include <random.h>

#include "uuid.h"

void Uuid::MakeUuidL(TUuid& aUuid, TUuidVersion aVersion)
/**
 * Generate a UUID of the specified version.
 * NOTE. As of this writing, only version 4
 * (ERandomBased) is supported
 **/
    {
    // We currently support only version 4
    if (aVersion != ERandomBased)
        {
        User::Leave(KErrNotSupported);
        }
    
    /* Fill UUID with random data */
    RandomizeL(aUuid);

    /* Brand UUID with version and variant */
    Brand(aUuid, aVersion);
    }

void Uuid::RandomizeL(TUuid& aUuid)
/**
 * Fill the UUID with pseudo-random data
 */
    {
    // Delegate the task to the Symbian Random Number Generator
    // (we trust that it is the best alternative available)
    
    TPckg<TUuid> pckgUuid(aUuid);
    TRandom::RandomL(pckgUuid);
    }

void Uuid::Brand(TUuid& aUuid, TUuidVersion aVersion)
/**
 * Set variant and version information to the UUID
 */
    {
    // We currently support only UUID version 4
    if (aVersion != ERandomBased)
        {
        return;
        }

    // First set the variant information to value:
    //     Msb0     Msb1    Msb2
    //     1        0       x
    // as specified in draft-mealling-uuid-urn-05.txt
    // (a variable number of the most significant bits
    // of the eight octet of the UUID, i.e. the
    // clock_seq_hi_and_reserved octet)
    aUuid.iClockSeqHiAndReserved &= 0x3F;     // Set Msb0 and Msb1 to 0, take the rest "as-is"
    aUuid.iClockSeqHiAndReserved |= (1 << 7); // Set Msb0 to 1

    // Then set the version number to indicate version 4
    //     Msb0     Msb1    Msb2    Msb3
    //     0        1       0       0
    // as specified in draft-mealling-uuid-urn-05.txt
    // (in the most significant four bits of the time
    // stamp)
    aUuid.iTimeHiAndVersion &= 0x0FFF;    // Set Msb0-Msb3 to 0, take the rest "as-is"
    aUuid.iTimeHiAndVersion |= (1 << 14); // Set Msb1 to 1
    }

void Uuid::UuidToString(const TUuid& aUuid, TUuidString& aUuidString)
/**
 * Create and return a string representation of the given UUID.
 * E.g. f81d4fae-7dec-11d0-a765-00a0c91e6bf6
 */
    {
    aUuidString.Format(KUuidFormatString,
                       static_cast<TUint>(aUuid.iTimeLow),
                       static_cast<TUint>(aUuid.iTimeMid),
                       static_cast<TUint>(aUuid.iTimeHiAndVersion),
                       static_cast<TUint>(aUuid.iClockSeqHiAndReserved),
                       static_cast<TUint>(aUuid.iClockSeqLow),
                       static_cast<TUint>(aUuid.iNode[0]),
                       static_cast<TUint>(aUuid.iNode[1]),
                       static_cast<TUint>(aUuid.iNode[2]),
                       static_cast<TUint>(aUuid.iNode[3]),
                       static_cast<TUint>(aUuid.iNode[4]),
                       static_cast<TUint>(aUuid.iNode[5]));
    }

void Uuid::UuidToString(const TUuid& aUuid, TUuidString8& aUuidString)
/**
 * Create and return a string representation of the given UUID.
 * E.g. f81d4fae-7dec-11d0-a765-00a0c91e6bf6
 */
    {
    aUuidString.Format(KUuidFormatString8,
                       static_cast<TUint>(aUuid.iTimeLow),
                       static_cast<TUint>(aUuid.iTimeMid),
                       static_cast<TUint>(aUuid.iTimeHiAndVersion),
                       static_cast<TUint>(aUuid.iClockSeqHiAndReserved),
                       static_cast<TUint>(aUuid.iClockSeqLow),
                       static_cast<TUint>(aUuid.iNode[0]),
                       static_cast<TUint>(aUuid.iNode[1]),
                       static_cast<TUint>(aUuid.iNode[2]),
                       static_cast<TUint>(aUuid.iNode[3]),
                       static_cast<TUint>(aUuid.iNode[4]),
                       static_cast<TUint>(aUuid.iNode[5]));
    }
