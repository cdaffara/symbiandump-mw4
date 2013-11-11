/*
* Copyright (c) 2000-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Policy store
*
*/



#include <e32math.h>
#include <cmconnectionmethod.h>
#include <cmpluginvpndef.h> // vpn plugin
#include "policystore.h"
#include "pinparser.h"
#include "ikepolparser.h"
#include "pkiserviceapi.h"
#include "policyimporter.h"
#include "uuid.h"
#include "log_r6.h"

CPolicyStore* CPolicyStore::NewL(RFs& aFs)
    {
    CPolicyStore* self = new (ELeave) CPolicyStore(aFs);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(); // self
    return self;
    }

CPolicyStore::CPolicyStore(RFs& aFs) : iFs(aFs), iFileUtil(aFs)
    {
    }

inline void CPolicyStore::ConstructL()
    {
    LOG_("CPolicyStore::ConstructL");    
    ConstructPolicyListL();
    LOG_("CPolicyStore::ConstructL:end");    
    }

CPolicyStore::~CPolicyStore()
    {
    delete iPolicyListAll;
    delete iPolicyListVisible;
    }

void CPolicyStore::ConstructPolicyListL()
    {
    // Using two policy lists (one for all and one for the visible
    // ones) makes it easy to support the concept of hidden and
    // visible policies. There's a slight memory penalty but this
    // is not serious as the number of policies is typically small).
    // Hidden policies differ from visible ones in only one aspect - 
    // they are not included in the policy listing returned to
    // the caller.
    
    // A list of all policies (both visible and hidden)
    iPolicyListAll = new (ELeave) CArrayFixFlat<TVpnPolicyInfo>(2);
    // A list of visible policies only
    iPolicyListVisible = new (ELeave) CArrayFixFlat<TVpnPolicyInfo>(2);

    TFindFile fileFinder(iFs);
    CDir* fileList; 

    TPath privateDir;
    User::LeaveIfError(iFs.PrivatePath(privateDir));
    TInt ret = fileFinder.FindWildByDir(KPolFilePat, privateDir, fileList);
    if (ret == KErrNone)
        {
        CleanupStack::PushL(fileList);
        
        for (TInt i = 0; i < fileList->Count(); i++)
            {
            TParse fileNameParser;
            fileNameParser.Set((*fileList)[i].iName, &fileFinder.File(), NULL);

            TVpnPolicyId policyId;
            
            // Only add the policy to the list its ID length is
            // acceptable (this is the case with all policies
            // that have been properly imported to the store)
            if (fileNameParser.Name().Length() <= policyId.MaxLength())
                {
                policyId.Copy(fileNameParser.Name());

                HBufC* pinFile = iFileUtil.GetPinFileNameLC(policyId);

                if (iFileUtil.FileExists(*pinFile))
                    {
                    AddPolicyL(policyId);
                    }
                CleanupStack::PopAndDestroy(pinFile);
                }
            }
        
        CleanupStack::PopAndDestroy(); // fileList
        }
    }

TUint8* CPolicyStore::RawPolicyData()
    {
    // The list of policies returned to the caller contains ONLY
    // visible policies. 
    return reinterpret_cast<TUint8*>(&(iPolicyListVisible->At(0)));
    }

void CPolicyStore::ImportPolicyL(
    const TFileName& aPinFile, const TFileName& aPolFile,
    TVpnPolicyId* aNewPolicyId)
    {
    LOG_("-> CPolicyStore::ImportPolicyL()");            
    TVpnPolicyId policyId = NewPolicyIdL();

    // Write the new policy ID to the caller
    if (aNewPolicyId)
        {
        aNewPolicyId->Copy(policyId);
        }

    // Create new PIN and POL file names
    // (Dynamic allocation to conserve stack space)
    HBufC* pinFileP = iFileUtil.GetPinFileNameLC(policyId);
    HBufC* polFileP = iFileUtil.GetPolFileNameLC(policyId);

    // Move the files from the install directory
    // to the target directory (the policy store)
    iFileUtil.CopyFileL(aPinFile, *pinFileP);
    iFileUtil.CopyFileL(aPolFile, *polFileP);

    CleanupStack::PopAndDestroy(polFileP);

    // Make sure that the name of the new policy
    // does not collide with an existing name
    EnsureUniquePolicyNameL(*pinFileP);

    CleanupStack::PopAndDestroy(pinFileP);
    
    // Add the imported policy to the policy list
    AddPolicyL(policyId);
    LOG(Log::Printf(_L("<- CPolicyStore::ImportPolicyL()")));            
    }

