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

#ifndef __PARSEHANDLERBASE_H__
#define __PARSEHANDLERBASE_H__

#include <xml/contenthandler.h>
#include <xml/contentsource.h>
#include <xml/parserfeature.h>
#include <e32base.h>

using namespace Xml;
	
/** Provides an MContentHandler inteface that has a virtual destructor and implements
default behavior for parsers which can be overriden by derived parsers as required
*/
class CParseHandlerBase : public CBase, public MContentHandler
	{
public:
	virtual ~CParseHandlerBase();
	
	// Overridden as required by derived classes
	inline virtual void ParseStartElementL(const RTagInfo& aElement, const RAttributeArray& aAttributes);
	inline virtual void ParseEndElementL(const RTagInfo& aElement);
	inline virtual void ParseContentL(const TDesC8& aBytes);
	inline virtual void ParsePrefixL(const RString& aPrefix, const RString& aUri);

	// From MContentHandler
	inline virtual void OnStartDocumentL(const RDocumentParameters& aDocParam, TInt aErrorCode);
    inline virtual void OnEndDocumentL(TInt aErrorCode);
	virtual void OnStartElementL(const RTagInfo& aElement, const RAttributeArray& aAttributes, TInt aErrorCode);
	virtual void OnEndElementL(const RTagInfo& aElement, TInt aErrorCode);
	virtual void OnContentL(const TDesC8& aBytes, TInt aErrorCode);
	virtual void OnStartPrefixMappingL(const RString& aPrefix, const RString& aUri, TInt aErrorCode);
	inline virtual void OnEndPrefixMappingL(const RString& aPrefix, TInt aErrorCode);
	inline virtual void OnIgnorableWhiteSpaceL(const TDesC8& aBytes, TInt aErrorCode);
	inline virtual void OnSkippedEntityL(const RString& aName, TInt aErrorCode);
	inline virtual void OnProcessingInstructionL(const TDesC8& aTarget, const TDesC8& aData, TInt aErrorCode);
	inline virtual void OnError(TInt aErrorCode);
	inline virtual TAny* GetExtendedInterface(const TInt32 aUid);
	TBool IsBlankSpace(const TDesC8& aBytes);
protected:
	void StartChildParserL(CParseHandlerBase* aChildParser, const RTagInfo& aElement, const RAttributeArray& aAttributes);
		
private:
	inline TInt SiblingCount();
		
private:
	CParseHandlerBase* iChildParser; // delete when depth = 0
protected:	
		TInt iSiblingsCount;
		RString iString;
		};

TInt CParseHandlerBase::SiblingCount()
	{
	return iSiblingsCount;
	}

#include "parsehandlerbase.inl"
#endif
