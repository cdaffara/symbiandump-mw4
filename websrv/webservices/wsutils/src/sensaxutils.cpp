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
#include "sensaxutils.h"
#include <SenXmlConstants.h>


EXPORT_C TInt SenSaxUtils::SearchStartTagL(const TDesC8& aDocument,
                                           const TDesC8& aLocalName,
                                           TInt& aStartTagStart,
                                           TInt& aStartTagEnd,
                                           TPtrC8& aPrefix)
    {
    return SearchStartTagL(aDocument, aLocalName, aStartTagStart, aStartTagEnd,
                           aPrefix, EFalse);
    }


EXPORT_C TInt SenSaxUtils::SearchStartTagL(const TDesC8& aDocument,
                                           const TDesC8& aPrefix,
                                           const TDesC8& aLocalName,
                                           TInt& aStartTagStart,
                                           TInt& aStartTagEnd)
    {
    TPtrC8 tmpPrefix(aPrefix);
    return SearchStartTagL(aDocument, aLocalName, aStartTagStart, aStartTagEnd,
                           tmpPrefix, ETrue);
    }

EXPORT_C TInt SenSaxUtils::SearchEndTagL(const TDesC8& aDocument,
                                                const TDesC8& aLocalName,
                                                TInt& aEndTagStart,
                                                TInt& aEndTagEnd,
                                                TPtrC8& aPrefix)
    {
    return SearchEndTagL(aDocument, aLocalName, aEndTagStart, aEndTagEnd,
                         aPrefix, EFalse);
    }

EXPORT_C TInt SenSaxUtils::SearchEndTagL(const TDesC8& aDocument,
                                                const TDesC8& aPrefix,
                                                const TDesC8& aLocalName,
                                                TInt& aEndTagStart,
                                                TInt& aEndTagEnd)
    {
    TPtrC8 tmpPrefix(aPrefix);
    return SearchEndTagL(aDocument, aLocalName, aEndTagStart, aEndTagEnd,
                         tmpPrefix, ETrue);
    }