TInt CPolicyStore::DeletePolicyL(const TVpnPolicyId& aPolicyId)
    {
    LOG(Log::Printf(_L("-> CPolicyStore::DeletePolicyL()")));            
    // Delete the policy from the list of all policies
    
    TInt itemToDelete = PolicyIndex(aPolicyId);
    
    if (itemToDelete != KUnfoundIndex)
        {
        iPolicyListAll->Delete(itemToDelete);
        iPolicyListAll->Compress();
        }

    // Delete the policy also from the list of
    // visible policies if it happens to be there
    
    TInt itemToDeleteVisible = PolicyIndexVisible(aPolicyId);
    
    if (itemToDeleteVisible != KUnfoundIndex)
        {
        iPolicyListVisible->Delete(itemToDeleteVisible);
        iPolicyListVisible->Compress();
        }

    // Delete the PIN and POL files
    HBufC* pinFile = iFileUtil.GetPinFileNameLC(aPolicyId);    
    HBufC* polFile = iFileUtil.GetPolFileNameLC(aPolicyId);

    if (iFileUtil.FileExists(*pinFile))
        {
        iFileUtil.DeleteFileL(*pinFile); 
        }
    
    if (iFileUtil.FileExists(*polFile))
        {
        iFileUtil.DeleteFileL(*polFile);
        }
    CleanupStack::PopAndDestroy(2); // polFile, pinFile
    LOG(Log::Printf(_L("<- CPolicyStore::DeletePolicyL()")));            
    return KErrNone;
    }
    
TInt CPolicyStore::PolicyCount()
    {
    // The list of policies returned to the caller contains ONLY
    // visible policies. Thus we return just the count of those.
    return iPolicyListVisible->Count();
    }

TInt CPolicyStore::GetPolicyInfo(const TVpnPolicyId& aPolicyId, TVpnPolicyInfo& aPolicyInfo)
    {
    TInt policyIndex = PolicyIndex(aPolicyId);
    
    if (policyIndex == KUnfoundIndex)
        {
        return KVpnErrPolicyNotFound;
        }
    else
        {
        aPolicyInfo = iPolicyListAll->At(policyIndex);
        return KErrNone;
        }
    }

TInt CPolicyStore::GetPolicyDetailsL(
    const TVpnPolicyId& aPolicyId, 
    TVpnPolicyDetails& aPolicyDetails)
    {
    LOG(Log::Printf(_L("-> CPolicyStore::GetPolicyDetailsL()")));            
    TInt policyIndex = PolicyIndex(aPolicyId);
    
    if (policyIndex == KUnfoundIndex)
        {
        LOG(Log::Printf(_L("<- CPolicyStore::GetPolicyDetailsL() (KVpnErrPolicyNotFound)")));
        return KVpnErrPolicyNotFound;
        }
    else
        {
        STACK_LEFT;
        
        // Get static policy information from the PIN file
        TPinParser* pinParser = new (ELeave) TPinParser(iFileUtil);
        CleanupStack::PushL(pinParser);

        HBufC* pinFile = iFileUtil.GetPinFileNameLC(aPolicyId);
        pinParser->ParsePolicyDetailsL(*pinFile, aPolicyDetails);

        CleanupStack::PopAndDestroy(2); // pinFile, pinParser

        // Find out dynamic policy information from the system
        aPolicyDetails.iPkiStatus = PolicyPkiStatusL(aPolicyId);
        aPolicyDetails.iUsageStatus = 
            PolicyUsageStatusL(aPolicyId);
        // Set the policy ID as well
        aPolicyDetails.iId.Copy(aPolicyId);
        
        LOG(Log::Printf(_L("<- CPolicyStore::GetPolicyDetailsL()")));
        return KErrNone;
        }
    }

TInt CPolicyStore::LoadPolicyDataL(const TVpnPolicyId& aPolicyId, 
    HBufC8*& aPolicyData)
    {
    TInt policyIndex = PolicyIndex(aPolicyId);
    
    if (policyIndex == KUnfoundIndex)
        {
        return KVpnErrPolicyNotFound;
        }
    HBufC* polFile = iFileUtil.GetPolFileNameLC(aPolicyId);
    
    aPolicyData = iFileUtil.LoadFileDataL(*polFile);

    CleanupStack::PopAndDestroy();
    return KErrNone;
    }

