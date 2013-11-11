/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Implementation of CXmlPullParser.
*
*/



#include <eikenv.h>
#include <e32std.h>
#include <e32des8.h>
#include "XppImpl.h"

const TInt KExtraPunctuation = 5;
const TInt KBufSizeIncrement = 1024;
const TInt KBufInitSize = 2048;

//
// Common literal text
//
_LIT8(KOpenBracket, "<");
_LIT8(KCloseBracket, ">");
//_LIT8(KStartEndTag, "</");
//_LIT8(KCloseTag, "/>");
_LIT8(KSpace, " ");
_LIT8(KEquals, "=");
_LIT8(KSlash, "/");
_LIT8(KSingleQuote, "\'");
_LIT8(KDoubleQuote, "\"");
//_LIT8(KQuestionMark, "?");
//_LIT8(KExclamationMark, "!");
//_LIT8(KOpenCurlyBracket, "{");
//_LIT8(KCloseCurlyBracket, "}");
//_LIT8(KHyphen, "-");
_LIT8(KOpenAngleBracket , "[");
_LIT8(KCloseAngleBracket, "]");

_LIT8(KAmpersand, "&");
_LIT8(KAmpersandHash, "&#");
_LIT8(KSemicolon, ";");

// XML special marks
//_LIT8(KStartXmlProlog, "<?xml");
//_LIT8(KStartComment, "<!--");
_LIT8(KEndComment, "-->");
//_LIT8(KStartProcessingInstructions, "<?");
_LIT8(KEndPIAndProlog, "?>");
_LIT8(KStartCData, "<![CDATA[");
_LIT8(KEndCData, "]]>");
//_LIT8(KStartDocType, "<!DOCTYPE");
//_LIT8(KStartElement, "<!ELEMENT");
//_LIT8(KStartAttList, "<!ATTLIST");
//_LIT8(KStartEntity, "<!ENTITY");
//_LIT8(KStartNotation, "<!NOTATION");
//_LIT8(KEndCdata, "]]>");

_LIT8(KEntityLowerThan, "&lt;");           // <
_LIT8(KEntityGreaterThan, "&gt;");         // >
_LIT8(KEntityAmpersand, "&amp;");          // &
_LIT8(KEntityApostrophe, "&apos;");        // '
_LIT8(KEntityQuotation, "&quot;");         // "


////////////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////////////
CXmlPullParser::CXmlPullParser()
: iState(EStateStartDocument), iCurrentElementPos(0), iPathBufPtr(NULL, 0), iDepth(0), 
iIsEmptyElement(EFalse), iCdataStartFound(EFalse), iCdataSectionText(EFalse),
iStringValuePtr(NULL, 0), iStringValueArray(NULL)
    {
    }

////////////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////////////
// NewLC with two stage construction 
EXPORT_C CXmlPullParser* CXmlPullParser::NewLC(CDesC8ArrayFlat& aStringValueArray)
    {
    // get new, leave if can't
    CXmlPullParser* self = new (ELeave) CXmlPullParser();
    // push onto cleanup stack in case self->ConstructL leaves 
    CleanupStack::PushL(self);
    // complete construction with second phase constructor 
    self->ConstructL(aStringValueArray);
    return self;
    }

////////////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////////////
EXPORT_C CXmlPullParser* CXmlPullParser::NewL(CDesC8ArrayFlat& aStringValueArray) 
    {
    CXmlPullParser* self=NewLC(aStringValueArray);
    CleanupStack::Pop();
    return self;
    }

////////////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////////////
void CXmlPullParser::ConstructL(CDesC8ArrayFlat& aStringValueArray)
    {
    iPathBuf = HBufC8::NewMaxL(KBufInitSize);
    iPathBufPtr.Set(iPathBuf->Des());
    iPathBufPtr.SetLength(0);  // Otherwise the Length is set to KBufInitSize
    iCAttrDataArray = new (ELeave) CArrayFixFlat<TAttrData>(1);
    iStringValue = HBufC8::NewMaxL(KBufInitSize);
    iStringValuePtr.Set(iStringValue->Des());
    iStringValuePtr.SetLength(0);  // Otherwise the Length is set to KBufInitSize
    //iStringValueArray = new (ELeave) CDesC8ArrayFlat(1);
    iStringValueArray = &aStringValueArray;
    }

////////////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////////////
CXmlPullParser::~CXmlPullParser()
    {
    delete (iPathBuf);
    delete iCAttrDataArray;
    delete (iStringValue);
    //delete iStringValueArray;
    }


////////////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////////////
void CXmlPullParser::CheckSpaceL(const TDesC8& aText)
    {
    TInt spaceLeft = iPathBufPtr.MaxLength() - iPathBufPtr.Length() - KExtraPunctuation;
    if (aText.Length() > spaceLeft)
        {
        TInt newMaxLength = iPathBufPtr.MaxLength() + aText.Length() + KBufSizeIncrement;
        iPathBuf = iPathBuf->ReAllocL(newMaxLength);
        iPathBufPtr.Set(iPathBuf->Des());
        }
    }


