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
#include <xml/attribute.h> // From syslibs, needed for RAttributeArray

#include <SenXmlReader.h>
#include <SenXmlConstants.h>
#include <SenXmlUtils.h>
#include <MSenContentHandlerClient.h>
#include "senxmldebug.h"

#ifdef EKA2
    #ifndef __SERIES60_30__
        // S60 3.1 or newer
        #include <xml/matchdata.h>
    #endif
#else
    // Use fss with S60 2nd edition FP3 (2.8) - temp file is needed with expat
    #include <s32file.h> // filestream
#endif

// DEBUG:
#include "sendebug.h"

using namespace Xml;

namespace
    {
    const TInt KArraySize = 128;
    
    typedef struct
        {
        CSenXmlReader*  ipReader;
        TInt            iEnabledFeature;
        } TReaderData;
    }
    
_LIT8(KDefaultMimeType, "");  // Expat is made default, so don't have to give mime type

EXPORT_C CSenXmlReader* CSenXmlReader::NewL()
    {
    CSenXmlReader* pNew = NewLC();
    CleanupStack::Pop();
    return pNew;
    }

EXPORT_C CSenXmlReader* CSenXmlReader::NewLC()
    {
    
    CSenXmlReader* pNew = new (ELeave) CSenXmlReader(KSenDefaultParserFeature);
    CleanupStack::PushL(pNew);
    
    pNew->ConstructL(KDefaultMimeType);// KXmlParserMimeType : Defaulted to Expat
    return pNew;
    }

EXPORT_C CSenXmlReader* CSenXmlReader::NewL(TInt aParserFeature)
    {
    CSenXmlReader* pNew = NewLC(aParserFeature);
    CleanupStack::Pop();
    return pNew;
    }

EXPORT_C CSenXmlReader* CSenXmlReader::NewLC(TInt aParserFeature)
    {
    CSenXmlReader* pNew = new (ELeave) CSenXmlReader(aParserFeature);
    CleanupStack::PushL(pNew);
    pNew->ConstructL(KDefaultMimeType);// KXmlParserMimeType : Defaulted to Expat
    if(aParserFeature<0)
        {
        User::Leave(KErrArgument);
        }
    return pNew;
    }

EXPORT_C CSenXmlReader* CSenXmlReader::NewL(const TDesC8& aParserMimeType)
    {
    CSenXmlReader* pNew = NewLC(aParserMimeType);
    CleanupStack::Pop();
    return pNew;
    }

EXPORT_C CSenXmlReader* CSenXmlReader::NewLC(const TDesC8& aParserMimeType)
    {
    CSenXmlReader* pNew = new (ELeave) CSenXmlReader(KSenDefaultParserFeature);
    CleanupStack::PushL(pNew);
    pNew->ConstructL(aParserMimeType);
    return pNew;
    }

EXPORT_C CSenXmlReader* CSenXmlReader::NewL(const TDesC8& aParserMimeType,
                                            TInt aParserFeature)
    {
    CSenXmlReader* pNew = NewLC(aParserMimeType, aParserFeature);
    CleanupStack::Pop();
    return pNew;
    }

EXPORT_C CSenXmlReader* CSenXmlReader::NewLC(const TDesC8& aParserMimeType,
                                             TInt aParserFeature)
    {
    CSenXmlReader* pNew = new (ELeave) CSenXmlReader(aParserFeature);
    CleanupStack::PushL(pNew);
    pNew->ConstructL(aParserMimeType);
    if(aParserFeature<0)
        {
        User::Leave(KErrArgument);
        }
    return pNew;
    }

EXPORT_C CSenXmlReader::CSenXmlReader(TInt aEnabledFeature)
:
iParser(NULL),
iContentHandler(NULL),
ipNsPrefixes(NULL),
ipNsUris(NULL),
iEnabledFeature(aEnabledFeature)
    {
    }

EXPORT_C CSenXmlReader::~CSenXmlReader()
    {
    delete iParser;
    delete ipNsPrefixes;
    delete ipNsUris;
    iStringPool.Close();
    }

