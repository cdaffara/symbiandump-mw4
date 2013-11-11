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



#include "policyimporter.h"
#include "vpnapiservant.h"
#include "fileutil.h"
#include "policypatcher.h"
#include "policystore.h"
#include "vpnclientuids.h"
#include "vpnmanagerserverdefs.h"
#include "ikepolparser.h"
#include "pkiutil.h"
#include "cmmanagerutils.h"
#include "log_r6.h"

#include <f32file.h>
#include <x509cert.h>

const TInt KDefaultKeySize(1024);

enum TImportState
    {
    EStateBeginPolicyImport = 1,
    EStateImportCaCert,
    EStateAfterImportCaCert,
    EStateImportPeerCert,
    EStateAfterImportPeerCert,
    EStateImportUserPrivKey,
    EStateAfterImportUserPrivKey,
    EStateAttachCertificate,
    EStateAfterAttachCertificate,
    EStateImportPinAndPol,
    EStateCreateVpnDestination,
    EStateEndPolicyImport
    };

CPolicyImporter* CPolicyImporter::NewL(const RMessage2& aMessage, CVpnApiServant& aVpnApiServant,
                                       CPolicyStore& aPolicyStore, RFs& aFs)
    {
    LOG_("-> CPolicyImporter::NewL()");
    CPolicyImporter* self = new (ELeave) CPolicyImporter(aMessage, aVpnApiServant, aPolicyStore, aFs);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(); // self
    LOG_("<- CPolicyImporter::NewL()");
    return self;
    }

CPolicyImporter* CPolicyImporter::NewL(TRequestStatus& aStatus, CVpnApiServant& aVpnApiServant,
                                       CPolicyStore& aPolicyStore, RFs& aFs)
    {
    LOG_("-> CPolicyImporter::NewL()");
    CPolicyImporter* self = new (ELeave) CPolicyImporter(aStatus, aVpnApiServant, aPolicyStore, aFs);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(); // self
    LOG_("<- CPolicyImporter::NewL()");
    return self;
    }

CPolicyImporter::CPolicyImporter(const RMessage2& aMessage, CVpnApiServant& aVpnApiServant,
                                 CPolicyStore& aPolicyStore, RFs& aFs) :
    CActive(0), iMessage(aMessage), iVpnApiServant(aVpnApiServant),
    iPolicyStore(aPolicyStore), iFs(aFs), iFileUtil(aFs)
    {
    }

CPolicyImporter::CPolicyImporter(TRequestStatus& aStatus, CVpnApiServant& aVpnApiServant,
                                 CPolicyStore& aPolicyStore, RFs& aFs) :
    CActive(0), iExtStatus(&aStatus), iVpnApiServant(aVpnApiServant),
    iPolicyStore(aPolicyStore), iFs(aFs), iFileUtil(aFs)
    {
    }

void CPolicyImporter::ConstructL()
    {
    CActiveScheduler::Add(this);
    User::LeaveIfError(iPkiService.Connect());
    
    //Policy importer allow installation of
    //future and expired certificates.
    iPkiService.SetInformational(ETrue);
    }

CPolicyImporter::~CPolicyImporter()
    {
    LOG_("-> CPolicyImporter::~CPolicyImporter()");
    Cancel();
    iPkiService.Close();

    delete iPolicyIdList;

    delete iCurrIkeDataArray;

    delete iCurrCaCertList;
    delete iCurrPeerCertList;
    delete iCurrUserPrivKeyList;
    delete iCurrUserCertList;
    delete iCurrOtherCaCertList;

    delete iCertFileData;
    delete iKeyFileData;
    LOG_("<- CPolicyImporter::~CPolicyImporter()");
    }

TInt CPolicyImporter::RunError(TInt aError)
    {
    LOG_EVENT_2B(R_VPN_MSG_POLICY_INSTALL_FAIL, iNewPolicyId, NULL,
                 aError, iImportSinglePolicy);

    ImportComplete(aError);
    return KErrNone;
    }

void CPolicyImporter::RunL()
    {
    ChangeStateL();
    }

void CPolicyImporter::DoCancel()
    {
    CancelOngoingOperation();
    if (iImportSinglePolicy)
        {
        User::RequestComplete(iExtStatus, KErrCancel);
        }
    else
        {
        iMessage.Complete(KErrCancel);
        }
    CleanImportDirectory();
    }

void CPolicyImporter::GotoState(TInt aState)
    {
    LOG_1("-> CPolicyImporter::GotoState() STATE %d", aState);
    SetNextState(aState);
    SetActive();
    TRequestStatus* status = &iStatus;
    User::RequestComplete(status, KErrNone);
    LOG_("<- CPolicyImporter::GotoState()");
    }

void CPolicyImporter::SetCurrState(TInt aState)
    {
    iCurrState = aState;
    }

void CPolicyImporter::SetNextState(TInt aState)
    {
    iNextState = aState;
    }

TInt CPolicyImporter::CurrState()
    {
    return iCurrState;
    }

TInt CPolicyImporter::NextState()
    {
    return iNextState;
    }

