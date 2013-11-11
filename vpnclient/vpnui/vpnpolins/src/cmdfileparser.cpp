/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Parses the XML command file (VPN command file)
*
*/



#include <e32std.h>

// XML parser includes
#include "XwImpl.h"
#include "XppImpl.h"

#include "cmdfilexmltags.h"
#include "cmdfileparser.h"
#include "policyinstaller_constants.h"
#include "log_r6.h"


CCmdFileParser::CCmdFileParser()
    {
    }

CCmdFileParser::~CCmdFileParser() 
    {
    LOG_("-> CCmdFileParser::~CCmdFileParser()");
    ReleaseResources();
    LOG_("<- CCmdFileParser::~CCmdFileParser()");
    }

void CCmdFileParser::ParseL(const TDesC8& aInputData)
    {
    LOG_("-> CCmdFileParser::ParseL()");
    // Release any allocated resources, for a clean parse
    ReleaseResources();

    iParserStack = new (ELeave) CArrayFixFlat<HBufC8*>(3);
    iParsedValuesArray = new (ELeave) CArrayFixFlat<HBufC8*>(3);

    // Parse XML and extract the parameters
    ParseXmlL(aInputData);

    iParseReady = ETrue;

    LOG_("<- CCmdFileParser::ParseL()");
    }

void CCmdFileParser::ReleaseResources() 
    {
    LOG_("-> CCmdFileParser::ReleaseResources()");

    iParseReady = EFalse;

    delete iFileContents;
    iFileContents = NULL;

    delete iPKCS12Pwd;
    iPKCS12Pwd = NULL;

    delete iFileName;
    iFileName = NULL;

    // Delete parser stack entries before deleting
    // the stack pointer
    if (iParserStack) 
        {
        LOG_(" Deleting items from parser stack");

        for (TInt i = iParserStack->Count() - 1; i >= 0; i--) 
            {
            LOG_1(" Deleting at %d", i);
            delete iParserStack->At(i);
            iParserStack->At(i) = NULL;
            }

        LOG_(" Reset parser");
        iParserStack->Reset();
        }

    delete iParserStack;
    iParserStack = NULL;

    // Delete parsed values entries before deleting
    // the array pointer
    if (iParsedValuesArray) 
        {
        LOG_(" Deleting items from parsed values");
        for (TInt i = iParsedValuesArray->Count() - 1; i >= 0; i--) 
            {
            LOG_1(" Deleting at %d", i);
            delete iParsedValuesArray->At(i);
            iParsedValuesArray->At(i) = NULL;
            }

        LOG_(" Reset parsed values");
        iParsedValuesArray->Reset();
        }

    delete iParsedValuesArray;
    iParsedValuesArray = NULL;

    LOG_("<- CCmdFileParser::ReleaseResources()");
    }

void CCmdFileParser::PushNodeL(const TDesC8& aName) 
    {
    LOG_("-> CCmdFileParser::PushNodeL()");

    HBufC8* node = aName.AllocLC();

    LOG8_1("   PUSH Node: '%S'", node);

    iParserStack->AppendL(node);
    CleanupStack::Pop(node);

    LOG_("<- CCmdFileParser::PushNodeL()");
    }

void CCmdFileParser::PopNode() 
    {
    LOG_("-> CCmdFileParser::PopNode()");

    // Just pop, unless we ought to stop
    TInt lastIdx = iParserStack->Count()-1;
    if (lastIdx >= 0) 
        {
        LOG8_1("   POP Node: '%S'", iParserStack->At(lastIdx));
        LOG_1("    Removing from parserstack index %d", lastIdx);

        delete iParserStack->At(lastIdx);
        iParserStack->Delete(lastIdx);
        }

    LOG_("<- CCmdFileParser::PopNode()");
    }

TBool CCmdFileParser::IsTopmostNode(const TDesC8& aName) const
    {
    LOG_("-> CCmdFileParser::IsTopmostNode()");
    LOG8_1("  CHECKING: '%S'", &aName);

    TBool ret(EFalse);
    if (iParserStack) 
        {
        TInt lastIdx = iParserStack->Count()-1;
        TPtrC8 top = *(iParserStack->At(lastIdx));

        LOG8_1("  FOUND: '%S'", &top);

        if (aName.Compare(top) == 0)
            {
            ret = ETrue;
            }
        }

    LOG_1("<- CCmdFileParser::IsTopmostNode() val: %d", ret);

    return ret;
    }

