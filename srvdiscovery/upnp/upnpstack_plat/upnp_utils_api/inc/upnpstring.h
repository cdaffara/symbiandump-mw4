/** @file
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies  this distribution, and is available 
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Declares String functions
*
*/


#ifndef C_UPNPSTRING_H
#define C_UPNPSTRING_H

// INCLUDES
#include "in_sock.h"

// CONSTANTS
_LIT8(KEt, "&");
_LIT8(KEtAmp, "&amp;");
_LIT8(KEtlt, "&lt;");
_LIT8(KEtgt, "&gt;");
_LIT8(KApostrophe, "'");
_LIT8(KEtApos, "&apos;");
_LIT8(KEtQuot, "&quot;");
_LIT8(KCSpace, "%20");
const TInt KUrlCharNoLen = 3;
const TInt KOneCharLen = 1;


// ================= MEMBER FUNCTIONS =======================
namespace UpnpString
    {

    /**
    * Checks whether character Colon or not.
    * @since Series 60 2.o
    * @param aChar character to be investigated.
    * @return Boolean value.
    */
    IMPORT_C TBool IsColon(const TUint8& aChar);
    
    /**
    * Converts TDesC16 (UTF-16) to HBufC8 (UTF-8)
    * @param    aBuffer    data to be converted
    * @return HBufC8*    the converted data 
    */
    IMPORT_C HBufC8* FromUnicodeL(const TDesC& aBuffer);

    /**
    * Converts TDesC8 (UTF-8) to HBufC16 (UTF-16)
    * @param    aBuffer    data to be converted
    * @return HBufC16*    the converted data 
    */
    IMPORT_C HBufC16* ToUnicodeL(const TDesC8& aBuffer);
    
    /**
    * Returns current date in string representation in a 
    * form Sun, 06 Nov 1994 08:49:37 GMT
    * @return    HBufC8* string representation of current date
    */
    IMPORT_C HBufC8* CurrentDateLC();
    
    /**
    * Returns date in string representation in a 
    * form Sun, 06 Nov 1994 08:49:37 GMT
    * @return    HBufC8* string representation of a given date
    */
    HBufC8* GetDateLC(const TTime& aTime);
    /**
    * Get proper string for a week day    
    * @param    aDate    
    * @return    Abbreviated name of a weekday
    */
    const TDesC8& GetWeekday(const TTime& aDate);
    
    /**
    * Get proper string for a month
    * @param    aDate    
    * @return    Abbreviated name of a month
    */
    const TDesC8& GetMonth(const TDateTime& aDate);
    
    /**
    * Cuts data in aPtr into smaller pieces where aDelim is found
    * and puts the data into the aArray
    * @param    aPtr    stores the data to be cut to pieces
    * @param    aDelim    the delimeter from which the aPtr is cut
    * @param    aArray    holds the cut up parts of aPtr
    * @return    None.
    */
    IMPORT_C void CutToPiecesL(
        TPtrC8 aPtr,
        TChar aDelim,
        RPointerArray<TPtrC8>& aArray
        );
        
    /**
    * Removes spaces and (if wanted) quatation marks from the end
    * and beginning of the given string
    * @param    aPtr        The data to be trimmed
    * @param    aQuatations    TBool: Remove quotations?
    * @return    TPtrC8        Trimmed string
    */
    IMPORT_C TPtrC8 Trim(TPtrC8 aPtr, TBool aQuotations = EFalse);

    /**
    * Removes spaces and (if wanted) quatation marks from
    * the end and beginning of the given string
    * @since Series 60 2.0
    * @param aBuf buffer to be trimmed
    * @param aQuotations indicates whether '"' are trimmed.
    * @return Trimmed buffer
    */
    IMPORT_C HBufC8* TrimLC(const TDesC8& aBuf, TBool aQuotations);

    /**
    * Reads the aPlaceth element from the CSV string
    * @param    aCSV    data CSV string
    * @param    aPlace    the position of the wanted element
    * @return    HBufC8*    the wanted element
    */
    IMPORT_C HBufC8* ValueFromCsvL(const TDesC8& aCSV, TInt aPlace);

    /**
    * Adds a value to a CSV string
    * @param    aCSV    original CSV string
    * @param    aValue    value that is added to string
    * @return    HBufC8*    the result CSV string of the join operation
    */
    IMPORT_C HBufC8* AddValueToCsvL(const TDesC8& aCSV, const TDesC8& aValue);

    /**
    * Removes an element from the CSV string
    * @param    aCSV    original CSV string
    * @param    aValue    value to be removed
    * @return    HBufC8*    resulting CSV string 
    */
    IMPORT_C HBufC8* RemoveFromCsvLC(const TDesC8& aCSV, const TDesC8& aValue );

    /**
    * Adds elements from the given CSV string to the given array
    * @param    aCSV    CSV string that is going to be ripped apart
    * @param    aArray    the results of the conversion
    * @return    None.
    */
    IMPORT_C void CsvToRPointerArrayL(
        const TDesC8& aCSV,
        RPointerArray<TPtrC8>& aArray
        );

    /**
    * Converts array of elements into a CSV string
    * @param    aArray    element array
    * @return    HBufC8    result CSV string
    */
    IMPORT_C HBufC8* RPointerArrayToCsvLC( RPointerArray<TPtrC8>& aArray);

    /**
    * Adds aString2 to aString
    * @param aString    string that is going to be appended
    * @param aString2    string that is added 
    * @return    None.
    */
    IMPORT_C void AppendStringL(CBufFlat& aString, const TDesC8& aString2);

    /**
    * Adds aString2 to aString
    * @param aString    string that is going to be appended
    * @param aString2    string that is added 
    * @return    None.
    */
    IMPORT_C void AppendStringL(HBufC8*& aString, const TDesC8& aString2);

    /**
    * Converts given string to integer format
    * @param    aStr    string to be converted
    * @param    aInt    Resulting integer
    * @return    TInt    system wide error code.
    */
    IMPORT_C TInt StringToInt(const TDesC8& aStr, TInt* aInt);

    /**
    * Replaces a part of the given string with a new part
    * @param    aStr            Original string
    * @paran    aResult            resulting string
    * @param    aTrg            the target part of the string
    * @param    aReplacement    the replacement part
    * @return    None.
    */    
    IMPORT_C void StringReplaceL(
        const TDesC8& aOrginal,
        HBufC8*& aResult,
        const TDesC8& aTrg,
        const TDesC8& aReplacement
        );
    
    /**
    * Replaces a part of the given string with a new part
    * @param    aStr            Original string
    * @param    aTrg            the target part of the string
    * @param    aReplacement    the replacement part
    * @return    HBufC*            resulting string
    */
    IMPORT_C HBufC8* StringReplaceL(
        const TDesC8& aStr,
        const TDesC8& aTrg,
        const TDesC8& aReplacement
        );    

    /**
    Replaces a part of the given string with a new part
    @param    aString            Original string, replacements done here
    @param    aTrg            the target part of the string
    @param    aReplacement    the replacement part
    @return    none
    **/
    IMPORT_C void StringReplaceInPlaceL(TDes8& aString, const TDesC8& aTrg, const TDesC8& aReplacement);

    /**
    * Encodes the XML string
    * @param    aBuf    the string to be converted
    * @return    HBufC8*    the converted string
    */
    IMPORT_C HBufC8* EncodeXmlStringL( HBufC8*& aBuf );
    
    /**
    * Encodes the XML string
    * @param    aBuf    the string to be converted
    * @result   HBufC8* the converted string
    */
    IMPORT_C HBufC8* EncodeXmlStringL( const TDesC8& aBuf );
    
    /**
    * Decodes the XML string 
    * @param    aBuf        string to be decoded
    * @return    HBufC8*        decoded string
    */
    IMPORT_C HBufC8* DecodeXmlStringL( HBufC8*& aBuf );

    /**
    decodes the XML string 
    @param    aBuf        string to be decoded
    @return    HBufC8*        decoded string
    **/
    IMPORT_C HBufC8* DecodeXmlStringL( const TDesC8& aBuf );

    /**
    * Returns a HBufC8 representation of the InetAddress
    * @param    aAddress    InetAddress 
    * @return HBufC8*        string representation of InetAddress
    */
    IMPORT_C HBufC8* InetToStringL(const TInetAddr& aAddress);
    
    /**
    * Replaces URL codes "%xx" by the corresponding character
    * @param    aString    InetAddress string to be processed
    */
    IMPORT_C void ReplaceHttpCharactersL( TDes& aString );
    IMPORT_C void ReplaceHttpCharacters( TDes8& aString );
    
    }


#endif // C_UPNPSTRING_H