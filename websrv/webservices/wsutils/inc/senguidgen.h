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
* Description: Header declaration
*
*/










#ifndef CCGUIDGEN_H
#define CCGUIDGEN_H

// INCLUDES
#include <e32base.h>
#include <e32std.h>

// CONSTANTS
_LIT8(KSenUuidPrefix, "uuid:"); // GUID is valid URN
const TInt KSenUuidMaxHostIdLength = 64;
const TInt KSenUuidMaxInt64Length = 20;
const TInt KSenUuidNumOfSlashes = 4;
const TInt KSenUuidHexWidth = 2;
const TInt KSenUuidPrefixLength = 5; // KSenUuidPrefix().Length(); 
const TInt KSenUuidHashMultiplier = 2;
const TInt KSenUuidHashReserved = 2;
const TInt KSenUuidMaxHashLength = KSenUuidMaxHostIdLength + KSenUuidMaxInt64Length * KSenUuidHashMultiplier + KSenUuidHashReserved;
const TInt KSenUuidMaxLength = KSenUuidPrefixLength + KSenUuidMaxHashLength * KSenUuidHexWidth + KSenUuidNumOfSlashes; 

// FORWARD DECLARATIONS
class CMD5;

// CLASS DECLARATION
class CSenGuidGen : public CBase
    {
    public: // Constructors and destructor
        
        IMPORT_C static CSenGuidGen* NewL();
        IMPORT_C static CSenGuidGen* NewLC();
        ~CSenGuidGen();
        
        // New functions
        
        /**
        *  Generate totally random (but not cryptographically strong)
        *  GUIDs. The GUID is based on the MD5 hash of a random number
        *  and the current time. Some kind of host ID should be also
        *  used but the author did not find an easy way of getting it.
        */
        IMPORT_C HBufC* GetRandomGuidLC();
        IMPORT_C HBufC8* GetRandomGuid8LC();
        IMPORT_C HBufC16* GetRandomGuid16LC();
        
    private:
    /**
    * C++ default constructor.
        */
        CSenGuidGen();
        /**
        * By default Symbian 2nd phase constructor is private.
        */  
        void ConstructL();
        
        // New functions
        
        TPtrC8 GetRandomHash();
        
    private: // Data
        TInt64 iSeed;
        HBufC8 *ipHostId;
        CMD5 *ipMd5HashGen;
    };

#endif // CCGUIDGEN_H

// End of File

