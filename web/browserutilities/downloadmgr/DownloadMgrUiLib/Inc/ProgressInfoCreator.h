/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Utility. Creates progress info string from given data.
*
*/



#ifndef PROGRESSINFOCREATOR_H
#define PROGRESSINFOCREATOR_H

//  INCLUDES
#include <e32base.h>
#include <e32def.h>
#include <e32std.h>

// FORWARD DECLARATIONS
class CCoeEnv;

// CLASS DECLARATION

/**
*  Utility.
*  It requires that DownloadMgrUiLib.rsc be loaded!
*
*  @lib Download Manager UI Lib
*  @since Series 60 2.8
*/
NONSHARABLE_CLASS( CProgressInfoCreator ) : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CProgressInfoCreator* NewL();
        
        /**
        * Destructor.
        */
        virtual ~CProgressInfoCreator();

    public: // New functions

        /**
        * Construct progress info string containing size info according 
        * to the specification.
        * @param aSize The size in bytes.
        * @param aResult The result is written into this buffer. Must be great enough!
        * @return None.
        */      
        void ProgressInfo( TInt32 aSize, TDes& aResult );

        /**
        * Construct progress info string containing percentage and size info.
        * @param aPartialSize The partial size in bytes.
        * @param aFullSize The full size in bytes.
        * @param aResult The result is written into this buffer. Must be great enough!
        * @return None.
        */      
        void ProgressInfo( TInt32 aPartialSize, TInt32 aFullSize, TDes& aResult );

        /**
        * Divides the given parameters, and returns the result
        * as a formatted string.
        * @param aNumerator The numerator of the division.
        * @param aDenominator The denominator of the division.
        * @param aResult The result is written into this buffer. Must be great enough!
        * @return None.
        */      
		void Division( TInt aNumerator, TInt aDenominator, TDes& aResult );

    protected: // Constructors

        /**
        * C++ default constructor.
        */
        CProgressInfoCreator();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:  // Data

        CCoeEnv& iCoeEnv; ///< Reference to CONE environment.

        HBufC* iUnitKbFormatString; ///< Owned.
        HBufC* iUnitMbFormatString; ///< Owned.
        HBufC* iUnitPcFormatString; ///< Owned.

        /**
        * Formatted MB. 4 characters are enough, but allocate a bigger buffer. 
        * It is a result buffer of DivisionL() method.
        */
        TBuf<16> iFormattedMegaBytes;

        /**
        * 7 characters should be enough, but allocate a bigger buffer.
        * It is a result buffer of ProgressInfoL(2) method.
        */
        TBuf<16> iPartialSizeString;

        /** 
        * Temporary buffers for the DivisionL() method.
        * Max 3 characters for the quotient + 1 for the separator + 
        * 2 for the remainder = 6 characters.
        */
        TBuf<6> iDivResult;
        
        TUint64 iTUint64Helper1;
        TUint64 iTUint64Helper2;

    };

#endif /* PROGRESSINFOCREATOR_H */