////////////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////////////
EXPORT_C void CXmlPullParser::SetInput(const TDesC8& aInput)
    {
    iState = EStateStartDocument;
    iText.Set(KNullDesC8);
    iCAttrDataArray->Delete(0, iCAttrDataArray->Count());
    iDocument.Set(aInput);
    iDocPart.Set(KNullDesC8);
    iLexer.Assign(aInput);  // Assigns a string to this object from a descriptor
    iCurrentElementPos = 0;
    iPathBufPtr.Zero();
    iElement.Set(KNullDesC8);
    iDepth = 0;
    iIsEmptyElement = EFalse;
    iCdataStartFound = EFalse;
    iCdataSectionText = EFalse;
    }

////////////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////////////
EXPORT_C TInt CXmlPullParser::NextL()
    {
    TInt rv;
    
    switch (iState)
        {
        case EStateStartDocument:
            // skip
            if ((rv = SkipLeadingNonRelevantElements()) == ERcDocumentError)
                {
                return rv;
                }
            else
                {
                iCurrentElementPos = iLexer.Offset();
                rv = GetElementStartTagL();
                return rv;
                }
            //break;
            
        case EStateStartTag:
            // skip
            if ((rv = SkipNonRelevantElements()) == ERcDocumentError)
                {
                return rv;
                }
            else
                {
                iCurrentElementPos = iLexer.Offset();
                if (iIsEmptyElement)
                    {
                    // delete /end tag from the iPathBuf
                    // this is done by subtracting the length of the buffer
                    iPathBufPtr.SetLength(iPathBufPtr.LocateReverse('/'));
                    
                    // decrement iDepth
                    iDepth--;

                    // if we have an XML-document that is only one empty-element
                    // then we must check if we are already in the end of the document
                    if (CheckIfEndOfFile())
                        {
                        return KErrNone;
                        }
                    }
                if (CheckIfNextEndTag())  // Check iState!!
                    {
                    if (iState == EStateError)
                        {
                        return ERcDocumentError;
                        }
                    else
                        {
                        rv = GetElementEndTag();
                        return rv;
                        }
                    }
                else if (CheckIfNextStartTag())  // Check iState!!
                    {
                    if (iState == EStateError)
                        {
                        return ERcDocumentError;
                        }
                    else
                        {
                        rv = GetElementStartTagL();
                        return rv;
                        }
                    }
                else // etext
                    {
                    rv = GetElementText();
                    return rv;
                    }
                }
            //break;

        case EStateText:
            // skip
            if ((rv = SkipNonRelevantElements()) == ERcDocumentError)
                {
                return rv;
                }
            else
                {
                iCurrentElementPos = iLexer.Offset();
                if (iState == EStateEndDocument)
                    {
                    // in EStateText we can not be in the end of the xml-document
                    iState = EStateError;
                    return ERcDocumentError;
                    }

                if (iCdataStartFound)
                    {
                    rv = GetElementText();
                    return rv;
                    }
                else
                    {
                    if (CheckIfNextEndTag())  // Check iState!!
                        {
                        if (iState == EStateError)
                            {
                            return ERcDocumentError;
                            }
                        else
                            {
                            // get end tag
                            rv = GetElementEndTag();
                            return rv;
                            }
                        }
                    else if (CheckIfNextStartTag())  // Check iState!!
                        {
                        if (iState == EStateError)
                            {
                            return ERcDocumentError;
                            }
                        else
                            {
                            // get start tag
                            rv = GetElementStartTagL();
                            return rv;
                            }
                        }
                    else // etext
                        {
                        // get text
                        rv = GetElementText();
                        return rv;
                        }
                    }
                }
            //break;

        case EStateEndTag:
            // after end tag can be end of file, new start tag, new end tag or etext
            // skip
            if ((rv = SkipNonRelevantElements()) == ERcDocumentError)
                {
                return rv;
                }
            else
                {
                iCurrentElementPos = iLexer.Offset();
                if (CheckIfEndOfFile())
                    {
                    return KErrNone;
                    }
                else
                    {
                    if (CheckIfNextStartTag())  // Check iState
                        {
                        if (iState == EStateError)
                            {
                            return ERcDocumentError;
                            }
                        else
                            {
                            rv = GetElementStartTagL();
                            return rv;
                            }
                        }
                    else if (CheckIfNextEndTag())  // Check iState!!
                        {
                        if (iState == EStateError)
                            {
                            return ERcDocumentError;
                            }
                        else
                            {
                            rv = GetElementEndTag();
                            return rv;
                            }
                        }
                    else // etext
                        {
                        // get text
                        rv = GetElementText();
                        return rv;
                        }
                    }
                }

            //break;
        case EStateEndDocument:
            return ERcWrongParserState;
            //break;
        case EStateError:
            return ERcWrongParserState;
            //break;
        default:
            return ERcUnknown;  // all the States are handled above
            //break;
        }
    }

////////////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////////////
EXPORT_C CXmlPullParser::TParserState CXmlPullParser::State()
    {
    return iState;
    }

