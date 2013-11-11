/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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






#include "RSenDocument.h"
#include <xml/utils/xmlengxestd.h>
#include <xml/dom/xmlengdomimplementation.h>

LOCAL_C void CleanupRSenDocument(TAny* aDocumentData)
	{
	TSenDocumentData* pData = 
		REINTERPRET_CAST(TSenDocumentData*, aDocumentData);
    if ( *pData->ipOpenDocuments <= 1)
        {
        RSenDocument document;
    	document.ipData = pData;
    	document.Close();
        }
    else
        {
        *pData->ipOpenDocuments = *pData->ipOpenDocuments-1;
        }
	}

EXPORT_C RSenDocument RSenDocument::NewL()
    {
    RSenDocument self = RSenDocument::NewLC();
    CleanupStack::Pop();
    return self;
    }

EXPORT_C RSenDocument RSenDocument::NewLC()
    {
	TBool* pDisabled = (TBool*)Dll::Tls();
	if ( !pDisabled )
	    {
        XmlEngineAttachL();
	    }
    RXmlEngDOMImplementation domImpl;
    domImpl.OpenL();
    RXmlEngDocument doc;
    doc.OpenL(domImpl);
    RSenDocument document;
    CleanupClosePushL(document);
	document.ipData =  NULL;
	document.iInternal = NULL;
    document.ipData = new (ELeave) TSenDocumentData;
    document.iInternal =
        (reinterpret_cast<RSenDocument*>(const_cast<RXmlEngDocument*>(&doc)))->iInternal;
    document.ipData->iInternal = document.iInternal;
	document.ipData->ipOpenDocuments = NULL;
    document.ipData->ipOpenDocuments = new (ELeave) TInt(1);
    document.ipData->iDomImpl = domImpl;
    CleanupStack::Pop();
    TCleanupItem cleanup(CleanupRSenDocument, document.ipData);
    CleanupStack::PushL(cleanup);
    return document;
    }

EXPORT_C RSenDocument RSenDocument::NewL(void* aInternal)
    {
    RSenDocument self = RSenDocument::NewLC(aInternal);
    CleanupStack::Pop();
    return self;
    }

EXPORT_C RSenDocument RSenDocument::NewLC(void* aInternal)
    {
	TBool* pDisabled = (TBool*)Dll::Tls();
	if ( !pDisabled ) // NOTE: when enabled, pointer does NOT EXIST in TLS!
	    {
        XmlEngineAttachL();
	    }
    RXmlEngDOMImplementation domImpl;
    domImpl.OpenL();
    RXmlEngDocument doc;
    doc.OpenL(domImpl, aInternal);
    RSenDocument document;
    CleanupClosePushL(document);
	document.ipData =  NULL;
	document.iInternal = NULL;	 
    document.ipData = new (ELeave) TSenDocumentData;
    document.iInternal =
        (reinterpret_cast<RSenDocument*>(const_cast<RXmlEngDocument*>(&doc)))->iInternal;
    document.ipData->iInternal = document.iInternal;
	document.ipData->ipOpenDocuments = NULL;	
    document.ipData->ipOpenDocuments = new (ELeave) TInt(1);
    document.ipData->iDomImpl = domImpl;
     CleanupStack::Pop();
    TCleanupItem cleanup(CleanupRSenDocument, document.ipData);
    CleanupStack::PushL(cleanup);
    return document;
    }

EXPORT_C RSenDocument RSenDocument::Copy()
    {
    *ipData->ipOpenDocuments = *ipData->ipOpenDocuments + 1;
    return *this;
    }

EXPORT_C void RSenDocument::Close()
    {
	if(ipData && ipData->ipOpenDocuments)
		{
	    *ipData->ipOpenDocuments = *ipData->ipOpenDocuments - 1;
	    if ( !*ipData->ipOpenDocuments )
	        {
	        RXmlEngDOMImplementation domImpl = ipData->iDomImpl;
	        iInternal = ipData->iInternal;
	        delete ipData->ipOpenDocuments;
	        delete ipData;
	        RXmlEngDocument::Close();
	        domImpl.Close();
	    	TBool* pDisabled = (TBool*)Dll::Tls();
	    	if ( !pDisabled )
	    	    {
	            XmlEngineCleanup();
	    	    }
	        }
	    else
	        {
	        iInternal = NULL;
	        }
       }
    }

EXPORT_C void RSenDocument::Destroy()
    {
    RSenDocument::Close();
    /*        
    *ipData->ipOpenDocuments = *ipData->ipOpenDocuments - 1;
    if ( !*ipData->ipOpenDocuments )
        {
        RXmlEngDOMImplementation domImpl = ipData->ipDomImpl;            
        delete ipData->ipOpenDocuments;
        delete ipData;
        ipData = NULL;
        RXmlEngDocument::Destroy();
        domImpl.Close();        
    	TBool* pDisabled = (TBool*)Dll::Tls();
    	if ( !pDisabled )
    	    {
            XmlEngineCleanup();
    	    }
        }
    else
        {
        iInternal = NULL;
        }
    */        
    }
    
EXPORT_C TInt RSenDocument::ManualXmlEngineTlsAttachL()
    {
	TBool* pDisabled = (TBool*)Dll::Tls();
		
	if ( !pDisabled )
  		{
  		pDisabled = new (ELeave) TBool();
        if ( pDisabled )
            {
      		Dll::SetTls(pDisabled);
            }
        else
            {
            return KErrUnknown;
            }
  	    }
  	else
  	    {
  	    return KErrAlreadyExists;
  	    }
  	
    XmlEngineAttachL();
    
  	return KErrNone;
    }

EXPORT_C TInt RSenDocument::ManualXmlEngineTlsCleanup()
    {
	TBool* pDisabled = (TBool*)Dll::Tls();
	if ( pDisabled )
  		{
        delete pDisabled;
        Dll::FreeTls();	            
  	    }
  	else
  	    {
  	    return KErrNotFound;
  	    }

    XmlEngineCleanup();
  	
  	return KErrNone;
    }
    
// End of File