void CPolicyImporter::ChangeStateL()
    {
    switch (NextState())
        {
        case EStateBeginPolicyImport:
            StateBeginPolicyImportL();
            break;

        case EStateImportCaCert:
            StateImportCaCertL();
            break;

        case EStateAfterImportCaCert:
            StateAfterImportCaCertL();
            break;

        case EStateImportPeerCert:
            StateImportPeerCertL();
            break;

        case EStateAfterImportPeerCert:
            StateAfterImportPeerCertL();
            break;

        case EStateImportUserPrivKey:
            StateImportUserPrivKeyL();
            break;

        case EStateAfterImportUserPrivKey:
            StateAfterImportUserPrivKeyL();
            break;

        case EStateAttachCertificate:
            StateAttachCertificateL();
            break;

        case EStateAfterAttachCertificate:
            StateAfterAttachCertificateL();
            break;

        case EStateImportPinAndPol:
            StateImportPinAndPolL();
            break;

        case EStateCreateVpnDestination:
            StateCreateVpnDestinationL();
            break;

        case EStateEndPolicyImport:
            StateEndPolicyImportL();
            break;

        default:
            User::Panic(KVpnManagerServer, EInvalidImportState);
            break;
        }
    }

void CPolicyImporter::CancelOngoingOperation()
    {
    switch (CurrState())
        {
        case EStateImportCaCert:
        case EStateImportPeerCert:
        case EStateImportUserPrivKey:
        case EStateAttachCertificate:
            iPkiService.CancelPendingOperation();
            iPkiService.Finalize(iPkiOpContext);
            break;

        default:
            break;
        }
    }

void CPolicyImporter::ImportComplete(TInt aReturnValue)
    {
    LOG_("-> CPolicyImporter::ImportComplete()");
    if (iImportSinglePolicy)
        {
        User::RequestComplete(iExtStatus, aReturnValue);
        }
    else
        {
        iMessage.Complete(aReturnValue);
        }
    CleanImportDirectory();
    iVpnApiServant.PolicyImportComplete();
    LOG_("<- CPolicyImporter::ImportComplete()");
    }

void CPolicyImporter::ImportPolicyL(const TDesC& aDir)
    {
    LOG_("-> CPolicyImporter::ImportPolicyL()");
    iImportSinglePolicy = EFalse;
    iNewPolicyId = &iPolicyId;
    DoImportPolicyL(aDir);
    LOG_("<- CPolicyImporter::ImportPolicyL()");
    }

void CPolicyImporter::ImportSinglePolicyL(const TDesC& aDir, TVpnPolicyId& aNewPolicyId)
    {
    LOG_("-> CPolicyImporter::ImportSinglePolicyL()");
    iImportSinglePolicy = ETrue;
    iNewPolicyId = &aNewPolicyId;
    DoImportPolicyL(aDir);
    LOG_("<- CPolicyImporter::ImportSinglePolicyL()");
    }

void CPolicyImporter::DoImportPolicyL(const TDesC& aDir)
    {
    LOG_("-> CPolicyImporter::DoImportPolicyL()");
    iImportDir.Copy(aDir);
    iCurrPolicyIdIndex = -1;

    BuildPolicyIdListL();

    if (iPolicyIdList->Count() == 0)
        {
        ImportComplete(KErrNone);
        return;
        }

    if (iImportSinglePolicy && iPolicyIdList->Count() != 1)
        {
        // We're supposed to import a single policy
        // only but the import directory contains
        // multiple policies...
        ImportComplete(KErrArgument);
        return;
        }

    // All is well, so begin import
    GotoState(EStateBeginPolicyImport);
    LOG_("<- CPolicyImporter::DoImportPolicyL()");
    }

void CPolicyImporter::StateBeginPolicyImportL()
    {
    SetCurrState(EStateBeginPolicyImport);

    iCurrPolicyIdIndex++;

    if (iCurrPolicyIdIndex == iPolicyIdList->Count())
        {
        ImportComplete(KErrNone);
        return;
        }

    iCurrPolicyId.Copy(iPolicyIdList->At(iCurrPolicyIdIndex));

    ParseIkeDataL();

    BuildCaCertListL();
    iCurrCaCertIndex = -1;

    BuildPeerCertListL();
    iCurrPeerCertIndex = -1;

    BuildUserPrivKeyAndUserCertListL();
    iCurrUserPrivKeyIndex = -1;
    iCurrUserCertIndex = -1;

    BuildOtherCaCertListL();
    iCurrOtherCaCertIndex=-1;
    GotoState(EStateImportCaCert);
    }

void CPolicyImporter::StateImportCaCertL()
    {
    LOG_("CPolicyImporter::StateImportCaCertL() entry");
    SetCurrState(EStateImportCaCert);

    iCurrCaCertIndex++;

    if (iCurrCaCertIndex == iCurrCaCertList->Count())
        {
        GotoState(EStateImportPeerCert);
        LOG_("CPolicyImporter::StateImportCaCertL() exit (all CA certs imported)");
        return;
        }

    delete iCertFileData;
    iCertFileData = NULL;
    iCertFileData = iFileUtil.LoadFileDataL(iCurrCaCertList->At(iCurrCaCertIndex));

    iPkiService.StoreCertificateL(EPKICACertificate, KDefaultKeySize, EPKIRSA, *iCertFileData,
                                  &iPkiOpContext, iStatus);

    SetNextState(EStateAfterImportCaCert);
    SetActive();
    LOG_("CPolicyImporter::StateImportCaCertL() exit");
    }

