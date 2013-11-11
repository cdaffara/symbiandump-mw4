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



// INCLUDE FILES
#include    "ProgressInfoCreator.h"
#include    "DMgrUiLibPanic.h"
#include    "UiLibLogger.h"
#include    <DownloadMgrUiLib.rsg>
#include    <StringLoader.h>
#include    <AknUtils.h>

// LOCAL CONSTANTS AND MACROS
_LIT( KCharSpace, " " );
_LIT( KBracketLeft, "(" );
_LIT( KBracketRight, ")" );
LOCAL_C const TInt32 KKiloExchangeRate = 1000; // not 1024, because of usability...
LOCAL_C const TInt32 KBytesInOneMB = KKiloExchangeRate*KKiloExchangeRate;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CProgressInfoCreator::CProgressInfoCreator
// -----------------------------------------------------------------------------
//
CProgressInfoCreator::CProgressInfoCreator()
:   iCoeEnv( *CCoeEnv::Static() )
    {
    }

// -----------------------------------------------------------------------------
// CProgressInfoCreator::ConstructL
// -----------------------------------------------------------------------------
//
void CProgressInfoCreator::ConstructL()
    {
    // Load often used resource strings for progress indication.
    iUnitKbFormatString = iCoeEnv.AllocReadResourceL( R_DMUL_UNIT_KB );
    iUnitMbFormatString = iCoeEnv.AllocReadResourceL( R_DMUL_UNIT_MB );
    HBufC* percentFormatBaseString = 
           iCoeEnv.AllocReadResourceLC( R_DMUL_UNIT_PERCENT );
    // Insert brackets
    iUnitPcFormatString = HBufC::NewMaxL( percentFormatBaseString->Length() + 
                                          KBracketLeft().Length() + 
                                          KBracketRight().Length() );
    iUnitPcFormatString->Des().Copy( KBracketLeft );
    iUnitPcFormatString->Des().Append( *percentFormatBaseString );
    iUnitPcFormatString->Des().Append( KBracketRight );
    CleanupStack::PopAndDestroy( percentFormatBaseString ); // percentFormatBaseString
    }

