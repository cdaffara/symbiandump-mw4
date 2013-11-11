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








#include <utf.h>

#include "SenXmlConstants.h"
#include "SenXmlUtils.h"
#include "SenElement.h"
#include "SenBaseAttribute.h"

#ifdef SYMBIAN_SECURE_ECOM 
    // for 2.8, 3.0 or newer:
    #include <xml/attribute.h> // needed for RAttributeArray
#else // for 2.6 or olde
    #include "Attribute.h"
#endif

using namespace Xml;

EXPORT_C HBufC8* SenXmlUtils::ToUtf8LC(const TDesC16& aUnicodeString)
    {
    // 6 times multiplier is the worst case...
    HBufC8* pBuf8 = HBufC8::NewLC(6 * aUnicodeString.Length());
    TPtr8 des8 = pBuf8->Des();
    TInt ret = CnvUtfConverter::ConvertFromUnicodeToUtf8(des8, aUnicodeString);
    User::LeaveIfError(ret);

    // Shrink allocated memory if possible
    HBufC8* pReallocBuf8 = pBuf8->ReAlloc(pBuf8->Length());
    if (pReallocBuf8 != NULL)
        {
        // It could reallocate
        CleanupStack::Pop(); // pop pBuf8;
        CleanupStack::PushL(pReallocBuf8);
        pBuf8 = pReallocBuf8;
        }
    return pBuf8;
    }

EXPORT_C HBufC16* SenXmlUtils::ToUnicodeLC(const TDesC8& aUtf8String)
    {
    HBufC16* pBuf = HBufC16::NewLC(2 * aUtf8String.Length());
    TPtr16 des = pBuf->Des();
    TInt ret = CnvUtfConverter::ConvertToUnicodeFromUtf8(des, aUtf8String);
    User::LeaveIfError(ret);
    return pBuf;
    }


EXPORT_C TBool SenXmlUtils::StartsWith(const TDesC8& aDes,
                                       const TDesC8& aPrefix)
    {
    if (aDes.Length() < aPrefix.Length())
        {
        return EFalse;
        }
    else
        {
        return (aDes.Left(aPrefix.Length()) == aPrefix);
        }
    }

EXPORT_C TBool SenXmlUtils::EndsWith(const TDesC8& aDes,
                                     const TDesC8& aPostfix)
    {
    if (aDes.Length() < aPostfix.Length())
        {
        return EFalse;
        }
    else
        {
        return (aDes.Right(aPostfix.Length()) == aPostfix);
        }
    }

EXPORT_C TPtrC8 SenXmlUtils::NsPrefix(const TDesC8& aQName)
    {
    TInt colonPos = aQName.Find(KSenColon);
    if (colonPos < 0)
        {
        return KNullDesC8();
        }
    else
        {
        return aQName.Left(colonPos);
        }
    }

EXPORT_C TPtrC8 SenXmlUtils::LocalName(const TDesC8& aQName)
    {
    TInt colonPos = aQName.Find(KSenColon);
    if (colonPos < 0)
        {
        return aQName;
        }
    else
        {
        return aQName.Mid(colonPos + 1);
        }
    }

EXPORT_C TPtrC8 SenXmlUtils::AttrValue( const RAttributeArray& aAttributes,
                                        const TDesC8& aAttrName)
    {
    TInt count(aAttributes.Count());
    for (TInt i=0; i<count;i++)
        {
        if (aAttributes[i].Attribute().LocalName().DesC() == aAttrName)
            {
            return aAttributes[i].Value().DesC();
            }
        }
    return KNullDesC8();
    }

//EXPORT_C TPtrC8 SenXmlUtils::AttrType(const RAttributeArray& aAttributes,
//                                        const TDesC8& aAttrName)
//  {
//  TInt count(aAttributes.Count());
//  for (TInt i=0; i<count;i++)
//      {
//      if (aAttributes[i].Attribute().LocalName().DesC() == aAttrName)
//          {
//          return aAttributes[i].Type().DesC();
//          }
//      }
//  return KNullDesC8();
//  }