void CPolicyImporter::StateAfterImportCaCertL()
    {
    SetCurrState(EStateAfterImportCaCert);

    iPkiService.Finalize(iPkiOpContext);


    if (iStatus == KErrArgument)
        {
        User::Leave(KVpnErrInvalidCaCertFile);
        }
    else if (iStatus != KErrNone)
        {
        User::Leave(iStatus.Int());
        }

    // Set VPN trusted
    CX509Certificate* tempCert = CX509Certificate::NewLC(*iCertFileData);
    RArray<TUid> appArray;
    CleanupClosePushL(appArray);
    appArray.AppendL(TUid::Uid(KUidVpnManager));

    const TPtrC8* serialNumber = tempCert->DataElementEncoding(
        CX509Certificate::ESerialNumber);
    const TPtrC8* issuername = tempCert->DataElementEncoding(
        CX509Certificate::EIssuerName);

    iPkiService.SetApplicabilityL(
        *issuername,
        *serialNumber,
        appArray);

    CleanupStack::PopAndDestroy(2); // appArray, tempCert

    // Handle the next certificate, if present
    GotoState(EStateImportCaCert);
    }

void CPolicyImporter::StateImportPeerCertL()
    {
    LOG_("CPolicyImporter::StateImportOtherCaCertL() entry");
    SetCurrState(EStateImportPeerCert);

    iCurrOtherCaCertIndex++;

    if (iCurrOtherCaCertIndex == iCurrOtherCaCertList->Count())
        {
        GotoState(EStateImportUserPrivKey);
        LOG_("CPolicyImporter::StateImportOtherCaCertL() exit (all intermediate CAs imported)");
        return;
        }

    delete iCertFileData;
    iCertFileData = NULL;
    iCertFileData = iFileUtil.LoadFileDataL(iCurrOtherCaCertList->At(iCurrOtherCaCertIndex));
    CIkeData* data = iCurrIkeDataArray->At(iCurrIkeDataIndex);
    TPkiServiceStoreType storeType = GetStoreTypeL(data);
    iPkiService.SetStoreType(storeType);

    iPkiService.StoreCertificateL(EPKICACertificate, KDefaultKeySize, EPKIRSA, *iCertFileData,
                                  &iPkiOpContext, iStatus);

    SetNextState(EStateAfterImportPeerCert);
    SetActive();
    LOG_("CPolicyImporter::StateImportOtherCACertL() exit");
    }

void CPolicyImporter::StateAfterImportPeerCertL()
    {
    SetCurrState(EStateAfterImportPeerCert);

    iPkiService.Finalize(iPkiOpContext);

    if (iStatus != KErrNone)
        {
        User::Leave(iStatus.Int());
        }

    // Handle the next certificate, if present
    GotoState(EStateImportPeerCert);
    }

TPkiServiceStoreType CPolicyImporter::GetStoreTypeL(CIkeData* aData)
    {
    TPkiServiceStoreType ret(EPkiStoreTypeAny);
    if (aData->iClientCertType)
        {
        HBufC8* storename = aData->iClientCertType->GetAsciiDataL();
        CleanupStack::PushL(storename);
        LOG(Log::Printf(_L8("CPolicyImporter::BuildPeerCertListL() Store type defined in policy: %S\n"), &(*storename)));

        if (storename->Compare(_L8("DEVICE")) == 0)
            {
            LOG_("CPolicyImporter::BuildPeerCertListL() Policy uses DEVICE store\n");
            ret = EPkiStoreTypeDevice;
            }
        else
            {
            LOG_("CPolicyImporter::BuildPeerCertListL() Policy uses USER store\n");
            ret = EPkiStoreTypeUser;
            }

        CleanupStack::PopAndDestroy(storename);
        }
    else
        {
        LOG_("CPolicyImporter::GetStoreType() No store type specified in policy");
        }
    return ret;
    }
void CPolicyImporter::StateImportUserPrivKeyL()
    {
    LOG_("CPolicyImporter::StateImportUserPrivKeyL() entry");
    SetCurrState(EStateImportUserPrivKey);

    iCurrUserPrivKeyIndex++;

    if (iCurrUserPrivKeyIndex == iCurrUserPrivKeyList->Count())
        {
        GotoState(EStateImportPinAndPol);
        LOG_("CPolicyImporter::StateImportUserPrivKeyL() exit (all keys imported)");
        return;
        }

    delete iKeyFileData;
    iKeyFileData = NULL;
    iKeyFileData = iFileUtil.LoadFileDataL(iCurrUserPrivKeyList->At(iCurrUserPrivKeyIndex));
    CIkeData* data = iCurrIkeDataArray->At(iCurrIkeDataIndex);
    TPkiServiceStoreType storeType = GetStoreTypeL(data);
    iPkiService.SetStoreType(storeType);

    iPkiService.StoreKeypair(iCurrKeyId, *iKeyFileData, iStatus);

    SetNextState(EStateAfterImportUserPrivKey);
    SetActive();
    LOG_("CPolicyImporter::StateImportUserPrivKeyL() exit");
    }

