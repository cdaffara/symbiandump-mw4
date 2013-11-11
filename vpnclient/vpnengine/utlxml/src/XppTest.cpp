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
* Description: XML pull parser tester.
*
*/


#include <e32base.h>
#include <e32cons.h>
#include <badesca.h>
#include "XppImpl.h"
#include "XwImpl.h"

_LIT(KTxtXppTest, "XppTest");
_LIT(KTxtTitle, "XML pull parser tester");
_LIT(KTxtFailed, "Failed: leave code = %d");
_LIT(KTxtOK, "OK");
_LIT(KTxtPressAnyKey, " [press any key]");

LOCAL_D CConsoleBase* console;

LOCAL_C void callExampleL();
LOCAL_C void doExampleL();
LOCAL_C void XmlParsingExampleL();
LOCAL_C void XmlWritingExampleL();

// The starting point
GLDEF_C TInt E32Main()
    {
    __UHEAP_MARK;
    
    CTrapCleanup* cleanup = CTrapCleanup::New();
    TRAPD(error, callExampleL());
    __ASSERT_ALWAYS(!error, User::Panic(KTxtXppTest, error));
    delete cleanup;
    
    __UHEAP_MARKEND;
    
    return 0;
    }

// Console initialization and the doExample call  
LOCAL_C void callExampleL()
    {
    console = Console::NewL(KTxtTitle, TSize(KConsFullScreen, KConsFullScreen));
    CleanupStack::PushL(console);
    
    TRAPD(error, doExampleL());
    
    if (error)
        console->Printf(KTxtFailed, error);
    else
        console->Printf(KTxtOK);
    
    console->Printf(KTxtPressAnyKey);
    console->Getch();
    
    CleanupStack::PopAndDestroy(); // console
    }

//----------------------------------------------------------------------------- 
// XML parsing example
//----------------------------------------------------------------------------- 
    
// The test XML document

_LIT8(KTestXML,
     "<?xml version=\"1.0\" encoding=\"ISO-8859-1\" ?> \
      <!--  This is a comment that should be skipped when parsing --> \
      <!--  This is the 2nd comment that should be skipped when parsing --> \
      <!DOCTYPE note [ \
          <!ELEMENT note (to, from, heading, body)> \
          <!ELEMENT to (#PCDATA)> \
          <!ELEMENT from (#PCDATA)> \
          <!ELEMENT heading (#PCDATA)> \
          <!ELEMENT body (#PCDATA)> \
      ]> \
      <Metadata xmlns='&#ab;attr1&#x5d; acu' attr2=\"second&#xerror;attribute\" > \
          <Name  ><![CDATA[<1st cdata section>]]>name &#x3f; 1</Name > \
          <Name>&#110;&#97;&#x6D;&#x65;&#x20;&#x32;<![CDATA[</2dn cdata section>&lt;&#x3e;]]></Name> \
          <Name>name 3 start<![CDATA[]</3rd cdata section>]]>name 3 end</Name > \
          <Type>a&lt;b&#x3C; c &gt;d&#x7b;e&amp;f&#35;g&apos;h&#x2F;i&quot;j&#x7f;</Type> \
          <Id>12345</Id> \
      </Metadata> \
      <!--  This is the 3rd comment -->");

// The second test XML document
_LIT8(KTestXML2,
      "<VPNPolicyPkg><File name='nokia_nic_vpn-sales-1_0.pin'>data1</File><File name='nokia_nic_vpn-sales-1_0.pol'>data2</File></VPNPolicyPkg>");

// 2nd Tag/attribute names
_LIT8(KTagVPNPolicyPkg, "/VPNPolicyPkg");
_LIT8(KTagFile, "/VPNPolicyPkg/File");
_LIT8(KAttrName, "name");

// Tag/attribute names
_LIT8(KTagMetadata, "/Metadata");
_LIT8(KTagName, "/Metadata/Name");
_LIT8(KTagType, "/Metadata/Type");
_LIT8(KTagId, "/Metadata/Id");
_LIT8(KAttrXmlns, "xmlns");
_LIT8(KAttrAttr2, "attr2");


