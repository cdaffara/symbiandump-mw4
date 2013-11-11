/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   static method for certficate handling
*
*/

#include <x509cert.h>
#include <asn1dec.h>
#include <x500dn.h>

#include "ikepkiutils.h"
#include "ikev1pkiservice.h"
#include "ikepublickey.h"
#include "utlcrypto.h"
#include "ikecert.h"
#include "ikecalist.h"
#include "ikecaelem.h"
#include "ikev2const.h"
#include "ikev2payloads.h"
#include "ikecertconst.h"

EXPORT_C TBool IkePkiUtils::CertifyIdentityL(const CX509Certificate* aCert, 
                                             TDesC8& aId, TInt aIdType)
                                          
    {
	TBool status = EFalse;
	if ( aCert )
	    {	
	   //
	   // Check that specified identity exist in current certificate
	   // data. If aIdType type is ID_DER_ASN1_DN, identity must match
	   // with Certificate Subject name. 
	   // All other aIdTypes ara checked against SubjectAltNames data,
	   // if present 
	   //
		if ( aIdType == ID_DER_ASN1_DN )
		    {
	        //
	        // Binary DER encoding of an ASN.1 X.500 Distinguished Name identity
	        // is certified by comparing it to peer certificate subject name
	        //
	        			
			CX500DistinguishedName* asn1DnName = CX500DistinguishedName::NewLC(aId);
		    status = asn1DnName->ExactMatchL(aCert->SubjectName());			   			   
		    CleanupStack::PopAndDestroy(asn1DnName);

		    }
		else
		    {
            //
            // Other identity types are certified by comparing it to the SubjectAltName field        
            //
			HBufC8* idBuf = HBufC8::NewLC(aId.Length() + 2);
			TUint8* idHdr = (TUint8*)idBuf->Des().Ptr();
			idHdr[1]      = (TUint8)aId.Length();

			switch ( aIdType ) 
			    {
				case ID_IPV4_ADDR:
					idHdr[0] = 0x87;
					break;
				case ID_FQDN:
					idHdr[0] = 0x82;                       
					break;
				case ID_RFC822_ADDR:
					idHdr[0] = 0x81;                                   
					break;
				case ID_IPV6_ADDR:
					idHdr[0] = 0x87;           
					break;
				default:
					idHdr = NULL;
                    break;
		    	}

			if ( idHdr )
			    {	
			   Mem::Copy(&idHdr[2], aId.Ptr(), aId.Length());
			   TPtrC8 ptrId(idHdr, (aId.Length() + 2));
			   status = IkeCert::AltNameExistsL(aCert, ptrId);
		    	}
			CleanupStack::PopAndDestroy();  //IdBuf			
		    }
    	}	

	return status;
    }


EXPORT_C TBool IkePkiUtils::VerifyIkev2SignatureL(const TDesC8& aSignature, 
                                                  const TDesC8& aAuthData, 
                                                  const CX509Certificate& aCert)
    {
	//
	// Calculate SHA1 hash over aAuthdata to build reference hash and
	// verify IKEv2 signature with it
	//
	CUtlMessageDigest* Digest = TUtlCrypto::MakeMessageDigesterL(TUtlCrypto::EUtlMessageDigestSha1);
	CleanupStack::PushL(Digest);			
	TPtrC8 RefHash = Digest->Final(aAuthData);
	TBool Status   = VerifySignatureL(MAJORV2, aSignature, RefHash, aCert);
	CleanupStack::PopAndDestroy(Digest);
	
	return Status; 
    }	 


EXPORT_C TBool IkePkiUtils::VerifyIkev1SignatureL(const TDesC8& aSignature, 
                                                  const TDesC8& aRefHash, 
                                                  const CX509Certificate& aCert)
    {
	//
	// Verify IKEv1 signature. 
	//
	return VerifySignatureL(MAJORV1, aSignature, aRefHash, aCert);
    }	 
    
    