void CPolicyImporter::StateAfterImportUserPrivKeyL()
    {
    SetCurrState(EStateAfterImportUserPrivKey);

    if (iStatus == KErrArgument || iStatus == KErrNotSupported)
        {
        User::Leave(KVpnErrInvalidUserPrivKeyFile);
        }
    else if (iStatus != KErrNone)
        {
        User::Leave(iStatus.Int());
        }

    // Attach user certificates to the imported private key
    iCurrIkeDataIndex = -1;
    GotoState(EStateAttachCertificate);
    }

void CPolicyImporter::StateAttachCertificateL()
    {
    LOG_("CPolicyImporter::StateAttachCertificateL() entry");
    SetCurrState(EStateAttachCertificate);

    iCurrIkeDataIndex++;

    if (iCurrIkeDataIndex == iCurrIkeDataArray->Count())
        {
        // Import the next private key, if present
        GotoState(EStateImportUserPrivKey);
        return;
        }

    CIkeData* ikeData = iCurrIkeDataArray->At(iCurrIkeDataIndex);
    HBufC* fileName(NULL);
    TPkiServiceStoreType storeType = GetStoreTypeL(ikeData);
    iPkiService.SetStoreType(storeType);

    fileName = iFileUtil.MakeFileName(iImportDir, ikeData->iPrivKey.iData).AllocLC();

    if (fileName->CompareF(iCurrUserPrivKeyList->At(iCurrUserPrivKeyIndex)) == 0)
        {
        CleanupStack::PopAndDestroy(fileName);
        fileName = NULL;

        fileName = iFileUtil.MakeFileName(iImportDir, ikeData->iOwnCert.iData).AllocLC();

        delete iCertFileData;
        iCertFileData = NULL;
        iCertFileData = iFileUtil.LoadFileDataL(*fileName);

        iPkiService.AttachCertificateL(iCurrKeyId, KDefaultKeySize, EPKIRSA, *iCertFileData,
                                       &iPkiOpContext, iStatus);

        SetNextState(EStateAfterAttachCertificate);
        SetActive();
        }
    else
        {
        // Attach the next certificate, if present
        GotoState(EStateAttachCertificate);
        }
    CleanupStack::PopAndDestroy(fileName); // fileName
    LOG_("CPolicyImporter::StateAttachCertificateL() exit");
    }

void CPolicyImporter::StateAfterAttachCertificateL()
    {
    SetCurrState(EStateAfterAttachCertificate);

    iPkiService.Finalize(iPkiOpContext);

    if (iStatus == KErrArgument)
        {
        User::Leave(KVpnErrInvalidUserCertFile);
        }
    else if (iStatus != KErrNone)
        {
        User::Leave(iStatus.Int());
        }

    // Attach the next certificate, if present
    GotoState(EStateAttachCertificate);
    }

void CPolicyImporter::StateImportPinAndPolL()
    {
    LOG_("-> CPolicyImporter::StateImportPinAndPolL()");
    SetCurrState(EStateImportPinAndPol);

    HBufC* pinFile = iFileUtil.MakeFileNameLC(iImportDir, iCurrPolicyId, KPinFileExt);
    HBufC* polFile = iFileUtil.MakeFileNameLC(iImportDir, iCurrPolicyId, KPolFileExt);

    if (!iFileUtil.FileExists(*pinFile))
        {
        LOG_("<- CPolicyImporter::StateImportPinAndPolL() LEAVE: KVpnErrNoPolicyInfoFile");
        User::Leave(KVpnErrNoPolicyInfoFile);
        }
    else if (!iFileUtil.FileExists(*polFile))
        {
        LOG_("<- CPolicyImporter::StateImportPinAndPolL() LEAVE: KVpnErrNoPolicyFile");
        User::Leave(KVpnErrNoPolicyFile);
        }
    else
        {
        PatchPolicyCaCertInfoL(*polFile);

        iPolicyStore.ImportPolicyL(*pinFile, *polFile, iNewPolicyId);

        //iImportSinglePolicy is used when policy is installed via
        //OMA DM or ACU. If the policy is installed from .vpn file
        //the iImportSinglePolicy is not used.
        //The VPN destination is only created in .vpn case.
        if (iImportSinglePolicy)
            {
            GotoState(EStateEndPolicyImport);
            }
        else
            {
            GotoState(EStateCreateVpnDestination);
            }
        }
    CleanupStack::PopAndDestroy(2); // polfile, pinfile
    LOG_("<- CPolicyImporter::StateImportPinAndPolL()");
    }

