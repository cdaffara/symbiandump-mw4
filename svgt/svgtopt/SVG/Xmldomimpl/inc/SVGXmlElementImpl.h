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


#ifndef __INC_CXMLELEMENTIMPL__
#define __INC_CXMLELEMENTIMPL__

#if !defined(__E32BASE_H__)
#include <e32base.h>
#endif

#include "SVGXmlElement.h"



class   MXmlDocument;


/**
 * Class description
 */
class CXmlElementImpl : public CBase, public MXmlElement
    {
    public:

        /**
         * Set the attribute value for a given attribute name of the current XML element
         * Actual specification requires a new node object be created for the new attribute
         * This implementation just sets a variable in the element class instance
         *
         * @since 1.0
         * @param aName - A string buffer containig the name of the attribute to be set
         * @param aValue - A string buffer containig the value of the attribute to be set
         * @return An integer containing success/failure for this action
         */
        virtual TInt            SetAttributeL( const TDesC& aName,
                                               const TDesC& aValue );


        /**
         * Remove attribute for an XML element
         * Actual specification requires the attribute node be removed
         * Since this implementation does not create an attribute node, it
         * just changes the state of the corresponding variable
         *
         * @since 1.0
         * @param aName - A string buffer containing the name of the attribute to
         *                be removed
         * @return An integer containing success/failure for this action
         */
        virtual TInt            RemoveAttribute( const TDesC& aName );


        /**
         * Accessor for the tag name of an XML element
         *
         * @since 1.0
         * @param None
         * @return A string buffer containing the tag name
         */
        virtual const TDesC&    TagName();


        /**
         * Append child method for an XML element
         *
         * @since 1.0
         * @param aAppendChild - The element object that needs to be appended
         * @return NULL if failed to append;
         *         A MXmlElement pointer to the object that was just appended
         */
        virtual MXmlElement*    AppendChildL( MXmlElement* aAppendChild, TBool aIsJSR226Element = EFalse );


        /**
         * Remove child method for an XML element
         *
         * @since 1.0
         * @param aRemoveChild - A pointer to the object that is to be removed
         * @return None
         */
        virtual void            RemoveChild ( MXmlElement* aRemoveChild );


        /**
         * 'Has child nodes' method that finds out if the current element has any child
         *  nodes
         *
         * @since 1.0
         * @param None
         * @return A boolean
         *         ETrue if there are child nodes; EFalse otherwise
         */
        virtual TBool           HasChildNodes();


        /**
         * Accessor method for 'first child' of an XML element
         *
         * @since 1.0
         * @param None
         * @return An MXmlElement pointer to the first child of the current XML element
         */
        virtual MXmlElement*    FirstChild();


        /**
         * Accessor method for 'last child' of an XML element
         *
         * @since 1.0
         * @param None
         * @return An MXmlElement pointer to the last child of the current XML element
         */
        virtual MXmlElement*    LastChild();


        /**
         * Accessor method for 'owner document' of an XML element
         *
         * @since 1.0
         * @param None
         * @return A MXmlDocument pointer to the document object for this XML document
         */
        virtual MXmlDocument*   OwnerDocument();


        /**
         * Accessor method for 'next sibling' of an XML element
         *
         * @since 1.0
         * @param None
         * @return An MXmlElement pointer to the next sibling of the current XML element
         */
        virtual MXmlElement*    NextSibling();
		virtual MXmlElement*	PreviousSibling();

        /**
         * Accessor method for 'parent node' of an XML element
         *
         * @since 1.0
         * @param None
         * @return An MXmlElement pointer to the parent node of the current XML element
         */
        virtual MXmlElement*    ParentNode();


        /**
         * Accessor to set first child of an XML element
         *
         * @since 1.0
         * @param An MXmlElement pointer to the object that is being set as first child
         * @return None
         */
        virtual void            SetFirstChild( MXmlElement* );


        /**
         * Accessor to set last child of an XML element
         *
         * @since 1.0
         * @param An MXmlElement pointer to the object that is being set as last child
         * @return None
         */
        virtual void            SetLastChild( MXmlElement* );


        /**
         * Accessor to set owner document of an XML element
         *
         * @since 1.0
         * @param An MXmlDocument pointer to the object that is being set as owner document
         * @return None
         */
        virtual void            SetOwnerDocument( MXmlDocument* );


        /**
         * Accessor to set next sibling of an XML element
         *
         * @since 1.0
         * @param An MXmlElement pointer to the object that is being set as next sibling
         * @return None
         */
        virtual void            SetNextSibling( MXmlElement* );


        /**
         * Accessor to set parent node of an XML element
         *
         * @since 1.0
         * @param An MXmlElement pointer to the object that is being set as parent node
         * @return None
         */
        virtual void            SetParentNode( MXmlElement* );


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        //        static CXmlElementImpl*     NewL( const TDesC& aTagName );


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        //        static CXmlElementImpl*     NewLC( const TDesC& aTagName );


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual                 ~CXmlElementImpl();

    protected:



        TUint8                  iElemID;
         /**
         * Initialize the state of XML element object
         *
         * @since 1.0
         * @param aTagName - A string buffer containing the tag name with which
         *                   the element object is being built
         * @return None
         */
        virtual void            InitializeL( const TUint8 aElemID );




        MXmlElement*            iParentNode;

        MXmlElement*            iFirstChild;

        MXmlElement*            iLastChild;

        MXmlElement*            iNextSibling;

        MXmlDocument*           iOwnerDocument;

    public:

        /**
         * A method that performs deep copy for this object
         *
         * @since 1.0
         * @param  None
         * @return An MXmlElement pointer to the newly created object
         */
        MXmlElement* CloneL(MXmlElement* aParentElement);

        /**
         * A method that return integer ID of the element
         *
         * @since 1.0
         * @param  None
         * @return An TUint8 value correspondign the ID of the element
         */

        virtual TUint8    ElemID();

    protected:

        /**
         * Copy this object to an object that is already created and initialized
         *
         * @since 1.0
         * @param aDestElement - A CXmlElementImpl pointer to an object
         *                       requiring the copy
         * @return None
         */
        void CopyL( CXmlElementImpl* aDestElement );

    };



#endif