#if defined( __SERIES60_30__ )
void CSenXmlReader::ConstructL(const TDesC8& aParserMimeType)
    {
    if(aParserMimeType.Length()==0)
        {
        // Use default MIME type
        iParser = CParser::NewL(KXmlParserMimeType, *this);
        }
    else
        {
        // Pass aParserMimeType
        iParser = CParser::NewL(aParserMimeType, *this);
        }
    iParser->EnableFeature(iEnabledFeature);
    iStringPool.OpenL();
    }
#else
void CSenXmlReader::ConstructL(const TDesC8& aParserMimeType)
    {
    CMatchData* pMatchData = CMatchData::NewL();
    CleanupStack::PushL(pMatchData);
    
    if(aParserMimeType.Length()==0)
        {
        // Use default MIME type and variant
        // for this parser instance (text/xml)
        pMatchData->SetMimeTypeL(KXmlParserMimeType);
        }
    else
        {
        // Use specified aParserMimeType:
        pMatchData->SetMimeTypeL(aParserMimeType);
	    
	    // Set the default variant type (libxml2)
		pMatchData->SetVariantL(KXmlVariant);             
        }
    
    iParser = CParser::NewL(*pMatchData, *this);
    CleanupStack::PopAndDestroy(pMatchData);

    iParser->EnableFeature(iEnabledFeature);
    iStringPool.OpenL();
    }
#endif

EXPORT_C void CSenXmlReader::SetContentHandler(
                                    MSenContentHandlerClient& aContentHandler)
    {
    iContentHandler = &aContentHandler;
    }

#ifdef EKA2 
// In S60 3rd edition platform there is a fixed version of expat parser which
// can parse XML documents which size is 2048 bytes or greater. The previous 
// versions crashed with larger documents.
EXPORT_C void CSenXmlReader::ParseL(const TDesC8& aBuff)
    {
    CleanUp();

#if defined(__SERIES60_30__)
    RecreateParserL();
#endif // __SERIES_30__ defined

    // Note(!): Store the currently enabled feature, since the same member
    // is used to carry a possible error code from OnErrorL() callback:
    TInt feature(iEnabledFeature);

    TReaderData* pData = new (ELeave) TReaderData;
    pData->ipReader         = this;
    pData->iEnabledFeature  = iEnabledFeature;
    CleanupStack::PushL( TCleanupItem( CSenXmlReader::CleanupParser, pData ) );
    // Parse the XML document:
    iParser->ParseL(aBuff);
    CleanupStack::Pop();
    delete pData;

    iParser->ParseEndL(); // Enables Symbian XML framework errors to propagete OnErrorL() callback

    CleanUp();

    // Check if iEnabledFeature member was used to carry an error from OnErrorL() callback..
    if (iEnabledFeature < 0)
        {
        TInt error(iEnabledFeature);
        SENDEBUG((_L("CSenXmlReader::ParserL: leaving (%d)"), error));
        // Switch back the originally enabled feature
        iEnabledFeature = feature;
        User::Leave(error);
        }
    iContentHandler->EndDocument();
    }