void CPolicyStore::AddPolicyL(const TVpnPolicyId& aPolicyId)
    {
    LOG(Log::Printf(_L("-> CPolicyStore::AddPolicyL")));    

    TVpnPolicyInfo* policyInfo = new (ELeave) TVpnPolicyInfo();
    CleanupStack::PushL(policyInfo);

    policyInfo->iId = aPolicyId;
    // Policy name is read from the PIN file
    HBufC* pinFile = iFileUtil.GetPinFileNameLC(aPolicyId);

    TPinParser* pinParser = new (ELeave) TPinParser(iFileUtil);
    CleanupStack::PushL(pinParser);

    pinParser->ParsePolicyInfoL(*pinFile, *policyInfo);
    
    CleanupStack::PopAndDestroy(pinParser);
    CleanupStack::PopAndDestroy(pinFile);

    LOG(Log::Printf(_L("pinParser.ParsePolicyInfoL completed")));
    iPolicyListAll->AppendL(*policyInfo);

    // A visible policy is added also
    // to the list of visible policies
    if (!IsHiddenPolicyL(policyInfo->iId))
        {
        iPolicyListVisible->AppendL(*policyInfo);
        }

    CleanupStack::PopAndDestroy(policyInfo);
    LOG(Log::Printf(_L("<- CPolicyStore::AddPolicyL")));            
    }

TVpnPolicyId CPolicyStore::NewPolicyIdL()
    {
    TUuid uuid;
    TUuidString uuidString;
    
    Uuid::MakeUuidL(uuid);

    // It has become apparent that certain
    // Symbian OS devices generate duplicate random
    // number sequences after gold boot due to improper
    // seeding of the random number generator. Should
    // this happen to be the case, we insert at least
    // one component to the UUID that depends on the 
    // current system time. This is not perfect, but 
    // should give us policy IDs that are unique enough.
    TTime now;
	now.UniversalTime();
    TInt64 randSeed = now.Int64();
    TInt randomNum = Math::Rand(randSeed);
    uuid.iTimeLow = static_cast<TUint32>(randomNum);
    
    Uuid::UuidToString(uuid, uuidString);

    TVpnPolicyId newPolicyId;
    newPolicyId.Copy(uuidString);
    
    return newPolicyId;
    }
    
TInt CPolicyStore::PolicyIndex(const TVpnPolicyId& aPolicyId)
    {
    for (TInt i = 0; i < iPolicyListAll->Count(); i++)
        {
        if (iPolicyListAll->At(i).iId.Compare(aPolicyId) == 0)
            {
            return i;
            }
        }
    LOG_1("CPolicyStore::PolicyIndex not found:%S", &aPolicyId);
    return KUnfoundIndex;
    }

TInt CPolicyStore::PolicyIndexVisible(const TVpnPolicyId& aPolicyId)
    {
    for (TInt i = 0; i < iPolicyListVisible->Count(); i++)
        {
        if (iPolicyListVisible->At(i).iId.Compare(aPolicyId) == 0)
            {
            return i;
            }
        }
    return KUnfoundIndex;
    }
    
TPolicyUsageStatus CPolicyStore::PolicyUsageStatusL(
    const TVpnPolicyId& aPolicyId )
    {
    
    LOG(Log::Printf(_L("-> CPolicyStore::PolicyUsageStatusL")));
    
    STACK_LEFT;
    
    TPolicyUsageStatus usageStatus = EUsageStatusUnused;

using namespace CMManager;    
    RCmManager cmManager;
    cmManager.OpenL(); // Do not use LC it's not working yet
    CleanupClosePushL( cmManager );
    
    TBool policyActive(EFalse);
    if (PolicyAssignedToIapL(cmManager, aPolicyId, policyActive))
        {
        usageStatus = policyActive ? EUsageStatusActive : 
            EUsageStatusAssignedToIap;
        }
    CleanupStack::PopAndDestroy(); // cmManager
    
    LOG(Log::Printf(_L("<- CPolicyStore::PolicyUsageStatusL")));
    return usageStatus;
    }

TBool CPolicyStore::PolicyAssignedToIapL(
    RCmManager& aCmManager,
    const TVpnPolicyId& aPolicyId,
    TBool& aPolicyActive)
    {
    STACK_LEFT;
             
    RArray<TUint32> vpnConnections;
    ConnectionMethodsLC( 
        vpnConnections, aCmManager, aPolicyId, aPolicyActive );
    
    TBool policyAssignedToIap( vpnConnections.Count() ? ETrue : EFalse );
    CleanupStack::PopAndDestroy(); // vpnConnections
    LOG_1("CPolicyStore::PolicyAssignedToIapL %d", policyAssignedToIap); 
    return policyAssignedToIap;
    }

