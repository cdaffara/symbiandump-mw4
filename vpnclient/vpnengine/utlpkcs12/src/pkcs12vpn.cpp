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



#include <x509cert.h>
#include <mpkcs12.h>

#include <e32std.h>

#include <securityerr.h>
#include <sysutil.h>

#include "pkcs12vpn.h"
#include "logvpncommon.h"
#include "vpnnotifierdefs.h"

///////////////////////////////////
// Construction and destruction

EXPORT_C CPKCS12Handler* CPKCS12Handler::NewL(RPKIServiceAPI& aPkiServiceApi)
    {
    LOG_("-> CPKCS12Handler::NewL()");
    CPKCS12Handler* self = NewLC(aPkiServiceApi);
    CleanupStack::Pop(self);
    LOG_("<- CPKCS12Handler::NewL()");
    return self;
    }

EXPORT_C CPKCS12Handler* CPKCS12Handler::NewLC(RPKIServiceAPI& aPkiServiceApi)
    {
    LOG_("-> CPKCS12Handler::NewLC()");
    CPKCS12Handler* self = new (ELeave) CPKCS12Handler(aPkiServiceApi);
    CleanupStack::PushL(self);
    self->ConstructL();
    LOG_("<- CPKCS12Handler::NewLC()");
    return self;
    }


EXPORT_C CPKCS12Handler* CPKCS12Handler::NewL()
    {
    LOG_("-> CPKCS12Handler::NewL(empty)");
    CPKCS12Handler* self = NewLC();
    CleanupStack::Pop(self);
    LOG_("<- CPKCS12Handler::NewL(empty)");
    return self;
    }

EXPORT_C CPKCS12Handler* CPKCS12Handler::NewLC()
    {
    LOG_("-> CPKCS12Handler::NewLC(empty)");
    CPKCS12Handler* self = new (ELeave) CPKCS12Handler();
    CleanupStack::PushL(self);
    self->ConstructL();
    LOG_("<- CPKCS12Handler::NewLC(empty)");
    return self;
    }

/**
 * Release resources.
 * Note: CPKCS12Handler's parent owns the RPKIServiceAPI instance
 */
EXPORT_C CPKCS12Handler::~CPKCS12Handler() 
    {
    LOG_("-> CPKCS12Handler::~CPKCS12Handler()");
    iPkcsHandler->Release();
    iNotifier.Close();
    delete iPassword;
    delete iOutputDir;

    iFileOut.Close();
    iFs.Close();

    LOG_("<- CPKCS12Handler::~CPKCS12Handler()");
    }

/**
 * Maintain a reference to an instance of PKI API provided by the caller
 */
CPKCS12Handler::CPKCS12Handler(RPKIServiceAPI& aPkiServiceApi) : 
                iPkiService(&aPkiServiceApi), iDeletable(ETrue)
    {
    LOG_("-> CPKCS12Handler::CPKCS12Handler(RPKIServiceAPI&)");
    LOG_("<- CPKCS12Handler::CPKCS12Handler(RPKIServiceAPI&)");
    }

CPKCS12Handler::CPKCS12Handler()
    {
    LOG_("-> CPKCS12Handler::CPKCS12Handler()");
    LOG_("<- CPKCS12Handler::CPKCS12Handler()");
    }

/**
 * Instantiate S60 PKCS#12 handler upon construction.
 */
void CPKCS12Handler::ConstructL()
    {
    LOG_("-> CPKCS12Handler::ConstructL()");

    iPkcsHandler = PKCS12Factory::CreateL();
    User::LeaveIfError(iNotifier.Connect());
    User::LeaveIfError(iFs.Connect());

    LOG_("<- CPKCS12Handler::ConstructL() OK");
    }


///////////////////////////////////
// Public methods

