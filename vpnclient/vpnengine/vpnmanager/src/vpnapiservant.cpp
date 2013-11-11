/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Handles API requests for VPN Manager.
*
*/



#include "vpnapiservant.h"
#include "policystore.h"
#include "policyimporter.h"
#include "pwdchanger.h"
#include "vpnapidefs.h"
#include "vpnmanagerserverdefs.h"
#include "vpnmaninternal.h"
#include "log_r6.h"


CVpnApiServant* CVpnApiServant::NewL(RFs& aFs)
    {
    CVpnApiServant* self = new (ELeave) CVpnApiServant(aFs);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(); // self
    return self;
    }

CVpnApiServant::CVpnApiServant(RFs& aFs) : iFs(aFs)
    {
    }

void CVpnApiServant::ConstructL()
    {
    iPolicyStore = CPolicyStore::NewL(iFs);
    INIT_EVENT_LOGGER(*iPolicyStore);
    }

CVpnApiServant::~CVpnApiServant()
    {
    delete iPolicyStore;
    RELEASE_EVENT_LOGGER;
    }
    
TBool CVpnApiServant::ServiceL(const RMessage2& aMessage)
    {
    TBool requestHandled = ETrue;
    
    switch (aMessage.Function())
        {
        case EVpnImportPolicy:
            ImportPolicyL(aMessage);
            break;
            
        case EVpnCancelImport:
            CancelImportPolicy(aMessage);
            break;
            
        case EVpnEnumeratePolicies:
            EnumeratePoliciesL(aMessage);
            break;
            
        case EVpnGetPolicyInfo:
            GetPolicyInfoL(aMessage);
            break;
            
        case EVpnGetPolicyDetails:
            GetPolicyDetailsL(aMessage);
            break;
            
        case EVpnDeletePolicy:
            DeletePolicyL(aMessage);
            break;
            
        case EVpnChangePassword:
            ChangePasswordL(aMessage);
            break;

        case EVpnCancelChange:
            CancelChangePassword(aMessage);
            break;

        case EVpnGetPolicySize:
            GetPolicySizeL(aMessage);
            break;
            
        case EVpnGetPolicyData:
            GetPolicyDataL(aMessage);
            break;

        // New methods (to facilitate
        // OMA DM based policy management)
        
        case EVpnAddPolicy:
            AddPolicyL(aMessage);
            break;

        case EVpnUpdatePolicyDetails:
            UpdatePolicyDetailsL(aMessage);
            break;
            
        case EVpnUpdatePolicyData:
            UpdatePolicyDataL(aMessage);
            break;
            
        default:
            requestHandled = EFalse;
            break;
        }

    return requestHandled;
    }

void CVpnApiServant::ImportPolicyL(const RMessage2& aMessage)
    {
    if (iPolicyImporter)
        {
        aMessage.Complete(KVpnErrImportOngoing);
        }
    else
        {
        TInt len = aMessage.GetDesLength(0);
        HBufC* input = HBufC::NewL(len);
        CleanupStack::PushL(input);
        
        TPtr ptrInput = input->Des();
        aMessage.ReadL(FIRST_ARGUMENT, ptrInput);
        
        if (input->Length() > 0)
            {                    
            iPolicyImporter = CPolicyImporter::NewL(aMessage, *this, *iPolicyStore, iFs);
            iPolicyImporter->ImportPolicyL(*input);
            }
        else
            {
            aMessage.Complete(KVpnErrPolicyNotFound);
            }

        CleanupStack::PopAndDestroy(); // input
        }
    }

void CVpnApiServant::ImportSinglePolicyL(const TDesC& aDir, TVpnPolicyId& aNewPolicyId,
                                         TRequestStatus& aStatus)
    {
    if (iPolicyImporter)
        {
        User::Leave(KVpnErrImportOngoing);
        }
    else
        {
        iPolicyImporter = CPolicyImporter::NewL(aStatus, *this, *iPolicyStore, iFs);
        aStatus = KRequestPending;
        iPolicyImporter->ImportSinglePolicyL(aDir, aNewPolicyId);
        }
    }
    
void CVpnApiServant::CancelImportPolicy(const RMessage2& aMessage)
    {
    if (iPolicyImporter)
        {
        iPolicyImporter->Cancel();
        delete iPolicyImporter;
        iPolicyImporter = NULL;
        }
    
    aMessage.Complete(KErrNone);
    }

