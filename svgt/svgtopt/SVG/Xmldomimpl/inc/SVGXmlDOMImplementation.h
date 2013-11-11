/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  XML DOM Implementation header file
 *
*/


#ifndef SVGXMLDOMIMPLEMENTATION_H
#define SVGXMLDOMIMPLEMENTATION_H

#if !defined(__E32BASE_H__)
#include <e32base.h>
#endif

class MXmlDocument;
class MXmlElement;
class MXmlDocumentType;


/**
 * Class description 
 */
class MXmlDOMImplementation
    {
    public:
        /**
         * Create a document object for the XML file
         *
         * @since 1.0
         * @param aNamespaceUri - A string buffer containting the name space for this XML file
         * @param aQualifiedName - A string buffer containing qualified name for the document
         * @return A document object as a pointer of interface MXmlDocument
         */
        IMPORT_C    virtual MXmlDocument*   CreateDocumentL( const TDesC& aNamespaceUri,
                                                             const TDesC& aQualifiedName/*,MXmlDocumentType* aDocType*/ ) = 0;
    };

#endif      // SVGXMLDOMIMPLEMENTATION_H
