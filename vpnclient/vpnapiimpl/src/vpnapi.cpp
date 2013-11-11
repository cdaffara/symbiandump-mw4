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
* Description:  The VPN API allows Symbian OS applications and servers
*                to perform VPN-specific operations.
*
*/


#include "vpnapi.h"
#include "vpnmanagerserverdefs.h"
#include "clistatic.h"

EXPORT_C RVpnServ::RVpnServ() : RSessionBase()
/**
 * Constructor
 */
    {
    }

EXPORT_C TInt RVpnServ::Connect()
/**
 * Opens a connection (session) to the VPN Manager server.
 *
 * @return KErrNone if the connection succeeds, a system-wide error code
 * if not.
 */
    {
    const TInt KVpnManagerServerStackSize    = 0x4000;
    const TInt KVpnManagerServerInitHeapSize = 0x1000;
    const TInt KVpnManagerServerMaxHeapSize  = 0x1000000;

    TInt retry = 2;

	  for (;;)
        {
        TInt r=CreateSession(KVpnManagerServer,
                             Version(),
                             KDefaultMessageSlots);

        if (r!=KErrNotFound && r!=KErrServerTerminated)
            return r;
        if (--retry==0)
            return r;
        r = Launcher::LaunchServer(KVpnManagerServer, KVpnManagerFile,
                                   KVpnManagerUid3, KVpnManagerServerInitHeapSize,
                                   KVpnManagerServerMaxHeapSize, KVpnManagerServerStackSize);

        if (r!=KErrNone && r!=KErrAlreadyExists)
            return r;
        }
    }

EXPORT_C void RVpnServ::Close()
/**
 * Closes the connection (session) to the VPN Manager server.
 */
    {
    RSessionBase::Close();
    }

EXPORT_C TVersion RVpnServ::Version() const
/**
 * Returns the version of the server with which this client is compatible.
 *
 * @return The version
 */
    {
    return TVersion(KVpnManagerMajorVersionNumber,
                    KVpnManagerMinorVersionNumber,
                    KVpnManagerBuildVersionNumber);
    }

EXPORT_C void RVpnServ::ImportPolicy(const TDesC& aDir, TRequestStatus& aStatus)
/**
 * Imports one or more VPN policies to the policy store
 * maintained by the VPN Manager.
 *
 * The files that constitute the VPN policies are assumed
 * to reside in the specified directory. For each policy,
 * the files are:
 * <ol>
 * <li>Policy file (REQUIRED)</li>
 * <li>Policy information file (REQUIRED)</li>
 * <li>CA certificate files (REQUIRED)</li>
 * <li>Client/user private key and certificate files (OPTIONAL)</li>
 * <li>Gateway (peer) certificate files</li>
 * </ol>
 *
 * The files must follow a certain naming convention and
 * utilize certain file formats. The naming convention and the file
 * formats are specified in a separate document.
 *
 * The policies in this case can refer to the CA certificates
 * via a BIN type reference (i.e. a certificate file name).
 * If they do, certificates with the specified names must be
 * imported at the same time with the policy.
 *
 * The policy being imported can be marked as hidden by
 * including the value of the KHiddenPolicyIndicator constant
 * (defined in vpnapidefs.h) in the description section of policy
 * informationation file.
 *
 * The return value is returned in the aStatus argument
 * when the request completes. This can be one of:
 *
 * <ol>
 * <li>KErrNone The import was successful</li>
 * <li>\<VpnError\> A VPN error code if the import fails for some
 * identified reason</li>
 * <li>\<SystemError\> A system-wide error code if an out-of-resource
 * error occurred while processing the request</li>
 * </ol>
 *  
 * @param aDir An absolute path to a directory that contains the
 * files that constitute the VPN policy. NOTE. As this method is
 * asynchronous, this reference must point to a variable that
 * remains valid until this method is complete (i.e. it cannot be
 * e.g. a local variable of the calling method)
 * @param aStatus [out] A reference to the standard request status
 * object. On request completion, contains the return code of the request.
 *
 */
    {
    SendReceive(EVpnImportPolicy, TIpcArgs(&aDir), aStatus);
    }

EXPORT_C void RVpnServ::CancelImport()
/**
 * Cancels an ongoing policy import operation.
 */
    {
    SendReceive(EVpnCancelImport, TIpcArgs(NULL));
    }

