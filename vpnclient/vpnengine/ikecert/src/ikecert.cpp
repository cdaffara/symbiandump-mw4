/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   General utility methods for certificate handling
*
*/



#include <x509cert.h>

#include "ikecert.h"
#include "ikev1pkiservice.h"
#include "ikev2const.h"
#include "ikecaelem.h"
#include "ikecertconst.h"


const TUint8 Pkcs1v15Sha1Header[15] = {0x30, 0x21, 0x30, 0x09, 0x06, 0x05, 0x2b, 0x0e, 0x03, 0x02, 0x1a, 0x05, 0x00, 0x04, 0x14};


TUint8* IkeCert::BERGetLengthL(TUint8* aP, TInt &aLen)
{
    ASSERT(aP);
	aP++;  // skip tag
	if (*aP <= 127) 
	{   
		aLen = *aP;
		aP++;
	}
	else if (*aP == 0x81)
	{
		aP++;
		aLen = *aP;
		aP++;
	}
	else if (*aP == 0x82)
	{
		aP++;
		aLen = *aP;
		aP++;
		aLen *= 256;
		aLen += *aP;
		aP++;
	}
	else {
		User::Leave(KErrGeneral);
	}
	return aP;
}


EXPORT_C HBufC8* IkeCert::GetCertificateFieldDERL(const CX509Certificate* aCert, TInt aField)
{
	if ( !aCert )
	   return NULL;	
	const TPtrC8 SignedData = aCert->SignedDataL();
	if ( SignedData.Length() == 0 )
	   return NULL;
	TUint8* Ptr = (TUint8*)SignedData.Ptr();
	TUint8* FieldPtr;
	HBufC8* FieldBfr = NULL;
	TInt length = 0;	
	 // begin sequence
	Ptr = IkeCert::BERGetLengthL(Ptr, length);
	// context specific a0 03
	if (*Ptr==0xa0)
		Ptr += 2; 
	// version
	if (*Ptr==2)
	{
		Ptr = IkeCert::BERGetLengthL(Ptr, length);
		Ptr += length;
	}
	// seq number
	if (*Ptr==2)
	{
		Ptr = IkeCert::BERGetLengthL(Ptr, length);
		Ptr += length;
	}
	// sign algorithm
	Ptr = IkeCert::BERGetLengthL(Ptr, length);
	Ptr += length;
	// issuer name
	FieldPtr = Ptr;
	Ptr = IkeCert::BERGetLengthL(Ptr, length);
	Ptr += length;
	if ( aField == KIssuerName )
	    {   
	       FieldBfr = HBufC8::NewL(Ptr - FieldPtr);
	       FieldBfr->Des().Copy(FieldPtr, (Ptr - FieldPtr));
	    }
	// validity period
	Ptr = IkeCert::BERGetLengthL(Ptr, length);
	Ptr += length;
	// subject name
	FieldPtr = Ptr;
	Ptr = IkeCert::BERGetLengthL(Ptr, length);
	Ptr += length;
	if ( aField == KSubjectName )
	{	
	   FieldBfr = HBufC8::NewL(Ptr - FieldPtr);
	   FieldBfr->Des().Copy(FieldPtr, (Ptr - FieldPtr));
	}
	// public key info	
	FieldPtr = Ptr;
	Ptr = IkeCert::BERGetLengthL(Ptr, length);
	Ptr += length;
	if ( aField == KPublicKeyInfo )
	{	
		FieldBfr = HBufC8::NewL(Ptr - FieldPtr);
		FieldBfr->Des().Copy(FieldPtr, (Ptr - FieldPtr));
	}

	return FieldBfr;
}	


HBufC8* IkeCert::BuildPkcs1v15HashL(const TDesC8 &aHashIn)
{
	//
	// Build Pkcs1v15 format ASN1 header for specified hash.
	// Current implementation supports only hash algorithm SHA1 so
	// the aHashIn length data MUST be exactly the length of SHA1 hash
	// (20 bytes) 
	//
	HBufC8* Pkcs1v15Hash = NULL;
	
	ASSERT( aHashIn.Length() == 20 );
	
   Pkcs1v15Hash = HBufC8::NewL(20 + sizeof(Pkcs1v15Sha1Header));
   if ( Pkcs1v15Hash )
   {
       Pkcs1v15Hash->Des().Copy((TUint8*)Pkcs1v15Sha1Header, sizeof(Pkcs1v15Sha1Header));
       Pkcs1v15Hash->Des().Append(aHashIn);
   }	   
	return Pkcs1v15Hash;
}


EXPORT_C HBufC8* IkeCert::GetCertificateFieldDERL(HBufC8* aCertBfr, TInt aField)
{
	if ( !aCertBfr )
	   return NULL;	
	CX509Certificate* Cert = CX509Certificate::NewL(*aCertBfr);
	CleanupStack::PushL(Cert);	
    HBufC8* Field  = IkeCert::GetCertificateFieldDERL(Cert, aField);
	CleanupStack::PopAndDestroy(Cert); 
	return Field;
}