_LIT8(KTestXML3, 
      "<sessions esgId=\"44\" lang=\"FI\" modified=\"3261983767\" expiryTime=\"3262005367\"/>");

// 3rd Tag/attribute names
_LIT8(KTagSessions, "/sessions");
_LIT8(KAttrEsgId, "esgId");
_LIT8(KAttrLang, "lang");
_LIT8(KAttrModified, "modified");
_LIT8(KAttrExpiryTime, "expiryTime");


// Outputting formatting
_LIT(KFormatName, "Name: %S\n");
_LIT(KPosition, "Position: %u\n");
_LIT(KStartPosition, "Element Start Position: %u\n");
_LIT(KLength, "Total length: %u\n");
_LIT(KFormatType, "Type: %S\n");
_LIT(KFormatId, "Id: %S\n");
_LIT(KFormatXmlns, "Id: %S\n");
_LIT(KFormatAttr2, "attr2: %S\n");
_LIT(KFormatAttrName, "Attr Name: %S\n");
_LIT(KFormatEndTag, "End tag: %S\n");
_LIT(KFormatEndDocumentEndTag, "End doc end tag: %S\n");
_LIT(KFormatPart, "Document part: %S\n");
_LIT(KFormatError, "Error\n");

// Output format for 3rd test
_LIT(KFormatAttrEsgId, "esgId: %S\n");
_LIT(KFormatAttrLang, "lang: %S\n");
_LIT(KFormatAttrModified, "modified: %S\n");
_LIT(KFormatAttrExpiryTime, "expiryTime: %S\n");


// The actual example
LOCAL_C void doExampleL()
    {
    // To make code scanner silent
    TRAP_IGNORE(XmlWritingExampleL());
    }