#else 
EXPORT_C void CSenXmlReader::ParseL(const TDesC8& aBuff)
    {
    // In S60 2nd edition FP3 (2.8) platform a temp file has to be used, since the
    // underlying expat parser would crash when parsing a document which consists 
    // of more that 2048 bytes of XML.

    if (aBuff.Length() > 2048) // there is 2048 bytes limitation in expat parser!
        {
        SENDEBUG_L("CSenXmlReader::ParseL(): parsing over 2048 bytes of XML (EKA1)");
        SENDEBUG((_L("- document size: %d bytes."), aBuff.Length() ));

        // Parse large XML documents using file server
        RFs fss;
        User::LeaveIfError(fss.Connect());
        CleanupClosePushL(fss);

        // Now create a new temp file in the specified path using unique 
        // file name which is generated by the file server. Since file 
        // server is not responsible to delete this file, it must be done 
        // after parsing is finished.

        // Path for temp file
        _LIT(KFilePath, "c:\\");

        // Stores the temp file name when fileOutStream.Temp() returns:
        TFileName tempFilename; 
    
        // Try to generate new temporary file, leave if it failes:
        RFileWriteStream fileOutStream;
        User::LeaveIfError(fileOutStream.Temp(fss, KFilePath, tempFilename, EFileWrite));
        CleanupClosePushL(fileOutStream);
        SENDEBUG((_L("CSenXmlReader::ParseL(): used temp file name: '%S'"), &tempFilename));

        // Write XML document into the file:
        fileOutStream.WriteL(aBuff);

        // fileOutStream.Close(). Must be done prior ParseL()!
        CleanupStack::PopAndDestroy(); 

        // Parse the file:
        ParseL(fss, tempFilename);

        // Now delete the temporary file (when it is not in locked in parser's use)
        fss.Delete(tempFilename);

        // Finally close the file server session
        CleanupStack::PopAndDestroy(); // fss.Close()
        SENDEBUG_L("CSenXmlReader::ParseL() successfully parsed > 2048 bytes of XML.");
        }
    else
        {
        // Smaller documents may be parsed normally, even with older versions
        // of Symbian XML framework's expat parser.
        
        CleanUp();
        
#if defined(__SERIES60_30__)
    RecreateParserL();
#endif // __SERIES_30__ defined

        TInt feature(iEnabledFeature);

        TReaderData* pData = new (ELeave) TReaderData;
        pData->ipReader         = this;
        pData->iEnabledFeature  = iEnabledFeature;
        CleanupStack::PushL( TCleanupItem( CSenXmlReader::CleanupParser, pData ) );
        // Parse the XML document:
        iParser->ParseL(aBuff);
        CleanupStack::Pop();
        delete pData;
        
        iParser->ParseEndL(); // Enables Symbian XML framework errors to propagete OnErrorL() callback

        CleanUp();

        if (iEnabledFeature < 0) 
            {
            TInt error(iEnabledFeature);
            iEnabledFeature = feature;
            SENDEBUG((_L("CSenXmlReader::ParserL: leaving (%d)"), iEnabledFeature));
            User::Leave(error);
            }
            
        iContentHandler->EndDocument();
        }
    }
#endif // not EKA2 


EXPORT_C void CSenXmlReader::ParseL(RFs &aRFs, const TDesC& aFileToParse)
    {
    CleanUp();

#if defined(__SERIES60_30__)
    RecreateParserL();
#endif // __SERIES_30__ defined
    
    TInt feature(iEnabledFeature);

    TReaderData* pData = new (ELeave) TReaderData;
    pData->ipReader         = this;
    pData->iEnabledFeature  = iEnabledFeature;
    CleanupStack::PushL( TCleanupItem( CSenXmlReader::CleanupParser, pData ) );
    // Parse the XML document:
    Xml::ParseL(*iParser, aRFs, aFileToParse); 
    // Note: Xml::ParseL calls ParseL() and ParseEndL() internally:
    CleanupStack::Pop();
    delete pData;
    
    CleanUp();

    if(iEnabledFeature < 0) 
        {
        TInt leaveError = iEnabledFeature;
        iEnabledFeature = feature;
        User::Leave(leaveError);
        }
        
    iContentHandler->EndDocument();
    }

// protected helper
void CSenXmlReader::RecreateParserL()
    {
    delete iParser;
    iParser = NULL;

    delete ipNsPrefixes;
    ipNsPrefixes = NULL;
    delete ipNsUris;
    ipNsUris = NULL;

#if defined(__SERIES60_30__)
    // Use default MIME type
    iParser = CParser::NewL(KXmlParserMimeType, *this);
#else
    CMatchData* pMatchData = CMatchData::NewL();
    CleanupStack::PushL(pMatchData);
    
    pMatchData->SetMimeTypeL(KXmlParserMimeType);
    pMatchData->SetVariantL(KXmlVariant);

    iParser = CParser::NewL(*pMatchData, *this);
    CleanupStack::PopAndDestroy(pMatchData);
#endif

    iParser->EnableFeature(iEnabledFeature);
    }

