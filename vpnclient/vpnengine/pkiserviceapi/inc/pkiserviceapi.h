/*
* Copyright (c) 2003-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   PKI Service API
*
*/



/**
 *  $file pkiserviceapi.h
 *  
 *  PKI server API module
 *
 */

#ifndef __PKISERVICEAPI_H__
#define __PKISERVICEAPI_H__

#include "pkidefs.h"

/** 
 *  @mainpage PKI Service API
 * 
 *  @section intro Overview
 *
 *  PKI Service API is an interface to a module called PKI Service.
 *  PKI Service is responsible of maintaining PKI keys and certificates
 *  and it provides a set of operations addressed to these objects, such as:
 *
 *  <ul>
 *  <li>Save keypair</li>
 *  <li>Create keypair</li>
 *  <li>Save certificate</li>
 *  <li>Attach enrolled certificate to a generated key</li>
 *  <li>Sign using specified key</li>
 *  <li>Decrypt using specified key</li>
 *  <li>Read public key of a generated key</li>
 *  <li>Read certificate</li>
 *  <li>Remove keypaiR</li>
 *  <li>Remove certificate</li>
 *  <li>Build PKCS#10 certificate enrollment request for a generated key</li>
 *  </ul>
 *
 *  PKI Service is implemented upon the Symbian Crypto Token Framework (CTF).
 *  CFT concept supports different types of stores to hold the PKI tokens.
 *  Main store types are: file store and WIM based store.
 *
 *  Only one asynchronous operation can be pending at any time. KPKIErrServiceBusy status code will be returned,
 *  if any function except CancelPendingOperation is called.
*/

/**
 * PKI Service API
 *
 * The API follows the standard Symbian OS client-server
 * programming patterns.
 */
class RPKIServiceAPI:public RSessionBase
{
    /**
    @internalComponent
    */
    public:
        /**
         * Constructor
         */
        IMPORT_C RPKIServiceAPI(void);
        /**
         * Opens a connection (session) to the PKI server.
         *
         * @return KErrNone if the connection succeeds, a system-wide error code
         * if not.
         */
        IMPORT_C TInt Connect();
        /**
         * Closes the connection (session) to the PKI server.
         */
        IMPORT_C void Close();
        
        /**
        * Lock keystore
        * PIN code must be given again to unlock the keystore
        *
        * @param aRequestStatus [out] A reference to the request status object. On
        * request completion, contains the return code of the request.
        *
        */
        IMPORT_C void Logoff(TRequestStatus& aRequestStatus);

        /**
        * All operations referencing to private keys require a PIN,
        * which protects the private key storage.
        * The given PIN is valid until the service is terminated.
        * By giving the PIN using the Logon function, you can avoid
        * PIN dialogs during other private key operations.
        *
        * @param aRequestStatus [out] A reference to the request status object. On
        * request completion, contains the return code of the request.
        *
        */
        IMPORT_C void Logon(TRequestStatus& aRequestStatus);

        /**
        * Cancel the latest asynchronous operation
        *
        * @return Synchronous general error code
        */
        IMPORT_C TInt CancelPendingOperation();

        /**
        * This function returns the required buffer size for the operation
        * which has failed with error code KPKIErrBufferTooShort
        *
        * @return Synchronous general error code
        */
        IMPORT_C TInt GetRequiredBufferSize(TInt &aSize);

        /**
        * Change existing PIN value. Decrypts the key store using the old PIN
        * and encrypts the contents with the new PIN
        *
        * @param aRequestStatus [out] A reference to the request status object. On
        * request completion, contains the return code of the request.
        *
        */
        IMPORT_C void ChangePassword(TRequestStatus& aRequestStatus);

        /**
        * Sign using key specified by given SHA1 hash
        *
        * @param aKeyId Idenfies the key used in signing
        * @param aHashIn The data to be signed
        * @param aSignature [out] The result of the signing operation       
        */
        IMPORT_C TInt Sign(const TPKIKeyIdentifier& aKeyId,
                           const TDesC8& aHashIn, TDes8& aSignature) const;
        