EXPORT_C HBufC8* IkePkiUtils::GetIdentityFromCertL(TUint8 aIdType, const TDesC8& aCertData)
    {
	//
	// Get IKE Identity data from specified Certificate data. The Id
	// type parameter specifies the field of Certificate from where the
	// Identity data is taken
	//
	HBufC8* identity;
	CX509Certificate* cert = CX509Certificate::NewL(aCertData);
	CleanupStack::PushL(cert);			
	if ( (aIdType == ID_DER_ASN1_DN) || (aIdType == ID_NOT_DEFINED) || (aIdType > ID_IPV6_ADDR) )
	    {       
	    // 
	    // DER encoded ASN.1 X.500 Distinguished Name as IKE identity
		// Get Id data from certificate subject name
	    //
		identity = IkeCert::GetCertificateFieldDERL(cert, KSubjectName);
	    }
	else
	    {
	     //
	     // Get IKE Identity from own certifate subject alt name extension (according to IdType value)
	     //
		identity = IkeCert::GetSubjectAltNameDataL(cert, aIdType); 
	    }
	CleanupStack::PopAndDestroy(cert); 
	
	return identity;
    }	 


EXPORT_C TBool IkePkiUtils::GetCertSubjectNameDERL(const CX509Certificate* aCert, 
                                                   TDes8& aSubjectName)
    {
	TBool status = ETrue;
	HBufC8* nameBfr = IkeCert::GetCertificateFieldDERL(aCert, KSubjectName);
    if ( nameBfr && ( nameBfr->Des().Length() <= aSubjectName.MaxLength() ) )
	    {
		aSubjectName.Copy(nameBfr->Des());
		delete nameBfr;
	    }
	else 
	    {
		aSubjectName.SetLength(0);
		status = EFalse;
	    }

	return status;
    }	        
 

EXPORT_C CX509Certificate* IkePkiUtils::VerifyCertificateL(const CArrayFixFlat<const TCertificateISAKMP*>& aCerts,
                                                           const CIkeCaList& aTrustedCAList)
    {
    const CArrayFixFlat<TCertPayloadIkev2*>* certificateArray = IkePkiUtils::CastCertArray(&aCerts);    
    return VerifyCertificateL(*certificateArray, aTrustedCAList);
    
    }

 
EXPORT_C CX509Certificate* IkePkiUtils::VerifyCertificateL(const CArrayFixFlat<TCertPayloadIkev2*>& aCerts,
                                                           const CIkeCaList& aTrustedCAList)
   {
	//
	// Find a requested certificate or chain of certificates from IKE certificate payload array
	// using trusted CA list (iTrustedCAList).
	// When requested certificate found verify this certificate. 
	//
	CX509Certificate* certOk = NULL;	
	CX509Certificate* caCert = NULL;
	CX509Certificate* currCert=NULL;
	const TCertPayloadIkev2* firstCertPayload=NULL;
	const TCertPayloadIkev2* certPayload = aCerts.At(0);
    __ASSERT_DEBUG(certPayload != NULL, User::Invariant());
    CArrayFixFlat<CX509Certificate*>* caArray=new (ELeave) CArrayFixFlat<CX509Certificate*> (aCerts.Count());
    CleanupStack::PushL(caArray);
    for ( TInt i = 0; i < aCerts.Count(); i++  ) 
        {
         const TCertPayloadIkev2* currCertPayload=aCerts.At(i);
         __ASSERT_DEBUG(currCertPayload != NULL, User::Invariant());
         if ( currCertPayload->GetEncoding() == X509_CERTIFICATE_SIGN )
        {          
             firstCertPayload=currCertPayload;
             const TPtrC8 CertStream(currCertPayload->Certificate(),
                                    (TPayloadIkev2::Cast(currCertPayload)->GetLength() - 
                                     TCertPayloadIkev2::Size()));
             currCert = CX509Certificate::NewLC(CertStream);
             caArray->AppendL(currCert);
            }
        }
    if ( caArray->Count() == 1)
           {
 	         __ASSERT_DEBUG(firstCertPayload != NULL, User::Invariant());
         const TPtrC8 userCertStream(firstCertPayload->Certificate(),
                                    (TPayloadIkev2::Cast(firstCertPayload)->GetLength() - 
                                     TCertPayloadIkev2::Size()));
         firstCertPayload=NULL;
            CX509Certificate* clientCert = CX509Certificate::NewLC(userCertStream);
            caCert = IkePkiUtils::FindCaCertificateL(*clientCert, aTrustedCAList);
            if ( caCert )
                {
                 certOk = IkePkiUtils::VerifyX509CertificateL(*caCert, *clientCert);
             if ( certOk )  // CertOk = clientCert
                    {
                     CleanupStack::Pop(clientCert);  // CertOk = Cert              
                clientCert=NULL;
                
                CleanupStack::PopAndDestroy(currCert);
                currCert=NULL;
                
                CleanupStack::PopAndDestroy(caArray);
                caArray=NULL;
                
                return certOk; 
                }
            }   
         CleanupStack::PopAndDestroy(clientCert);  // Cert
         clientCert=NULL;
         
         CleanupStack::PopAndDestroy(currCert);
         currCert=NULL;
         
         CleanupStack::PopAndDestroy(caArray);
         caArray=NULL;
         return NULL;
         }
    if ( caArray->Count()>1 ) //if certificate chain is received
       {    
           CX509Certificate* userCert=NULL;
           CX509Certificate* certChainRoot = IkePkiUtils::VerifyCertChainL(*caArray, userCert, aTrustedCAList);
           
           CX509Certificate* realUserCert=userCert; //Real user certificate found from chain as a parameter by reference 
                             
           //cleaning
           TInt certCount=caArray->Count();
           for ( TInt i=0; i<certCount ;i++)
               CleanupStack::Pop();
           CleanupStack::Pop(caArray);
           for ( TInt i=0; i<certCount ;i++)
               {
                CX509Certificate* itemPtr=caArray->At(i);
                if ( realUserCert != itemPtr && certChainRoot != itemPtr )
                    delete itemPtr;
               }
           for ( TInt i=0; i<certCount ;i++)
                 caArray->Delete(0);
           delete caArray;
               
           if ( certChainRoot )
               caCert = IkePkiUtils::FindCaCertificateL(*certChainRoot, aTrustedCAList);
           else
               caCert=NULL;
           if ( caCert )
               certOk = IkePkiUtils::VerifyX509CertificateL(*caCert, *certChainRoot);
           if ( certChainRoot != userCert )
           			delete certChainRoot;
           if ( certOk )
              return realUserCert;
              
           else
              return NULL;
           }
    return NULL;
    }