void CSenXmlReader::OnStartDocumentL(
                                const RDocumentParameters& /* aDocParam */,
                                TInt /* aErrorCode */)
    {
    if(!iContentHandler)
        {
        SENDEBUG_L("OnStartDocumentL: KErrSenXmlContentHandlerNotSet");
        User::Leave(KErrSenXmlContentHandlerNotSet);
        }
    iContentHandler->StartDocument();
    }

void CSenXmlReader::OnEndDocumentL(TInt /* aErrorCode */)
    {
    if(!iContentHandler)
        {
        SENDEBUG_L("OnEndDocumentL: KErrSenXmlContentHandlerNotSet");
        User::Leave(KErrSenXmlContentHandlerNotSet);
        }
    iContentHandler->EndDocument();
    }


void CSenXmlReader::OnStartElementL(const RTagInfo& aElement,
                             const RAttributeArray& aAttributes,
                             TInt /* aErrorCode */)
    {
    if(!iContentHandler)
        {
        SENDEBUG_L("OnStartElementL: KErrSenXmlContentHandlerNotSet");
        User::Leave(KErrSenXmlContentHandlerNotSet);
        }


    const TPtrC8 localName = aElement.LocalName().DesC();
    const TPtrC8 nsUri = aElement.Uri().DesC();
    const TPtrC8 prefix = aElement.Prefix().DesC();

    TPtrC8 qualifiedName = localName;

    if (prefix != KNullDesC8)
        {
        HBufC8* pQName = HBufC8::NewLC(prefix.Length()+localName.Length()+
                                        KSenColon().Length());
        TPtr8 qName = pQName->Des();
        qName.Append(prefix);
        qName.Append(KSenColon);
        qName.Append(localName);
        qualifiedName.Set(qName);
        }

    if(ipNsPrefixes)
        {
        // there are namespaces to declare!

        // make a new array for all attributes including namespace (to be added)
        RAttributeArray attributesAndNamespaces;

        CleanupClosePushL(attributesAndNamespaces);
        TInt nsDeclarationCount(ipNsPrefixes->Count());
        for(TInt i=0; i<nsDeclarationCount; i++)
            {
            // open and take ownership of RString - xmlnsAttrPrefix
            RAttribute nsAttribute;
            //CleanupClosePushL(nsAttribute);

            TPtrC8 nsPrefix = ipNsPrefixes->MdcaPoint(i);
            TPtrC8 nsURI =  ipNsUris->MdcaPoint(i);

            if (nsPrefix != KNullDesC8)
                {
                nsAttribute.Open(iStringPool.OpenStringL(nsURI), 
                                iStringPool.OpenStringL(KSenXmlns()),
                                iStringPool.OpenStringL(nsPrefix),
                                iStringPool.OpenStringL(nsURI) );

                }
            else
                {
                nsAttribute.Open(iStringPool.OpenStringL(nsURI),
                            iStringPool.OpenStringL(KNullDesC8()),
                            iStringPool.OpenStringL(KSenXmlns()),
                            iStringPool.OpenStringL(nsURI) );

                }   


            // append the namespace attribute (declaration)
            CleanupClosePushL(nsAttribute);
            attributesAndNamespaces.AppendL(nsAttribute);
            CleanupStack::Pop(); // nsAttribute
            }

        // the ns declarations have been done using NON-CANONIZING method
        delete ipNsPrefixes;
        ipNsPrefixes = NULL;
        delete ipNsUris;
        ipNsUris = NULL;



        // append all other ("real") attributes
        TInt count(aAttributes.Count());
        for(TInt a=0; a<count; a++)
            {
            attributesAndNamespaces.AppendL(const_cast <RAttribute&> (aAttributes[a]).Copy());
            }


        // now give the stream content forward to the interested handler object.
        // we have successfully added the namespace declaration as NON-canonized(!)
        // attribute (if conditions have been met).
        iContentHandler->StartElement(nsUri, localName, qualifiedName, attributesAndNamespaces);

        // close the copied attributes previously added into this array as copies
        count = attributesAndNamespaces.Count();
        for(TInt j=0; j<count; j++)
            {
            attributesAndNamespaces[j].Close();
            }
        // close the actual array
        CleanupStack::PopAndDestroy(); // attributesAndNamespaces.Close();
        }
    else
        {
        // give the original attributes to content handler (no new namespaces declared in attrs)
        iContentHandler->StartElement(nsUri, localName, qualifiedName, aAttributes);
        }
    

    // delete qualified element name, if one was allocated
    if (prefix != KNullDesC8)
        {
        CleanupStack::PopAndDestroy(); // pQName
        }


    }



