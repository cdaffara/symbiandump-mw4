/*
* Copyright (c) 2003-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Policy importer
*
*/



#ifndef __POLICYIMPORTER_H__
#define __POLICYIMPORTER_H__

#include <e32std.h>
#include <e32base.h>
#include "vpnapidefs.h"
#include "fileutil.h"

class CX509Certificate;
#include "pkiserviceapi.h"

class CVpnApiServant;
class CPolicyStore;
class RFs;
class CPolicyPatchInfoList;
class CIkeDataArray;
class CIkeData;

const TInt KMaxExtPolicyIdLength = 256;
const TInt KDoNotGetKeySize      = -1;

typedef TBuf<KMaxExtPolicyIdLength> TExtVpnPolicyId;

/**
 *  Handles the importing of VPN policies to the system.
 *
 *  @lib internal (vpnmanager.exe)
 *  @since S60 v3.0
 */
class CPolicyImporter : public CActive
    {
public:
    static CPolicyImporter* NewL(const RMessage2& aMessage,
        CVpnApiServant& aVpnApiServant,
        CPolicyStore& iPolicyStore, RFs& aFs);
    static CPolicyImporter* NewL(TRequestStatus& aStatus,
        CVpnApiServant& aVpnApiServant,
        CPolicyStore& iPolicyStore, RFs& aFs);
    ~CPolicyImporter();

    void ImportPolicyL(const TDesC& aDir);
    void ImportSinglePolicyL(const TDesC& aDir, TVpnPolicyId& aNewPolicyId);

private:
    CPolicyImporter(const RMessage2& aMessage, CVpnApiServant& aVpnApiServant,
                    CPolicyStore& iPolicyStore, RFs& aFs);
    CPolicyImporter(TRequestStatus& aStatus, CVpnApiServant& aVpnApiServant,
                    CPolicyStore& iPolicyStore, RFs& aFs);
    void ConstructL();

protected: // From CActive
    void DoCancel();
    void RunL();
    TInt RunError(TInt aError);

private:

    void DoImportPolicyL(const TDesC& aDir);

    void GotoState(TInt aState);
    void SetCurrState(TInt aState);
    void SetNextState(TInt aState);
    TInt CurrState();
    TInt NextState();
    void ChangeStateL();
    void CancelOngoingOperation();
    void ImportComplete(TInt aReturnValue);
    void StateBeginPolicyImportL();
    void StateImportCaCertL();
    void StateAfterImportCaCertL();
    void StateImportPeerCertL();
    void StateAfterImportPeerCertL();
    void StateImportUserPrivKeyL();
    void StateAfterImportUserPrivKeyL();
    void StateAttachCertificateL();
    void StateAfterAttachCertificateL();
    void StateImportPinAndPolL();
    void StateCreateVpnDestinationL();
    void StateEndPolicyImportL();
    void BuildPolicyIdListL();
    TPkiServiceStoreType GetStoreTypeL(CIkeData* aData);


    /**
     * Checks that BIN format CA certs defined in the policy can be
     * found in the same directory, where the .pol and .pin files are and
     * collects found certificate file names to the iCurrCaCertList list.
     *
     * Leaves with KVpnErrCaCertFileMissing, if a defined file is not found.
     */
    void BuildCaCertListL();

    /**
     * Checks that BIN format peer certs defined in the policy can be
     * found in the same directory, where the .pol and .pin files are and
     * collects found certificate file names to the iCurrPeerCertList list.
     *
     * Leaves with KVpnErrCaCertFileMissing, if a defined file is not found.
     */
    void BuildPeerCertListL();

    /**
     * Checks that BIN format user certs have and assosiated bin format
     * private key. Checks that both files exists in the same directory, where
     * the .pol and .pin files are. Collects the user cert file names to iCurrUserCertList
     * and private key file names to iCurrUserPrivKeyList.
     *
     * Leaves with KVpnErrCaCertFileMissing, if a defined file is not found.
     * of with KVpnErrInvalidPolicyFile if the user certificate doesn't has
     * a assosiated private key.
     */
    void BuildUserPrivKeyAndUserCertListL();
    /**
     * Checks if there is other CA certificates which are not defined in policy file
     *  (like intermediate CAs) and installs them to device. 
     */
    void BuildOtherCaCertListL();

    void ParseIkeDataL();
    void PatchPolicyCaCertInfoL(const TFileName& aPolicyFile);
    CPolicyPatchInfoList* BuildPolicyPatchInfoListL();
    HBufC8* CertSubjectNameL(const TFileName& aCertFile);
    void CleanImportDirectory();
    void AppendIfNotFoundL(CArrayFixFlat<TFileName>* aList,
        TFileName* aFileName);

    /**
     * Returns subject name of the certificate and sets aKeySize.
     * Parameter aKeySize is not set if aKeySize equals KDoNotGetKeySize.
     */
    HBufC8* CertInfoL(const TFileName& aCertFile, TInt& aKeySize);
    /**
     * Returns Issuer name of certificate
     */
    HBufC8* CPolicyImporter::CertIssuerL(const TFileName& aCertFile);
    TFileName CPolicyImporter::GetCAFromFileListL(const TDesC8& aCertSubjectName, CArrayFixFlat<TFileName>* aCertFileArray);

private:
    RMessage2 iMessage;
    TRequestStatus* iExtStatus;
    CVpnApiServant& iVpnApiServant;
    CPolicyStore& iPolicyStore;
    RFs& iFs;
    TFileUtil iFileUtil;
    RPKIServiceAPI iPkiService;

    TFileName iImportDir;

    TInt iCurrState;
    TInt iNextState;

    CArrayFixFlat<TExtVpnPolicyId>* iPolicyIdList;
    TInt iCurrPolicyIdIndex;
    TExtVpnPolicyId iCurrPolicyId;

    CIkeDataArray* iCurrIkeDataArray;
    TInt iCurrIkeDataIndex;

    CArrayFixFlat<TFileName>* iCurrCaCertList;
    TInt iCurrCaCertIndex;

    CArrayFixFlat<TFileName>* iCurrPeerCertList;
    TInt iCurrPeerCertIndex;

    CArrayFixFlat<TFileName>* iCurrUserPrivKeyList;
    TInt iCurrUserPrivKeyIndex;

    CArrayFixFlat<TFileName>* iCurrUserCertList;
    TInt iCurrUserCertIndex;
    //for intermediate CAs
    CArrayFixFlat<TFileName>* iCurrOtherCaCertList;
    TInt iCurrOtherCaCertIndex;

    TPKIKeyIdentifier iCurrKeyId;

    HBufC8* iCertFileData;
    HBufC8* iKeyFileData;

    TAny* iPkiOpContext;

    TBool iImportSinglePolicy;
    TVpnPolicyId* iNewPolicyId;

    TVpnPolicyId iPolicyId;
    };

#endif // __POLICYIMPORTER_H__
