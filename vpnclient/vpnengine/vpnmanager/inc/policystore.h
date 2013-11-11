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
* Description:   Policy store
*
*/



#ifndef __POLICYSTORE_H__
#define __POLICYSTORE_H__

#include <e32std.h>
#include <e32base.h>
#include <es_sock.h>
#include <es_enum.h>
#include <cmmanager.h>

#include "fileutil.h"
#include "vpnapidefs.h"
#include "pkiutil.h"

const TInt KUnfoundIndex( KErrNotFound );

class RConnection;
class CVpnApiServant;
class CIkeData;
class RPKIServiceAPI;

 /**
 *  Maintains the VPN policy store.
 *
 *  @lib internal (vpnmanager.exe)
 *  @since S60 3.0
 */
class CPolicyStore : public CBase
    {
public:
    static CPolicyStore* NewL(RFs& aFs);
    ~CPolicyStore();

    void ImportPolicyL(const TFileName& aPinFile, const TFileName& aPolFile,
                       TVpnPolicyId* aNewPolicyId = NULL);
    TInt DeletePolicyL(const TVpnPolicyId& aPolicyId);
    void ReplacePolicyL(const TVpnPolicyId& aPolicyToReplace,
                        const TVpnPolicyId& aReplacingPolicy);
    TInt PolicyCount();
    TInt GetPolicyInfo(const TVpnPolicyId& aPolicyId, 
                       TVpnPolicyInfo& aPolicyInfo);
    TInt GetPolicyDetailsL(const TVpnPolicyId& aPolicyId, 
                           TVpnPolicyDetails& aPolicyDetails);
                           
                           
    TInt LoadPolicyDataL(const TVpnPolicyId& aPolicyId, HBufC8*& aPolicyData);
    
    TUint8* RawPolicyData();

    void AddNewPolicyL(TVpnPolicyDetails& aPolicyDetails, const TDesC8& aPolicyData);
    void UpdatePolicyDetailsL(TVpnPolicyDetails& aPolicyDetails);
    void UpdatePolicyDataL(const TVpnPolicyId& aPolicyId, const TDesC8& aPolicyData);
    
private: // C'tor
    CPolicyStore(RFs& aFs);
    void ConstructL();

    void ConstructPolicyListL();

private: // implementation
    void AddPolicyL(const TVpnPolicyId& aPolicyId);
    TVpnPolicyId NewPolicyIdL();
    TVpnPolicyInfo* PolicyInfo(const TVpnPolicyId& aPolicyId);
    TInt PolicyIndex(const TVpnPolicyId& aPolicyId);
    TInt PolicyIndexVisible(const TVpnPolicyId& aPolicyId);
    TPolicyUsageStatus PolicyUsageStatusL(const TVpnPolicyId& aPolicyId );
    TBool PolicyAssignedToIapL(RCmManager& aCmManager,
                               const TVpnPolicyId& aPolicyId,
                               TBool& aPolicyActive);
    void ConnectionMethodsLC(RArray<TUint32>& aVpnConnections,
                             RCmManager& aCmManager,
                             const TVpnPolicyId& aPolicyId,
                             TBool& aPolicyActive);
    TPolicyPkiStatus PolicyPkiStatusL(const TVpnPolicyId& aPolicyId);
 
    TCertStatus PolicyCertificateStatusL(CIkeData* aIkeData) const;
    
    void EnsureUniquePolicyNameL(const TFileName& aPinFile);
    TBool DoEnsureUniquePolicyNameL(TVpnPolicyDetails& aPolicyDetails);    
    TBool PolicyNameExists(const TDesC& aPolicyName);
    HBufC* MakeNewPolicyNameL(const TDes& aOriginalPolicyName, TInt aSequenceNumber);
    TBool IsHiddenPolicyL(const TVpnPolicyId& aPolicyId);
    void UpdateVisiblePolicyInfoL(const TVpnPolicyDetails& aPolicyDetails);    
    
private: // data 
    RFs& iFs;
    TFileUtil iFileUtil;
    /// Own: Policy list
    CArrayFixFlat<TVpnPolicyInfo>* iPolicyListAll;
    /// Own: List of visible policies
    CArrayFixFlat<TVpnPolicyInfo>* iPolicyListVisible;
    };

 /**
 *  Internal helper class
 *
 *  @lib internal (vpnmanager.exe)
 *  @since S60 3.0
 */
class CFileInfoContainer : public CBase
    {
public:
    ~CFileInfoContainer() 
        {
        delete iPinFileToReplace;
        delete iPolFileToReplace;
        delete iReplacingPinFile;
        delete iReplacingPolFile;
        };

    HBufC* iPinFileToReplace;
    HBufC* iPolFileToReplace;
    HBufC* iReplacingPinFile;
    HBufC* iReplacingPolFile;
    TVpnPolicyDetails iOldPolicyDetails;
    TVpnPolicyDetails iNewPolicyDetails;
    };

#endif // __POLICYSTORE_H__