EXPORT_C TInt RVpnServ::EnumeratePolicies(TInt& aCount)
/**
 * Returns the number of installed, visible VPN policies.
 * Policies marked as hidden (by including the
 * KHiddenPolicyIndicator in the iDescription policy details
 * field) are not included in the count.
 *
 * @param aCount [out] The policy count
 *
 * @return KErrNone, if the request was processed successfully;
 *         \<SystemError\> A system-wide error code if the request
 *         failed for some unexpected reason.
 */
    {
    TPckg<TInt> pckgPolicyCount(aCount);
    
    return SendReceive(EVpnEnumeratePolicies, TIpcArgs(&pckgPolicyCount));
    }

EXPORT_C TInt RVpnServ::GetPolicyInfoList(CArrayFixFlat<TVpnPolicyInfo>* aPolicyInfoList)
/**
 * Fills the given list with information about the installed, visible
 * policies. The method resizes the list according to the number of
 * installed policies. Policies marked as hidden (by including the
 * KHiddenPolicyIndicator in the iDescription policy details field)
 * are not included in the listing.
 *
 * @param aPolicyInfoList A reference to a pointer to a list
 * of policy information structures.
 * 
 * @return \<SystemError\> A system-wide error code if the request
 *         failed for some unexpected reason.
 */
    {
	TInt ret = KErrNone;

    // Get the current policy count
    TInt policyCount;
    ret = EnumeratePolicies(policyCount);
    if (ret != KErrNone)
        {
        return ret;
        }

    // If there are no policies, we can stop here
    if (policyCount == 0)
        {
        return KErrNone;
        }
    
	// Make sure that the (client-side) policy
    // info array has the correct size
	TRAP(ret, aPolicyInfoList->ResizeL(policyCount));
	if (ret != KErrNone)
        {
        return ret;
        }

	// Create a writable descriptor in this thread's address space
    // where the server will write the policy information list
	TPtr8 policyList((TUint8*)&aPolicyInfoList->At(0), policyCount * aPolicyInfoList->Length());

    return SendReceive(EVpnGetPolicyInfo, TIpcArgs(policyCount, &policyList));
    }

EXPORT_C TInt RVpnServ::GetPolicyDetails(const TVpnPolicyId& aPolicyId, TVpnPolicyDetails& aPolicyDetails)
/**
 * Returns detailed information about the specified policy.
 *
 * @param aPolicyId The ID of the policy to return information
 * about
 * @param aPolicyDetails [out] Detailed policy information
 * 
 * @return KErrNone, if the request was processed successfully;
 *         KVpnErrPolicyNotFound, if the specified policy was not found;
 *         \<SystemError\> A system-wide error code if the request
 *         failed for some unexpected reason.
 */
    {
    TPckg<TVpnPolicyId> pckgPolicyId(aPolicyId);
    TPckg<TVpnPolicyDetails> pckgPolicyDetails(aPolicyDetails);

    return SendReceive(EVpnGetPolicyDetails, TIpcArgs(&pckgPolicyId, &pckgPolicyDetails));
    }
    
EXPORT_C TInt RVpnServ::DeletePolicy(const TVpnPolicyId& aPolicyId)
/**
 * Deletes the specified policy from the VPN policy store
 * maintained by the VPN Manager.
 *
 * NOTE. The policy is deleted even if its active.
 *
 * @param aPolicyId The ID of the policy to delete
 *
 * @return KErrNone, if the request was processed successfully;
 *         KVpnErrPolicyNotFound, if the policy was not found;
 *         \<SystemError\> A system-wide error code if the request
 *         failed for some unexpected reason.
 */
    {
    TPckg<TVpnPolicyId> pckgPolicyId(aPolicyId);
    
    return SendReceive(EVpnDeletePolicy, TIpcArgs(&pckgPolicyId));
    }

EXPORT_C void RVpnServ::ChangePassword(const TPckg<TVpnPolicyId>& aPolicyId, TRequestStatus& aStatus) 
/**
 * Initiates a user dialogue for changing the password that is used
 * to protect the private keys associated with the installed VPN
 * policies.
 *
 * The return value is returned in the aStatus argument
 * when the request completes. This can be one of:
 * <ol>
 * <li>KErrNone, if the request was processed successfully</li>
 * <li>\<SystemError\> A system-wide error code if the request
 * failed for some unexpected reason</li>
 * </ol>
 *
 *
 * @param aPolicyId The ID of the policy whose associated key
 * protection password is to be changed (NOTE 1. this parameter has
 * no effect at the moment as all private keys are protected with
 * the same password. NOTE 2: As this method is asynchronous, this
 * reference must point to a variable that remains valid until this
 * method is complete (i.e. it cannot be e.g. a local variable of
 * the calling method))
 * 
 * @param aStatus [out] A reference to the standard request status
 * object. On request completion, contains the return code of the request.
 * 
 */
    {
    SendReceive(EVpnChangePassword, TIpcArgs(&aPolicyId), aStatus);
    }