TInt SenSaxUtils::SearchStartTagL(const TDesC8& aDocument,
                                  const TDesC8& aLocalName,
                                  TInt& aStartTagStart,
                                  TInt& aStartTagEnd,
                                  TPtrC8& aPrefix,
                                  TBool aSearchPrefix = EFalse)

    {
    TPtrC8 localNamePtr(KNullDesC8);
    TPtrC8 stPrefixPtr(KNullDesC8);
        
    TInt i;
    TBool tagFound(EFalse);
    TInt colon(KErrNotFound);
    TInt lessThan(KErrNotFound);
    TInt space(KErrNotFound);
    
    TInt size = aDocument.Size()-(aLocalName.Size()+3); // </a> <=> at least size-4
    TPtrC8 currentCharacter(KNullDesC8);
    for (i = 0; i < size; i++)
        {
        currentCharacter.Set(aDocument.Ptr()+i,1);
        
        // ************* Handle Quotes <=> ' or " ******** //
        if ( currentCharacter == KSenDblQuot ) // Check "\""
            {
            i = SearchMarkStartingFromPlus(aDocument, KSenDblQuot, i+1, size);
            if ( i == KErrNotFound )
                {
                return i;
                }
            }
        else if ( currentCharacter == KSenQuot ) // Check "'"
            {
            i = SearchMarkStartingFromPlus(aDocument, KSenQuot, i+1, size);
            if ( i == KErrNotFound )
                {
                return i;
                }
            }
        else
            {
            // ************* Handle "<" ****************** //
            if ( currentCharacter == KSenLessThan )   // Check "<"
                {
                currentCharacter.Set(aDocument.Ptr()+i,4);
                // ************* Handle comments ********* //
                if ( currentCharacter == KSenXmlCommentStart )          // Check "<!--"
                    {
                    i+=3;
                    i = SearchEndOfCommentStartingFrom(aDocument, i, size);
                    if ( i == KErrNotFound )
                        {
                        return i;
                        }
                    }
                else
                    {
                    currentCharacter.Set(aDocument.Ptr()+i,2);
                    if ( currentCharacter == KSenXmlInstructionStart || // Check "<?"
                         currentCharacter == KSenXmlEntityStart ||      // Check "<!"
                         currentCharacter == KSenLessThanSlash )        // Check "<\"
                        {
                        i+=1;
                        i = SearchMatchingGreaterThanStartingFrom(aDocument,
                                                                  i,
                                                                  size);
                        if ( i == KErrNotFound )
                            {
                            return i;
                            }
                        }
                    else
                        {
                        colon = KErrNotFound;
                        space = KErrNotFound;
                        lessThan = i;
                        }
                    }
                }
            // ************* Handle ">" ****************** //                
            else if ( currentCharacter == KSenGreaterThan ) // Check ">"
                {
                if ( lessThan != KErrNotFound )
                    {
                    if ( colon != KErrNotFound )
                        {
                        // Both Prefix and LocalName can be found.
                        if ( space != KErrNotFound )
                            {
                            localNamePtr.Set(aDocument.Ptr()+colon+1, space-colon-1);
                            }
                        else
                            {
                            localNamePtr.Set(aDocument.Ptr()+colon+1, i-colon-1);
                            }
                        if ( !aSearchPrefix )
                            {
                            aPrefix.Set(aDocument.Ptr()+lessThan+1, colon-lessThan-1);
                            }
                        else
                            {
                            stPrefixPtr.Set(aDocument.Ptr()+lessThan+1, colon-lessThan-1);
                            }
                        }
                    else
                        {
                        // Only LocalName can be found.
                        if ( space != KErrNotFound )
                            {
                            localNamePtr.Set(aDocument.Ptr()+lessThan+1, space-lessThan-1);
                            }
                        else
                            {
                            localNamePtr.Set(aDocument.Ptr()+lessThan+1, i-lessThan-1);
                            }
                        stPrefixPtr.Set(KNullDesC8);                            
                        }

                    if ( localNamePtr == aLocalName )
                        {
                        if ( aSearchPrefix )              
                            {
                            if ( stPrefixPtr == aPrefix )
                                {
                                tagFound = ETrue;
                                break;
                                }
                            else
                                {
                                tagFound = EFalse;
                                lessThan = KErrNotFound;
                                colon = KErrNotFound;
                                space = KErrNotFound;
                                }
                            }
                        else
                            {
                            tagFound = ETrue;
                            break;
                            }
                        }
                    else
                        {
                        tagFound = EFalse;
                        lessThan = KErrNotFound;
                        colon = KErrNotFound;
                        space = KErrNotFound;
                        }
                    }
                }
            // ************* Handle ":" ****************** //                
            else if ( currentCharacter == KSenColon ) // Check ":"
                {
                if ( lessThan != KErrNotFound &&
                     space == KErrNotFound &&
                     colon == KErrNotFound )
                    {
                    colon = i;
                    }
                }
            // ************* Handle " " ****************** //                
            else if ( currentCharacter == KSenSpace ) // Check " "
                {
                if ( lessThan != KErrNotFound && 
                     space == KErrNotFound )
                    {
                    space = i;
                    }
                }
            }        
        }

    if ( tagFound )
        {
        aStartTagStart   = lessThan;
        aStartTagEnd     = i;
        }
    else
        {
        aStartTagStart   = KErrNotFound;
        aStartTagEnd     = KErrNotFound;
        return KErrNotFound;
        }

    return KErrNone;
    }