LOCAL_C void XmlParsingExampleL()
    {
    CDesC8ArrayFlat* stringValueArray = new (ELeave) CDesC8ArrayFlat(1);  // jakovist test
    // Create the parser
    CXmlPullParser* parser = CXmlPullParser::NewLC(*stringValueArray);
    //CXmlPullParser* parser = CXmlPullParser::NewLC();

    // Specify the XML document to parse
    parser->SetInput(KTestXML);
    //parser->SetInput(KTestXML2);

    // Pull all interesting data from the document...
    CXmlPullParser::TParserState state;

    do
        {
        User::LeaveIfError(parser->NextL());

        state = parser->State();

        TPtrC8 path;
        parser->PathL(path);

        TPtrC8 value;

        // needed in console print
        //TBuf<1024> copyBuf;
        TBuf<512> copyBuf;

//#if 0 //test
        // Get and show element texts
        if (state == CXmlPullParser::EStateText)
            {
            if (path.Compare(KTagName) == 0)
                {
                TInt retVal = parser->TextL(value);
                if (retVal == KErrNone)
                    {
                    copyBuf.Copy(value);
                    console->Printf(KFormatName, &copyBuf);
                    //console->Printf(KStartPosition, parser->CurrentElementPos());
                    //console->Printf(KPosition, parser->Pos());
                    }
                }
            if (path.Compare(KTagType) == 0)
                {
                TInt retVal = parser->TextL(value);
                if (retVal == KErrNone)
                    {
                    copyBuf.Copy(value);
                    console->Printf(KFormatType, &copyBuf);
                    //console->Printf(KStartPosition, parser->CurrentElementPos());
                    //console->Printf(KPosition, parser->Pos());
                    }
                retVal = parser->TextPredefEntitiesNotEscaped(value);
                if (retVal == KErrNone)
                    {
                    copyBuf.Copy(value);
                    console->Printf(KFormatType, &copyBuf);
                    }
                }
            if (path.Compare(KTagId) == 0)
                {
                parser->TextL(value);
                copyBuf.Copy(value);
                console->Printf(KFormatId, &copyBuf);
                //console->Printf(KStartPosition, parser->CurrentElementPos());
                //console->Printf(KPosition, parser->Pos());
                }
            }
//#endif  // if 0 

        // Get and shown an attribute value
        if (state == CXmlPullParser::EStateStartTag)
            {
            if (path.Compare(KTagMetadata) == 0)
                {
                TInt retVal = parser->AttributeValueL(KAttrXmlns, value);
                if (retVal == KErrNone)
                    {
                    copyBuf.Copy(value);
                    console->Printf(KFormatXmlns, &copyBuf);
                    //console->Printf(KStartPosition, parser->CurrentElementPos());
                    //console->Printf(KPosition, parser->Pos());
                    }

                retVal = parser->AttributeValuePredefEntitiesNotEscaped(KAttrXmlns, value);
                if (retVal == KErrNone)
                    {
                    copyBuf.Copy(value);
                    console->Printf(KFormatXmlns, &copyBuf);
                    }

                
                retVal = parser->AttributeValueL(KAttrAttr2, value);
                if (retVal == KErrNone)
                    {
                    copyBuf.Copy(value);
                    console->Printf(KFormatAttr2, &copyBuf);
                    }

                TInt attrCount;
                parser->AttributeCount(attrCount);
                retVal = parser->AttributeValueL((attrCount - 1), value);
                if (retVal == KErrNone)
                    {
                    copyBuf.Copy(value);
                    console->Printf(KFormatAttr2, &copyBuf);
                    }

/*
                // Unsuccessfull attribute name getting tests
                retVal = parser->AttributeName(-1, value); // negative index value
                if (retVal == KErrNone)
                    {
                    copyBuf.Copy(value);
                    console->Printf(KFormatAttrName, &copyBuf);
                    }
                else
                    {
                    console->Printf(KFormatError);
                    }

                retVal = parser->AttributeName(2, value); // index value too big
                if (retVal == KErrNone)
                    {
                    copyBuf.Copy(value);
                    console->Printf(KFormatAttrName, &copyBuf);
                    }
                else
                    {
                    console->Printf(KFormatError);
                    }

                // Unsuccessfull attribute getting tests
                retVal = parser->AttributeValueL(-1, value); // negative index value
                if (retVal == KErrNone)
                    {
                    copyBuf.Copy(value);
                    console->Printf(KFormatXmlns, &copyBuf);
                    }
                else
                    {
                    console->Printf(KFormatError);
                    }

                retVal = parser->AttributeValueL(2, value); // index value too big
                if (retVal == KErrNone)
                    {
                    copyBuf.Copy(value);
                    console->Printf(KFormatXmlns, &copyBuf);
                    }
                else
                    {
                    console->Printf(KFormatError);
                    }
*/
                }

            //Second test
            if (path.Compare(KTagFile) == 0)
                {
                TInt retVal = parser->AttributeValueL(KAttrName, value);
                if (retVal == KErrNone)
                    {
                    copyBuf.Copy(value);
                    console->Printf(KFormatXmlns, &copyBuf);
                    //console->Printf(KStartPosition, parser->CurrentElementPos());
                    //console->Printf(KPosition, parser->Pos());
                    }
                }
            }

        // Get and show a part of document being parsed

        if (state == CXmlPullParser::EStateEndDocument)
            {
            TPtrC8 docPart = parser->DocPart(30, 31);  // aStartPos and aEndPos just an example
            copyBuf.Copy(docPart);
            console->Printf(KFormatPart, &copyBuf);
            }

        // Get and show end tag value
/*        if (state == CXmlPullParser::EStateEndTag)
            {
            if (path.Compare(KTagMetadata) == 0)
                {
                TInt retVal = parser->Name(value);
                if (retVal == KErrNone)
                    {
                    copyBuf.Copy(value);
                    console->Printf(KFormatEndTag, &copyBuf);
                    console->Printf(KPosition, parser->Pos());
                    }
                }
            }
*/

        // Get and show end tag value
/*
        if (state == CXmlPullParser::EStateEndDocument)
            {
            TInt retVal = parser->Name(value);
            if (retVal == KErrNone)
                {
                copyBuf.Copy(value);
                console->Printf(KFormatEndDocumentEndTag, &copyBuf);
                console->Printf(KLength, parser->Length());
                }
            }
*/
        
        }while (state != CXmlPullParser::EStateEndDocument);

#if 0
    // this is for testing new feature: same parser instance can 
    // be used
    parser->SetInput(KTestXML2);

    // Pull all interesting data from the document...
    //CXmlPullParser::TParserState state;

    do
        {
        User::LeaveIfError(parser->NextL());

        state = parser->State();

        TPtrC8 path;
        parser->PathL(path);

        TPtrC8 value;

        // needed in console print
        TBuf<512> copyBuf;

        // Get and show element texts
        if (state == CXmlPullParser::EStateText)
            {
            if (path.Compare(KTagVPNPolicyPkg) == 0)
                {
                parser->TextL(value);
                copyBuf.Copy(value);
                console->Printf(KFormatName, &copyBuf);
                }
            if (path.Compare(KTagFile) == 0)
                {
                parser->TextL(value);
                copyBuf.Copy(value);
                console->Printf(KFormatType, &copyBuf);
                }
            }

        // Get and shown an attribute value
        if (state == CXmlPullParser::EStateStartTag)
            {
            if (path.Compare(KTagFile) == 0)
                {
                TInt retVal = parser->AttributeValueL(KAttrName, value);
                if (retVal == KErrNone)
                    {
                    copyBuf.Copy(value);
                    console->Printf(KFormatAttrName, &copyBuf);
                    }
                }
            }

        // Get and show a part of document being parsed
        if (state == CXmlPullParser::EStateEndDocument)
            {
            TPtrC8 docPart = parser->DocPart(0, 10);  // aStartPos and aEndPos just an example
            copyBuf.Copy(docPart);
            console->Printf(KFormatPart, &copyBuf);
            }
        } while (state != CXmlPullParser::EStateEndDocument);


    // Test number 3:

    parser->SetInput(KTestXML3);
    // Pull all interesting data from the document...
    //CXmlPullParser::TParserState state;

    do
        {
        User::LeaveIfError(parser->NextL());

        state = parser->State();

        TPtrC8 path;
        parser->PathL(path);

        TPtrC8 value;

        // needed in console print
        TBuf<512> copyBuf;

        // Get and shown an attribute value
        if (state == CXmlPullParser::EStateStartTag)
            {
            if (path.Compare(KTagSessions) == 0)
                {
                TInt retVal = parser->AttributeValueL(KAttrEsgId, value);
                if (retVal == KErrNone)
                    {
                    copyBuf.Copy(value);
                    console->Printf(KFormatAttrEsgId, &copyBuf);
                    }
                retVal = parser->AttributeValueL(KAttrLang, value);
                if (retVal == KErrNone)
                    {
                    copyBuf.Copy(value);
                    console->Printf(KFormatAttrLang, &copyBuf);
                    }
                retVal = parser->AttributeValueL(KAttrModified, value);
                if (retVal == KErrNone)
                    {
                    copyBuf.Copy(value);
                    console->Printf(KFormatAttrModified, &copyBuf);
                    }
                retVal = parser->AttributeValueL(KAttrExpiryTime, value);
                if (retVal == KErrNone)
                    {
                    copyBuf.Copy(value);
                    console->Printf(KFormatAttrExpiryTime, &copyBuf);
                    }
                }
            }

        // Get and show a part of document being parsed
        if (state == CXmlPullParser::EStateEndDocument)
            {
            TPtrC8 docPart = parser->DocPart(0, 10);  // aStartPos and aEndPos just an example
            copyBuf.Copy(docPart);
            console->Printf(KFormatPart, &copyBuf);
            }
        } while (state != CXmlPullParser::EStateEndDocument);
#endif  // if 0

    delete stringValueArray;
    CleanupStack::PopAndDestroy(); // parser
    }

