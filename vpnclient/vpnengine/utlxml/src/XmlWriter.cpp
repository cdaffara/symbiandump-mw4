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
* Description: Implementation of CXmlWriter.
*
*/



#include <eikenv.h>
#include <e32std.h>
#include <e32des8.h>
#include "XwImpl.h"

const TInt KExtraPunctuation = 13;  // CDATA section has 12 extra characters
const TInt KBufSizeIncrement = 1024;
const TInt KBufInitSize = 2048;

//
// Common literal text
//
_LIT8(KPrologStart, "<?xml");
_LIT8(KPrologEnd, "?>");
_LIT8(KVersion, "version");
_LIT8(K10, "1.0");
_LIT8(KEncoding, "encoding");
_LIT8(KIso88591, "ISO-8859-1");
_LIT8(KOpenBracket, "<");
_LIT8(KCloseBracket, ">");
_LIT8(KStartEndTag, "</");
_LIT8(KCloseTag, "/>");
_LIT8(KSpace, " ");
_LIT8(KEquals, "=");
_LIT8(KSingleQuote, "\'");
//_LIT8(KDoubleQuote, "\"");
_LIT8(KCDStart, "<![CDATA[");
_LIT8(KCDEnd, "]]>");

_LIT8(KEntityLowerThan, "&lt;");           // <
_LIT8(KEntityGreaterThan, "&gt;");         // >
_LIT8(KEntityAmpersand, "&amp;");          // &
_LIT8(KEntityApostrophe, "&apos;");        // '
_LIT8(KEntityQuotation, "&quot;");         // "


////////////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////////////

CXmlWriter::CXmlWriter() 
: iBufferPtr(NULL, 0), iTempBufferPtr(NULL, 0)
    {
    }


////////////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////////////
// NewLC with two stage construction 
EXPORT_C CXmlWriter* CXmlWriter::NewLC()
    {
    // get new, leave if can't
    CXmlWriter* self = new (ELeave) CXmlWriter();
    // push onto cleanup stack in case self->ConstructL leaves 
    CleanupStack::PushL(self);
    // complete construction with second phase constructor 
    self->ConstructL();
    return self;
    }

////////////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////////////
EXPORT_C CXmlWriter* CXmlWriter::NewL() 
    {
    CXmlWriter* self=NewLC();
    CleanupStack::Pop();
    return self;
    }

////////////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////////////
void CXmlWriter::ConstructL() 
    {
    iBuffer = HBufC8::NewMaxL(KBufInitSize);
    iBufferPtr.Set(iBuffer->Des());
    iBufferPtr.SetLength(0);  // Otherwise the Length is set to KBufInitSize
    iTempBuffer = HBufC8::NewMaxL(KBufInitSize);
    iTempBufferPtr.Set(iTempBuffer->Des());
    iTempBufferPtr.SetLength(0);  // Otherwise the Length is set to KBufInitSize
    }

////////////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////////////
CXmlWriter::~CXmlWriter()
    {
    delete (iBuffer);
    delete (iTempBuffer);
    }

////////////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////////////
void CXmlWriter::CheckSpaceL(const TInt aSpace)
    {
    // Make sure that we have enough space for the new text
    TInt spaceLeft = iBufferPtr.MaxLength() - iBufferPtr.Length()
                    - KExtraPunctuation;
    
    if (aSpace > spaceLeft)
        {
        // Allocate enough space for the new text + some additional 
        // free space so that allocations are not too frequent
        
        TInt newMaxLength = iBufferPtr.MaxLength() + aSpace + KBufSizeIncrement;
        
        iBuffer = iBuffer->ReAllocL(newMaxLength);
        iBufferPtr.Set(iBuffer->Des());
        }
    }

////////////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////////////
void CXmlWriter::CheckSpaceL(const TDesC8& aText)
    {
    // Make sure that we have enough space for the new text
    TInt spaceLeft = iBufferPtr.MaxLength() - iBufferPtr.Length()
                    - KExtraPunctuation;
    
    if (aText.Length() > spaceLeft)
        {
        // Allocate enough space for the new text + some additional 
        // free space so that allocations are not too frequent

        TInt newMaxLength = iBufferPtr.MaxLength() + aText.Length() 
                        + KBufSizeIncrement;

        iBuffer = iBuffer->ReAllocL(newMaxLength);
        iBufferPtr.Set(iBuffer->Des());
        }
    }

