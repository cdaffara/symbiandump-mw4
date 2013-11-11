/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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


// INCLUDE FILES
#include "SVGSolidColorElementImpl.h"
#include "SVGFloatCssValueImpl.h"
#include "SVGDocumentImpl.h"
#include "SVGSchemaData.h"

_LIT(SOLIDOPACITY, "solid-opacity");
_LIT(INHERIT, "inherit");

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
CSvgSolidColorElementImpl* CSvgSolidColorElementImpl::NewL(const TUint8 aElemID,
																   CSvgDocumentImpl* aDoc )
	{
    CSvgSolidColorElementImpl *self = new (ELeave) CSvgSolidColorElementImpl(aDoc);
    CleanupStack::PushL(self);
    self->ConstructL(aElemID);
    CleanupStack::Pop(self);
    return self;
	}

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
CSvgSolidColorElementImpl* CSvgSolidColorElementImpl::NewLC(  const TUint8 aElemID,
                                                     CSvgDocumentImpl* aDoc )
    {
    CSvgSolidColorElementImpl*  self    = new ( ELeave )
                                      CSvgSolidColorElementImpl( aDoc );
    CleanupStack::PushL( self );
    self->ConstructL(  aElemID );

    return self;
    }


// ---------------------------------------------------------------------------
// Symbian default constructor that can leave.
// ---------------------------------------------------------------------------
void CSvgSolidColorElementImpl::ConstructL(const TUint8 aElemID)
    {
    CSvgElementImpl::InitializeL( aElemID );

    iSvgStyleProperties = new(ELeave) RPointerArray<CCssValue>(KCSS_MAX_ATTR);
    User::LeaveIfError( iSvgStyleProperties->Append( NULL ) );
    iSvgStyleProperties->Remove( 0 );

    iSvgUriReferenceImpl = CSvgUriReferenceImpl::NewL();
    iSvgTransformable = CSvgTransformableImpl::NewL();

    }
// ---------------------------------------------------------------------------
//  C++ default constructor
// ---------------------------------------------------------------------------

CSvgSolidColorElementImpl::CSvgSolidColorElementImpl( CSvgDocumentImpl* aDoc )
    {
    SetOwnerDocument(aDoc);

    iSolidOpacity = TFloatFixPt(1);

    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
CSvgSolidColorElementImpl::~CSvgSolidColorElementImpl()
    {

    if ( iSvgStyleProperties )
        {
        iSvgStyleProperties->Close();
        delete iSvgStyleProperties;
        iSvgStyleProperties = NULL;
        }

    if ( iSvgUriReferenceImpl )
        {
        delete iSvgUriReferenceImpl;
        iSvgUriReferenceImpl = NULL;
        }

    }

// ============================ MEMBER FUNCTIONS ========================================

// -----------------------------------------------------------------------------
// SetAttributeL: The implemented function will be called from CSvgContentHandler.
// SetAttributeFloatL: The implemented function will be called from CSvgContentHandler.
//
// From CSvgElementImpl
//
// There will be two attributes been set: solid-color and solid-opacity.
// These attributes are redundant and are same as fill and fill-opacity
//
// Returns: ETrue always in this case.
// -----------------------------------------------------------------------------
//
TInt CSvgSolidColorElementImpl::SetAttributeL( const TDesC& aName,
                                           const TDesC& aValue )
    {
     TFloatFixPt KZero;
     TFloatFixPt KOne( KFloatFixOne );

    	if(aName == SOLIDOPACITY)
        {
        if(aValue == INHERIT)
            {
            // inherit does not make any sense since the stop-opacity is
            //specific to stop element only and a stop element can not have another stop element as
            //its child.
            iSolidOpacity = KOne;
            }
        else
            {
    #ifdef SVG_FLOAT_BUILD
            iSolidOpacity = TFloatFixPt::ConvertString( aValue );
            if (!(iSolidOpacity > TFloatFixPt(0) && iSolidOpacity < TFloatFixPt(1)) )

    #else
            if ( TFloatFixPt::ConvertString (aValue, iSolidOpacity ) != KErrNone )
    #endif
                {
                iSolidOpacity = KOne;
                }
            else
                {
                if (aValue.Locate('%') != KErrNotFound)
		 			{
		 			//if stop opacity was listed as a percentage
		 			iSolidOpacity = iSolidOpacity / TFloatFixPt(100);
                    }

                if ( iSolidOpacity < KZero )
                    iSolidOpacity = KZero;
                else if ( iSolidOpacity > KOne )
                    iSolidOpacity = KOne;
                }
            }

            //CFloatCssValueImpl* lFloatObject = (CFloatCssValueImpl*)iSvgStyleProperties[KCSS_ATTR_FILL_OPACITY];
            CFloatCssValueImpl* lFloatObject;
            this->FindProperty( KCSS_ATTR_FILL_OPACITY, (CCssValue*&)lFloatObject, this );
            lFloatObject->SetValueL(iSolidOpacity);



		}

	CSvgElementImpl::SetAttributeL(aName,aValue);
    return KErrNone;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgSolidColorElementImpl::GetAttributeFloat(const TInt aNameId, TFloatFixPt& aValue)
{
	switch(aNameId)
	{
	case KAtrSolidOpacity:
		aValue = iSolidOpacity;
		break;
	default:
		CSvgElementImpl::GetAttributeFloat(aNameId,aValue);
		break;
	}

    return 0;

}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgSolidColorElementImpl::SetAttributeFloatL(const TInt aNameId,const TFloatFixPt aValue)
{
	switch(aNameId)
	{
    case KAtrSolidOpacity:
        {
        iSolidOpacity = aValue;

		//set the solid opacity into the fill-opacity here
//        CFloatCssValueImpl* lFloatObject = (CFloatCssValueImpl*)iSvgStyleProperties[KCSS_ATTR_FILL_OPACITY];
//        lFloatObject->SetValueL(iSolidOpacity);
        CFloatCssValueImpl* lFloatObject;
        this->FindProperty( KCSS_ATTR_FILL_OPACITY, (CCssValue*&)lFloatObject, this );
        lFloatObject->SetValueL(iSolidOpacity);

        }
		break;

    default:
        return CSvgElementImpl::SetAttributeFloatL(aNameId,aValue);
    }

    return 0;
}


// ---------------------------------------------------------------------------=======
// ApplyOpacitytoElement: Applies solid-opacity to appropriate element
// ---------------------------------------------------------------------------=======
void CSvgSolidColorElementImpl::ApplyOpacitytoElement(CSvgElementImpl* aElement)
     {
       CCssValue*  lCssValue = NULL;
       // get soild-opacity attribute from SolidColor element
       // solid-opacity is same as fill-opacity attribute
       this->FindProperty( KCSS_ATTR_FILL_OPACITY, lCssValue, this );
       if (lCssValue != NULL)
          {
          TBool lResult = ETrue;
          // apply solid-opacity to the element
          TRAPD( err, lResult = aElement->SetPropertyFloatL( KCSS_ATTR_FILL_OPACITY,
      	                 ((CFloatCssValueImpl*)lCssValue)->Value()) );
          if ( err || !lResult )
             {
           	#ifdef _DEBUG
            _LIT( KErrMsg, "CPaintCssValueImpl::Value() Error in SetValueL" );
             RDebug::Print(KErrMsg);
            #endif //_DEBUG*/
             }

           }//

     }

void CSvgSolidColorElementImpl::Print( TBool aIsEncodeOn )
{
	if (!aIsEncodeOn)
	{
		#ifdef _DEBUG
		RDebug::Printf("<solidColor/>", this);
		#endif
	}
}

//  End of File