EXPORT_C const CArrayFixFlat<TCertPayloadIkev2*>* IkePkiUtils::CastCertArray(const CArrayFixFlat<const TCertificateISAKMP*>* aCerts) 
    { 
    return reinterpret_cast<const CArrayFixFlat<TCertPayloadIkev2*>*>(aCerts); 
    }


CX509Certificate* IkePkiUtils::VerifyX509CertificateL(const CX509Certificate& aCaCert, 
                                                      CX509Certificate& aCert)
    {
	TPKIKeyAlgorithm reqKeyType = EPKIInvalidAlgorithm;

	switch (aCert.SigningAlgorithm().AsymmetricAlgorithm().Algorithm())
    	{
		case ERSA:
			reqKeyType = EPKIRSA;
			break;
		case EDSA:
			reqKeyType = EPKIDSA;
			break;
		default:
			break;
	    }

	if ( reqKeyType == EPKIInvalidAlgorithm )
		return NULL;
		
	CIkePublicKey* pubKey = CIkePublicKey::NewL(aCaCert);
	if ( !pubKey )
		return NULL;	

	if ( reqKeyType != pubKey->Algorithm() )
	    {
		delete pubKey;
		return NULL;
	    }	
	CleanupStack::PushL(pubKey);
	TInt  ret   = KErrNotSupported;
	TBool valid = EFalse;

	switch (pubKey->Algorithm())
    	{
		case EPKIRSA:
			TRAP(ret, valid = aCert.VerifySignatureL(pubKey->KeyData()));
			break;

		case EPKIDSA:
		    {
			TX509KeyFactory keyFactory;
			CDSAParameters* params = keyFactory.DSAParametersL(pubKey->KeyParams());
			CleanupStack::PushL(params);

			CSigningKeyParameters*sgkp = CSigningKeyParameters::NewLC();
			sgkp->SetDSAParamsL(*params);
			aCert.SetParametersL(*sgkp);
			TRAP(ret, valid = aCert.VerifySignatureL(pubKey->KeyData()));
			CleanupStack::PopAndDestroy(sgkp); 
			CleanupStack::PopAndDestroy(params); 
			break;
		    }

		case EPKIInvalidAlgorithm:
			break;
	    }
			
	CX509Certificate* ValidCert = NULL;	
	if ( valid && (ret == KErrNone) )
	    {
		//
		// One hour error margin, no warning margin
		//
		ret = IkeCert::CheckValidityPeriod(aCert, 0, KDefaultErrorMargin);
		if ( ret == KErrNone )
	    	{	
			//
			// Verify certificate extensions
			//
			ret = IkeCert::VerifyCertExtensionsL(aCert);
			if ( ret == KErrNone || ret == KCertVerifyCACertificate )
				ValidCert = &aCert;			
	    	}		
    	}	
		
	CleanupStack::PopAndDestroy(pubKey);
	return ValidCert;  
}


