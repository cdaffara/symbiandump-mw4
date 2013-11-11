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
* Description:   static method for certficate handling
*
*/




#ifndef IKEPKIUTILS_H
#define IKEPKIUTILS_H

#include <e32base.h>

class CX509Certificate;
class TCertPayloadIkev2;
class CIkeCaList;
class TCertificateISAKMP;
class TASN1DecGeneric;

class IkePkiUtils    
    {
public:

    /*
    * Verifies an IKEv2 signature with a public of
    * specified certificate
    * 
    * @param aSignature Signature data verified.
    * @param aAuthData Data signed.
    * @param aCert Certificate which contains the public key used for verification      
    * @result ETrue if signature is vefified OK, otherwise EFalse
    */
	IMPORT_C static TBool VerifyIkev2SignatureL(const TDesC8& aSignature, 
	                                            const TDesC8& aRefHash, 
	                                            const CX509Certificate& aCerts);
	         
	         
    /*    
    * Verifies an IKEv1 signature with a public of
    * specified certificate
    *    
    * @param aSignature Signature data verified.
    * @param aRefHash Hash data which has been signed (= Encrypted with private key)
    * @param aCert Certificate which contains the public key used for verification      
    * @result ETrue if signature is vefified OK, otherwise EFalse
    */	                                            
	IMPORT_C static TBool VerifyIkev1SignatureL(const TDesC8& aSignature, 
	                                            const TDesC8& aRefHash, 
	                                            const CX509Certificate& aCerts);

    /*
    * Gets IKE DER encoded Subject Name from the certificate and
    * copy it into specified buffer 
    * 
    * @param aCert Certificate object pointer
    * @param aSubjectName Reference to the subject name (returned)
    * @result ETrue if operation succeeded, otherwise EFalse
    */
    IMPORT_C static TBool GetCertSubjectNameDERL(const CX509Certificate* aCert, 
                                                 TDes8& aSubjectName);
                                                 
    /*
    * Gets IKE identity data from specified certificate
    * 
    * @param aIdType Specified IKE Identity type wanted.
    * @param aCertData Certificate data from where the specified Identity data is taken
    * @result The identity data buffer of NULL
    */
    IMPORT_C static HBufC8* GetIdentityFromCertL(TUint8 aIdType, const TDesC8& aCertData);
    
    
    /*
    * Ccertifies a specified IKE identity with
    * Certificate Identities
    *
    * @param aCert Verified user certificate
    * @param aId IKE identity data used by peer
    * @param aIdType IKE identity type 
    * @result ETrue if Identity is certified otherwise EFalse
    */
    IMPORT_C static TBool CertifyIdentityL(const CX509Certificate* aCert, 
                                           TDesC8& aId, TInt aIdType);


    /*
    * Verifies the user Certificate used by the IKE peer.
    * 
    * @param aCerts The array of IKE certificate payload(s) from where verified
    *               Certificate is found
    * @result A pointer to a verified X509 certificate or NULL 
    */
    IMPORT_C static CX509Certificate* VerifyCertificateL(const CArrayFixFlat<TCertPayloadIkev2*>& aCerts,
                                                         const CIkeCaList& aTrustedCAList);


    /*
    * Verifies the user Certificate used by the IKE peer.
    * 
    * @param aCerts The array of IKE certificate payload(s) from where verified
    *               Certificate is found
    * @result A pointer to a verified X509 certificate or NULL 
    */
    IMPORT_C static CX509Certificate* VerifyCertificateL(const CArrayFixFlat<const TCertificateISAKMP*>& aCerts,
                                                         const CIkeCaList& aTrustedCAList);

    IMPORT_C static CX509Certificate* VerifyCertChainL(const CArrayFixFlat<CX509Certificate*>& aCerts, CX509Certificate*& realUserCert,const CIkeCaList& aTrustedCAList);
    
private:
    static TBool VerifySignatureL(const TInt aIkeVersion, 
                                  const TDesC8& aSignature, 
                                  const TDesC8& aRefHash, 
                                  const CX509Certificate& aCert);     
     
    static CArrayPtrFlat<TASN1DecGeneric>* DecodeDERL(const TDesC8& aPtr, TInt& aPosition);     
    
  	/*
	* Verifies user certificate by taking the following actions:
	* -- Verify the signature of the user certificate with the public
	*    key of the CA certificate.
	* -- Check the validity of the user certificate 
	* -- Check that extension field of the user certificate are correct
	*/
    static CX509Certificate* VerifyX509CertificateL(const CX509Certificate& aCaCert, 
                                                    CX509Certificate& aCert);


    static CX509Certificate* FindCaCertificateL(const CX509Certificate& aUserCert, 
                                                const CIkeCaList& aTrustedCAList);
                                                
    static const CArrayFixFlat<TCertPayloadIkev2*>* CastCertArray(const CArrayFixFlat<const TCertificateISAKMP*>* aCerts);                                                

    
    
    
    };

#endif //IKEPKIUTILS_H
