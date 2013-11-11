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


#include "SvgStopElementImpl.h"
#include "SVGDocumentImpl.h"
#include "SVGGradientElementImpl.h"
#include "SVGIntCssValueImpl.h"

_LIT(OFFSET, "offset");
_LIT(STOPOPACITY, "stop-opacity");
_LIT(INHERIT, "inherit");
_LIT(STOPCOLOR, "stop-color");

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgStopElementImpl* CSvgStopElementImpl::NewL(const TUint8 aElemID,
											   CSvgDocumentImpl* aDoc )
	{
    CSvgStopElementImpl *self = new (ELeave) CSvgStopElementImpl(aDoc);
    CleanupStack::PushL(self);
    self->ConstructL(aElemID);
    CleanupStack::Pop(self);
    return self;
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgStopElementImpl::ConstructL(const TUint8 aElemID)
	{
    CSvgElementImpl::InitializeL( aElemID );
     iSvgStyleProperties = new(ELeave) RPointerArray<CCssValue>(KCSS_MAX_ATTR);
     User::LeaveIfError( iSvgStyleProperties->Append( NULL ) );
     iSvgStyleProperties->Remove( 0 );


   	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgStopElementImpl::CSvgStopElementImpl( CSvgDocumentImpl* aDoc ):
	iOffset(-1),// This implies that offset value is yet to be initialized.
	iStopOpacity(1),
	iStopColor(0),

	iValidOffsetValue(ETrue)
	{
	SetOwnerDocument(aDoc);
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgStopElementImpl::GetAttributeFloat(const TInt aNameId, TFloatFixPt& aValue)
{
	switch(aNameId)
	{
	case KAtrOffset:
		aValue= iOffset;
		break;
	case KAtrStopOpacity:
		aValue = iStopOpacity;
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
TInt CSvgStopElementImpl::SetAttributeFloatL(const TInt aNameId,const TFloatFixPt aValue)
{
	switch(aNameId)
	{
	case KAtrOffset:
		iOffset= aValue;
		
		// Adjust the value of offset
		if(((((CSvgElementImpl*)ParentNode())->ElemID()) == KSvgRadialGradientElement) ||
			((((CSvgElementImpl*)ParentNode())->ElemID()) == KSvgLinearGradientElement))
		   {
		   CSvgGradientElementImpl *parent = ((CSvgGradientElementImpl *)ParentNode());
		    
		    if(parent)
		       {
		       parent->UpdateOffsetValues((CSvgStopElementImpl*)this);
		       }
		   }
		
		break;
	case KAtrStopOpacity:
		iStopOpacity= aValue;
		break;

	default:
		CSvgElementImpl::SetAttributeFloatL(aNameId,aValue);
		break;
	}

    return 0;

}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgStopElementImpl::GetAttributeIntL( const TInt aNameId, TInt32& aValue )
{
	switch(aNameId)
	{
		case KAtrStopColor:
		aValue = iStopColor;
		break;
		default:
			CSvgElementImpl::GetAttributeIntL(  aNameId, aValue );
			break;
	}
	return 0;
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgStopElementImpl::SetAttributeIntL( const TInt aNameId,
                                        const TInt32 aValue )
{
	switch(aNameId)
	{
	case KAtrStopColor:
		{
		iStopColor = (TUint32)aValue;
		}
		break;
	default:
		 CSvgElementImpl::SetAttributeIntL( aNameId,
                                         aValue );
		 break;
	}
	return 0;
}

TBool CSvgStopElementImpl::IsValidValue(const TDesC& aValue)
{
    TBool lValidValue = ETrue;
	// aValue can only contain these characters "-+.%0123456789", If it contains any thing
	// other than this it should be treated as 0.
	
	_LIT(KValidChars,"-+.%0123456789");
	TBufC<14> lValidChars(KValidChars);
		
	TInt llength = aValue.Length();
		
	for( int charpos = 0 ; charpos < llength; charpos++ )
	{
		if( lValidChars.Locate(aValue[charpos]) == KErrNotFound )
		{
			
			lValidValue = EFalse;
			break;
		}
	}
	
	return lValidValue;
}

TBool CSvgStopElementImpl::IsValidElement() const
{
	return iValidOffsetValue;
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgStopElementImpl::SetAttributeL(const TDesC &aName, const TDesC &aValue)
	{
	CSvgElementImpl::SetAttributeL(aName,aValue);
	TFloatFixPt KZero(KFloatFixZero);
    TFloatFixPt KOne( KFloatFixOne );

    if(aName == OFFSET)
		{
		iValidOffsetValue = IsValidValue(aValue);
			
		if( iValidOffsetValue  )
		   {
		   iOffset=TFloatFixPt::ConvertString(aValue); 
		        
		   if (aValue.Locate('%') != KErrNotFound)
		       {
		 	   iOffset = iOffset / TFloatFixPt(100);
		       }
		    
		    // Finally if the iOffset value is lesser than 0, it should be made to 0.
		    if( iOffset < KZero )
			   {
			   iOffset = KZero ;
			   }
			else if( iOffset > KOne )
			   {
			   iOffset = KOne ;
			   }
		   }
		 else
		   {
		   // aValue contains invalid characters hence "0" is assigned to it.
		   // Any stop element with wrong offset value should not be considered while 
		   // rendereing and the effect should be as if no stop-offset is declared.
		   iOffset = KZero;
		   }
		 
		}
    else if(aName == STOPOPACITY)
        {

        if(aValue == INHERIT)
            {
            // inherit does not make any sense since the stop-opacity is
            //specific to stop element only and a stop element can not have another stop element as
            //its child.
            iStopOpacity = KOne;
            }
        else
            {
            if( IsValidValue(aValue) )
            {
    #ifdef SVG_FLOAT_BUILD
            iStopOpacity = TFloatFixPt::ConvertString (aValue);

            if (!(iStopOpacity >= TFloatFixPt(0) && iStopOpacity <= TFloatFixPt(1)))
    #else
            if ( TFloatFixPt::ConvertString (aValue, iStopOpacity ) != KErrNone )
    #endif
                {
                iStopOpacity = KOne;
                }
            else
                {
                if (aValue.Locate('%') != KErrNotFound)
                    {
                    //if stop opacity was listed as a percentage
                    iStopOpacity = iStopOpacity / TFloatFixPt(100);
                    }

                if ( iStopOpacity < KZero )
                    iStopOpacity = KZero;
                else if ( iStopOpacity > KOne )
                    iStopOpacity = KOne;
                }
            }
                else
                {
                	iStopOpacity = KOne;
                }	
			}
		}
    else if (aName == STOPCOLOR)
		{
        TUint32 tempVal;

        TSvgColor tColor(KGfxColorNull);
		if( tColor.GetStringL( aValue,tempVal) )
			{

            }
        else
        	{
			tempVal = 0;
			}
        iStopColor=tempVal;
		}

    return 0;

}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgStopElementImpl::~CSvgStopElementImpl()
	{
	if ( iSvgStyleProperties )
		{
		iSvgStyleProperties->Close();
		delete iSvgStyleProperties;
		iSvgStyleProperties = NULL;
		}
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgStopElementImpl::SetStopColorL(TUint32 &aVal)
	{
	iStopColor= aVal;
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgStopElementImpl::Print( TBool aIsEncodeOn )
{
	if (!aIsEncodeOn)
	{
		#ifdef _DEBUG
		RDebug::Printf("<stop stop-color=\"hmmm\" stop-opacity=\"%d\" offset=\"%d\" />",
		/*iStopColor,*/ (int)iStopOpacity, (int)iOffset );
		#endif
	}
}

