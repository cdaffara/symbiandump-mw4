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
* Description:  XML DOM Implementation source file
 *
*/


#include "SVGXmlElementImpl.h"
#include "SVGXmlDocument.h"

// ---------------------------------------------------------------------------
// This method acts like a ConstructL.
// This class is never instantiated
// The derived classes that are instantiated, call this method within
// their ConstructL methods
// ---------------------------------------------------------------------------

void CXmlElementImpl::InitializeL(  const TUint8 aElemID )
    {
    iElemID=aElemID;
    }

// ---------------------------------------------------------------------------
// Accessor for the integer ID of the current element
// ---------------------------------------------------------------------------
TUint8 CXmlElementImpl::ElemID()
    {
    return iElemID;
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
CXmlElementImpl::~CXmlElementImpl()
    {

    CXmlElementImpl* child = (CXmlElementImpl*)FirstChild();
    while ( child != NULL )
        {
        CXmlElementImpl* sibling = (CXmlElementImpl*)child->NextSibling();
        delete child;
        child = sibling;
        }
    }

// ---------------------------------------------------------------------------
// Set the value for a given attribute
// ---------------------------------------------------------------------------
TInt CXmlElementImpl::SetAttributeL( const TDesC& /* aName */,
                                     const TDesC& /* aValue */ )
    {
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// Remove a given attribute
// At this time this method is not implemented
// ---------------------------------------------------------------------------
TInt CXmlElementImpl::RemoveAttribute( const TDesC& /* aName */ )
    {
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// Accessor for the tag name of the current element
// ---------------------------------------------------------------------------
const TDesC& CXmlElementImpl::TagName()
    {

    HBufC *lEmptyBuf = NULL;
    return *lEmptyBuf;

    }

// ---------------------------------------------------------------------------
// Find out if the current element has child elements
// ---------------------------------------------------------------------------
TBool CXmlElementImpl::HasChildNodes()
    {
    return ( iFirstChild != NULL );
    }

// ---------------------------------------------------------------------------
// Accessor for the first child object of the current element
// ---------------------------------------------------------------------------
MXmlElement* CXmlElementImpl::FirstChild()
    {
    return iFirstChild;
    }

// ---------------------------------------------------------------------------
// Accessor for the last child object of the current element
// ---------------------------------------------------------------------------
MXmlElement* CXmlElementImpl::LastChild()
    {
    return iLastChild;
    }

// ---------------------------------------------------------------------------
// Accessor for the document object of the current element
// ---------------------------------------------------------------------------
MXmlDocument* CXmlElementImpl::OwnerDocument()
    {
    return iOwnerDocument;
    }

// ---------------------------------------------------------------------------
// Accessor for the next sibling (element) object of the current element
// ---------------------------------------------------------------------------
MXmlElement* CXmlElementImpl::NextSibling()
    {
    return iNextSibling;
    }

// ---------------------------------------------------------------------------
// Accessor for the previous sibling (element) object of the current element
// ---------------------------------------------------------------------------
MXmlElement* CXmlElementImpl::PreviousSibling()
    {
    CXmlElementImpl* sibling = this; 
    CXmlElementImpl* previousSibling = NULL;
    
    while ( sibling != NULL )
        {
        sibling = (CXmlElementImpl*)sibling->NextSibling();

        if (sibling == this)
        {
        	return previousSibling;
        }

        	previousSibling = sibling;
        }

    return NULL;
    }

// ---------------------------------------------------------------------------
// Accessor for the parent element object of the current element
// ---------------------------------------------------------------------------
MXmlElement* CXmlElementImpl::ParentNode()
    {
    return iParentNode;
    }

// ---------------------------------------------------------------------------
// Accessor to set the first child object of the current element
// ---------------------------------------------------------------------------
void CXmlElementImpl::SetFirstChild( MXmlElement* aElement )
    {
    iFirstChild = aElement;
    }

// ---------------------------------------------------------------------------
// Accessor to set the last child object of the current element
// ---------------------------------------------------------------------------
void CXmlElementImpl::SetLastChild( MXmlElement* aElement )
    {
    iLastChild = aElement;
    }

// ---------------------------------------------------------------------------
// Accessor to set the document object of the current element
// ---------------------------------------------------------------------------
void CXmlElementImpl::SetOwnerDocument( MXmlDocument* aElement )
    {
    iOwnerDocument = aElement;
    }

// ---------------------------------------------------------------------------
// Accessor to set the next sibling (element) object of the current element
// ---------------------------------------------------------------------------
void CXmlElementImpl::SetNextSibling( MXmlElement* aElement )
    {
    iNextSibling = aElement;
    }

// ---------------------------------------------------------------------------
// Accessor to set the parent element object of the current element
// ---------------------------------------------------------------------------
void CXmlElementImpl::SetParentNode( MXmlElement* aElement )
    {
    iParentNode = aElement;
    }

// ---------------------------------------------------------------------------
// Append a new object as the child of the current element
// Place the new object in the proper location, as the last child
// ---------------------------------------------------------------------------
MXmlElement* CXmlElementImpl::AppendChildL( MXmlElement* aNewChild, TBool /*aIsJSR226Element*/ )
    {
    // if no node was passed, then nothing to do
    if ( aNewChild == NULL )
        {
        return NULL;
        }

    // first remove the node from its parent if it has one
    MXmlElement* lparent = aNewChild->ParentNode();

    
    //AJD the parent != this needs to be removed so AppendChild works correctly
//	if ( parent != NULL && parent != this)

    if ( lparent != NULL )
        {
        lparent->RemoveChild(aNewChild);
        }

    // Set the new node's parent
    aNewChild->SetParentNode( this );
    // Set the new node's next sibling
    aNewChild->SetNextSibling( NULL );
    // If previous sibling is tracked:
    // Set the new node's previous sibling
    //        aNewChild->SetPreviousSibling(iXmlElementImpl->LastChild());
    // Set new node as the first child, if it is
    if ( FirstChild() == NULL )
        {
        SetFirstChild( aNewChild );
        }
    // Set the new node as the next sibling of previously last child
    MXmlElement* lLast   = LastChild();
    if ( lLast )
        {
        lLast->SetNextSibling( aNewChild );
        }
    // Set the new node as last child
    SetLastChild( aNewChild );

    // notity of change to document
    if ( iOwnerDocument )
        {
        aNewChild->SetOwnerDocument(iOwnerDocument);

        iOwnerDocument->ElementAppendedOrRemoved();
        }

    return aNewChild;
    }

// ---------------------------------------------------------------------------
// Remove a given object from the list of child elements
// ---------------------------------------------------------------------------
void CXmlElementImpl::RemoveChild ( MXmlElement* aRemoveChild )
    {
    if ( aRemoveChild )
        {
        // the remove child is the first child
        if ( FirstChild() == aRemoveChild )
            {
            if( aRemoveChild->NextSibling() )
                {
                SetFirstChild( aRemoveChild->NextSibling() );
                }
            else
                {
                // the remove child is the only child
                SetFirstChild( NULL );
                SetLastChild ( NULL );
                }
            }
        // the remove child is any sibling
        else
            {
            // find the child pointing to remove child
            MXmlElement*preSibling   = FirstChild();
            if (preSibling)
            {
            		while ( preSibling && (( preSibling->NextSibling() ) != aRemoveChild) )
                	{
                	preSibling = preSibling->NextSibling();
                	}
					
					if (preSibling)
					{
            			// check if the remove child is the end child
            			if ( LastChild() != aRemoveChild  )
                		{
                			preSibling->SetNextSibling(aRemoveChild->NextSibling());
                		}
            			else
                		{
                			SetLastChild(preSibling);
                		preSibling->SetNextSibling(NULL);
                		}
                	}
                }
            }

        // remove child
        aRemoveChild->SetParentNode( NULL );
        aRemoveChild->SetNextSibling( NULL );
        }

    // notity of change to document
    if ( iOwnerDocument )
        {
        iOwnerDocument->ElementAppendedOrRemoved();
        }
    }

// ---------------------------------------------------------------------------
// Perform a deep clone of this object
// This method is not currently implemented
// ---------------------------------------------------------------------------
MXmlElement* CXmlElementImpl::CloneL(MXmlElement* )
    {
    return NULL;
    }

// ---------------------------------------------------------------------------
// Copy the contents of this object to the passed object
// This method assumes that the passed object has already been
// Created using the proper NewL method.
// ---------------------------------------------------------------------------
void CXmlElementImpl::CopyL( CXmlElementImpl* aDestElement )
    {

    // copy the children and everything down the tree
    MXmlElement* currentChild = FirstChild();

    while ( currentChild != NULL)
        {
        MXmlElement* newElement = currentChild->CloneL(aDestElement);
		if( newElement == NULL )/*Duplicate Cloning of Use returns NULL*/
			{
			break; //breaking out of the while in case of loop
			}
        aDestElement->CXmlElementImpl::AppendChildL(newElement);
        currentChild = currentChild->NextSibling();
        }
    }