void CVpnApiServant::CancelImportSinglePolicy()
    {
    if (iPolicyImporter)
        {
        iPolicyImporter->Cancel();
        delete iPolicyImporter;
        iPolicyImporter = NULL;
        }
    }
    
void CVpnApiServant::PolicyImportComplete()
    {
    delete iPolicyImporter;
    iPolicyImporter = NULL;
    }

void CVpnApiServant::EnumeratePoliciesL(const RMessage2& aMessage)
    {
    TInt policyCount = iPolicyStore->PolicyCount();

    TPckg<TInt> pckgPolicyCount(policyCount);
    aMessage.WriteL(FIRST_ARGUMENT, pckgPolicyCount);

    aMessage.Complete(KErrNone);
    }

void CVpnApiServant::GetPolicyInfoL(const RMessage2& aMessage)
    {
	TInt expectedPolicyCount = aMessage.Int0();

    if (expectedPolicyCount != iPolicyStore->PolicyCount())
        {
        aMessage.Complete(KVpnErrPolicyCountChanged);
        return;
        }

    TUint8* rawPolicyData = iPolicyStore->RawPolicyData();
    
	// Write the state array back to the client's address space
	TPtrC8 policyData(rawPolicyData, expectedPolicyCount * sizeof(TVpnPolicyInfo));
    
    aMessage.WriteL(SECOND_ARGUMENT, policyData);

    aMessage.Complete(KErrNone);
    }

void CVpnApiServant::GetPolicyDetailsL(const RMessage2& aMessage)
    {
    LOG(Log::Printf(_L("CVpnApiServant::GetPolicyDetailsL")));    
    
    TVpnPolicyId policyId;
    TPckg<TVpnPolicyId> pckgPolicyId(policyId);

    aMessage.ReadL(FIRST_ARGUMENT, pckgPolicyId);

    TVpnPolicyDetails policyDetails;
    TInt ret = iPolicyStore->GetPolicyDetailsL(policyId, policyDetails);

    if (ret == KErrNone)
        {
        TPckg<TVpnPolicyDetails> pckgPolicyDetails(policyDetails);
        aMessage.WriteL(SECOND_ARGUMENT, pckgPolicyDetails);
        }
        
            
    aMessage.Complete(ret);
    
    LOG(Log::Printf(_L("CVpnApiServant::GetPolicyDetailsL: aMesage completed withd %d"), ret));    
    }

void CVpnApiServant::DeletePolicyL(const RMessage2& aMessage)
    {
    TVpnPolicyId policyId;
    TPckg<TVpnPolicyId> pckgPolicyId(policyId);

    aMessage.ReadL(FIRST_ARGUMENT, pckgPolicyId);

    TVpnPolicyDetails *policyDetails = new (ELeave) TVpnPolicyDetails;
    CleanupStack::PushL(policyDetails);    
    
    iPolicyStore->GetPolicyDetailsL(policyId, *policyDetails);    
    iPolicyStore->DeletePolicyL(policyId);


    HBufC8* tempBuf = HBufC8::NewLC(policyDetails->iName.Length());
    tempBuf->Des().Copy(policyDetails->iName);
    
    LOG_EVENT(R_VPN_MSG_DELETED_POLICY, tempBuf, NULL, 0, 0);

    CleanupStack::PopAndDestroy(2); //tempBuf, policyDetails
    
    aMessage.Complete(KErrNone);
    }

void CVpnApiServant::ChangePasswordL(const RMessage2& aMessage)
    {
    if (iPwdChanger)
        {
        aMessage.Complete(KVpnErrPwdChangeOngoing);
        }
    else
        {
        iPwdChanger = CPwdChanger::NewL(aMessage, *this);
        iPwdChanger->ChangePassword();
        }
    }

void CVpnApiServant::CancelChangePassword(const RMessage2& aMessage)
    {
    if (iPwdChanger)
        {
        iPwdChanger->Cancel();
        delete iPwdChanger;
        iPwdChanger = NULL;
        }
    
    aMessage.Complete(KErrNone);
    }

void CVpnApiServant::PasswordChangeComplete()
    {
    delete iPwdChanger;
    iPwdChanger = NULL;
    }