EXPORT_C void RVpnServ::CancelChange()
/**
 * Cancels an ongoing password changing operation.
 */
    {
    SendReceive(EVpnCancelChange, TIpcArgs(NULL));
    }

EXPORT_C TInt RVpnServ::GetPolicyData(const TVpnPolicyId& aPolicyId, HBufC8*& aPolicyData)
/**
 * Returns policy data.
 */
    {
    TInt ret = KErrNone;
    TRAP(ret, DoGetPolicyDataL(aPolicyId, aPolicyData));
    return ret;
    }

void RVpnServ::DoGetPolicyDataL(const TVpnPolicyId& aPolicyId, HBufC8*& aPolicyData)    
    {
    TPckg<TVpnPolicyId> pckgPolicyId(aPolicyId);

    // First get the policy size
    TInt policySize;
    TPckg<TInt> policySizePckg(policySize);

    User::LeaveIfError(SendReceive(EVpnGetPolicySize, TIpcArgs(&pckgPolicyId, &policySizePckg)));

    // Allocate a buffer to hold the policy data
    HBufC8* policyData = HBufC8::NewL(policySize);
    CleanupStack::PushL(policyData);

    TPtr8 policyDataPtr = policyData->Des();

    // Fetch the policy data
    User::LeaveIfError(SendReceive(EVpnGetPolicyData, TIpcArgs(&pckgPolicyId, &policySizePckg, &policyDataPtr)));

    aPolicyData = policyData;
    
    CleanupStack::Pop(); // policyData
    }

// Additions to make it easier to implement OMA DM based VPN policy management
    
EXPORT_C TInt RVpnServ::AddPolicy(TVpnPolicyDetails& aPolicyDetails, const TDesC8& aPolicyData)
/**
 * Adds a new VPN policy to the policy store maintained by the
 * VPN Manager.
 *
 * The policy details in this case CAN include a (globally
 * unique) policy ID for the policy. This policy ID is defined
 * by the policy author according to the author's own rules.
 * This is the ID that becomes also the local ID of the policy.
 * In other words, a single global ID is used to identify a
 * policy both inside and outside the device.
 * If a policy with the specified ID already exists in the
 * policy store, the method returns KErrAlreadyExists.
 * If the policy ID is missing from policy details argument on
 * input, a globally unique ID is automatically created for the
 * policy. This ID is placed in the policy details argument on
 * output.
 *
 * The policy details must also include a non-empty policy name.
 * If a name is missing, the method returns KErrArgument.
 * If the proposed policy name is already in use, a sequence number
 * is added to the policy name. The new policy name is placed in
 * the policy details argument on output.
 * 
 * The policy data argument contains the policy content in the
 * text format described in separate VPN policy format documentation.
 * If the policy data argument is empty, the method returns the
 * KErrArgument error code. The policy data does not include any
 * PKI objects, as the assumption is that the PKI data associated
 * with the policy is placed in the device's PKI store via some
 * other mechanism and APIs. The policy in this case must refer to
 * the CA certificates via some other reference type than BIN
 * (file reference). BIN type references are supported only when
 * policies are imported to the policy store with the ImportPolicy
 * method. All supported reference types are described in the IKE
 * policy format documentation.
 *
 * The policy being added can be marked as hidden by
 * including the descriptor KHiddenPolicyIndicator in the
 * iDescription field of the policy details argument.
 * The policy is assumed to be visible if the iDescription
 * field does not contain the KHiddenPolicyIndicator
 * descriptor.
 *
 * @param aPolicyDetails Details (metadata) about the policy
 * @param aPolicyData The policy data (content)
 *
 * @return KErrNone if the addition was successful;
 *         KErrArgument, if the policy ID is missing from policy details;
 *         \<VpnError\> A VPN error code if the addition fails for some
 *         identified reason;
 *         \<SystemError\> A system-wide error code if an out-of-resource
 *         error occurred while processing the request.
 */
    {
    TPckg<TVpnPolicyDetails> pckgPolicyDetails(aPolicyDetails);

    return SendReceive(EVpnAddPolicy, TIpcArgs(&pckgPolicyDetails, &aPolicyData));
    }