EXPORT_C void SenXmlUtils::BuildQNameL( const TDesC8& aPrefix,
                                        const TDesC8& aLocalName,
                                        HBufC8*& aQName)
    {
    aQName = HBufC8::NewLC( aPrefix.Length()+KSenColon().Length()
                            +aLocalName.Length() );
    if (aPrefix != KNullDesC8)
        {
        aQName->Des().Append(aPrefix);
        aQName->Des().Append(KSenColon);
        }
    aQName->Des().Append(aLocalName);
    CleanupStack::Pop();

    }

EXPORT_C TBool SenXmlUtils::EncodeHttpCharactersL(const TDesC8& aOriginal,
                                                     HBufC8*& aEncoded)
    {
    TBool retVal = EFalse;
    delete aEncoded;
    aEncoded = NULL;

    if (aOriginal == KNullDesC8)
        {
        return retVal;
        }
    TPtrC8 tokens[] =
        {
        KSenEscapedAmp(),
        KSenEscapedApos(),
        KSenEscapedDblQuot(),
        KSenEscapedGt(),
        KSenEscapedLt()
        };
    TText16 tokenChars[] =
        {
        '&',
        '\'',
        '\"',
        '>',
        '<'
        };
    
    // Replace escaped characters, if any
    for (TInt i = 0; i < aOriginal.Length(); i++)
        {
        TBool foundChar = EFalse;
        for (TUint j = 0; j < (sizeof(tokenChars) / sizeof(TText16)); j++)
            {
            if (aOriginal[i] == tokenChars[j])
                {
                if (!aEncoded)
                    {
                    aEncoded =
                        HBufC8::NewL(aOriginal.Length() * KSenMaxXmlEscapedLength);
                    aEncoded->Des().Append(aOriginal.Left(i));
                    }
                foundChar = ETrue;
                aEncoded->Des().Append(tokens[j]);
                retVal = ETrue; // indicate, that encoding was done
                break;
                }
            }
        if (!foundChar)
            {
            if (aEncoded)
                {
                
                aEncoded->Des().Append(aOriginal[i]);
                }
            }
        }

    return retVal;
    }

EXPORT_C HBufC8* SenXmlUtils::EncodeHttpCharactersLC( const TDesC8& aOriginal )
    {
    HBufC8* pDecoded = NULL;
    if (!EncodeHttpCharactersL(aOriginal, pDecoded))
        {
        // if anything was allocated (never in current implementation)
        delete pDecoded; 
        pDecoded = NULL;

        // make a copy of original, so that caller always knows that
        // there is something from which to take ownership.
        pDecoded = aOriginal.AllocL();
        }
    CleanupStack::PushL(pDecoded);
    return pDecoded;
    }

TBool SenXmlUtils::ReplaceAll(TPtr8 aDestination,
                              const TDesC8& aFrom,
                              const TDesC8& aTo)
    {
    TInt retVal(EFalse);
    
    TInt pos = aDestination.Find(aFrom);
    if (pos != KErrNotFound) retVal = ETrue;
    
    while (pos != KErrNotFound)
        {
        aDestination.Replace(pos,aFrom.Length(),aTo);
        pos = aDestination.Find(aFrom);
        }

    return retVal;
    }
/**
 * DecodeHttpCharactersL() returns ETrue, if any XML escaping
 * (some Basic Entity) character-sequence was decoded.
 *
 * Note that aDecoded points to a copy of the original string
 * even if there were no basic entities to decode. Otherwise,
 * it points to a newly allocated descriptor, where some
 * basic entity or entities has been decoded (replaced) from
 * aOriginal descriptor.
 * 
 */