TInt SenSaxUtils::SearchEndTagL(const TDesC8& aDocument,
                                const TDesC8& aLocalName,
                                TInt& aEndTagStart,
                                TInt& aEndTagEnd,
                                TPtrC8& aPrefix,
                                TBool aSearchPrefix)
    {
    // Search EndTag starting from the end
    // of XML Document
    TPtrC8 localNamePtr(KNullDesC8);
    TPtrC8 etPrefixPtr(KNullDesC8);
    
    TInt colon(KErrNotFound);
    TInt greaterThan(KErrNotFound);
    TBool tagFound      = EFalse;

    TInt i=aDocument.Size()-1;
    TPtrC8 currentCharacter(KNullDesC8);
    for (; i > 2; i--)
        {
        currentCharacter.Set(aDocument.Ptr()+i,1);
        
        // ************* Handle Quotes <=> ' or " ******** //
        if ( currentCharacter == KSenDblQuot ) // Check "\""
            {
            i = SearchMarkStartingFromMinus(aDocument, KSenDblQuot, i-1);
            if ( i == KErrNotFound )
                {
                return i;
                }
            }
        else if ( currentCharacter == KSenQuot ) // Check "'"
            {
            i = SearchMarkStartingFromMinus(aDocument, KSenQuot, i-1);
            if ( i == KErrNotFound )
                {
                return i;
                }
            }
        else
            {
            // ************* Handle ">" ****************** //
            if ( currentCharacter == KSenGreaterThan ) // Check ">"
                {
                currentCharacter.Set(aDocument.Ptr()+i-2,3);
                // ************* Handle comments ********* //
                if ( currentCharacter == KSenXmlCommentEnd ) // Check "-->"
                    {
                    i-=3;
                    i = SearchStartOfCommentStartingFrom(aDocument, i);
                    if ( i == KErrNotFound )
                        {
                        return i;
                        }
                    }
                else
                    {
                    greaterThan = i;
                    colon = KErrNotFound;
                    }
                }
            // ************* Handle "<" ****************** //
            else if ( currentCharacter == KSenLessThan ) // Check "<"
                {
                currentCharacter.Set(aDocument.Ptr()+i,2);
                if ( currentCharacter == KSenXmlInstructionStart || // Check "<?"
                     currentCharacter == KSenXmlEntityStart)        // Check "<!" 
                    {
                    greaterThan = KErrNotFound;
                    colon = KErrNotFound;
                    }
                else if ( currentCharacter == KSenLessThanSlash )   // Check "</"
                    {
                    if ( greaterThan != KErrNotFound )
                        {
                        if ( colon != KErrNotFound )
                            {
                            // Both Prefix and LocalName was found.
                            localNamePtr.Set(aDocument.Ptr()+colon+1, greaterThan-colon-1);
                            if ( !aSearchPrefix )
                                {
                                aPrefix.Set(aDocument.Ptr()+i+2, colon-i-2);
                                }
                            else
                                {
                                etPrefixPtr.Set(aDocument.Ptr()+i+2, colon-i-2);
                                }
                            }
                        else
                            {
                            // Only LocalName was found.
                            localNamePtr.Set(aDocument.Ptr()+i+2, greaterThan-i-2);
                            etPrefixPtr.Set(KNullDesC8);
                            }

                        if ( localNamePtr == aLocalName )
                            {
                            if ( aSearchPrefix )              
                                {
                                if ( etPrefixPtr == aPrefix )
                                    {
                                    tagFound = ETrue;
                                    break;
                                    }
                                else
                                    {
                                    tagFound = EFalse;
                                    greaterThan = KErrNotFound;
                                    colon = KErrNotFound;
                                    }
                                }
                            else
                                {
                                tagFound = ETrue;
                                break;
                                }
                            }
                        else
                            {
                            tagFound = EFalse;
                            greaterThan = KErrNotFound;
                            colon = KErrNotFound;
                            }
                        }
                    }
                }
            else if ( currentCharacter == KSenColon ) // Check ":"
                {
                if ( greaterThan != KErrNotFound )
                    {
                    colon = i;
                    }
                }
            }
        }
        
    if ( tagFound )
        {
        aEndTagStart = i;
        aEndTagEnd   = greaterThan;
        }
    else
        {
        aEndTagStart = KErrNotFound;
        aEndTagEnd   = KErrNotFound;
        return KErrNotFound;
        }
        
    return KErrNone;
    }