EXPORT_C TInt RVpnServ::UpdatePolicyDetails(TVpnPolicyDetails& aPolicyDetails)
/**
 * Updates the details of the specified VPN policy.
 * The ID of the policy whose details are to be
 * updated is specified in the policy details
 * argument. If the ID is missing, the method returns
 * KErrArgument. If a policy with the specified ID
 * cannot be found, the method returns KVpnErrPolicyNotFound.
 *
 * The policy details must include a non-empty policy name.
 * If a name is missing, the method returns KErrArgument.
 * If the policy details contain a new name for the policy,
 * the new name is checked agains existing other policy names.
 * If the name is already in use, a sequence number is added
 * to the policy name. The new policy name is placed in the
 * policy details argument on output.
 *
 * The policy being updated can be marked as hidden by
 * including the descriptor KHiddenPolicyIndicator in the
 * iDescription field of the policy details argument.
 * The policy is assumed to be visible if the iDescription
 * field does not contain the KHiddenPolicyIndicator
 * descriptor.
 *
 * @param aPolicyDetails Detailed policy information
 *
 * @return KErrNone, if the update was successful;
 *         \<VpnError\> A VPN error code if the addition fails for some
 *         identified reason; 
 *         \<SystemError\> A system-wide error code if an out-of-resource
 *         error occurred while processing the request.
 *
 */
    {
    TPckg<TVpnPolicyDetails> pckgPolicyDetails(aPolicyDetails);

    return SendReceive(EVpnUpdatePolicyDetails, TIpcArgs(&pckgPolicyDetails));
    }

EXPORT_C TInt RVpnServ::UpdatePolicyDetails(const TVpnPolicyDetails& aPolicyDetails)
/**
 * Updates the details of the specified VPN policy.
 * The ID of the policy whose details are to be
 * updated is specified in the policy details
 * argument. If the ID is missing, the method returns
 * KErrArgument. If a policy with the specified ID
 * cannot be found, the method returns KVpnErrPolicyNotFound.
 *
 * The policy details must include a non-empty policy name.
 * If a name is missing, the method returns KErrArgument.
 * If the policy details contain a new name for the policy,
 * the new name is checked agains existing other policy names.
 * If the name is already in use, a sequence number is added
 * to the policy name. The new policy name is placed in the
 * policy details argument on output.
 *
 * The policy being updated can be marked as hidden by
 * including the descriptor KHiddenPolicyIndicator in the
 * iDescription field of the policy details argument.
 * The policy is assumed to be visible if the iDescription
 * field does not contain the KHiddenPolicyIndicator
 * descriptor.
 *
 * @param aPolicyDetails Detailed policy information
 *
 * @return KErrNone, if the update was successful;
 *         \<VpnError\> A VPN error code if the addition fails for some
 *         identified reason; 
 *         \<SystemError\> A system-wide error code if an out-of-resource
 *         error occurred while processing the request.
 */
    {
    TPckg<TVpnPolicyDetails> pckgPolicyDetails(aPolicyDetails);

    return SendReceive(EVpnUpdatePolicyDetails, TIpcArgs(&pckgPolicyDetails));
    }


EXPORT_C TInt RVpnServ::UpdatePolicyData(const TVpnPolicyId& aPolicyId, const TDesC8& aPolicyData)
/**
 * Updates the data of the specified VPN policy. If a policy with the
 * specified ID cannot be found, the method returns the
 * KVpnErrPolicyNotFound error code. If the policy ID or data argument
 * is empty, the method returns the KErrArgument error code.
 *
 * @param aPolicyId The ID of the policy to update
 * @param aPolicyData The policy data
 *
 * @return KErrNone, if the update was successful;
 *         \<VpnError\> A VPN error code if the update fails for some
 *         identified reason; 
 *         \<SystemError\> A system-wide error code if an out-of-resource
 *         error occurred while processing the request.
 */
    {
    TPckg<TVpnPolicyId> pckgPolicyId(aPolicyId);

    return SendReceive(EVpnUpdatePolicyData, TIpcArgs(&pckgPolicyId, &aPolicyData));
    }
