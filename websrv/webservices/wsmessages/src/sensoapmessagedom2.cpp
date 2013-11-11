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
#include <xml/dom/xmlengbinarycontainer.h>

#include "SenWsSecurityHeader2.h"
#include "sensoapmessagedom2.h"


EXPORT_C CSenSoapMessageDom2* CSenSoapMessageDom2::NewL()
    {
    CSenSoapMessageDom2* pNew = CSenSoapMessageDom2::NewLC();
    CleanupStack::Pop(pNew);
    return pNew;    
    }

EXPORT_C CSenSoapMessageDom2* CSenSoapMessageDom2::NewLC()
    {
    CSenSoapMessageDom2* pNew = new (ELeave) CSenSoapMessageDom2();
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL();
    return pNew;
    }

EXPORT_C CSenSoapMessageDom2* CSenSoapMessageDom2::NewL(TSOAPVersion aVersion)
    {
    CSenSoapMessageDom2* pNew = CSenSoapMessageDom2::NewLC(aVersion);
    CleanupStack::Pop(pNew);
    return pNew;    
    }

EXPORT_C CSenSoapMessageDom2* CSenSoapMessageDom2::NewLC(TSOAPVersion aVersion)
    {
    CSenSoapMessageDom2* pNew = new (ELeave) CSenSoapMessageDom2();
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(aVersion);
    return pNew;
    }

EXPORT_C CSenSoapMessageDom2* CSenSoapMessageDom2::NewL(TSOAPVersion aVersion,
                                                  const TDesC8& aSecurityNs)
    {
    CSenSoapMessageDom2* pNew = CSenSoapMessageDom2::NewLC(aVersion, aSecurityNs);
    CleanupStack::Pop(pNew);
    return pNew;    
    }

EXPORT_C CSenSoapMessageDom2* CSenSoapMessageDom2::NewLC(TSOAPVersion aVersion,
                                                   const TDesC8& aSecurityNs)
    {
    CSenSoapMessageDom2* pNew = new (ELeave) CSenSoapMessageDom2();
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(aVersion, aSecurityNs);
    return pNew;
    }

EXPORT_C CSenSoapMessageDom2* CSenSoapMessageDom2::NewL(MSenMessageContext& aContext)
    {
    CSenSoapMessageDom2* pNew = CSenSoapMessageDom2::NewLC(aContext);
    CleanupStack::Pop(pNew);
    return pNew;    
    }

EXPORT_C CSenSoapMessageDom2* CSenSoapMessageDom2::NewLC(MSenMessageContext& aContext)
    {
    CSenSoapMessageDom2* pNew = new (ELeave) CSenSoapMessageDom2();
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(aContext);
    return pNew;
    }

EXPORT_C CSenSoapMessageDom2* CSenSoapMessageDom2::NewL(MSenMessageContext& aContext,
                                                  TSOAPVersion aVersion)
    {
    CSenSoapMessageDom2* pNew = CSenSoapMessageDom2::NewLC(aContext, aVersion);
    CleanupStack::Pop(pNew);
    return pNew;    
    }

EXPORT_C CSenSoapMessageDom2* CSenSoapMessageDom2::NewLC(MSenMessageContext& aContext,
                                                   TSOAPVersion aVersion)
    {
    CSenSoapMessageDom2* pNew = new (ELeave) CSenSoapMessageDom2();
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(aContext, aVersion);
    return pNew;
    }

EXPORT_C CSenSoapMessageDom2* CSenSoapMessageDom2::NewL(MSenMessageContext& aContext,
                                                  TSOAPVersion aVersion,
                                                  const TDesC8& aSecurityNs)
    {
    CSenSoapMessageDom2* pNew = CSenSoapMessageDom2::NewLC(aContext,
                                                      aVersion,
                                                      aSecurityNs);
    CleanupStack::Pop(pNew);
    return pNew;    
    }

EXPORT_C CSenSoapMessageDom2* CSenSoapMessageDom2::NewLC(MSenMessageContext& aContext,
                                                  TSOAPVersion aVersion,
                                                  const TDesC8& aSecurityNs)
    {
    CSenSoapMessageDom2* pNew = new (ELeave) CSenSoapMessageDom2();
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(aContext, aVersion, aSecurityNs);
    return pNew;
    }
    
EXPORT_C CSenSoapMessageDom2* CSenSoapMessageDom2::NewL(CSenSoapMessageDom2& aMessage)
    {
    CSenSoapMessageDom2* pNew = CSenSoapMessageDom2::NewLC(aMessage);
    CleanupStack::Pop(pNew); // pNew
    return pNew;    
    }

EXPORT_C CSenSoapMessageDom2* CSenSoapMessageDom2::NewLC(CSenSoapMessageDom2& aMessage)
    {
    CSenSoapMessageDom2* pNew = new (ELeave) CSenSoapMessageDom2();
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(aMessage);
    return pNew;
    }

EXPORT_C CSenSoapMessageDom2::~CSenSoapMessageDom2()
    {
    }

EXPORT_C CSenSoapMessageDom2::CSenSoapMessageDom2()
    {
    }

EXPORT_C MSenMessage::TClass CSenSoapMessageDom2::Type()
    {
    return MSenMessage::ESoapMessageDom2;
    }

EXPORT_C CSenFragmentBase* CSenSoapMessageDom2::CreateBodyFragmentL(
                                                    const TDesC8& aNsUri,
                                                    const TDesC8& aLocalName,
                                                    const TDesC8& aPrefix,
                                                    const RAttributeArray& aAttributes,
                                                    TXmlEngElement& aParent,
                                                    RSenDocument& aOwnerDocument)
    {
	return CSenDomFragmentBase::NewL(aNsUri, aLocalName,
		                             aPrefix, aAttributes,
		                             aParent, aOwnerDocument
		                            );
    }

EXPORT_C MSenMessage* CSenSoapMessageDom2::CloneL()
    {
    CSenSoapMessageDom2* pMessage = CSenSoapMessageDom2::NewL(*this);
    return pMessage;
    }

// End of File