EXPORT_C void CPKCS12Handler::SaveSecurityObjectsToDiskL(const TDesC8& aData,
                                                         const TDesC& aPwd,
                                                         const TDesC& aDir) 
    {
    LOG_("-> CPKCS12Handler::SaveSecurityObjectsToDiskL()");

    if (iOutputDir) 
        {
        delete iOutputDir;
        iOutputDir = NULL;
        }

    // Output dir needs to end with backslash
    iOutputDir = aDir.AllocL();

    LOG_1(" Using output dir: '%S'", iOutputDir);

    ExtractSecurityObjectsL(aData, aPwd);

    // Save CA/user certificates to disk
    SaveCertificatesToDiskL();

    // Save private keys to disk
    SaveKeysToDiskL();

    LOG_("<- CPKCS12Handler::SaveSecurityObjectsToDiskL()");
    }
EXPORT_C void CPKCS12Handler::StorePKCS12ObjectL(const TDesC8& aData, 
                                                 const TDesC16& aPwd) 
    {
    LOG_("-> CPKCS12Handler::StorePKCS12ObjectL()");

    ExtractSecurityObjectsL(aData, aPwd);

    // Store CA certificates to PKI
    StoreCertificatesL();

    // Store private keys to PKI
    StoreKeyPairsL();

    // Attach related user certificates to PKI priv keys
    AttachCertificatesL();

    LOG_("<- CPKCS12Handler::StorePKCS12ObjectL() OK");
    }

EXPORT_C void CPKCS12Handler::SetApplicability(const RArray<TUid>& aUids) 
    {
    LOG_("-> CPKCS12Handler::SetApplicability()");
    iApplications = &aUids;
    LOG_("<- CPKCS12Handler::SetApplicability()");
    }

EXPORT_C void CPKCS12Handler::SetDeletable(TBool aDeletable) 
    {
    LOG_1("-> CPKCS12Handler::SetDeletable() Deletable: %d", aDeletable);
    iDeletable = aDeletable;
    LOG_("<- CPKCS12Handler::SetDeletable()");
    }




///////////////////////////////////
// Internal methods

void CPKCS12Handler::ExtractSecurityObjectsL(const TDesC8& aData, 
                                             const TDesC16& aPwd) 
    {
    LOG_("-> CPKCS12Handler::ExtractSecurityObjectsL()");
    if (iPassword) 
        {
        delete iPassword;
        iPassword = NULL;
        }

    // Make sure the data is in PKCS#12 format
    if (!VerifyType(aData)) 
        {
        LOG_("<- CPKCS12Handler::ExtractSecurityObjectsL() LEAVE (KErrNotSupported)");
        User::Leave(KErrArgument);
        }
        
    // If we have been provided with a valid password, then proceed
    // to decrypt / parse; otherwise, prompt for pwd.
    if (aPwd.Length() > 0)
        {
        iPassword = aPwd.AllocL();
        LOG_(" Password provided by OMADM...");
        }
    else 
        {
        LOG_(" No password provided, prompting the user for one");

        iPassword = QueryPasswordL();
        }

    // Keep asking for the password until user cancels or inputs the
    // correct password
    while (ETrue) 
        {
        if (iPassword)
            {
            LOG_1(" Non-NULL password '%S' in use, decrypting", iPassword);
            TRAPD(err, iPkcsHandler->ParseL(aData, *iPassword));
            if (err != KErrNone) 
                {
                LOG_(" Breaking news: Password proved a miserable failure! Program terminated abruptly!");
                DisplayWrongPasswordNote();
                delete iPassword;
                iPassword = NULL;
                iPassword = QueryPasswordL();
                }
            else 
                {
                // Correct password provided by the user, 
                // break free from the vicious cycle.
                delete iPassword;
                iPassword = NULL;
                break;
                }
            }
        else
            {
            // User got tired of guessing and resorted to cancel
            LOG_("<- CPKCS12Handler::ExtractSecurityObjectsL() LEAVE (KErrCancel)");
            User::Leave(KErrBadPassphrase);
            }
        }

    // Fetch references to keys and certs
    ExtractKeysAndCerts();

    LOG_("<- CPKCS12Handler::ExtractSecurityObjectsL()");
    }

