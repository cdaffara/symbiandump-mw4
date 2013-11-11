/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:        Class implements XML Parser capable of service multiple handlers
*
*/








// INCLUDE FILES
#include "senparserimpl.h"
#include "SenFragmentBase.h"

#include <xml/matchdata.h> // to select libxml2 sax parser
namespace
    {
#ifdef RD_SEN_FORCE_LIBXML_SAX_PLUGIN_MIMETYPE
    _LIT8(KXmlParserMimeType,   "text/wsstarlibxml2");
    _LIT8(KXmlVariant,          "wsstarlibxml2");
#else
    _LIT8(KXmlParserMimeType,   "text/xml"); // Expat on S60 3.0, and on S60 3.1, goes according to variant
    _LIT8(KXmlVariant,          "libxml2");  // LibXml2 SAX parser
#endif
    }

CSenParserImpl* CSenParserImpl::NewL()
    {
    CSenParserImpl* self = CSenParserImpl::NewLC();
    CleanupStack::Pop(self);
    return self;
    }

CSenParserImpl* CSenParserImpl::NewLC()
    {
    CSenParserImpl* self = new (ELeave) CSenParserImpl(NULL);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

CSenParserImpl* CSenParserImpl::NewL(CParser* apParser)
    {
    CSenParserImpl* self = CSenParserImpl::NewLC(apParser);
    CleanupStack::Pop(self);
    return self;
    }

CSenParserImpl* CSenParserImpl::NewLC(CParser* apParser)
    {
    CSenParserImpl* self = new (ELeave) CSenParserImpl(apParser);
    CleanupStack::PushL(self);
    return self;
    }

CSenParserImpl* CSenParserImpl::NewL(const TDesC8& aParserMimeType)
    {
    CSenParserImpl* self = CSenParserImpl::NewLC(aParserMimeType);
    CleanupStack::Pop(self);
    return self;
    }

CSenParserImpl* CSenParserImpl::NewLC(const TDesC8& aParserMimeType)
    {
    CSenParserImpl* self = new (ELeave) CSenParserImpl(NULL);
    CleanupStack::PushL(self);
    self->ConstructL(aParserMimeType);
    return self;
    }

CSenParserImpl::CSenParserImpl(CParser* apParser)
:   ipParser(apParser)
    {
    }


void CSenParserImpl::ConstructL()
    {
    CMatchData* pMatchData = CMatchData::NewL();
    CleanupStack::PushL(pMatchData);
    // Set the default variant type (libxml2)
    pMatchData->SetVariantL(KXmlVariant);
    pMatchData->SetMimeTypeL(KXmlParserMimeType);
    ipContentHandler = CSenContentHandler::NewL();
    ipParser = CParser::NewL(*pMatchData, *ipContentHandler);  // to select libxml2 sax parser

    CleanupStack::PopAndDestroy(pMatchData);
    }

void CSenParserImpl::ConstructL(const TDesC8& aParserMimeType)
    {
    ipContentHandler = CSenContentHandler::NewL();
    ipParser = CParser::NewL(aParserMimeType, *ipContentHandler); // Left to client to chose the appropriat parser
    }

CSenParserImpl::~CSenParserImpl()
    {
    delete ipContentHandler;
    delete ipParser;
    }
    
void CSenParserImpl::SetContentHandler(CSenFragmentBase& aContentHandler)
    {
    ipContentHandler->SetContentHandler(aContentHandler);
    }

void CSenParserImpl::ParseBeginL()
    {
    ipParser->ParseBeginL();
    }

void CSenParserImpl::ParseBeginL(const TDesC8& aDocumentMimeType)
    {
    ipParser->ParseBeginL(aDocumentMimeType);
    }

void CSenParserImpl::ParseL(const TDesC8& aFragment,
                                    CSenFragmentBase& aContentHandler)
    {
    ipContentHandler->SetContentHandler(aContentHandler);
    aContentHandler.SetParser(*this);
    
    ipParser->ParseL(aFragment);
    // Following enables Symbian XML framework to invoke OnErrorL() callback:
    ipParser->ParseEndL();
    }
    
void CSenParserImpl::ParseL(RFs& aFs, const TDesC& aFilename, CSenFragmentBase& aContentHandler)
    {
    ipContentHandler->SetContentHandler(aContentHandler);
    aContentHandler.SetParser(*this);
    Xml::ParseL( *ipParser, aFs, aFilename );
    }

void CSenParserImpl::ParseL(RFile& aFile, CSenFragmentBase& aContentHandler)
    {
    ipContentHandler->SetContentHandler(aContentHandler);
    aContentHandler.SetParser(*this);
    Xml::ParseL( *ipParser, aFile );
    }

void CSenParserImpl::ParseEndL()
    {
    ipParser->ParseEndL();
    }

void CSenParserImpl::SetProcessorChainL(const RContentProcessorUids& aPlugins)
    {
    ipParser->SetProcessorChainL(aPlugins);
    }

TInt CSenParserImpl::EnableFeature(TInt aParserFeature)
    {
    return ipParser->EnableFeature(aParserFeature);
    }

TInt CSenParserImpl::DisableFeature(TInt aParserFeature)
    {
    return ipParser->DisableFeature(aParserFeature);
    }

TBool CSenParserImpl::IsFeatureEnabled(TInt aParserFeature) const
    {
    return ipParser->IsFeatureEnabled(aParserFeature);
    }

void CSenParserImpl::AddPreloadedDictionaryL(const TDesC8& aPublicId)
    {
    ipParser->AddPreloadedDictionaryL(aPublicId);
    }

RStringPool& CSenParserImpl::StringPool()
    {
    return ipParser->StringPool();
    }

RStringDictionaryCollection& CSenParserImpl::StringDictionaryCollection()
    {
    return ipParser->StringDictionaryCollection();
    }

CSenContentHandler* CSenContentHandler::NewL(MContentHandler& aContentHandler)
    {
    CSenContentHandler* self = CSenContentHandler::NewLC(aContentHandler);
    CleanupStack::Pop(self);
    return self;
    }

CSenContentHandler* CSenContentHandler::NewLC(MContentHandler& aContentHandler)
    {
    CSenContentHandler* self = new (ELeave) CSenContentHandler(&aContentHandler);
    CleanupStack::PushL(self);
    return self;
    }

CSenContentHandler* CSenContentHandler::NewL()
    {
    CSenContentHandler* self = CSenContentHandler::NewLC();
    CleanupStack::Pop(self);
    return self;
    }

CSenContentHandler* CSenContentHandler::NewLC()
    {
    CSenContentHandler* self = new (ELeave) CSenContentHandler(NULL);
    CleanupStack::PushL(self);
    return self;
    }
    
CSenContentHandler::CSenContentHandler(MContentHandler* apContentHandler)
:   ipContentHandler(apContentHandler)
    {
    }

MContentHandler& CSenContentHandler::ContentHandler()
    {
    return *ipContentHandler;
    }
    
CSenContentHandler::~CSenContentHandler()
    {
    }

void CSenContentHandler::SetContentHandler(MContentHandler& aContentHandler)
    {
    ipContentHandler = &aContentHandler;
    }

void CSenContentHandler::OnStartDocumentL(const RDocumentParameters& aDocParam, TInt aErrorCode)
    {
    ipContentHandler->OnStartDocumentL(aDocParam, aErrorCode);
    }

void CSenContentHandler::OnEndDocumentL(TInt aErrorCode)
    {
    ipContentHandler->OnEndDocumentL(aErrorCode);
    }

void CSenContentHandler::OnStartElementL(const RTagInfo& aElement, const RAttributeArray& aAttributes,
                                 TInt aErrorCode)
    {
    ipContentHandler->OnStartElementL(aElement, aAttributes, aErrorCode);
    }

void CSenContentHandler::OnEndElementL(const RTagInfo& aElement, TInt aErrorCode)
    {
    ipContentHandler->OnEndElementL(aElement, aErrorCode);
    }

void CSenContentHandler::OnContentL(const TDesC8& aBytes, TInt aErrorCode)
    {
    ipContentHandler->OnContentL(aBytes, aErrorCode);
    }

void CSenContentHandler::OnStartPrefixMappingL(const RString& aPrefix, const RString& aUri,
                                       TInt aErrorCode)
    {
    ipContentHandler->OnStartPrefixMappingL(aPrefix, aUri, aErrorCode);
    }

void CSenContentHandler::OnEndPrefixMappingL(const RString& aPrefix, TInt aErrorCode)
    {
    ipContentHandler->OnEndPrefixMappingL(aPrefix, aErrorCode);
    }

void CSenContentHandler::OnIgnorableWhiteSpaceL(const TDesC8& aBytes, TInt aErrorCode)
    {
    ipContentHandler->OnIgnorableWhiteSpaceL(aBytes, aErrorCode);
    }

void CSenContentHandler::OnSkippedEntityL(const RString& aName, TInt aErrorCode)
    {
    ipContentHandler->OnSkippedEntityL(aName, aErrorCode);
    }

void CSenContentHandler::OnProcessingInstructionL(const TDesC8& aTarget, const TDesC8& aData,
                                          TInt aErrorCode)
    {
    ipContentHandler->OnProcessingInstructionL(aTarget, aData, aErrorCode);
    }

void CSenContentHandler::OnError(TInt aErrorCode)
    {
    ipContentHandler->OnError(aErrorCode);
    }

TAny* CSenContentHandler::GetExtendedInterface(const TInt32 aUid)
    {
    return ipContentHandler->GetExtendedInterface(aUid);
    }
    
// End of File



