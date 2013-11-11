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
* A simple XML writer that implements
* the XML writer interface.
*
*/


#ifndef __XW_IMPL__
#define __XW_IMPL__

#include <e32base.h>
#include "XwApi.h"

/**
 * A simple XML writer that implements
 * the XML writer interface.
 */
class CXmlWriter : public CBase, public MXmlWriter
    {
public: // Construction and destruction
    IMPORT_C static CXmlWriter* NewL();
    IMPORT_C static CXmlWriter* NewLC();
    IMPORT_C virtual ~CXmlWriter();

private: // Construction
    CXmlWriter();
    void ConstructL();

public: // MXmlWriter methods
    IMPORT_C void AddPrologL();
    IMPORT_C void AddPrologL(const TDesC8& aVersionNum);
    IMPORT_C void AddStartTagL(const TDesC8& aName);
    IMPORT_C void OpenStartTagL(const TDesC8& aName);
    IMPORT_C void AddAttributeL(const TDesC8& aName, const TDesC8& aValue);
    IMPORT_C void CloseStartTagL(TBool aEndElement);
    IMPORT_C void AddTextL(const TDesC8& aText);
    IMPORT_C void AddXmlTextL(const TDesC8& aText);
    IMPORT_C void AddEndTagL(const TDesC8& aName);
    IMPORT_C void AddCdataSectL(const TDesC8& aText);
    IMPORT_C void InsertTextL(TInt aPos, const TDesC8& aText);
    IMPORT_C void InsertXmlTextL(TInt aPos, const TDesC8& aText);
    IMPORT_C TInt Length();
    IMPORT_C TPtrC8 DocPart(TInt aStartPos, TInt aEndPos);
    IMPORT_C void Reset();

private:
    void CheckSpaceL(const TInt aSpace);
    void CheckSpaceL(const TDesC8& aText);
    void CheckSpaceL(const TDesC8& aText1, const TDesC8& aText2);
    void CheckTempBufSpaceL(const TInt aSpace);
    void InsertEntitiesL();

private:
    TPtrC8 iDoc;

    HBufC8* iBuffer;
    TPtr8 iBufferPtr;

    HBufC8* iTempBuffer;
    TPtr8 iTempBufferPtr;
    };

#endif // __XPP_IMPL__