TBool IkeCert::AltNameExistsL(const CX509Certificate *aX509Cert, const TDesC8 &aId)
{
    ASSERT(aX509Cert);
	const CX509CertExtension *AltNameExt = aX509Cert->Extension(KSubjectAltName);
	CX509GeneralName *NameId = CX509GeneralName::NewLC(aId);
	TBool found = EFalse;
	if (AltNameExt)
	{
		CX509AltNameExt* AltExt = CX509AltNameExt::NewLC(AltNameExt->Data());
		const CArrayPtrFlat<CX509GeneralName>&Names = AltExt->AltName();
		TInt Count = Names.Count();
		for (TInt i = 0; i < Count; i++)
		{
			const CX509GeneralName *Name = Names.At(i);
			if  (NameId->Tag()  == Name->Tag() &&
				 NameId->Data() == Name->Data())
			{
				found = ETrue;
				break;
			}
		}
		CleanupStack::PopAndDestroy(AltExt);
	}
	CleanupStack::PopAndDestroy(NameId);
	return found;
}


EXPORT_C HBufC8* IkeCert::GetSubjectAltNameDataL(const CX509Certificate* aX509Cert, TUint8 aIkeIdType)
{
    ASSERT(aX509Cert);
	HBufC8* Identity = NULL;
	const CX509CertExtension* AltNameExt = aX509Cert->Extension(KSubjectAltName);

	if ( AltNameExt )
	{
		TGNType SubjAltNameType;
		switch ( aIkeIdType )
		{
			case ID_IPV4_ADDR:
				SubjAltNameType = EX509IPAddress;
				break;
			case ID_FQDN:
				SubjAltNameType = EX509DNSName;
				break;
			case ID_RFC822_ADDR:
				SubjAltNameType = EX509RFC822Name;
				break;
			case ID_IPV6_ADDR:
				SubjAltNameType = EX509IPAddress;
				break;
			default:
				SubjAltNameType = EX509RFC822Name;
				break;
		}   
		CX509AltNameExt* AltExt = CX509AltNameExt::NewLC(AltNameExt->Data());
		const CArrayPtrFlat<CX509GeneralName>&Names = AltExt->AltName();
		TInt count = Names.Count();
		for (TInt i = 0; i < count; i++)
		{
			const CX509GeneralName *Name = Names.At(i);
			if  ( Name->Tag() == SubjAltNameType )
			{
				//
				// Allocate buffer and  Copy subject alt name data to it (type tag and length is not copied !)
				//
				Identity = HBufC8::NewL(Name->Data().Length() - 2);
				Identity->Des().Copy(((TUint8*)(Name->Data().Ptr()) + 2), (Name->Data().Length() - 2));
				break;
			}
		}
		CleanupStack::PopAndDestroy(AltExt); 

	}   
	return Identity;
}


TInt IkeCert::CheckValidityPeriod(const CX509Certificate& aCert, TInt aWarningMargin, TInt aErrorMargin )
{
	TInt  Status = KErrNone;
	TTime current;
	current.UniversalTime();
	TTimeIntervalSeconds ErrorMargin(aErrorMargin); 
	TTime StartTime  = aCert.ValidityPeriod().Start();
	TTime FinishTime = aCert.ValidityPeriod().Finish();
	if ( (current + ErrorMargin) < StartTime )
	{
		Status = KCertVerifyErrNotValidYet;
	}
	else
	{
		if (current > (FinishTime + ErrorMargin) )
		{
			Status = KCertVerifyErrExpired;
		}
		else
		{
		  //
		  // If a warning margin defined, check is the certificate within that
		  //
			if ( aWarningMargin )
			{
				TTimeIntervalSeconds WarningMargin(aWarningMargin);
				if ( (current + WarningMargin) > (FinishTime + ErrorMargin) ) {
					Status = KCertVerifyWithinMargin;             
				}   
			}   
		}       
	}

	return Status;
}   

//
// Verify certificate extensions
// 
TInt IkeCert::VerifyCertExtensionsL(const CX509Certificate& aX509Cert)
{
	TInt Status = KErrNone;
	const CArrayPtrFlat<CX509CertExtension>& CertExtensions = aX509Cert.Extensions();	
	CX509CertExtension* Extension;  
	TInt Count = CertExtensions.Count();
	TInt i     = 0;

	while ( i < Count )
	{
		Extension = CertExtensions.At(i);
		if ( Extension->Id() == KKeyUsage )
		{
		   //
		   // KeyUsage extension MUST have either digitalSignature or
		   // nonRepudiation bit set
		   //
			CX509KeyUsageExt* KeyUsage = CX509KeyUsageExt::NewL(Extension->Data());
			if ( !KeyUsage->IsSet(EX509DigitalSignature) && !KeyUsage->IsSet(EX509NonRepudiation) )
			{
				delete KeyUsage;  
				Status = KCertVerifyKeyUsageErr;
				break;
			}
			else delete KeyUsage;
		}
		else if ( Extension->Id() == KBasicConstraints )
		{
		   //
		   // BasicConstraints extension MUST NOT have CA indicator
		   //
			CX509BasicConstraintsExt* BasicConstraints = CX509BasicConstraintsExt::NewL(Extension->Data());
			if ( BasicConstraints->IsCA() )
			{
				delete BasicConstraints; 
				Status = KCertVerifyCACertificate;
				break;
			}
			else delete BasicConstraints;
		}	
		else if ( Extension->Id() != KSubjectAltName && Extension->Critical() )				
		{
		   //
		   // Unsupported critical section ==> Certificate NOT accepted 
		   //
			Status = KCertVerifyCriticalExt;
			break;
		}   	

		i++;
	}

	return Status;
}
          
    
void IkeCert::CleanupSequence(TAny* aArray)
    {
    ASSERT(aArray);
	CArrayPtrFlat<TASN1DecGeneric>* array = reinterpret_cast<CArrayPtrFlat<TASN1DecGeneric>*>(aArray);
    ASSERT(array);
	array->ResetAndDestroy();
	delete array;
    }