////////////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////////////
EXPORT_C TInt CXmlPullParser::Name(TPtrC8& aName)
    {
    if ((iState == EStateStartTag) || 
        (iState == EStateText) || 
        (iState == EStateEndTag) ||
        (iState == EStateEndDocument))
        {
        aName.Set(iElement);
        return KErrNone;
        }
    else
        {
        return ERcWrongParserState;
        }
    }

////////////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////////////
EXPORT_C TInt CXmlPullParser::PathL(TPtrC8& aPath)
    {
    if ((iState == EStateStartTag) || 
        (iState == EStateText) || 
        (iState == EStateEndTag))
        {
        aPath.Set(iPathBufPtr);
        return KErrNone;
        }
    else
        {
        return ERcWrongParserState;
        }
    }

////////////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////////////
EXPORT_C TInt CXmlPullParser::Depth(TInt& aDepth)
    {
    if ((iState == EStateStartTag) || 
        (iState == EStateText) || 
        (iState == EStateEndTag))
        {
        aDepth = iDepth;
        return KErrNone;
        }
    else
        {
        return ERcWrongParserState;
        }
    }

////////////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////////////
EXPORT_C TInt CXmlPullParser::AttributeCount(TInt& aCount)
    {
    if (iState == EStateStartTag)
        {
        aCount = iCAttrDataArray->Count();
        return KErrNone;
        }
    else
        {
        return ERcWrongParserState;
        }
    }

////////////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////////////
EXPORT_C TInt CXmlPullParser::AttributeName(TInt aIndex, TPtrC8& aName)
    {
    if (iState == EStateStartTag)
        {
        if ((aIndex < iCAttrDataArray->Count()) && (aIndex >= 0))
            {
            aName.Set(iCAttrDataArray->At(aIndex).iName);
            return KErrNone;
            }
        else
            {
            return KErrNotFound;
            }
        }
    else
        {
        return ERcWrongParserState;
        }
    }

////////////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////////////
EXPORT_C TInt CXmlPullParser::AttributeValuePredefEntitiesNotEscaped(TInt aIndex, TPtrC8& aValue)
    {
    if (iState == EStateStartTag)
        {
        if ((aIndex < iCAttrDataArray->Count()) && (aIndex >= 0))
            {
            aValue.Set(iCAttrDataArray->At(aIndex).iValue);
            return KErrNone;
            }
        else
            {
            return KErrNotFound;
            }
        }
    else
        {
        return ERcWrongParserState;
        }
    }

////////////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////////////
EXPORT_C TInt CXmlPullParser::AttributeValuePredefEntitiesNotEscaped(const TDesC8& aName, TPtrC8& aValue)
    {
    TInt count;
    TInt index = 0;
    if (iState == EStateStartTag)
        {
        count = iCAttrDataArray->Count();
        while (index < count)
            {
            if (iCAttrDataArray->At(index).iName == aName)
                {
                aValue.Set(iCAttrDataArray->At(index).iValue);
                return KErrNone;
                }
            else
                {
                index ++;
                }
            }
        return KErrNotFound;
        }
    else
        {
        return ERcWrongParserState;
        }
    }


////////////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////////////
EXPORT_C TInt CXmlPullParser::AttributeValueL(TInt aIndex, TPtrC8& aValue)
    {
    TInt ret;
    ret = AttributeValuePredefEntitiesNotEscaped(aIndex, aValue);
    
    if (ret == KErrNone)
        {
        CheckStringValueBufferSpaceL(aValue);
        
        iStringValuePtr = aValue;
        
        if ( (ret = EscapeCharRefAndPreDefEntities()) != KErrNone )
            {
            return ret;
            }
        else
            {
            iStringValueArray->AppendL(iStringValuePtr);
            aValue.Set(iStringValueArray->MdcaPoint(iStringValueArray->MdcaCount()-1));
            return KErrNone;
            }
        }
    else
        {
        return ret;
        }
    }


////////////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////////////
EXPORT_C TInt CXmlPullParser::AttributeValueL(const TDesC8& aName, TPtrC8& aValue)
    {
    TInt ret;
    ret = AttributeValuePredefEntitiesNotEscaped(aName, aValue);

    if (ret == KErrNone)
        {
        CheckStringValueBufferSpaceL(aValue);

        iStringValuePtr = aValue;

        if ( (ret = EscapeCharRefAndPreDefEntities()) != KErrNone )
            {
            return ret;
            }
        else
            {
            iStringValueArray->AppendL(iStringValuePtr);
            aValue.Set(iStringValueArray->MdcaPoint(iStringValueArray->MdcaCount()-1));
            return KErrNone;
            }
        }
    else
        {
        return ret;
        }
    }

////////////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////////////
void CXmlPullParser::CheckStringValueBufferSpaceL(const TDesC8& aValue)
    {
    TInt newLength = aValue.Length();
    if ( newLength > (iStringValuePtr.MaxLength()) )
        {
        iStringValue = iStringValue->ReAllocL(newLength);
        iStringValuePtr.Set(iStringValue->Des());
        }
    }

////////////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////////////
EXPORT_C TInt CXmlPullParser::IsEmptyElement(TBool& aIsEmptyElement)
    {
    if (iState == EStateStartTag)
        {
        aIsEmptyElement = iIsEmptyElement;
        return KErrNone;
        }
    else
        {
        return ERcWrongParserState;
        }
    }