void CPolicyStore::ConnectionMethodsLC(
    RArray<TUint32>& aVpnConnections,
    RCmManager& aCmManager,
    const TVpnPolicyId& aPolicyId,
    TBool& aPolicyActive)
    {
using namespace CMManager;         
    LOG_1("CPolicyStore::ConnectionMethodsLC : %S", &aPolicyId);
    CleanupClosePushL( aVpnConnections );

    aCmManager.ConnectionMethodL( 
        aVpnConnections,
        ETrue,
        EFalse,
        EFalse ); 
    LOG_1("CPolicyStore::ConnectionMethodsLC cnt: %d", 
        aVpnConnections.Count());
    
    TInt index(aVpnConnections.Count());
    
    while( index )
        {
        --index;
        LOG_1("CPolicyStore::ConnectionMethodsLC at: %d", index);
        TUint32 id(aVpnConnections[index]);
        RCmConnectionMethod connectioMethod = 
            aCmManager.ConnectionMethodL( id );
        CleanupClosePushL( connectioMethod );  
        if( connectioMethod.GetIntAttributeL(ECmBearerType) != 
            KPluginVPNBearerTypeUid )
            {
            aVpnConnections.Remove( index );
            }
        else
            {
            HBufC* policyId = connectioMethod.GetStringAttributeL(
                EVpnServicePolicy);
            if( aPolicyId.Compare( *policyId ) )
                {
                aVpnConnections.Remove( index );
                }
            else
                {
                if( connectioMethod.GetBoolAttributeL(ECmConnected) )
                    {
                    LOG_1("PolicyActiveL:%d", index);
                    aPolicyActive = ETrue;
                    }
                }
            delete policyId;      
            }
        CleanupStack::PopAndDestroy(); // connectioMethod 
        }
    }

TPolicyPkiStatus CPolicyStore::PolicyPkiStatusL(const TVpnPolicyId& aPolicyId)
    {
    HBufC8* policyData;
    
    if (LoadPolicyDataL(aPolicyId, policyData) != KErrNone)
        {
        return EPkiStatusUnknown;
        }
    
    CleanupStack::PushL(policyData);

    HBufC* policyData16 = HBufC::NewL(policyData->Length());
    CleanupStack::PushL(policyData16);
    
    policyData16->Des().Copy(*policyData);

    CIkeDataArray* ikeDataArray = CIkeDataArray::NewL(1);
    CleanupStack::PushL(ikeDataArray);
    
    TIkeParser ikeParser(*policyData16);
    ikeParser.ParseIKESectionsL(ikeDataArray);

    // Go through each IKE section (VPN gateway definition)
    // to find out the collective policy PKI status:
    // - EPkiStatusReady if a valid user certificate is present for
    // each VPN gateway defined in the policy
    // - EPkiStatusNoCert if no user certificate is present for
    // one or more VPN gateway defined in the policy
    // - EPkiStatusCertExpired if a user certificate related
    // to one or more VPN gateway has expired
    // - EPkiStatusCertNotValidYet if a user certificate related
    // to one or more VPN gateway is not yet valid
    
    TPolicyPkiStatus policyPkiStatus = EPkiStatusReady;
    
    for (TInt i = 0; i < ikeDataArray->Count(); i++)
        {
        CIkeData* ikeData = ikeDataArray->At(i);

        TCertStatus gwPkiStatus = PolicyCertificateStatusL(ikeData);

        // Determine the overall policy PKI status
        
        if (gwPkiStatus == ECertValid ||
            gwPkiStatus == ECertNotNeeded)
            {
            continue;
            }
        else if (gwPkiStatus == ECertNotFound)
            {
            policyPkiStatus = EPkiStatusNoCert;
            break;
            }
        else if (gwPkiStatus == ECertExpired)
            {
            policyPkiStatus = EPkiStatusCertExpired;
            break;
            }
        else if (gwPkiStatus == ECertNotValidYet)
            {
            policyPkiStatus = EPkiStatusCertNotValidYet;
            }
        }

    CleanupStack::PopAndDestroy(3); // ikeDataArray, policyData16, policyData
    
    return policyPkiStatus;
    }
         


