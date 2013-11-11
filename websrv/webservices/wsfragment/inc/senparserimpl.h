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
* Description:        This is the real, internal implementation of CSenParser
*
*/








#ifndef SEN_PARSERIMPL_H
#define SEN_PARSERIMPL_H

#include <SenParser.h>

class CSenParserImpl : public CSenParser
    {
    public: // Constructors and destructor

        static CSenParserImpl* NewL();
        static CSenParserImpl* NewLC();

        static CSenParserImpl* NewL(CParser* aParser);
        static CSenParserImpl* NewLC(CParser* aParser);

        static CSenParserImpl* NewL(const TDesC8& aParserMimeType);
        static CSenParserImpl* NewLC(const TDesC8& aParserMimeType);

        virtual ~CSenParserImpl();
        
        virtual void SetContentHandler(CSenFragmentBase& aContentHandler);

        virtual void ParseBeginL();
        virtual void ParseBeginL(const TDesC8& aDocumentMimeType);


        virtual void ParseL(const TDesC8& aFragment,
                                CSenFragmentBase& aContentHandler);
        virtual void ParseL(RFs& aFs, const TDesC& aFilename,
                                 CSenFragmentBase& aContentHandler);
        virtual void ParseL(RFile& aFile, CSenFragmentBase& aContentHandler);
                                
        virtual void ParseEndL();

        virtual void SetProcessorChainL(const RContentProcessorUids& aPlugins);

        virtual TInt EnableFeature(TInt aParserFeature);
        virtual TInt DisableFeature(TInt aParserFeature);
        virtual TBool IsFeatureEnabled(TInt aParserFeature) const;

        virtual void AddPreloadedDictionaryL(const TDesC8& aPublicId);

        virtual RStringPool& StringPool();
        virtual RStringDictionaryCollection& StringDictionaryCollection();

    private:
        CSenParserImpl(CParser* apParser);
        void ConstructL();
        void ConstructL(const TDesC8& aParserMimeType);

    private: // Data
        CSenContentHandler*     ipContentHandler;
        CParser*                ipParser;
    };

NONSHARABLE_CLASS(CSenContentHandler) : public CBase, public MContentHandler
    {
    public: // Constructors and destructor

        static CSenContentHandler* NewL();
        static CSenContentHandler* NewLC();

        static CSenContentHandler* NewL(MContentHandler& aContentHandler);
        static CSenContentHandler* NewLC(MContentHandler& aContentHandler);
        
        virtual ~CSenContentHandler();

        void SetContentHandler(MContentHandler& aContentHandler);

        MContentHandler& ContentHandler();

    public: // From MContentHandler

        virtual void OnStartDocumentL(const RDocumentParameters& aDocParam, TInt aErrorCode);

        virtual void OnEndDocumentL(TInt aErrorCode);


        virtual void OnStartElementL(const RTagInfo& aElement, const RAttributeArray& aAttributes,
                                 TInt aErrorCode);

        virtual void OnEndElementL(const RTagInfo& aElement, TInt aErrorCode);

        virtual void OnContentL(const TDesC8& aBytes, TInt aErrorCode);

        virtual void OnStartPrefixMappingL(const RString& aPrefix, const RString& aUri,
                                       TInt aErrorCode);

        virtual void OnEndPrefixMappingL(const RString& aPrefix, TInt aErrorCode);

        virtual void OnIgnorableWhiteSpaceL(const TDesC8& aBytes, TInt aErrorCode);

        virtual void OnSkippedEntityL(const RString& aName, TInt aErrorCode);

        virtual void OnProcessingInstructionL(const TDesC8& aTarget, const TDesC8& aData,
                                          TInt aErrorCode);

        /*
        * Symbian XML Framework errocodes:
        * [from XmlParserErrors.h]
        * EXmlParserError = -1000,
        * EXmlSyntax = 2+EXmlParserError,       // -998
        * EXmlNoElements,                       // -997
        * EXmlInvalidToken,                     // -996
        * EXmlUnclosedToken,                    // -995
        * EXmlPartialChar,                      // -994
        * EXmlTagMismatch,                      // -993
        * EXmlDuplicateAttribute,               // -992
        * EXmlJunkAfterDocElement,              // -991
        * EXmlPeRef,                            // -990
        * EXmlUndefinedEntity,                  // -989
        * EXmlRecursiveEntity,                  // -988
        * EXmlAsyncEntity,                      // -987
        * EXmlBadCharRef,                       // -986
        * EXmlBinaryEntityRef,                  // -985
        * EXmlAttributeExternalEntityRef,       // -984
        * EXmlMisplacedPi,                      // -983
        * EXmlUnknownEncoding,                  // -982
        * EXmlIncorrectEncoding,                // -981
        * EXmlUnclosedCdata,                    // -980
        * EXmlExternalEntityHandling,           // -979
        * EXmlNotStandalone,                    // -978
        * EXmlUnexpectedState,                  // -977
        * EXmlEntityDeclInPe,                   // -976
        * EXmlDtdRequired,                      // -975
        * EXmlFeatureLockedWhileParsing         // -974
        */
        virtual void OnError(TInt aErrorCode);

        virtual TAny* GetExtendedInterface(const TInt32 aUid);

    private:
        CSenContentHandler(MContentHandler* apContentHandler);

    private: // Data
        MContentHandler* ipContentHandler;
    };

#endif //SEN_PARSERIMPL_H

// End of File