////////////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////////////
EXPORT_C TInt CXmlPullParser::TextPredefEntitiesNotEscaped(TPtrC8& aText)
    {
    if (iState == EStateText)
        {
        aText.Set(iText);
        return KErrNone;
        }
    else
        {
        return ERcWrongParserState;
        }
    }

////////////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////////////
EXPORT_C TInt CXmlPullParser::TextL(TPtrC8& aText)
    {
    TInt ret;
    ret = TextPredefEntitiesNotEscaped(aText);

    if (ret == KErrNone)
        {
        CheckStringValueBufferSpaceL(aText);

        iStringValuePtr = aText;

        if ( !(iCdataSectionText) )
            {
            if ( (ret = EscapeCharRefAndPreDefEntities()) != KErrNone )
                {
                return ret;
                }
            else
                {
                iStringValueArray->AppendL(iStringValuePtr);
                aText.Set(iStringValueArray->MdcaPoint(iStringValueArray->MdcaCount()-1));
                }
            }
        return KErrNone;
        }
    else
        {
        return ret;
        }
    }


////////////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////////////
EXPORT_C TInt CXmlPullParser::Pos()
    {
    return iLexer.Offset();
    }

////////////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////////////
EXPORT_C TInt CXmlPullParser::CurrentElementPos()
    {
    return iCurrentElementPos;
    }

////////////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////////////
EXPORT_C TInt CXmlPullParser::Length()
    {
    return iDocument.Length();
    }

////////////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////////////
EXPORT_C TPtrC8 CXmlPullParser::DocPart(TInt aStartPos, TInt aEndPos)
    {
    iDocPart.Set(iDocument.Mid(aStartPos, (aEndPos - aStartPos + 1)));
    return iDocPart;
    }

////////////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////////////
TInt CXmlPullParser::SkipLeadingNonRelevantElements()
    {
    TPtrC8 token;
    TInt endStringOffset;
    TBool prologFound = ETrue;
    TBool commentFound = ETrue;
    TBool piFound = ETrue;
    TBool dtdFound = ETrue;

    iLexer.SkipSpaceAndMark(iMarkPosition);  // Remember where we are
    
    if (iLexer.Eos())
        {
        iState = EStateEndDocument;
        return KErrNone;
        }
    else
        {
        token.Set(iLexer.Remainder());  // pointer to the rest of the xml document
        
        while (prologFound || commentFound || piFound || dtdFound)
            {
            // check for prolog: if prolog found skip it
            if (iLexer.Get() == '<' && iLexer.Get() == '?' &&
                iLexer.Get() == 'x' && iLexer.Get() == 'm' && iLexer.Get() == 'l')
                {
                token.Set(iLexer.Remainder());  // pointer to the rest of the xml document
                if (token.Find(KEndPIAndProlog) != KErrNotFound)
                    {
                    endStringOffset = token.Find(KEndPIAndProlog) + KEndPIAndProlog().Length();
                    iLexer.Inc(endStringOffset);
                    iLexer.SkipSpaceAndMark(iMarkPosition);  // Remember where we are
                    prologFound = ETrue;
                    }
                else
                    {
                    iState = EStateError;
                    return ERcDocumentError; // no end string found
                    }
                }
            else
                {
                iLexer.UnGetToMark(iMarkPosition);
                prologFound = EFalse;
                }
            
            // check for comment: if found skip it
            if (iLexer.Get() == '<' && iLexer.Get() == '!' &&
                iLexer.Get() == '-' && iLexer.Get() == '-')
                {
                token.Set(iLexer.Remainder());  // pointer to the rest of the xml document
                if (token.Find(KEndComment) != KErrNotFound)
                    {
                    endStringOffset = token.Find(KEndComment) + KEndComment().Length();
                    iLexer.Inc(endStringOffset);
                    iLexer.SkipSpaceAndMark(iMarkPosition);  // Remember where we are
                    commentFound = ETrue;
                    }
                else
                    {
                    iState = EStateError;
                    return ERcDocumentError; // no end string found
                    }
                }
            else
                {
                iLexer.UnGetToMark(iMarkPosition);
                commentFound = EFalse;
                }
            
            // check for processing instructions: if found skip it
            if (iLexer.Get() == '<' && iLexer.Get() == '?')
                {
                token.Set(iLexer.Remainder());  // pointer to the rest of the xml document
                if (token.Find(KEndPIAndProlog) != KErrNotFound)
                    {
                    endStringOffset = token.Find(KEndPIAndProlog) + KEndPIAndProlog().Length();
                    iLexer.Inc(endStringOffset);
                    iLexer.SkipSpaceAndMark(iMarkPosition);  // Remember where we are
                    piFound = ETrue;
                    }
                else
                    {
                    iState = EStateError;
                    return ERcDocumentError; // no end string found
                    }
                }
            else
                {
                iLexer.UnGetToMark(iMarkPosition);
                piFound = EFalse;
                }
            
            // check for document type declaration: if found skip it
            if (iLexer.Get() == '<' && iLexer.Get() == '!' &&
                iLexer.Get() == 'D' && iLexer.Get() == 'O' &&
                iLexer.Get() == 'C' && iLexer.Get() == 'T' &&
                iLexer.Get() == 'Y' && iLexer.Get() == 'P' &&
                iLexer.Get() == 'E')
                {
                token.Set(iLexer.Remainder());  // pointer to the rest of the xml document
                // if '[' found skip to ']' and then search for '>' (= end of doctype)
                if (token.Find(KOpenAngleBracket) != KErrNotFound)
                    {
                    iLexer.Inc(token.Find(KOpenAngleBracket)
                                          + KOpenAngleBracket().Length());
                    token.Set(iLexer.Remainder());
                    // Search for ']'
                    if (token.Find(KCloseAngleBracket) != KErrNotFound)
                        {
                        iLexer.Inc(token.Find(KCloseAngleBracket) + KCloseAngleBracket().Length());
                        token.Set(iLexer.Remainder());
                        }
                    else
                        {
                        iState = EStateError;
                        return ERcDocumentError; // no end string found
                        }
                    }
                if (token.Find(KCloseBracket) != KErrNotFound)
                    {
                    endStringOffset = token.Find(KCloseBracket) + KCloseBracket().Length();
                    iLexer.Inc(endStringOffset);
                    iLexer.SkipSpaceAndMark(iMarkPosition);  // Remember where we are
                    dtdFound = ETrue;
                    }
                else
                    {
                    iState = EStateError;
                    return ERcDocumentError; // no end string found
                    }
                }
            else
                {
                iLexer.UnGetToMark(iMarkPosition);
                dtdFound = EFalse;
                }
            }
        }
        return KErrNone;
    }

