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
* Description:        
*
*/








// INCLUDE FILES
#include <badesca.h>
#include <SenXmlReader.h>
#include <s32mem.h>
#include <utf.h>

#include <SenWsSecurityHeader.h>

#include "sendebug.h"

#include "idwsfsoapmessage.h"
#include "idwsfservicesession.h"


CIdWsfSoapMessage* CIdWsfSoapMessage::NewL()
    {
    CIdWsfSoapMessage* pNew = new (ELeave) CIdWsfSoapMessage;
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL();
    CleanupStack::Pop(); // pNew;
    return pNew;
    }

CIdWsfSoapMessage::CIdWsfSoapMessage() :
    ipSecurityMechanismURI(NULL)
    {
    }


CIdWsfSoapMessage::~CIdWsfSoapMessage()
    {
    }

void CIdWsfSoapMessage::BaseConstructL()
    {
    CSenSoapMessage::BaseConstructL();
    }

CSenWsSecurityHeader* CIdWsfSoapMessage::NewSecurityHeaderLC(
                                                        const TDesC8* aData)
    {
    /*
    CIdWsfSecurityHeader* pNew = NULL;
    if(aData)
        {
        // ID-WSF 1.0 compliant security header with credential
        pNew = CIdWsfSecurityHeader::NewL(*aData);
        }
    else
        {
        // ID-WSF 1.0 compliant security header WITHOUT credential
        pNew = CIdWsfSecurityHeader::NewL();
        }
    CleanupStack::PushL(pNew);
    return pNew;
    */

    CSenWsSecurityHeader* pHeader = NULL;

    if(ipSecurityMechanismURI && (
        (*ipSecurityMechanismURI).Compare(KSecTlsBearer11) == 0 
                                ||
        (*ipSecurityMechanismURI).Compare(KSecNullBearer11) == 0))
        {
          SENDEBUG_L("CIdWsfSoapMessage::NewSecurityHeaderLC \
               - Setting header for ID-WSF 1.1");
            // We need to use (newer) security header for ID-WSF 1.1
            // this could be done in IdWsfSoapMessage class, if it would
            // somehow know in NewSecurityHeaderL() method, that it is
            // actually 1.0 or 1.1. But this current solution is rather
            // clear also. Each new header type, we need a new 
            // else-if code segment inside this session class method,
            // -> in here.
           

          if(aData)
            {
            // ID-WSF 1.1 compliant security header with credential
            pHeader = 
                CSenWsSecurityHeader::NewL(*aData); 
            }
          else
            {
            // ID-WSF 1.1 compliant security header WITHOUT credential
            pHeader = 
                CSenWsSecurityHeader::NewL(); 
            }
        }
       else // either no security mechanism or 1.0 some compliant mechanism
        {
           SENDEBUG_L("CIdWsfSoapMessage::NewSecurityHeaderLC \
               - Setting header for ID-WSF 1.0");
          // Add new ID-WSF 1.0 compliant (draft) security header to msg
          if(aData)
            {
            // ID-WSF 1.0 compliant security header with credential
            pHeader = 
                CIdWsfSecurityHeader::NewL(*aData); 
            }
          else
            {
            // ID-WSF 1.0 compliant security header WITHOUT credential
            pHeader = 
                CIdWsfSecurityHeader::NewL(); 
            }
          // Above is equal with next line, if an aMsg argument 
          // is actually SenIdWsfSoapMessage(!), ID-WSF 1.1, matmatt
          //CSenWebServiceSession::SetFrameworkHeaders(aMsg);

        }
    CleanupStack::PushL(pHeader);
    return pHeader;
    }


void CIdWsfSoapMessage::SetSecurityMechanism(HBufC8* apSecurityMechanismURI)
    {
    // this method is used to indicate which security mechanism is
    // being used. Further on, this info can be used when creating
    // new security header. For example, in ID-WSF 1.0, we can
    // use the draft namespace in the header, and in ID-WSF 1.1 
    // newer (oasis) version.
    ipSecurityMechanismURI = apSecurityMechanismURI;
    }

// END OF FILE

