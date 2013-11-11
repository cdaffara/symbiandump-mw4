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
* Description:  A misc. char-encoding related utilities.
*
*/


#include <charconv.h>

#include "XmlEncoding.h"


// -----------------------------------------------------------------------------
// CXmlEncoding::NewL
//
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CXmlEncoding* CXmlEncoding::NewL()
    {
    CXmlEncoding* self = new (ELeave) CXmlEncoding();
    
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

        
// -----------------------------------------------------------------------------
// CXmlEncoding::CXmlEncoding
//
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CXmlEncoding::CXmlEncoding():
        iCharEncodings(5)
    {
    }
        

// -----------------------------------------------------------------------------
// CXmlEncoding::ConstructL
//
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CXmlEncoding::ConstructL()
    {
    User::LeaveIfError(iRfs.Connect());
    iConverter = CCnvCharacterSetConverter::NewL();
    }
        

// -----------------------------------------------------------------------------
// CXmlEncoding::~CXmlEncoding
//
// Deconstructor.
// -----------------------------------------------------------------------------
//
CXmlEncoding::~CXmlEncoding()
    {
    iRfs.Close();
    delete iConverter;

    // Delete the cached encoding-map.
    for (TInt i = 0; i < iCharEncodings.Count(); i++)
        {
        delete iCharEncodings[i].charEncoding;
        }

    iCharEncodings.Close();
    }


// -----------------------------------------------------------------------------
// CXmlEncoding::ResolveCharEncodingL
// 
// Resolves the given char-encoding into its uid.
// -----------------------------------------------------------------------------
//
TBool CXmlEncoding::ResolveCharEncodingL(const TDesC8& aCharEncoding, TUint& aUid) const
    {
    TBool  found = EFalse;

    // First look it up in the cached encodings.
    for (TInt i = 0; i < iCharEncodings.Count(); i++)
        {
        if (iCharEncodings[i].charEncoding->CompareF(aCharEncoding) == 0)
            {
            aUid = iCharEncodings[i].uid;
            found = ETrue;

            break;
            }
        }

    // Otherwise use the CCnvCharacterSetConverter
    if (!found)
        {
        TUint  tid;

        // Look it up.
        tid = iConverter->ConvertStandardNameOfCharacterSetToIdentifierL(aCharEncoding, 
                const_cast<CXmlEncoding*>(this)->iRfs);

        // If found add it to the cached encodings.
        if (tid != 0)
            {
            SupportedEncodings  encoding;
            TInt                err;

            encoding.charEncoding = aCharEncoding.AllocL();
            encoding.uid = tid;

            err = const_cast<CXmlEncoding*>(this)->iCharEncodings.Append(encoding);
            if (err != KErrNone)
                {
                delete encoding.charEncoding;
                User::Leave(err);
                }
            
            aUid = tid;
            found = ETrue;
            }
        }

    return found;
    }


// -----------------------------------------------------------------------------
// XmlEncoding::DetermineCharEncoding
//
// Determine the char-encoding.
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CXmlEncoding::DetermineCharEncodingL(const TDesC8& aBuffer, 
        const TDesC& aCharSet, TUint& aEncoding) const
    {
    TBool  foundEncoding = EFalse;

    // Try to determine the encoding via the BOM (byte order mask).
    foundEncoding = DetermineCharEncodingFromBom(aBuffer, aEncoding);

    // Try to determine the encoding via the xml-prolog.
    if (!foundEncoding)
        {
        foundEncoding = DetermineCharEncodingFromXmlProlog(aBuffer, aEncoding);
        }

    // Try to determine the encoding via the char-set provided by 
    // the orignal source.
    if (!foundEncoding)
        {
        HBufC8*  str = NULL;

        // Convert it to 8bit first.
        str = HBufC8::NewL(aCharSet.Length());
        CleanupStack::PushL(str);
        str->Des().Append(aCharSet);

        foundEncoding = ResolveCharEncodingL(*str, aEncoding);
        CleanupStack::PopAndDestroy(str);
        }

    return foundEncoding;
    }


// -----------------------------------------------------------------------------
// XmlEncoding::DetermineCharEncodingFromBom
//
// Determine the char-encoding from the BOM.
// -----------------------------------------------------------------------------
//
TBool CXmlEncoding::DetermineCharEncodingFromBom(const TDesC8& aBuffer, 
        TUint& aEncoding) const
    {
    _LIT8(KUcs2Big, "UTF-16BE");
    _LIT8(KUcs2Little, "UTF-16LE");
    _LIT8(KUtf8, "UTF-8");

    TBool  foundEncoding = EFalse;
    
    if (aBuffer.Length() < 3)
        {    
        return EFalse;
        }

    // Extract the first three bytes.
    TUint8 c1 = aBuffer[0];
    TUint8 c2 = aBuffer[1];
    TUint8 c3 = aBuffer[2];
      
    // Check for the BOM.
    if ((c1 == 0xFE) && (c2 == 0xFF))
        {
        TRAP_IGNORE(foundEncoding = ResolveCharEncodingL(KUcs2Big, aEncoding));
        }
    else if ((c1 == 0xFF) && (c2 == 0xFE))
        {
        TRAP_IGNORE(foundEncoding = ResolveCharEncodingL(KUcs2Little, aEncoding));
        } 
    else if ((c1 == 0xEF) && (c2 == 0xBB) && (c3 == 0xBF))
        {
        TRAP_IGNORE(foundEncoding = ResolveCharEncodingL(KUtf8, aEncoding));
        }

    return foundEncoding;
    }


// -----------------------------------------------------------------------------
// XmlEncoding::DetermineCharEncodingFromXmlProlog
//
// Determine the char-encoding from the char-encoding in the xml-prolog.
// -----------------------------------------------------------------------------
//
TBool CXmlEncoding::DetermineCharEncodingFromXmlProlog(const TDesC8& aBuffer, 
        TUint& aEncoding) const
    {
    TBool  foundEncoding = EFalse;
    TInt   begin;
    TInt   end;
    TInt   valueBegin;
    TInt   valueEnd;

    // Try to determine the encoding via the xml-prolog.
    if (FindEncoding(aBuffer, begin, end, valueBegin, valueEnd))
        {
        TPtrC8  str(aBuffer.Ptr() + valueBegin, valueEnd - valueBegin + 1);

        TRAP_IGNORE(foundEncoding = ResolveCharEncodingL(str, aEncoding));
        }

    return foundEncoding;
    }


// -----------------------------------------------------------------------------
// XmlEncoding::StripCharEncoding
//
// Erase the char-encoding attribute (if any) from the xml-prolog.
// -----------------------------------------------------------------------------
//
void CXmlEncoding::StripCharEncoding(HBufC8& aUtf8Buffer) const
    {
    TInt  begin;
    TInt  end;
    TInt  valueBegin;
    TInt  valueEnd;

    if (FindEncoding(aUtf8Buffer, begin, end, valueBegin, valueEnd))
        {
        aUtf8Buffer.Des().Delete(begin, end - begin + 1);
        }
    }


// -----------------------------------------------------------------------------
// XmlEncoding::StripCharEncoding
//
// Erase the char-encoding attribute (if any) from the xml-prolog.
// -----------------------------------------------------------------------------
//
void CXmlEncoding::StripCharEncoding(HBufC16& aUcs2Buffer) const
    {
    TInt  begin;
    TInt  end;
    TInt  valueBegin;
    TInt  valueEnd;

    if (FindEncoding(aUcs2Buffer, begin, end, valueBegin, valueEnd))
        {
        aUcs2Buffer.Des().Delete(begin, end - begin + 1);
        }
    }


// -----------------------------------------------------------------------------
// XmlEncoding::ConvertToUtf8L
//
// Converts the given native buffer into utf8.
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC8* CXmlEncoding::ConvertToUtf8L(TUint aEncoding, const TDesC8& aBuffer) const
	{
	HBufC16*  usc2Buffer = NULL;
	HBufC8*   utf8Buffer = NULL;

    // First convert it to ucs2.
    usc2Buffer = ConvertToUcs2L(aEncoding, aBuffer);
    CleanupStack::PushL(usc2Buffer);

    // Then convert it to utf8.
    utf8Buffer = ConvertToUtf8L(*usc2Buffer);

    CleanupStack::PopAndDestroy(usc2Buffer);
    usc2Buffer = NULL;

    // Erase the char-encoding attribute (if any) from the xml-prolog.  If forces
    // Libxml2 to use the default encoding, utf8.
    StripCharEncoding(*utf8Buffer);

    return utf8Buffer;
	}


// -----------------------------------------------------------------------------
// XmlEncoding::ConvertToUcs2L
//
// Converts the given native buffer into ucs2.
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC16* CXmlEncoding::ConvertToUcs2L(TUint aEncoding, const TDesC8& aBuffer) const
	{
	TBuf<100>  temp16Buffer;
	HBufC*     unicode = NULL;
	TPtrC8	   source8Ptr(aBuffer);

    // Init the converter and ensure the encoding is supported.
	if (iConverter->PrepareToConvertToOrFromL(aEncoding, const_cast<CXmlEncoding*>(this)->iRfs) != 
            CCnvCharacterSetConverter::EAvailable)
		{
		User::Leave(KErrNotSupported);
		}

	for(;;) // conversion loop
        {
        TInt  returnValue;
		TInt  state = CCnvCharacterSetConverter::KStateDefault;

		returnValue = iConverter->ConvertToUnicode(temp16Buffer, source8Ptr, state);
        if (returnValue == CCnvCharacterSetConverter::EErrorIllFormedInput)
			{
            User::Leave(KErrCorrupt);
			}
        else
			{
			if (returnValue < 0) // future-proof against "TError" expanding
				{
				User::Leave(KErrGeneral);
				}
			}

		if (!unicode)
			{
			unicode = temp16Buffer.AllocLC();
			}
		else
			{
			HBufC* tmp = unicode->ReAllocL(unicode->Length() + temp16Buffer.Length());
			CleanupStack::Pop(unicode);
			unicode = tmp;
			CleanupStack::PushL(unicode);
			unicode->Des().Append(temp16Buffer);
			}

        if (returnValue == 0)	// All is converted without Errors
			{
            break;
			}

		// There is "returnValue" bytes not converted yet
        source8Ptr.Set(source8Ptr.Right(returnValue));
        }

    // Erase the char-encoding attribute (if any) from the xml-prolog -- as the
    // encoding is no longer valid.
    StripCharEncoding(*unicode);

	CleanupStack::Pop(unicode);
	return unicode;
	}


// -----------------------------------------------------------------------------
// XmlEncoding::ConvertToUtf8L
//
// Converts the given ucs2 buffer into utf8.
// -----------------------------------------------------------------------------
//
HBufC8* CXmlEncoding::ConvertToUtf8L(HBufC16& aUsc2Buffer) const
	{
	TBuf8<100>	temp8Buffer;
	HBufC8*		target = NULL;
	TPtrC		source16Ptr(aUsc2Buffer);

    // Init the converter and ensure the encoding is supported.
	if (iConverter->PrepareToConvertToOrFromL(KCharacterSetIdentifierUtf8, 
	        const_cast<CXmlEncoding*>(this)->iRfs) != CCnvCharacterSetConverter::EAvailable)
		{
		User::Leave(KErrNotSupported);
		}

	for(;;) // conversion loop
        {
		TInt  returnValue;
        TInt  state = CCnvCharacterSetConverter::KStateDefault;

		returnValue = iConverter->ConvertFromUnicode(temp8Buffer, source16Ptr, state);
        if (returnValue == CCnvCharacterSetConverter::EErrorIllFormedInput)
			{
            User::Leave(KErrCorrupt);
			}
        else
			{
			if (returnValue < 0) // future-proof against "TError" expanding
				{
				User::Leave(KErrGeneral);
				}
			}

		if (!target)
			{
			target = temp8Buffer.AllocLC();
			}
		else
			{
			HBufC8* tmp = target->ReAllocL(target->Length() + temp8Buffer.Length());
			CleanupStack::Pop(target);
			target = tmp;
			CleanupStack::PushL(target);
			target->Des().Append(temp8Buffer);
			}

        if (returnValue == 0)	// All is converted without Errors
			{
            break;
			}

		// There is "returnValue" bytes not converted yet
        source16Ptr.Set(source16Ptr.Right(returnValue));
        }
	CleanupStack::Pop(target);

	return target;
	}


// -----------------------------------------------------------------------------
// XmlEncoding::FindEncoding
//
// Finds the location of the encoding attribute in the xml-prolog.
// Refer to: http://www.w3.org/TR/2000/REC-xml-20001006#sec-prolog-dtd.
// -----------------------------------------------------------------------------
//
TBool CXmlEncoding::FindEncoding(const TDesC8& aBuffer, TInt& aBegin, TInt& aEnd, 
        TInt& aValueBegin, TInt& aValueEnd) const
    {
    _LIT8(KPiStart, "<?xml");
    _LIT8(KPiEnd, "?>");
    _LIT8(KEncoding, "encoding");

    TInt    piStartLoc = KErrNotFound;
    TInt    piEndLoc = KErrNotFound;

    aBegin = KErrNotFound;
    aEnd = KErrNotFound;
    aValueBegin = KErrNotFound;
    aValueEnd = KErrNotFound;

    // Determine if the xml-prolog contains a char-encoding attribute.
    piStartLoc = aBuffer.FindF(KPiStart);
    piEndLoc = aBuffer.FindF(KPiEnd);
    aBegin = aBuffer.FindF(KEncoding);
    
    if ((piStartLoc != KErrNotFound) && (piEndLoc != KErrNotFound) && 
            (aBegin != KErrNotFound) && (piStartLoc < piEndLoc) && (aBegin < piEndLoc))
        { 
        TInt   i;
        TBool  foundEquals = EFalse;
        TInt   quoteCount = 0;

        // If so, find the end of the attribute.
        i = aBegin + KEncoding().Length();
        while (i < piEndLoc)
            {
            TInt8  c;

            c = aBuffer[i];

            // The '=' char must be the first non-whitespace after "encoding".
            if (!foundEquals)
                {
                if (c == '=')
                    {
                    foundEquals = ETrue;
                    }
                else if ((c != 0x20) && (c != 0x09) && (c != 0x0D) && (c != 0x0A))
                    {
                    // Give up.
                    return EFalse;
                    }
                }

            // Otherwise look for the two quotes
            else if ((c == '\"') || (c == '\''))
                {
                quoteCount++;

                if (quoteCount == 1)
                    {
                    aValueBegin = i + 1;
                    }
                else if (quoteCount == 2)
                    {
                    aEnd = i;
                    aValueEnd = i - 1;
                    break;
                    }
                }

            i++;
            }
        }

    return ((aValueBegin != KErrNotFound) && (aValueEnd != KErrNotFound));
    }


// -----------------------------------------------------------------------------
// XmlEncoding::FindEncoding
//
// Finds the location of the encoding attribute in the xml-prolog.
// Refer to: http://www.w3.org/TR/2000/REC-xml-20001006#sec-prolog-dtd.
// -----------------------------------------------------------------------------
//
TBool CXmlEncoding::FindEncoding(const TDesC& aBuffer, TInt& aBegin, 
        TInt& aEnd, TInt& aValueBegin, TInt& aValueEnd) const
    {
    _LIT16(KPiStart, "<?xml");
    _LIT16(KPiEnd, "?>");
    _LIT16(KEncoding, "encoding");

    TInt    piStartLoc = KErrNotFound;
    TInt    piEndLoc = KErrNotFound;

    aBegin = KErrNotFound;
    aEnd = KErrNotFound;
    aValueBegin = KErrNotFound;
    aValueEnd = KErrNotFound;

    // Determine if the xml-prolog contains a char-encoding attribute.
    piStartLoc = aBuffer.FindF(KPiStart);
    piEndLoc = aBuffer.FindF(KPiEnd);
    aBegin = aBuffer.FindF(KEncoding);
    
    if ((piStartLoc != KErrNotFound) && (piEndLoc != KErrNotFound) && 
            (aBegin != KErrNotFound) && (piStartLoc < piEndLoc) && (aBegin < piEndLoc))
        { 
        TInt   i;
        TBool  foundEquals = EFalse;
        TInt   quoteCount = 0;

        // If so, find the end of the attribute.
        i = aBegin + KEncoding().Length();
        while (i < piEndLoc)
            {
            TInt16  c;

            c = aBuffer[i];

            // The '=' char must be the first non-whitespace after "encoding".
            if (!foundEquals)
                {
                if (c == '=')
                    {
                    foundEquals = ETrue;
                    }
                else if ((c != 0x20) && (c != 0x09) && (c != 0x0D) && (c != 0x0A))
                    {
                    // Give up.
                    return EFalse;
                    }
                }

            // Otherwise look for the two quotes
            else if ((c == '\"') || (c == '\''))
                {
                quoteCount++;

                if (quoteCount == 1)
                    {
                    aValueBegin = i + 1;
                    }
                else if (quoteCount == 2)
                    {
                    aEnd = i;
                    aValueEnd = i - 1;
                    break;
                    }
                }

            i++;
            }
        }

    return ((aValueBegin != KErrNotFound) && (aValueEnd != KErrNotFound));
    }
