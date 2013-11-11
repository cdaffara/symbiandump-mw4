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

#include "parsehandlerbase.h"
CParseHandlerBase::~CParseHandlerBase()
	{
	delete iChildParser;
	iString.Close();
	}
	
void CParseHandlerBase::StartChildParserL(CParseHandlerBase* aChildParser, const RTagInfo& aElement, const RAttributeArray& aAttributes)
	{
	if(iChildParser)
		{
		User::Leave(KErrCorrupt);
		}
	else
		{
		delete iChildParser;
		iChildParser = aChildParser;
		iChildParser->ParseStartElementL(aElement, aAttributes);		
		}
	}

void CParseHandlerBase::OnContentL(const TDesC8& aBytes, TInt aErrorCode)
	{
	if(iChildParser)
		{
		iChildParser->OnContentL(aBytes, aErrorCode);
		}
	else
		{
		User::LeaveIfError(aErrorCode);
		ParseContentL(aBytes);
		}
	}
	
	
void CParseHandlerBase::OnStartElementL(const RTagInfo& aElement, const RAttributeArray& aAttributes, TInt aErrorCode)
	{
	if(iChildParser)
		{
		// If the child handler is present get it to parse the tags
		iChildParser->OnStartElementL(aElement, aAttributes, aErrorCode);
		}
	else
		{
		User::LeaveIfError(aErrorCode);
		ParseStartElementL(aElement, aAttributes);	
		}
	}
	
void CParseHandlerBase::OnEndElementL(const RTagInfo& aElement, TInt aErrorCode)
	{
	if(iChildParser)
		{
		iChildParser->OnEndElementL(aElement, aErrorCode);
		if(!iChildParser->SiblingCount())
			{
			// finished with child parser handler
			delete iChildParser;
			iChildParser = NULL;
			}
		}	
	else
		{
		// Overriden by derived classes to implement specific behavior for an end tag
		User::LeaveIfError(aErrorCode);
		ParseEndElementL(aElement);
		}
	}

void CParseHandlerBase::OnStartPrefixMappingL(const RString& aPrefix, const RString& aUri, TInt aErrorCode)
	{
	User::LeaveIfError(aErrorCode);
	if(iChildParser)
		{
		iChildParser->OnStartPrefixMappingL( aPrefix , aUri, aErrorCode );
		}
	else
		ParsePrefixL( aPrefix , aUri );
	}

TBool CParseHandlerBase::IsBlankSpace(const TDesC8& aBytes)
	{
	RBuf8 trimBuf;
	TBool retValue = EFalse;
	trimBuf.Create(aBytes);
	trimBuf.Trim();
	
	if(!trimBuf.Length())
		retValue = ETrue;
	trimBuf.Close();
	
	return retValue;
	}


