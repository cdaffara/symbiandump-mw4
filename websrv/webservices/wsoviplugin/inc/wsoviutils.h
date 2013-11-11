/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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









#ifndef WSOVIUTILS_H
#define WSOVIUTILS_H

// INCLUDES
#include <e32base.h>
#include <e32std.h>

// CONSTANTS
const TInt KHexWidth = 2;
const TInt KCharWidth = 1;
const TInt KSpaceChar = 32;

// FORWARD DECLARATIONS
class CMD5;
//class RMapDescriptors;

// CLASS DECLARATION
class CWSOviUtils : public CBase
    {
    public:
                
        // New functions
		static HBufC8* NormalizeStringLC(const TDesC8& aMethod, const TDesC8& aUrl, const TDesC8& aHeader );
		static HBufC8* DigestAuthStringL(const TDesC8& aNonce, const TDesC8& aTimestamp, const TDesC8& aUser, const TDesC8& aPassword);
        static HBufC8* EncodeCharsLC(const TDesC8& aString);
        static HBufC8* FormEncodeCharsLC(const TDesC8& aString);

    private:

		~CWSOviUtils();       
        // New functions
        static TInt CountCharsToEncode(const TDesC8& aString);
        
    private: // Data
    

    };

#endif // WSOVIUTILS_H

// End of File