        /**
        * Sign using the key specified by arguments of the corresponding user certificate
        *
        * @param aTrustedAuthority Text or ASN1 format subtree name of the CA
        * @param aIdentitySubjectName Text or ASN1 format string representing part of the subject name of the certificate.
        * @param aIdentityRfc822Name Text format string representing rfc822Name in subjectAltName.
        * @param aKeyUsage Usage bits of the certified key. Use 'OR' to build a required bit combination.
        * @param aKeySize Size of the key in bits.
        * @param aKeyAlgorithm Algorithm of the key.
        * @param aHashIn Data to be signed.
        * @param aSignature [out] Returned signature value.        
        *
        */
        IMPORT_C TInt Sign(const TDesC8& aTrustedAuthority,
                           const TDesC8& aIdentitySubjectName,
                           const TDesC8& aIdentityRfc822Name,
                           const TPKIKeyUsage aKeyUsage,
                           const TUint aKeySize,
                           const TPKIKeyAlgorithm aKeyAlgorithm,
                           const TDesC8& aHashIn,
                           TDes8& aSignature) const;

        /**
        * Decrypt using the key specified by given SHA1 hash
        *
        * @param aKeyId Idenfies the key used in decrypting
        * @param aDataIn The data to be decrypted
        * @param aDataOut [out] The result of the decryption operation
        */
        IMPORT_C TInt Decrypt(const TPKIKeyIdentifier& aKeyId,
                              const TDesC8& aDataIn, TDes8& aDataOut) const;

        /**
        * Store keypair having listed characteristics, returns keyId (SHA1 hash of the key)
        *
        * @param aKeyId [out] Returned keyId
        * @param aKeyDataIn A descriptor of the buffer containing keypair bytes (ASN1 or encrypted PKCS#5 format).
        * @param aRequestStatus [out] A reference to the request status object. On
        * request completion, contains the return code of the request.
        */
        IMPORT_C void StoreKeypair(TPKIKeyIdentifier& aKeyId,
                                   const TDesC8& aKeyDataIn,
                                   TRequestStatus& aRequestStatus);

        /**
        * Generate keypair having given characteristics
        *
        * @param aKeyId [out] Returned keyId
        * @param aKeySize Size of the key in bits.
        * @param aKeyAlgorithm Algorithm of the key.
        * @param aRequestStatus [out] A reference to the request status object. On
        * request completion, contains the return code of the request.
        */
        IMPORT_C void GenerateKeypair(TPKIKeyIdentifier& aKeyId,
                                      const TUint aKeySize,
                                      const TPKIKeyAlgorithm aKeyAlgorithm,
                                      TRequestStatus& aRequestStatus);
        
        /**
        * Read public key of a generated keypair
        *
        * @param aKeyId Idenfies the key used.
        * @param aDataOut [out] Returned public key bytes in ASN1 format.
        */
        IMPORT_C TInt ReadPublicKey(const TPKIKeyIdentifier& aKeyId,
                                    TDes8& aDataOut) const;
        
        /**
        * Read a certificate having listed characteristics
        * For CA certificates, only aTrustedAuthority, aOwnerType and aCert parameters meaningful.
        * @param aTrustedAuthority Text or ASN1 format subtree name of the CA
        * @param aIdentitySubjectName Text or ASN1 format string representing part of the subject name of the certificate.
        * @param aIdentityRfc822Name Text format string representing rfc822Name in subjectAltName.
        * @param aOwnerType Type of the requested certificate.
        * @param aKeySize Size of the key in bits.
        * @param aKeyAlgorithm Algorithm of the key.
        * @param aCert [out] Returned ASN1 encoded certificate.
        * @param aResArray [out] This returned object must be given as a parameter in the Finalize call when this function has completed.
        * @param aRequestStatus [out] A reference to the request status object. On
        * request completion, contains the return code of the request.
        */
        IMPORT_C void ReadCertificateL(const TDesC8& aTrustedAuthority,
                                       const TDesC8& aIdentitySubjectName,
                                       const TDesC8& aIdentityRfc822Name,
                                       const TPKICertificateOwnerType aOwnerType,
                                       const TUint aKeySize,
                                       const TPKIKeyAlgorithm aKeyAlgorithm,
                                       TDes8 &aCert,
                                       TAny **aResArray,  
                                       TRequestStatus& aRequestStatus);
        
        /**
        * Read a certificate having listed characteristics
        * @param aTrustedAuthority Text or ASN1 format subtree name of the CA
        * @param aSerialNumber serial number of the certificate
        * @param aCert [out] Returned ASN1 encoded certificate.
        */
        IMPORT_C TInt ReadCertificate(const TDesC8& aTrustedAuthority,
                                       const TDesC8& aSerialNumber,
                                       TDes8 &aCert);

