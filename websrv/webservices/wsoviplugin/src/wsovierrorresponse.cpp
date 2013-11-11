/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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








#include "wsovierrorresponse.h"
#include "wsovicons.h"

using namespace WSOviResponse;
CWSOviErrorResponse* CWSOviErrorResponse::NewL()
    {
    CWSOviErrorResponse* self = NewLC();
    CleanupStack::Pop(self);
    return self;
    }

CWSOviErrorResponse* CWSOviErrorResponse::NewLC()
    {
    CWSOviErrorResponse* self = new (ELeave) CWSOviErrorResponse();
    CleanupStack::PushL (self);
    self->ConstructL();
    return self;
    }
// Second phase construction.
void CWSOviErrorResponse::ConstructL()
    {
        CSenBaseFragment::BaseConstructL(TPtrC8(NULL,0),
                KErrorNode);
    }

void CWSOviErrorResponse::StartElementL(const TDesC8& /*aNsUri*/,
                               const TDesC8& aLocalName,
                               const TDesC8& /*aQName*/,
                               const RAttributeArray& /*aAttributes*/)
    {
       if ((aLocalName == KErrorCode) ||
           (aLocalName == KErrorText))
        	{
        	iState = KStateSave;
        	}
    }                       
    
void CWSOviErrorResponse::EndElementL(const TDesC8& /*aNsUri*/,
                             const TDesC8& aLocalName,
                             const TDesC8& /*aQName*/)
    {
    TPtrC8 content = Content();
    if (iState == KStateSave)
        {
    	if (aLocalName == KErrorCode)
            {
            iCode = HBufC8::NewL(content.Length());
            iCode->Des().Append(content);
            ResetContentL();
            iState = KStateIgnore;
            }
    	else if (aLocalName == KErrorText)
            {
            iText = HBufC8::NewL(content.Length());
            iText->Des().Append(content);
            ResetContentL();
            iState = KStateIgnore;
            }
        }
    }


CWSOviErrorResponse::CWSOviErrorResponse()
	{
	}

CWSOviErrorResponse::~CWSOviErrorResponse()
	{
	delete iCode;
	delete iText;
	}

TPtrC8 CWSOviErrorResponse::Code()
    {
    if (iCode)
        {
        return *iCode;    
        }
    else 
        {
        return KNullDesC8();
        }
    }

TPtrC8 CWSOviErrorResponse::Text()
    {
    if (iText)
        {
        return *iText;    
        }
    else 
        {
        return KNullDesC8();
        }
    }