void CPolicyImporter::StateCreateVpnDestinationL()
    {
    LOG_("-> CPolicyImporter::StateCreateVpnDestinationL()");
    SetCurrState(EStateCreateVpnDestination);

        //Gets the IAP name from policy name
    TVpnPolicyInfo* policyInfo = new (ELeave) TVpnPolicyInfo;
    CleanupDeletePushL(policyInfo);

    User::LeaveIfError(iPolicyStore.GetPolicyInfo(*iNewPolicyId, *policyInfo));

    CmManagerUtils::CreateVPNConnectionMethodToIntranetL(*policyInfo,
                                                         *(iVpnApiServant.iEventLogger));

    CleanupStack::PopAndDestroy(); //policyInfo
    GotoState(EStateEndPolicyImport);

    LOG_("<- CPolicyImporter::StateCreateVpnDestinationL()");
    }

void CPolicyImporter::StateEndPolicyImportL()
    {
    LOG_("-> CPolicyImporter::StateEndPolicyImportL()");

    STACK_LEFT;

    SetCurrState(EStateEndPolicyImport);

    // Delete the files that were just imported from the import/install directory

    HBufC* fileFilter = iFileUtil.MakeFileNameLC(iImportDir, iCurrPolicyId, KAllFilesPat);
    iFileUtil.DeleteFilesL(*fileFilter);

    CleanupStack::PopAndDestroy(); // fileFilter

    LOG_EVENT_2B(R_VPN_MSG_INSTALLED_POLICY_FILE, iNewPolicyId, NULL, 0, iImportSinglePolicy);

    GotoState(EStateBeginPolicyImport);
    LOG_("<- CPolicyImporter::StateEndPolicyImportL()");
    }

void CPolicyImporter::BuildPolicyIdListL()
    {
    delete iPolicyIdList;
    iPolicyIdList = NULL;
    iPolicyIdList = new (ELeave) CArrayFixFlat<TExtVpnPolicyId>(2);

    TFindFile* fileFinder = new (ELeave) TFindFile(iFs);
    CleanupStack::PushL(fileFinder);

    CDir* fileList;

    TInt ret = fileFinder->FindWildByDir(KPinFilePat, iImportDir, fileList);

    if (ret == KErrNone)
        {
        CleanupStack::PushL(fileList);

        for (TInt i = 0; i < fileList->Count(); i++)
            {
            TParse* fileNameParser = new (ELeave) TParse();
            CleanupStack::PushL(fileNameParser);

            fileNameParser->Set((*fileList)[i].iName, NULL, NULL);

            TExtVpnPolicyId policyId;
            policyId.Copy(fileNameParser->Name());

            iPolicyIdList->AppendL(policyId);

            CleanupStack::PopAndDestroy(); // fileNameParser
            }
        CleanupStack::PopAndDestroy(); // fileList
        }
    CleanupStack::PopAndDestroy(); // fileFinder
    }

void CPolicyImporter::BuildCaCertListL()
    {
    LOG_("-> CPolicyImporter::BuildCaCertListL()");
    delete iCurrCaCertList;
    iCurrCaCertList = NULL;
    iCurrCaCertList = new (ELeave) CArrayFixFlat<TFileName>(2);

    TFileName *fileName = new (ELeave) TFileName;
    CleanupStack::PushL(fileName);

    LOG_("Pre-for");
    for (TInt i = 0; i < iCurrIkeDataArray->Count(); i++)
        {
        LOG_("For start");
        CIkeData* ikeData = iCurrIkeDataArray->At(i);
        fileName->Zero();

        if (ikeData->iCAList)
            {
            LOG_("CAlist found");
            for (TInt j = 0; j < ikeData->iCAList->Count(); j++)
                {
                LOG_("CA iter start");
                if (ikeData->iCAList->At(j)->iFormat == BIN_CERT)
                    {
                    LOG_("Bin cert found");
                    *fileName = iFileUtil.MakeFileName(iImportDir, ikeData->iCAList->At(j)->iData);
                    if (!iFileUtil.FileExists(*fileName))
                        {
                        LOG_("<- CPolicyImporter::BuildCaCertListL() LEAVE (KVpnErrCaCertFileMissing)");
                        User::Leave(KVpnErrInvalidCaCertFile);
                        }
                    //Makes sure every file name is appended only once.
                    AppendIfNotFoundL( iCurrCaCertList, fileName );
                    }
                }
            }
        }

    CleanupStack::PopAndDestroy(); //fileName
    LOG_("<- CPolicyImporter::BuildCaCertListL()");
    }


void CPolicyImporter::BuildPeerCertListL()
    {
    LOG(Log::Printf(_L8("-> CPolicyImporter::BuildPeerCertListL()\n")));
    delete iCurrPeerCertList;
    iCurrPeerCertList = NULL;
    iCurrPeerCertList = new (ELeave) CArrayFixFlat<TFileName>(2);

    TFileName *fileName = new (ELeave) TFileName;
    CleanupStack::PushL(fileName);

    for (TInt i = 0; i < iCurrIkeDataArray->Count(); i++)
        {
        CIkeData* ikeData = iCurrIkeDataArray->At(i);
        fileName->Zero();

        if (ikeData->iPeerCert.iData.Length() > 0 &&
            ikeData->iPeerCert.iFormat == BIN_CERT)
            {
            *fileName = iFileUtil.MakeFileName(iImportDir, ikeData->iPeerCert.iData);
            if (!iFileUtil.FileExists(*fileName))
                {
                User::Leave(KVpnErrPeerCertFileMissing);
                }
            AppendIfNotFoundL( iCurrPeerCertList, fileName );
            }
        }

    CleanupStack::PopAndDestroy(); //fileName
    LOG_("<- CPolicyImporter::BuildPeerCertListL()");
    }