HBufC8* CCmdFileParser::ParseTreeUriL(const TDesC8& aText) const
    {
    LOG_("-> CCmdFileParser::ParseTreeUriL()");
    LOG8_1(" Input: '%S'", &aText);

    HBufC8* ret(NULL);
    TInt uriLen(aText.Length());
    TInt itemCount(0);
    if (iParserStack)
        {
        // First, with precise scientifical approach establish 
        // the amount of wide bytes required for storing the URI
        itemCount = iParserStack->Count();

        LOG_1(" Items in stack: %d", itemCount);

        for (TInt i = 0; i < itemCount; i++) 
            {
            uriLen += iParserStack->At(i)->Length();
            }

        // The number of "slash" ("/") characters required,
        // with utmost precision
        uriLen += itemCount;
        ret = HBufC8::NewL(uriLen);

        // And then store the URI to a new string.
        for (TInt i = 0; i < itemCount; i++) 
            {
            ret->Des().Append(*(iParserStack->At(i)));
            ret->Des().Append(KCmdUriSeparator);
            }
        ret->Des().Append(aText);
        }

    LOG_("<- CCmdFileParser::ParseTreeUriL()");
    return ret;
    }

TBool CCmdFileParser::IsValidText(const TDesC8& aText) const
    {
    LOG_("-> CCmdFileParser::IsValidText()");
    TBool ret(ETrue);

    // We don't support "CR/LF" characters in the
    // beginning of valid value names
    TInt idx = aText.Find(KCmdCr);
    if (idx >= 0 && idx < 2) 
        {
        ret = EFalse;
        }
    else 
        {
        idx = aText.Find(KCmdLf);
        if (idx >= 0 && idx < 2) 
            {
            ret = EFalse;
            }
        }

    LOG_1("<- CCmdFileParser::IsValidText() ret: %d", ret);
    return ret;
    }
    
void CCmdFileParser::ParseXmlL(const TDesC8& aInputData)
    {
    LOG_("-> CCmdFileParser::ParseXmlL()");

    LOG_(" Instantiate parser array");

    CDesC8ArrayFlat* parserValueArray = new (ELeave) CDesC8ArrayFlat(10);
    CleanupStack::PushL(parserValueArray);

    LOG_(" Instantiate XML parser");
    CXmlPullParser* parser = CXmlPullParser::NewLC(*parserValueArray);

    TPtrC8 name;
    TPtrC8 attribute;

    LOG_(" Set input");
    parser->SetInput(aInputData);

    LOG_(" Get length");
    TInt len = parser->Length();
    LOG_1(" XML length: %d bytes", len);

    // Check the first element
    LOG_(" Get the next (first) element");
    parser->NextL();

    if (parser->State() != CXmlPullParser::EStateStartTag) 
        {
        LOG_(" LEAVE: XML data is not in VPN's required format!");
        User::Leave(KErrCorrupt);
        }

    LOG_(" Starting the main parser loop");
    TPtrC8 text;

    // Main loop:
    // - While XML nodes still exist do:
    //   - If "opening" node (<start>), push to the stack
    //   - If "closing" node (</start>), pop from the stack
    //   - If a text node, make sure it's valid and then store
    //     the URI so the value can be fetched later on.
    while (parser->State() != CXmlPullParser::EStateEndDocument)
        {
        parser->Name(name);

        if (parser->State() == CXmlPullParser::EStateStartTag) 
            {
            LOG8_1(" START: '%S'", &name);
            PushNodeL(name);
            }
        else if (parser->State() == CXmlPullParser::EStateEndTag) 
            {
            LOG8_1(" END: '%S'", &name);
            if (IsTopmostNode(name)) 
                {
                PopNode();
                }
            else 
                {
                // Item on top of the stack wasn't what was
                // expected. Indicates corrupted XML.
                LOG_(" ERROR: Node end tag mismatch with expected");
                User::Leave(KErrCorrupt);
                }
            }
        else if (parser->State() == CXmlPullParser::EStateText)
            {
            parser->TextL(text);
            if (IsValidText(text)) 
                {
                LOG8_1(" Text: '%S'", &text);
                
                HBufC8* uri = ParseTreeUriL(text);

                LOG8_1(" URI: '%S'", uri);

                CleanupStack::PushL(uri);
                
                iParsedValuesArray->AppendL(uri);
                
                CleanupStack::Pop(uri);
                }
            }

        parser->NextL();
        }

    LOG_(" Popping");

    CleanupStack::PopAndDestroy(2, parserValueArray);

    LOG_("<- CCmdFileParser::ParseXmlL()");
    }