TCertStatus CPolicyStore::PolicyCertificateStatusL(CIkeData* aIkeData) const
    {
    LOG(Log::Printf(_L("-> CPolicyStore::PolicyCertificateStatusL()")));
    TCertStatus status = ECertValid;

    CArrayFixFlat<TCertInfo*> *cAList = aIkeData->iCAList;
    if ((cAList == NULL || cAList->Count() == 0) && 
        !aIkeData->iOwnCert.iOwnCertExists)
        {
        status = ECertNotNeeded;
        }
    else
        {        
        RPKIServiceAPI pkiService;
        User::LeaveIfError(pkiService.Connect());
        CleanupClosePushL(pkiService);

        pkiService.SetInformational(ETrue);
                    
        CDesC8ArrayFlat* caSubjectNameArray = new (ELeave) CDesC8ArrayFlat(2);
        CleanupStack::PushL(caSubjectNameArray);                        
            
        if (cAList != NULL && cAList->Count() > 0)
            {                    
            status = PkiUtil::GetValidCaCertSubjectNameListL(pkiService, *cAList,
                                                             *caSubjectNameArray);        
            }
            
        if (status == ECertValid)
            {
			// Set store type to device store,
			// if Own_cert_type is defined as "DEVICE"            
			if ( aIkeData->iClientCertType != NULL )
			  	{
				TPtrC16 certStoreType = aIkeData->iClientCertType->GetData();
				if ( certStoreType.CompareF(_L("DEVICE")) == 0 )
					{
					LOG(Log::Printf(_L("Set store type to STORETYPE_DEVICE")));    
					pkiService.SetStoreType(EPkiStoreTypeDevice);            
					}
                else
                    {
					LOG(Log::Printf(_L("Set store type to STORETYPE_USER")));    
					pkiService.SetStoreType(EPkiStoreTypeUser);                                
                    }
				}
            else
                {
				LOG(Log::Printf(_L("Set store type to STORETYPE_USER")));    
				pkiService.SetStoreType(EPkiStoreTypeUser);                                                
                }
            status = PkiUtil::CheckUserCertValidityL(pkiService, *caSubjectNameArray,
                                                     aIkeData->iOwnCert);
            }
        CleanupStack::PopAndDestroy(caSubjectNameArray);
        CleanupStack::PopAndDestroy(); //pkiService            
        }
	LOG(Log::Printf(_L("<- CPolicyStore::PolicyCertificateStatusL()")));        
    return status;
    }



void CPolicyStore::EnsureUniquePolicyNameL(const TFileName& aPinFile)
    {
    LOG_("-> CPolicyStore::EnsureUniquePolicyNameL");

    // Dynamic allocations to conserve stack space

    TVpnPolicyDetails* policyDetails = new (ELeave) TVpnPolicyDetails();
    CleanupStack::PushL(policyDetails);

    TPinParser* pinParserP = new (ELeave) TPinParser(iFileUtil);
    CleanupStack::PushL(pinParserP);
            
    pinParserP->ParsePolicyDetailsL(aPinFile, *policyDetails);

    TBool policyNameChanged = DoEnsureUniquePolicyNameL(*policyDetails);

    if (policyNameChanged)
        {
        HBufC* pinFileContent = pinParserP->PolicyDetailsAsTextL(*policyDetails);
        CleanupStack::PushL(pinFileContent);

        iFileUtil.SaveFileDataL(aPinFile, *pinFileContent);

        CleanupStack::PopAndDestroy(pinFileContent); // pinFileContent
        }
    CleanupStack::PopAndDestroy(); // pinParserP
    CleanupStack::PopAndDestroy(); // policyDetails

    LOG_("<- CPolicyStore::EnsureUniquePolicyNameL");
    }

TBool CPolicyStore::DoEnsureUniquePolicyNameL(TVpnPolicyDetails& aPolicyDetails)     
    {
    
    LOG(Log::Printf(_L("-> CPolicyStore::DoEnsureUniquePolicyNameL")));    
    TBool nameChanged = EFalse;
    TBool isUnique = EFalse;
    
    HBufC* newName = aPolicyDetails.iName.AllocLC();
    TInt sequenceNumber = 2;
    
    while (!isUnique && sequenceNumber < KMaxTInt)
        {
        if (PolicyNameExists(*newName))
            {
            if (newName != NULL) 
                {
                CleanupStack::PopAndDestroy(newName);
                LOG_(" Pop (newName)");
                newName = NULL;
                }
            newName = MakeNewPolicyNameL(aPolicyDetails.iName, sequenceNumber);
            CleanupStack::PushL(newName);

            sequenceNumber++;

            nameChanged = ETrue;
            }
        else
            {
            isUnique = ETrue;
            }
        }

    if (nameChanged)
        {
        // We now have a new unique policy name so we can save it
        // Make sure there's no risk of overflow
        aPolicyDetails.iName.Copy(newName->Left(aPolicyDetails.iName.MaxLength()));
        }
    
    if (newName != NULL) 
        {
        CleanupStack::PopAndDestroy(); // newName
        }
    
    LOG(Log::Printf(_L("<- CPolicyStore::DoEnsureUniquePolicyName")));    
    return nameChanged;
    }

