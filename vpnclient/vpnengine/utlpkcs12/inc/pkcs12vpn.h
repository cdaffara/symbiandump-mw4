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
* Description:   PKCS#12 data handler
*
*/



#ifndef __PKCS12VPN_H__
#define __PKCS12VPN_H__

#include <f32file.h>

#include "pkiserviceapi.h"

#include "pkidefs.h"
#include "vpnnotifierdefs.h"


// Used locally within PKCS#12 handler
#define DEFAULT_KEY_LEN 1024

// Filename stems used for security object dumping
// (%d will range from 1 to n)
_LIT(KCAFileNameStem, "ca-%d.cer");
_LIT(KUserCertFileNameStem, "user-%d.cer");
_LIT(KPrivateKeyFileNameStem, "user-%d.key");

// Forward declarations
class CPKCS12;
class MPKCS12;
class CX509Certificate;

/**
 * Class CPKCS12Handler provides basic PKCS12 handling functionality. 
 * This includes operations for password prompting, integrity verification 
 * and data decrypting. CPKCS12Handler also manages PKI operations related 
 * to PKCS#12 package contents (storing key data, certificate attachment).
 *
 */
class CPKCS12Handler : public CBase
    {

/////////////////////////////
// Construction / Destruction
/////////////////////////////
public:

    /**
     * CPKCS12Handler must always be instantiated with a reference to
     * a (connected) PKI Service API resource handle.
     * @param aPkiServiceApi Reference to an instantiated RPKIServiceAPI
     *                       object.
     */
    IMPORT_C static CPKCS12Handler* NewL(RPKIServiceAPI& aPkiServiceApi);
    IMPORT_C static CPKCS12Handler* NewLC(RPKIServiceAPI& aPkiServiceApi);
    IMPORT_C static CPKCS12Handler* NewL();
    IMPORT_C static CPKCS12Handler* NewLC();

    EXPORT_C ~CPKCS12Handler();
    
protected:

    void ConstructL();

private:

    CPKCS12Handler(RPKIServiceAPI& aPkiServiceApi);



/////////////////////////////
// Public functions
/////////////////////////////
public:

    /**
     * High-level function for storing all certificates/private keys 
     * contained in a single PKCS#12 bundle. Also sets the applicability
     * and deletable properties, which must be set prior to storing
     * if non-default values are required.
     *
     * Stores the data directly to PKI.
     *
     * @param aData PKCS#12 in raw binary data format
     * @param aPwd The password for decoding the PKCS#12 package.
     *
     * @return Leaves with KErrNotSupported if aData is not a valid PKCS#12
     *         buffer; KErrPermissionDenied if the password is wrong.
     *         Symbian standard leaves can occur as well (KErrNoMemory)
     */
    IMPORT_C void StorePKCS12ObjectL(const TDesC8& aData, 
                                     const TDesC16& aPwd);

    /**
     * Gets a locally applicable reference to applicability data that will 
     * be applied to all applicable certificates deciphered from the 
     * PKCS#12 package by the application.
     * "Lazy" functionality: applicability parameter is only applied to
     * a certificate when the actual certificate is stored (attached).
     * In other words, SetApplicability must be called before StorePKCS12ObjectL().
     * Has no effect for PKCS#12 file dump (SaveSecurityObjectsToDiskL()).
     *
     * @param aUids Array of TUid objects, indicating the applicability
     *              scope of the certificate(s). Ownership doesn't transfer.
     * 
     */
    IMPORT_C void SetApplicability(const RArray<TUid>& aUids);
    
    /**
     * Sets the deletable attribute that will be applied to all
     * certificates within the package.
     * "Lazy" functionality: deletable parameter is only applied to
     * a certificate when the actual certificate is stored (attached).
     * In other words, SetDeletable must be called before StorePKCS12ObjectL().
     * Has no effect for PKCS#12 file dump (SaveSecurityObjectsToDiskL()).
     *
     * @param aDeletable ETrue, if the certificate(s) can be deleted
     *                   from the certificate store.
     *
     */
    IMPORT_C void SetDeletable(TBool aDeletable);
    
    /**
     * Saves the keys/certs to disk. It's important that the files be 
     * deleted afterwards by the caller.
     *
     * @param aData PKCS#12 data in raw binary
     * @param aPwd PKCS#12 password or KNullDesC
     * @param aDir The target directory for extraction operation
     *
     * @leave May leave if write ops fail
     *
     */
    IMPORT_C void SaveSecurityObjectsToDiskL(const TDesC8& aData, 
                                             const TDesC& aPwd,
                                             const TDesC& aDir);



/////////////////////////////
// Private functions
/////////////////////////////
private:

    /**
     * Default constructor not available for public
     */
    CPKCS12Handler();

    /**
     * Make sure the given data is in PKCS#12 format.
     *
     * @param aData Binary data buffer containing the PKCS#12 data
     *
     * @return Returns ETrue if the data represents a valid PKCS#12 object,
     *         EFalse otherwise
     */
    TBool VerifyType(const TDesC8& aData) const;

    /**
     * Stores all the private keys from PKCS#12 object to PKI
     *
     */
    void StoreKeyPairsL();
    
    /**
     * Store a single private key to PKI
     *
     * @param aKey The private key in binary format.
     */
    void StoreSingleKeyL(const TDesC8& aKey);

    /**
     * Attach all the user certificates contained in the PKCS#12 
     * package
     *
     */
    void AttachCertificatesL();

    /**
     * Store all the CA certificates contained in the PKCS#12 
     * package
     *
     */
    void StoreCertificatesL();

    /**
     * Store a single CA certificate
     *
     */
    void StoreCertificateL(CX509Certificate* aCert);

    /**
     * Attach given certificate to a private key, based on key id
     * 
     * @param aCert The certificate to be attached
     * @param aKeyId The ID of the associated private key
     *
     */
    void AttachCertificateL(CX509Certificate* aCert, 
                            const TPKIKeyIdentifier& aKeyId);
    
    /**
     * Convenience function for getting local references to the certs/keys
     * contained in the PKCS#12
     *
     */
    void ExtractKeysAndCerts();
    

    /**
     * Prompt for PKCS#12 package password.
     * @return The password. Memory control is transferred to the caller.
     */
    HBufC* QueryPasswordL();
    
    /**
     * Performs PKI applicability set operation for the given
     * certificate. Locally stored applicability values will be used. 
     * If no applicability values have been given, defaults to none.
     *
     * @param aCert The certificate for which the 
     * 
     */
    void SetApplicabilityL(CX509Certificate* aCert);

    /**
     * Performs PKI deletable set operation for the given
     * certificate. Locally stored deletable value will be used.
     * If the value hasn't been set, defaults to ETrue (deletable).
     *
     * @param aCert The certificate for which the 
     * 
     */
    void SetDeletable(CX509Certificate* aCert);
    
    /**
     * Pops the "wrong password" note on screen.
     */
    void DisplayWrongPasswordNote();

    /**
     * Extracts the security objects from PKCS#12 bundle to 
     * memory.
     *
     * @param aData the PKCS#12 raw data
     * @param aPwd the PKCS#12 password or KNullDesC
     *
     * @leave May leave due to cancel (KErrCancel), 
     *        due to corrupted PKCS#12 data (KErrNotSupported),
     *        due to OOM, ...
     *
     */
    void ExtractSecurityObjectsL(const TDesC8& aData, 
                                const TDesC16& aPwd);

    /**
     * Dumps all CA and user certificates from memory to local file system
     * 
     * @leave May leave if write ops fail, or for the usual reasons (OOM etc)
     */
    void SaveCertificatesToDiskL();

    /**
     * Dumps all private keys to local file system.
     *
     * @leave May leave if write ops fail 
     *
     */
    void SaveKeysToDiskL();
    
    /**
     * Writes the given binary data to a file
     *
     * @leave May leave if write fails
     *
     */
    void WriteToFileL(const TDesC8& aData, const TDesC& aFileName);

/////////////////////////////
// Private data
/////////////////////////////
private:

    RPKIServiceAPI*                         iPkiService; // ref
    MPKCS12*                                iPkcsHandler;

    const CArrayPtr<CX509Certificate>*      iCACerts;       // ref
    const CArrayPtr<CX509Certificate>*      iUserCerts;     // ref
    const CArrayPtr<HBufC8>*                iPrivKeys;      // ref
    
    HBufC*                                  iPassword;

    TBool                                   iDeletable;
    const RArray<TUid>*                     iApplications; // ref
    
	RNotifier                               iNotifier;

    TPckgBuf<TVpnDialogInfo>                iDialogInfoDes;
    TPckgBuf<TVpnDialogOutput>              iDialogResponseDes;
    
    HBufC*                                  iOutputDir;

    RFs                                     iFs;
    RFile                                   iFileOut;
    };
    
#endif

