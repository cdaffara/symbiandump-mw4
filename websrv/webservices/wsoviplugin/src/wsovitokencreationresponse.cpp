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








#include "wsovitokencreationresponse.h"
#include "wsovicons.h"
#include "sendebug.h"
#include "senlogger.h"

using namespace WSOviResponse;
CWSOviTokenCreationResponse* CWSOviTokenCreationResponse::NewL()
    {
    CWSOviTokenCreationResponse* self = NewLC();
    CleanupStack::Pop(self);
    return self;
    }

CWSOviTokenCreationResponse* CWSOviTokenCreationResponse::NewLC()
    {
    CWSOviTokenCreationResponse* self = new (ELeave) CWSOviTokenCreationResponse();
    CleanupStack::PushL (self);
    self->ConstructL();
    return self;
    }
// Second phase construction.
void CWSOviTokenCreationResponse::ConstructL()
    {
    CSenBaseFragment::BaseConstructL(TPtrC8(NULL,0), KTokenCreationResponseLocalName);
    }

void CWSOviTokenCreationResponse::StartElementL(const TDesC8& /*aNsUri*/,
                               const TDesC8& aLocalName,
                               const TDesC8& /*aQName*/,
                               const RAttributeArray& /*aAttributes*/)
    {
    switch (iState)
        {
        case KStateIgnore:
            {
            if (aLocalName == KTokenInfoLocalName)
            	{
            	iState = KStateParsingTokenInfo;
            	}
            else if (aLocalName == KTokenCreationResponseLocalName)
                {
                iIsTokenUpdate = EFalse;
                }
            else if (aLocalName == KUserInfoLocalName)
                {
                iState = KStateParsingUserInfo;
                }
                break;
            }
        case KStateParsingTokenInfo:
        	{
        	if (aLocalName == KTokenSecretLocalName)
        		{
        		iState = KStateSave;
        		}
          else if (aLocalName == KTokenLocalName)
              {
              iState = KStateSave;
              }
          else if (aLocalName == KTTLLocalName)
	          {
	      	  iState = KStateSave;
	          }
          else if (aLocalName == KExpiresLocalName)
              {
              iState = KStateSave;
              }        	
        	break;
        	}
        case KStateParsingUserInfo:
            {
            if (aLocalName == KUserNameLocalName)
                {
                iState = KStateSave;
                }
            break;
            }
        default:
            break;
        }
    }                       
    
void CWSOviTokenCreationResponse::EndElementL(const TDesC8& /*aNsUri*/,
                             const TDesC8& aLocalName,
                             const TDesC8& /*aQName*/)
    {
    TPtrC8 content = Content();
    switch (iState)
        {
        case KStateSave:
            {
        	if (aLocalName == KTokenLocalName)
                {
                if (iToken)
                	{
                	delete iToken;
                	iToken = NULL;
                	}
                TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"WSOviTokenCreationResponse::token parsing");
                TLSLOG_ALL(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel, content);
                	
                iToken = HBufC8::NewL(content.Length()+KTokenTag().Length()+KTokenTagEnd().Length());
                iToken->Des().Append(KTokenTag);
                iToken->Des().Append(content);
                iToken->Des().Append(KTokenTagEnd);
                ResetContentL();
                iState = KStateParsingTokenInfo;
                }
            if (aLocalName == KTokenSecretLocalName)
                {
                if (iTokenSecret)
                    {
                    delete iTokenSecret;
                    iTokenSecret = NULL;
                    }
                TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"WSOviTokenCreationResponse::tokenSecret parsing");
                TLSLOG_ALL(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel, content);
                    
                iTokenSecret = content.AllocL();
                ResetContentL();
                iState = KStateParsingTokenInfo;
                }
            else if (aLocalName == KTTLLocalName)
                {
                if (iTTL)
                	{
                	delete iTTL;
                	iTTL = NULL;
                	}
                TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"WSOviTokenCreationResponse::ttl parsing");
                TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel, content);
                iTTL = content.AllocL();
                ResetContentL();
                iState = KStateParsingTokenInfo;
                }
            else if (aLocalName == KExpiresLocalName)
                {
                if(iValidUntil)
                	{
                	delete iValidUntil;
                	iValidUntil = NULL;
                	}
                TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"WSOviTokenCreationResponse::expires parsing");
                TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KMinLogLevel, _L8("iValidUntil : %S"), &content));
                
                iValidUntil = content.AllocL();
                
                ResetContentL();
                iState = KStateParsingTokenInfo;
                }            
            else if ( aLocalName == KUserNameLocalName)
                {
                if (iUsername)
                    {
                    delete iUsername;
                    iUsername = NULL;
                    }
                TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"WSOviTokenCreationResponse::username parsing");
                TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel, content);
                iUsername = content.AllocL();
                ResetContentL();
                iState = KStateParsingUserInfo;
                }
            break;
            }
        case KStateParsingTokenInfo:
        	{
        	if (aLocalName == KTokenInfoLocalName)
        		{
            	iState = KStateIgnore;
        		}
        	break;
        	}
        case KStateParsingUserInfo:
            {
            if (aLocalName == KUserInfoLocalName)
                {
                iState = KStateIgnore;
                }
            break;
            }
        default:
            break;
        }
    }


CWSOviTokenCreationResponse::CWSOviTokenCreationResponse():
iIsTokenUpdate(ETrue)
	{
	}

CWSOviTokenCreationResponse::~CWSOviTokenCreationResponse()
	{
	delete iToken;
	delete iTokenSecret;
	delete iTTL;
	delete iUsername;
	delete iValidUntil;
	}

TPtrC8 CWSOviTokenCreationResponse::Token()
    {
    if (iToken)
        {
        return *iToken;    
        }
    else 
        {
        return KNullDesC8();
        }
    }

TPtrC8 CWSOviTokenCreationResponse::TokenSecret()
    {
    if (iTokenSecret)
        {
        return *iTokenSecret;    
        }
    else 
        {
        return KNullDesC8();
        }
    }

TPtrC8 CWSOviTokenCreationResponse::TTL()
    {
    if (iTTL)
        {
        return *iTTL;    
        }
    else 
        {
        return KNullDesC8();
        }
    }

TPtrC8 CWSOviTokenCreationResponse::ValidUntil()
    {
    if (iValidUntil)
        {
        return *iValidUntil;    
        }
    else 
        {
        return KNullDesC8();
        }    	
    }

TPtrC8 CWSOviTokenCreationResponse::Username()
    {
    if (iUsername)
        {
        return *iUsername;    
        }
    else 
        {
        return KNullDesC8();
        }
    }

TBool CWSOviTokenCreationResponse::IsTokenUpdate()
    {
    return iIsTokenUpdate;
    }
