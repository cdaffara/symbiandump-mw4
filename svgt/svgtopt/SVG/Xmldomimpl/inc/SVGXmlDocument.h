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


#ifndef SVGXMLDOCUMENT_H
#define SVGXMLDOCUMENT_H

#if !defined(__E32BASE_H__)
#include <e32base.h>
#endif

#include "SVGXmlElement.h"


/**
 * Class description
 */
class MXmlDocument
    {
    public:

        /**
         * Create a XML element with the given tag
         *
         * @since 1.0
         * @param aTagName - A string buffer containing the name tag for the XML element
         * @return An element object as a pointer to a MXmlElement interface
         */
       virtual MXmlElement* CreateElementL( const TDesC& aTagName ) = 0;

		virtual MXmlElement* CreateElementL( const TUint8 aTagName ) = 0;


        /**
         * Create an attribute for a given attribute name
         * Strictly speaking a Node element should be created for an attribute
         * This is not implemented for this release
         *
         * @since 1.0
         * @param aName - A string buffer containing the name of the attribute
         * @return An integer containing success/failure information
         */
        virtual TInt        CreateAttribute( const TDesC& aName ) = 0;


        /**
         * Find an element that matches a given element ID
         *
         * @since 1.0
         * @param aElementId - A string buffer containing the required element ID
         * @return An element object as a pointer to a MXmlElement interface
         */
        virtual MXmlElement* GetElementById( const TDesC& aElementId ) = 0;


        /**
         * Append an 'svg' element to its document
         *
         * @since 1.0
         * @param aAppendChild - An XML element pointer to the object that needs
         *                       to be appended
         * @return A pointer to the element just appended if it is an 'svg'
                   Otherwise NULL
         */
        virtual MXmlElement* AppendChildL( MXmlElement* aAppendChild ) = 0;

        /**
         * Method for notification from child element of AppendChildL or
         * RemoveChild.
         *
         * @since 1.0
         * @return none
         */
        virtual void ElementAppendedOrRemoved() = 0;
    };

#endif      // SVGXMLDOCUMENT_H