void CVpnApiServant::GetPolicySizeL(const RMessage2& aMessage)
    {
    TVpnPolicyId policyId;
    TPckg<TVpnPolicyId> pckgPolicyId(policyId);

    aMessage.ReadL(FIRST_ARGUMENT, pckgPolicyId);

    TInt policySize = PolicySizeL(policyId);

    TPckg<TInt> pckgPolicySize(policySize);
    aMessage.WriteL(SECOND_ARGUMENT, pckgPolicySize);

    aMessage.Complete(KErrNone);
    }

void CVpnApiServant::GetPolicyDataL(const RMessage2& aMessage)
    {
    TVpnPolicyId policyId;
    TPckg<TVpnPolicyId> pckgPolicyId(policyId);

    aMessage.ReadL(FIRST_ARGUMENT, pckgPolicyId);

    TInt expectedPolicySize;
    TPckg<TInt> pckgExpectedPolicySize(expectedPolicySize);

    aMessage.ReadL(SECOND_ARGUMENT, pckgExpectedPolicySize);

    TInt currentPolicySize = PolicySizeL(policyId);

    if (expectedPolicySize != currentPolicySize)
        {
        aMessage.Complete(KVpnErrPolicySizeChanged);
        return;
        }

    HBufC8* policyData = NULL;
    User::LeaveIfError(iPolicyStore->LoadPolicyDataL(policyId, policyData));
    CleanupStack::PushL(policyData);
    
    aMessage.WriteL(THIRD_ARGUMENT, *policyData);

    CleanupStack::PopAndDestroy(); // policyData

    aMessage.Complete(KErrNone);
    }
    
CPolicyStore* CVpnApiServant::PolicyStore()
    {
    return iPolicyStore;
    }

TInt CVpnApiServant::PolicySizeL(const TVpnPolicyId& aPolicyId)
    {
    HBufC8* policyData = NULL;
    
    User::LeaveIfError(iPolicyStore->LoadPolicyDataL(aPolicyId, policyData));
    TInt policySize = policyData->Size();
    delete policyData;
    
    return policySize;
    }

// New methods to facilitate OMA DM based VPN policy management

void CVpnApiServant::AddPolicyL(const RMessage2& aMessage)
    {
    
    LOG(Log::Printf(_L("CVpnApiServant::AddPolicyL")));    
    
    // Read policy details
    TVpnPolicyDetails *policyDetails = new (ELeave) TVpnPolicyDetails;
    CleanupStack::PushL(policyDetails);
    
    TPckg<TVpnPolicyDetails> pckgPolicyDetails(*policyDetails);

    aMessage.ReadL(FIRST_ARGUMENT, pckgPolicyDetails);

    // Read policy data
    TInt len = aMessage.GetDesLength(SECOND_ARGUMENT);
    HBufC8* policyData = HBufC8::NewLC(len);
    TPtr8 ptrPolicyData = policyData->Des();
    aMessage.ReadL(SECOND_ARGUMENT, ptrPolicyData);

    //Make a validy check to the policy data
    //by parsing it and checking certificate formats
    HBufC* policyData16 = HBufC::NewLC(policyData->Length());
    policyData16->Des().Copy(*policyData);
    
    CIkeDataArray* ikeDataArray = CIkeDataArray::NewL(1);
    CleanupStack::PushL(ikeDataArray);
    
    TIkeParser* ikeParser = new (ELeave) TIkeParser(*policyData16);
    CleanupStack::PushL(ikeParser);
    ikeParser->ParseIKESectionsL(ikeDataArray);

    for (TInt i = 0; i < ikeDataArray->Count(); ++i)
        {
        const CIkeData* ikeData = ikeDataArray->At(i);
        if (ikeData->iCAList != NULL)
            {
            for (TInt j = 0; j < ikeData->iCAList->Count(); j++)
                {
                if (ikeData->iCAList->At(j)->iFormat == BIN_CERT)
                    {
                    LOG(Log::Printf(_L("Policy contains BIN certificates --> Failing")));
                    //Ca cert in wrong format --> Error
                    User::Leave(KVpnErrInvalidPolicyFile);
                    }
                }            
            }
        if ((ikeData->iOwnCert.iData.Length() > 0 &&
             ikeData->iOwnCert.iFormat == BIN_CERT) ||
            (ikeData->iPrivKey.iData.Length() > 0 &&
             ikeData->iPrivKey.iFormat == BIN_CERT) ||
            (ikeData->iPeerCert.iData.Length() > 0 &&
            ikeData->iPeerCert.iFormat == BIN_CERT))
            {
            LOG(Log::Printf(_L("Policy contains BIN certificates --> Failing")));    
            //Key or user cert in wrong format
            User::Leave(KVpnErrInvalidPolicyFile);
            }        
        }
    
    CleanupStack::PopAndDestroy(); //ikeParser
    CleanupStack::PopAndDestroy(ikeDataArray);
    CleanupStack::PopAndDestroy(policyData16);
    
    LOG(Log::Printf(_L("Calling: iPolicyStore->AddNewPolicyL")));    
    TRAPD(err, iPolicyStore->AddNewPolicyL(*policyDetails, *policyData));
    if (err == KErrNone)
    {
        TBuf<20> serverName(_L("VPN DM Server"));
        TPtrC8 tempCastPtr8(reinterpret_cast<const TUint8*>(serverName.Ptr()), serverName.Length() * 2);
        LOG_EVENT(R_VPN_MSG_INSTALLED_POLICY_SERVER, &(policyDetails->iId),
                &tempCastPtr8, err, EFalse);
    }
    else
    {
        LOG_EVENT(R_VPN_MSG_INSTALLED_POLICY_SERVER, &(policyDetails->iId), NULL,
                 err, EFalse);
        User::Leave(err);
    }    

    LOG(Log::Printf(_L("Writing back policy details")));    
    // Write back the possibly changed policy details
    aMessage.WriteL(FIRST_ARGUMENT, pckgPolicyDetails);

    CleanupStack::PopAndDestroy(policyData);    
    CleanupStack::PopAndDestroy(); //policyDetails

    aMessage.Complete(KErrNone);
    }