EXPORT_C TBool SenXmlUtils::DecodeHttpCharactersL( const TDesC8& aOriginal,
                                                   HBufC8*& aDecoded )
    {
    delete aDecoded;
    aDecoded = NULL;

    // make a copy of the original
    aDecoded = aOriginal.AllocL();

    TBool retVal(EFalse);


    // replace all five basic entities with XML escaping counterpart,
    // if found.
    if (ReplaceAll(aDecoded->Des(), KSenEscapedAmp(), KSenAmpersandDesC8()))
        {
        retVal = ETrue;
        }
    if (ReplaceAll(aDecoded->Des(), KSenEscapedApos(), KSenAposDesC8()))
        {
        retVal = ETrue;
        }
    if (ReplaceAll(aDecoded->Des(), KSenEscapedDblQuot(), KSenDblQuotDesC8()))
        {
        retVal = ETrue;
        }
    if (ReplaceAll(aDecoded->Des(), KSenEscapedGt(), KSenGtDesC8()))
        {
        retVal = ETrue;
        }
    if (ReplaceAll(aDecoded->Des(), KSenEscapedLt(), KSenLtDesC8()))
        {
        retVal = ETrue;
        }

    return retVal;
    }

/**
* In Symbian OS v9.1, see
* Location: EscapeUtils.h 
* Link against: inetprotutil.lib 
*
*/
EXPORT_C HBufC8* SenXmlUtils::DecodeHttpCharactersLC(const TDesC8& aOriginal)
    {
    HBufC8* pDecoded = NULL;

    // Attempt to decode. Note, that even if no Basic Entities were
    // decoded into XML escaping characters (&,',",< or >), this
    // function will return a pointer to a copy of the original
    // descriptor.
    DecodeHttpCharactersL(aOriginal, pDecoded);

    // Push to cleanup stack
    CleanupStack::PushL(pDecoded);
    return pDecoded;
    }

EXPORT_C HBufC8* SenXmlUtils::AllocAttrValueL(  const RAttributeArray& apAttrs,
                                                const TDesC8& aAttrName )
    {
    TPtrC8 p = AttrValue(apAttrs, aAttrName);
    if (p == KNullDesC8)
        {
        return NULL;
        }
    else
        {
        return p.AllocL();
        }
    }

// the non-optimized version:
/* 
EXPORT_C void SenXmlUtils::LeaveOnXmlEscapesL(const TDesC8& aCandidate)
    {
    HBufC8* pValidator = NULL;
    
    // check if there are any XML escaping character by calling
    // basic entity encoding function. 
    TBool containsEscapes = SenXmlUtils::EncodeHttpCharactersL(aCandidate, pValidator);

    delete pValidator;

    if(containsEscapes)
        {
        // some XML-escaping character was found.
        // Indicate with Leave, that the given 
        // aChars descriptor is illegal, because
        // it contained invalid characters
        User::Leave(KErrSenInvalidCharacters);
        }
    }
*/

// Optimized, but not throughoutly tested version.
// Based on UPnP project optimizations (2005-10-13).

EXPORT_C void SenXmlUtils::LeaveOnXmlEscapesL(const TDesC8& aCandidate)
    {
    // these are the escapes which are searched after
    TText16 tokenChars[] =
        {
        '&',
        '\'',
        '\"',
        '>',
        '<'
        };
    
    // Search for XML escaping characters
    for (TInt i=0; i<aCandidate.Length(); i++)
        {
        for (TUint j = 0; j < (sizeof(tokenChars) / sizeof(TText16)); j++)
            {
            if (aCandidate[i] == tokenChars[j])
                {
                // some XML-escaping character was found.
                // Indicate with Leave, that the given 
                // aChars descriptor is illegal, because
                // it contained invalid characters
                User::Leave(KErrSenInvalidCharacters);
                }
            }
        }
    }

void SenXmlUtils::LeaveOnInvalidElementNameL(const TDesC8& aCandidate)
    {
    if (aCandidate == KNullDesC8)
        {
        User::Leave(KErrSenZeroLengthDescriptor);
        }
    SenXmlUtils::LeaveOnXmlEscapesL(aCandidate);
    }