TInt SenSaxUtils::SearchMarkStartingFromPlus(const TDesC8& aDocument,
                                             const TDesC8& aMark,
                                             TInt position,
                                             TInt size)
    {
    TPtrC8 currentCharacter;
    TInt i = position;
    for (; i < size; i++ )
        {
        currentCharacter.Set(aDocument.Ptr()+i,1);
        if ( currentCharacter == aMark )
            {
            return i;
            }
        }
    
    return KErrNotFound;
    }

TInt SenSaxUtils::SearchMarkStartingFromMinus(const TDesC8& aDocument,
                                              const TDesC8& aMark,
                                              TInt position)
    {
    TPtrC8 currentCharacter;
    TInt i = position;
    for (; i >= 0; i--)
        {
        currentCharacter.Set(aDocument.Ptr()+i,1);
        if ( currentCharacter == aMark )
            {
            return i;
            }
        }
    
    return KErrNotFound;
    }

TInt SenSaxUtils::SearchMatchingGreaterThanStartingFrom(const TDesC8& aDocument,
                                                        TInt position,
                                                        TInt size)
    {
    TInt lessThanCount = 1;
    TPtrC8 currentCharacter;
    
    for (TInt i = position; i < size; i++)
        {
        currentCharacter.Set(aDocument.Ptr()+i,1);
        
        if ( currentCharacter == KSenDblQuot ) // Check "\""
            {
            i = SearchMarkStartingFromPlus(aDocument, KSenDblQuot, i+1, size);
            if ( i == KErrNotFound )
                {
                return i;
                }
            }
        else if ( currentCharacter == KSenQuot ) // Check "'"
            {
            i = SearchMarkStartingFromPlus(aDocument, KSenQuot, i+1, size);
            if ( i == KErrNotFound )
                {
                return i;
                }
            }
        else
            {
            if ( currentCharacter == KSenLessThan )   // Check "<"
                {
                currentCharacter.Set(aDocument.Ptr()+i,4);
                if ( currentCharacter == KSenXmlCommentStart )          // Check "<!--"
                    {
                    i+=3;
                    i = SearchEndOfCommentStartingFrom(aDocument, i, size);
                    }
                else
                    {
                    lessThanCount++;
                    }
                }
            else if ( currentCharacter == KSenGreaterThan ) // Check ">"
                {
                lessThanCount--;
                if ( lessThanCount == 0)
                    {
                    return i;
                    }
                }
            }            
        }
    
    return KErrNotFound;
    }

TInt SenSaxUtils::SearchEndOfCommentStartingFrom(const TDesC8& aDocument,
                                                 TInt position,
                                                 TInt size)
    {
    TPtrC8 currentCharacter;

    for (TInt i = position; i < size; i++)
        {
        currentCharacter.Set(aDocument.Ptr()+i,1);
        
        if ( currentCharacter == KSenGreaterThan ) // Check ">"
            {
            currentCharacter.Set(aDocument.Ptr()+i-2,3);
            if ( currentCharacter == KSenXmlCommentEnd ) // Check "-->"
                {
                return i;
                }
            }
        }
        
    return KErrNotFound;        
    }

TInt SenSaxUtils::SearchStartOfCommentStartingFrom(const TDesC8& aDocument,
                                                   TInt position)
    {
    TPtrC8 currentCharacter;

    for (TInt i = position; i >= 0; i--)
        {
        currentCharacter.Set(aDocument.Ptr()+i,1);
        
        if ( currentCharacter == KSenLessThan ) // Check "<"
            {
            currentCharacter.Set(aDocument.Ptr()+i,4);
            if ( currentCharacter == KSenXmlCommentStart ) // Check "<!--"
                {
                return i;
                }
            }
        }
        
    return KErrNotFound;        
    }

// End of File