void CSenXmlReader::OnEndElementL(const RTagInfo& aElement, TInt /* aErrorCode */)
    {
    if(!iContentHandler)
        {
        SENDEBUG_L("OnEndElementL: KErrSenXmlContentHandlerNotSet");
        User::Leave(KErrSenXmlContentHandlerNotSet);
        }


    const TPtrC8 localName = aElement.LocalName().DesC();
    const TPtrC8 nsUri = aElement.Uri().DesC();
    const TPtrC8 prefix = aElement.Prefix().DesC();

    TPtrC8 qualifiedName = localName;

    if (prefix != KNullDesC8)
        {
        HBufC8* pQName = HBufC8::NewLC(prefix.Length()+localName.Length()+
                                        KSenColon().Length());
        TPtr8 qName = pQName->Des();
        qName.Append(prefix);
        qName.Append(KSenColon);
        qName.Append(localName);
        qualifiedName.Set(qName);
        }


    iContentHandler->EndElement(nsUri,
                                localName,
                                qualifiedName);

    if (prefix != KNullDesC8)
        {
        CleanupStack::PopAndDestroy(); // pQName
        }

    }


void CSenXmlReader::OnContentL(const TDesC8& aBytes, TInt /* aErrorCode */)
    {
    if(!iContentHandler)
        {
        SENDEBUG_L("OnContentL: KErrSenXmlContentHandlerNotSet");
        User::Leave(KErrSenXmlContentHandlerNotSet);
        }
    iContentHandler->Characters(aBytes,0,aBytes.Length());
    }


void CSenXmlReader::OnStartPrefixMappingL(
                                   const RString& aPrefix,
                                   const RString& aUri,
                                   TInt /*aErrorCode*/)
    {
    if(!iContentHandler)
        {
        SENDEBUG_L("OnStartPrefixMappingL: KErrSenXmlContentHandlerNotSet");
        User::Leave(KErrSenXmlContentHandlerNotSet);
        }

    if(!ipNsPrefixes)
        {
        ipNsPrefixes = new (ELeave) CDesC8ArrayFlat(KArraySize);
        }

    if(!ipNsUris)
        {
        ipNsUris = new (ELeave) CDesC8ArrayFlat(KArraySize);
        }

    ipNsPrefixes->AppendL(aPrefix.DesC());
    ipNsUris->AppendL(aUri.DesC());

    iContentHandler->StartPrefixMappingL(aPrefix.DesC(), aUri.DesC());
    }



void CSenXmlReader::OnEndPrefixMappingL(const RString& aPrefix,
                                        TInt /*aErrorCode*/)
    {
    if(!iContentHandler)
        {
        SENDEBUG_L("OnEndPrefixMappingL: KErrSenXmlContentHandlerNotSet");
        User::Leave(KErrSenXmlContentHandlerNotSet);
        }
    iContentHandler->EndPrefixMappingL(aPrefix.DesC());
    }


void CSenXmlReader::OnIgnorableWhiteSpaceL(const TDesC8& aBytes,
                                           TInt /*aErrorCode*/)
    {
    if(!iContentHandler)
        {
        SENDEBUG_L("OnIgnorableWhiteSpaceL: KErrSenXmlContentHandlerNotSet");
        User::Leave(KErrSenXmlContentHandlerNotSet);
        }
    iContentHandler->OnIgnorableWhiteSpaceL(aBytes);
    }


