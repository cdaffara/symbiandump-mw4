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


#include "SVGGradientElementImpl.h"
#include "SVGDocumentImpl.h"

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgGradientElementImpl::CSvgGradientElementImpl( CSvgDocumentImpl* aDoc )
	{
	SetOwnerDocument(aDoc);
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgGradientElementImpl::ConstructL(const TUint8 aElemID)
	{
    CSvgElementImpl::InitializeL( aElemID );

    iSvgStyleProperties = new(ELeave) RPointerArray<CCssValue>(KCSS_MAX_ATTR);
    User::LeaveIfError( iSvgStyleProperties->Append( NULL ) );
    iSvgStyleProperties->Remove( 0 );
	iSvgUriReferenceImpl = CSvgUriReferenceImpl::NewL();

    iSvgTransformable = CSvgTransformableImpl::NewL();
    iSvgStopElementArray = new( ELeave ) RPointerArray<CSvgStopElementImpl> ( 1 );

    iStopElementData = new(ELeave) RArray<TSvgStopData> (1);
    iStopElementData->Reset();
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgGradientElementImpl::~CSvgGradientElementImpl()
    {
    if(iSvgStopElementArray)
		{
        iSvgStopElementArray->Close();
        delete iSvgStopElementArray;
		iSvgStopElementArray = NULL;
		}

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

	if(iStopElementData)
		{
		iStopElementData->Close();
		delete iStopElementData;
		iStopElementData = NULL;
		}
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgGradientElementImpl::SetAttributeL( const TDesC& aName,const TDesC& aValue )
	{
	CSvgElementImpl::SetAttributeL(aName,aValue);

    if(aName==_L("gradientUnits")||aName==_L("patternUnits"))
		{
        if(aValue==_L("userSpaceOnUse"))
            iGradientPaint->iGradientUnits = KuserSpaceOnUse;
        else if(aValue==_L("objectBoundingBox"))
            iGradientPaint->iGradientUnits = KobjectBoundingBox;
		}
    else if(aName==_L("spreadMethod"))
		{
        if(aValue==_L("pad"))
			iGradientPaint->iSpreadMethod = KspreadMethodPad;
        else if(aValue==_L("reflect"))
			iGradientPaint->iSpreadMethod = KspreadMethodReflect;
        else if(aValue==_L("repeat"))
			iGradientPaint->iSpreadMethod = KspreadMethodRepeat;
		}
	return KErrNone;
	}

TInt CSvgGradientElementImpl::SetAttributeIntL( const TInt aNameId,
                                        const TInt32 aValue )
{
	if(aNameId == KAtrSpreadMethods)
	{

			iGradientPaint->iSpreadMethod = (TUint16) aValue;
	}

	else if(aNameId == KAtrGradientUnits)
		{
		iGradientPaint->iGradientUnits = (TUint16) aValue;
		}

	else
		CSvgElementImpl::SetAttributeIntL (aNameId,aValue);
	return 0;
}
TInt CSvgGradientElementImpl::GetAttributeIntL( const TInt aNameId, TInt32& aValue )
{
	if(aNameId == KAtrSpreadMethods)
	{
		aValue = iGradientPaint->iSpreadMethod ;
	}

	else if(aNameId == KAtrGradientUnits)
		{
		aValue = iGradientPaint->iGradientUnits ;
		}

	else
		CSvgElementImpl::GetAttributeIntL (aNameId,aValue);
	return 0;
}

// ---------------------------------------------------------------------------
// Adjusts the offset values of all the previously added elements such that
// each gradient offset value is greater than the previous gradient stop's
// offset value.
// ---------------------------------------------------------------------------
void CSvgGradientElementImpl::UpdateOffsetValues(CSvgStopElementImpl *aStopEl)
{
    TInt count = iSvgStopElementArray->Count();
    
    if (count == 1)
        {
    	return;
        }
    
    TFloatFixPt aNewOffset = aStopEl->iOffset;
    
    //Since the Stop Element is already added, The loop runs for "count - 1" times.
    for (TInt i=0; i < count - 1; i++)
	     {
		 TFloatFixPt OldOffset = iSvgStopElementArray->operator[](i)->iOffset;
		
		 if ( OldOffset >= aNewOffset )
		     {
		     //set this latter value up so it has at least as high a value as the
			 //previous stop offsets
			 aStopEl->iOffset = OldOffset;
			 
			 if (aStopEl->iOffset == TFloatFixPt(0))
				 {
				 aStopEl->iOffset += TFloatFixPt(.001f);
				 }
				else
				 {
				 //two stop offsets have the same value now so make the one before ever so slightly less
				 //the latter one will control the "overlap point"
				 OldOffset -= TFloatFixPt(.001f);
				 }
				
			 iSvgStopElementArray->operator[](i)->iOffset = OldOffset;
		     }
	     }
    	
}

// ---------------------------------------------------------------------------
// The function not only adds the element in Stop element array but also
// adjusts the offset values of all the previously added elements such that
// each gradient offset value is greater than the previous gradient stop's
// offset value.
// ---------------------------------------------------------------------------
void CSvgGradientElementImpl::AddStopElementInArray(CSvgStopElementImpl *aStopEl)
	{
	
	TBool lFound = EFalse;
	
	TInt err = iSvgStopElementArray->Find(aStopEl);
	
	if( err != KErrNotFound )
	   {
	   lFound = ETrue;
	   }
		
	if(!lFound)
	   {
	   iSvgStopElementArray->Append(aStopEl);
	   UpdateOffsetValues(aStopEl);
	   }
	
	}

void CSvgGradientElementImpl::SetGradientTransMatrix(SVGReal aMatrix[2][3])
	{
	TGfxAffineTransform tempTransform;
    MSvgTransformList*  trList;
    GetTransform( trList );
    TGfxAffineTransform originalTransform   = trList->GetItem( 0 );
    tempTransform.iM00.iValue = aMatrix[0][0];
    tempTransform.iM01.iValue = aMatrix[0][1];
    tempTransform.iM02.iValue = aMatrix[0][2];
    tempTransform.iM10.iValue = aMatrix[1][0];
    tempTransform.iM11.iValue = aMatrix[1][1];
	tempTransform.iM12.iValue = aMatrix[1][2];

    TUint32 lTransType = KTransformIdentity;

    TFloatFixPt KZero;
    TFloatFixPt KOne( KFloatFixOne );
    if ( tempTransform.iM02 != KZero || tempTransform.iM12 != KZero )
        lTransType |= KTransformTranslate;

    if ( tempTransform.iM00 != KZero || tempTransform.iM11 != KZero )
        lTransType |= KTransformScale;

    if ( tempTransform.iM01 != KZero || tempTransform.iM10 != KZero )
        lTransType |= KTransformShear;

    if ( (int)lTransType == KTransformScale &&
                    tempTransform.iM00 == KOne && tempTransform.iM11 == KOne )
         lTransType = KTransformIdentity;

	tempTransform.iTransType = lTransType;

    originalTransform.Concatenate( tempTransform );
    trList->ReplaceItem( originalTransform, 0 );
	}
