/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:        This class represents the policy information
*
*/


// INCLUDE FILES
#include <e32std.h>
#include <s32strm.h>
#include "sendebug.h"
#include "senservicepolicy.h"
#include "SenXmlElement.h"
#include "senguidgen.h"
#include "SenXmlServiceDescription.h"
#include "senlogger.h"

// ============================= LOCAL FUNCTIONS ===============================

namespace
	{
	_LIT8(KServicePolicyLocalName, "ServicePolicy");
	_LIT8(KSenNsUri, "urn:com.nokia.Sen.config.1.0");
	_LIT8(KPolicyLocalName, "Policy");
	_LIT8(KPolicyArrayLocalName, "PolicyArray");
	_LIT8(KWsPolicyNsUri , "http://schemas.xmlsoap.org/ws/2004/09/policy"); // Namespace of WS-Policy
	_LIT8(KClientServicePolicyLocalName, "ClientPolicy");
	}


CSenServicePolicy* CSenServicePolicy::NewL()
	{
    CSenServicePolicy* pNew = NewLC();
    CleanupStack::Pop();
    return(pNew) ;
    }

CSenServicePolicy* CSenServicePolicy::NewLC()
    {
    CSenServicePolicy* pNew = new (ELeave) CSenServicePolicy();
    CleanupStack::PushL(pNew);
	pNew->BaseConstructL();
    return pNew;
    }

CSenServicePolicy::CSenServicePolicy()
    {
    }

CSenServicePolicy::~CSenServicePolicy()
	{
	}

void CSenServicePolicy::BaseConstructL()
	{
	CServicePolicy::BaseConstructL(KServicePolicyLocalName());
	}
	
//Rebuild both client and Service Policy from the given Template
TInt CSenServicePolicy::RebuildServicePolicyFrom(MSenServicePolicy& aTemplate)
{
    TInt retVal(KErrNone);
    TInt err( KErrNone );
    // check for possible policies definition
    CSenServicePolicy* policy = (CSenServicePolicy*)&aTemplate;
    CSenElement& pServicePolicyElement = policy->AsElement();
    CSenElement* copy = NULL;
    TRAP( err, copy = CSenXmlElement::NewL(KServicePolicyLocalName); )
    if( !copy ) 
        {
        return KErrNoMemory;
        }
    else if( !err )
        {
        TRAP( err, copy->CopyFromL( pServicePolicyElement ); )
        if( !err )
            {
#ifdef _SENDEBUG
                { // parenthesis -pair added to avoid local variable visibility scope collision
                TLSLOG_L(KSenServConnLogChannelBase,KMaxLogLevel,"TInt CSenServicePolicy::RebuildServicePolicyFrom POLICY");
                TLSLOG_L(KSenServConnLogChannelBase,KMaxLogLevel,"-----------COPY----------------------------------------");
                TLSLOG_L(KSenServConnLogChannelBase,KMaxLogLevel,"-------------------------------------------------------");
                TLSLOG_L(KSenServConnLogChannelBase,KMaxLogLevel,"-------------------------------------------------------");
                HBufC8* pXml = NULL;
                TRAP_IGNORE
                    ( 
                    pXml = copy->AsXmlL(); 
                    if( pXml )
                        {
                        TLSLOG_ALL(KSenServConnLogChannelBase,KMaxLogLevel,( *pXml ));
                        }
                    )
                delete pXml;
                }
#endif // _SENDEBUG
            // UpdateClientPolicy        
            CSenElement* pClientPolicy = copy->Element( KClientServicePolicyLocalName );
            if(!pClientPolicy)
                {
                pClientPolicy = copy->Element( KSenNsUri, KClientServicePolicyLocalName );
                }
            if( pClientPolicy )
                {
                TRAP( err, retVal = AddClientPolicyL( pClientPolicy ); )
                //if( err )
                //    {
                //    retVal = err;
                //    }
                }
    
            // UpdateServicePolicyArray        
            CSenElement* pPolicyArray = copy->Element( KPolicyArrayLocalName );
            if(!pPolicyArray)
                {
                pPolicyArray = copy->Element( KSenNsUri, KPolicyArrayLocalName );
                }
                
            if(!pPolicyArray)
                {
                pPolicyArray = copy->Element( KNullDesC8, KPolicyArrayLocalName );
                }
                
            if(pPolicyArray)
                {
                RPointerArray<CSenElement> elements;
                TRAP( err, pPolicyArray->ElementsL( elements, KWsPolicyNsUri, KPolicyLocalName ); )
                // -- CleanupClosePushL(elements); // not needed, since array is living in non-leaving code
                if( !err )
                    {
                    TInt count = elements.Count();
                    CSenElement* pPolicy = NULL;
                    for( TInt i = 0; i < count; i++ )
                        {
                        pPolicy = elements[i];
                        if( pPolicy )
                            {
                            TRAP( err, AddPolicyL( pPolicy ); )
                            }
                        }
                    // -- CleanupStack::PopAndDestroy(&elements);
                    }
                }
            }
        }
    delete copy;
    copy = NULL;
    return retVal;  
    }
    
//Do nuthing
TBool CSenServicePolicy::Accepts(MSenServicePolicy& aPolicyPattern)
{
   return Accepts(aPolicyPattern);
}

CSenElement* CSenServicePolicy::ClientPolicy()
{
     CSenElement* pClientPolicyElement = AsElement().Element(KClientServicePolicyLocalName);
    
    if(!pClientPolicyElement)
        {
        pClientPolicyElement =  AsElement().Element(KSenNsUri, KClientServicePolicyLocalName);
        }
        
    return pClientPolicyElement;   
}

// End of file