void CVpnApiServant::UpdatePolicyDetailsL(const RMessage2& aMessage)
    {
    LOG(Log::Printf(_L("CVpnApiServant::UpdatePolicyDetailsL")));    
    
    // Read policy details
    TVpnPolicyDetails* policyDetails = new (ELeave) TVpnPolicyDetails;
    CleanupStack::PushL(policyDetails);
    
    TPckg<TVpnPolicyDetails> pckgPolicyDetails(*policyDetails);

    aMessage.ReadL(FIRST_ARGUMENT, pckgPolicyDetails);

    // Update the policy details
    TRAPD(err, iPolicyStore->UpdatePolicyDetailsL(*policyDetails));
    if (err != KErrNone)
    {
        LOG_EVENT(R_VPN_MSG_INSTALLED_POLICY_SERVER, &(policyDetails->iId), NULL,
                 err, ETrue);
        User::Leave(err);
    }

    // Write back the possibly changed policy details
    aMessage.WriteL(FIRST_ARGUMENT, pckgPolicyDetails);

    CleanupStack::PopAndDestroy(); //policyDetails
    
    aMessage.Complete(KErrNone);
    }
    
void CVpnApiServant::UpdatePolicyDataL(const RMessage2& aMessage)
    {
    // Read policy ID
    TVpnPolicyId policyId;
    TPckg<TVpnPolicyId> pckgPolicyId(policyId);

    aMessage.ReadL(FIRST_ARGUMENT, pckgPolicyId);

    // Read policy data
    TInt len = aMessage.GetDesLength(SECOND_ARGUMENT);
    HBufC8* policyData = HBufC8::NewL(len);
    CleanupStack::PushL(policyData);

    TPtr8 ptrPolicyData = policyData->Des();
    aMessage.ReadL(SECOND_ARGUMENT, ptrPolicyData);

    // Update the policy data
    TRAPD(err, iPolicyStore->UpdatePolicyDataL(policyId, *policyData));
    if (err == KErrNone)
    {
        TBufC8<20> serverName(_L8("VPN DM Server"));
        LOG_EVENT(R_VPN_MSG_INSTALLED_POLICY_SERVER, &policyId,
                &serverName, err, ETrue);
    }
    else
    {
        LOG_EVENT(R_VPN_MSG_INSTALLED_POLICY_SERVER, &policyId, NULL,
                 err, ETrue);
        User::Leave(err);
    }
    CleanupStack::PopAndDestroy(); // policyData

    aMessage.Complete(KErrNone);
    }