CX509Certificate* IkePkiUtils::FindCaCertificateL(const CX509Certificate& aUserCert,
                                                  const CIkeCaList& aTrustedCAList)
    {
	//
	// Find a CA certificate from Trusted CA list (iTrustedCAList) for
	// specified user certificate
	//	
	const CX500DistinguishedName& issuerName = aUserCert.IssuerName();	
	CX509Certificate* caCert = NULL;

    for (TInt i = 0; i < aTrustedCAList.Count(); i++)
	    {
    	//
	    // Find CA certificate for current user certificate by
	    // comparing certificate issuer name to the subject name of a
	    // CA certificate
	    //		
		caCert = aTrustedCAList.At(i)->Certificate();
		if ( issuerName.ExactMatchL(caCert->SubjectName()) )
		    {
		    break;	
		    }
		caCert = NULL;
	    }	

	return caCert;
    }


TBool IkePkiUtils::VerifySignatureL(TInt aIkeVersion, 
                                    const TDesC8& aSignature, 
                                    const TDesC8& aRefHash, 
                                    const CX509Certificate& aCert)
    {
	//
	// Verify IKE signature. 
	//
	TBool status = EFalse;
	
	if ( aSignature.Length() > 0 )	
	    {	    	
    	CIkePublicKey* publicKey = CIkePublicKey::NewL(aCert);
    	if ( !publicKey )
    	    {	    
    		return EFalse;			
    	    }

    	CleanupStack::PushL(publicKey);			

    	switch (publicKey->Algorithm())
    	    {
    		case EPKIRSA:
    			{
                HBufC8 *resBuf;
                TUtlCrypto::RsaPublicKeyDecryptL(publicKey->KeyData(), aSignature, resBuf);
                CleanupStack::PushL(resBuf);							
                
                if ( aIkeVersion == MAJORV1 ) 
                    {
                    //
                    // Because IKEv1 signature is not a "real" PKCS1
                    // encoded signature but pure private encrypted has
                    // signature is verified by using RSA public key
                    // decrypt and result comparison to reference hash
                    //
                    status = (aRefHash.Compare(*resBuf) == 0); //Compare the result with the hash to see if they match
                    }
                else
    				{
                    //
                    // IKEv2(n) signature is encoded as PKCS1v1_5
                    // signature (EMSA-PKCS1-v1_5)
                    // ASN1 encoding of signature is the following:
                    //	DigestInfo::=SEQUENCE{
                    //	  digestAlgorithm  AlgorithmIdentifier,
                    //	  digest OCTET STRING }
                    //
                    CArrayPtrFlat<TASN1DecGeneric>* seq = NULL;
                    TInt position = 0;
    									
                    TRAPD(err, seq = DecodeDERL(*resBuf, position));
                    if ( err == KErrNone )
                        {
                        TCleanupItem CleanupSeq(IkeCert::CleanupSequence, seq);						
                        CleanupStack::PushL(CleanupSeq);
                        if (seq->Count() == 2)
                            {
                            //
                            // Currently the digestAlgorithm is not
                            // verified, but only digest value itself is
                            // compared with reference hash.
                            // ( see CPKCS1SignatureResult::DoVerifyL() in
                            //   x509cert.cpp)
                            // 
                            const TASN1DecGeneric* gen2 = seq->At(1);                            
                            TPtrC8 digest(gen2->GetContentDER());
                            status = (aRefHash.Compare(digest) == 0);
                            }
                        CleanupStack::PopAndDestroy(); //CleanupSeq
                        }
                    else
                        {
                        //
                        // Verify signature as pure encrypted (SHA1)
                        // hash as old IKEv1 style "signature" 
                        //
                        //DEB(iService.PrintText(_L("Old IKEv1 style signature used by IKEv2 peer !\n"));)					   
                        status = (aRefHash.Compare(*resBuf) == 0); //Compare the result with the hash to see if they match					   
                        }	
    				}
                CleanupStack::PopAndDestroy(resBuf);
    			break;
    			}
            case EPKIDSA:
                {
                const TPtrC8 sigR = aSignature.Left(aSignature.Length() / 2);
                const TPtrC8 sigS = aSignature.Right(aSignature.Length() / 2);

                status = TUtlCrypto::DsaVerifySignatureL(publicKey->KeyData(), 
                                                         publicKey->KeyParams(), 
                                                         sigR, sigS, aRefHash);
                break;
                }
            default:        //Only RSA and DSA are valid
                User::Invariant();
            	break;
    		}

    	CleanupStack::PopAndDestroy(publicKey);
	    }
	return status;
    }	