////////////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////////////
TInt CXmlPullParser::SkipNonRelevantElements()
    {
    TPtrC8 token;
    TInt endStringOffset;
    TBool prologFound = ETrue;
    TBool commentFound = ETrue;
    TBool piFound = ETrue;
    TBool dtdFound = ETrue;

    iLexer.Mark(iMarkPosition);  // Remember where we are
    
    if (iLexer.Eos())
        {
        iState = EStateEndDocument;
        return KErrNone;
        }
    else
        {
        token.Set(iLexer.Remainder());  // pointer to the rest of the xml document
        
        while (prologFound || commentFound || piFound || dtdFound)
            {
            // check for prolog: if prolog found -> error
            // Spec says: "The document type declaration must appear before 
            // the first element in the document."
            if (iLexer.Get() == '<' && iLexer.Get() == '?' &&
                iLexer.Get() == 'x' && iLexer.Get() == 'm' && iLexer.Get() == 'l')
                {
                prologFound = ETrue;
                iState = EStateError;
                return ERcDocumentError;
                }
            else
                {
                iLexer.UnGetToMark(iMarkPosition);
                prologFound = EFalse;
                }
            
            // check for comment: if found skip it
            if (iLexer.Get() == '<' && iLexer.Get() == '!' &&
                iLexer.Get() == '-' && iLexer.Get() == '-')
                {
                token.Set(iLexer.Remainder());  // pointer to the rest of the xml document
                if (token.Find(KEndComment) != KErrNotFound)
                    {
                    endStringOffset = token.Find(KEndComment) + KEndComment().Length();
                    iLexer.Inc(endStringOffset);
                    iLexer.Mark(iMarkPosition);  // Remember where we are
                    commentFound = ETrue;
                    }
                else
                    {
                    iState = EStateError;
                    return ERcDocumentError; // no end string found
                    }
                }
            else
                {
                iLexer.UnGetToMark(iMarkPosition);
                commentFound = EFalse;
                }
            
            // check for processing instructions: if found skip it
            if (iLexer.Get() == '<' && iLexer.Get() == '?')
                {
                token.Set(iLexer.Remainder());  // pointer to the rest of the xml document
                if (token.Find(KEndPIAndProlog) != KErrNotFound)
                    {
                    endStringOffset = token.Find(KEndPIAndProlog) + KEndPIAndProlog().Length();
                    iLexer.Inc(endStringOffset);
                    iLexer.Mark(iMarkPosition);  // Remember where we are
                    piFound = ETrue;
                    }
                else
                    {
                    iState = EStateError;
                    return ERcDocumentError; // no end string found
                    }
                }
            else
                {
                iLexer.UnGetToMark(iMarkPosition);
                piFound = EFalse;
                }
            
            // check for document type declaration: if found skip it
            if (iLexer.Get() == '<' && iLexer.Get() == '!' &&
                iLexer.Get() == 'D' && iLexer.Get() == 'O' &&
                iLexer.Get() == 'C' && iLexer.Get() == 'T' &&
                iLexer.Get() == 'Y' && iLexer.Get() == 'P' &&
                iLexer.Get() == 'E')
                {
                token.Set(iLexer.Remainder());  // pointer to the rest of the xml document
                // if '[' found skip to ']' and then search for '>' (= end of doctype)
                if (token.Find(KOpenAngleBracket) != KErrNotFound)
                    {
                    iLexer.Inc(token.Find(KOpenAngleBracket)
                                          + KOpenAngleBracket().Length());
                    token.Set(iLexer.Remainder());
                    // Search for ']'
                    if (token.Find(KCloseAngleBracket) != KErrNotFound)
                        {
                        iLexer.Inc(token.Find(KCloseAngleBracket) + KCloseAngleBracket().Length());
                        token.Set(iLexer.Remainder());
                        }
                    else
                        {
                        iState = EStateError;
                        return ERcDocumentError; // no end string found
                        }
                    }
                if (token.Find(KCloseBracket) != KErrNotFound)
                    {
                    endStringOffset = token.Find(KCloseBracket) + KCloseBracket().Length();
                    iLexer.Inc(endStringOffset);
                    iLexer.SkipSpaceAndMark(iMarkPosition);  // Remember where we are
                    dtdFound = ETrue;
                    }
                else
                    {
                    iState = EStateError;
                    return ERcDocumentError; // no end string found
                    }
                
                }                   
            else
                {
                iLexer.UnGetToMark(iMarkPosition);
                dtdFound = EFalse;
                }
            }
        }
        return KErrNone;
    }

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////
TInt CXmlPullParser::GetElementStartTagL()
    {
    TPtrC8 token;
    TPtrC8 startTag;
    TInt offset;
    TInt eqpos;
    TInt spacepos;
    TInt attrValueLen;
    TLex8 tokenLexer;
    TChar ch;


    iLexer.Mark(iMarkPosition);  // Remember where we are

    if (iLexer.Eos())
        {
        iState = EStateError;   // we can not be in the end of the doc when 
                                // we should have Start Tag 
        return ERcDocumentError;
        }
    else
        {

        // Delete attributes from iCAttrDataArray
        iCAttrDataArray->Delete(0, iCAttrDataArray->Count());


        if (iLexer.Get() == '<' && iLexer.Peek().IsAlpha())
            {
            iLexer.Mark(iMarkPosition);  // start of the element
            token.Set(iLexer.Remainder());
            offset = token.Find(KCloseBracket);
            
            // start tag and possible attribute(s)
            startTag.Set(token.Left(offset));
            tokenLexer.Assign(startTag);

            // Start tag format: '<' Name (S Attribute)* S?'>'
            // tag name -> iElement
            if (startTag.Find(KSpace) == KErrNotFound)
                {
                // case: no attributes/space in the start tag
                iElement.Set(startTag.Left(offset));
                if (iElement[iElement.Length() - 1] == '/')
                    {
                    iElement.Set(iElement.Left(iElement.Length() - 1));
                    }
                }
            else
                {
                // case: space after tag name or
                // case: at least one attribute in the start tag
                iElement.Set(startTag.Left(startTag.Find(KSpace)));
                }
            
            // set state
            iState = EStateStartTag;

            // Add /tag name to iPathBuf
            CheckSpaceL(iElement);  // Check that there is enough space in iPathBuf

            iPathBufPtr.Append(KSlash);  // '/'
            iPathBufPtr.Append(iElement);  // tag name

            // increment iDepth
            iDepth++;

            while ((eqpos = startTag.Find(KEquals)) != KErrNotFound)
                {
                TAttrData startTagAttr;
                // get attribute(s)
                spacepos = startTag.Find(KSpace);
                tokenLexer.Inc(spacepos);
                startTagAttr.iName.Set(startTag.Mid(spacepos + KSpace().Length(), 
                                        eqpos - spacepos - KEquals().Length()));
                // rest of the start tag and attributes
                startTag.Set(startTag.Right(startTag.Length() - eqpos - KEquals().Length()));
                tokenLexer.Inc(eqpos - spacepos + KEquals().Length());
                ch = tokenLexer.Get();
                if (ch == '\'' || ch == '\"')  // this should be ' or "
                    {
                    startTag.Set(startTag.Right(startTag.Length() - 1));
                    // if attribute ending ' or " not found return error
                    if ((attrValueLen = startTag.Locate(ch)) != KErrNotFound)
                        {
                        startTagAttr.iValue.Set(startTag.Left(attrValueLen));
                        iCAttrDataArray->AppendL(startTagAttr);
                        startTag.Set(startTag.Right(startTag.Length() - attrValueLen));
                        tokenLexer.Inc(attrValueLen);
                        }
                    else
                        {
                        iState = EStateError;
                        return ERcDocumentError;
                        }
                    }
                else
                    {
                    iState = EStateError;
                    return ERcDocumentError;
                    }
                }

            // check if tag is empty element
            iLexer.Inc(offset - KCloseBracket().Length());  // iLexer into the end of the tag - 1 position
            iLexer.Mark(iMarkPosition);

            if (iLexer.Get() == '/')  // next char is '>' for sure
                {
                iLexer.Inc(KCloseBracket().Length());
                iIsEmptyElement = ETrue;
                iLexer.Mark(iMarkPosition);
                }
            else if (iLexer.Get() == '>')
                {
                iIsEmptyElement = EFalse;
                iLexer.Mark(iMarkPosition);
                }
            else
                {
                iState = EStateError;
                return ERcDocumentError;
                }
            }
        else
            {
            iState = EStateError;
            return ERcDocumentError;
            }
        }
    return KErrNone;
    }

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////
TInt CXmlPullParser::GetElementText()
    {
    TPtrC8 elementText;
    TInt offset;
    TInt cdataStartOffset;
    TInt cdataEndOffset;

    iLexer.Mark(iMarkPosition);  // Remember where we are

    if (iLexer.Eos())
        {
        iState = EStateError;   // we can not be in the end of the doc when 
                                // we should have Start Tag 
        return ERcDocumentError;
        }
    else
        {
        elementText.Set(iLexer.Remainder());

        if (iCdataStartFound)
            {
            if ((cdataEndOffset = elementText.Find(KEndCData)) != KErrNotFound)
                {
                iCdataStartFound = EFalse;
                iCdataSectionText = ETrue;  // text inside CDATA section
                iText.Set(elementText.Left(cdataEndOffset));
                if (iText.Find(KStartCData) != KErrNotFound)
                    {
                    // Nested CDATA sections are not allowed
                    iState = EStateError;
                    return ERcDocumentError;
                    }
                iLexer.Inc(cdataEndOffset + KEndCData().Length());
                }
            else
                {
                // CDATA end not found -> error
                iState = EStateError;
                return ERcDocumentError;
                }
            }
        else
            {
            // Start of CDATA section '<![CDATA['
            if ((cdataStartOffset = elementText.Find(KStartCData)) == 0)
                {
                if ((cdataEndOffset = elementText.Find(KEndCData)) != KErrNotFound)
                    {
                    iCdataStartFound = EFalse;
                    iCdataSectionText = ETrue;  // text inside CDATA section
                    iText.Set(elementText.Mid(KStartCData().Length(), cdataEndOffset - KStartCData().Length()));
                    if (iText.Find(KStartCData) != KErrNotFound)
                        {
                        // Nested CDATA sections are not allowed
                        iState = EStateError;
                        return ERcDocumentError;
                        }
                    iLexer.Inc(cdataEndOffset + KEndCData().Length());
                    }
                else
                    {
                    // CDATA end not found -> error
                    iState = EStateError;
                    return ERcDocumentError;
                    }
                }
            else
                {
                offset = elementText.Find(KOpenBracket);  // any '<'
                
                if ((cdataStartOffset != KErrNotFound) && !(offset < cdataStartOffset))
                    {
                    iCdataStartFound = ETrue;
                    iCdataSectionText = EFalse;  // text before CDATA section
                    iText.Set(elementText.Left(cdataStartOffset)); 
                    iLexer.Inc(cdataStartOffset + KStartCData().Length());
                    }
                else
                    {
                    // No CDATA section found
                    // iCdataStartFound = EFalse; // this is EFalse any way
                    iCdataSectionText = EFalse;
                    if (offset != KErrNotFound)
                        {
                        iText.Set(elementText.Left(offset));
                        iLexer.Inc(offset);
                        }
                    else
                        {
                        iState = EStateError;
                        return ERcDocumentError;
                        }
                    }
                }
            }
        iState = EStateText;
        return KErrNone;
        }
    }

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////
TBool CXmlPullParser::CheckIfNextEndTag()
    {
    iLexer.Mark(iMarkPosition);

    if (iLexer.Eos())
        {
        iState = EStateError;   // we can not be in the end of the doc when 
                                // we should have Start/End Tag 
        return ETrue;  // the caller must check the iState!!
        }
    else
        {
        if (iLexer.Get() == '<' && iLexer.Get() == '/')
            {
            iLexer.UnGetToMark(iMarkPosition);
            return ETrue;
            }
        else
            {
            iLexer.UnGetToMark(iMarkPosition);
            return EFalse;
            }
        }
    }

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////
TBool CXmlPullParser::CheckIfNextStartTag()
    {
    iLexer.Mark(iMarkPosition);

    if (iLexer.Eos())
        {
        iState = EStateError;   // we can not be in the end of the doc when 
                                // we should have Start/End Tag 
        return ETrue;  // the caller must check the iState!!
        }
    else
        {
        if (iLexer.Get() == '<' && iLexer.Get().IsAlpha())
            {
            iLexer.UnGetToMark(iMarkPosition);
            return ETrue;
            }
        else
            {
            iLexer.UnGetToMark(iMarkPosition);
            return EFalse;
            }
        }
    }