//----------------------------------------------------------------------------- 
// XML writing example
//----------------------------------------------------------------------------- 

// Tag/attribute names and values
_LIT8(KVerNum, "1.0");
_LIT8(KTagRoot, "root");
_LIT8(KTagElement1, "element1");
_LIT8(KTagElement2, "element2");
_LIT8(KTagElement3, "element3");
_LIT8(KAttr1, "attr1");
_LIT8(KAttr2, "attr2");
_LIT8(KVal1, "val1<<a>b& \'1\' \"2\"");
_LIT8(KVal2, "val2");
_LIT8(KText, "text a<b x&z t>n \'g\' \"99\"&&><");
_LIT8(KGreeting, "<greeting>");
    
LOCAL_C void XmlWritingExampleL()
    {
    /*
     * Create a document like this (newlines
     * and indentation will no appear in the
     * actual output):
     *
     * <?xml version="1.0" encoding="ISO-8859-1"?>
     * <root>
     *     <element1>
     *         text a<b x&z t>n \'g\' \"99\"&&><
     *     </element1>
     *     <element2 attr1='val1&lt;&lt;b&amp; &apos;1&apos; &quot;2&quot;' attr2='val2'/>
     *     <element3 attr1='val1&lt;&lt;b&amp; &apos;1&apos; &quot;2&quot;'>
     *         text a&lt; x&amp;z t&gt;n &apos;g&apos; &quot;99&quot;&amp;&amp;&gt;&lt;<![CDATA[<geeting>]]>
     *     </element3>
     * </root>
     */

    // Create the writer
    CXmlWriter* writer = CXmlWriter::NewLC();

    // Write the prolog
    writer->AddPrologL(KVerNum);
    //writer->AddPrologL();

    // Write the root element start tag
    writer->AddStartTagL(KTagRoot);
    
    // Write an element with some text content
    writer->AddStartTagL(KTagElement1);
    writer->AddXmlTextL(KText);
    writer->AddEndTagL(KTagElement1);

    // Write an empty element with some attributes
    writer->OpenStartTagL(KTagElement2);
    writer->AddAttributeL(KAttr1, KVal1);
    writer->AddAttributeL(KAttr2, KVal2);
    writer->CloseStartTagL(ETrue);

    // Write an element with some attributes
    // and some text content
    writer->OpenStartTagL(KTagElement3);
    writer->AddAttributeL(KAttr1, KVal1);
    writer->CloseStartTagL(EFalse);
    writer->AddTextL(KText);
    writer->AddCdataSectL(KGreeting);
    writer->AddEndTagL(KTagElement3);
    
    // Write the root element end tag
    writer->AddEndTagL(KTagRoot);

    // Get the resulting XML document
    // and do something with it
    TPtrC8 doc = writer->DocPart(0, (writer->Length() - 1));


    /*
     * Create a new document using same writer object.
     * The new document looks like this (newlines
     * and indentation will no appear in the
     * actual output):
     *
     * <?xml version="1.0" encoding="ISO-8859-1"?>
     * <start>
     *     <ele1 attribute='value'>
     *         some text
     *     </ele1>
     *     <ele2 attr1='val1'/>
     * </start>
     */

_LIT8(KTagStart, "start");
_LIT8(KTagEle1, "ele1");
_LIT8(KTagAttribute, "attribute");
_LIT8(KTagValue, "value");
_LIT8(KSomeText, "some text");
_LIT8(KTagEle2, "ele2");

    // Reset writer
    writer->Reset();

    // Write the prolog
    writer->AddPrologL();

    // Write the root element start tag
    writer->AddStartTagL(KTagStart);

    // Write an element with attribute and some text content
    writer->AddStartTagL(KTagEle1);
    writer->AddAttributeL(KTagAttribute, KTagValue);
    writer->AddTextL(KSomeText);
    writer->AddEndTagL(KTagEle1);

    // Write an empty element with some attributes
    writer->OpenStartTagL(KTagEle2);
    writer->AddAttributeL(KAttr1, KVal1);
    writer->CloseStartTagL(ETrue);

    // Write the root element end tag
    writer->AddEndTagL(KTagStart);

    // Get the resulting XML document
    // and do something with it
    doc.Set(writer->DocPart(0, (writer->Length() - 1)));

    CleanupStack::PopAndDestroy(); // writer
    }
    