HBufC* CCmdFileParser::GetParsedValueStrL(const TDesC8& aUri) 
    {
    LOG_("-> CCmdFileParser::GetParsedValueStrL()");

    HBufC* ret(NULL);

    // Get the "leaf" value (i.e. attribute's value)
    // and alloc a new string for it
    TPtrC8 ptr = FindUri(aUri);
    if (ptr.Length() > 0)
        {
        LOG_(" Ptr length > 0");
        LOG8_1(" Ptr: '%S'", &ptr);
        ret = HBufC::NewL(ptr.Length());
        ret->Des().Copy(ptr);
        }
    else 
        {
        LOG_(" Ptr length < 1");
        }

    LOG_("<- CCmdFileParser::GetParsedValueStrL()");

    return ret;
    }

void CCmdFileParser::GetParsedValueIntL(const TDesC8& aUri, TInt& aVal) 
    {
    LOG_("-> CCmdFileParser::GetParsedValueIntL()");

    // Get the attribute's value as string...
    HBufC* str = GetParsedValueStrL(aUri);

    if (str)
        {
        LOG_(" Valid string");
        CleanupStack::PushL(str);

        // ... and convert it to an integer
        LOG_(" Converting string to int");
        TLex lexer(*str);
        TInt status = lexer.Val(aVal);

        if (status != KErrNone) 
            {
            LOG_1("<- CCmdFileParser::GetParsedValueIntL() LEAVE: %d", status);
            User::Leave(status);
            }

        CleanupStack::PopAndDestroy(str);
        }
    else 
        {
        aVal = KErrNotFound;
        }
    LOG_("<- CCmdFileParser::GetParsedValueIntL()");
    }

void CCmdFileParser::GetParsedValueBoolL(const TDesC8& aUri, TBool& aVal)
    {
    LOG_("-> CCmdFileParser::GetParsedValueBoolL()");

    // Get the attribute's value as string...
    HBufC* str = GetParsedValueStrL(aUri);

    if (str) 
        {
        CleanupStack::PushL(str);

        // ... and see whether it matches "true"
        // Note that this comparison is extremely case-sensitive
        if (str->Compare(KTrue) == 0) 
            {
            aVal = ETrue;
            }
        else 
            {
            aVal = EFalse;
            }

        CleanupStack::PopAndDestroy(str);
        }
    else 
        {
        aVal = EFalse;
        }

    LOG_("<- CCmdFileParser::GetParsedValueBoolL()");
    }

/**
 * Extracts the "leaf value" ie. attribute's value instance
 * from an uri.
 * E.g. returns "BEEFFACED" if the full URI was
 * root/node1/node2/attribute1/BEEFFACED
 * (Or in XML representation,
 * <node1>
 *   <node2>
 *     <attribute1>BEEFFACED</attribute1>
 *   </node2>
 * </node1>)
 *
 */
TPtrC8 CCmdFileParser::FindUri(const TDesC8& aUri) const
    {
    LOG_("-> CCmdFileParser::FindUri()");
    LOG8_1(" Searching URI '%S'", &aUri);

    TPtrC8 ret(KNullDesC8);

    if (iParsedValuesArray) 
        {
        TInt count = iParsedValuesArray->Count();
        for (TInt i = 0; i < count; i++) 
            {
            TPtrC8 fulluri = *(iParsedValuesArray->At(i));

            // Check if we find the requested URI from the
            // very beginning of the parsed value's URI
            if (fulluri.Find(aUri) == 0) 
                {
                // Extract the "remaining" string -- the value (assuming correct syntax)
                ret.Set(fulluri.Right(fulluri.Length() - aUri.Length()));
                }
            }
        }

    LOG_("<- CCmdFileParser::FindUri()");

    return ret;
    }