////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////
TInt CXmlPullParser::GetElementEndTag()
    {
    TPtrC8 elementText;
    TInt offset;

    iLexer.Mark(iMarkPosition);  // Remember where we are

    if (iLexer.Eos())
        {
        iState = EStateError;   // we can not be in the end of the doc when 
                                // we should have End Tag 
        return ERcDocumentError;
        }
    else
        {
        if (iLexer.Get() == '<' && iLexer.Get() == '/')
            {
            elementText.Set(iLexer.Remainder());
            if ((offset = elementText.Find(KCloseBracket)) != KErrNotFound)
                {
                // End tag format: '</' Name S? '>'
                iElement.Set(elementText.Left(offset));
                if (iElement[iElement.Length() - 1] == ' ')
                    {
                    iElement.Set(iElement.Left(iElement.Length() - 1));
                    }

                // delete /end tag from the iPathBuf
                // this is done by subtracting the length of the buffer
                iPathBufPtr.SetLength(iPathBufPtr.LocateReverse('/'));

                // decrement iDepth
                iDepth--;

                iLexer.Inc(offset + KCloseBracket().Length());
                iState = EStateEndTag;
                return KErrNone;
                }
            else
                {
                iState = EStateError;
                return ERcDocumentError;
                }
            }
        else
            {
            iState = EStateError;
            return ERcDocumentError;
            }
        }
    }

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////
TBool CXmlPullParser::CheckIfEndOfFile()
    {
    iLexer.Mark(iMarkPosition);

    if ( (iDepth == 0) && (iPathBufPtr.Length() == 0) )
        {
        iState = EStateEndDocument;
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }

////////////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////////////
TInt CXmlPullParser::EscapeCharRefAndPreDefEntities()
    {
    TInt retVal;

    if (iStringValuePtr.Find(KAmpersandHash) != KErrNotFound)
        {
        retVal = CharacterReferencesEscaped();  // handle &#xaa; and &#aa;
        if ( retVal != KErrNone )
            {
            return retVal;
            }
        }
    
    if (iStringValuePtr.Find(KAmpersand) != KErrNotFound)
        {
        retVal = PredefinedEntitiesEscaped();  // handle &lt; &gt; &amp; &apos; and &quot;
        if ( retVal != KErrNone )
            {
            return retVal;
            }
        }

    return KErrNone;
    }

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////
TInt CXmlPullParser::CharacterReferencesEscaped()
    {
    TInt startPos, length;
    TInt retVal;
    TInt i = 0;
    TRadix radix = EDecimal;
    
    TLex8 tempLexer;
    TLexMark8 mark;
    TUint number;

    TPtrC8 tempPtr;
   
    while ( (startPos = iStringValuePtr.Find(KAmpersandHash)) != KErrNotFound)
        {
        tempPtr.Set(iStringValuePtr.Right(iStringValuePtr.Length() - startPos));
        length = tempPtr.Find(KSemicolon) - KAmpersandHash().Length();
        tempPtr.Set(iStringValuePtr.Mid(startPos + KAmpersandHash().Length(), length));
        tempLexer.Assign(tempPtr);
        if ( tempLexer.Peek() == 'x' )
            {
            tempLexer.Inc();  //increment to next character position
            tempLexer.Mark(mark);
            for ( i = 0; i < (length - 1); i++ )  // check that whole input is hexadecimal
                {
                if ( !(tempLexer.Get()).IsHexDigit() )
                    {
                    return ERcDocumentError;  // after &#x there must be hexadecimal digits (0-9, a-f, A-F).
                    }
                }
            tempLexer.UnGetToMark(mark);
            radix = EHex;
            }
        else 
            {
            tempLexer.Mark(mark);
            for ( i = 0; i < length; i++ )
                {
                if ( !(tempLexer.Get()).IsDigit() )
                    {
                    return ERcDocumentError; // after &# there must be standard decimal digits (0-9).
                    }
                }
            tempLexer.UnGetToMark(mark);
            radix = EDecimal;
            }

        if ( (retVal = tempLexer.Val(number, radix)) == KErrNone)
            {
            TChar ch(number);
            TBuf8<12> buf;
            buf.Append(ch);
            iStringValuePtr.Replace(startPos, length + KAmpersandHash().Length() + 
                KSemicolon().Length(), buf);
            }
        else
            {
            return retVal;
            }
        }
    return KErrNone;
    }

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////
TInt CXmlPullParser::PredefinedEntitiesEscaped()
    {
    TInt pos;
   
    while (iStringValuePtr.Find(KAmpersand) != KErrNotFound)
        {
        if ((pos = iStringValuePtr.Find(KEntityLowerThan)) != KErrNotFound)
            {
            iStringValuePtr.Replace(pos, KEntityLowerThan().Length(), KOpenBracket);
            }
        else if ((pos = iStringValuePtr.Find(KEntityGreaterThan)) != KErrNotFound)
            {
            iStringValuePtr.Replace(pos, KEntityGreaterThan().Length(), KCloseBracket);
            }
        else if ((pos = iStringValuePtr.Find(KEntityAmpersand)) != KErrNotFound)
            {
            iStringValuePtr.Replace(pos, KEntityAmpersand().Length(), KAmpersand);
            }
        else if ((pos = iStringValuePtr.Find(KEntityApostrophe)) != KErrNotFound)
            {
            iStringValuePtr.Replace(pos, KEntityApostrophe().Length(), KSingleQuote);
            }
        else if ((pos = iStringValuePtr.Find(KEntityQuotation)) != KErrNotFound)
            {
            iStringValuePtr.Replace(pos, KEntityQuotation().Length(), KDoubleQuote);
            }
        else
            {
            return KErrNone;
            }
        }
    return KErrNone;
    }