EXPORT_C CSenBaseAttribute* SenXmlUtils::RemoveAttributeL(CSenElement& aElement,
                                                          const TDesC8& aAttrName)
    {
    CSenBaseAttribute* pAttribute = SenXmlUtils::FindAttrL( aElement, aAttrName );
    if ( pAttribute )
        {
        RPointerArray<CSenBaseAttribute>& attributes = aElement.AttributesL();
        
        TInt index(attributes.Find(pAttribute));
        if ( index != KErrNotFound )
            {
            attributes.Remove(index); // release ownership
            }
        else
            {
            // should never occur:
            pAttribute = NULL; // ownership could not be transferred..
            }
        }

    return pAttribute;
    }

EXPORT_C CSenBaseAttribute* SenXmlUtils::RemoveAttributeL(CSenElement& aElement,
                                                          CSenBaseAttribute* apAttribute)
    {
    return SenXmlUtils::RemoveAttributeL(aElement, apAttribute->Name());
    }
    
CSenBaseAttribute* SenXmlUtils::FindAttrL( CSenElement& aElement,
                                           const TDesC8& aName )
    {
    RPointerArray<CSenBaseAttribute>& attributes = aElement.AttributesL();
    
    TInt count(attributes.Count());
    for (TInt i = 0; i < count; i++)
        {
        CSenBaseAttribute* pAttribute = attributes[i];
        if ( pAttribute->Name() == aName )
            {
            return pAttribute;
            }
        }

    return NULL;
    }

EXPORT_C const TDesC8& SenXmlUtils::AddAttributeL(CSenElement& aElement,
                                                  const TDesC8& aQName,
                                                  const TDesC8& aLocalName,
                                                  const TDesC8& aValue)
    {
    // first we check for possible namespace declarations...
    if ( aQName == KSenXmlns || aLocalName == KSenXmlns)
        {
        // this is a default name space declaration
        aElement.SetNamespaceL(aValue);
        }
    else if (aLocalName == aElement.NsPrefix())
        {
        //we have a new declaration for the namespace of this element
        aElement.SetNamespaceL(aLocalName, aValue);
        }
    else if(aQName.Find(KSenXmlNsAttNamePlusColon) == 0)
        {
        // we have an additional namespace declaration
        aElement.AddNamespaceL(aLocalName, aValue);
        }
    else
        {
        // we have a real attribute!
        CSenBaseAttribute* pAttribute = CSenBaseAttribute::NewL(aQName,
                                                                aLocalName,
                                                                aValue);
        AddAttributeL(aElement, pAttribute);
        }
    return aValue;
    }

EXPORT_C const TDesC8& SenXmlUtils::AddAttributeL(CSenElement& aElement,
                                                  const TDesC8& aAttrName,
                                                  const TDesC8& aValue)
    {
    // note, the aAttrName may be a qualified name or simply localname
    // strip off the possible prefix from possible qualified name:
    TPtrC8 localName = SenXmlUtils::LocalName(aAttrName);
    return AddAttributeL(aElement, aAttrName, localName, aValue);
    }
    
EXPORT_C const TDesC8& SenXmlUtils::AddAttributeL(CSenElement& aElement,
                                                  CSenBaseAttribute* apAttribute)
    {
    CSenBaseAttribute* pOldAtt = FindAttrL( aElement, apAttribute->Name() );

    if(!pOldAtt)
        {
        // transfrer the ownership to this class:
        RPointerArray<CSenBaseAttribute>& attributes = aElement.AttributesL();
#ifdef EKA2
        attributes.AppendL(apAttribute);
#else
        User::LeaveIfError(attributes.Append(apAttribute));
#endif // EKA2
        return apAttribute->Value();
        }
    else
        {
        pOldAtt->SetValueL(apAttribute->Value());
        delete apAttribute;
        apAttribute = NULL;
        return pOldAtt->Value();
        }
    }

// END OF FILE

