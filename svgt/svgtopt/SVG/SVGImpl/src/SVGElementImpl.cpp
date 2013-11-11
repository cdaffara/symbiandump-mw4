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


#if !defined(__E32BASE_H__)
#include <e32base.h>
#endif

#include "SVGElementImpl.h"
#include "SVGXmlElementImpl.h"

#include "GfxAffineTransform.h"

#include "SVGClrCssValueImpl.h"
#include "SVGPaintCssValueImpl.h"
#include "SVGIntCssValueImpl.h"
#include "SVGFloatCssValueImpl.h"
#include "SVGStrCssValueImpl.h"
#include "SVGVectorCssValueImpl.h"

#include "SVGSchemaData.h"
#include "SVGAnimationBase.h"
#include "SVGLineElementImpl.h"
#include "SVGRectElementImpl.h"
#include "SVGCircleElementImpl.h"
#include "SVGMpathElementImpl.h"

#include "SVGFontFaceElementImpl.h"

#include "SVGDocumentImpl.h"
#include "SVGStringTokenizer.h"

#include "SVGEngineImpl.h"
#include "SVGUseElementImpl.h"
#include "SVGAnimationElementImpl.h"
#include "SVGTokenizer.h"

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgElementImpl::~CSvgElementImpl()
    {
     if ( iId )
        {
        delete iId;
        iId = NULL;
        }

    //need to remove from event list if it was an animation

    CSvgDocumentImpl* thisDocument = ( CSvgDocumentImpl* ) OwnerDocument();

    if (thisDocument != NULL)
    {
        thisDocument->RemoveFromEventReceiverList(this);
    }

    if ( iSvgTests )
    {
        delete iSvgTests;
        iSvgTests = NULL;
    }

    if( iSvgLangSpace )
    {
        delete iSvgLangSpace;
        iSvgLangSpace = NULL;
    }

    if ( iSvgTransformable )
        {
        delete iSvgTransformable;
        iSvgTransformable = NULL;
        }
    ResetReferenceAttrSet();
    }
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TBool CSvgElementImpl::StyleInParent(CSvgElementImpl* aElement, TInt aAttrIndex)
		{
			 if(aElement != NULL)
			 {
			   CSvgElementImpl* lParentElement = (CSvgElementImpl*)aElement->ParentNode ();
			   while(lParentElement && (lParentElement->iSvgStyleProperties != NULL))
			   {
			      if (aElement->iSvgStyleProperties->operator[](aAttrIndex) ==
			          lParentElement->iSvgStyleProperties->operator[](aAttrIndex))
			      {
			      	return ETrue;
			      }
			      lParentElement = (CSvgElementImpl*)lParentElement->ParentNode ();
			   }
			 }
		   return EFalse;
		}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgElementImpl::CSvgElementImpl() : iAnimateTransformMatrixIndex(-1),
									 iSvgStyleProperties(NULL),
                                     iAnimateMotionMatrixIndex( -1 )

    {
    SetAllAttributesAdded( ETrue );
    }