void CPolicyImporter::BuildUserPrivKeyAndUserCertListL()
    {
    LOG_("-> CPolicyImporter::BuildUserPrivKeyAndUserCertListL()");
    delete iCurrUserPrivKeyList;
    iCurrUserPrivKeyList = NULL;
    iCurrUserPrivKeyList = new (ELeave) CArrayFixFlat<TFileName>(2);

    delete iCurrUserCertList;
    iCurrUserCertList = NULL;
    iCurrUserCertList = new (ELeave) CArrayFixFlat<TFileName>(2);

    TFileName *fileName = new (ELeave) TFileName;
    CleanupStack::PushL(fileName);


    for (TInt i = 0; i < iCurrIkeDataArray->Count(); i++)
        {
        CIkeData* ikeData = iCurrIkeDataArray->At(i);
        fileName->Zero();

        if (ikeData->iOwnCert.iData.Length() > 0 &&
            ikeData->iOwnCert.iFormat == BIN_CERT)
            {
            //First check that defined user cert is found and if so
            //add the file name to the list

            *fileName = iFileUtil.MakeFileName(iImportDir, ikeData->iOwnCert.iData);
            if (!iFileUtil.FileExists(*fileName))
                {
                User::Leave(KVpnErrInvalidUserCertFile);
                }
            AppendIfNotFoundL( iCurrUserCertList, fileName );

            //After the user cert is found check that the assosiated private key
            //is found.
            if (ikeData->iPrivKey.iData.Length() > 0 &&
                ikeData->iPrivKey.iFormat == BIN_CERT)
                {
                fileName->Zero();
                *fileName = iFileUtil.MakeFileName(iImportDir, ikeData->iPrivKey.iData);
                if (!iFileUtil.FileExists(*fileName))
                    {
                    User::Leave(KVpnErrInvalidUserPrivKeyFile);
                    }
                AppendIfNotFoundL( iCurrUserPrivKeyList, fileName );
                }
            else
                {
                User::Leave(KVpnErrInvalidPolicyFile);
                }
            }
        }

    CleanupStack::PopAndDestroy(); //fileName
    LOG_("<- CPolicyImporter::BuildUserPrivKeyAndUserCertListL()");
    }

void CPolicyImporter::BuildOtherCaCertListL()
    {
    LOG(Log::Printf(_L8("-> CPolicyImporter::BuildOtherCACertListL()\n")));
    delete iCurrOtherCaCertList;
    iCurrOtherCaCertList = NULL;
    iCurrOtherCaCertList = new (ELeave) CArrayFixFlat<TFileName>(2);
    TFileName *fileName = new (ELeave) TFileName;
    CleanupStack::PushL(fileName);
    TFileName *totalPath= new (ELeave) TFileName;
	CleanupStack::PushL(totalPath);   
    CDir* dirList=NULL;
    _LIT(KFileSpec, "*ca-?.*er");
    *totalPath=iImportDir;
    totalPath->Append(KFileSpec);
    
    
    User::LeaveIfError(
         iFs.GetDir(*totalPath,
                    KEntryAttMaskSupported,
                    ESortByName, dirList));
    CleanupStack::PushL(dirList);
    if ( dirList->Count()>1 )
        {
        for (TInt i=0;i<dirList->Count();i++)
          {
          *fileName = (*dirList)[i].iName;
          *totalPath = iImportDir;
          totalPath->Append(*fileName);
          AppendIfNotFoundL(iCurrOtherCaCertList, totalPath);
          }
        }
      CleanupStack::PopAndDestroy(dirList);   
      CleanupStack::PopAndDestroy(totalPath);   
      
    CleanupStack::PopAndDestroy(); //fileName
    LOG_("<- CPolicyImporter::BuildOtherCaCertListL()");
    }

void CPolicyImporter::ParseIkeDataL()
    {
    LOG_("-> CPolicyImporter::ParseIkeDataL()");

    HBufC* polFile = iFileUtil.MakeFileNameLC(iImportDir, iCurrPolicyId, KPolFileExt);

    if (!iFileUtil.FileExists(*polFile))
        {
        LOG_("<- CPolicyImporter::ParseIkeDataL() LEAVE (KVpnErrNoPolicyFile)");
        User::Leave(KVpnErrNoPolicyFile);
        }

    HBufC8* fileData = iFileUtil.LoadFileDataL(*polFile);
    CleanupStack::PushL(fileData);

    HBufC* fileData16 = HBufC::NewLC(fileData->Length());

    fileData16->Des().Copy(*fileData);

    delete iCurrIkeDataArray;
    iCurrIkeDataArray = NULL;
    iCurrIkeDataArray = CIkeDataArray::NewL(1);

    TIkeParser* ikeParser = new (ELeave) TIkeParser(*fileData16);
    CleanupStack::PushL(ikeParser);
    ikeParser->ParseIKESectionsL(iCurrIkeDataArray);

    CleanupStack::PopAndDestroy(4); // ikeParser, fileData16, fileData, polFile
    LOG_("<- CPolicyImporter::ParseIkeDataL()");
    }


