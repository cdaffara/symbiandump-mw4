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
* Description: 
* Element attribute name and value storage class.
* A simple XML parser that implements
* the XML pull parsing interface.
*
*/


#ifndef __XPP_IMPL__
#define __XPP_IMPL__

#include <e32base.h>
#include <badesca.h>
#include "XppApi.h"

/**
 * Element attribute name and value storage class
 */
class TAttrData
    {
public:
    TPtrC8 iName;
    TPtrC8 iValue;
    };

/**
 * A simple XML parser that implements
 * the XML pull parsing interface.
 */
class CXmlPullParser : public CBase, public MXmlPullParser
    {
public: // Construction and destruction
    IMPORT_C static CXmlPullParser* NewL(CDesC8ArrayFlat& aStringValueArray);
    IMPORT_C static CXmlPullParser* NewLC(CDesC8ArrayFlat& aStringValueArray);
    IMPORT_C virtual ~CXmlPullParser();

private: // Construction
    CXmlPullParser();
    void ConstructL(CDesC8ArrayFlat& aStringValueArray);

public: // MXmlPullParser methods
    IMPORT_C void SetInput(const TDesC8& aInput);
    IMPORT_C TInt NextL();
    IMPORT_C CXmlPullParser::TParserState State();
    IMPORT_C TInt Name(TPtrC8& aName);
    IMPORT_C TInt PathL(TPtrC8& aPath);
    IMPORT_C TInt Depth(TInt& aDepth);
    IMPORT_C TInt AttributeCount(TInt& aCount);
    IMPORT_C TInt AttributeName(TInt aIndex, TPtrC8& aName);
    IMPORT_C TInt AttributeValuePredefEntitiesNotEscaped(TInt aIndex, TPtrC8& aValue);
    IMPORT_C TInt AttributeValuePredefEntitiesNotEscaped(const TDesC8& aName, TPtrC8& aValue);
    IMPORT_C TInt AttributeValueL(TInt aIndex, TPtrC8& aValue);
    IMPORT_C TInt AttributeValueL(const TDesC8& aName, TPtrC8& aValue);
    IMPORT_C TInt IsEmptyElement(TBool& aIsEmptyElement);
    IMPORT_C TInt TextPredefEntitiesNotEscaped(TPtrC8& aText);
    IMPORT_C TInt TextL(TPtrC8& aText);
    IMPORT_C TInt Pos();
    IMPORT_C TInt CurrentElementPos();
    IMPORT_C TInt Length();
    IMPORT_C TPtrC8 DocPart(TInt aStartPos, TInt aEndPos);

private:  // private methods
    void CheckSpaceL(const TDesC8& aText);
    TInt SkipLeadingNonRelevantElements();
    TInt SkipNonRelevantElements();
    TInt GetElementStartTagL();
    TInt GetElementText();
    TBool CheckIfNextEndTag();
    TBool CheckIfNextStartTag();
    TInt GetElementEndTag();
    TBool CheckIfEndOfFile();
    void CheckStringValueBufferSpaceL(const TDesC8& aValue);
    TInt EscapeCharRefAndPreDefEntities();
    TInt PredefinedEntitiesEscaped();
    TInt CharacterReferencesEscaped();

private:
    TParserState iState;
    //const TDesC8* iInput;
    TPtrC8 iText;  // element text

    CArrayFixFlat<TAttrData> *iCAttrDataArray;

    TPtrC8 iDocument;  // pointer to whole document to parse
    TLexMark8 iMarkPosition;
    TPtrC8 iDocPart;
    TLex8 iLexer;
    TInt iCurrentElementPos;

    HBufC8* iPathBuf;
    TPtr8 iPathBufPtr;
    
    TPtrC8 iElement;  // element name
    TInt iDepth;
    TBool iIsEmptyElement;

    TBool iCdataStartFound;
    TBool iCdataSectionText;

    HBufC8* iStringValue;  // used when entities are escaped
    TPtr8 iStringValuePtr;

    CDesC8ArrayFlat* iStringValueArray;

    };


#endif // __XPP_IMPL__