        /**
        * Read a certificate having listed characteristics
        * @param aKeyId SubjectKeyID.
        * @param aCert [out] Returned ASN1 encoded certificate.
        * @param aResArray [out] This returned object must be given as a parameter in the Finalize call when this function has completed.
        * @param aRequestStatus [out] A reference to the request status object. On
        * request completion, contains the return code of the request.
        */
        IMPORT_C void ReadCertificateL(const TPKIKeyIdentifier& aKeyId,
                                       TDes8 &aCert,
                                       TAny **aResArray,
                                       TRequestStatus& aRequestStatus);
                                       
        /**
        * List all certificates  
        * @param aCertList [out] Returned list of certificates.
        */
		IMPORT_C void ListCertificatesL(CArrayFix<TCertificateListEntry> *&aCertList);

		/**
        * List CA certificates applicable for all requested applications  
        * @param aApplications List of applications
        * @param aCertList [out] Returned list of certificates.
        */
		IMPORT_C void ListApplicableCertificatesL(const RArray<TUid>& aApplications,
										        CArrayFix<TCertificateListEntry> *&aCertList);

        /**
        * List all keys. Returns list of all keys stored in the device.
        *
        * @param aKeyList [out] Returned list of keys.
        */
        IMPORT_C void ListKeysL(CArrayFix<TKeyListEntry> *&aKeyList);

                                            
        /**
        * Store a certificate having listed characteristics
        * For CA certificates, only aOwnerType and aDataIn parameters are meaningful.
        *
        * @param aOwnerType Type of the certificate.
        * @param aKeySize Size of the key in bits.
        * @param aKeyAlgorithm Algorithm of the key.
        * @param aDataIn A descriptor of the buffer conatining ASN1 coded certificate bytes.
        * @param aResArray [out] This returned object must be given as a parameter in the Finalize call when this function has completed.
        * @param aRequestStatus [out] A reference to the request status object. On
        * request completion, contains the return code of the request.
        */
        IMPORT_C void StoreCertificateL(const TPKICertificateOwnerType aOwnerType,
                                        const TUint aKeySize,
                                        const TPKIKeyAlgorithm aKeyAlgorithm,
                                        const TDesC8& aDataIn,
                                        TAny **aResArray,
                                        TRequestStatus& aRequestStatus);
        
        /**
        * Store a certificate having listed characteristics
        * For CA certificates, only aOwnerType and aDataIn parameters are meaningful.
        *
        * @param aOwnerType Type of the certificate.
        * @param aIsDeletable true if certificate is deletable
        * @param aKeySize Size of the key in bits.
        * @param aKeyAlgorithm Algorithm of the key.
        * @param aDataIn A descriptor of the buffer conatining ASN1 coded certificate bytes.
        */
        IMPORT_C TInt StoreCertificate(const TPKICertificateOwnerType aOwnerType,
									   const TBool& aIsDeletable,
                                       const TUint aKeySize,
                                       const TPKIKeyAlgorithm aKeyAlgorithm,
                                       const TDesC8& aDataIn) const;

        /**
        * Attach a user certificate having listed characteristics to a generated key identified by keyId
        *
        * @param aKeyId Idenfies the key to attach the certificate
        * @param aKeySize Size of the key in bits.
        * @param aKeyAlgorithm Algorithm of the key.
        * @param aDataIn A descriptor of the buffer conatining ASN1 coded certificate bytes.
        * @param aResArray [out] This returned object must be given as a parameter in the Finalize call when this function has completed.
        * @param aRequestStatus [out] A reference to the request status object. On
        * request completion, contains the return code of the request.
        */
        IMPORT_C void AttachCertificateL(const TPKIKeyIdentifier& aKeyId,
                                         const TUint aKeySize,
                                         const TPKIKeyAlgorithm aKeyAlgorithm,
                                         const TDesC8& aDataIn,
                                         TAny **aResArray,
                                         TRequestStatus& aRequestStatus);
        