void CSenXmlReader::OnSkippedEntityL(const RString& aName, TInt /*aErrorCode*/)
    {
    if(!iContentHandler)
        {
        SENDEBUG_L("OnSkippedEntityL: KErrSenXmlContentHandlerNotSet");
        User::Leave(KErrSenXmlContentHandlerNotSet);
        }
    iContentHandler->SkippedEntity(aName.DesC());

    //TInt retVal = iContentHandler->SkippedEntity(aName.DesC());
    // content handler spesific code returned error
    //User::LeaveIfError(retVal); 
    }


void CSenXmlReader::OnProcessingInstructionL(
                                      const TDesC8& aTarget,
                                      const TDesC8& aData,
                                      TInt /*aErrorCode */)
    {
    if(!iContentHandler)
        {
        SENDEBUG_L("OnProcessingInstructionL: KErrSenXmlContentHandlerNotSet");
        User::Leave(KErrSenXmlContentHandlerNotSet);
        }
    iContentHandler->ProcessingInstructions(aTarget, aData);
    }

// Note: Symbian XML framework error codes are listed in XmlFrameworkErrors.h
void CSenXmlReader::OnError(TInt aErrorCode)
    {
#ifdef _SENDEBUG
    // Symbian XML framework signals about some error:
    SENDEBUG_L("CSenXmlReader::OnError");
    SENDEBUG_L(" -Symbian XML framework signalled an error: ");
    TBuf<32> buf;
    buf.AppendNum(aErrorCode);
    SENDEBUG((buf));
#endif    
 
     if(!iContentHandler)
        {
        SENDEBUG_L("OnError: KErrSenXmlContentHandlerNotSet");
        // Cannot report any signalled error to content handler,
        // since it has not been set. Force ParseL to leave by 
        // setting spesific error code (KErrSenXmlContentHandlerNotSet)
        iEnabledFeature = KErrSenXmlContentHandlerNotSet;
        return;
        }
    TInt retVal(iContentHandler->Error(aErrorCode));

#ifdef _SENDEBUG
    // Symbian XML framework signals about some error:
    SENDEBUG_L(" -Error() callback to content handler returned an error:");
    TBuf<32> buf2;
    buf2.AppendNum(retVal);
    SENDEBUG((buf2));
#endif    
    retVal = 0; // not used in release builds

    // In 3.0, iEnabledFeature member was used to indicate ParseL
    // that it should leave(!). 
    iEnabledFeature = aErrorCode;
    }


TAny* CSenXmlReader::GetExtendedInterface(const TInt32 aUid)
    {
    if(!iContentHandler)
        {
        SENDEBUG_L("GetExtendedInterface: KErrSenXmlContentHandlerNotSet");
        return NULL;
        }
    return iContentHandler->GetExtendedInterface(aUid);
    }


EXPORT_C TInt CSenXmlReader::EnabledParserFeature()
    {
    return iEnabledFeature;
    }


TInt CSenXmlReader::SetParserFeature(TInt aParserFeature)
    {
    if(iParser)
        {
        iEnabledFeature = aParserFeature;
        TInt retCode = iParser->EnableFeature(aParserFeature);
        return retCode;
        }
    else
        {
        // internal error: iParser should always be available!
        TInt leaveCode(KErrNone);
        // try to re-instantiate the parser - once
        TRAP(leaveCode, RecreateParserL());
        if(leaveCode!=KErrNone)
            {
            return leaveCode;
            }
        iEnabledFeature = aParserFeature;
        TInt retCode2 = iParser->EnableFeature(aParserFeature);
        return retCode2;
        }
    }

// DEPRECATED
EXPORT_C TInt CSenXmlReader::ParserFeature()
    {
    return EnabledParserFeature();
    }
    
void CSenXmlReader::CleanUp()
    {
    delete ipNsPrefixes;
    ipNsPrefixes = NULL;
    delete ipNsUris;
    ipNsUris = NULL;
#if defined(__SERIES60_30__)
    delete iParser;
    iParser = NULL;
#endif // __SERIES_30__ defined
    }

void CSenXmlReader::CleanupParser(TAny* apReaderData)
	{
	TReaderData* pData = REINTERPRET_CAST(TReaderData*, apReaderData);
	pData->ipReader->SetParserFeature(pData->iEnabledFeature);
	delete pData;
	}
    
// END OF FILE


