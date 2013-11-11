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


#ifndef SVGXMLELEMENT_H
#define SVGXMLELEMENT_H

#if !defined(__E32BASE_H__)
#include <e32base.h>
#endif

class   MXmlDocument;


/**
 * Class description 
 */
class MXmlElement
    {
    public:
         /**
         * Set attribute interface for an XML element
         * 
         * @since 1.0
         * @param aName - A string buffer containing the name of the attribute
         * @param aValue - A string buffer containing the value of the attribute
         * @return An integer containing success/failure for this action
         */
        virtual TInt            SetAttributeL( const TDesC& aName,
                                                           const TDesC& aValue ) = 0;

        /**
         * Remove attribute interface for an XML element
         *
         * @since 1.0
         * @param aName - A string buffer containing the name of the attribute to
         *                be removed
         * @return An integer containing success/failure for this action
         */
        virtual TInt            RemoveAttribute( const TDesC& aName ) = 0;


        /**
         * Accessor interface for the tag name of an XML element
         *
         * @since 1.0
         * @param None
         * @return A string buffer containing the tag name
         */
        virtual const TDesC&    TagName() = 0;


         /**
         * Append child interface for an XML element
         *
         * @since 1.0
         * @param aAppendChild - The element object that needs to be appended
         * @return NULL if failed to append; 
         *         A MXmlElement pointer to the object that was just appended
         */
        virtual MXmlElement*    AppendChildL( MXmlElement* aAppendChild, TBool aIsJSR226Element = EFalse ) = 0;
        

        /**
         * Remove child interface for an XML element
         *
         * @since 1.0
         * @param aRemoveChild - A pointer to the object that is to be removed
         * @return None
         */
        virtual void            RemoveChild ( MXmlElement* aRemoveChild ) = 0;


        /**
         * 'Has child nodes' interface that finds out if the current element has any child
         *  nodes
         *
         * @since 1.0
         * @param None 
         * @return A boolean
         *         ETrue if there are child nodes; EFalse otherwise
         */
        virtual TBool           HasChildNodes() = 0 ;


        /**
         * Accessor interface for 'first child' of an XML element
         *
         * @since 1.0
         * @param None
         * @return An MXmlElement pointer to the first child of the current XML element
         */
        virtual MXmlElement*    FirstChild() = 0;


        /**
         * Accessor interface for 'last child' of an XML element
         *
         * @since 1.0
         * @param None
         * @return An MXmlElement pointer to the last child of the current XML element
         */
        virtual MXmlElement*    LastChild() = 0;


        /**
         * Accessor interface for 'owner document' of an XML element
         *
         * @since 1.0
         * @param None
         * @return A MXmlDocument pointer to the document object for this XML document
         */
        virtual MXmlDocument*   OwnerDocument() = 0;


        /**
         * Accessor interface for 'next sibling' of an XML element
         *
         * @since 1.0
         * @param None
         * @return An MXmlElement pointer to the next sibling of the current XML element
         */
        virtual MXmlElement*    NextSibling() = 0;

        /**
         * Accessor interface for 'parent node' of an XML element
         *
         * @since 1.0
         * @param None
         * @return An MXmlElement pointer to the parent node of the current XML element
         */
        virtual MXmlElement*    ParentNode() = 0;


        /**
         * Accessor to set first child of an XML element
         *
         * @since 1.0
         * @param An MXmlElement pointer to the object that is being set as first child
         * @return None
         */
        void virtual            SetFirstChild( MXmlElement* ) = 0;


        /**
         * Accessor to set last child of an XML element
         *
         * @since 1.0
         * @param An MXmlElement pointer to the object that is being set as last child
         * @return None
         */
        void virtual            SetLastChild( MXmlElement* ) = 0;


        /**
         * Accessor to set owner document of an XML element
         *
         * @since 1.0
         * @param An MXmlDocument pointer to the object that is being set as owner document
         * @return None
         */
        void virtual            SetOwnerDocument( MXmlDocument* ) = 0;


        /**
         * Accessor to set next sibling of an XML element
         *
         * @since 1.0
         * @param An MXmlElement pointer to the object that is being set as next sibling
         * @return None
         */
        void virtual            SetNextSibling( MXmlElement* ) = 0;


        /**
         * Accessor to set parent node of an XML element
         *
         * @since 1.0
         * @param An MXmlElement pointer to the object that is being set as parent node
         * @return None
         */
        void virtual            SetParentNode( MXmlElement* ) = 0;

 
        /**
         * A method to (deep) clone the current element
         *
         * @since 1.0
         * @param None
         * @return An MXmlElement pointer to the object that is a copy of the current element
         */
        virtual MXmlElement* CloneL(MXmlElement* aParentElement) = 0;

    };

#endif      // SVGXMLELEMENT_H