        /**
        * Attach a user certificate having listed characteristics to a generated key identified by keyId
        *
        * @param aKeyId Idenfies the key to attach the certificate
        * @param aIsDeletable true if certificate is deletable
        * @param aKeySize Size of the key in bits.
        * @param aKeyAlgorithm Algorithm of the key.
        * @param aDataIn A descriptor of the buffer conatining ASN1 coded certificate bytes.
        */
        IMPORT_C TInt AttachCertificate(const TPKIKeyIdentifier& aKeyId,
										const TBool& aIsDeletable,
                                        const TUint aKeySize,
                                        const TPKIKeyAlgorithm aKeyAlgorithm,
                                        const TDesC8& aDataIn) const;

        /**
        * Remove keypair identified by keyId
        *
        * @param aKeyId Idenfies the key used
        */
        IMPORT_C TInt RemoveKeypair(const TPKIKeyIdentifier& aKeyId) const;                                              

        /**
        * Remove certificate identified by listed characteristics
        * For CA certificates, only aTrustedAuthority and aOwnerType parameters meaningful.
        *
        * @param aTrustedAuthority Text or ASN1 format subtree name of the CA
        * @param aSerialNumber serial number of the certificate
        */
        IMPORT_C TInt RemoveCertificate(const TDesC8& aTrustedAuthority,
                                         const TDesC8& aSerialNumber) const;

        
        /**
        * Creates and saves a certificate request
        *
        * @param aKeyId KeyId of the key for which the certificate will be generated  
        * @param aSubjectName Subject name of the certificate owner
        * @param aSubjectAltName SubjectAlt name of the certificate owner
        * @param aChallengePw ChallengePw of the certificate owner
        * @param aDNSName DNS name of the certificate owner
        * @param aCertRequestRef Identifier of the returned certificate request
        * @param arequestLength [out] Length of the ertificate request
        */
        IMPORT_C void CreateAndSaveCertificateRequestL(const TPKIKeyIdentifier& aKeyId,
                                                       const TDesC8& aSubjectName,
                                                       const TDesC8& aSubjectAltNameRfc822,       
                                                       const TDesC8& aChallengePw,       
                                                       const TDesC8& aDNSName,
                                                       TDes& aCertRequestRef,
                                                       TInt& aRequestLength);
        
        /**
        * Reads a certificate request
        *
        * @param aCertRequestRef Identifier of the certificate request
        * @param aCertRequest [out] Certificate request data
        */
        IMPORT_C TInt ReadCertificateRequest(const TDesC& aCertRequestRef,
                                             TDes8& aCertRequest) const;
        
        /**
        * Deletes a certificate request
        *
        * @param aCertRequestRef Identifier of the certificate request
        */
        IMPORT_C TInt DeleteCertificateRequest(const TDesC& aCertRequestRef) const;
        
        /**
        * ReleaseResources. Must be called every time when an asyncronous request has completed and synchronously returned TAny **aResArray.
        *
        * @param aResObject Object pointer returned as a result in an earlier asynchronous operation.
        *
        */
        IMPORT_C void Finalize(TAny *aResObject);

        /**
        * Change trust setting of a certificate in Symbian certificate store
        *
        * @param aTrustedAuthority Text or ASN1 format subtree name of the CA
        * @param aSerialNumber serial number of the certificate
        * @param aTrusted ETrue, if trusted; EFalse, otherwise
        */
        IMPORT_C TInt SetTrust(const TDesC8& aTrustedAuthority,
                               const TDesC8& aSerialNumber,
                               const TBool& aTrusted) const;

        /**
        * Read trust setting of a certificate in Symbian certificate store
        *
        * @param aTrustedAuthority Text or ASN1 format subtree name of the CA
        * @param aSerialNumber serial number of the certificate
        * @param aTrusted returns ETrue, if trusted; EFalse, otherwise
        */
        IMPORT_C TInt Trusted(const TDesC8& aTrustedAuthority,
                              const TDesC8& aSerialNumber,
                              TBool& aTrusted) const;
        
        /**
        * Set applications of a certificate in Symbian certificate store
        *
        * @param aTrustedAuthority Text or ASN1 format subtree name of the CA
        * @param aSerialNumber serial number of the certificate
        * @param aApplications list of applications (uids) for whom the certificate is applicable
        */
        IMPORT_C void SetApplicabilityL(const TDesC8& aTrustedAuthority,
                                        const TDesC8& aSerialNumber,
                                        const RArray<TUid>& aApplications) const;