void CSvgElementImpl::InitializeL(  TUint8 aElemID)
    {
    CXmlElementImpl::InitializeL( aElemID );
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TBool CSvgElementImpl::IsDrawable()
    {
    return ETrue;
    }

//*****************************************************
//
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgElementImpl::DrawShapeL( CGfx2dGc* aGc,
                                  MGfxShape& aShape,
                                  CSvgElementImpl* /*aElement */)
    {
    if ( !AreAllAttributesAdded() )
        {
        return;
        }

    // Paint is a special case.
    // Paint must be reset locally

    SetGCL( aGc/*, aElement */);

    if( iSvgTransformable )
        {
        aGc->SetTransform( GetCTM() );
        }

    aGc->DrawL( &aShape );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgElementImpl::DrawShapeL( CGfx2dGc* aGc, CSvgElementImpl* /*aElement*/ )
    {

    SetGCL( aGc/*, aElement */);

    if(iSvgTransformable )
        {
        aGc->SetTransform( GetCTM() );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TBool CSvgElementImpl::DrawL( CGfx2dGc* /* aGc */,
                              CSvgElementImpl* /* aElement */ )
    {
    return ETrue;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
const TDesC* CSvgElementImpl::Id()
    {
        return iId;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgElementImpl::SetIdandXmlbaseL( const TDesC& aPropertyName,
                                       const TDesC& aValue )
    {
    _LIT( KId, "id" );
    if ( aPropertyName == KId)
        {
        delete iId;
        iId = NULL;
        iId = aValue.AllocL();
        }
    else
        {
        return EFalse;
        }
    return ETrue;
    }

// ---------------------------------------------------------------------------
//
// This method might leave with KErrNoMemory.
// ---------------------------------------------------------------------------
MXmlElement* CSvgElementImpl::AppendChildL( MXmlElement* aNewChild, TBool aIsJSR226Element )
    {

    CXmlElementImpl::AppendChildL( aNewChild );

    if ( aNewChild  && (((CSvgElementImpl *)aNewChild)->ElemID() != KSvgSvgElement) )
        {
        TInt err = KErrNone;

        if (!aIsJSR226Element)
        {
        	err = ((CSvgElementImpl *)aNewChild)->InitSvgStyleProperties();
        }
        else
        {
   				// Error return needs to be checked
        	((CSvgElementImpl*)aNewChild)->InitAllSvgStyleProperties((CSvgElementImpl*)aNewChild);
        }
        if (err != KErrNone)
            #ifdef _Debug
                RDebug::Print(_L("CSvgElementImpl::AppendChildL[%x]: memory when append child"), this);
            #endif //_Debug
            User::Leave(err);
        }


    return aNewChild;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------

TInt CSvgElementImpl::SetAttributeL( const TDesC&  aName ,
                                     const TDesC&  aValue  )
    {


    if ( SetTransform( aName, aValue ) ||
         SetIdandXmlbaseL( aName, aValue ) ||
         SetLangSpaceAttributeL( aName, aValue ) ||
         SetTestAttributeL( aName, aValue )
       )
        {
        return KErrNone;
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgElementImpl::RemoveAttribute( const TInt aAttrId )
    {
    if( iSvgStyleProperties && aAttrId < iSvgStyleProperties->Count() )
        {
        CCssValue *lCurrentCssPointer = iSvgStyleProperties->operator[](aAttrId);
        CCssValue *lParentCssPointer = NULL ;

        CSvgElementImpl* lParentElement = (CSvgElementImpl*)iParentNode;
        while(lParentElement && (lParentElement->iSvgStyleProperties == NULL))
            {
            lParentElement = (CSvgElementImpl*)lParentElement->ParentNode ();
            }

        if ( lParentElement )
            {
            FindProperty( (TInt8)aAttrId, lParentCssPointer, ( CSvgElementImpl * )lParentElement );
            }

        iSvgStyleProperties->operator[](aAttrId) = lParentCssPointer;
        PropagateToChildren( lCurrentCssPointer,lParentCssPointer, aAttrId );
        }
    return KErrNone;

    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------

void CSvgElementImpl::SetPresentationAttribute( const TInt8 aPropertyId,
                                                CCssValue*& aCssValue )

    {
    if( iSvgStyleProperties && aPropertyId < iSvgStyleProperties->Count() )
        {
        iSvgStyleProperties->operator[](aPropertyId) = aCssValue;
        }

    }

// ---------------------------------------------------------------------------
// This method sets a property value that takes a float
// The switch statement can be expanded to add more attributes
// ---------------------------------------------------------------------------
TBool CSvgElementImpl::SetPropertyFloatL( const TInt& aAttrId,
                                                TReal32 aValue)
    {
    CSvgDocumentImpl*   doc      = ( CSvgDocumentImpl* ) OwnerDocument();

    if (!doc)
    {
    	return EFalse;
    }

    TInt                position = aAttrId;


    CSvgElementImpl* lParentElement = (CSvgElementImpl*)iParentNode;
    while(lParentElement && (lParentElement->iSvgStyleProperties == NULL))
        {
        lParentElement = (CSvgElementImpl*)lParentElement->ParentNode ();
        }

        switch ( position )
        {
        case KCSS_ATTR_STROKE_OPACITY:
        case KCSS_ATTR_FILL_OPACITY:
        case KCSS_ATTR_GROUP_OPACITY:
            {
            TFloatFixPt KZero;
            TFloatFixPt KOne( KFloatFixOne );
            TFloatFixPt opacityValue = aValue;
            if (opacityValue < KZero)
               {
               opacityValue = KZero;
               }
            else if (opacityValue > KOne)
               {
               opacityValue = KOne;
               }
            CFloatCssValueImpl* tParentValue = NULL;
            CFloatCssValueImpl* tValue = (CFloatCssValueImpl *)(iSvgStyleProperties->operator[](position));

            if( this != doc->RootElement() && lParentElement )
                {
                tParentValue  = (CFloatCssValueImpl *)
                                    (lParentElement->iSvgStyleProperties->operator[](position));
                }
            if (tParentValue == tValue || tValue == NULL)
                {
                tValue = doc->iMemoryManager->GetCssFloatObjectL( opacityValue );

                iSvgStyleProperties->operator[](position) = tValue;
                }
            else
                {
                tValue->SetValueL( opacityValue );
                }
            }
        break;
        default:
        return EFalse;

        }

    return ETrue;

    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------

TBool CSvgElementImpl::SetPropertyL( const TInt& aAttrId,
                                const TDesC& aValue)
    {
    _LIT( KInherit, "inherit" );

    CSvgDocumentImpl*   doc                 = ( CSvgDocumentImpl* ) OwnerDocument();

    if (!doc)
    {
    	return EFalse;
    }

    CSvgSchemaData*     lSchemaData         = doc->SchemaData();

    TInt                position    = aAttrId;


    if( (aValue == KInherit) || (iSvgStyleProperties == NULL))
        {
        return ETrue;
        }

    CSvgElementImpl* lParentElement = (CSvgElementImpl*)iParentNode;
    while(lParentElement && (lParentElement->iSvgStyleProperties == NULL))
        {
        lParentElement = (CSvgElementImpl*)lParentElement->ParentNode ();
        }


    switch ( position )
        {

        case KCSS_ATTR_FILL:
            //fill
            {
            CPaintCssValueImpl* tParentValue = NULL;
            CPaintCssValueImpl* tValue = (CPaintCssValueImpl *)(iSvgStyleProperties->operator[](position));
            if( this != doc->RootElement() && lParentElement)
                {
                tParentValue  = (CPaintCssValueImpl *)
                            (lParentElement->iSvgStyleProperties->operator[](position));
                }
            if (tParentValue == tValue || tValue == NULL)
                {
				tValue = doc->iMemoryManager->GetCssPaintObjectL(aValue, this);


                if ( tValue->IsDefaultSet() )
                	{
                	// if default color is set, check see if parent has one
                	if ( lParentElement )
                		{
		                tParentValue  = (CPaintCssValueImpl *)
	                            (lParentElement->iSvgStyleProperties->operator[](position));
                		}

                	if ( tParentValue != NULL )
                		{
		                iSvgStyleProperties->operator[](position) = tParentValue;
		                break;
                		}

                	}
                else
               		{
	                iSvgStyleProperties->operator[](position) = tValue;
               		}
                }
            else
                {
                tValue->SetValueL( aValue );
                }

            break;

            }
        case KCSS_ATTR_STROKE_OPACITY:
        case KCSS_ATTR_FILL_OPACITY:
        case KCSS_ATTR_GROUP_OPACITY:
            {
            TFloatFixPt KZero;
            TFloatFixPt KOne( KFloatFixOne );
            TFloatFixPt opacityValue;
            if ( TFloatFixPt::ConvertString( aValue, opacityValue ) == KErrNone )
                {
                if (aValue.Locate('%') != KErrNotFound)
                    {
                    //if a percentage
                    opacityValue = opacityValue / TFloatFixPt(100);
                    }
                }
            else
                {
                opacityValue = KOne;
                }

                if (opacityValue < KZero)
                opacityValue = KZero;
            else if (opacityValue > KOne)
                opacityValue = KOne;

            CFloatCssValueImpl* tParentValue = NULL;
            CFloatCssValueImpl* tValue = (CFloatCssValueImpl *)(iSvgStyleProperties->operator[](position));

            if( this != doc->RootElement() && lParentElement )
                {
                tParentValue  = (CFloatCssValueImpl *)
                                    (lParentElement->iSvgStyleProperties->operator[](position));
                }
            if (tParentValue == tValue || tValue == NULL)
                {
               	tValue = doc->iMemoryManager->GetCssFloatObjectL(opacityValue);

                iSvgStyleProperties->operator[](position) = tValue;
                }
            else
                {
                tValue->SetValueL( opacityValue );
                }
            }
        break;

        case KCSS_ATTR_STROKE:
        case KCSS_ATTR_COLOR:
            {

            CClrCssValueImpl* tParentValue = NULL;
            CClrCssValueImpl* tValue = (CClrCssValueImpl *)(iSvgStyleProperties->operator[](position));
            if( this != doc->RootElement() && lParentElement )
                {
                tParentValue  = (CClrCssValueImpl *)
                                    (lParentElement->iSvgStyleProperties->operator[](position));
                }
            if (tParentValue == tValue || tValue == NULL)
                {
                tValue = doc->iMemoryManager->GetCssClrObjectL( aValue );

                iSvgStyleProperties->operator[](position) = tValue;
                }
            else
                {
                tValue->SetValueL( aValue );
                }

            break;

            }
        case KCSS_ATTR_VISIBILITY:
        // visibility
            {
            TPtrC               lVal;
            if ( aValue == _L( "visible" ) )
                {
                lVal.Set( _L( "0" ) );
                }
            else if ( aValue == _L( "hidden" ) )
                {
                lVal.Set( _L( "1" ) );
                }
            else if ( aValue == _L( "collapse" ) )
                {
                lVal.Set( _L( "3" ) );
                }

            CIntCssValueImpl* tParentValue = NULL;
            CIntCssValueImpl* tValue = (CIntCssValueImpl *)(iSvgStyleProperties->operator[](position));
            if( this != doc->RootElement() && lParentElement )
                {
                tParentValue  = (CIntCssValueImpl *)
                            (lParentElement->iSvgStyleProperties->operator[](position));
                }
            if (tParentValue == tValue || tValue == NULL)
                {
                tValue = doc->iMemoryManager->GetCssIntObjectL( lVal );

                iSvgStyleProperties->operator[](position) = tValue;
                }
            else
                {
                tValue->SetValueL( lVal );
                }
            }
        break;

        case KCSS_ATTR_DISPLAY:
        // display
            {

            TPtrC               lVal;
            if ( aValue == _L( "inline" ) )
                {
                lVal.Set( _L( "0" ) );
                }
            else if ( aValue == _L( "none" ) )
                {
                lVal.Set( _L( "16" ) );
                }

            CIntCssValueImpl* tParentValue = NULL;
            CIntCssValueImpl* tValue = (CIntCssValueImpl *)(iSvgStyleProperties->operator[](position));
            if( this != doc->RootElement() && lParentElement)
                {
                tParentValue  = (CIntCssValueImpl *)
                            (lParentElement->iSvgStyleProperties->operator[](position));
                }
            if (tParentValue == tValue || tValue == NULL)
                {
                tValue = doc->iMemoryManager->GetCssIntObjectL( lVal );

                iSvgStyleProperties->operator[](position) = tValue;
                }
            else
                {
                tValue->SetValueL( lVal );
                }

            break;
            }
        case KCSS_ATTR_FONTSTYLE:
        // font-style
            {
            TInt               i;

            i = lSchemaData->FindFontStyle(aValue);

            TBufC<12>   buf;
            TPtr        lVal    ( buf.Des() );
            _LIT( KFixptFormat, "%d" );
            lVal.Format( KFixptFormat, i );

            CIntCssValueImpl* tParentValue = NULL;
            CIntCssValueImpl* tValue = (CIntCssValueImpl *)(iSvgStyleProperties->operator[](position));
            if( this != doc->RootElement() && lParentElement )
                {
                tParentValue  = (CIntCssValueImpl *)
                            (lParentElement->iSvgStyleProperties->operator[](position));
                }
            if (tParentValue == tValue || tValue == NULL)
                {
                tValue = doc->iMemoryManager->GetCssIntObjectL( lVal );

                iSvgStyleProperties->operator[](position) = tValue;
                }
            else
                {
                tValue->SetValueL( lVal );
                }
            }
        break;
        case KCSS_ATTR_FONTWEIGHT:
        // font-weight
            {

            TInt               i;
            i = lSchemaData->FindFontWeightEnum(aValue);
            TBufC<12>           buf;
            TPtr        lVal    ( buf.Des() );
            _LIT( KFixptFormat, "%d" );
            lVal.Format( KFixptFormat, i );

            CIntCssValueImpl* tParentValue = NULL;
            CIntCssValueImpl* tValue = (CIntCssValueImpl *)(iSvgStyleProperties->operator[](position));
            if( this != doc->RootElement() && lParentElement)
                {
                tParentValue  = (CIntCssValueImpl *)
                                    (lParentElement->iSvgStyleProperties->operator[](position));
                }
            if (tParentValue == tValue || tValue == NULL)
                {

                tValue = doc->iMemoryManager->GetCssIntObjectL( lVal );

                iSvgStyleProperties->operator[](position) = tValue;
                }
            else
                {
                tValue->SetValueL( lVal );
                }
            }
        break;

        case KCSS_ATTR_FONTFAMILY:
            if( ElemID() == KSvgFontfaceElement )
                {
                ((CSvgFontFaceElementImpl *)this)->SetFontFamilyL(aValue);
                }
				break;
				
        case KCSS_ATTR_STROKE_LINECAP:
        case KCSS_ATTR_STROKE_LINEJOIN:
        case KCSS_ATTR_FILLRULE:
            {
            CStrCssValueImpl* tParentValue = NULL;
            CStrCssValueImpl* tValue = (CStrCssValueImpl *)(iSvgStyleProperties->operator[](position));
            if( this != doc->RootElement() && lParentElement )
                {
                tParentValue  = (CStrCssValueImpl *)
                                        (lParentElement->iSvgStyleProperties->operator[](position));
                }
            if (tParentValue == tValue || tValue == NULL)
                {
                tValue = doc->iMemoryManager->GetCssStrObjectL( aValue );
                iSvgStyleProperties->operator[](position) = tValue;
                }
            else
                {
                tValue->SetValueL( aValue );
                }
            }
        break;
        case KCSS_ATTR_STROKE_DASHARRAY:
            {

            CVectorCssValueImpl* tParentValue = NULL;
            CVectorCssValueImpl* tValue = (CVectorCssValueImpl *)(iSvgStyleProperties->operator[](position));
            if( this != doc->RootElement() && lParentElement)
                {
                tParentValue  = (CVectorCssValueImpl *)
                                    (lParentElement->iSvgStyleProperties->operator[](position));
                }
            if (tParentValue == tValue || tValue == NULL)
                {
                tValue = doc->iMemoryManager->GetCssVectorObjectL( aValue );
                iSvgStyleProperties->operator[](position) = tValue;
                }
            else
                {
                tValue->SetValueL( aValue );
                }

            if(tValue)
	            {
                CArrayFix<TFloatFixPt>* lDashArray = tValue->Value();
				TFloatFixPt lSum( 0 );
				TUint numElem = lDashArray->Count();
				// Iterate through the array to check whether the sum of elements
				// of the dash array is non-zero.
				for (TUint arrCount = 0; arrCount < numElem; arrCount++)
					{
					lSum = lSum + lDashArray->At(arrCount);
					}
				// If the sum is zero then, the behaviour should be similar to "none" i.e. no dashing applied.
				if ( lSum == TFloatFixPt( 0 ) )
				    {
				    iSvgStyleProperties->operator[](position)= NULL;


				    // Not deleting the tValue as this would be freed in the
				    // destructor of CSvgMemoryManager
				    }
    	        }

            }
        break;
        case KCSS_ATTR_STROKEWIDTH:
            {
            // stroke-width can not be negative
            TReal32 strokeWidth;
            TPtrC remainder;
            TBool validValue = TTokenizer::ParseDecimal( aValue, strokeWidth, remainder );

            // Check for valid number characters and no characters following
            // and must be non-negative
            if ( !validValue || strokeWidth < 0.0f )
                {
                strokeWidth = 1.0f;
                }
			/*
			"1pt" equals "1.25px" (and therefore 1.25 user units)
			"1pc" equals "15px" (and therefore 15 user units)
			"1mm" would be "3.543307px" (3.543307 user units)
			"1cm" equals "35.43307px" (and therefore 35.43307 user units)
			"1in" equals "90px" (and therefore 90 user units)
			*/
            else if ( remainder.Length() > 0 )
            	{
            	if ( remainder == _L("px") )
	            	{
	            	// do nothing
	            	}
            	if ( remainder == _L("pt") )
	            	{
	            	strokeWidth *= 1.25f;
	            	}
            	else if ( remainder == _L("pc") )
	            	{
	            	strokeWidth *= 15.0f;
	            	}
            	else if ( remainder == _L("mm") )
	            	{
	            	strokeWidth *= 3.543307f;
	            	}
            	else if ( remainder == _L("cm") )
	            	{
	            	strokeWidth *= 35.43307f;
	            	}
            	else if ( remainder == _L("in") )
	            	{
	            	strokeWidth *= 90.0f;
	            	}
            	else
	            	{
	                strokeWidth = 1.0f;
	            	}
            	}

            CFloatCssValueImpl* tParentValue = NULL;
            CFloatCssValueImpl* tValue = (CFloatCssValueImpl *)(iSvgStyleProperties->operator[](position));
            if( this != doc->RootElement() && lParentElement)
                {
                tParentValue  = (CFloatCssValueImpl *)
                                (lParentElement->iSvgStyleProperties->operator[](position));
                }
            if (tParentValue == tValue || tValue == NULL)
                {
                tValue = doc->iMemoryManager->GetCssFloatObjectL( strokeWidth );
                iSvgStyleProperties->operator[](position) = tValue;
                }
            else
                {
                tValue->SetValueL( strokeWidth );
                }
            }
        break;

        case KCSS_ATTR_FONTSIZE:
        case KCSS_ATTR_STROKE_DASHOFFSET:
        case KCSS_ATTR_STROKE_MITERLIMIT:
            {
            CFloatCssValueImpl* tParentValue = NULL;
            CFloatCssValueImpl* tValue = (CFloatCssValueImpl *)(iSvgStyleProperties->operator[](position));
            if( this != doc->RootElement() && lParentElement )
                {
                tParentValue  = (CFloatCssValueImpl *)
                                    (lParentElement->iSvgStyleProperties->operator[](position));
                }
            if (tParentValue == tValue || tValue == NULL)
                {
                tValue = doc->iMemoryManager->GetCssFloatObjectL(aValue);

                iSvgStyleProperties->operator[](position) = tValue;
                }
            else
                {
                tValue->SetValueL( aValue );
                }
            }
        break;
        case KCSS_ATTR_TEXTANCHOR:
        //text-anchor
            {

            TInt               i;

            i = lSchemaData->FindTextAnchorEnum(aValue);

            TBufC<12>   buf;
            TPtr        lVal    ( buf.Des() );
            _LIT( KFixptFormat, "%d" );
            lVal.Format( KFixptFormat, i );

            CIntCssValueImpl* tParentValue = NULL;
            CIntCssValueImpl* tValue = (CIntCssValueImpl *)(iSvgStyleProperties->operator[](position));
            if( this != doc->RootElement() && lParentElement )
                {
                tParentValue  = (CIntCssValueImpl *)
                                    (lParentElement->iSvgStyleProperties->operator[](position));
                }
            if (tParentValue == tValue || tValue == NULL)
                {
                tValue = doc->iMemoryManager->GetCssIntObjectL( lVal );

                iSvgStyleProperties->operator[](position) = tValue;
                }
            else
                {
                tValue->SetValueL( lVal );
                }
            }
        break;
        case KCSS_ATTR_TEXTDECORATION:
        //text-decoration
            {

            TInt               i;
            i = lSchemaData->FindTextDecorationEnum(aValue);
            TBufC<12>   buf;
            TPtr        lVal    ( buf.Des() );
            _LIT( KFixptFormat, "%d" );
            lVal.Format( KFixptFormat, i );

            CIntCssValueImpl* tParentValue = NULL;
            CIntCssValueImpl* tValue = (CIntCssValueImpl *)(iSvgStyleProperties->operator[](position));
            if( this != doc->RootElement() && lParentElement)
                {
                tParentValue  = (CIntCssValueImpl *)
                                    (lParentElement->iSvgStyleProperties->operator[](position));
                }
            if (tParentValue == tValue || tValue == NULL)
                {
                tValue = doc->iMemoryManager->GetCssIntObjectL(lVal);

                iSvgStyleProperties->operator[](position) = tValue;
                }
            else
                {
                tValue->SetValueL( lVal );
                }
            }
        break;

        default:
        return EFalse;

        }

    return ETrue;

    }



// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TBool CSvgElementImpl::SetTransform( const TDesC& aAttributeName,
                                     const TDesC& aValue )
    {

    if( iSvgTransformable == NULL )
        {
        return EFalse;
        }

    // If this attribute is not transform attribute, return immediately.
    _LIT( KTransform, "transform" );
    if ( aAttributeName != KTransform )
        return EFalse;

    TLex                input   ( aValue );
    TReal32             M00, M10, M01, M11, M02, M12;
    TGfxAffineTransform tempTransform;
    MSvgTransformList*  trList;

    iSvgTransformable->GetTransform( trList );
    TGfxAffineTransform originalTransform   = trList->GetItem( 0 );

    _LIT( KMatrixP, "matrix(" );
    _LIT( KTranslateP, "translate(" );
    _LIT( KScaleP, "scale(" );
    _LIT( KRotateP, "rotate(" );
    _LIT( KSkewxP, "skewX(" );
    _LIT( KSkewyP, "skewY(" );

    while ( !( input.Eos() ) )
        {
        input.Mark();
        input.SkipCharacters();
        TPtrC   token   = input.MarkedToken();    // then extract token

        if ( token == KMatrixP )
            {
            input.SkipSpace();  if ( input.Val( M00, '.' ) != KErrNone ) return EFalse;
            input.SkipSpace();  if ( input.Val( M10, '.' ) != KErrNone ) return EFalse;
            input.SkipSpace();  if ( input.Val( M01, '.' ) != KErrNone ) return EFalse;
            input.SkipSpace();  if ( input.Val( M11, '.' ) != KErrNone ) return EFalse;
            input.SkipSpace();  if ( input.Val( M02, '.' ) != KErrNone ) return EFalse;
            input.SkipSpace();  if ( input.Val( M12, '.' ) != KErrNone ) return EFalse;

            //check to make sure the user isn't setting in scale of 0 because that can never be undone
            //they probably just mean that they don't want to scale
           /* if (M00 == 0)
            {
            	M00 = 1;
            }
            if (M11 == 0)
            {
            	M11 = 1;
            }*/

            tempTransform.SetTransform( M00, M10, M01, M11, M02, M12 );
            originalTransform.Concatenate( tempTransform );
            trList->ReplaceItem( originalTransform, 0 );
            }
        else if ( token == KTranslateP )
            {
            // parse tx
            input.SkipSpace();
            if ( input.Val( M00, '.' ) != KErrNone ) return EFalse;

            // parse ty
            input.SkipSpace();
            if ( input.Val( M10, '.' ) != KErrNone ) M10 = 0.0;

            // set translate
            TFloatFixPt  fixM00  ( M00 );
            TFloatFixPt  fixM10  ( M10 );
            originalTransform.Translate( fixM00, fixM10 );
            trList->ReplaceItem( originalTransform, 0 );
            }
        else if ( token == KScaleP )
            {
            input.SkipSpace();
            if(input.Peek().IsDigit() || (input.Peek() == '-') || (input.Peek() == '.'))
                {
                if ( input.Val( M00, TChar('.') ) != KErrNone )
                    return EFalse;
                }
            else
                {
                M00 = 1.0;
                }
            input.SkipSpace();
            // If only one value is given then both scale values are set to that value
            if ( input.Eos() || input.Peek() == ')' )
                {
                M11 = M00;
                }
            else
                {
                if(input.Peek().IsDigit() || (input.Peek() == '-') || (input.Peek() == '.'))
                    {
                    if ( input.Val( M11, TChar('.')) != KErrNone )
                        return EFalse;
                    }
                else
                    {
                    M11 = 1.0;
                    }
                }
            originalTransform.Scale( M00, M11 );
            trList->ReplaceItem( originalTransform, 0 );
            }
        else if ( token == KRotateP )
            {
            // parse r
            input.SkipSpace();
            if ( input.Val( M00, '.' ) != KErrNone ) return EFalse;
            // parse cx
            input.SkipSpace();
            if ( input.Val( M01, '.' ) == KErrNone )
                {
                // parse cy
                input.SkipSpace();
                if ( input.Val( M10, '.' ) != KErrNone ) return EFalse;
                M00 = M00 * KGradiansPerDegree; // Convert to Radians
                originalTransform.Rotate( M00, TFloatFixPt(M01), TFloatFixPt(M10) );
                trList->ReplaceItem( originalTransform, 0 );
                }
            else
                {
                M00 = M00 * KGradiansPerDegree; // Convert to Radians
                originalTransform.Rotate( M00 );
                trList->ReplaceItem( originalTransform, 0 );
                }
            }
        else if ( token == KSkewxP )
            {
            input.SkipSpace();  if ( input.Val( M00, '.' ) != KErrNone ) return EFalse;
            M00 = M00 * KGradiansPerDegree; // Convert to Radians
            originalTransform.Shear( M00, 0.0f  );
            trList->ReplaceItem( originalTransform, 0 );
            }
        else if ( token == KSkewyP )
            {
            input.SkipSpace();  if ( input.Val( M00, '.' ) != KErrNone ) return EFalse;
            M00 = M00 * KGradiansPerDegree; // Convert to Radians
            originalTransform.Shear(  0.0f, M00 );
            trList->ReplaceItem( originalTransform, 0 );
            }

        // Skip close paranthesis
        input.SkipSpace();
        input.Mark();
        input.MarkedToken();    // then extract token
        }
    return ETrue;
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TSvgColor* CSvgElementImpl::CurrentColor()
    {


    if( iSvgStyleProperties )
        {
        CCssValue*  lColorValue = NULL;
        lColorValue = iSvgStyleProperties->operator[](KCSS_ATTR_COLOR);

        if (    lColorValue  &&
                ( ( CClrCssValueImpl * ) lColorValue )->Value() &&
                ( ( TSvgColor*)(( CClrCssValueImpl * ) lColorValue )->Value())->GetColor() != KSvgCurrentColor
            )
            {
            // current color found and useful, return it
            return ( TSvgColor*)(( CClrCssValueImpl * ) lColorValue )->Value();
            }
        else
            {
            // current color found, but it is "currentColor"
            // find the real color from searching parents

            if ( iParentNode )
                {
                return ( ( CSvgElementImpl * ) iParentNode )->CurrentColor();
                }
            }
        }
    else
        {
        if ( iParentNode )
            {
            return ( ( CSvgElementImpl * ) iParentNode )->CurrentColor();
            }
        }
    return NULL;
    }

void CSvgElementImpl::InitAllSvgStyleProperties(CSvgElementImpl* aElement)
{
	if (aElement && aElement->ParentNode())
	{
		aElement->InitSvgJSRStyleProperties((CSvgElementImpl*)aElement->ParentNode());
	}

	if (aElement && aElement->iSvgStyleProperties && aElement->iSvgStyleProperties->Count() == KCSS_MAX_ATTR)
	{
		CSvgElementImpl* lChildElement = (CSvgElementImpl*)aElement->FirstChild();

		if ( lChildElement )
		{
			InitAllSvgStyleProperties( lChildElement );
		}

		CSvgElementImpl* lSiblingElement = (CSvgElementImpl*)aElement->NextSibling();

		if ( lSiblingElement )
		{
			InitAllSvgStyleProperties( lSiblingElement );
		}
	}
}

void CSvgElementImpl::InitSvgJSRStyleProperties(CSvgElementImpl* aParentElement)
{
	if ( iSvgStyleProperties && iSvgStyleProperties->Count() == KCSS_MAX_ATTR )
    {
    	//JSR226 ELEMENT CREATION WILL COME TO THIS
        // any non NULL values in the iSvgStyleProperties array should be kept and
        // the NULL ones should be replaced by the parent values

        if( aParentElement && aParentElement->iSvgStyleProperties)
            {
                TInt  count = aParentElement->iSvgStyleProperties->Count();
                for ( TInt i = 0; i < count; i++ )
                {
                    CCssValue*   lCssValue   = aParentElement->iSvgStyleProperties->operator[]( i );
                    CCssValue*   thisCssValue = this->iSvgStyleProperties->operator[]( i );

                    if( lCssValue && !thisCssValue )
                    {
                        // the parent had a property but we just had NULL, so lets inherit the parent property
                        (*iSvgStyleProperties)[i] = lCssValue;
                    }
                    else
                    {
                        //this element must have a new value of its own in it so leave it alone
                    }
                }
            }
    }
}

TInt CSvgElementImpl::InitSvgStyleProperties( )
    {
    // all styles from all elements for the normal engine
    // flow will be setup in this function

    TInt err(KErrNone);

    if( iSvgStyleProperties && iSvgStyleProperties->Count() == 0 )
        {
        //NORMAL ENGINE ELEMENT CREATION...
        CSvgElementImpl* lParentElement = (CSvgElementImpl*)iParentNode;
        while(lParentElement && ((lParentElement->iSvgStyleProperties == NULL) || (lParentElement->iSvgStyleProperties->Count() == 0)) )
            {
            lParentElement = (CSvgElementImpl*)lParentElement->ParentNode ();
            }

        if( lParentElement )
            {
            TInt  count       = lParentElement->iSvgStyleProperties->Count();
            for ( TInt i = 0; i < count; i++ )
                {
                CCssValue*   lCssValue   = lParentElement->iSvgStyleProperties->operator[]( i );
                err = iSvgStyleProperties->Insert(lCssValue,i);
                if (err)
                    return err;
                }
            }
        }

    return err;
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------

void CSvgElementImpl::FindProperty( const TInt8 aPropertyId,
                                    CCssValue*& aCssValue,
                                    CSvgElementImpl* aElement )

    {
        if (aElement != NULL)
            aElement->FindProperty(aPropertyId, aCssValue);
        else
            FindProperty(aPropertyId, aCssValue);
    }

void CSvgElementImpl::FindProperty( const TInt8 aPropertyId,
                                    CCssValue*& aCssValue)

    {
        if( iSvgStyleProperties && iSvgStyleProperties->Count() > aPropertyId)
        {
        aCssValue = iSvgStyleProperties->operator[](aPropertyId);
        }
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgElementImpl::SetTransform( MSvgTransformList*& aTransformList )
    {
    if( !iSvgTransformable )
    {
        // iSvgTransformable should not be NULL (created by subclass)
        TRAP_IGNORE( iSvgTransformable = CSvgTransformableImpl::NewL() );
    }

    if( iSvgTransformable )
        {
        iSvgTransformable->SetTransform( aTransformList );
        }
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgElementImpl::GetTransform( MSvgTransformList*& aTransformList )
    {
    if( !iSvgTransformable )
    {
        // iSvgTransformable should not be NULL (created by subclass)
        TRAP_IGNORE( iSvgTransformable = CSvgTransformableImpl::NewL() );
    }

     if( iSvgTransformable )
        {
        iSvgTransformable->GetTransform( aTransformList );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
const TGfxAffineTransform& CSvgElementImpl::GetCTM()
    {
    if( !iSvgTransformable )
    {
    	// iSvgTransformable should not be NULL (created by subclass)
    	TRAP_IGNORE( iSvgTransformable = CSvgTransformableImpl::NewL() );
    }
    return( iSvgTransformable->GetCTM() );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgElementImpl::SetCTM( TGfxAffineTransform& aTr )
    {
    if( !iSvgTransformable )
    {
        // iSvgTransformable should not be NULL (created by subclass)
        TRAP_IGNORE( iSvgTransformable = CSvgTransformableImpl::NewL() );
    }

    if( iSvgTransformable )
        {
        iSvgTransformable->SetCTM( aTr );
        }
    else
    	{
			#ifdef _DEBUG
    		RDebug::Printf("CSvgElementImpl::SetCTM element %d wasnt transformable CTM not set", this->ElemID());
			#endif
    	}
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgElementImpl::SetGCL( CGfx2dGc* aGc)
    {


    TSvgColor*          lStroke         = NULL;
    MGfxPaint*          lFill           = NULL;
    TGfxStroke          lStrokeC;
    TFloatFixPt              lStrokeWidth    = 1;
    TPtrC               lFillRule; // Should be a defined constant for nonzero
    TPtrC               lSLinecap; // Should be a defined constant for linecap
    TPtrC               lSLinejoin; // Should be a defined constant for linejoin
    TInt                lFontStyle      = -1; // Should be a defined constant for fontstyle
    TFloatFixPt              lFontSize       = 10; // Should be a defined constant for fontstyle
    TInt                lFontWeight     = -1; // Should be a defined constant for fontweight
    TInt                lTextanchor     = -1;
    TInt                lTextDecoration = -1;
    TFloatFixPt              lDashOffset     = 0;
    TFloatFixPt              lSMiterlimit; // Should be a defined constant for strokemiterlimit
    CArrayFix<TFloatFixPt>*  lDasharray      = NULL;

    CCssValue*          lCssValue       = NULL;

    // Get fill color

    FindProperty( KCSS_ATTR_FILL, lCssValue);
    if ( lCssValue )
        {
        // Currently only TSvgColor is represented by PaintCssValue
        // It will be extended to handle CGfxTexturePaint and CGfxGradientPaint
        lFill = ( ( CPaintCssValueImpl * ) lCssValue )->Value();
        // Set fill color
        if(lFill)
            {
            if (lFill->GetColor() == KSvgCurrentColor )
                {

                lFill = CurrentColor();
                if (lFill && lFill->GetColor() != KGfxColorNull )
                    {
                    aGc->SetPaint( lFill );

                    }
                else
                    {
                    aGc->SetPaint( NULL );
                    }
                }
            else if (lFill->GetColor() != KGfxColorNull )
                {
                 aGc->SetPaint( lFill );
                }
            else
                {
                aGc->SetPaint( NULL );
                }
            }
        else
            {
            aGc->SetPaint( NULL );
            }
        }
    else
        {
        TSvgColor   black   ( 0 );
        aGc->SetPaint( &black );
        }

    // this is just regular opacity which needs to be an entire elements opacity
    // Opacity set using stroke and fill opacity
    CCssValue* lOpacityCssValue = NULL;
    FindProperty( KCSS_ATTR_GROUP_OPACITY, lOpacityCssValue);
    if ( lOpacityCssValue && ( ((CFloatCssValueImpl*)lOpacityCssValue)->Value() < TFloatFixPt(1) ) )
        {
        aGc->SetStrokeOpacity( ( ( CFloatCssValueImpl * ) lOpacityCssValue )->Value() );
        aGc->SetFillOpacity( ( ( CFloatCssValueImpl * ) lOpacityCssValue )->Value() );
        }

    lCssValue       = NULL;
    FindProperty( KCSS_ATTR_STROKE_OPACITY, lCssValue);
    if ( lCssValue )
    {
    	TFloatFixPt lValue = ( ( CFloatCssValueImpl * ) lCssValue )->Value();
        aGc->SetStrokeOpacity( lValue );
    }
    else
    {
        aGc->SetStrokeOpacity(1);
    }

    lCssValue       = NULL;
    FindProperty( KCSS_ATTR_FILL_OPACITY, lCssValue);
    if ( lCssValue )
    {
    	TFloatFixPt lValue = ( ( CFloatCssValueImpl * ) lCssValue )->Value();
        aGc->SetFillOpacity( lValue );
    }
    else
    {
        aGc->SetFillOpacity(1);
    }

    // Get stroke color
    lCssValue       = NULL;
    FindProperty( KCSS_ATTR_STROKE, lCssValue);

    if ( lCssValue )
        {
        lStroke = ( ( CClrCssValueImpl * ) lCssValue )->Value();
        // Set stroke color
        if ( lStroke && lStroke->GetColor () == KSvgCurrentColor )
            {
            lStroke = CurrentColor();
            if(lStroke)
                {
                aGc->SetForegroundColor( *lStroke );
                }
            }
        else if ( lStroke )
            {
            aGc->SetForegroundColor( *lStroke );
            }
        }

    // Get stroke-width
    lCssValue       = NULL;
    FindProperty( KCSS_ATTR_STROKEWIDTH, lCssValue);

    if ( lCssValue )
        {
        lStrokeWidth = ( ( CFloatCssValueImpl * ) lCssValue )->Value();
        if (lStrokeWidth <= TFloatFixPt(0))
            {
            lStrokeC.SetStrokeWidth( TFloatFixPt(0) );
            }
        else
            {
            // Set stroke-width
            lStrokeC.SetStrokeWidth( lStrokeWidth );
            }
         }

    // Get and set fill-rule
    lCssValue       = NULL;
    FindProperty( KCSS_ATTR_FILLRULE, lCssValue);

    if ( lCssValue )
        {
        lFillRule.Set( ( ( CStrCssValueImpl * ) lCssValue )->Value() );
            if ( lFillRule == _L("evenodd" ) )// evenodd
            {
            aGc->SetWindingRule( EGfxWindEvenOdd );
            }
            else if ( lFillRule == _L("nonzero") )
            {
            	aGc->SetWindingRule( EGfxWindNonZero );
            }
        }

    // Get and set stroke-linecap
    lCssValue       = NULL;
    FindProperty( KCSS_ATTR_STROKE_LINECAP, lCssValue);

    if ( lCssValue )
        {
        lSLinecap.Set( ( ( CStrCssValueImpl * ) lCssValue )->Value() );

        if (lSLinecap == _L("butt"))
        {
            lStrokeC.SetCapType( EGfxCapButt );
        }
        else if (lSLinecap == _L("round"))
        {
            lStrokeC.SetCapType( EGfxCapRound );
        }
        else if (lSLinecap == _L("square"))
        {
            lStrokeC.SetCapType( EGfxCapSquare );
        }
        }

    lCssValue = NULL;
    FindProperty( KCSS_ATTR_STROKE_LINEJOIN, lCssValue);

    if ( lCssValue )
        {
        lSLinejoin.Set( ( ( CStrCssValueImpl * ) lCssValue )->Value() );

        if (lSLinejoin == _L("miter"))
        {
            lStrokeC.SetJoinType( EGfxJoinMiter );
        }
        else if (lSLinejoin == _L("round"))
        {
            lStrokeC.SetJoinType( EGfxJoinRound );
        }
        else if (lSLinejoin == _L("bevel"))
        {
            lStrokeC.SetJoinType( EGfxJoinBevel );
        }
        }

   
    lCssValue = NULL;

    FindProperty( KCSS_ATTR_STROKE_DASHOFFSET, lCssValue);
      if ( lCssValue )
        {
        lDashOffset = ( ( CFloatCssValueImpl * ) lCssValue )->Value();
        // Set dashoffset into GC
        aGc->SetDashOffset( lDashOffset );
        }

    // Get and set stroke-dasharray
    lCssValue       = NULL;
    FindProperty( KCSS_ATTR_STROKE_DASHARRAY, lCssValue);
      if ( lCssValue )
        {
        ( ( CVectorCssValueImpl * ) lCssValue )->Value( lDasharray );
        // Set stroke-dasharray into GC
        aGc->SetDashArrayL( lDasharray );
        }

    // get and set stroke-miterlimit
    lCssValue       = NULL;
    FindProperty( KCSS_ATTR_STROKE_MITERLIMIT, lCssValue);

    if ( lCssValue )
        {
        lSMiterlimit = ( ( CFloatCssValueImpl * ) lCssValue )->Value();

        lStrokeC.SetMiterLimit(lSMiterlimit);
        }

    if( iElemID == KSvgTextAreaElement || iElemID == KSvgTextElement)
    {
        lCssValue       = NULL;
        FindProperty( KCSS_ATTR_FONTSIZE, lCssValue);

	    if ( lCssValue )
	        {
	        lFontSize = ( ( CFloatCssValueImpl * ) lCssValue )->Value();

		#ifdef SVG_FLOAT_BUILD
	        if(lFontSize <= TFloatFixPt ( 0 ) )
	        {
	            lFontSize = TFloatFixPt ( 10 );
	        }
		#else
	        if(lFontSize <= TFloatFixPt ( 0,ETrue ) )
	            lFontSize = TFloatFixPt ( 10 );
		#endif
	        aGc->SetFontSize( lFontSize );
	        }

	    lCssValue       = NULL;
	    FindProperty( KCSS_ATTR_FONTSTYLE, lCssValue);

	    if ( lCssValue )
	        {
	        lFontStyle = ( ( CIntCssValueImpl * ) lCssValue )->Value();
	        aGc->SetFontStyle( lFontStyle );
	        }

	    // Get and set font-weight
	    lCssValue       = NULL;
	    FindProperty( KCSS_ATTR_FONTWEIGHT, lCssValue);

	    if ( lCssValue )
	        {
	        lFontWeight = ( ( CIntCssValueImpl * ) lCssValue )->Value();
	        aGc->SetFontWeight( lFontWeight );
	        }

	    // Get and set text-anchor
	    lCssValue       = NULL;
	    FindProperty( KCSS_ATTR_TEXTANCHOR, lCssValue);

	    if ( lCssValue )
	        {
	        lTextanchor = ( ( CIntCssValueImpl * ) lCssValue )->Value();
	        aGc->SetTextanchor( lTextanchor );
	        }

	    // Get and set text-decoration
	    lCssValue       = NULL;
	    FindProperty( KCSS_ATTR_TEXTDECORATION, lCssValue);

	    if ( lCssValue )
	        {
	        lTextDecoration = ( ( CIntCssValueImpl * ) lCssValue )->Value();
	        aGc->SetTextDecoration( lTextDecoration );
	        }
	    }
		// Sset the stroke into the graphics library now that we are done
		aGc->SetStroke( lStrokeC );

    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgElementImpl::UpdateCTM( )
{
    CSvgDocumentImpl* lDocument = (CSvgDocumentImpl*)(this->OwnerDocument());

	if ( !lDocument )
	{
		return;
	}

    TInt lElementId = this->ElemID();
    if( this->IsTransformable() )
        {
        MSvgTransformList*  trList;

        // Current delta transformation due to animation, etc.
        TGfxAffineTransform localUserSpace;
        // Element's CTM
        TGfxAffineTransform lCurrentTM;


        // we do not need this for the radial and linear Gradient Element.
        //
        if ( this != lDocument->RootElement()
                && !(lElementId == KSvgLinearGradientElement
                || lElementId == KSvgRadialGradientElement ))

            {
            CSvgElementImpl* lParentEl   = ( CSvgElementImpl* )this->ParentNode();
            while (lParentEl && !(lParentEl->IsTransformable()) )
                {
                lParentEl   = ( CSvgElementImpl* )lParentEl->ParentNode();
                }
            if( lParentEl )
                {
                lCurrentTM = lParentEl->GetCTM();
                }
            }

        this->GetTransform( trList );
        localUserSpace = trList->Consolidate( IsOverwriteTransforms() );
        lCurrentTM.Concatenate( localUserSpace );

        if(lElementId == KSvgUseElement)
            {
            lCurrentTM.Translate(((CSvgUseElementImpl *)this)->X(),((CSvgUseElementImpl *)this)->Y() );
            }
       /* else if(lElementId == KSvgAnimationElement)
            {
            lCurrentTM.Translate(((CSvgAnimationElementImpl *)this)->X(),((CSvgAnimationElementImpl *)this)->Y() );
            }*/
        else if ( lElementId == KSvgSvgElement )
            {
            // Viewbox
            CSvgSvgElementImpl* tmpSvgEl    = ( CSvgSvgElementImpl* )
                                              this;
            TGfxRectangle2D     tmpRect     ( tmpSvgEl->X(),
                                              tmpSvgEl->Y(),
                                              tmpSvgEl->Width(),
                                              tmpSvgEl->Height() );

            // Add stop for bogus viewports being set if the SVG element hasn't been set
            // from the Java side.
                  if (tmpRect.iWidth >= TFloatFixPt(0) && tmpRect.iHeight >= TFloatFixPt(0))
                  {
                    tmpSvgEl->SetWindowViewportTrans( tmpRect, lCurrentTM );
                  }

            }

        this->SetCTM( lCurrentTM );
        }

    CSvgElementImpl*lNewElement = ( CSvgElementImpl* )
                                  this->FirstChild();
    while ( lNewElement != NULL )
        {
        lNewElement->UpdateCTM();
        lNewElement = ( CSvgElementImpl * ) lNewElement->NextSibling();
        }
}





// *******************************************************

// ---------------------------------------------------------------------------
// From MSvgTests
// ---------------------------------------------------------------------------
void CSvgElementImpl::GetRequiredFeatures( const CDesCArrayFlat*& aRequiredFeatures )
    {
    if( iSvgTests )
        {
        iSvgTests->GetRequiredFeatures( aRequiredFeatures );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgElementImpl::GetRequiredExtensions( const CDesCArrayFlat*& aRequiredExtensions )
    {
    if( iSvgTests )
        {
        iSvgTests->GetRequiredExtensions( aRequiredExtensions );
        }
    }

// ---------------------------------------------------------------------------
// returns false if system language was empty string
// ---------------------------------------------------------------------------
TBool CSvgElementImpl::GetSystemLanguage( const CDesCArrayFlat*& aSystemLanguage )
    {
    if( iSvgTests )
        {
        return (iSvgTests->GetSystemLanguage( aSystemLanguage ));
        }

        return ETrue;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TBool CSvgElementImpl::HasExtension( const TDesC& aExtension )
    {
    if( iSvgTests )
        {
        return iSvgTests->HasExtension( aExtension );
        }
    return EFalse;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TBool CSvgElementImpl::HasAnyTests( )
    {
    if( iSvgTests )
        {
        return iSvgTests->HasAnyTests( );
        }
    return EFalse;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TBool CSvgElementImpl::HasFeature( const TDesC& aFeature )
    {
    if( iSvgTests )
        {
        return iSvgTests->HasFeature( aFeature );
        }
    return EFalse;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TBool CSvgElementImpl::SetTestAttributeL( const TDesC& aPropertyName,
                                          const TDesC& aValue )
    {

    _LIT( KRequiredFeatures, "requiredFeatures" );
    _LIT( KRequiredExtensions, "requiredExtensions" );
    _LIT( KSystemLanguage, "systemLanguage" );

    if (aPropertyName == KRequiredFeatures || aPropertyName == KRequiredExtensions || aPropertyName == KSystemLanguage)
    {
        if (!iSvgTests)
        {
            iSvgTests = CSvgTestsImpl::NewL();
        }

        return iSvgTests->SetTestAttributeL( aPropertyName, aValue );
    }

    return EFalse;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgElementImpl::CreateSvgTestL(const TUint16 aAttrName)
{
	if (aAttrName == KAtrRequiredFeatures || aAttrName == KAtrRequiredExtensions || aAttrName == KAtrSystemLanguage)
	{
		if (!iSvgTests)
        {
            iSvgTests = CSvgTestsImpl::NewL();
        }
	}
}
// *******************************************************
// ---------------------------------------------------------------------------
// From MSvgLangSpace.h
// ---------------------------------------------------------------------------
const TDesC& CSvgElementImpl::XMLLang()
    {
    if( iSvgLangSpace )
        {
        return iSvgLangSpace->XMLLang();
        }
    else
        {
        return KNullDesC;
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgElementImpl::SetXMLLangL( const TDesC& aXMLLang )
    {
    if( iSvgLangSpace )
        {
        iSvgLangSpace->SetXMLLangL( aXMLLang );
        }
    return KErrNone;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
const TDesC& CSvgElementImpl::XMLSpace()
    {
    if( iSvgLangSpace )
        {
        return iSvgLangSpace->XMLSpace();
        }
    else
        {
        return KNullDesC;
        }

    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgElementImpl::SetXMLSpaceL( const TDesC& aXMLSpace )
    {
    if(  iSvgLangSpace )
        {
        iSvgLangSpace->SetXMLSpaceL( aXMLSpace );
        }
    return KErrNone;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TBool CSvgElementImpl::SetLangSpaceAttributeL( const TDesC& aPropertyName,
                                               const TDesC& aValue )
{
        _LIT( KXMLLang, "xml:lang" );
        _LIT( KXMLSpace, "xml:space" );

    if (aPropertyName == KXMLLang || aPropertyName == KXMLSpace)
    {
        if (!iSvgLangSpace)
        {
            iSvgLangSpace = CSvgLangSpaceImpl::NewL();
        }

        return iSvgLangSpace->SetLangSpaceAttributeL( aPropertyName, aValue );
    }
    else
    {
        return EFalse;
    }
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TBool CSvgElementImpl::SetXlinkAttributeL( const TDesC& aPropertyName,
                                           const TDesC& aValue )
    {
    if( iSvgUriReferenceImpl )
        {
        return iSvgUriReferenceImpl->SetXlinkAttributeL( aPropertyName, aValue );
        }
    else
        {
        return EFalse;
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
const TDesC& CSvgElementImpl::Href()
    {
    if( iSvgUriReferenceImpl )
        {
        return iSvgUriReferenceImpl->Href();
        }
    return KNullDesC;

    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TDesC* CSvgElementImpl::HrefPointer()
    {
    if( iSvgUriReferenceImpl )
        {
        return iSvgUriReferenceImpl->HrefPointer();
        }
    return NULL;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------

const TDesC& CSvgElementImpl::XlinkShow()
    {
    if(iSvgUriReferenceImpl )
        {
        return iSvgUriReferenceImpl->XlinkShow();
        }
    return KNullDesC;
    }


// *******************************************************
// From MSvgEventReceiver
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TBool CSvgElementImpl::ReceiveEventL( MSvgEvent* aEvent, const TUint16 aSubEventMask )
    {
    CSvgEngineImpl* engine  = ( ( CSvgDocumentImpl* ) OwnerDocument() )->Engine();

    // Accepts Internal event and mouse events
    switch ( aEvent->EventType() )
        {
        case ESvgEngineInternalEvent:
        break;
        case ESvgEngineEventMouseDown:
        case ESvgEngineEventMouseUp:
        case ESvgEngineEventMouseMove:
            {
            // Check if pointer is inside of the element
            MSvgUiMouseEvent*   mevent  = ( MSvgUiMouseEvent* ) aEvent;
            //TGfxRectangle2D p(mevent->X(), mevent->Y(), 0,0);
            TGfxPoint2D         p       ( mevent->X(), mevent->Y() );
            TGfxRectangle2D     bbox;
            TInt32 lVisible;
            TInt32 lDisplay;
            this->GetAttributeIntL(KCSS_ATTR_VISIBILITY,lVisible);
            this->GetAttributeIntL(KCSS_ATTR_DISPLAY,lDisplay);
            GetBBox( bbox );
            if ( bbox.Contains( p ) && lDisplay != 16 && lVisible == 0 )
                {
                // Invoke internal event
                switch ( aEvent->EventType() )
                    {
                    case ESvgEngineEventMouseDown:
                        {
                        TSvgInternalEvent   evt ( ESvgEventMousedown,
                                                  ( CSvgElementImpl* )
                                                       this );
                        engine->ProcessEventL( 
                            ( CSvgDocumentImpl* )OwnerDocument(), &evt );
                        SetMouseDown( ETrue );
                        }
                    break;
                    case ESvgEngineEventMouseUp:
                        {
                        TSvgInternalEvent   evt ( ESvgEventMouseup,
                                                  ( CSvgElementImpl* )
                                                       this );
                        engine->ProcessEventL( 
                            ( CSvgDocumentImpl* )OwnerDocument(), &evt );
                        
                        if ( IsMouseDown() )
                            {
                            // Check the aSubEventMask to know which event 
                            // has occured                                                   
                            if(CanGenerateInternalEvent(aSubEventMask,ESvgEventActivate))
                               {
                                
                               TSvgInternalEvent   evt1 (
                                                    ESvgEventActivate,
                                                    ( CSvgElementImpl* )this );
                            
                               engine->ProcessEventL(
                                        ( CSvgDocumentImpl* ) OwnerDocument(),
                                         &evt1 );
                               }
                            if ( CanGenerateInternalEvent(aSubEventMask,ESvgEventClick))
                                {
                               
                               TSvgInternalEvent   evt ( 
                                                    ESvgEventClick,
                                                    ( CSvgElementImpl* )this );
                            
                               engine->ProcessEventL( 
                                        ( CSvgDocumentImpl* ) OwnerDocument(),
                                         &evt );
                                  
                                }                                
                            }
                        SetMouseDown( EFalse );
                        }
                    break;
                    case ESvgEngineEventMouseMove:
                        {
                        
                        engine->NotifyElementActivatedLinkEntered(this);
                        
                        if ( CanGenerateInternalEvent(aSubEventMask,ESvgEventMousemove) )
                        	{
                         
                            TSvgInternalEvent evt1( 
                                                  ESvgEventMousemove,
                                                  ( CSvgElementImpl* )this );
                            engine->ProcessEventL( 
                                        ( CSvgDocumentImpl* ) OwnerDocument(),
                                         &evt1 );
                            }
                        
                        if ( !IsMouseIn() )
                            {
                            
                            engine->NotifyInteractiveElementEntered(this);                       
                            
                            // Check the aSubEventMask to know which event 
                            // has occured
                            if ( CanGenerateInternalEvent(aSubEventMask,ESvgEventMouseover) )
                                {
                         
                                TSvgInternalEvent   evt ( 
                                                    ESvgEventMouseover,
                                                    ( CSvgElementImpl* )this );
                                engine->ProcessEventL( 
                                        ( CSvgDocumentImpl* ) OwnerDocument(),
                                         &evt );
                                }
                        
                            if ( CanGenerateInternalEvent(aSubEventMask,ESvgEventFocusin) )
                                {
                        
                                TSvgInternalEvent   evt2 ( 
                                                    ESvgEventFocusin,
                                                    ( CSvgElementImpl* )this );
                                    
                                engine->ProcessEventL( 
                                        ( CSvgDocumentImpl* ) OwnerDocument(),
                                         &evt2 );
                                    
                                 }
                            }
                        SetMouseIn( ETrue );
                        }
                    break;
                    default:
                        break;
                    }
                }
            else
                {
                engine->NotifyElementActivatedLinkExited(this);
                    
                                
                // if state was 'mouse down'then reset the state
                SetMouseDown( EFalse );

                // if state was 'mouse in' and event is ESvgEngineEventMouseMove,
                // invoke mouseout event
                
                //MouseMove: in case of scrolling over a element focusOut events must
                //           be generated
                
                //MouseDown: In touch cases, if you tap on a element and then tap outside
                //          then a focusout event must be generated
                if ( IsMouseIn() &&
                     (aEvent->EventType() == ESvgEngineEventMouseMove || aEvent->EventType() == ESvgEngineEventMouseDown )
                     )
                    {
                    
                    // Check the aSubEventMask to know whether any valid bit  
                    // is set
                    if(engine->Document()->IsValidSubEventMask(aSubEventMask))
                        {
                        engine->NotifyInteractiveElementExited(this);    
                        }               
                    if ( CanGenerateInternalEvent(
                                    aSubEventMask,ESvgEventMouseout ) )
                        {
                        TSvgInternalEvent   mouseOutEvent ( 
                                                    ESvgEventMouseout, this );
                        engine->ProcessEventL( 
                                    ( CSvgDocumentImpl* ) OwnerDocument(),
                                     &mouseOutEvent );                                    
                        }
                    if ( CanGenerateInternalEvent(
                                    aSubEventMask,ESvgEventFocusout))
                        {
                        TSvgInternalEvent   focusOutEvent ( 
                                                    ESvgEventFocusout, this );
                        engine->ProcessEventL( 
                                    ( CSvgDocumentImpl* ) OwnerDocument(),
                                     &focusOutEvent ); 
                        }
                    }                
                    SetMouseIn( EFalse );
                }
            }
        break;
        default:
        break;
        }

    return EFalse;
    }
    
    // *******************************************************
// From MSvgEventReceiver
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TBool CSvgElementImpl::ReceiveEventL( MSvgEvent* /*aEvent*/ )
    {
    // Dummy implementation
    return EFalse;
    }




// *******************************************************
// From MXmlElementOpt
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgElementImpl::GetAttributeIntL( const TInt aNameId, TInt32& aValue )
    {


    CCssValue*          lCssValue           = NULL;


    if( aNameId == KAtrAnimMotionMatrixIndex )
        {
        aValue = AnimateMotionMatrixIndexL();
        }
    else if(aNameId== KAtrAnimTransformMatrixIndex)
        {
        aValue = AnimateTransformMatrixIndexL();
        }
    else
        {
        if( (aNameId >= KCSS_MAX_ATTR || aNameId < 0))
            {
            return KErrInvalidAttribute;
            }

				if (iSvgStyleProperties == NULL )
				{
					//if the style properties havent been set then just get a default one to be safe
					GetDefaultAttributeL(aNameId, lCssValue);
				}
				else
				{
        			lCssValue = iSvgStyleProperties->operator[](aNameId);
    			}

        if( lCssValue )
            {
          	aValue = (( CIntCssValueImpl* )lCssValue)->Value();

            if( aNameId == KCSS_ATTR_FILL )
                {

                MGfxPaint* lPaintValue = (MGfxPaint *)((( CPaintCssValueImpl* )lCssValue)->Value());
				/*
                if(lPaintValue && lPaintValue->GetColor()== KSvgCurrentColor)
                    {
                    lPaintValue = CurrentColor();
                    aValue = lPaintValue->GetColor();                     
                    }
                else*/ if(lPaintValue)
                	{
                	aValue = lPaintValue->GetColor();
                	}
                else
                	{
                    return KErrNoAttribute;
                	}
                }
            else if(aNameId == KCSS_ATTR_STROKE || aNameId == KCSS_ATTR_COLOR)
                {
                TSvgColor *lColorValue  = (( CClrCssValueImpl* )lCssValue)->Value();
                if(lColorValue)
                	{
                	aValue = lColorValue->GetColor();
                	}
                else
                	{
                    return KErrNoAttribute;
                	}
                }
            return KErrNone;
            }
        else
            {
            return KErrNoAttribute;
            }
        }
    return KErrNone;

    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgElementImpl::SetAttributeIntL( const TInt aNameId,
                                        const TInt32 aValue )
    {


    CSvgDocumentImpl*   doc                 = ( CSvgDocumentImpl* ) OwnerDocument();

    if (!doc)
    {
    	return KErrNoAttribute;
    }

    if( aNameId >= KCSS_MAX_ATTR  || (iSvgStyleProperties == NULL))
        {
        return KErrNoAttribute;
        }

    CSvgElementImpl* lParentElement = (CSvgElementImpl*)ParentNode ();
    while(lParentElement && (lParentElement->iSvgStyleProperties == NULL))
        {
        lParentElement = (CSvgElementImpl*)lParentElement->ParentNode ();
        }
    switch(aNameId)
        {
        case KCSS_ATTR_FILL:
            {
            CPaintCssValueImpl* tValue = (CPaintCssValueImpl *)(iSvgStyleProperties->operator[](aNameId));
            CPaintCssValueImpl* tParentValue = NULL ;
            if( this != doc->RootElement() && lParentElement )
                {
                tParentValue  = (CPaintCssValueImpl *)(lParentElement->iSvgStyleProperties->operator[](aNameId));
                }

            if( tParentValue == tValue || tValue == NULL) //Not Yet set
                {
                tValue = doc->iMemoryManager->GetCssPaintObjectL();

                CCssValue*  oldValue = iSvgStyleProperties->operator[](aNameId);
                PropagateToChildren( oldValue, tValue, aNameId );
                iSvgStyleProperties->operator[](aNameId) = tValue;
                }
            tValue->SetValueL( aValue );

            break;
            }
        case KCSS_ATTR_STROKE:
        case KCSS_ATTR_COLOR:
            {

            CClrCssValueImpl* tValue = (CClrCssValueImpl *)(iSvgStyleProperties->operator[](aNameId));
            CClrCssValueImpl* tParentValue = NULL ;
            if( this != doc->RootElement() && lParentElement)
                {
                tParentValue  = (CClrCssValueImpl *)(lParentElement->iSvgStyleProperties->operator[](aNameId));
                }

            if( tParentValue == tValue || tValue == NULL) //Not Yet set
                {
                tValue = doc->iMemoryManager->GetCssClrObjectL();

                CCssValue*  oldValue = iSvgStyleProperties->operator[](aNameId);
                PropagateToChildren( oldValue, tValue, aNameId );
                iSvgStyleProperties->operator[](aNameId) = tValue;
                }
            tValue->SetValueL( aValue );

            break;
            }
        default:
            {
            CIntCssValueImpl* tValue = (CIntCssValueImpl *)(iSvgStyleProperties->operator[](aNameId));
            if( tValue != NULL && tValue->Value() == aValue )
            {
                return KErrNone;
            }
            CIntCssValueImpl* tParentValue = NULL ;
            if( this != doc->RootElement() && lParentElement )
                {
                tParentValue  = (CIntCssValueImpl *)(lParentElement->iSvgStyleProperties->operator[](aNameId));
                }

            if( tParentValue == tValue || tValue == NULL) //Not Yet set
                {
                tValue = doc->iMemoryManager->GetCssIntObjectL();

                CCssValue*  oldValue = iSvgStyleProperties->operator[](aNameId);
                PropagateToChildren( oldValue, tValue, aNameId );
                iSvgStyleProperties->operator[](aNameId) = tValue;

                }
            if( lParentElement && aValue == KInherit )    
            {
            //tParentValue  = (CIntCssValueImpl *)(lParentElement->iSvgStyleProperties->operator[](aNameId));
            //(*iSvgStyleProperties)[aNameId]= tParentValue;
						if(tParentValue)
								 tValue->SetValueL(tParentValue->Value());
            }
            else
            {
            tValue->SetValueL( aValue );	
            }

            break;

            }
        }
    return KErrNoAttribute;


    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgElementImpl::GetAttributeFloat( const TInt aNameId, TFloatFixPt& aValue )
    {

    CCssValue*          lCssValue           = NULL;
    if((aNameId < KCSS_MAX_ATTR) && iSvgStyleProperties )
        {
        lCssValue = iSvgStyleProperties->operator[](aNameId);
        if( lCssValue )
            {
            aValue = (( CFloatCssValueImpl * )lCssValue)->Value();
            return KErrNone;
            }
        else
            {
            TRAPD(err, GetDefaultAttributeL(aNameId, lCssValue));
            if (err)
                {
                #ifdef _DEBUG
                RDebug::Printf("CSvgElementImpl::GetAttributeFloat: Error trapped=%d", err);
                #endif
                return KErrNoAttribute;
                }
            aValue = (( CFloatCssValueImpl * )lCssValue)->Value();
            return KErrNone;
            }
        }
    return KErrNoAttribute;

    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgElementImpl::SetAttributeFloatL( const TInt aNameId,
                                          const TFloatFixPt aValue )
    {
    CSvgDocumentImpl*   doc                 = ( CSvgDocumentImpl* ) OwnerDocument();

    if (!doc)
    {
    	return KErrNoAttribute;
    }

    if(aNameId < KCSS_MAX_ATTR  && iSvgStyleProperties)
        {
        CSvgElementImpl* lParentElement = (CSvgElementImpl*)ParentNode ();
        while(lParentElement && (lParentElement->iSvgStyleProperties == NULL))
            {
            lParentElement = (CSvgElementImpl*)lParentElement->ParentNode ();
            }


        CFloatCssValueImpl* tValue = (CFloatCssValueImpl *)(iSvgStyleProperties->operator[](aNameId));
        CFloatCssValueImpl* tParentValue = NULL ;
        if( this != doc->RootElement() && lParentElement)
            {
            tParentValue  = (CFloatCssValueImpl *)(lParentElement->iSvgStyleProperties->operator[](aNameId));

            }

        if( tParentValue == tValue || tValue == NULL) //Not Yet set
            {
           	tValue = doc->iMemoryManager->GetCssFloatObjectL();


            CCssValue*  oldValue = iSvgStyleProperties->operator[](aNameId);
            PropagateToChildren( oldValue, tValue, aNameId );

            iSvgStyleProperties->operator[](aNameId) = tValue;
            }
        return tValue->SetValueL( aValue );

        }
    return KErrNoAttribute;

    }


void CSvgElementImpl::GetSvgStylePropertiesArray( RPointerArray<CCssValue>*& aSvgStyleProperties)
    {
    aSvgStyleProperties = iSvgStyleProperties;
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgElementImpl::GetAttributePath( const TInt /* aNameId */,
                                        CGfxGeneralPath*& /* aValue */ )
    {
    return KErrNoAttribute;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgElementImpl::SetAttributePathL( const TInt /*aNameId */,
                                         CGfxGeneralPath* /* aValue */ )
    {
    return KErrNoAttribute;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgElementImpl::SetAttributePathRef( const TInt /*aNameId */,
                                         CGfxGeneralPath*& /* aValue */ )
    {
    return KErrNoAttribute;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgElementImpl::GetAttributeDes( const TInt aNameId, TPtrC16& aValue )
    {
    CCssValue*          lCssValue           = NULL;

    if(aNameId < KCSS_MAX_ATTR && iSvgStyleProperties)
        {
        lCssValue = iSvgStyleProperties->operator[](aNameId);
        if( lCssValue )
            {
            aValue.Set( (( CStrCssValueImpl * )lCssValue)->Value() );
            return KErrNone;

            }
        else
            {
                TBool defaultAttr = EFalse;
                TRAPD(err, defaultAttr = GetDefaultAttributeL(aNameId, lCssValue));
                if (err)
                    {
                    #ifdef _DEBUG
                    RDebug::Printf("CSvgElementImpl::GetAttributeDes: Error trapped=%d", err);
                    #endif
                    return KErrNoAttribute;
                    }

            	if (defaultAttr && lCssValue)
            	{
            		aValue.Set( (( CStrCssValueImpl * )lCssValue)->Value() );
            		return KErrNone;
           	 	}
            return KErrNoAttribute;
            }
        }
    // This is added for JSR API's and this is helpful for testing of the JSR API's
    if(aNameId == KAtrId)
       {
        if( Id() )
            {
        aValue.Set( *Id() );
        return KErrNone;
            }
       }
    return KErrNoAttribute;

    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgElementImpl::SetAttributeDesL( const TInt aNameId,
                                        const TDesC& aValue )
    {
    CSvgDocumentImpl*   doc                 = ( CSvgDocumentImpl* ) OwnerDocument();

	if (!doc)
	{
		return KErrNoAttribute;
	}

    if(aNameId < KCSS_MAX_ATTR && iSvgStyleProperties)
        {

        CSvgElementImpl* lParentElement = (CSvgElementImpl*)ParentNode ();
        while(lParentElement && (lParentElement->iSvgStyleProperties == NULL))
            {
            lParentElement = (CSvgElementImpl*)lParentElement->ParentNode ();
            }

        CStrCssValueImpl* tValue = (CStrCssValueImpl *)(iSvgStyleProperties->operator[](aNameId));
        CStrCssValueImpl* tParentValue = NULL ;
        if( this != doc->RootElement() && lParentElement)
            {
            tParentValue  = (CStrCssValueImpl *)(lParentElement->iSvgStyleProperties->operator[](aNameId));
            }

        if( tParentValue == tValue || tValue == NULL) //Not Yet set
            {
            tValue = doc->iMemoryManager->GetCssStrObjectL(KNullDesC);


            CCssValue*  oldValue = iSvgStyleProperties->operator[](aNameId);
            PropagateToChildren( oldValue, tValue, aNameId );

            iSvgStyleProperties->operator[](aNameId) = tValue;

            }
        return tValue->SetValueL( aValue );

        }
    // the following code is added to modify the Id. This might be needed for the JSR API's
    if(aNameId == KAtrId)
       {
        delete iId;
        iId = NULL;
        iId = aValue.AllocL();
        return KErrNone;
       }
    return KErrNoAttribute;


    }

// ---------------------------------------------------------------------------
// Internal public method (public but not exported)
// ---------------------------------------------------------------------------
TInt32 CSvgElementImpl::AnimateMotionMatrixIndexL()
    {
    if ( iAnimateMotionMatrixIndex < 0 && iSvgTransformable )
        {
        TGfxAffineTransform ident;
        MSvgTransformList*  trList;
        this->GetTransform( trList );
        trList->AppendItemL( ident, ETrue, ETrue ); // Set identity transform
        iAnimateMotionMatrixIndex = ( TInt16 ) ( trList->NumberOfItems() - 1 );// Keep index in list for this anim element
        }

    return iAnimateMotionMatrixIndex;
    }
TInt32 CSvgElementImpl::AnimateTransformMatrixIndexL()
    {
    if ( iAnimateTransformMatrixIndex < 0 && iSvgTransformable )
        {
        TGfxAffineTransform ident;
        MSvgTransformList*  trList;
        this->GetTransform( trList );
        trList->AppendItemL( ident, ETrue, 2); // Set identity transform
        iAnimateTransformMatrixIndex = trList->NumberOfItems() - 1;// Keep index in list for this anim element */
        }

    return iAnimateTransformMatrixIndex;
    }
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgElementImpl::GetBBox( TGfxRectangle2D& /*aBbox*/ )
    {
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgElementImpl::GetUnscaledBBox( TGfxRectangle2D& /*aBbox*/ )
    {
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgElementImpl::ResetReferenceElementL()
    {
    }

// ---------------------------------------------------------------------------
// perform a deep clone of this object
// ---------------------------------------------------------------------------
MXmlElement* CSvgElementImpl::CloneL(MXmlElement*)
    {
    return NULL;
    }

// ---------------------------------------------------------------------------
// copy the contents of this object to the passed object
// this method assumes that the passed object has already been
// created using the proper NewL method.
// ---------------------------------------------------------------------------
void CSvgElementImpl::CopyL( CSvgElementImpl* aDestElement )
    {

    if(aDestElement)
    {

    if( iSvgStyleProperties )
        {
        aDestElement->iSvgStyleProperties->Reset();

        CloneSvgStylePropertiesL(aDestElement,(CSvgElementImpl *)(aDestElement->ParentNode()));
        }


    // copy iSvgTransformable (CSvgTransformableImpl)
    if( iSvgTransformable )
        {
        if ( aDestElement->iSvgTransformable != NULL )
            {
            delete aDestElement->iSvgTransformable;
            aDestElement->iSvgTransformable = NULL;
            }
        aDestElement->iSvgTransformable = this->iSvgTransformable->CloneL();
        }

    // copy iSvgTests (CSvgTestImpl)
    if( iSvgTests )
        {
        if ( aDestElement->iSvgTests != NULL )
            {
            delete aDestElement->iSvgTests;
            aDestElement->iSvgTests = NULL;
            }
        aDestElement->iSvgTests = this->iSvgTests->CloneL();
        }

    // copy iSvgUriReferenceImpl (CSvgUriReferenceImpl)
    if( iSvgUriReferenceImpl )
        {
        if ( aDestElement->iSvgUriReferenceImpl != NULL )
            {
            delete aDestElement->iSvgUriReferenceImpl;
            aDestElement->iSvgUriReferenceImpl = NULL;
            }
        aDestElement->iSvgUriReferenceImpl = this->iSvgUriReferenceImpl->CloneL();
        }

    // copy iSvgLangSpace (CSvgLangSpaceImpl)
    if( iSvgLangSpace )
        {
        if ( aDestElement->iSvgLangSpace != NULL )
            {
            delete aDestElement->iSvgLangSpace;
            aDestElement->iSvgLangSpace = NULL;
            }
        aDestElement->iSvgLangSpace = this->iSvgLangSpace->CloneL();
        }
	
	// copy Id (iId)
	if( iId )
	{
		aDestElement->SetAttributeDesL(	KAtrId, *iId );
	}
	
	// copy stuff from superclass
    this->CXmlElementImpl::CopyL(aDestElement);
    }

    }

void CSvgElementImpl::PropagateToChildren( CCssValue *oldValue, CCssValue *newValue, const TInt aNameId )
    {
    CSvgElementImpl *lChild = (CSvgElementImpl *)FirstChild();

    while(lChild)
        {
        if( lChild->iSvgStyleProperties )
            {
            if( lChild->iSvgStyleProperties->operator[](aNameId) == oldValue)
                {
                lChild->iSvgStyleProperties->operator[](aNameId) = newValue;
                if( lChild->FirstChild() )
                    {
                    lChild = (CSvgElementImpl *)lChild->FirstChild();
                    continue;
                    }

                }
            }
        else
            {
            if( lChild->FirstChild() )
                {
                lChild = (CSvgElementImpl *)lChild->FirstChild();
                continue;
                }
            }


        if( lChild->NextSibling() )
            {
            lChild = (CSvgElementImpl *)lChild->NextSibling();
            }
        else
            {
            lChild = (CSvgElementImpl *)lChild->ParentNode();
            while( lChild != this && lChild->NextSibling() == NULL )
                {
                lChild = (CSvgElementImpl *)lChild->ParentNode();
                }
            if( lChild == this )
                {
                lChild = NULL ;
                }
            else
                {
                lChild = (CSvgElementImpl *)(lChild->NextSibling());
                }
            }
        }
    }

void CSvgElementImpl::CloneSvgStylePropertiesL( CSvgElementImpl* aNewChild, CSvgElementImpl* aParentElement)
    {

	CSvgDocumentImpl* doc = (CSvgDocumentImpl*)OwnerDocument();

	if (!doc)
	{
		return;
	}

    if( iSvgStyleProperties )
        {
        TInt                        count       = iSvgStyleProperties->Count();
        CSvgElementImpl* lTmpParent = (CSvgElementImpl*)ParentNode ();
        while(lTmpParent && (lTmpParent->iSvgStyleProperties == NULL))
        {
        lTmpParent = (CSvgElementImpl*)lTmpParent->ParentNode ();
        }

        RPointerArray<CCssValue>*lSvgStyleProperties = NULL;

        (  ( CSvgElementImpl * ) aNewChild  )->GetSvgStylePropertiesArray( lSvgStyleProperties );

        CSvgElementImpl* lTmpCloneParent = aParentElement;
        while(lTmpCloneParent && (lTmpCloneParent->iSvgStyleProperties == NULL))
            {
            lTmpCloneParent = (CSvgElementImpl*)lTmpCloneParent->ParentNode ();
            }

        for ( TInt i = 0; i < count; i++ )
            {
            if( lTmpParent && (lTmpParent->iSvgStyleProperties->operator[]( i ) == iSvgStyleProperties->operator[]( i )) )
                {
                CCssValue*   lCssValue = NULL;
                if( lTmpCloneParent )
                    {
                    lCssValue   = lTmpCloneParent->iSvgStyleProperties->operator[]( i );
                    }
                lSvgStyleProperties->InsertL(lCssValue,i);
                }
            else
                {

                CCssValue*   lCssValue = (iSvgStyleProperties->operator[]( i ));

                //CLONING VALUES THROUGH MEMORY MANAGER

                CCssValue* lCssValueCloned = doc->iMemoryManager->CloneCssValueL(lCssValue);

                lSvgStyleProperties->InsertL(lCssValueCloned,i);
                }
            }
            }
    }

void CSvgElementImpl::ResetReferenceAttrSet()
    {
      if( iAnimateAttrSet )
        {
        iAnimateAttrSet->Close();
        delete iAnimateAttrSet;
        iAnimateAttrSet = NULL ;
        }
    }


// functions for encoding

void  CSvgElementImpl::SetTransformList(TGfxAffineTransform& aTr)
    {
    if( iSvgTransformable )
        {
        iSvgTransformable->SetTransformList(aTr);
        }
    }


void  CSvgElementImpl::SetRequiredFeatures(  CDesCArrayFlat*& aRequiredFeatures )
    {
    if( iSvgTests )
        {
        iSvgTests->SetRequiredFeatures (aRequiredFeatures);
        }
    }

void  CSvgElementImpl::SetRequiredExtensions(  CDesCArrayFlat*& aRequiredExtensions )
    {
    if( iSvgTests )
        {
        iSvgTests->SetRequiredExtensions (aRequiredExtensions);
        }
    }

void  CSvgElementImpl::SetSystemLanguage(  CDesCArrayFlat*& aSystemLanguage )
    {
    if( iSvgTests )
        {
        iSvgTests->SetSystemLanguage (aSystemLanguage);
        }
    }

TBool CSvgElementImpl::IsTransformable()
    {
    if( iSvgTransformable )
        return ETrue;
    else
        return EFalse;
    }
TInt  CSvgElementImpl::SetUriRefDesL( const TUint16 aNameId,
                                        const TDesC& aValue )
    {
    if( iSvgUriReferenceImpl )
        {
        return iSvgUriReferenceImpl->SetUriRefDesL(aNameId, aValue);
        }
    return (TInt) EFalse;
    }
void CSvgElementImpl::Reset(MSvgEvent* /*aEvent*/)
    {

    }

void CSvgElementImpl::ReInitializeAnimation()
{

}

CGfxGeneralPath*  CSvgElementImpl::GetPathAttribute(TInt /*aAttributeId*/)
{
    return NULL;
}
void CSvgElementImpl::SetPathAttribute(TInt /*aSvgAttrId*/, CGfxGeneralPath* /*aPathHandle*/)
{

}

//this element has an animation associated with it
TBool CSvgElementImpl::HasAnimation()
{
	if (iHasAnimationBase)
	{
		return ETrue;
	}

	return EFalse;
}

void CSvgElementImpl::SetIsAnimating( TBool aBool )
{
    if ( aBool )
    {
        iBooleanFlags = iBooleanFlags | KAnimatingBit;
    }
    else
    {
        iBooleanFlags = iBooleanFlags & ~KAnimatingBit;
    }
}

TBool CSvgElementImpl::IsAnimating()
{
    return iBooleanFlags & KAnimatingBit;
}

void CSvgElementImpl::SetTurnOff( TBool aBool )
{
    if ( aBool )
    {
        iBooleanFlags = iBooleanFlags | KWasTurnedOffBit;
    }
    else
    {
        iBooleanFlags = iBooleanFlags & ~KWasTurnedOffBit;
    }
}

TBool CSvgElementImpl::WasTurnedOff()
{
    return iBooleanFlags & KWasTurnedOffBit;
}

void CSvgElementImpl::SetOverwriteTransforms( TBool aBool )
{
    if ( aBool )
    {
        iBooleanFlags = iBooleanFlags | KOverwriteTransformsBit;
    }
    else
    {
        iBooleanFlags = iBooleanFlags & ~KOverwriteTransformsBit;
    }
}

TBool CSvgElementImpl::IsOverwriteTransforms()
{
    return iBooleanFlags & KOverwriteTransformsBit;
}

void CSvgElementImpl::SetAllAttributesAdded( TBool aBool )
{
    if ( aBool )
    {
        iBooleanFlags = iBooleanFlags | KAllAttributesAddedBit;
    }
    else
    {
        iBooleanFlags = iBooleanFlags & ~KAllAttributesAddedBit;
    }
}

TBool CSvgElementImpl::AreAllAttributesAdded()
{
    return iBooleanFlags & KAllAttributesAddedBit;
}

void CSvgElementImpl::SetMouseDown( TBool aBool )
{
    if ( aBool )
    {
        iBooleanFlags = iBooleanFlags | KMouseDownBit;
    }
    else
    {
        iBooleanFlags = iBooleanFlags & ~KMouseDownBit;
    }
}

TBool CSvgElementImpl::IsMouseDown()
{
    return iBooleanFlags & KMouseDownBit;
}

void CSvgElementImpl::SetMouseIn( TBool aBool )
{
    if ( aBool )
    {
        iBooleanFlags = iBooleanFlags | KMouseInBit;
    }
    else
    {
        iBooleanFlags = iBooleanFlags & ~KMouseInBit;
    }
}

TBool CSvgElementImpl::IsMouseIn()
{
    return iBooleanFlags & KMouseInBit;
}

void CSvgElementImpl::DeactivateAnimation()
{
}

TBool  CSvgElementImpl::DoAnimProcL(MSvgEvent* /*aEvent*/)
{
return ETrue;
}
void CSvgElementImpl ::PointToParent(const TInt aAttrId)
{
	//this method doesnt look too good to me.
	//especially since the OldPointer that is getting passed
	//to PropagateToChildren is NULL

    if(aAttrId < KCSS_MAX_ATTR && iSvgStyleProperties)
        {
        CSvgElementImpl* lTmpParent = (CSvgElementImpl*)ParentNode ();
        if(lTmpParent)
            {
            if(iSvgStyleProperties->operator[]( aAttrId ) != lTmpParent->iSvgStyleProperties->operator[](aAttrId ))
                {

                //store the old value.
                CCssValue *OldPointer = iSvgStyleProperties->operator[]( aAttrId );

                //this deletion is no longer needed as the memory manager is in charge of this now

                // set the pointer
                iSvgStyleProperties->operator[]( aAttrId ) = lTmpParent->iSvgStyleProperties->operator[](aAttrId );

                // update the child pointers to the previous values.
                PropagateToChildren(OldPointer, lTmpParent->iSvgStyleProperties->operator[](aAttrId ), aAttrId );
                }
            }
        }
}
TBool CSvgElementImpl::IsInherited(const TInt aAttrId)
{
if(aAttrId < KCSS_MAX_ATTR && iSvgStyleProperties)
    {
    CSvgElementImpl* lTmpParent = (CSvgElementImpl*)ParentNode ();
    if(lTmpParent && lTmpParent->iSvgStyleProperties)
        {
        return (iSvgStyleProperties->operator[]( aAttrId ) == lTmpParent->iSvgStyleProperties->operator[](aAttrId ));
        }
    }
return EFalse;
}

//probably dont need this anymore
void CSvgElementImpl::InitSvgStylePropertiesWithNullL()
{
    for( int i=0; i < KCSS_MAX_ATTR; i++ )
    {
        User::LeaveIfError( iSvgStyleProperties->Append( NULL ) );
    }
}

TBool CSvgElementImpl::IsVisible()
{
    if ( !AreAllAttributesAdded() )
        {
        return EFalse;
        }

    TInt32 visbilityValue = -1;
    TInt lReturnValue = KErrGeneral;
    TRAPD(err, lReturnValue = this->GetAttributeIntL( KCSS_ATTR_VISIBILITY, visbilityValue ));
    if (err)
        {
        #ifdef _DEBUG
        RDebug::Printf("CSvgElementImpl::IsVisible() GetAttributeIntL error");
        #endif
        }

    return ( ( lReturnValue == KErrNone) && (visbilityValue == 0) );
}

void CSvgElementImpl::PrintStyleProperties( )
{
	//prints all of the style properties out that this element has ahold of

	#ifdef _DEBUG
	if (!iSvgStyleProperties)
	{
		RDebug::Printf("CSvgElementImpl::PrintStyleProperties[%x]: NO STYLES FOUND", this);
		return;
	}

	RDebug::Printf("----STYLES---");
	for (TInt i=0; i<iSvgStyleProperties->Count(); i++)
	{
		switch (i)
		{
			case 0:
				RDebug::Printf("fill=\"");
			break;
			case 22:
				RDebug::Printf("fill-opacity=\"");
			break;
			case 23:
				RDebug::Printf("stroke-opacity=\"");
			break;
			case 1:
				RDebug::Printf("stroke=\"");
			break;
			case 2:
				RDebug::Printf("stroke-width=\"");
			break;
			case 3:
				RDebug::Printf("visibility=\"");
			break;
			case 4:
				RDebug::Printf("font-family=\"");
			break;
			case 5:
				RDebug::Printf("font-size=\"");
			break;
			case 6:
				RDebug::Printf("font-style=\"");
			break;
			case 7:
				RDebug::Printf("font-weight=\"");
			break;
			case 8:
				RDebug::Printf("stroke-dasharray=\"");
			break;
			case 9:
				RDebug::Printf("display=\"");
			break;
			case 10:
				RDebug::Printf("fill-rule=\"");
			break;
			case 11:
				RDebug::Printf("stroke-linecap=\"");
			break;
			case 12:
				RDebug::Printf("stroke-linejoin=\"");
			break;
			case 13:
				RDebug::Printf("stroke-dashoffset=\"");
			break;
			case 14:
				RDebug::Printf("stroke-miterlimit=\"");
			break;
			case 15:
				RDebug::Printf("color=\"");
			break;
			case 16:
				RDebug::Printf("text-anchor=\"");
			break;
			case 17:
				RDebug::Printf("text-decoration=\"");
			break;
			case 18:
				RDebug::Printf("color-interpolation=\"");
			break;
			case 19:
				RDebug::Printf("color-rendering=\"");
			break;
			case 20:
				RDebug::Printf("letter-spacing=\"");
			break;
			case 21:
				RDebug::Printf("word-spacing=\"");
			break;
			case 24: //THIS SHOULD BE = KSS_MAX_ATTR
				RDebug::Printf("group-opacity=\"");
			break;
		default:
			RDebug::Printf("UNKNOWN STYLE FOUND!!");
		break;
		}

		if ( iSvgStyleProperties->operator[]( i ) )
		{
			iSvgStyleProperties->operator[]( i )->Print();
			RDebug::Printf("\"");
		}
		else
		{
			//that style was empty
			RDebug::Printf("EMPTY!!\"", i);
		}
	}
	RDebug::Printf("---END OF STYLES---");

	#endif

}

TBool CSvgElementImpl::GetDefaultAttributeL( const TInt aNameId, CCssValue*& aValue )
{
	CSvgDocumentImpl* lDocument = (CSvgDocumentImpl*)OwnerDocument();

	if (!lDocument)
	{
		return EFalse;
	}

	CSvgElementImpl* lRootElement = (CSvgElementImpl*)(lDocument->RootElement());

	if (lRootElement)
	{
		RPointerArray<CCssValue>*lSvgStyleProperties = NULL;

		lRootElement->GetSvgStylePropertiesArray(lSvgStyleProperties);

		if (lSvgStyleProperties && lSvgStyleProperties->operator[](aNameId) != NULL )
		{
			aValue = (CCssValue*)lSvgStyleProperties->operator[](aNameId);
			return ETrue;
		}
	}

	switch( aNameId )
	{
		// Initilaise Svg Element with default values
		case KCSS_ATTR_FILL:
		{
			CPaintCssValueImpl* lPaintObject = lDocument->iMemoryManager->GetCssPaintObjectL(_L("black"),(CSvgElementImpl *)this);

			iSvgStyleProperties->InsertL(lPaintObject, KCSS_ATTR_FILL );
		}
		case KCSS_ATTR_FILL_OPACITY:
		{
			CFloatCssValueImpl* lFloatObject5 = lDocument->iMemoryManager->GetCssFloatObjectL(_L("1"));

			iSvgStyleProperties->InsertL(lFloatObject5,KCSS_ATTR_FILL_OPACITY);
		}
		break;

		case KCSS_ATTR_STROKE_OPACITY:
		{
			CFloatCssValueImpl* lFloatObject6 = lDocument->iMemoryManager->GetCssFloatObjectL(_L("1"));

			iSvgStyleProperties->InsertL(lFloatObject6,KCSS_ATTR_STROKE_OPACITY);
		}
		break;

		case KCSS_ATTR_STROKE:
		{
			CClrCssValueImpl* lClrObject = lDocument->iMemoryManager->GetCssClrObjectL(_L("none"));

			iSvgStyleProperties->InsertL(lClrObject, KCSS_ATTR_STROKE);
		}
		break;

		case KCSS_ATTR_STROKEWIDTH:
		{
			CFloatCssValueImpl* lFloatObject = lDocument->iMemoryManager->GetCssFloatObjectL(_L("1"));

			iSvgStyleProperties->InsertL(lFloatObject, KCSS_ATTR_STROKEWIDTH);
		}
		break;

		case KCSS_ATTR_VISIBILITY:
		{
			CIntCssValueImpl* lIntObject = lDocument->iMemoryManager->GetCssIntObjectL(_L("0"));

			iSvgStyleProperties->InsertL(lIntObject,KCSS_ATTR_VISIBILITY);
		}
		break;

		case KCSS_ATTR_FONTFAMILY:
		{
			iSvgStyleProperties->InsertL(NULL,KCSS_ATTR_FONTFAMILY);
		}
		break;

		case KCSS_ATTR_FONTSIZE:
		{
			CFloatCssValueImpl* lFloatObject2 = lDocument->iMemoryManager->GetCssFloatObjectL(_L("10"));

			iSvgStyleProperties->InsertL(lFloatObject2 ,KCSS_ATTR_FONTSIZE);
		}
		break;

		case KCSS_ATTR_FONTSTYLE:
		{
			CIntCssValueImpl* lIntObject2 = lDocument->iMemoryManager->GetCssIntObjectL(_L("0"));

			iSvgStyleProperties->InsertL(lIntObject2,KCSS_ATTR_FONTSTYLE);
		}
		break;

		case KCSS_ATTR_FONTWEIGHT:
		{
			CIntCssValueImpl* lIntObject3 = lDocument->iMemoryManager->GetCssIntObjectL(_L("0"));

            iSvgStyleProperties->InsertL(lIntObject3,KCSS_ATTR_FONTWEIGHT);
		}
		break;

		case KCSS_ATTR_STROKE_DASHARRAY:
		{
			iSvgStyleProperties->InsertL(NULL,KCSS_ATTR_STROKE_DASHARRAY);
		}
		break;

		case KCSS_ATTR_DISPLAY:
    {
    	//set default to inline
    	CIntCssValueImpl* lIntObject4 = lDocument->iMemoryManager->GetCssIntObjectL(_L("0"));

    	iSvgStyleProperties->InsertL(lIntObject4,KCSS_ATTR_DISPLAY);
		}
    break;

		case KCSS_ATTR_FILLRULE:
		{
			CStrCssValueImpl* lStrObject1 = lDocument->iMemoryManager->GetCssStrObjectL(_L("nonzero"));

			iSvgStyleProperties->InsertL(lStrObject1,KCSS_ATTR_FILLRULE);
		}
		break;

		case KCSS_ATTR_STROKE_LINECAP:
		{
			CStrCssValueImpl* lStrObject2 = lDocument->iMemoryManager->GetCssStrObjectL(_L("butt"));

			iSvgStyleProperties->InsertL(lStrObject2,KCSS_ATTR_STROKE_LINECAP);
		}
		break;

		case KCSS_ATTR_STROKE_LINEJOIN:
		{
			CStrCssValueImpl* lStrObject3 = lDocument->iMemoryManager->GetCssStrObjectL(_L("miter"));

			iSvgStyleProperties->InsertL(lStrObject3,KCSS_ATTR_STROKE_LINEJOIN);
		}
		break;

		case KCSS_ATTR_STROKE_DASHOFFSET:
		{
			CFloatCssValueImpl* lFloatObject3 = lDocument->iMemoryManager->GetCssFloatObjectL(_L("0"));

			iSvgStyleProperties->InsertL(lFloatObject3,KCSS_ATTR_STROKE_DASHOFFSET);
		}
		break;

		case KCSS_ATTR_STROKE_MITERLIMIT:
		{
			CFloatCssValueImpl* lFloatObject4 = lDocument->iMemoryManager->GetCssFloatObjectL(_L("4"));

    	iSvgStyleProperties->InsertL(lFloatObject4,KCSS_ATTR_STROKE_MITERLIMIT);
		}
		break;

		case KCSS_ATTR_COLOR:
		{
			CClrCssValueImpl* lClrObject4 = lDocument->iMemoryManager->GetCssClrObjectL(_L("black"));

    	iSvgStyleProperties->InsertL(lClrObject4,KCSS_ATTR_COLOR);
		}
		break;

		case KCSS_ATTR_TEXTANCHOR:
		{
			CIntCssValueImpl* lIntObject5 = lDocument->iMemoryManager->GetCssIntObjectL(_L("0"));

			iSvgStyleProperties->InsertL(lIntObject5,KCSS_ATTR_TEXTANCHOR);
		}
		break;

		case KCSS_ATTR_TEXTDECORATION:
		{
			CIntCssValueImpl* lIntObject6 = lDocument->iMemoryManager->GetCssIntObjectL(_L("-1"));

    	iSvgStyleProperties->InsertL(lIntObject6,KCSS_ATTR_TEXTDECORATION);
		}
		break;

		case KCSS_ATTR_COLORINTERPOLATION:
		{
			iSvgStyleProperties->InsertL(NULL,KCSS_ATTR_COLORINTERPOLATION);
		}
		break;

		case KCSS_ATTR_COLORRENDERING:
		{
			iSvgStyleProperties->InsertL(NULL,KCSS_ATTR_COLORRENDERING);
		}
		break;

		case KCSS_ATTR_LETTERSPACING:
		{
			iSvgStyleProperties->InsertL(NULL,KCSS_ATTR_LETTERSPACING);
		}
		break;

		case KCSS_ATTR_WORDSPACING:
		{
			iSvgStyleProperties->InsertL(NULL,KCSS_ATTR_WORDSPACING);
		}
		break;

		case KCSS_ATTR_GROUP_OPACITY:
		{
			CFloatCssValueImpl* lFloatObject7 = lDocument->iMemoryManager->GetCssFloatObjectL(_L("1"));

			iSvgStyleProperties->InsertL(lFloatObject7,KCSS_ATTR_GROUP_OPACITY);
		}
		break;

		default:
			{
			//don't know the element
			return EFalse;
			}
		}

    aValue = (CCssValue*)iSvgStyleProperties->operator[](aNameId);
    return ETrue;
}

TFloatFixPt CSvgElementImpl::GetCurrentScale( )
{
    const TGfxAffineTransform& ctm = GetCTM();
	//GET THE SCALE FACTOR FROM THE CURRENT ELEMENT
	TGfxPoint2D ep( 1, 0 ), org( 2, 0 );
    ctm.Transform( &ep, & ep, 1 );
    ctm.Transform( &org, & org, 1 );
    ep.iX -= org.iX;
    ep.iY -= org.iY;

    TFloatFixPt scale = TFloatFixPt::Sqrt( ep.iX * ep.iX + ep.iY * ep.iY );
    // adjust for rounding error when rotated (and not scaled)
    if ( scale < TFloatFixPt(1.0f) && scale > TFloatFixPt(0.998f) )
        {
        scale = TFloatFixPt(1.0f);
        }

    return scale;
}


//ALL animated element types must be listed here
TBool CSvgElementImpl::IsAnimatedElement(  )
{
	return ( iElemID == KSvgAnimateElement ||
			iElemID == KSvgAnimateColorElement ||
			iElemID == KSvgAnimateMotionElement ||
			iElemID == KSvgAnimateTransformElement ||
			iElemID == KSvgSetElement ||
			//iElemID == KSvgDiscardElement ||	//discard has a begin but doesn't inherit from animationbase?
			iElemID == KSvgAudioElement ||
            iElemID == KSvgMediaAnimationElement );	 
}

TBool CSvgElementImpl::CanGenerateInternalEvent(TUint16 aSubEventmask, TSvgEvent aEvent)
    {
    TUint16 tmp=aSubEventmask;
    switch(aEvent)
        {
        case ESvgEventActivate:
        tmp= tmp & ESvgInteractiveEventActivate;
        break;
        case ESvgEventClick:
        tmp = tmp & ESvgInteractiveEventClick;
        break;
        case ESvgEventMouseover:
        tmp = tmp & ESvgInteractiveEventMouseover;
        break;
        case ESvgEventMousemove:
        tmp = tmp & ESvgInteractiveEventMousemove;
        break;
        case ESvgEventFocusin:
        tmp = tmp & ESvgInteractiveEventFocusin;
        break;
        case ESvgEventMouseout:
        tmp = tmp & ESvgInteractiveEventMouseout;
        break;
        case ESvgEventFocusout:
        tmp = tmp & ESvgInteractiveEventFocusout;
        break;
        default:
        tmp=0;
        }
    if(tmp)
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }
TBool CSvgElementImpl::IsSVGEnginePaused()
{
	CSvgEngineImpl* lEngine  = ( ( CSvgDocumentImpl* ) OwnerDocument() )->Engine();
    return ( lEngine->IsSVGEnginePaused() );

}