TBool CPolicyStore::PolicyNameExists(const TDesC& aPolicyName)
    {
    LOG_("-> CPolicyStore::PolicyNameExists");
    TBool policyNameExists = EFalse;
    
    // Check the name against the existing names
    for (TInt i = 0; i < iPolicyListAll->Count(); i++)
        {
        // Ignore case in the name comparison
        if (iPolicyListAll->At(i).iName.CompareF(aPolicyName) == 0)
            {
            policyNameExists = ETrue;
            break;
            }
        }

    LOG_("<- CPolicyStore::PolicyNameExists");    
    return policyNameExists;
    }

HBufC* CPolicyStore::MakeNewPolicyNameL(const TDes& aOriginalPolicyName, 
                                     TInt aSequenceNumber)
    {
    LOG_("-> CPolicyStore::MakeNewPolicyName");

    const TInt KMaxSequenceNumberStringLength = 32;
    _LIT(KSequenceNumberFormat, "(%d)");
    HBufC* sequenceNumberString = HBufC::NewLC(KMaxSequenceNumberStringLength);
    sequenceNumberString->Des().Format(KSequenceNumberFormat, aSequenceNumber);

    // New name string to be returned
    HBufC* retBuf(NULL);
    // Usage of Trim method in PinParser decreases the length by 2 items
    TInt spaceLeft = aOriginalPolicyName.MaxLength() - 2 - aOriginalPolicyName.Length();
    TInt sequenceNumberStringLength = sequenceNumberString->Length();
                                      
    if (sequenceNumberStringLength <= spaceLeft)
        {
        // There's enough free space for the sequence
        // number, so we can just add append it
        LOG_1(" Sequence number string: '%S'", &(*sequenceNumberString)); 

        // Determine final string length for dynamic allocation
        TInt len = aOriginalPolicyName.Length() + sequenceNumberString->Length();
        retBuf = HBufC::NewLC(len);

        // Construct the final name string
        TPtr16 ptr = retBuf->Des();
        ptr.Append(aOriginalPolicyName.Left(aOriginalPolicyName.Length()));
        ptr.Append(sequenceNumberString->Left(sequenceNumberString->Length()));
        }
    else
        {
        // There's not enough space for the sequence
        // number so we override the end of the policy
        // name with the sequence number
        TInt lengap = sequenceNumberStringLength - spaceLeft;

        // Determine final string length for dynamic allocation
        TInt len = aOriginalPolicyName.MaxLength();
        retBuf = HBufC::NewLC(len);

        // Construct the final name string
        TPtr16 ptr = retBuf->Des();
        ptr.Append(aOriginalPolicyName.Left(aOriginalPolicyName.Length() - lengap));
        ptr.Append(sequenceNumberString->Left(sequenceNumberString->Length()));
        }

    // Just pop, and delete the non-returned value explicitly
    CleanupStack::Pop(2); // retBuf, sequenceNumberString
    delete sequenceNumberString;

    LOG_("<- CPolicyStore::MakeNewPolicyName");
    return retBuf;
    }

