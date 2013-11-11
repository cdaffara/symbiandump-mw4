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









// INCLUDE FILES
#include <hash.h> 
#include <e32math.h>
#include "wsoviutils.h"
#include "wsovicons.h"
#include "sencryptoutils.h"

HBufC8* CWSOviUtils::NormalizeStringLC(const TDesC8& aMethod, const TDesC8& aUrl, const TDesC8& aHeader )
	{

	
	TInt methodLen = aMethod.Length();
	TInt urlLen = aUrl.Length() + KHexWidth*CountCharsToEncode(aUrl);
	TInt headerLen = aHeader.Length() + KHexWidth*CountCharsToEncode(aHeader);
	TInt normalizedStringLen = methodLen + OAuth::KAmpMark().Length() + urlLen + OAuth::KAmpMark().Length() + headerLen;
		
	HBufC8* pNormalizedString = HBufC8::NewLC(normalizedStringLen);
	
	HBufC8* method = aMethod.AllocLC();
	method->Des().UpperCase();
	pNormalizedString->Des().Append(*method);
	pNormalizedString->Des().Append(OAuth::KAmpMark());

	HBufC8* urlLowercase = aUrl.AllocLC();
	_LIT8(KSlash,"/");
	_LIT8(KDoubleSlash,"//");
	_LIT8(KQuestionMark,"?");
	TInt posStart = urlLowercase->Find(KDoubleSlash) + KDoubleSlash().Length();
	TInt posEnd = urlLowercase->Mid(posStart).Find(KSlash);
	if(posEnd == KErrNotFound)
		{
		urlLowercase->Des().LowerCase();
		}
	else 
		{
		urlLowercase->Des().LeftTPtr(posEnd).LowerCase();
		}
	TPtr8 urlLowercaseBase(urlLowercase->Des());
		
	//cut query & fragment, see http://oauth.net/core/1.0/ (9.1.2.  Construct Request URL)
	posEnd = urlLowercase->Find(KQuestionMark);
	if(posEnd != KErrNotFound)
		{
		urlLowercaseBase.Set(urlLowercase->Des().LeftTPtr(posEnd));
		}
	
	
	HBufC8* url = EncodeCharsLC(urlLowercaseBase);
	pNormalizedString->Des().Append(*url);
	CleanupStack::PopAndDestroy(url);
	CleanupStack::PopAndDestroy(urlLowercase);
	pNormalizedString->Des().Append(OAuth::KAmpMark());
	
	HBufC8* header = EncodeCharsLC(aHeader);
	pNormalizedString->Des().Append(*header);
	CleanupStack::PopAndDestroy(header);
	CleanupStack::PopAndDestroy(method);
			
	return pNormalizedString;
	}

HBufC8* CWSOviUtils::DigestAuthStringL(const TDesC8& aNonce, const TDesC8& aTimestamp, const TDesC8& aUser, const TDesC8& aPassword)
	{
	_LIT8(KFormatUser, "/accounts/%S/");
	HBufC8* validatorString = HBufC8::NewLC(KFormatUser().Length()+aUser.Length());
	TPtr8 validatorPtr = validatorString->Des();
	validatorPtr.Format(KFormatUser,&aUser);
	CSHA1* sha1 = CSHA1::NewL();
	sha1->Update(*validatorString);
	TPtrC8 validator = sha1->Final(aPassword);
	CleanupStack::PopAndDestroy(validatorString);
	CleanupStack::PushL(sha1);
	HBufC8* base64validator = SenCryptoUtils::EncodeBase64L(validator);
	sha1->Reset();
	sha1->Update(aNonce);
	sha1->Update(aTimestamp);
	TPtrC8 digest = sha1->Final(*base64validator);
	delete base64validator;
	HBufC8* digestBase64 = SenCryptoUtils::EncodeBase64L(digest);
	CleanupStack::PopAndDestroy(sha1);
	return digestBase64;
	}


TInt CWSOviUtils::CountCharsToEncode(const TDesC8& aString)
	{
	const TUint8* ref=NULL;
	const TUint8 charLen = 1;
	TUint count(0);
	TPtrC8 unreserveChars(OAuth::KUnReserveChars);
	TUint strLen = aString.Length();
	
	ref=aString.Ptr();
	for(TInt i=0; i<strLen; i++)
		{
		if(unreserveChars.Find(ref, charLen) < 0)
		count++;
		ref+=1;
		}
	
	return count;	
	}
	
HBufC8* CWSOviUtils::EncodeCharsLC(const TDesC8& aString)
	{
	TInt len = aString.Length();
	TInt encodedLength = aString.Length() + KHexWidth*CountCharsToEncode(aString);
	const TUint8* ref=NULL;
	TPtrC8 unreserveChars(OAuth::KUnReserveChars);
	ref=aString.Ptr();
			
	HBufC8* pEncoded = HBufC8::NewLC(encodedLength);
	TPtr8 buf = pEncoded->Des();
	
    for (TInt i = 0; i < len; i++)
        {
		if(unreserveChars.Find(ref, KCharWidth) < 0)
			{
			TBuf8<2> charcode;
			buf.Append(OAuth::KPercent);
			charcode.AppendNumFixedWidth(aString[i], EHex, KHexWidth);
			charcode.UpperCase();
			buf.Append(charcode);			
			}
			else
			{
			buf.Append(&aString[i], KCharWidth);				
			}
		ref+=1;
        }
		
	return pEncoded;
	}

HBufC8* CWSOviUtils::FormEncodeCharsLC(const TDesC8& aString)
    {
    TInt len = aString.Length();
    TInt encodedLength = aString.Length() + KHexWidth*CountCharsToEncode(aString);
    const TUint8* ref=NULL;
    TPtrC8 unreserveChars(OAuth::KUnReserveChars);
    ref=aString.Ptr();
            
    HBufC8* pEncoded = HBufC8::NewLC(encodedLength);
    TPtr8 buf = pEncoded->Des();
    
    for (TInt i = 0; i < len; i++)
        {
        if(unreserveChars.Find(ref, KCharWidth) < 0)
            {
            if (*ref == KSpaceChar)
                {
                buf.Append('+');
                }
            else{
                TBuf8<2> charcode;
                buf.Append(OAuth::KPercent);
                charcode.AppendNumFixedWidth(aString[i], EHex, KHexWidth);
                charcode.UpperCase();
                buf.Append(charcode);
                }
            }
            else
            {
            buf.Append(&aString[i], KCharWidth);                
            }
        ref+=1;
        }
        
    return pEncoded;
    }