void CPolicyImporter::PatchPolicyCaCertInfoL(const TFileName& aPolicyFile)
    {
    LOG_("-> CPolicyImporter::PatchPolicyCaCertInfoL()");
    HBufC8* policyData = iFileUtil.LoadFileDataL(aPolicyFile);
    CleanupStack::PushL(policyData);

    CPolicyPatchInfoList* patchInfoList = BuildPolicyPatchInfoListL();
    CleanupStack::PushL(patchInfoList);

    CPolicyPatcher* patcher = CPolicyPatcher::NewL();
    CleanupStack::PushL(patcher);

    HBufC8* patchedPolicyData = patcher->PatchPolicyL(*policyData, patchInfoList);
    CleanupStack::PushL(patchedPolicyData);

    iFileUtil.SaveFileDataL(aPolicyFile, *patchedPolicyData);

    CleanupStack::PopAndDestroy(4); // patchedPolicyData, patcher, patchInfoList, policyData
    LOG_("<- CPolicyImporter::PatchPolicyCaCertInfoL()");
    }

CPolicyPatchInfoList* CPolicyImporter::BuildPolicyPatchInfoListL()
    {
    LOG_("-> CPolicyImporter::BuildPolicyPatchInfoListL()");
    CPolicyPatchInfoList* patchInfoList = new (ELeave) CPolicyPatchInfoList(2);
    CleanupStack::PushL(patchInfoList);
    HBufC8* subjectName;
    // First, append the CA certs to patch list...
    for (TInt i = 0; i < iCurrCaCertList->Count(); i++)
        {

        CPolicyPatchInfo* patchInfo = new (ELeave) CPolicyPatchInfo();
        CleanupStack::PushL(patchInfo);

        TParse fileNameParser;
        fileNameParser.Set(iCurrCaCertList->At(i), NULL, NULL);

        patchInfo->iCertFileName.Copy(fileNameParser.NameAndExt());
        subjectName = CertSubjectNameL(iCurrCaCertList->At(i));
        CleanupStack::PushL(subjectName);
        if ( iCurrOtherCaCertList->Count()>1 && iCurrCaCertList->Count()==1 ) //if other than basic CA certificate exists
            {
            // Set original intermediate CA untrusted. . 
            HBufC8* certData = iFileUtil.LoadFileDataL(iCurrCaCertList->At(0));
            CleanupStack::PushL(certData);
            CX509Certificate* tempCert = CX509Certificate::NewLC(*certData);
            RArray<TUid> appArray;
            CleanupClosePushL(appArray);
            const TPtrC8* serialNumber = tempCert->DataElementEncoding(
                  CX509Certificate::ESerialNumber);
            const TPtrC8* issuername = tempCert->DataElementEncoding(
                  CX509Certificate::EIssuerName);

            iPkiService.SetApplicabilityL(
                       *issuername,
                       *serialNumber,
                       appArray);

            CleanupStack::PopAndDestroy(3); // appArray, tempcert
            
              //get CA from chain
            TFileName rootCAFile=GetCAFromFileListL(*subjectName, iCurrOtherCaCertList);
            CleanupStack::PopAndDestroy(subjectName);
            subjectName=NULL;
            subjectName = CertSubjectNameL(rootCAFile);
            CleanupStack::PushL(subjectName);
            
            //Set highest CA as trusted
            certData = iFileUtil.LoadFileDataL(rootCAFile);
            CleanupStack::PushL(certData);
            tempCert = CX509Certificate::NewLC(*certData);
            CleanupClosePushL(appArray);
            appArray.AppendL(TUid::Uid(KUidVpnManager));
            serialNumber = tempCert->DataElementEncoding(
                  CX509Certificate::ESerialNumber);
            issuername = tempCert->DataElementEncoding(
                  CX509Certificate::EIssuerName);

            iPkiService.SetApplicabilityL(
                       *issuername,
                       *serialNumber,
                       appArray);
 
            CleanupStack::PopAndDestroy(3); // appArray, tempcert, certData
            }
        patchInfo->SetCertSubjectNameL(*subjectName);

        patchInfoList->AppendL(patchInfo);
        CleanupStack::PopAndDestroy(subjectName);
        subjectName=NULL;
        CleanupStack::Pop(patchInfo); // patcInfo (now owned by the list)
        }

    // ... then, append also the user certificates.
    for (TInt i = 0; i < iCurrUserCertList->Count(); i++)
        {
        TInt keySize = 0;
        HBufC8* subjectName = CertInfoL(iCurrUserCertList->At(i), keySize);
        CleanupStack::PushL(subjectName);

        CPolicyPatchInfo* patchInfo = new (ELeave) CPolicyPatchInfo();
        CleanupStack::PushL(patchInfo);

        TParse fileNameParser;
        fileNameParser.Set(iCurrUserCertList->At(i), NULL, NULL);

        patchInfo->iCertFileName.Copy(fileNameParser.NameAndExt());
        patchInfo->SetCertSubjectNameL(*subjectName);
        patchInfo->SetUserCertKeyLen(keySize);

        patchInfoList->AppendL(patchInfo);

        CleanupStack::Pop(); // patchInfo (now owned by the list)
        CleanupStack::PopAndDestroy(); // subjectName
        }

    CleanupStack::Pop(); // patchInfoList, ownership transferred

    LOG_("<- CPolicyImporter::BuildPolicyPatchInfoListL()");
    return patchInfoList;
    }