void CPolicyStore::ReplacePolicyL(const TVpnPolicyId& aPolicyToReplace, const TVpnPolicyId& aReplacingPolicy)
    {
    CFileInfoContainer* fileInfo = new (ELeave) CFileInfoContainer();
    CleanupStack::PushL(fileInfo);

    // Construct PIN and POL file names for
    // both the old and the new policy

    // Ownership is transferred
    fileInfo->iPinFileToReplace = iFileUtil.GetPinFileNameL(aPolicyToReplace);
    fileInfo->iPolFileToReplace = iFileUtil.GetPolFileNameL(aPolicyToReplace);

    fileInfo->iReplacingPinFile = iFileUtil.GetPinFileNameL(aReplacingPolicy);
    fileInfo->iReplacingPolFile = iFileUtil.GetPolFileNameL(aReplacingPolicy);


    // Store the name of the old policy (the original name)
    TPinParser* pinParser = new (ELeave) TPinParser(iFileUtil);
    CleanupStack::PushL(pinParser);

    pinParser->ParsePolicyDetailsL(*fileInfo->iPinFileToReplace, fileInfo->iOldPolicyDetails);

    // Delete the old PIN and POL files (note that this
    // policy is NOT deleted from the policy list)
    
    iFileUtil.DeleteFileL(*fileInfo->iPinFileToReplace);
    iFileUtil.DeleteFileL(*fileInfo->iPolFileToReplace);

    // Rename the new PIN and POL files such
    // that appear just as updated old files
    
    iFileUtil.MoveFileL(*fileInfo->iReplacingPinFile, *fileInfo->iPinFileToReplace);
    iFileUtil.MoveFileL(*fileInfo->iReplacingPolFile, *fileInfo->iPolFileToReplace);

    // Delete the new policy from the policy list
    // (it has been imported normally and thus appears in the list)
    
    DeletePolicyL(aReplacingPolicy);

    // Because of import, the updated policy has a sequence number in
    // its name, get rid of this by reverting to the original policy name
    
    pinParser->ParsePolicyDetailsL(*fileInfo->iPinFileToReplace, fileInfo->iNewPolicyDetails);

    fileInfo->iNewPolicyDetails.iName.Copy(fileInfo->iOldPolicyDetails.iName);

    HBufC* pinFileContent = pinParser->PolicyDetailsAsTextL(fileInfo->iNewPolicyDetails);
    CleanupStack::PushL(pinFileContent);

    iFileUtil.SaveFileDataL(*fileInfo->iPinFileToReplace, *pinFileContent);

    CleanupStack::PopAndDestroy(3); // pinFileContent, pinParser, fileInfo
    }

// New methods to facilitate OMA DM based VPN policy management

void CPolicyStore::AddNewPolicyL(TVpnPolicyDetails& aPolicyDetails, const TDesC8& aPolicyData)
    {
    
    LOG(Log::Printf(_L("-> CPolicyStore::AddNewPolicyL()")));
    
    // A non-empty policy name must be given
    if (aPolicyDetails.iName.Length() == 0)
        {
        User::Leave(KErrArgument);
        }

    // Policy data must be present
    if (aPolicyData.Length() == 0)
        {
        User::Leave(KErrArgument);
        }

    // If a (globally unique) policy ID
    // has not been specified, create it
    if (aPolicyDetails.iId.Length() == 0)
        {
        aPolicyDetails.iId.Copy(NewPolicyIdL());
        }
                           
    // If a policy with the same globally unique ID is already
    // present, then we cannot be adding a new policy
    if (PolicyIndex(aPolicyDetails.iId) != KUnfoundIndex)
        {
        User::Leave(KErrAlreadyExists);
        }

    DoEnsureUniquePolicyNameL(aPolicyDetails);

    LOG(Log::Printf(_L("Policy name ensured")));                
                        
    // Save policy details to the PIN file
    TPinParser* pinParser = new (ELeave) TPinParser(iFileUtil);
    CleanupStack::PushL(pinParser);    

    HBufC* pinFileContent = pinParser->PolicyDetailsAsTextL(aPolicyDetails);
    CleanupStack::PushL(pinFileContent);

    HBufC* pinFile = iFileUtil.GetPinFileNameLC(aPolicyDetails.iId);

    iFileUtil.SaveFileDataL(*pinFile, *pinFileContent);

    CleanupStack::PopAndDestroy(3); // pinFile, pinFileContent, pinParser

    LOG(Log::Printf(_L(" File data saved")));                

    // Save policy data to the POL file    

    HBufC* polFile = iFileUtil.GetPolFileNameLC(aPolicyDetails.iId);

    iFileUtil.SaveFileDataL(*polFile, aPolicyData);
    CleanupStack::PopAndDestroy(polFile);
    
    // Add the policy to the policy list
    AddPolicyL(aPolicyDetails.iId);
    LOG(Log::Printf(_L("<- CPolicyStore::AddNewPolicyL()")));
    }