// -----------------------------------------------------------------------------
// CProgressInfoCreator::NewL
// -----------------------------------------------------------------------------
//
CProgressInfoCreator* CProgressInfoCreator::NewL()
    {
    CProgressInfoCreator* self = new ( ELeave ) CProgressInfoCreator();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// Destructor
CProgressInfoCreator::~CProgressInfoCreator()
    {
    delete iUnitPcFormatString;
    delete iUnitMbFormatString;
    delete iUnitKbFormatString;
    }

// -----------------------------------------------------------------------------
// CProgressInfoCreator::ProgressInfo
// -----------------------------------------------------------------------------
//
void CProgressInfoCreator::ProgressInfo( TInt32 aSize, TDes& aResult )
    {
    __ASSERT_DEBUG( aResult.MaxSize()>=7, Panic( EUiLibPanSmallDivBuffer ) );

    // Indicate only the size.
    aResult.Zero();

    // aSize is in bytes.
    // Format string is: "%N kB" or "%U MB". %N and %U is 1-4 characters long.
    // So aResult must be at least 7 (=4+3) characters long.

    if ( aSize < KBytesInOneMB )
        {
        // The unit must be "kB". Apply integer division. No decimals are shown.
        // With the exchange rate 1000 between kB and MB, the following 
        // division's result must be < 1000, so less than 3 characters:
        TInt sizeInKiloBytes = aSize / KKiloExchangeRate;
        if ( 0 < aSize && sizeInKiloBytes == 0 )
        	{
        	// Don't let progress show 0kB when there is already something 
        	// downloaded (0 < aSize):
        	sizeInKiloBytes = 1;
        	}
        StringLoader::Format
            ( aResult, *iUnitKbFormatString, KErrNotFound, sizeInKiloBytes );
        }
    else
        {
        // Size is 1 MB or more.
        // It is always shown in 3 numbers.
        Division( aSize, KBytesInOneMB, iFormattedMegaBytes );
        AknTextUtils::DisplayTextLanguageSpecificNumberConversion( iFormattedMegaBytes );
        StringLoader::Format
            ( aResult, *iUnitMbFormatString, KErrNotFound, iFormattedMegaBytes );
        }
    }

// -----------------------------------------------------------------------------
// CProgressInfoCreator::ProgressInfo
// -----------------------------------------------------------------------------
//
void CProgressInfoCreator::ProgressInfo
    ( TInt32 aPartialSize, TInt32 aFullSize, TDes& aResult )
    {
    __ASSERT_DEBUG( aFullSize!=0, Panic( EUiLibPanDivisionWithZero ) );

    // Indicate pecentage & downloaded size.
    aResult.Zero();

    // Percentage value is shown in 1-2 numbers, so must be less than 100.
    // Apply integer division.
    // Note that we have to use 64-bit integers to avoid overflow when 
    // aPartialSize is multiplied with 100, because 100*KMaxTInt (the max 
    // value of aPartialSize) would not fit into TUint32, thus it would cause overflow.
    iTUint64Helper1 = aPartialSize;
    iTUint64Helper2 = aFullSize;
    TInt percent = (TInt) ((TUint64(100)*iTUint64Helper1) / iTUint64Helper2);
    if ( percent >= 100 )
        {
        percent = 99;
        }
    // OK. Insert the percentage.
    StringLoader::Format( aResult, *iUnitPcFormatString, KErrNotFound, percent );

    // Append space separator.
    aResult.Append( KCharSpace );

    // Construct and append partal size with unit.
    ProgressInfo( aPartialSize, iPartialSizeString );
    aResult.Append( iPartialSizeString );
    }

// -----------------------------------------------------------------------------
// CProgressInfoCreator::Division
// -----------------------------------------------------------------------------
//
void CProgressInfoCreator::Division
    ( TInt aNumerator,TInt aDenominator, TDes& aResult )
	{
    __ASSERT_DEBUG( aDenominator!=0, Panic( EUiLibPanDivisionWithZero ) );
    __ASSERT_DEBUG( aResult.MaxSize()>=4, Panic( EUiLibPanSmallDivBuffer ) );

    // Reset the result buffer.
    aResult.Zero();
    iDivResult.Zero();
    TLocale locale;

    TInt quotient = aNumerator / aDenominator;
	TInt remainder = aNumerator % aDenominator;

    // Only 3 numbers needed, so if 999 < quotient, then show only 999.
    if ( 999 < quotient )
        {
        quotient = 999;
        }

    // Copy the integer part of the division.
	iDivResult.AppendNum( quotient );
    //Copy the decimal separator.
    iDivResult.Append( locale.DecimalSeparator() );
 
    // Copy the fraction part, we need 2 decimal fractions.
    // Example: 2.00 or 3.50...
    for ( TInt i=0;i<2;i++ )
        {
        TInt newRemainder = ( remainder * 10 ) % aDenominator;
        remainder = ( remainder * 10 ) / aDenominator;
        iDivResult.AppendNum( remainder );
        remainder = newRemainder;
        }

    // Only 3 numbers needed in the string. So, if the integer part is bigger
    // than 99, i.e. the decimel separator is at the 4. place, 
    // we chop the remainder part...
    // Example "100.34" will be "100", "158.65" will be "158", 
    // because of the specification.

    //If the integer value is bigger than 99, then chop...
    if ( ( TChar )iDivResult[3] == locale.DecimalSeparator() )
        {
        aResult.Copy( iDivResult.Left( 3 ) );
        }
    // Or simply copy to the result...
    else
        {
        aResult.Copy( iDivResult.Left( 4 ) );
        }
	}

/* End of file. */

