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

void CParseHandlerBase::ParseStartElementL(const RTagInfo& /*aElement*/, const RAttributeArray& /*aAttributes*/)
	{
	// not expecting a start tag
	User::Leave(KErrCorrupt);
	}
	
void CParseHandlerBase::ParseEndElementL(const RTagInfo& /*aElement*/)
	{
	// not expecting an end tag
	User::Leave(KErrCorrupt);
	}
	
void CParseHandlerBase::ParseContentL(const TDesC8& /*aBytes*/)
	{
	// ignore content, probably just tabs and white space	
	}
void CParseHandlerBase::ParsePrefixL(const RString& /*aPrefix*/, const RString& /*aUri*/)
	{
	// not expecting anything here
	}

void CParseHandlerBase::OnStartDocumentL(const RDocumentParameters& /*aDocParam*/, TInt aErrorCode)
	{
	User::LeaveIfError(aErrorCode);
	}

void CParseHandlerBase::OnEndDocumentL(TInt aErrorCode)
	{
	User::LeaveIfError(aErrorCode);
	}

void CParseHandlerBase::OnEndPrefixMappingL(const RString& /*aPrefix*/, TInt aErrorCode)
	{
	User::LeaveIfError(aErrorCode);
	
	// Do nothing
	
	}

void CParseHandlerBase::OnIgnorableWhiteSpaceL(const TDesC8& /*aBytes*/, TInt aErrorCode)
	{
	User::LeaveIfError(aErrorCode);
	// Don't care about ignorable white space
	}

void CParseHandlerBase::OnSkippedEntityL(const RString& /*aName*/, TInt aErrorCode)
	{
	User::LeaveIfError(aErrorCode);
	}

void CParseHandlerBase::OnProcessingInstructionL(const TDesC8& /*aTarget*/, const TDesC8& /*aData*/, TInt aErrorCode)
	{
	User::LeaveIfError(aErrorCode);
	}

void CParseHandlerBase::OnError(TInt aErrorCode)
	{
	User::LeaveIfError( aErrorCode );
	}

TAny* CParseHandlerBase::GetExtendedInterface(const TInt32 /*aUid*/)
	{
	// we don't support any extended interfaces
	return NULL;
	}