void CPolicyStore::UpdatePolicyDetailsL(TVpnPolicyDetails& aPolicyDetails)
    {
    TVpnPolicyId policyId;
    policyId.Copy(aPolicyDetails.iId);

    // The caller must specify the ID of the policy to be updated
    if (policyId.Length() == 0)
        {
        User::Leave(KErrArgument);
        }
    
    // A non-empty policy name must be specified, too
    if (aPolicyDetails.iName.Length() == 0)
        {
        User::Leave(KErrArgument);
        }
    
    // Make sure that the specified policy is present
    TInt policyIndex = PolicyIndex(policyId);
    if (policyIndex == KUnfoundIndex)
        {
        User::Leave(KVpnErrPolicyNotFound);
        }

    // See if the new name is different from the existing one
    if (iPolicyListAll->At(policyIndex).iName.CompareF(aPolicyDetails.iName) != 0)
        {
        // Make sure that the new name remains to be unique
        DoEnsureUniquePolicyNameL(aPolicyDetails);
        }
    
    // Save policy details to the PIN file
    TPinParser* pinParser = new (ELeave) TPinParser(iFileUtil);
    CleanupStack::PushL(pinParser);
    
    HBufC* pinFileContent = pinParser->PolicyDetailsAsTextL(aPolicyDetails);
    CleanupStack::PushL(pinFileContent);

    HBufC* pinFile = iFileUtil.GetPinFileNameLC(policyId);
    iFileUtil.SaveFileDataL(*pinFile, *pinFileContent);

    CleanupStack::PopAndDestroy(3); // pinFile, pinFileContent, pinParser

    // Update the policy name in the policy list as the name may have changed
    iPolicyListAll->At(policyIndex).iName.Copy(aPolicyDetails.iName);

    UpdateVisiblePolicyInfoL(aPolicyDetails);
    }

void CPolicyStore::UpdateVisiblePolicyInfoL(const TVpnPolicyDetails& aPolicyDetails)
    {
    TVpnPolicyId policyId = aPolicyDetails.iId;
            
    // Find out whether the policy is hidden or visible
    TBool isHiddenPolicy = IsHiddenPolicyL(policyId);
            
    // See if the policy is in the list of visible ones
    TInt policyIndexVisible = PolicyIndexVisible(policyId);

    // If the policy is NOT in the visible list
    if (policyIndexVisible == KUnfoundIndex)
        {
        // If the policy is _no_longer_ hidden, add
        // it to the list of visible policies
        if (!isHiddenPolicy)
            {
            iPolicyListVisible->AppendL(static_cast<TVpnPolicyInfo>(aPolicyDetails));
            }
        }
    else // The policy IS in the visible list
        {
        // If the policy is _now_ hidden, remove
        // it from the list of visible ones
        if (isHiddenPolicy)
            {
            iPolicyListVisible->Delete(policyIndexVisible);
            iPolicyListVisible->Compress();
            }
        else
            { 
            // Otherwise update the policy name of the policy in the
            // list of visible policies as the name may have changed
            iPolicyListVisible->At(policyIndexVisible).iName.Copy(aPolicyDetails.iName);
            }
        }
    }

void CPolicyStore::UpdatePolicyDataL(
    const TVpnPolicyId& aPolicyId, const TDesC8& aPolicyData)
    {
    // The caller must specify the ID of the policy to be updated
    if (aPolicyId.Length() == 0)
        {
        User::Leave(KErrArgument);
        }

    // Policy data must be present
    if (aPolicyData.Length() == 0)
        {
        User::Leave(KErrArgument);
        }
    
    // Make sure that the policy is present
    if (PolicyIndex(aPolicyId) == KUnfoundIndex)
        {
        User::Leave(KVpnErrPolicyNotFound);
        }

    // Save policy data to the POL file

    HBufC* polFile = iFileUtil.GetPolFileNameLC(aPolicyId);
    iFileUtil.SaveFileDataL(*polFile, aPolicyData);
    CleanupStack::PopAndDestroy(polFile);
    }

TBool CPolicyStore::IsHiddenPolicyL(const TVpnPolicyId& aPolicyId)
    {
    TVpnPolicyDetails* policyDetails = new (ELeave) TVpnPolicyDetails();
    CleanupStack::PushL(policyDetails);
        
    TPinParser* pinParser = new (ELeave) TPinParser(iFileUtil);
    CleanupStack::PushL(pinParser);

    HBufC* pinFile = iFileUtil.GetPinFileNameLC(aPolicyId);

    pinParser->ParsePolicyDetailsL(*pinFile, *policyDetails);
    CleanupStack::PopAndDestroy(2); // pinFile, pinParser
    
    // A policy is marked as hidden if it's description 
    // field contains the "hidden indicator" string
    if (policyDetails->iDescription.FindF(KHiddenPolicyIndicator) != 
        KErrNotFound)
        {
        CleanupStack::PopAndDestroy(); // policyDetails
        return ETrue;
        }
    else
        {
        CleanupStack::PopAndDestroy(); // policyDetails
        return EFalse;
        }
    }