        /**
        * Get applications of a certificate in Symbian certificate store
        *
        * @param aTrustedAuthority Text or ASN1 format subtree name of the CA
        * @param aSerialNumber serial number of the certificate
        * @param aApplications list of applications (uids) for whom the certificate is applicable
        */
        IMPORT_C void ApplicationsL(const TDesC8& aTrustedAuthority,
                                    const TDesC8& aSerialNumber,
                                    RArray<TUid>& aApplications) const;

        /**
        * Get certificate details of a certificate in Symbian certificate store
        *
        * @param aKeyId SubjectKeyID
        * @param aCertDetails details of a certificate
        */
        IMPORT_C TInt CertificateDetails(const TDesC8& aTrustedAuthority,
                                         const TDesC8& aSerialNumber,
                                         TCertificateListEntry &aCertDetails) const;

		/**
        * Get details of a key.
        * This method searches the key from both the User store 
        * and device cert store.
        *
        * @param aKeyId SubjectKeyID
        * @param aKeyDetails [out] Returned details of a key.
        *
        * @return KErrNone if no error occured or an error code.
        */
		IMPORT_C TInt KeyDetails(const TPKIKeyIdentifier& aKeyId,
								  TKeyListEntry &aKeyDetails) const;


		
        /**
        * List all certificate request  
        * @param aCertReqList [out] Returned list of certificates.
        */
        IMPORT_C void ListCertificateRequestsL(CArrayFix<TCertificateRequestListEntry> *&aCertReqList) const;
        
        /**
        * Specify which certificate store to use for keystore AND certstore operations within this
        * PKI session. This setting can be changed at any time, and all subsequent operations will
        * use the specified store type until the state is changed again.
        *
        * If SetStoreType is not called, then both cert store and key store are set to type
        * STORETYPE_USER.
        *
        * Options are:
        * STORETYPE_DEVICE: Use device keystore / certstore (will not prompt for password)
        * STORETYPE_USER: Use user keystore / certstore (will prompt for password)
        * STORETYPE_ANY: Use device and user keystore / certstore
        * 
        * @param aStoreType Desired store type for all consequent operations on both 
        *                   certificate store and key store. Supported values:
        *                   STORETYPE_DEVICE, STORETYPE_USER, STORETYPE_ANY.
        *
        * @return KErrNone iff store type was changed successfully.
        */
        IMPORT_C TInt SetStoreType(TPkiServiceStoreType aStoreType) const;

        /**
        * Specify the store type for EITHER the keystore OR the certstore.
        *
        * @param aStore Either STORE_KEYSTORE or STORE_CERTSTORE.
        * @param aStoreType Desired store type for all operations on the specified store. 
        *                   Supported values: STORETYPE_DEVICE, STORETYPE_USER, STORETYPE_ANY.
        *
        * @return KErrNone iff store type was changed successfully.
        */
        IMPORT_C TInt SetStoreType(TInt aStore, TPkiServiceStoreType aStoreType) const;
        
        /**
         * Get cert store type.
         *
         * @param aStoreType [out] Either STORETYPE_DEVICE, STORETYPE_USER or STORETYPE_ANY.
         * 
         * @return KErrNone iff store type supported
         */
         IMPORT_C TInt CertStoreType(TPkiServiceStoreType& aStoreType) const;
         
         /**
          * Get key store type.
          *
          * @param aStoreType [out] Either STORETYPE_DEVICE, STORETYPE_USER or STORETYPE_ANY.
          * 
          * @return KErrNone iff store type supported
          */
          IMPORT_C TInt KeyStoreType(TPkiServiceStoreType& aStoreType) const;
         
         /**
        * Makes every subsequent certificate-related query an informational one.
        * Informational, in this context, means that even expired certificates
        * turn up in queries / searches. This should only be set when the certificate
        * isn't used for any functional purpose -- i.e. only when using the certificate 
        * data to display certificate details on VPN UI Policy Details view.
        *
        * @param aInfoOnly If ETrue, all subsequent queries will produce results 
        *                  that include expired certificates; if EFalse, only
        *                  temporally valid certificates will be included.
        */
        IMPORT_C void SetInformational(const TBool aInfoOnly);

        
    private:
        static TBool Pkcs10SignCallbackL(const TDesC8& aDigest, TDes8& aSignature, 
                                         const TPKIKeyIdentifier& aKeyId, 
                                         TAny* aContext);        
};

#endif