void CPKCS12Handler::SaveCertificatesToDiskL() 
    {
    LOG_("-> CPKCS12Handler::SaveCertificatesToDiskL()");

    TInt certCount(0);

    // first the CAs (if any; not required necessarily)
    if (iCACerts) 
        {
        certCount = iCACerts->Count();
        LOG_1(" Saving %d CA Certificates", certCount);

        for (TInt i = 0; i < certCount; i++)
            {
            CX509Certificate* cert = iCACerts->At(i);

            // Generate filename with running identifier
            // Use TFileName, because the function isn't stack
            // critical
            TFileName fn;
            fn.Format(KCAFileNameStem(), i+1);

            WriteToFileL(cert->Encoding(), fn);
            }
        }
    LOG(else LOG_(" No CA Certs found!"));

    // Then the user certs
    if (iUserCerts)
        {
        certCount = iUserCerts->Count();
        LOG_1(" Saving %d User Certificates", certCount);

        for (TInt i = 0; i < certCount; i++) 
            {
            CX509Certificate* cert = iUserCerts->At(i);

            TFileName fn;
            fn.Format(KUserCertFileNameStem(), i+1);

            WriteToFileL(cert->Encoding(), fn);
            }
        }

    LOG(else LOG_(" No User Certs found!"));

    LOG_("<- CPKCS12Handler::SaveCertificatesToDiskL()");
    }

void CPKCS12Handler::SaveKeysToDiskL() 
    {
    LOG_("-> CPKCS12Handler::SaveKeysToDiskL()");

    if (iPrivKeys) 
        {
        TInt keycount = iPrivKeys->Count();
        LOG_1(" Saving %d Private Keys", keycount);

        for (TInt i = 0; i < keycount; i++) 
            {
            HBufC8* key = iPrivKeys->At(i);
            TPtrC8 keyPtr = *key;

            TFileName fn;
            fn.Format(KPrivateKeyFileNameStem(), i+1);

            WriteToFileL(keyPtr, fn);
            }
        }
    LOG(else LOG_(" No Private Keys found!"));

    LOG_("<- CPKCS12Handler::SaveKeysToDiskL()");
    }

// Note: directory needs to end with a backslash
// writes binary data
void CPKCS12Handler::WriteToFileL(const TDesC8& aData, 
                                  const TDesC& aFileName) 
    {
    LOG_("-> CPKCS12Handler::WriteToFileL()");

    ASSERT(iOutputDir);
    ASSERT(aFileName.Length() > 0);

    // Disk space criticality check before attempting
    // to install
    if (SysUtil::FFSSpaceBelowCriticalLevelL(0, 0)) 
        {
        User::Leave(KErrDiskFull);
        }

    TFileName fn;
    fn.Append(*iOutputDir);
    fn.Append(aFileName);

    LOG_1(" Opening file: '%S'", &fn);

    TInt ret = iFileOut.Replace(iFs, fn, EFileWrite|EFileShareExclusive|EFileStream);
    
    LOG_1(" File open result: %d", ret);

    if (ret != KErrNone) 
        {
        User::Leave(ret);
        }

    LOG_(" Writing data");

    ret = iFileOut.Write(aData);
    LOG_1(" Write result: %d", ret);

    ret = iFileOut.Flush();
    LOG_1(" Flush result: %d", ret);
    
    iFileOut.Close();
    
    LOG_("<- CPKCS12Handler::WriteToFileL()");
    }

TBool CPKCS12Handler::VerifyType(const TDesC8& aData) const
    {
    ASSERT(iPkcsHandler);

    LOG_("-> CPKCS12Handler::VerifyType()");

    TBool isPKCS12(EFalse);

    // Need to check the data length before IsPKCS12Data call,
    // otherwise an assert (instead of a more suitable) 
    // might occur
    if (aData.Length() >= KPKCS12DataMinLength) 
        {
        isPKCS12 = iPkcsHandler->IsPKCS12Data(aData);
        }

    LOG_1("<- CPKCS12Handler::VerifyType() RET: %d", isPKCS12);
    return isPKCS12;
    }

void CPKCS12Handler::StoreKeyPairsL()
    {
    LOG_("-> CPKCS12Handler::StoreKeyPairsL()");
    
    TInt keycount = iPrivKeys->Count();
    
    for (TInt i = 0; i < keycount; i++) 
        {
        HBufC8* key = iPrivKeys->At(i);
        TPtrC8 keyPtr = *key;
        StoreSingleKeyL(*key);
        }
    
    LOG_("<- CPKCS12Handler::StoreKeyPairsL() OK");
    }

