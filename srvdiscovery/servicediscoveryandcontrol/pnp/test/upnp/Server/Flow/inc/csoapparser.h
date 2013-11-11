// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#ifndef __CSOAPPARSER_H_
#define __CSOAPPARSER_H_
#include <xml/contenthandler.h>
#include <xml/parser.h>
#include <f32file.h>

using namespace Xml;


class CSoapParser: public CBase, public Xml::MContentHandler
    {
public:
	static CSoapParser* NewL ();

    ~CSoapParser();
    
	const TDesC8& SoapAction () const;	
    TBool ParseSoapL(const TDesC8& aBuffer);
private:
	CSoapParser();
	void ConstructL ();
	
    // From MContentHandler
    inline void OnStartDocumentL(const Xml::RDocumentParameters& aDocParam, TInt aErrorCode);
    inline void OnEndDocumentL(TInt aErrorCode);
    void OnStartElementL(const Xml::RTagInfo& aElement, const Xml::RAttributeArray& aAttributes, TInt aErrorCode);
    inline void OnEndElementL(const Xml::RTagInfo& aElement, TInt aErrorCode);
    inline void OnContentL(const TDesC8& aBytes, TInt aErrorCode);
    inline void OnStartPrefixMappingL(const RString& aPrefix, const RString& aUri, TInt aErrorCode);
    inline void OnEndPrefixMappingL(const RString& aPrefix, TInt aErrorCode);
    inline void OnIgnorableWhiteSpaceL(const TDesC8& aBytes, TInt aErrorCode);
    inline void OnSkippedEntityL(const RString& aName, TInt aErrorCode) ;
    inline void OnProcessingInstructionL(const TDesC8& aTarget, const TDesC8& aData, TInt aErrorCode);
    inline void OnError(TInt aErrorCode);
    inline TAny* GetExtendedInterface(const TInt32 aUid);
   
private:
	RBuf8 		iSoapAction;
	CParser* 	iParser;
	TBool 		iInBodySec;
	TInt 		iError;
    };
#include "csoapparser.inl"
#endif /*CSOAPPARSER_H_*/
