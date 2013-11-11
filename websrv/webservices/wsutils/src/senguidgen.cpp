/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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








// INCLUDE FILES
#include <hash.h> 
#include <e32math.h>
#include <f32file.h> // TVolumeInfo
#include "senguidgen.h"
#include "sendebug.h"
#include "senlogger.h"

CSenGuidGen::CSenGuidGen()
    {
    TTime homeTime;
    homeTime.HomeTime();
    iSeed = homeTime.Int64();
    }

void CSenGuidGen::ConstructL()
    {
    TLSLOG_L(KSenUtilsLogChannel, KMinLogLevel, "CSenGuidGen::ConstructL");
    // Get host name
    //ipHostId = _L8("localhost").AllocL();
    ipHostId = HBufC8::NewL( KSenUuidMaxHostIdLength );
    TPtr8 hostID = ipHostId->Des();
    
    TInt error(KErrNone);

    
    RFs rfs;
    User::LeaveIfError(rfs.Connect());
#ifndef __WINS__ // only required in RELEASE builds    
    TLSLOG_L(KSenUtilsLogChannel, KNormalLogLevel, "CSenGuidGen::ConstructL - about to call RFs.GetMediaSerialNumber()");
    TMediaSerialNumber mediaID;
    error = rfs.GetMediaSerialNumber(mediaID, EDriveC);
    TLSLOG_FORMAT((KSenUtilsLogChannel, KMinLogLevel, _L8("CSenGuidGen::ConstructL - GetMediaSerialNumber() returned: %d"), error));
    if( !error )
        {
        hostID.Append(mediaID);
        }
#else // WINS (emulator) environment
    TLSLOG_L(KSenUtilsLogChannel, KNormalLogLevel, "CSenGuidGen::ConstructL - about to call RFs.Volume()");
    TVolumeInfo info;
    error = rfs.Volume(info);
    TLSLOG_FORMAT((KSenUtilsLogChannel, KMinLogLevel, _L8("CSenGuidGen::ConstructL - Volume() returned: %d"), error));
    if( !error )
        {
        hostID.AppendNum(info.iUniqueID);
        }
#endif    
    rfs.Close();

    if( error )
        {
        hostID.Append( _L8("localhost") );
        }
    TLSLOG_L(KSenUtilsLogChannel, KMinLogLevel, "CSenGuidGen::ConstructL - host ID:");
    TLSLOG(KSenUtilsLogChannel, KMinLogLevel, hostID);
    // Create hash generator
    ipMd5HashGen = CMD5::NewL();
    }

CSenGuidGen::~CSenGuidGen()
    {
    delete ipMd5HashGen;
    delete ipHostId;
    }

EXPORT_C CSenGuidGen* CSenGuidGen::NewLC()
    {
    CSenGuidGen* pNew = new (ELeave) CSenGuidGen;
    CleanupStack::PushL(pNew);
    pNew->ConstructL();
    return pNew;
    }

EXPORT_C CSenGuidGen* CSenGuidGen::NewL()
    {
    CSenGuidGen* pNew = NewLC();
    CleanupStack::Pop(); // pNew;
    return pNew;
    }

EXPORT_C HBufC* CSenGuidGen::GetRandomGuidLC()
    {
#ifdef _UNICODE
    return GetRandomGuid16LC();
#else
    return GetRandomGuid8LC();
#endif
    }


EXPORT_C HBufC8* CSenGuidGen::GetRandomGuid8LC()
    {
    TPtrC8 hash = GetRandomHash();
    HBufC8* pResult = HBufC8::NewLC(hash.Length() * KSenUuidHexWidth + KSenUuidNumOfSlashes + KSenUuidPrefix().Length());
    TPtr8 result = pResult->Des();

    // Ensure that resulting uuid is a URI as well as an NCName
    result.Append(KSenUuidPrefix) ;

    for (TInt i = 0; i < hash.Length(); i++)
        {
        if ((i == 4) || (i == 6) || (i == 8) || (i == 10)) // four slashes
            {
            result.Append('-');
            }
        result.AppendNumFixedWidth(hash[i], EHex, KSenUuidHexWidth); // hash[i] as hex!, doubles the hash length
        }
    result.UpperCase();
    return pResult;
    }


EXPORT_C HBufC16* CSenGuidGen::GetRandomGuid16LC()
    {
    TPtrC8 hash = GetRandomHash();
    HBufC16* pResult = HBufC16::NewLC(hash.Length() * KSenUuidHexWidth + KSenUuidNumOfSlashes + KSenUuidPrefix().Length());
    TPtr16 result = pResult->Des();

    // Ensure that resulting uuid is a URI as well as an NCName

    TBuf16<KSenUuidPrefixLength> uuidPrefix16; // KSenUuidPrefixLength == KSenUuidPrefix().Length()
    
    uuidPrefix16.Copy(KSenUuidPrefix); // OK, since there are no Unicode characters in KSenUuidPrefix
    result.Append(uuidPrefix16);
    
    for (TInt i = 0; i < hash.Length(); i++)
        {
        if ((i == 4) || (i == 6) || (i == 8) || (i == 10)) // four slashes
            {
            result.Append('-');
            }
        result.AppendNumFixedWidth(hash[i], EHex, KSenUuidHexWidth); // hash[i] as hex!, doubles the hash length
        }
    result.UpperCase();
    return pResult;
    }

TPtrC8 CSenGuidGen::GetRandomHash()
    {
    TBuf8<KSenUuidMaxHashLength> valueBeforeMd5;

    // Append host id
    valueBeforeMd5.Append(*ipHostId);
    valueBeforeMd5.Append(':');

    // Append current system time
    TTime homeTime;
    homeTime.HomeTime();
    valueBeforeMd5.AppendNum(homeTime.Int64());
    valueBeforeMd5.Append(':');

    // Append random number
    valueBeforeMd5.AppendNum(Math::Rand(iSeed));

    // Calculate hash
    TPtrC8 hash = ipMd5HashGen->Hash(valueBeforeMd5);
    return hash;
    }

// End of File
