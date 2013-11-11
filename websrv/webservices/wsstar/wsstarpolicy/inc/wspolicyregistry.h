/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Header declaration
*
*/






 

#ifndef WSSTAR_POLICY_REGISTRY_H
#define WSSTAR_POLICY_REGISTRY_H

// INCLUDES
#include <e32std.h>
#include "senwsdescription.h"
#include "senpointermap.h"

// FORWARD DECLARATIONS
class CWSPolicy;
// CONSTANTS
typedef RSenPointerMap<TDesC8, CSenElement> RPolicyMap;


/**
 * Class: CPolicyRegistry  
 *
 * Description: This class holds information about all the policies present in the current session.
 * This information is used to solve the policy references with-in the policies.
 * Also while creating this registry, polices can be checked for validity/expiration and can be 
 * re-obtained from the service.
 */
class CPolicyRegistry : public CBase
{
  public:
IMPORT_C    static CPolicyRegistry* NewL();
IMPORT_C    static CPolicyRegistry* NewLC();
            ~CPolicyRegistry();
    
IMPORT_C	void SetParent(CPolicyRegistry* parent);
IMPORT_C	CPolicyRegistry* Parent();
IMPORT_C    CSenElement* LookupPolicy(const TPtrC8& aPolicyURI);
IMPORT_C    CSenElement* LookupPolicyByNameL(const TPtrC8& aPolicyName);	//codescannerwarnings
//IMPORT_C    void RegisterPolicy(TDesC8& aPolicyURI, CWSPolicy* aPolicy);
IMPORT_C    void RegisterPolicy(TDesC8& aPolicyID, CSenElement* aPolicy);
IMPORT_C	void UnregisterPolicy(const TPtrC8& aPolicyID);
	
IMPORT_C	void PopulateRegistryL(CSenElement* aServicePolicy);	//codescannerwarnings
IMPORT_C    void ResetRegistry();
private:       
    TPtrC8 PolicyIdL(CSenElement* aPolicy);
    TPtrC8 PolicyNameL(CSenElement* aPolicy);
    CPolicyRegistry();
    void ConstructL();
    
    RPolicyMap iRegistry;
    CPolicyRegistry* iParent;
}; 

#endif // WSSTAR_POLICY_REGISTRY_H
// END OF FILE