CArrayPtrFlat<TASN1DecGeneric>* IkePkiUtils::DecodeDERL(const TDesC8& aPtr, TInt& aPosition)
    {
	TASN1DecSequence decSeq;
	CArrayPtrFlat<TASN1DecGeneric>* seq =	decSeq.DecodeDERLC(aPtr, aPosition);
	CleanupStack::Pop(seq);
	return seq;
    }

EXPORT_C CX509Certificate* IkePkiUtils::VerifyCertChainL(const CArrayFixFlat<CX509Certificate*>& aCerts, CX509Certificate*& realUserCert, const CIkeCaList& aTrustedCAList)
    {
    CX509Certificate* currCaCert=NULL;
    CX509Certificate* certOk=NULL;
    CArrayFixFlat<const CX500DistinguishedName*>* issuerArray=new (ELeave) CArrayFixFlat<const CX500DistinguishedName*> (aCerts.Count()); 
    CArrayFixFlat<const CX500DistinguishedName*>* subjectArray=new (ELeave) CArrayFixFlat<const CX500DistinguishedName*> (aCerts.Count()); 
    CleanupStack::PushL(issuerArray);
    CleanupStack::PushL(subjectArray);
    for ( TInt i = 0; i < aCerts.Count(); i++  ) 
        {
        issuerArray->AppendL(&aCerts.At(i)->IssuerName());
        subjectArray->AppendL(&aCerts.At(i)->SubjectName());
        }
    TInt userCertIndex=0; 
    TInt caCertIndex=0;
    
    //find UserCert from aCerts array if not in first certificate payload
    for ( TInt i = 0; i < (aCerts.Count()); i++  )
        {
        for ( TInt j = 0; j < (aCerts.Count()); j++  )
            {
            caCertIndex=j;
            if ( i!=caCertIndex )
                {
                const CX500DistinguishedName& issuerNameStr=*issuerArray->At(j);
                if ( issuerNameStr.ExactMatchL(*subjectArray->At(i)) )
                    userCertIndex=i+1;
                }
            }
        if ( userCertIndex == i && i!=caCertIndex )
            break;
        userCertIndex=i;
        }
    CleanupStack::PopAndDestroy(subjectArray);
    CleanupStack::PopAndDestroy(issuerArray);
    CX509Certificate* currCert = aCerts.At(userCertIndex);
    TInt currCertIndex=0; 
    realUserCert=currCert;
    //Verify chain and return highest CA
    while ( currCertIndex < aCerts.Count())
        {
        CX509Certificate* trustedCaCert  = IkePkiUtils::FindCaCertificateL(*currCert, aTrustedCAList);
        if ( trustedCaCert )
            return currCert;
        else
            {
            for ( TInt j = 0; j < aCerts.Count(); j++ )
                {
                currCaCert = aCerts.At(j);
                if ( currCert->IssuerName().ExactMatchL(currCaCert->SubjectName()))
                    {
                    certOk = IkePkiUtils::VerifyX509CertificateL(*currCaCert, *currCert);
                    if ( certOk )
                        break;
                    }
                }       
            if ( !certOk  ) //if chain is malicious break and return NULL
                break;
            currCertIndex++;
            currCert=currCaCert;
            }
        }
        return NULL;
    }