////////////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////////////
void CXmlWriter::CheckSpaceL(const TDesC8& aText1, const TDesC8& aText2)
    {
    // Make sure that we have enough space for the new text
    TInt spaceLeft = iBufferPtr.MaxLength() - iBufferPtr.Length()
                    - KExtraPunctuation;
    
    if ((aText1.Length() + aText2.Length()) > spaceLeft)
        {
        // Allocate enough space for the new text + some additional 
        // free space so that allocations are not too frequent
        
        TInt newMaxLength = iBufferPtr.MaxLength() + aText1.Length() 
                            + aText2.Length() + KBufSizeIncrement;
        
        iBuffer = iBuffer->ReAllocL(newMaxLength);
        iBufferPtr.Set(iBuffer->Des());
        }
    }

////////////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////////////
void CXmlWriter::CheckTempBufSpaceL(const TInt aSpace)
    {
    // Make sure that we have enough space for the new text
    TInt spaceLeft = iTempBufferPtr.MaxLength() - iTempBufferPtr.Length();
    
    if (aSpace > spaceLeft)
        {
        // Allocate enough space for the new text + some additional 
        // free space so that allocations are not too frequent
        
        TInt newMaxLength = iTempBufferPtr.MaxLength() + aSpace + KBufSizeIncrement;
        
        iTempBuffer = iTempBuffer->ReAllocL(newMaxLength);
        iTempBufferPtr.Set(iTempBuffer->Des());
        }
    }


////////////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////////////
void CXmlWriter::InsertEntitiesL()
    {
    TInt i = 0;
    
    while ( i < iTempBufferPtr.Length() )
        {
        if ( iTempBufferPtr[i] == '<' )
            {
            CheckTempBufSpaceL(KEntityLowerThan().Length());
            iTempBufferPtr.Replace(i, 1, KEntityLowerThan);
            i += KEntityLowerThan().Length();
            }
        else if ( iTempBufferPtr[i] == '>' )
            {
            CheckTempBufSpaceL(KEntityGreaterThan().Length());
            iTempBufferPtr.Replace(i, 1, KEntityGreaterThan);
            i += KEntityGreaterThan().Length();
            }
        else if ( iTempBufferPtr[i] == '&' )
            {
            CheckTempBufSpaceL(KEntityAmpersand().Length());
            iTempBufferPtr.Replace(i, 1, KEntityAmpersand);
            i += KEntityAmpersand().Length();
            }
        else if ( iTempBufferPtr[i] == '\'' )
            {
            CheckTempBufSpaceL(KEntityApostrophe().Length());
            iTempBufferPtr.Replace(i, 1, KEntityApostrophe);
            i += KEntityApostrophe().Length();
            }
        else if ( iTempBufferPtr[i] == '\"' )
            {
            CheckTempBufSpaceL(KEntityQuotation().Length());
            iTempBufferPtr.Replace(i, 1, KEntityQuotation);
            i += KEntityQuotation().Length();
            }
        else
            {
            i++;
            }
        }
    }

////////////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////////////
EXPORT_C void CXmlWriter::AddPrologL()
    {
    CheckSpaceL(KPrologStart().Length() + 2*KSpace().Length() + KVersion().Length() + 
               2*KEquals().Length() + 4*KSingleQuote().Length() + K10().Length() +
               KEncoding().Length() + KIso88591().Length() + KPrologEnd().Length());

    iBufferPtr.Append(KPrologStart);  // <?xml
    iBufferPtr.Append(KSpace);
    iBufferPtr.Append(KVersion);  // version
    iBufferPtr.Append(KEquals);   // =
    iBufferPtr.Append(KSingleQuote);   // '
    iBufferPtr.Append(K10);  // 1.0
    iBufferPtr.Append(KSingleQuote);   // '
    iBufferPtr.Append(KSpace);
    iBufferPtr.Append(KEncoding);  // encoding
    iBufferPtr.Append(KEquals);  // =
    iBufferPtr.Append(KSingleQuote);  // '
    iBufferPtr.Append(KIso88591);  // ISO-8859-1
    iBufferPtr.Append(KSingleQuote);  // '
    iBufferPtr.Append(KPrologEnd);  // ?>
    }


////////////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////////////
EXPORT_C void CXmlWriter::AddPrologL(const TDesC8& aVersionNum)
    {
    CheckSpaceL(KPrologStart().Length() + 2*KSpace().Length() + KVersion().Length() + 
               2*KEquals().Length() + 4*KSingleQuote().Length() + aVersionNum.Length() +
               KEncoding().Length() + KIso88591().Length() + KPrologEnd().Length());

    iBufferPtr.Append(KPrologStart);  // <?xml
    iBufferPtr.Append(KSpace);
    iBufferPtr.Append(KVersion);  // version
    iBufferPtr.Append(KEquals);   // =
    iBufferPtr.Append(KSingleQuote);   // '
    iBufferPtr.Append(aVersionNum);
    iBufferPtr.Append(KSingleQuote);   // '
    iBufferPtr.Append(KSpace);
    iBufferPtr.Append(KEncoding);  // encoding
    iBufferPtr.Append(KEquals);  // =
    iBufferPtr.Append(KSingleQuote);  // '
    iBufferPtr.Append(KIso88591);  // ISO-8859-1
    iBufferPtr.Append(KSingleQuote);  // '
    iBufferPtr.Append(KPrologEnd);  // ?>
    }