/**
 * NOTE: It should be decided what to do in following cases:
 * 1. Key storage operation fails for a key (there can be multiple keys 
 *    within a PKCS#12 package)
 * 2. User cert attachment fails for a key (there can be multiple user 
 *    certificates for any given key)
 *
 * At the moment, the code leaves if anything unexpected occurs.
 * There is no rollback mechanism (anything that was added before
 * the failure will still be in PKI stores).
 *
 */
void CPKCS12Handler::StoreSingleKeyL(const TDesC8& aKey) 
    {
    LOG_("-> CPKCS12Handler::StoreSingleKeyL()");
    
    // Setup initial values
    TRequestStatus requestStatus;
    TPKIKeyIdentifier keyId;
    
    // Perform asynchronous PKI operation synchronously
    iPkiService->StoreKeypair(keyId, aKey, requestStatus);

    User::WaitForRequest(requestStatus);

    // Check for operation status    
    TInt status = requestStatus.Int();
    if (status != KErrNone)
        {
        LOG_1("<- CPKCS12Handler::StoreSingleKeyL() LEAVE (%d)", status);
        User::Leave(status);
        }

    LOG_("<- CPKCS12Handler::StoreSingleKeyL() OK");
    }


void CPKCS12Handler::AttachCertificatesL() 
    {
    LOG_("-> CPKCS12Handler::AttachCertificatesL()");
    TInt certCount = iUserCerts->Count();
    for (TInt i = 0; i < certCount; i++) 
        {
        CX509Certificate* cert = iUserCerts->At(i);
        TKeyIdentifier certKeyId = cert->KeyIdentifierL();
        
        // Note: KeyID parameter is effectively redundant
        // (it can always be fetched from CX509Certificate object)
        AttachCertificateL(cert, certKeyId);

        }
    LOG_("<- CPKCS12Handler::AttachCertificatesL() OK");
    }

void CPKCS12Handler::StoreCertificatesL() 
    {
    LOG_("-> CPKCS12Handler::StoreCertificatesL()");
    TInt certCount = iCACerts->Count();
    for (TInt i = 0; i < certCount; i++)
        {
        CX509Certificate* cert = iCACerts->At(i);
        StoreCertificateL(cert);
        }
    LOG_("<- CPKCS12Handler::StoreCertificatesL() OK");
    }

void CPKCS12Handler::StoreCertificateL(CX509Certificate* aCert)
    {

    LOG_("-> CPKCS12Handler::StoreCertificateL()");

    LOG_1(" Deletable: %d", iDeletable);

    TInt status = iPkiService->StoreCertificate(EPKICACertificate,
                                                iDeletable,
                                                0,
                                                EPKIRSA,
                                                aCert->Encoding());

    if (status) 
        {
        LOG_1("<- CPKCS12Handler::StoreCertificateL() LEAVE (%d)", status);
        User::Leave(status);
        }

    SetApplicabilityL(aCert);

    LOG_("<- CPKCS12Handler::StoreCertificateL() OK");
    }


void CPKCS12Handler::AttachCertificateL(CX509Certificate* aCert, 
                                        const TPKIKeyIdentifier& aKeyId)
    {

    LOG_("-> CPKCS12Handler::AttachCertificateL()");

    TRequestStatus requestStatus;
    TAny* resArray(NULL);

    // Perform asynchronous PKI operation synchronously
    iPkiService->AttachCertificateL(aKeyId, DEFAULT_KEY_LEN, EPKIRSA, 
                                    aCert->Encoding(), &resArray, 
                                    requestStatus);

    User::WaitForRequest(requestStatus);
    iPkiService->Finalize(resArray);

    // Check for operation status    
    TInt status = requestStatus.Int();
    if (status != KErrNone)
        {
        LOG_1("<- CPKCS12Handler::AttachCertificateL() LEAVE: %d", status);
        User::Leave(status);
        }

    LOG_("<- CPKCS12Handler::AttachCertificateL() OK");
    }