HBufC8* CPolicyImporter::CertSubjectNameL(const TFileName& aCertFile)
    {
    TInt keySize = KDoNotGetKeySize;
    return CertInfoL(aCertFile,keySize);
    }

void CPolicyImporter::CleanImportDirectory()
    {
    LOG_("-> CPolicyImporter::CleanImportDirectory()");

    TRAP_IGNORE(
        {
        HBufC* fileFilter = iFileUtil.MakeFileNameLC(iImportDir, KNullDesC, KAllFilesPat);
        iFileUtil.DeleteFilesL(*fileFilter);
        CleanupStack::PopAndDestroy();
        });

    LOG_("<- CPolicyImporter::CleanImportDirectory()");
    }

void CPolicyImporter::AppendIfNotFoundL(CArrayFixFlat<TFileName>* aList,
    TFileName* aFileName)
    {
    ASSERT(aList && aFileName);

    TKeyArrayFix arrayKey(0, ECmpFolded);
    TInt position;
    if ( aList->FindIsq( *aFileName, arrayKey, position ) )
        {
        aList->AppendL( *aFileName );
        }
    }

HBufC8* CPolicyImporter::CertInfoL(const TFileName& aCertFile, TInt& aKeySize)
    {
    HBufC8* certData = iFileUtil.LoadFileDataL(aCertFile);
    CleanupStack::PushL(certData);

    HBufC* subjectName = PkiUtil::CertSubjectNameL(*certData);
    CleanupStack::PushL(subjectName);

    HBufC8* subjectName8 = HBufC8::NewL(subjectName->Length());
    subjectName8->Des().Copy(*subjectName);

    if(KDoNotGetKeySize != aKeySize)
        {
        aKeySize = PkiUtil::CertKeySizeL(*certData);
        }

    CleanupStack::PopAndDestroy(2); // subjectName, certData

    return subjectName8;
    }

HBufC8* CPolicyImporter::CertIssuerL(const TFileName& aCertFile)
    {
    HBufC8* certData = iFileUtil.LoadFileDataL(aCertFile);
    CleanupStack::PushL(certData);

    HBufC* issuerName = PkiUtil::CertIssuerNameL(*certData);
    CleanupStack::PushL(issuerName);

    HBufC8* issuerName8 = HBufC8::NewL(issuerName->Length());
    issuerName8->Des().Copy(*issuerName);

    CleanupStack::PopAndDestroy(2); // subjectName, certData

    return issuerName8;
    }

TFileName CPolicyImporter::GetCAFromFileListL(const TDesC8& aCertSubjectName, CArrayFixFlat<TFileName>* aCertFileArray)
    {
    TFileName rootCa;
    TInt currCaIndex=0;
    TInt currIndex=1;
    TInt keySize = 0;
    for ( TInt i=0; i<aCertFileArray->Count(); i++)
        {
        HBufC8* certSubjectName = CertInfoL(aCertFileArray->At(i), keySize);
        CleanupStack::PushL(certSubjectName);
        if ( certSubjectName->Compare(aCertSubjectName) == 0)
            currCaIndex=i;
        CleanupStack::PopAndDestroy(certSubjectName);
        certSubjectName=NULL;
        }
  
    
    while ( currIndex< aCertFileArray->Count())
        {
        HBufC8* issuerName = CertIssuerL(aCertFileArray->At(currCaIndex));
        CleanupStack::PushL(issuerName);
        HBufC8* subjectName = CertInfoL(aCertFileArray->At(currCaIndex), keySize);
        CleanupStack::PushL(subjectName);
        
        for (TInt i=0; i<aCertFileArray->Count();i++)
            {
            HBufC8* certSubjectName = CertInfoL(aCertFileArray->At(i), keySize);
            CleanupStack::PushL(certSubjectName);
            if ( certSubjectName->Compare(*issuerName)==0 )
                {
                currCaIndex=i;
                CleanupStack::PopAndDestroy(certSubjectName);
                certSubjectName=NULL;
                break;
                }
            CleanupStack::PopAndDestroy(certSubjectName);
            certSubjectName=NULL;
            }
        
        CleanupStack::PopAndDestroy(subjectName);
        subjectName=NULL;
        CleanupStack::PopAndDestroy(issuerName);
        issuerName=NULL;
        currIndex++;
        }       
    
    return aCertFileArray->At(currCaIndex);
    
    }
/***/
