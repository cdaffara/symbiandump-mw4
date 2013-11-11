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



/**
 * @file
 *
 * Simple UUID generation and formatting support
 */

#ifndef __UUID_H__
#define __UUID_H__

const TInt KNodeMacOctets = 6;
const TInt KUuidStringLen = 36;

typedef TBuf<KUuidStringLen> TUuidString;
typedef TBuf8<KUuidStringLen> TUuidString8;

// Example UUID string: f81d4fae-7dec-11d0-a765-00a0c91e6bf6
_LIT(KUuidFormatString,   "%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x");
_LIT8(KUuidFormatString8, "%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x");

/**
 * Generally known UUID versions.
 * NOTE. This implementation supports
 * only version 4 (ERandomBased)
 */
enum TUuidVersion
    {
    ETimeBased      = (1 << 0), ///< v1, time-based version
    EDceSecurity    = (1 << 1), ///< v2, DCE Security version
    ENameBasedMd5   = (1 << 2), ///< v3, name-based version with MD5 hashing
    ERandomBased    = (1 << 3), ///< v4, randomly or pseudo-randomly generated version
    ENameBasedSha1  = (1 << 4)  ///< v5, name-based version with SHA-1 hashing
    };

class TUuid
/**
 * A value type class that represents a UUID
 */
    {
public:    
    TUint32 iTimeLow;               /* bits  0-31 of time field */
    TUint16 iTimeMid;               /* bits 32-47 of time field */
    TUint16 iTimeHiAndVersion;      /* bits 48-59 of time field plus 4 bit version */
    TUint8  iClockSeqHiAndReserved; /* bits  8-13 of clock sequence field plus 2 bit variant */
    TUint8  iClockSeqLow;           /* bits  0-7  of clock sequence field */
    TUint8  iNode[KNodeMacOctets];  /* bits  0-47 of node MAC address */
    };

class Uuid
/**
 * A class for creating UUIDs and formatting them as strings
 */
    {
public:    
    static void MakeUuidL(TUuid& aUuid, TUuidVersion aVersion = ERandomBased);
    static void UuidToString(const TUuid& aUuid, TUuidString& aUuidString);
    static void UuidToString(const TUuid& aUuid, TUuidString8& aUuidString);
    
private:
    static void RandomizeL(TUuid& aUuid);
    static void Brand(TUuid& aUuid, TUuidVersion aVersion);
    };

#endif // __UUID_H__
