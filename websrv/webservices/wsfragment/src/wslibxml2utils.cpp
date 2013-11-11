/*
* Copyright (c) 2004-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:        Utility for webservices
*
*/








#include "wslibxml2utils.h"

#include <stdapis/libxml2/libxml2_globals.h>
#include <xml/dom/xmlengelement.h>
#include <xml/dom/xmlengerrors.h>
#include <xml/utils/xmlengxestd.h>

#define INTERNAL_NODEPTR(aNodeObject)   (*reinterpret_cast<xmlNodePtr*>(&(aNodeObject)))
#define INTERNAL_NSPTR(aNsDefObject)    (*reinterpret_cast<xmlNsPtr*>(&(aNsDefObject)))
#define XE_ASSERT_DEBUG(assertion)  __ASSERT_DEBUG((assertion),  XmlEngLeaveL(KXmlEngErrWrongUseOfAPI))
#define XE_ASSERT_ALWAYS(assertion) __ASSERT_ALWAYS((assertion), XmlEngLeaveL(KXmlEngErrNullNode))


xmlChar* xmlCharFromDesC8L(const TDesC8& aDesc)
    {
    if(!aDesc.Length())
        {
        return NULL;
        }
    xmlChar* newContent = (xmlChar*)new(ELeave) TUint8[aDesc.Length() + 1];
    *(Mem::Copy((TAny*)newContent, aDesc.Ptr(), aDesc.Length())) = 0;
    return newContent;
    }
    
EXPORT_C void WsXmlUtils::XmlEngRenameElementL(TXmlEngElement aElement, const TDesC8& aLocalName, 
                               const TDesC8& aNamespaceUri, 
                               const TDesC8& aPrefix)
    {
			xmlNodePtr element = INTERNAL_NODEPTR(aElement);

    	XE_ASSERT_DEBUG( element->type == XML_ELEMENT_NODE );

      const xmlChar* oldName = element->name;
       	
      XE_ASSERT_ALWAYS(aLocalName.Length());
       	
      element->name = xmlCharFromDesC8L(aLocalName);
       	
      xmlFree((void*)oldName );
       	
      if (aNamespaceUri.Length() || aPrefix.Length())
        {
        	TXmlEngNamespace ns = aElement.FindOrCreateNsDeclL(aNamespaceUri, aPrefix);
          element->ns = INTERNAL_NSPTR(ns);
      	}
    }
	
	
// End of File