/**
 * Not in use currently -- is Applicability a meaningful parameter
 * for user certificates?
 */
void CPKCS12Handler::SetApplicabilityL(CX509Certificate* aCert) 
    {
    LOG_("-> CPKCS12Handler::SetApplicabilityL()");

    if (aCert) 
        {
        // Only set applicability if there is atleast one applicability
        // setting defined
        if (iApplications && iApplications->Count() > 0)
            {
            LOG_(" Resolving subject  name");

            // Use subject name for CA certs
            const TPtrC8* issuerName = 
                aCert->DataElementEncoding(CX509Certificate::EIssuerName);
            
            LOG_(" Resolving serial number");
            const TPtrC8* serialNumber = 
                aCert->DataElementEncoding(CX509Certificate::ESerialNumber);
            
            LOG_(" Issuing PKI call");
            iPkiService->SetApplicabilityL(*issuerName, *serialNumber, *iApplications);
            }
        }
    else 
        {
        LOG_("<- CPKCS12Handler::SetApplicabilityL() Leave: NULL argument");
        User::Leave(KErrArgument);
        }

    LOG_("<- CPKCS12Handler::SetApplicabilityL() OK");
    }

void CPKCS12Handler::ExtractKeysAndCerts() 
    {
    ASSERT(iPkcsHandler);
    iCACerts = &iPkcsHandler->CACertificates();
    iUserCerts = &iPkcsHandler->UserCertificates();
    iPrivKeys = &iPkcsHandler->PrivateKeys();
    }

HBufC* CPKCS12Handler::QueryPasswordL()
    {
    LOG_("-> CPKCS12Handler::QueryPasswordL()");
    TRequestStatus status(KErrNone);
    LOG_(" SANITY2");

    HBufC* ret(NULL);

    TVpnDialogInfo dialogInfo(TVpnDialog::EPKCS12Password, 0);

    TPckgBuf<TVpnDialogInfo>                dialogInfoDes;
    TPckgBuf<TVpnDialogOutput>              dialogResponseDes;


    dialogInfoDes() = dialogInfo;

    iNotifier.StartNotifierAndGetResponse(status, KUidVpnDialogNotifier,
                                          dialogInfoDes, dialogResponseDes);

    // Wait until user has given the input
    LOG_(" Waiting for request");
    User::WaitForRequest(status);
    LOG_1(" Dialog terminated with status: %d", status.Int());
    
    if (status != KErrCancel && dialogResponseDes().iOutBuf.Length() >= 0)
        {
        LOG_(" Allocating");
        ret = dialogResponseDes().iOutBuf.AllocL();
        LOG_(" Canceling");
        iNotifier.CancelNotifier(KUidVpnDialogNotifier);
        LOG_1("<- CPKCS12Handler::QueryPasswordL() return: '%S'", ret);
        }
    else 
        {
        LOG_("<- CPKCS12Handler::QueryPasswordL() return: NULL");
        }

    return ret;
    }
 
void CPKCS12Handler::DisplayWrongPasswordNote() 
    {
    LOG_("-> CPKCS12Handler::DisplayWrongPasswordNote()");
    TRequestStatus status(KErrNone);

    LOG_1(" Constructing dialoginfo, DID: %d", 
            TVpnNoteDialog::EVpnWrongPKCS12Password);

    TIPSecDialogInfo info(TNoteDialog::EInfo, 
                          TVpnNoteDialog::EVpnWrongPKCS12Password); //create the input information
    TPckgBuf<TIPSecDialogInfo> infoBuf(info); //package it in appropriate buf
    TPckgBuf<TIPSecDialogOutput> responseBuf; //create the buf to receive the response
    
    iNotifier.StartNotifierAndGetResponse(status, KUidVpnDialogNotifier,
                                          infoBuf, responseBuf);
    User::WaitForRequest(status);

    iNotifier.CancelNotifier(KUidVpnDialogNotifier);

    LOG_("<- CPKCS12Handler::DisplayWrongPasswordNote()");
    }

