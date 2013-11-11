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
* Description:  SVG Implementation source file
 *
*/


#include "SVGUriReferenceImpl.h"
#include "SVGSchemaData.h"

_LIT( KXlinkActuate, "xlink:actuate" );
_LIT( KXlinkArcrole, "xlink:arcrole" );
_LIT( KXlinkRole, "xlink:role" );
_LIT( KXlinkShow, "xlink:show" );
_LIT( KXlinkTitle, "xlink:title" );
_LIT( KXlinkType, "xlink:type" );
_LIT( KXlinkHref, "xlink:href" );


// ---------------------------------------------------------------------------
// Two phase constructiion:
// - Implements the SVGXlink
// - There is no DOM interface defined.
// ---------------------------------------------------------------------------
CSvgUriReferenceImpl* CSvgUriReferenceImpl::NewL()
    {
    CSvgUriReferenceImpl*   self    = new ( ELeave ) CSvgUriReferenceImpl();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgUriReferenceImpl* CSvgUriReferenceImpl::NewLC()
    {
    CSvgUriReferenceImpl*   self    = new ( ELeave ) CSvgUriReferenceImpl();
    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgUriReferenceImpl::~CSvgUriReferenceImpl()
    {
	if ( iXlinkActuate )
		{
		delete iXlinkActuate;
		iXlinkActuate = NULL;
		}

	if ( iXlinkArcrole )
		{
		delete iXlinkArcrole;
		iXlinkArcrole = NULL;
		}

	if ( iXlinkRole )
		{
		delete iXlinkRole;
		iXlinkRole = NULL;
		}

	if ( iXlinkShow )
		{
		delete iXlinkShow;
		iXlinkShow = NULL;
		}

	if ( iXlinkTitle )
		{
		delete iXlinkTitle;
		iXlinkTitle = NULL;
		}

	if ( iXlinkType)
		{
		delete iXlinkType;
		iXlinkType = NULL;
		}

	if ( iXlinkHref )
		{
		delete iXlinkHref;
		iXlinkHref = NULL;
		}
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgUriReferenceImpl::ConstructL()
    {
    iXlinkActuate = HBufC::NewL( 0 );
    iXlinkArcrole = HBufC::NewL( 0 );
    iXlinkRole = HBufC::NewL( 0 );
    iXlinkShow = HBufC::NewL( 0 );
    iXlinkTitle = HBufC::NewL( 0 );
    iXlinkType = HBufC::NewL( 0 );
    iXlinkHref = HBufC::NewL( 0 );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
const TDesC& CSvgUriReferenceImpl::XlinkActuate()
    {
    return *iXlinkActuate;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
const TDesC& CSvgUriReferenceImpl::XlinkArcrole()
    {
    return *iXlinkArcrole;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
const TDesC& CSvgUriReferenceImpl::XlinkRole()
    {
    return *iXlinkRole;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
const TDesC& CSvgUriReferenceImpl::XlinkShow()
    {
    return *iXlinkShow;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
const TDesC& CSvgUriReferenceImpl::XlinkTitle()
    {
    return *iXlinkTitle;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
const TDesC& CSvgUriReferenceImpl::XlinkType()
    {
    return *iXlinkType;
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
const TDesC& CSvgUriReferenceImpl::XlinkHref()
    {
    return *iXlinkHref;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
const TDesC& CSvgUriReferenceImpl::Href()
    {
    return *iXlinkHref;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TDesC* CSvgUriReferenceImpl::HrefPointer()
    {
    return iXlinkHref;
    }

// Implementation

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TBool CSvgUriReferenceImpl::SetXlinkAttributeL( const TDesC& aPropertyName,
                                                const TDesC& aValue )
    {
    if ( aPropertyName == KXlinkActuate )
        {
        if(iXlinkActuate)
	        {

	        delete iXlinkActuate;
	        iXlinkActuate = NULL;

	        }
	    iXlinkActuate = aValue.AllocL();
        }
    else if ( aPropertyName == KXlinkArcrole )
        {
        if(iXlinkArcrole)
	        {
        	delete iXlinkArcrole;
        	iXlinkArcrole = NULL;

	        }
	    iXlinkArcrole = aValue.AllocL();
        }
    else if ( aPropertyName == KXlinkRole )
        {
        if(iXlinkRole)
	        {
	        delete iXlinkRole;
	        iXlinkRole = NULL;

	        }
	       iXlinkRole = aValue.AllocL();
        }
    else if ( aPropertyName == KXlinkShow )
        {
        if(iXlinkShow)
	        {
	        delete iXlinkShow;
    	    iXlinkShow = NULL;

	        }
	    iXlinkShow = aValue.AllocL();
        }
    else if ( aPropertyName == KXlinkTitle )
        {
        if(iXlinkTitle)
	        {
        	delete iXlinkTitle;
        	iXlinkTitle = NULL;

	        }
	      iXlinkTitle = aValue.AllocL();
        }
    else if ( aPropertyName == KXlinkType )
        {
        if(iXlinkType)
	        {
        	delete iXlinkType;
        	iXlinkType = NULL;

	        }
	      iXlinkType = aValue.AllocL();
        }
    else if ( aPropertyName == KXlinkHref )
        {
        if(iXlinkType)
	        {
        	delete iXlinkHref;
        	iXlinkHref = NULL;

	        }
	     iXlinkHref = aValue.AllocL();
        }
    else
        {
        return EFalse;
        }

    return ETrue;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgUriReferenceImpl* CSvgUriReferenceImpl::CloneL()
    {
    CSvgUriReferenceImpl* retValue = CSvgUriReferenceImpl::NewL();
	CleanupStack::PushL(retValue);
    // copy iXlinkActuate
    if(retValue->iXlinkActuate)
	    {
	    delete retValue->iXlinkActuate;
	    retValue->iXlinkActuate = NULL;

	    }
	retValue->iXlinkActuate = this->iXlinkActuate->AllocL();

    // copy iXlinkArcrole
    if(retValue->iXlinkArcrole)
	    {
    	delete retValue->iXlinkArcrole;
    	retValue->iXlinkArcrole = NULL;
	    }
	retValue->iXlinkArcrole = this->iXlinkArcrole->AllocL();

    // copy iXlinkRole
    if(retValue->iXlinkRole)
	    {
    delete retValue->iXlinkRole;
    retValue->iXlinkRole = NULL;

	    }
	 retValue->iXlinkRole = this->iXlinkRole->AllocL();


    // copy iXlinkShow
    if(retValue->iXlinkShow)
	    {
    	delete retValue->iXlinkShow;
    	retValue->iXlinkShow = NULL;

	    }
	 retValue->iXlinkShow = this->iXlinkShow->AllocL();

    // copy iXlinkTitle
    if(retValue->iXlinkTitle)
	    {
    delete retValue->iXlinkTitle;
    retValue->iXlinkTitle = NULL;

	    }
	    retValue->iXlinkTitle = this->iXlinkTitle->AllocL();

    // copy iXlinkType
    if(retValue->iXlinkType)
	    {
    delete retValue->iXlinkType;
    retValue->iXlinkType = NULL;

	    }
	  retValue->iXlinkType = this->iXlinkType->AllocL();

    // copy iXlinkHref
    if(retValue->iXlinkHref)
	    {
    delete retValue->iXlinkHref;
    retValue->iXlinkHref = NULL;

	    }
	    retValue->iXlinkHref = this->iXlinkHref->AllocL();

	CleanupStack::Pop();
    return retValue;
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgUriReferenceImpl::SetUriRefDesL( const TUint16 aPropertyName,
                                                const TDesC& aValue )
    {
	if ( aPropertyName == KAtrXlinkactuate )
			{
			if(iXlinkActuate)
				{
				delete iXlinkActuate;
				iXlinkActuate = NULL;
				}
	        iXlinkActuate = aValue.AllocL();
	        }
	    else if ( aPropertyName == KAtrXlinkarcrole )
	        {
	        if(iXlinkArcrole)
				{
				delete iXlinkArcrole;
				iXlinkArcrole = NULL;
				}
	        iXlinkArcrole = aValue.AllocL();
	        }
	    else if ( aPropertyName == KAtrXlinkrole )
	        {
	        	if(iXlinkRole)
				{
				delete iXlinkRole;
				iXlinkRole = NULL;
				}
		    iXlinkRole = aValue.AllocL();
		   }
	    else if ( aPropertyName == KAtrXlinkshow )
	        {
	        if(iXlinkShow)
				{delete iXlinkShow;
				iXlinkShow = NULL;
				}
	        iXlinkShow = aValue.AllocL();
	        }
	    else if ( aPropertyName == KAtrXlinktitle )
	        {
	        if(iXlinkTitle)
				{
				delete iXlinkTitle;
				iXlinkTitle = NULL;
				}
	       iXlinkTitle = aValue.AllocL();
	       }
	   else if ( aPropertyName == KAtrXlinktype )
	     {
	    if(iXlinkType)
			{
			delete iXlinkType;
			iXlinkType = NULL;
			}
	     iXlinkType = aValue.AllocL();
	     }
	   else if ( aPropertyName == KAtrXlinkhref )
	     {
	     if(iXlinkHref)
			{
			 delete iXlinkHref;
			iXlinkHref = NULL;
			}
	     iXlinkHref = aValue.AllocL();
	     }
	 else
	     {
	     return KErrNotFound;
	     }
    return KErrNone;
    }
