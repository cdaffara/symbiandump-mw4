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

#include "csoapparser.h"
#include "upnpserverconstants.h"
CSoapParser* CSoapParser::NewL ()
	{
	CSoapParser* self = new (ELeave) CSoapParser;
	CleanupStack::PushL (self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CSoapParser::CSoapParser() 
	{
	}
	
CSoapParser::~CSoapParser() 
	{
	delete iParser;
	iSoapAction.Close();
	}

void CSoapParser::ConstructL ()
	{
	iParser = CParser::NewL (KXmlMimeType(), *this);
	iParser->ParseBeginL();	
	}

void CSoapParser::OnStartElementL(const RTagInfo& aElement, const RAttributeArray& /*aAttributes*/, TInt aErrorCode)
	{	
	User::LeaveIfError(aErrorCode);
	if (aElement.LocalName().DesC().Compare(KBody) == 0)
		{
		iInBodySec = ETrue;	
		return;		
		}
	if (iInBodySec && iSoapAction.Length() == 0)
		{
		const TChar KHash = '#';
		const TDesC8& localName = aElement.LocalName().DesC();
		const TDesC8& uri = aElement.Uri().DesC();	

        iSoapAction.CreateL(uri.Length() + localName.Length() + 1); // 1 for #
        iSoapAction.Append(uri);
        iSoapAction.Append(KHash);
        iSoapAction.Append(localName);
		}	
	}

const TDesC8& CSoapParser::SoapAction () const
	{
	return iSoapAction;	
	}

TBool CSoapParser::ParseSoapL(const TDesC8& aBuffer)
	{
	const TInt KXmlInputSize = 1024;
	TInt totalLength = aBuffer.Length();
	TInt consumedBytes = 0;
	while(totalLength && iSoapAction.Length() == 0)	
		{
		TInt len = (totalLength < KXmlInputSize) ? totalLength : KXmlInputSize;
		TPtrC8 inputBuffer(aBuffer.Ptr() + consumedBytes, len);
		iParser->ParseL(inputBuffer);
		totalLength -= len;
		consumedBytes += len;
		}
	
	if ( iError != KErrNone )
		{
		iParser->ParseEndL();
		User::Leave(iError);
		}
	
	if ( iSoapAction.Length() > 0 )
		{
		iParser->ParseEndL();
		return ETrue;
		}
	return EFalse;
	}
