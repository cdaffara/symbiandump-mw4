/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:        This class offers a set of utility functions for XML data
*
*/








#ifndef WS_LIBXML2_UTILS_H_INCLUDED
#define WS_LIBXML2_UTILS_H_INCLUDED

#include <e32base.h>
#include <xml/dom/xmlengelement.h>

class WsXmlUtils
{
  public:
  
/**
 * Renames TXmlEngElement node.
 *
 * @since S60 v5.1
 * @param aElement Element node
 * @param aLocalName New name
 * @param aNamespaceUri New namespace uri
 * @param aPrefix New namespace prefix
 */
			
	IMPORT_C void  XmlEngRenameElementL(TXmlEngElement aElement, const TDesC8& aLocalName, 
                               const TDesC8& aNamespaceUri, 
                               const TDesC8& aPrefix);  
                                   
    
};


#endif /* WS_LIBXML2_UTILS_H_INCLUDED */
