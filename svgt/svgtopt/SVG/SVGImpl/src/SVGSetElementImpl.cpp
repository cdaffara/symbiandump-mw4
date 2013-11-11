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
#include "SVGSetElementImpl.h"
#include "SVGDocumentImpl.h"
#include "SVGSchemaData.h"


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgSetElementImpl* CSvgSetElementImpl::NewL(  const TUint8 aElemID,
											  CSvgDocumentImpl* aDoc )
	{
	CSvgSetElementImpl* self    = new ( ELeave ) CSvgSetElementImpl( aDoc );
	CleanupStack::PushL( self );
	self->ConstructL( aElemID );
	CleanupStack::Pop();

	return self;
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgSetElementImpl* CSvgSetElementImpl::NewLC(  const TUint8 aElemID,
											   CSvgDocumentImpl* aDoc )
	{
	CSvgSetElementImpl* self    = new ( ELeave ) CSvgSetElementImpl( aDoc );
	CleanupStack::PushL( self );
	self->ConstructL( aElemID );

	return self;
	}


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgSetElementImpl::ConstructL(  const TUint8 aElemID)
	{
	CSvgAnimationBase::ConstructL( aElemID );
	iReqAttrFlag=KSVG_ANIMATE_ELEMFLAG;
	iIsInheritedAttribute = EFalse;
	}





// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgSetElementImpl::~CSvgSetElementImpl()
    {
    }

// *******************************************************
// Private

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgSetElementImpl::CSvgSetElementImpl( CSvgDocumentImpl* aDoc ) : CSvgAnimationBase( aDoc )

    {
    }

// *******************************************************
// From MSvgEventReceiver
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgSetElementImpl::ResetReferenceElementL()
    {
    if(iTargetElement != NULL )
        {
        if( ((iTargetElement->iAnimateAttrSet == NULL) || (iTargetElement->iAnimateAttrSet->Find(iAttrId) == KErrNotFound) ) )
            {
            if (iInitDone && iAnimStatus != KAnimNotActive)
                {
				iCheckFirstAnim= ETrue;
                SetToOriginalL();
                if( !(iTargetElement->iAnimateAttrSet) )
                    {
                    iTargetElement->iAnimateAttrSet = new (ELeave) RArray<TUint32>(1);
                    }
                iTargetElement->iAnimateAttrSet->Append((TUint32)(iAttrId));
                }
            }
        }
    }

// *******************************************************
// From MSvgEventReceiver
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TBool CSvgSetElementImpl::ReceiveEventL( MSvgEvent* aEvent )
    {
    return CSvgAnimationBase::ReceiveEventProcL( aEvent, this );
    }

// From CSvgAnimationBase
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TBool CSvgSetElementImpl::AnimProcL( MSvgTimerEvent* /* aEvent */)

    {

    if((iAnimStatus != KAnimActive) || iDoFreeze)
	   {
	   if(iFill== KAnimFillFreeze && !iDoFreeze && iAnimStatus == KAnimFinished)
	   		{
	   		}
	   else
		   {
		   CSvgAnimationBase::CheckForEndTimesAndFreezeL(this );
		   return EFalse;
		   }
	   }
    // Set value
    switch ( iDataType )
        {
        case KSvgTypeOpacity:
        case KSvgTypeLength:
            iEndFloat = iToFloat;
            iTargetElement->SetAttributeFloatL( iAttrId, iToFloat );
            break;
        case KSvgTypeList:
        case KSvgTypePath:
        case KSvgTypeTransform:
            break;
        case KSvgTypeColor:
        case KSvgTypeTextAnchor:
        case KSvgTypeInteger:
        case KSvgTypeVisibility:
        case KSvgTypeDisplay:
            iEndInt = iToInt;
            iTargetElement->SetAttributeIntL( iAttrId, iToInt );
            break;

		//viewBox
        case KSvgTypeViewBox:
			{
		    if(iTargetElement->ElemID() == KSvgSvgElement)
				{
				iEndViewBox=  iToViewBox;
				((CSvgSvgElementImpl*)iTargetElement)->SetViewBoxL( iToViewBox );
				}
			}
		break;

        default:
            break;
        }
			   CSvgAnimationBase::CheckForEndTimesAndFreezeL(this );
    return ETrue; //.
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgSetElementImpl::ResetAnimationL()
    {
    }
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgSetElementImpl::SetAttributeIntL( const TInt aNameId, const TInt32 aValue)
{
	return CSvgAnimationBase::SetAttributeIntL(aNameId,aValue);
}
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgSetElementImpl::GetAttributeIntL( const TInt aNameId, TInt32& aValue )
{
	return CSvgAnimationBase::GetAttributeIntL(aNameId,aValue);
}
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgSetElementImpl::SetAttributeFloatL(const TInt aNameId, TFloatFixPt aValue )
{
	return CSvgAnimationBase::SetAttributeFloatL(aNameId,aValue);
}
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgSetElementImpl::GetAttributeFloat(const TInt aNameId, TFloatFixPt& aValue )
{
	return CSvgAnimationBase::GetAttributeFloat(aNameId,aValue);
}
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgSetElementImpl::ReInitializeAnimation()
    {

/*	if( iCurrentRepeatCount > 1 && iAccumulate == KAccumSum )
		{*/

	if ( iAttrId == 0xffff )
        return;

	CSvgAnimationBase::ReInitializeAnimation();

	if(iCheckFirstAnim)
		{
		if(iIsInheritedAttribute)
			{
			iTargetElement->PointToParent(iAttrId);
			}
		else
			{
			TRAPD(error, SetToOriginalL());
				if (error != KErrNone)
				{
					//non error condition needs to be handled
				}
			}

		}
    }
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CGfxGeneralPath* CSvgSetElementImpl::GetPathAttribute(TInt aAttributeId)
	{
	return CSvgAnimationBase::GetPathAttribute(aAttributeId);
	}
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgSetElementImpl::SetPathAttribute(TInt aAttributeId, CGfxGeneralPath* aPathHandle)
	{
	CSvgAnimationBase::SetPathAttribute( aAttributeId, aPathHandle);
	}




void	CSvgSetElementImpl::Reset(MSvgEvent* aEvent)
	{
	iIsUserSeek  = ETrue;
	// first call the animation base function.
	TSvgTimerEvent* timerEvent  = ( TSvgTimerEvent* ) aEvent;

	if((TInt32)timerEvent->Time() < iAbsoluteBeginTime)
		{
		// let it come to initial position.
		((CSvgDocumentImpl*)iOwnerDocument)->iInitialDrawFlag = ETrue;
		ReInitializeAnimation();
		return;
		}

	TRAPD(error,CSvgAnimationBase::ResetL( aEvent, this));

	if (error != KErrNone)
	{
		// Error Processing
	}

	}
void CSvgSetElementImpl::DeactivateAnimation()
	{
CSvgAnimationBase::DeactivateAnimation(this);
	}
TBool CSvgSetElementImpl::DoAnimProcL(MSvgEvent* aEvent)
	{
return this->AnimProcL((MSvgTimerEvent*)aEvent);
	}
void CSvgSetElementImpl::InitAnimationL()
	{
	if(!iTargetElement)
		{
		// if target element is not found then remove it.
		((CSvgDocumentImpl*)iOwnerDocument)->RemoveFromEventReceiverList(this);
		((CSvgDocumentImpl*)iOwnerDocument)->Engine()->UpdatePresentation(1);
		return;
		}

	if (!iHaveValues)
		{
		if (!iHaveTo && !iHaveBy)
			{
			((CSvgDocumentImpl*)iOwnerDocument)->RemoveFromEventReceiverList(this );
            ((CSvgDocumentImpl*)iOwnerDocument)->Engine()->UpdatePresentation(1);
			}
		}

	iIsInheritedAttribute = iTargetElement->IsInherited(iAttrId);

	// this is to make sure that this is taken only once.

	CSvgAnimationBase::InitAnimationL();
	SetFillValueL();
	}

void CSvgSetElementImpl::SetToOriginalL()
	{
	// this is to make sure the inherited style attributes are made point back to the parent.
	if(iIsInheritedAttribute && iAnimStatus == KAnimFinished)
		{
		// only if the animation has finished
		iTargetElement->PointToParent(iAttrId);
		}
	else
		{
		// set this to original values.
		CSvgAnimationBase::SetToOriginalL();
		}
	}

void CSvgSetElementImpl::Print( TBool aIsEncodeOn )
{
	if (!aIsEncodeOn)
	{
		#ifdef _DEBUG
		RDebug::Printf("<set to=\"hmm\"\\>",this);
		#endif
	}
}
