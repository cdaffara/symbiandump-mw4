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
* Description:  The VPN API allows Symbian OS applications and servers
*                to perform VPN-specific operations.
*
*/


/**  
 @file
 @released
 @publishedPartner
 
 VPN API
 */

#ifndef __VPNAPI_H__
#define __VPNAPI_H__

#include <e32std.h>
#include <e32base.h>

#include "vpnapidefs.h"

/** 
 * @mainpage VPN API
 * 
 * @section intro Overview
 *
 * The VPN API allows Symbian OS applications and servers to perform
 * VPN-specific operations, especially those related to VPN policies.
 * The operations include:
 * <ul>
 * <li>Importing policies</li>
 * <li>Listing policies</li>
 * <li>Finding out detailed information about the policies</li>
 * <li>Deleting policies</li>
 * <li>(*Deprecated) Changing the password that is used to protect the private keys 
 *   associated with the policies</li>
 * </ul>
 * 
 * NOTE. Direct policy activation/deactivation is not possible through this API.
 * This is because the use of VPNs is based on the activation/deactivation
 * of VPN IAPs and policy activation/deactivation is a (hidden) part of this
 * process. The activation/deactivation of VPN IAPs is similar to the
 * activation/deactivation of any other IAPs and is thus performed via
 * standard Symbian OS interfaces.
 */

class RVpnServ : public RSessionBase
/**
 * An API that allows client applications to manage VPN policies and
 * the password that is used to protect private keys used for VPN
 * authentication.
 *
 * The API follows the standard Symbian OS client-server
 * programming patterns.
 *
 */
    {
public:
    IMPORT_C RVpnServ(void);

    IMPORT_C TInt Connect();
    IMPORT_C void Close();

    IMPORT_C TVersion Version() const;
    
    IMPORT_C void ImportPolicy(const TDesC& aDir, TRequestStatus& aStatus);
    IMPORT_C void CancelImport();

    IMPORT_C TInt EnumeratePolicies(TInt& aCount);
    IMPORT_C TInt GetPolicyInfoList(
        CArrayFixFlat<TVpnPolicyInfo>* aPolicyInfoList);

    IMPORT_C TInt GetPolicyDetails(
        const TVpnPolicyId& aPolicyId, TVpnPolicyDetails& aPolicyDetails);

    IMPORT_C TInt DeletePolicy(const TVpnPolicyId& aPolicyId);

    /**
     * ChangePassword / CancelChange
     * *Deprecated!! -> To Be removed 
     *   (Completes immediately with KErrNotSupported)
     *
     * @since S60 3.0
     * @param aPolicyId Policy id
     * @param aStatus async operation status. 
     */
    IMPORT_C void ChangePassword(
        const TPckg<TVpnPolicyId>& aPolicyId, TRequestStatus& aStatus);
    IMPORT_C void CancelChange();

    IMPORT_C TInt GetPolicyData(
        const TVpnPolicyId& aPolicyId, HBufC8*& aPolicyData);

    // New methods to facilitate OMA DM based VPN policy management
    
    IMPORT_C TInt AddPolicy(
        TVpnPolicyDetails& aPolicyDetails, const TDesC8& aPolicyData);

    IMPORT_C TInt UpdatePolicyDetails(
        const TVpnPolicyDetails& aPolicyDetails);
    
    IMPORT_C TInt UpdatePolicyData(
        const TVpnPolicyId& aPolicyId, const TDesC8& aPolicyData);

    IMPORT_C TInt UpdatePolicyDetails(TVpnPolicyDetails& aPolicyDetails);

private: // implementation
    void DoGetPolicyDataL(
        const TVpnPolicyId& aPolicyId, HBufC8*& aPolicyData);
    };

/** Another name for the API class */
typedef RVpnServ RVpnApi;

#endif // __VPNAPI_H__