////////////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////////////
EXPORT_C void CXmlWriter::AddStartTagL(const TDesC8& aName)
    {
    CheckSpaceL(aName);

    iBufferPtr.Append(KOpenBracket);  // <
    iBufferPtr.Append(aName);
    iBufferPtr.Append(KCloseBracket);  // >
    }

////////////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////////////
EXPORT_C void CXmlWriter::OpenStartTagL(const TDesC8& aName)
    {
    CheckSpaceL(aName);

    iBufferPtr.Append(KOpenBracket);  // <
    iBufferPtr.Append(aName);
    }

////////////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////////////
EXPORT_C void CXmlWriter::AddAttributeL(const TDesC8& aName, const TDesC8& aValue)
    {
    CheckTempBufSpaceL(aName.Length() + aValue.Length());

    iTempBufferPtr = aValue;

    InsertEntitiesL();

    CheckSpaceL(aName, iTempBufferPtr);

    iBufferPtr.Append(KSpace);  // 
    iBufferPtr.Append(aName);
    iBufferPtr.Append(KEquals);  // =
    iBufferPtr.Append(KSingleQuote);  // '
    iBufferPtr.Append(iTempBufferPtr);
    iBufferPtr.Append(KSingleQuote);  // '
    }

////////////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////////////
EXPORT_C void CXmlWriter::CloseStartTagL(TBool aEndElement)
    {
    CheckSpaceL(KCloseTag);
    
    if(aEndElement)
        {
        iBufferPtr.Append(KCloseTag);  // />
        }
    else
        {
        iBufferPtr.Append(KCloseBracket);
        }
    }

////////////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////////////
EXPORT_C void CXmlWriter::AddTextL(const TDesC8& aText)
    {
    CheckTempBufSpaceL(aText.Length());

    iTempBufferPtr = aText;

    InsertEntitiesL();

    CheckSpaceL(iTempBufferPtr);

    iBufferPtr.Append(iTempBufferPtr);
    }

////////////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////////////
EXPORT_C void CXmlWriter::AddXmlTextL(const TDesC8& aText)
    {
    CheckSpaceL(aText);

    iBufferPtr.Append(aText);
    }

////////////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////////////
EXPORT_C void CXmlWriter::AddEndTagL(const TDesC8& aName)
    {
    CheckSpaceL(aName);
    
    iBufferPtr.Append(KStartEndTag);  // </
    iBufferPtr.Append(aName);
    iBufferPtr.Append(KCloseBracket);  // >
    }

////////////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////////////
EXPORT_C void CXmlWriter::AddCdataSectL(const TDesC8& aText)
    {
    CheckSpaceL(aText);
    
    iBufferPtr.Append(KCDStart);  // <![CDATA[
    iBufferPtr.Append(aText);
    iBufferPtr.Append(KCDEnd);  // ]]>
    }

////////////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////////////
EXPORT_C void CXmlWriter::InsertTextL(TInt aPos, const TDesC8& aText)
    {
    iTempBufferPtr = aText;

    InsertEntitiesL();

    CheckSpaceL(iTempBufferPtr);

    iBufferPtr.Insert(aPos, iTempBufferPtr);  // aPos is an offset; a zero value 
                                              // refers to the leftmost data 
                                              // position
    }

////////////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////////////
EXPORT_C void CXmlWriter::InsertXmlTextL(TInt aPos, const TDesC8& aText)
    {
    CheckSpaceL(aText);
    
    iBufferPtr.Insert(aPos, aText);  // aPos is an offset; a zero value 
                                     // refers to the leftmost data 
                                     // position
    }

////////////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////////////
EXPORT_C TInt CXmlWriter::Length()
    {
    return iBufferPtr.Length();
    }

////////////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////////////
EXPORT_C TPtrC8 CXmlWriter::DocPart(TInt aStartPos, TInt aEndPos)
    {
    iDoc.Set(iBufferPtr.Mid(aStartPos, (aEndPos - aStartPos + 1)));
    return iDoc;
    }

////////////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////////////
EXPORT_C void CXmlWriter::Reset()
    {
    iBufferPtr.Zero();
    iDoc.Set(KNullDesC8);
    }

////////////////////////////////////////////////////////////////////////////////////////
// Dll entry point
////////////////////////////////////////////////////////////////////////////////////////
