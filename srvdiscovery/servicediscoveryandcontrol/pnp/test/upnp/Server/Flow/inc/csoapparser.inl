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


void CSoapParser::OnStartDocumentL(const Xml::RDocumentParameters& /*aDocParam*/, TInt aErrorCode)
	{
	User::LeaveIfError(aErrorCode);
	}

void CSoapParser::OnEndDocumentL(TInt aErrorCode)
	{
	User::LeaveIfError(aErrorCode);	
	}

void CSoapParser::OnStartPrefixMappingL(const RString& /*aPrefix*/, const RString& /*aUri*/, TInt aErrorCode)
	{
	User::LeaveIfError(aErrorCode);
	}

void CSoapParser::OnEndPrefixMappingL(const RString& /*aPrefix*/, TInt aErrorCode)
	{
	User::LeaveIfError(aErrorCode);
	}

void CSoapParser::OnIgnorableWhiteSpaceL(const TDesC8& /*aBytes*/, TInt aErrorCode)
	{
	User::LeaveIfError(aErrorCode);
	}

void CSoapParser::OnSkippedEntityL(const RString& /*aName*/, TInt aErrorCode)
	{	
	User::LeaveIfError(aErrorCode);	
	}

void CSoapParser::OnProcessingInstructionL(const TDesC8& /*aTarget*/, const TDesC8& /*aData*/, TInt aErrorCode)
	{
	User::LeaveIfError(aErrorCode);		
	}

void CSoapParser::OnError(TInt aErrorCode)	
	{
	iError = aErrorCode;
	}

TAny* CSoapParser::GetExtendedInterface(const TInt32 /*aUid*/)
	{
	return NULL;
	}
void CSoapParser::OnEndElementL(const RTagInfo& /*aElement*/, TInt aErrorCode)
	{
	User::LeaveIfError(aErrorCode);
	}

void CSoapParser::OnContentL(const TDesC8& /*aBytes*/, TInt aErrorCode)
	{
	User::LeaveIfError(aErrorCode);	
	}
