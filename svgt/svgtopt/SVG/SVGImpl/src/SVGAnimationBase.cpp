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

#include "SVGAnimationBase.h"
#include "SVGElementImpl.h"
#include "SVGDocumentImpl.h"
#include "SVGStringTokenizer.h"
#include "SVGPointLexer.h"
#include "SVGPathDataParser.h"
#include "SVGAnimTimingParser.h"
#include "SVGPaintCssValueImpl.h"
#include "SVGIntCssValueImpl.h"

//CONSTANTS
_LIT(KSEMICOLON, ";");

// ---------------------------------------------------------------------------
// No NewL or NewLC because this is abstract class
// ---------------------------------------------------------------------------
void CSvgAnimationBase::ConstructL(  const TUint8 aElemID )
    {
    CSvgElementImpl::InitializeL( aElemID );

    iAnimTime = CSvgAnimTimeController::NewL();
    iValuesFloat = new ( ELeave ) RArray<TFloatFixPt>( 1 );
    iValuesInt = new ( ELeave ) RArray<TInt32>( 1 );
    iValuesPath = new ( ELeave ) RPointerArray<CGfxGeneralPath>( 1 );
	// viewBox
	iValuesViewBox = new ( ELeave ) RArray<TGfxRectangle2D>( 1 );
    iEventList = new ( ELeave )CArrayFixFlat<TEventListItem>( 1 );
    iEndTimeList = new ( ELeave )CArrayFixFlat<TEventListItem>( 1 );
	/*Added to Support forward referencing*/
	iSvgUriReferenceImpl = CSvgUriReferenceImpl::NewL();
    ((CSvgDocumentImpl*)iOwnerDocument)->AddToEventReceiverListL(
                                                    this, KSvgEventMaskTimer );
    }




// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------

CSvgAnimationBase::CSvgAnimationBase( CSvgDocumentImpl* aDoc ) :
                                                                 iAnimStatus( KAnimNotActive ),
                                                                 iTargetElement( NULL ),
                                                                 iEndOffset( 0 ),
                                                                 iRepeatCount( 1 ),
                                                                 iRepeatEndValue ( 0 ) ,
                                                                 iAbsoluteBeginTime ( 0 ),
                                                                 iBeginTimeIndex( 0 ) ,
                                                              //   iEndAccessKeyCode( 0 ),
                                                                 iCurrentRepeatCount( 0 ),
                                                                 iAttrId( 0xffff ),
                                                                 iInitDone( EFalse ),
                                                                 iTargetSet( EFalse ),
                                                                 iNoFrom( ETrue ),
                                                                 iHaveTo( EFalse ),
                                                                 iHaveBy( EFalse ),
                                                                 iHaveValues (EFalse),
                                                                 iFill( KAnimFillRemove ),
                                                                 iHadBegun( EFalse ),
                                                                 iAnimRestart( KRestartAlways ),
                                                                 iAccumulate( KAccumNone ),
                                                                 iAdditive( KAdditivePass ),
                                                                 iAdditiveOrg( KAdditiveReplace ),
                                                                 iOverrideTime ( EFalse ),
                                                                 iRepeatCountSet ( EFalse ),
																 iKeyTimesPresent(EFalse),
																// iEndReferenceElemeId(NULL),
																 iIsUserSeek(EFalse),
																 iEndTimeIndex(0)


    {
    SetOwnerDocument(aDoc);
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgAnimationBase::~CSvgAnimationBase()
    {
    if ( iAnimTime )
        {
        delete iAnimTime;
        iAnimTime = NULL;
        }


	if (iRepeatId)
	{
		delete iRepeatId;
		iRepeatId = NULL;
	}

    if ( iValuesInt )
        {
        iValuesInt->Close();
        delete iValuesInt;
        iValuesInt = NULL;
        }

    if ( iValuesViewBox )
        {
        iValuesViewBox->Close();
        delete iValuesViewBox;
        iValuesViewBox = NULL;
        }

    if ( iValuesFloat )
        {
        iValuesFloat->Close();
        delete iValuesFloat;
        iValuesFloat = NULL;
        }

    if ( iOrgPath )
        {
        delete iOrgPath;
        iOrgPath = NULL;
        }

    if ( iEndPath )
        {
        delete iEndPath;
        iEndPath = NULL;
        }

    if ( iOrgFromPath )
        {
        delete iOrgFromPath;
        iOrgFromPath = NULL;
        }

    if ( iFromPath )
        {
        delete iFromPath;
        iFromPath = NULL;
        }

    if ( iToPath )
        {
        delete iToPath;
        iToPath = NULL;
        }

    if ( iValuesPath )
        {
        iValuesPath->ResetAndDestroy();
        iValuesPath->Close();
        delete iValuesPath;
        iValuesPath = NULL;
        }

	if ( iSvgUriReferenceImpl )
		{
		delete iSvgUriReferenceImpl;
		iSvgUriReferenceImpl = NULL;
		}


    if ( iEventList )
        {
		TInt lCount = iEventList->Count();
	    for(TInt i= 0; i< lCount; i++)
	    {
	    if(iEventList->operator[](i).iReferenceElemeId)
	    	{
	    	delete iEventList->operator[](i).iReferenceElemeId;
	    	iEventList->operator[](i).iReferenceElemeId = NULL;
	    	}
	 	}
        delete iEventList;
        iEventList = NULL;
        }

     if ( iEndTimeList )
        {
        TInt lCount2 = iEndTimeList->Count();
	    for(TInt i= 0; i< lCount2; i++)
	    {
	    if(iEndTimeList->operator[](i).iReferenceElemeId)
	    	{
	    	delete iEndTimeList->operator[](i).iReferenceElemeId;
	    	iEndTimeList->operator[](i).iReferenceElemeId = NULL;
	    	}
	 	}

        delete iEndTimeList;
        iEndTimeList = NULL;
        }

    //remove this from the animation list
    CSvgDocumentImpl* lOwnerDocument = (CSvgDocumentImpl*)iOwnerDocument;
    if (lOwnerDocument)
    {
    	TInt index = lOwnerDocument->iSvgAnimations.Find(this);

    	if (index != KErrNotFound)
    	{
    		lOwnerDocument->iSvgAnimations.Remove( index );
    	}
    }

    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TUint32 CSvgAnimationBase::SimpleDuration()
    {
	TUint32 lEndTime= KTimeIndefinite;
	TUint32 lDur= KTimeIndefinite;

	if(iAbsoluteBeginTime == (TInt32)KTimeIndefinite)
		{
		return KTimeIndefinite;
		}
	if ( iAnimTime->DurationTime() == 0 )
        { 
        // If the animation's dur is specified as 0, then the Simple duration
        // of this animation is 0. This is done so that the animation should not happen
        return 0;
        }

	lEndTime = iAnimTime->EndTime();
	if ( lEndTime <= iAnimTime->BeginTime() )
	   {
	    lEndTime= KTimeIndefinite;
	   } 

    if ( iRepeatCount == KRepeatCountMax )
        {
        if ( iAnimTime->RepeatDurationTime() == KTimeIndefinite )
            {
            lDur=  KTimeIndefinite;   // Neither are specified
            }
        else    // RepeatDur was specified.
            {
            lDur= iAbsoluteBeginTime + iAnimTime->RepeatDurationTime();
            }
        }

    else if( (iAnimTime->DurationTime() == KTimeIndefinite || iAnimTime->DurationTime() == KIndefiniteAnimationDuration)
     			&& ( this->ElemID() != KSvgSetElement ) )
    	{
         lDur= KTimeIndefinite;
    	 return lDur;
    	}
    else        // RepeatCount was specified.
        {
        if ( iAnimTime->RepeatDurationTime() == KTimeIndefinite )
            {
            lDur= (TUint32) (iAbsoluteBeginTime + iAnimTime->DurationTime() * iRepeatCount);
            }
        else    // Both are specified.  Return the minimum time.
            {
            if ( iAnimTime->RepeatDurationTime() < ( iAnimTime->DurationTime() * iRepeatCount ) )
                {
                lDur= iAbsoluteBeginTime + iAnimTime->RepeatDurationTime();
                }
            else
                {
                lDur= (TUint32) (iAbsoluteBeginTime + iAnimTime->DurationTime() * iRepeatCount);
                }
            }
        }

	// should return the Minimum of end time and calculated duration
	if(lDur <= lEndTime)
		return lDur;
	else
		return lEndTime;


    }
TUint32 CSvgAnimationBase::CompleteDuration()
    {
	TUint32 lDur= KTimeIndefinite;

    TUint32 lEndTime = iAnimTime->LastEndTime();
	TUint32 lBeginTime = iAnimTime->LastBeginTime();
	
	if(lBeginTime == (TInt32)KTimeIndefinite)
		{
		return KTimeIndefinite;
		}
	if ( iAnimTime->DurationTime() == 0 )
        { 
        // If the animation's dur is specified as 0, then the Simple duration
        // of this animation is 0. This is done so that the animation should not happen
        return 0;
        }

	
	if ( lEndTime <= lBeginTime)
	   {
	    // When animation's end time is <= its begin time, it means that the 
	    // animation should be treated as an indefinite animation and animation
	    // never becomes active
		lEndTime = KTimeIndefinite; 
		
	   } 

    if ( iRepeatCount == KRepeatCountMax )
        {
        if ( iAnimTime->RepeatDurationTime() == KTimeIndefinite )
            {
            lDur=  KTimeIndefinite;   // Neither are specified
            }
        else    // RepeatDur was specified.
            {
            lDur= lBeginTime + iAnimTime->RepeatDurationTime();
            }
        }

    else if( (iAnimTime->DurationTime() == KTimeIndefinite || iAnimTime->DurationTime() == KIndefiniteAnimationDuration)
     			&& ( this->ElemID() != KSvgSetElement ) )
    	{
         lDur= KTimeIndefinite;
    	 return lDur;
    	}
    else        // RepeatCount was specified.
        {
        if ( iAnimTime->RepeatDurationTime() == KTimeIndefinite )
            {
            lDur= (TUint32) (lBeginTime + iAnimTime->DurationTime() * iRepeatCount);
            }
        else    // Both are specified.  Return the minimum time.
            {
            if ( iAnimTime->RepeatDurationTime() < ( iAnimTime->DurationTime() * iRepeatCount ) )
                {
                lDur= lBeginTime + iAnimTime->RepeatDurationTime();
                }
            else
                {
                lDur= (TUint32) (lBeginTime + iAnimTime->DurationTime() * iRepeatCount);
                }
            }
        }

	// should return the Minimum of end time and calculated duration
	if(lDur <= lEndTime)
		return lDur;
	else
		return lEndTime;


    }


// From SVG DOM / MElementTimeControl
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TBool CSvgAnimationBase::BeginElementL()
    {

    SetBeginByEventL( this, ((CSvgDocumentImpl*)iOwnerDocument)->Engine()->CurrentTIme() );

    return EFalse;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TBool CSvgAnimationBase::BeginElementAtL( TFloatFixPt aOffset )
    {
    TInt32  offset  = aOffset.RawData() / KBeginElementOffsetRatio /* *1000/65536 */;
    SetBeginByEventL( this, ((CSvgDocumentImpl*)iOwnerDocument)->Engine()->CurrentTIme() + offset );

    return EFalse;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TBool CSvgAnimationBase::EndElement()
    {
    SetEndByEvent( this, ((CSvgDocumentImpl*)iOwnerDocument)->Engine()->CurrentTIme() );


    return EFalse;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TBool CSvgAnimationBase::EndElementAt( TFloatFixPt aOffset )
    {
    TInt32  offset  = aOffset.RawData() / 66 /* *1000/65536 */;

    SetEndByEvent( this, ((CSvgDocumentImpl*)iOwnerDocument)->Engine()->CurrentTIme() + offset );


    return EFalse;
    }


// *******************************************************
// SVG Implementation


// *******************************************************
// From MXmlElement


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgAnimationBase::SetAttributeL( const TDesC& aName, const TDesC& aValue )
    {

    // Before anything else set the target to parent
    if ( !iTargetSet )
        {
        iTargetElement = ( CSvgElementImpl * ) ParentNode();// default is parent element
			if (iTargetElement)
			{
        iTargetSet = ETrue;
			}
        }

    _LIT( KTmpAttrName, "attributeName" );
    _LIT( KTmpXlinkHref, "xlink:href" );
    _LIT( KTmpBegin, "begin" );
    _LIT( KTmpDur, "dur" );
    _LIT( KTmpRepeatDur, "repeatDur" );
    _LIT( KTmpEnd, "end" );
    _LIT( KTmpFrom, "from" );
    _LIT( KTmpTo, "to" );
    _LIT( KTmpBy, "by" );
    _LIT( KTmpFill, "fill" );
    _LIT( KTmpRestart, "restart" );
    _LIT( KTmpRepeatCount, "repeatCount" );
    _LIT( KTmpAccumulate, "accumulate" );
    _LIT( KTmpAdditive, "additive" );

    _LIT( KTmpValues, "values" );
    _LIT( KTmpCalcMode, "calcMode" );
    _LIT( KTmpKeyTimes, "keyTimes" );
    _LIT( KTmpKeySplines, "keySplines" );

    _LIT( KTmpIndefinite, "indefinite" );


    if ( SetIdandXmlbaseL( aName, aValue ) )
        {
        return KErrNone;
        }


     if ( aName == KTmpXlinkHref )
        {
		// STEP 1 - Get the reference element
		// If the first char is '#' then remove it
		// This is possible if coming from cXML parser and not CVG Decoder
		TInt pos = aValue.Locate( '#' );
		if ( pos != KErrNotFound && pos == 0 )
			{
			HBufC*  tBufC   = HBufC::NewLC( aValue.Length() );
			TPtr    tPtr    = tBufC->Des();
			tPtr.Copy( aValue );
			tPtr.Delete( pos, 1 );
			CSvgElementImpl *lElement = ( CSvgElementImpl * )
			((CSvgDocumentImpl*)iOwnerDocument)->GetElementById( tPtr );
			if(this->SetXlinkAttributeL( aName, tPtr ))
				{
				if(lElement)
					{
					SetTarget(lElement);
					}
				else
					{
					CleanupStack::PopAndDestroy( 1 );
					return KErrAnimateReferenceElementNotFound;
					}
				}
			CleanupStack::PopAndDestroy( 1 );
			}

        if ( !iTargetElement )
            {
            iAttrId = 0xffff;
            return KErrNoAttribute;
            }
       else if((    iTargetElement->ElemID() == KSvgRadialGradientElement
                    || iTargetElement->ElemID() == KSvgLinearGradientElement)
                     && (iAttrId == KAtrGradientTransform))
            {
            iAttrId = KAtrTransform;
            }
        GetAttrValueL();
        return KErrNone;
        }

    if ( aName == KTmpAttrName )
        {
        iReqAttrFlag = 0;

        iAnimStatus = KAnimNotActive;

        if (iTargetElement)
            {
          	iTargetElement->iHasAnimationBase = (TInt)this;
        	iTargetElement->SetIsAnimating( EFalse );
        	SetIsAnimating( EFalse );
            }

        if ( !IsElementAttrs( aValue ) && !IsPresentationAttrs( aValue ) )
            {
            iAttrId = 0xffff; // Not supported attribute.
            }
        else
            {
            if(!iTargetElement)
            {
             iAttrId = 0xffff;
             return KErrNoAttribute;
            }
            GetAttrValueL();
            }
        }
    else if ( aName == KTmpBegin )
        {
		iIsBeginSet=  ETrue;
        if ( aValue == KTmpIndefinite )
            {
            iAnimTime->SetBeginTime( KTimeIndefinite );
            iAbsoluteBeginTime = KTimeIndefinite;
            }

        else
            {
            TStringTokenizer    tkn ( aValue, KSEMICOLON );
				    while ( tkn.HasMoreTokens() )
				        {
				        TPtrC   lToken  = tkn.NextToken();
				        CSvgAnimTimingParser*   atParser    = CSvgAnimTimingParser::NewLC( lToken, this );
				        TInt32                  clockValue;
				        HBufC* lBeginSyncElementId = HBufC::NewL(lToken.Length());
				        TSvgEvent  lBeginReferenceEvent;
				        TReal32    lRepeatBeginValue;
				        TPtr ptr = lBeginSyncElementId->Des();
				        atParser->Parse( ptr,
				                     lBeginReferenceEvent,
				                     clockValue,
				                     lRepeatBeginValue,
				                     ETrue);
				
				        TEventListItem tmp;
				        if ( lBeginSyncElementId->Length() <= 0 )
				            {
				            tmp.iTargetElement = iTargetElement;
				            tmp.iReferenceElemeId= NULL;
				            }
				         else
				         	{
				        	// get element by id and append its pointer.
				
				        	// there can be an issue if the id is not found. in  case of forward reference.
				        	// for this we will store the id if it is not found at this time.
				        	tmp.iTargetElement = ( CSvgElementImpl * )
				             ((CSvgDocumentImpl*)iOwnerDocument)->GetElementById( *lBeginSyncElementId );
				
				             if( tmp.iTargetElement == NULL)
				             	{
				             	// this indicates that this is a forward reference in case of begin.
				
				             	tmp.iReferenceElemeId = lBeginSyncElementId->AllocL();
				             	}
				             else
				             	{
				             	// otherwise the element is found and we
				             	// do not need to keep iBeginReferenceElemeId so
				             	// make it NULL;
				             	tmp.iReferenceElemeId = NULL;
				             	}
				          	}
				        if ( clockValue < 0 ) // Himanshu:to check the negative begin value
				            {
				                iNegativeBeginTime = clockValue;
				                
				                // adjusting the duration time if already set before
				                TInt32 aNewDurationTime = iAnimTime->GetDurationTime();
				                if ( aNewDurationTime != KTimeIndefinite ) 
				                    {
				                      aNewDurationTime += clockValue;
				                      iAnimTime->SetDurationTime(aNewDurationTime); 
				                    }
				                // adjusting the end time if already set before    
				                TInt32 aNewEndTime = iAnimTime->GetEndTime();   
				                if ( aNewEndTime > 0 && aNewEndTime < KTimeIndefinite )  
				                    {
				                      aNewEndTime += clockValue;
				                      iAnimTime->SetEndTime(aNewEndTime);
				                    }
				                    
				                clockValue = 0; // set clockValue to 0 if negative begin
				            }
				
				        tmp.iEvent = lBeginReferenceEvent;
				        tmp.iTime  = clockValue;
				
				        switch ( lBeginReferenceEvent )
				            {
				            case ESvgEventKey:
					            {
					            iAnimTime->SetBeginTime( KTimeIndefinite );
					            iAbsoluteBeginTime = KTimeIndefinite;
					            tmp.iAccessKeyCode = atParser->AccekeyValue();
					            tmp.iOffset =  clockValue;
					            tmp.iTargetElement = NULL;
					            ((CSvgDocumentImpl*)iOwnerDocument)->AddToEventReceiverListL( this,
					                                                KSvgEventMaskExternalUI );
					            }
				            break;
				            case ESvgEventWallClock:
				                {
				            // not supported yet
				                }
				            break;
				
				            case ESvgEventNone:
				                 {
					            // Simple Offset-value
						            iAnimTime->AddBeginTime( clockValue );
												iAbsoluteBeginTime = clockValue;
												tmp.iTargetElement = NULL;
				                }
				            break;
				
				            case ESvgEventRepeatEvent:
				                {
				                iAnimTime->SetBeginTime( KTimeIndefinite );
				                iAbsoluteBeginTime = KTimeIndefinite;
				
				                tmp.iOffset =  clockValue;
				                tmp.iRepeatValue = lRepeatBeginValue;
				                }
				            break;
				            default:
					            {
				                iAnimTime->SetBeginTime( KTimeIndefinite );
				                iAbsoluteBeginTime = KTimeIndefinite;
				                tmp.iTime = KTimeIndefinite;
				                tmp.iOffset =  clockValue;
				
				                ((CSvgDocumentImpl*)iOwnerDocument)->AddToEventReceiverListL( this,
				                                                KSvgEventMaskInternal );
					            }
				            break;
				            }
								delete lBeginSyncElementId;
				        iEventList->AppendL(tmp);
				        CleanupStack::PopAndDestroy( 1 );     // atParser
				        }
				        iAnimTime->SaveBeginTime();
            }

            if ( ( ((CSvgDocumentImpl*)iOwnerDocument)->Engine() && ((CSvgDocumentImpl*)iOwnerDocument)->Engine()->CurrentTIme() >= iAnimTime->BeginTime() )
             || ( ((CSvgDocumentImpl*)iOwnerDocument)->iTimeForJSR226 >= iAnimTime->BeginTime() ) )
				{
                if (iTargetElement)
                    iTargetElement->SetIsAnimating( ETrue );
                
                SetIsAnimating( ETrue );
				}

        }
    else if ( aName == KTmpDur )
        {
        if ( aValue == KTmpIndefinite )
            {
            iAnimTime->SetDurationTime( KTimeIndefinite );
            }
        else
            {
            CSvgAnimTimingParser*   atParser    = CSvgAnimTimingParser::NewLC( aValue, this );
            TInt32                  clockValue;
            TLex                    lex         ( aValue );
            atParser->ParseClockValue( lex, clockValue );
            CleanupStack::PopAndDestroy( 1 );     // atParser

            if (clockValue >= 0)
                {
                  if ( iNegativeBeginTime < 0 ) //Himanshu: adjusting the duration if negative begin time is given
                    {
                        clockValue += iNegativeBeginTime; 
                    }
                iAnimTime->SetDurationTime( clockValue );
                }
            else
                {
                iAnimTime->SetDurationTime( KTimeIndefinite );

                if ( !iAnimTime->BeginTime() )
                    {
                    iAnimTime->SetBeginTime( KTimeIndefinite );
                    }
                }
            }
        }
    else if ( aName == KTmpRepeatDur )
        {
        if ( aValue == KTmpIndefinite )
            {
            iAnimTime->SetRepeatDurationTime( KTimeIndefinite );
            // Set Repeat Count to max if not set
            if ( ! iRepeatCountSet )
                {
                iRepeatCount = KRepeatCountMax;
                }            
            }
        else
            {
            CSvgAnimTimingParser*   atParser    = CSvgAnimTimingParser::NewLC( aValue, this );
            TInt32                  clockValue;
            TLex                    lex         ( aValue );
            atParser->ParseClockValue( lex, clockValue );
            CleanupStack::PopAndDestroy( 1 );     // atParser
            // Set the repeat duration time only if the clock value 
            // is valid
            if( clockValue != KTimeIndefinite )
                {
                iAnimTime->SetRepeatDurationTime( clockValue );
                    
                // Set Repeat Count to max if not set
                if ( ! iRepeatCountSet )
                    {
                    iRepeatCount = KRepeatCountMax;
                    }  
                }
            }
        }
    else if ( aName == KTmpEnd )
        {
        if ( aValue == KTmpIndefinite )
            {
            iAnimTime->SetEndTime( KTimeIndefinite );
            }
        else
            {
            TStringTokenizer    tkn ( aValue, KSEMICOLON );
            while ( tkn.HasMoreTokens() )
            {
            TPtrC   lToken  = tkn.NextToken();
            CSvgAnimTimingParser*   atParser    = CSvgAnimTimingParser::NewLC( lToken, this );
            TInt32                  clockValue;


			TBuf<28>   lEndSyncElementId;

            TSvgEvent  lEndReferenceEvent;
            TReal32    lRepeatEndValue;
            atParser->Parse( lEndSyncElementId,
                             lEndReferenceEvent,
                             clockValue,
                             lRepeatEndValue,
                             EFalse);
                TEventListItem tmp;

             if ( iNegativeBeginTime < 0 ) //Himanshu: adjusting the end time if negative begin is given
                {
                   clockValue += iNegativeBeginTime; 
                }  
              if ( lEndSyncElementId.Length() <= 0 )
                {
               tmp.iTargetElement = iTargetElement;
               tmp.iReferenceElemeId= NULL;
			    }
            else
            	{
            	// get element by id and append its pointer.

                	// there can be an issue if the id is not found. in  case of forward reference.
                	// for this we will store the id if it is not found at this time.
                	tmp.iTargetElement = ( CSvgElementImpl * )
                     ((CSvgDocumentImpl*)iOwnerDocument)->GetElementById( lEndSyncElementId );

                     if( tmp.iTargetElement == NULL)
                     	{
                     	// this indicates that this is a forward reference in case of begin.

                     	tmp.iReferenceElemeId = lEndSyncElementId.AllocL();
                     	}
                     else
                     	{
                     	// otherwise the element is found and we
                     	// do not need to keep iReferenceElemeId so
                     	// make it NULL;
                     	tmp.iReferenceElemeId = NULL;
                     	}
            	}
            	 tmp.iEvent = lEndReferenceEvent;
                tmp.iTime  = clockValue;
				tmp.iRepeatValue = lRepeatEndValue;


            switch ( lEndReferenceEvent )
                {
                case ESvgEventKey:
                //iAnimTime->SetEndTime( KTimeIndefinite );  // same as indefinite
                 tmp.iAccessKeyCode = atParser->AccekeyValue();
                 tmp.iOffset =  clockValue;
                 tmp.iTargetElement = NULL;

                /*iEndAccessKeyCode = atParser->AccekeyValue(); */

                ((CSvgDocumentImpl*)iOwnerDocument)->AddToEventReceiverListL( this,
                                                        KSvgEventMaskExternalUI );
                break;

                case ESvgEventWallClock:
                // not supported yet
                break;

                case ESvgEventNone:
                // Simple Offset-value
                iAnimTime->AddEndTime( clockValue );
                tmp.iTargetElement = NULL;
                break;

                default:
                // <id>.<event> +/- <offeset>
                //iAnimTime->SetEndTime( KTimeIndefinite );  // same as indifinite
               /* iEndOffset = clockValue;*/

				 tmp.iTime = KTimeIndefinite;
                 tmp.iOffset =  clockValue;

                ((CSvgDocumentImpl*)iOwnerDocument)->AddToEventReceiverListL( this,
                                                        KSvgEventMaskInternal );
                }
			iEndTimeList->AppendL(tmp);
            CleanupStack::PopAndDestroy( 1 );     // atParser
            }
            iAnimTime->SaveEndTime();
            }

            if ( ( ((CSvgDocumentImpl*)iOwnerDocument)->Engine() && iAnimTime && ((CSvgDocumentImpl*)iOwnerDocument)->Engine()->CurrentTIme() > iAnimTime->EndTime() )
             || ( iAnimTime && ((CSvgDocumentImpl*)iOwnerDocument)->iTimeForJSR226 > iAnimTime->EndTime() ) )
				{
        	    iTargetElement->SetIsAnimating( EFalse );
            	SetIsAnimating( EFalse );
				}
        }
    else if ( aName == KTmpFrom )
        {
        iNoFrom = EFalse;
        // parser needed
        switch ( iDataType )
            {
            case KSvgTypeOpacity:
                {

                TBool lPercentage = EFalse;
                if (aValue.Locate('%') != KErrNotFound)
		 		{
		 			lPercentage = ETrue;
		 		}

                TLex    lString ( aValue );
                TReal32  lVal;
                if (lString.Val( lVal, '.' ) != KErrNone)
                    {
                    lVal=1;
                    }

                if (lPercentage)
                {
                	lVal = lVal / TReal32(100);
                }

                if(lVal>1)
                    {
                    lVal=1;
                    }
                if(lVal<0)
                    {
                    lVal=0;
                    }
                iOrgFromFloat = iFromFloat = lVal;
                }
            break;
            case KSvgTypeGradientUnits:
                    {
                     if(aValue == _L("objectBoundingBox"))
                      {
                      iOrgFromInt = iFromInt =  1;
                      }
                     else if(aValue == _L("userSpaceOnUse"))
                      {
                     iOrgFromInt = iFromInt =  0;
                      }
                    }
            break;
            case KSvgTypeLength:
                {
                TLex    string  ( aValue );
                TReal32 val;
                if( string.Val( val, '.' ) != KErrNone)
                    {
                    val = 0;
                    }
                iOrgFromFloat = iFromFloat = val;
                }
            break;
            case KSvgTypeList:
            case KSvgTypePath:
                {
                if ( iOrgFromPath )
                    {
                    delete iOrgFromPath;
                    iOrgFromPath = NULL;
                    }
                iOrgFromPath = CGfxGeneralPath::NewL();

                // Destroy iFromPath first, because it was built before during init
                if ( iFromPath )
                    {
                    delete iFromPath;
                    iFromPath = NULL;
                    }
                iFromPath = CGfxGeneralPath::NewL();

                if ( iAttrId == KAtrPoints ) //.
                    {
                    TSvgPathDataParser::ParsePointDataL( aValue, iOrgFromPath );
                    TSvgPathDataParser::ParsePointDataL( aValue, iFromPath );
                    }
                else
                    {
                    TSvgPathDataParser::ParsePathData( aValue, iOrgFromPath );
                    TSvgPathDataParser::ParsePathData( aValue, iFromPath );
                    }
                }
            break;
            case KSvgTypeSpreadMethod:
                {
                TInt pos = ((CSvgDocumentImpl*)iOwnerDocument)->SchemaData()->FindSpreadMethod(aValue);
                if(pos != KErrNotFound)
                    {
                    iOrgFromInt = iFromInt = pos;
                    }
                else
                    {
                    iOrgFromInt = iFromInt =  0;// default pad
                    }
                }
                break;
            case KSvgTypeColor:
            	{
            	TLex lex ( aValue );
                TInt32  colorValue;
                if ( lex.Val( colorValue ) != KErrNone )
	            	{
            		// means we have a URL color definition
	            	iFromPaint = ((CSvgDocumentImpl*)iOwnerDocument)->iMemoryManager->GetCssPaintObjectL(aValue, ((CSvgElementImpl*)((CSvgDocumentImpl*)iOwnerDocument)->RootElement()));

	            	if (iFromPaint)
	            		{
	            		MGfxPaint* lVal = iFromPaint->Value();
	            		if (lVal)
	            			{
	            			iOrgFromInt = iFromInt = lVal->GetColor();
	            			}
	            		}
	            	break;
            		}
            	}
            	// fall through for other color values.
            case KSvgTypeInteger:
                {
                TLex    lString ( aValue );
                TInt32  lVal;
                if (lString.Val( lVal ) != KErrNone)
                    {
                    lVal=0;
                    }
                iOrgFromInt = iFromInt = lVal;
                }
            break;
            case KSvgTypeDisplay:
                {
                if ( aValue == _L( "inline" ) )
                    {
                    iOrgFromInt = iFromInt = 0;
                    }
                else if ( aValue == _L( "none" ) )
                    {
                    iOrgFromInt = iFromInt = 16;
                    }
                else
                	{
                		//assume inherit on everything else
                	iOrgFromInt = iFromInt = (TInt32) KEnumInherit;
                	}
                }
            break;

            case KSvgTypeTextAnchor:
					{
					TInt pos;
					pos = ((CSvgDocumentImpl*)iOwnerDocument)->SchemaData()->FindTextAnchorEnum(aValue);
					if(pos!= -1)
						{
						iOrgFromInt = iFromInt = pos ;
						}
					}
				break;

            case KSvgTypeVisibility:
                {
                if ( aValue == _L( "visible" ) )
                    {
                    iOrgFromInt = iFromInt = 0;
                    }
                else if ( aValue == _L( "hidden" ) )
                    {
                    iOrgFromInt = iFromInt = 1;
                    }
                }
                break;

           // viewBox
			case KSvgTypeViewBox:
                {
                ParseViewBoxValue(aValue, iOrgFromViewBox);
				iFromViewBox = iOrgFromViewBox;
				}
                break;
            }
        }
    else if ( aName == KTmpTo )
        {
        iHaveTo = ETrue;
        // parser needed
        switch ( iDataType )
            {
            case KSvgTypeOpacity:
                {
                	TBool lPercentage = EFalse;
                	if (aValue.Locate('%') != KErrNotFound)
		 			{
		 				lPercentage = ETrue;
		 			}

                    TLex    lString ( aValue );
                    TReal32  lVal;
                    if (lString.Val( lVal, '.' ) != KErrNone)
                        {
                        lVal=1;
                        }

                    if (lPercentage)
                	{
                		lVal = lVal / TReal32(100);
                	}

                    if(lVal>1)
                        {
                        lVal=1;
                        }
                    if(lVal<0)
                        {
                        lVal=0;
                        }
                    iToFloat = lVal;
                    break;
                }
            case KSvgTypeGradientUnits:
                    {
                     if(aValue == _L("objectBoundingBox"))
                      {
                      iToInt= 1;
                      }
                     else if(aValue == _L("userSpaceOnUse"))
                      {
                      iToInt = 0;
                      }
                    }
            break;

            case KSvgTypeTextAnchor:
			                	{
								TInt pos;
								pos = ((CSvgDocumentImpl*)iOwnerDocument)->SchemaData()->FindTextAnchorEnum(aValue);
								if(pos!= -1)
									{
									iToInt =  pos ;
									}
								}
				break;

            case KSvgTypeLength:
                {
                TLex    string  ( aValue );
                TReal32 val;
                if( string.Val( val, '.' ) != KErrNone )
                    {
                    val = 0;
                    }
                iToFloat = val;
                }
            break;
            case KSvgTypeList:
            case KSvgTypePath:
                {
                CGfxPathIterator*   itr1;
                CGfxPathIterator*   itr2;
                TGfxAffineTransform idenTransform;

                if ( iToPath )
                    {
                    delete iToPath;
                    iToPath = NULL;
                    }
                iToPath = CGfxGeneralPath::NewL();

                // Build a new path

                if ( iAttrId == KAtrPoints )
                    {
                    TSvgPathDataParser::ParsePointDataL( aValue, iToPath );
                    }
                else
                    {
                    TSvgPathDataParser::ParsePathData( aValue, iToPath );
                    }

                iToPath->GetPathIteratorL( &idenTransform, itr1 );
                CleanupStack::PushL( itr1 );

                if ( iNoFrom )
                    {
                    iOrgPath->GetPathIteratorL( &idenTransform, itr2 );
                    }
                else
                    {
                    iFromPath->GetPathIteratorL( &idenTransform, itr2 );
                    }
                CleanupStack::PushL( itr2 );


                TFloatFixPt  tmpcoords1[6];
                TFloatFixPt  tmpcoords2[6];
                while ( !itr1->IsDone() && !itr2->IsDone() )
                    {
                    TGfxSegType type1   = itr1->CurrentSegment( tmpcoords1 );
                    TGfxSegType type2   = itr2->CurrentSegment( tmpcoords2 );
                    if ( type1 != type2 )
                        {
                        // MISMATCH
                        // SHOULD END ANIMATION
                        break;
                        }

                    itr1->NextL();
                    itr2->NextL();
                    }
                }
            CleanupStack::PopAndDestroy( 2 ); // destroy itr1 & itr2
            break;
            case KSvgTypeSpreadMethod:
                {
                TInt pos = ((CSvgDocumentImpl*)iOwnerDocument)->SchemaData()->FindSpreadMethod(aValue);
                if(pos != KErrNotFound)
                    {
                    iToInt = pos;
                    }
                else
                    {
                    iToInt= 0;// default pad
                    }
                }
            break;

            case KSvgTypeColor:
            	{
            	TLex lex ( aValue );
                TInt32  colorValue;
                if ( lex.Val( colorValue ) != KErrNone )
	            	{
            		// means we have a URL color definition
            		iToPaint = ((CSvgDocumentImpl*)iOwnerDocument)->iMemoryManager->GetCssPaintObjectL(aValue, ((CSvgElementImpl*)((CSvgDocumentImpl*)iOwnerDocument)->RootElement()));

            		if (iToPaint)
	            		{
	            			MGfxPaint* lVal = iToPaint->Value();
	            			if (lVal)
		            			{
		            			iToInt = lVal->GetColor();
		            			}
	            		}
	            	break;
            		}
            	}
				// fall through for rgb values
            case KSvgTypeInteger:
                {
                TLex    lString ( aValue );
                TInt32  lVal;
                if (lString.Val( lVal ) != KErrNone)
                    {
                    lVal=0;
                    }
                iToInt = lVal;
                }
            break;
            case KSvgTypeDisplay:
                {
                if ( aValue == _L( "inline" ) )
                    {
                    iToInt = 0;
                    }
                else if ( aValue == _L( "none" ) )
                    {
                    iToInt = 16;
                    }
                else
                	{
                	//assume inherit on everything else
                	iToInt = (TInt32) KEnumInherit;
                	}
                }
            break;
            case KSvgTypeVisibility:
                {
                if ( aValue == _L( "visible" ) )
                    {
                    iToInt = 0;
                    }
                else if ( aValue == _L( "hidden" ) )
                    {
                    iToInt = 1;
                    }
                }
            break;

			// viewBox
			case KSvgTypeViewBox:
                {
                ParseViewBoxValue(aValue, iToViewBox);
				}
                break;

            default:
            break;
            }
        }
    else if ( aName == KTmpBy )
        {
        iHaveBy = ETrue;

        if(!iHaveTo)
            {
            switch ( iDataType )
                {
                case KSvgTypeOpacity:
                    {

                    TBool lPercentage = EFalse;
                	if (aValue.Locate('%') != KErrNotFound)
		 			{
		 				lPercentage = ETrue;
		 			}

                    TLex    lString ( aValue );
                    TReal32  lVal;
                    if (lString.Val( lVal, '.' ) != KErrNone)
                        {
                        lVal=1;
                        }

                    if (lPercentage)
                	{
                		lVal = lVal / TReal32(100);
                	}

                    if(lVal>1)
                        {
                        lVal=1;
                        }
                    if(lVal<0)
                        {
                        lVal=0;
                        }
                    if ( iNoFrom )
                        {
                        iAdditive = KAdditiveSum;
                        iAdditiveOrg = KAdditiveSum;
                        iToFloat = iFromFloat + TFloatFixPt( lVal );
                        iNoFrom = EFalse;
                        iOrgFromFloat = iFromFloat = (TReal32)0;
                        }
                    else
                        {
                        iToFloat = iFromFloat + TFloatFixPt( lVal );
                        }
                    break;
                    }
                case KSvgTypeLength:
                    {
                    TLex    string  ( aValue );
                    TReal32 val;

                    if( string.Val( val, '.' ) != KErrNone )
                        {
                        val = 0;
                        }

                    if ( iNoFrom )
                        {
                        iAdditive = KAdditiveSum;
                        iAdditiveOrg = KAdditiveSum;
                        iToFloat = iFromFloat + TFloatFixPt( val );
                        iNoFrom = EFalse;
                        iOrgFromFloat = iFromFloat = (TReal32)0;
                        }
                    else
                        {
                        iToFloat = iFromFloat + TFloatFixPt( val );
                        }
                    }
                break;
                case KSvgTypeList:
                case KSvgTypePath:
                    {
                    if ( iToPath )
                        {
                        delete iToPath;
                        iToPath = NULL;
                        }
                    iToPath = CGfxGeneralPath::NewL();

                    if ( iAttrId == KAtrPoints ) //.
                        {
                        TSvgPathDataParser::ParsePointDataL( aValue, iToPath );
                        }
                    else
                        {
                        TSvgPathDataParser::ParsePathData( aValue, iToPath );
                        }
                    CGfxGeneralPath*lCurPath    = CGfxGeneralPath::NewLC();
                    RArray<TFloatFixPt>* fromPoints;
                    RArray<TFloatFixPt>* toPoints    = iToPath->PointCoordsArrayAll();

                    if ( iNoFrom )
                        {
                        fromPoints = iOrgPath->PointCoordsArrayAll();
                        lCurPath->SetPointTypeArrayL( iOrgPath->PointTypeArray() );
                        }
                    else
                        {
                        fromPoints = iFromPath->PointCoordsArrayAll();
                        lCurPath->SetPointTypeArrayL( iFromPath->PointTypeArray() );
                        }
                    TInt    i       = 0;
                    TInt    fromCount   = fromPoints->Count();
                    TInt    toCount   = toPoints->Count();

                    while ( i < fromCount &&  i < toCount)
                        {
                        toPoints->operator[]( i ) = fromPoints->operator[]( i ) +
                        toPoints->operator[]( i );
                        i++;
                        }
                    CleanupStack::PopAndDestroy( 1 ); // lCurPath
                    return ETrue;
                    }
                case KSvgTypeColor:
                    {
                    TLex    lString ( aValue );
                    TInt32  lVal;
                    if (lString.Val( lVal ) != KErrNone)
                        {
                        lVal=0;
                        }

                    if ( iNoFrom )
                        {
                        iToInt = lVal;
                        iToInt = AddColor( iPropInt, lVal);
                        iOrgFromInt = iFromInt = (TInt32) 0;
                        }
                    else
                        {
                        iToInt = AddColor( iFromInt, lVal);
                        }
                    }
                break;
                case KSvgTypeSpreadMethod:
                    {
                    TInt pos = ((CSvgDocumentImpl*)iOwnerDocument)->SchemaData()->FindSpreadMethod(aValue);
                    if(pos != KErrNotFound)
                        {
                        iToInt = pos;
                        }
                    else
                        {
                        iToInt= 0;// default pad
                        }

                    if ( iNoFrom )
                        {
                        iAdditive = KAdditiveSum;
                        iAdditiveOrg = KAdditiveSum;
                        iNoFrom = EFalse;
                        iOrgFromInt = iFromInt = (TInt32) 0;
                        }
                    // by here has no meaning just take the absolute value
                    }
                break;
                case KSvgTypeGradientUnits:
                    {
                     if(aValue == _L("objectBoundingBox"))
                      {
                      iToInt= 1;
                      }
                     else if(aValue == _L("userSpaceOnUse"))
                      {
                      iToInt = 0;
                      }
                      if ( iNoFrom )
                        {
                        iAdditive = KAdditiveSum;
                        iAdditiveOrg = KAdditiveSum;
                        iNoFrom = EFalse;
                        iOrgFromInt = iFromInt = (TInt32) 0;
                        }
                    // by here has no meaning just take the absolute value
                    }
            break;
                case KSvgTypeInteger:
                    {
                    TLex    lString ( aValue );
                    TInt32  lVal;
                    if (lString.Val( lVal ) != KErrNone)
                        {
                        lVal=0;
                        }

                    if ( iNoFrom )
                        {
                        iAdditive = KAdditiveSum;
                        iAdditiveOrg = KAdditiveSum;
                        iToInt = lVal;
                        iNoFrom = EFalse;
                        iOrgFromInt = iFromInt = (TInt32) 0;
                        }
                    else
                        {
                        iToInt = iFromInt + lVal;
                        }
                    }
                break;
                case KSvgTypeVisibility:
                    {
                    if ( aValue == _L( "visible" ) )
                        {
                        iToInt = 0;
                        }
                    else if ( aValue == _L( "hidden" ) )
                        {
                        iToInt = 1;
                        }
                    }
                break;
                case KSvgTypeTextAnchor:
                	{
					TInt pos;
					pos = ((CSvgDocumentImpl*)iOwnerDocument)->SchemaData()->FindTextAnchorEnum(aValue);
					if(pos!= -1)
						{
						iToInt = pos ;
						}
					}
				break;
                case KSvgTypeDisplay:
                    {
                    if ( aValue == _L( "inline" ) )
                        {
                        iToInt = 0;
                        }
                    else if ( aValue == _L( "none" ) )
                        {
                        iToInt = 16;
                        }
                    else
                    	{
                    	//assume inherit on everything else
                    	iToInt = (TInt32) KEnumInherit;
                    	}

                    }
                break;

					// viewBox
				case KSvgTypeViewBox:
			        {

                    TGfxRectangle2D lVal;
				    ParseViewBoxValue(aValue, lVal);

					TReal32 lX=0;
					TReal32 lY=0;
					TReal32 lW=0;
					TReal32 lH=0;
					lX = lVal.iX + iFromViewBox.iX;
					lY = lVal.iY + iFromViewBox.iY;
					lW = lVal.iWidth + iFromViewBox.iWidth;
					lH = lVal.iHeight + iFromViewBox.iHeight;

					TGfxRectangle2D result = TGfxRectangle2D( lX, lY, lW, lH );

					if ( iNoFrom )
                        {
                        iAdditive = KAdditiveSum;
                        iAdditiveOrg = KAdditiveSum;
                        iToViewBox = result;
                        iNoFrom = EFalse;
                        iOrgFromViewBox = iFromViewBox = TGfxRectangle2D( 0, 0, 0, 0 );
                        }
                    else
                        {
                        iToViewBox = result;
                        }
					}
				break;

                default:
                break;
                }
            }
        }
    // repeatCount : numeric value | "indefinite" 
    // Specifies the number of iterations of the animation function. It can have the following attribute
    // values: 
    // * numeric value 
    //      This is a (base 10) "floating point" numeric value that specifies the number of iterations. 
    //      It can include partial iterations expressed as fraction values. A fractional value describes a
    //      portion of the simple duration. Values must be greater than 0. 
    // * "indefinite" 
    //      The animation is defined to repeat indefinitely (i.e. until the document ends). 
    else if ( aName == KTmpRepeatCount )
        {
        iRepeatCountSet = ETrue;
        if ( aValue == KTmpIndefinite )
            {
            iRepeatCount = KRepeatCountMax;
            }
        else
            {
            TLex    lexer  ( aValue );
            TReal32  val = 1.0f;
            // Check for invalid number string or negative value
            // Also check for value value followed by other characters, i.e ("6-4" or "3abd"
            // Starting and ending whitespaces are chopped by lexer.
            if ( lexer.Val( val, '.' ) != KErrNone || val <= 0 || lexer.Remainder().Length() > 0 )
                {
                val = 1.0f;
                }
            iRepeatCount = val;
            }
        }
    else if ( aName == KTmpFill )
        {
        if ( aValue == _L( "freeze" ) )
            {
            iFill = KAnimFillFreeze;
            }
        }
    else if ( aName == KTmpRestart )
        {
        if ( aValue == _L( "whenNotActive" ) )
            {
            iAnimRestart = KRestartWhenNotActive; //KAnimFillFreeze;
            }
        else if ( aValue == _L( "never" ) )
            {
            iAnimRestart = KRestartNever;
            }
        // Default is 'always'

        }
    else if ( aName == KTmpValues )
        {
		iHaveValues = ETrue;
        iNoFrom = EFalse;

        switch ( iDataType )
            {

            case KSvgTypeOpacity:
            case KSvgTypeLength:
            iValuesFloat->Reset();
            break;

            case KSvgTypeList:
            case KSvgTypePath:
            iValuesFloat->Reset();
            iValuesPath->Reset();  // could be ResetAndDestroy()
            break;
            case KSvgTypeTextAnchor:
            case KSvgTypeGradientUnits:
            case KSvgTypeColor:
            case KSvgTypeInteger:
            case KSvgTypeVisibility:
            case KSvgTypeDisplay:
            iValuesFloat->Reset();
            iValuesInt->Reset();
            break;

			case KSvgTypeViewBox:
            iValuesFloat->Reset();
            iValuesViewBox->Reset();
			break;

            default:
            break;
            }

        TStringTokenizer    tkn ( aValue, KSEMICOLON );
        while ( tkn.HasMoreTokens() )
            {
            switch ( iDataType )
                {
                case KSvgTypeOpacity:
                    {
                    TFloatFixPt KZero;
                    TFloatFixPt KOne( KFloatFixOne );
                    TFloatFixPt opacity;

                    TPtrC lToken = tkn.NextToken();

                    if ( TFloatFixPt::ConvertString( lToken, opacity ) != KErrNone )
                        {
                        opacity = KOne;
                        }
                    else
                        {

                        if ( lToken.Locate('%') != KErrNotFound )
                        {
                        opacity = opacity / TFloatFixPt(100);
                        }

                        if ( opacity < KZero )
                            opacity = KZero;
                        else if ( opacity > KOne )
                            opacity = KOne;
                        }
                    User::LeaveIfError( iValuesFloat->Append( opacity ) );
                    }
                break;

                case KSvgTypeLength:
                User::LeaveIfError(iValuesFloat->Append( TFloatFixPt::ConvertString( tkn.NextToken() ) ));
                break;

                case KSvgTypeList:
                case KSvgTypePath:
                    {
                    CGfxGeneralPath*tPath   = CGfxGeneralPath::NewL();
                    CleanupStack::PushL(tPath);
                    if ( iAttrId == KAtrPoints ) //.
                        {
                        TSvgPathDataParser::ParsePointDataL( tkn.NextToken(), tPath );
                        }
                        else
                        {
                    TSvgPathDataParser::ParsePathData( tkn.NextToken(),
                                                              tPath );
                        }
                    User::LeaveIfError(iValuesPath->Append( tPath ));
                    CleanupStack::Pop();
                    }

                break;
                case KSvgTypeColor:
                    {
                    TSvgColor   tColor  ( KGfxColorNull );
                    TUint32      tempVal;




                    TBool col = tColor.GetStringL( tkn.NextToken(), tempVal );

                    if(!col)
                    {

                    tempVal = 0x000000; // set the default to black,
                    }
                    User::LeaveIfError(iValuesInt->Append( tempVal ));
                    }
                break;

                case KSvgTypeSpreadMethod:
                    {
                    TInt pos = ((CSvgDocumentImpl*)iOwnerDocument)->SchemaData()->FindSpreadMethod(tkn.NextToken() );
                        if(pos != KErrNotFound)
                            {
                            User::LeaveIfError(iValuesInt->Append( pos ));
                            }
                        else
                            {
                            User::LeaveIfError(iValuesInt->Append( 0));// default pad
                            }
                    }
                break;

                case KSvgTypeInteger:
                User::LeaveIfError(iValuesInt->Append( TFloatFixPt::ConvertString( tkn.NextToken() ) ));
                break;

                case KSvgTypeVisibility:
                    {
                    TPtrC           tmptk           = tkn.NextToken();

                    if (tmptk.Find(_L( "visible" )) != KErrNotFound)
                        {
                        User::LeaveIfError(iValuesInt->Append( 0 ));
                        }
                    else if (tmptk.Find(_L( "hidden" )) != KErrNotFound)
                        {
                        User::LeaveIfError(iValuesInt->Append( 1 ));
                        }
                    else if ( tmptk.Find(_L("inherit")) != KErrNotFound )
                       {
                       User::LeaveIfError(iValuesInt->Append(KInherit));
                       }
                       

                    }
                break;
                case KSvgTypeGradientUnits:
                    {
                    TPtrC           tmptk           = tkn.NextToken();
                    if (tmptk.Find(_L( "objectBoundingBox" )) != KErrNotFound)
                        {
                        User::LeaveIfError(iValuesInt->Append( 1));
                        }
                    else if (tmptk.Find(_L( "userSpaceOnUse" )) != KErrNotFound)
                        {
                        User::LeaveIfError(iValuesInt->Append( 0));
                        }

                    }
                break;
                case KSvgTypeDisplay:
                    {
                    TPtrC tmptk = tkn.NextToken();

                    if (tmptk.Find(_L( "inline" )) != KErrNotFound)
                    {
             			//found inline
             			User::LeaveIfError(iValuesInt->Append( 0 ));
                    }
                    else if (tmptk.Find(_L( "none" )) != KErrNotFound)
                    {
                    	User::LeaveIfError(iValuesInt->Append( 16 ));
                    }
                    else
                   	{
                    	//assume inherit on everything else
                    	User::LeaveIfError(iValuesInt->Append( (TInt32) KEnumInherit ));
                    	//iValuesInt->Append( 0 );
                   	}

                    }
                break;

                case KSvgTypeTextAnchor:
                    {

                    TInt pos;

                    pos = ((CSvgDocumentImpl*)iOwnerDocument)->SchemaData()->FindTextAnchorEnum(tkn.NextToken());

                        if(pos!= -1)
                        {
                        User::LeaveIfError(iValuesInt->Append( pos ));
                        }
                    }
                break;

               // viewBox attribute
				case KSvgTypeViewBox:
					{
                    TGfxRectangle2D lVal;
					ParseViewBoxValue( tkn.NextToken(), lVal);
					User::LeaveIfError(iValuesViewBox->Append( lVal ));
					}
				break;

                default:
                break;
                }
            }
        }
    else if ( aName == KTmpCalcMode )
        {
        if ( aValue == _L( "discrete" ) )
            {
            iAnimTime->SetCalMode( KAnimCalcModeDiscrete );
            }
        else if ( aValue == _L( "paced" ) )
            {
            iAnimTime->SetCalMode( KAnimCalcModePaced );
            }
        else if ( aValue == _L( "spline" ) )
            {
            iAnimTime->SetCalMode( KAnimCalcModeSpline );
            // Indicate to the anim timing controller that
            // the Spline mode is set
            iAnimTime->SetIsSplineCalcMode( ETrue ); 
            TPtrC lSplineValue( iAnimTime->SplineValue() );
            if ( lSplineValue.Length() > 0 ) 
                { 
                TStringTokenizer tkn2 (lSplineValue, KSEMICOLON); 
                iAnimTime->PrepareTimeL( NULL ); 
                
                while ( tkn2.HasMoreTokens() ) 
                    { 
                    TSvgPointLexer  svgplex ( tkn2.NextToken() ); 
                    TChar           com;  // ingnor this 
                    TFloatFixPt          x1, y1, x2, y2; 
                    svgplex.GetNextPoint( com, x1, y1 ); 
                    svgplex.GetNextPoint( com, x2, y2 ); 
                    svgplex.Cleanup(); 
  
                    TFloatFixPt tmpZero = TFloatFixPt(0); 
                    TFloatFixPt tmpOne  = TFloatFixPt(1); 
  
#ifdef SVG_FLOAT_BUILD 
                    if ( ( x1 < ( tmpZero ) ) || ( x1 > ( tmpOne ) ) ) 
                        { 
                        x1 = tmpZero; 
                        } 
  
                    if ( ( y1 < ( tmpZero ) ) || ( y1 > ( tmpOne ) ) ) 
                        { 
                        y1 = tmpZero; 
                        } 
  
                    if ( ( x2 < ( tmpZero ) ) || ( x2 > ( tmpOne ) ) ) 
                        { 
                        x2 = tmpOne; 
                        } 
  
                    if ( ( y2 < ( tmpZero ) ) || ( y2 > ( tmpOne ) ) ) 
                        { 
                        y2 = tmpOne; 
                        } 
#else 
                    if ( ( x1.operator < ( tmpZero ) ) || ( x1.operator > ( tmpOne ) ) ) 
                        { 
                        x1 = tmpZero; 
                        } 
  
                    if ( ( y1.operator < ( tmpZero ) ) || ( y1.operator > ( tmpOne ) ) ) 
                        { 
                        y1 = tmpZero; 
                        } 
  
                    if ( ( x2.operator < ( tmpZero ) ) || ( x2.operator > ( tmpOne ) ) ) 
                        { 
                        x2 = tmpOne; 
                        } 
  
                    if ( ( y2.operator < ( tmpZero ) ) || ( y2.operator > ( tmpOne ) ) ) 
                        { 
                        y2 = tmpOne; 
                        } 
#endif 
                    iAnimTime->AddKeySplineL( x1, y1, x2, y2 ); 
                    } 
                }
            }
        else
            {
            iAnimTime->SetCalMode( KAnimCalcModeLinear );
            }
        }
    else if ( aName == KTmpKeyTimes )
        {
        // if paced spec says we ignore keyTimes
        if(iAnimTime->CalMode() != KAnimCalcModePaced)
            {
			iKeyTimesPresent = ETrue;
            iAnimTime->ResetKeyTime();
            TStringTokenizer    tkn ( aValue, KSEMICOLON );
            TInt32 first = 1;

            TFloatFixPt current_time;

            while ( tkn.HasMoreTokens() )
                {
                current_time = TFloatFixPt::ConvertString( tkn.NextToken() );

                if( (first && current_time != (TFloatFixPt) 0.0f) ||
                     current_time < (TFloatFixPt) 0.0f ||
                     current_time > (TFloatFixPt) 1.0f)
                    { // put on this keyframe animation
                      // Spec says freeze on errors, but it
                      // also says ignore keytimes on error

                    iAnimTime->ResetKeyTime();
					iKeyTimesPresent = EFalse;

                    break;
                    }

                first = 0;
                iAnimTime->AddKeyTime( current_time );
                }

			// last keytime value should be 1 for calcmode= linear or spline
            if( (current_time != (TFloatFixPt)1.0f) &&
				         (iAnimTime->CalMode() != KAnimCalcModeDiscrete) )
                {
                iAnimTime->ResetKeyTime();
                }
            }
        }
    else if ( aName == KTmpKeySplines )
        {
        // Count number of splines
        TInt32              splines = 0;
        TStringTokenizer    tkn1    ( aValue, KSEMICOLON );
        while ( tkn1.HasMoreTokens() )
            {
            tkn1.NextToken();
            splines++;
            }
            if(!iKeyTimesPresent)
        iAnimTime->CreateKeyTime( splines + 1 );
        if( iAnimTime->IsSplineCalcMode() ) 
            { 
            iAnimTime->PrepareTimeL( NULL ); 
            TStringTokenizer    tkn2    ( aValue, KSEMICOLON ); 
 
            while ( tkn2.HasMoreTokens() ) 
                { 
                TSvgPointLexer  svgplex ( tkn2.NextToken() ); 
                TChar           com;  // ingnor this 
                TFloatFixPt          x1, y1, x2, y2; 
                svgplex.GetNextPoint( com, x1, y1 ); 
                svgplex.GetNextPoint( com, x2, y2 ); 
                svgplex.Cleanup(); 
 
                TFloatFixPt tmpZero = TFloatFixPt(0); 
                TFloatFixPt tmpOne  = TFloatFixPt(1); 
 
#ifdef SVG_FLOAT_BUILD 
                if ( ( x1 < ( tmpZero ) ) || ( x1 > ( tmpOne ) ) ) 
                    { 
                    x1 = tmpZero; 
                    } 
 
                if ( ( y1 < ( tmpZero ) ) || ( y1 > ( tmpOne ) ) ) 
                    { 
                    y1 = tmpZero; 
                    } 
 
                if ( ( x2 < ( tmpZero ) ) || ( x2 > ( tmpOne ) ) ) 
                    { 
                    x2 = tmpOne; 
                    } 
 
                if ( ( y2 < ( tmpZero ) ) || ( y2 > ( tmpOne ) ) ) 
                    { 
                    y2 = tmpOne; 
                    } 
#else 
                if ( ( x1.operator < ( tmpZero ) ) || ( x1.operator > ( tmpOne ) ) ) 
                    { 
                    x1 = tmpZero; 
                    } 
 
                if ( ( y1.operator < ( tmpZero ) ) || ( y1.operator > ( tmpOne ) ) ) 
                    { 
                    y1 = tmpZero; 
                    } 
 
                if ( ( x2.operator < ( tmpZero ) ) || ( x2.operator > ( tmpOne ) ) ) 
                    { 
                    x2 = tmpOne; 
                    } 
 
                if ( ( y2.operator < ( tmpZero ) ) || ( y2.operator > ( tmpOne ) ) ) 
                    { 
                    y2 = tmpOne; 
                    } 
#endif 
                iAnimTime->AddKeySplineL( x1, y1, x2, y2 ); 
                } 
            } 
         else 
            { 
            iAnimTime->SetSplineValueL( aValue ); 
            } 
        }
    else if ( aName == KTmpAccumulate )
        {
        if ( aValue == _L( "sum" ) )
            {
            iAccumulate = KAccumSum;
            }
        }
    else if ( aName == KTmpAdditive )
        {
        if ( aValue == _L( "sum" ) )
            {
            iAdditive = KAdditiveSum;
            iAdditiveOrg = KAdditiveSum;
            }
        }
    else
        {
        if ( this->SetTestAttributeL( aName, aValue ) )
            {
            return KErrNone;
            }
        }
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// From CSvgElementImpl
// perform a deep clone of this object
// ---------------------------------------------------------------------------
MXmlElement* CSvgAnimationBase::CloneL(MXmlElement*)
    {
    return NULL;
    }


// ---------------------------------------------------------------------------
// Animation specific common methods
// ---------------------------------------------------------------------------
TBool CSvgAnimationBase::ReceiveEventProcL( MSvgEvent* aEvent,
                                           CSvgAnimationBase* aAnimElement )
    {
    if ( CSvgElementImpl::IsSVGEnginePaused() )
       {
       return EFalse;
       }
	TBool lGetTimerEvent = EFalse;
	TInt32 lEngineTime =0;

    TInt32 lCurTime = ((CSvgDocumentImpl *)(iOwnerDocument))->CurrentTime();
    // Check if target attribute is valid
    if ( iAttrId == 0xffff )
	    {
	    ((CSvgDocumentImpl*)iOwnerDocument)->RemoveFromEventReceiverList(this );
	    ((CSvgDocumentImpl*)iOwnerDocument)->Engine()->UpdatePresentation(1);
	     return EFalse;
	    }

    if ( !iInitDone )
        {
        iInitDone = ETrue;
        aAnimElement->InitAnimationL();
        if ( aAnimElement->iTargetElement == NULL )
            {
            return EFalse;
            }
        ((CSvgDocumentImpl*)iOwnerDocument)->AddEventBeginTime( aAnimElement, iAbsoluteBeginTime, aAnimElement->iTargetElement );
        }
    else if ( aAnimElement->iTargetElement == NULL )
        {
        return EFalse;
        }

    // select event type
    switch ( aEvent->EventType() )
        {
        case ESvgEngineEventTimerPrep :
        return EFalse;


        case ESvgEngineInternalEvent:
            {
            MSvgInternalEvent*  event       = ( MSvgInternalEvent* ) aEvent;
            CSvgElementImpl*               leventElement = (CSvgElementImpl*)  ( event->ObjectAddress() );

            CSvgAnimationBase*   ltargetElement = (CSvgAnimationBase*)event->ObjectAddress();
            TInt i;
            TInt lCount = iEventList->Count();

			if (event->SvgEvent() == ESvgEventRepeatEvent )
			{
				CSvgElementImpl* lElement = event->ObjectAddress();

				if (lElement)
				{
            //        NotifyAnimationsRepeatingOnThisOneL( lElement->Id(),   ltargetElement);
				}

			}

            for (i = 0; i < lCount; i++ )
                {
                if ( (iEventList->operator[](i)).iEvent != ESvgEventNone &&
                     (iEventList->operator[](i)).iEvent == event->SvgEvent() )
                    {
                    if ( leventElement == (iEventList->operator[](i)).iTargetElement )
                        {
                        switch(event->SvgEvent())
                            {
                                case ESvgEventRepeatEvent:
                                // Check whether the target element has repeated as many times
                                // as is defined for the begin of this element.
                                if ( ltargetElement->iCurrentRepeatCount == (TInt32) (iEventList->operator[](i)).iRepeatValue )
                                    {
                                    TReal32 lFractionalTime = (iEventList->operator[](i)).iRepeatValue - ((TInt32) (iEventList->operator[](i)).iRepeatValue);

                                    SetBeginByEventL( aAnimElement,
                                                    (TInt32) (lCurTime +
                                                    (iEventList->operator[](i)).iOffset +
                                                        ltargetElement->Duration()*lFractionalTime), ETrue);

									// Get THhe Engine's Current Media Time.
									lEngineTime = ((CSvgDocumentImpl*)iOwnerDocument)->Engine()->CurrentTIme();

									// calculate the start time.
									TInt32 lStartTimeForThis = (TInt32) (lCurTime +
                                                    (iEventList->operator[](i)).iOffset +
                                                    ltargetElement->Duration()*lFractionalTime) ;

									//compare BOTH.
									if(lStartTimeForThis <= lEngineTime )
										{
										// indicates that this element should get the last timer
										// event.
										lGetTimerEvent = ETrue;
										}

                                    }
                                break;

                                default :
									{

                                SetBeginByEventL( aAnimElement, lCurTime + (iEventList->operator[](i)).iOffset , ETrue);
									// calculate the start time.
									TInt32 lStartTimeForThis = (lCurTime + (iEventList->operator[](i)).iOffset ) ;

									// see the Engine's Current media time.
									lEngineTime = ((CSvgDocumentImpl*)iOwnerDocument)->Engine()->CurrentTIme();

									// Compare BOTH.
								/*	if(lStartTimeForThis <= lEngineTime )
										{
										// indicates that this element should get the last timer
										// event.
										lGetTimerEvent = ETrue;
										}*/
									}
                                break;
                            }
                        }
					}
                    }

          	TInt lCount2 = iEndTimeList->Count();
            for (i = 0; i < lCount2; i++ )
            {

       		 if ( (iEndTimeList->operator[](i)).iEvent != ESvgEventNone &&
                     (iEndTimeList->operator[](i)).iEvent == event->SvgEvent() )
                    {
                    if ( leventElement == (iEndTimeList->operator[](i)).iTargetElement )
                        {
                        switch(event->SvgEvent())
                            {
                                case ESvgEventRepeatEvent:
                                if ( ltargetElement->iCurrentRepeatCount == (TInt32) (iEndTimeList->operator[](i)).iRepeatValue )
                                    {
                                    TReal32 lFractionalTime = (iEndTimeList->operator[](i)).iRepeatValue - ((TInt32) (iEndTimeList->operator[](i)).iRepeatValue);

                                    SetEndByEvent( aAnimElement,
                                                    (TInt32) (lCurTime +
                                                    (iEndTimeList->operator[](i)).iOffset +
                                                    ltargetElement->Duration()*lFractionalTime) );

								     }
                                break;

                                default :
									{
                                	SetEndByEvent( aAnimElement, lCurTime + (iEndTimeList->operator[](i)).iOffset );
									}
                                break;
                            }
                        }
					}

				}

			}
		if(lGetTimerEvent)
			{
			// NO return this means that the Element should get the Last timer event.
			break;
			}
		else
			{
			// this should not get the last timer event.
        return EFalse;
			}


        case ESvgEngineEventKeyPress:
            {
            MSvgUiKeyEvent* event   = ( MSvgUiKeyEvent* ) aEvent;
            TInt i;

            TInt lCount = iEventList->Count();
            for (i = 0; i < lCount; i++ )
                 {

                 if ( (iEventList->operator[](i)).iEvent == ESvgEventKey &&
                      (iEventList->operator[](i)).iAccessKeyCode == event->KeyCode() )
                    {
                    SetBeginByEventL( aAnimElement, lCurTime + (iEventList->operator[](i)).iOffset, ETrue );
                    }
                 }
            TInt lEndTimesCount = iEndTimeList->Count();
            for (i = 0; i < lEndTimesCount; i++ )
                 {
            if ( (iEndTimeList->operator[](i)).iEvent == ESvgEventKey &&
                      (iEndTimeList->operator[](i)).iAccessKeyCode == event->KeyCode() )
                    {
                    SetEndByEvent( aAnimElement, lCurTime + (iEndTimeList->operator[](i)).iOffset );
                    }
                 }

             }
        return EFalse;


        case ESvgEngineEventTimer:
        break;

        default:
        // mouse event is ignored here
        return EFalse;
        }


    // Only timer event come thorugh after here
    TSvgTimerEvent* timerEvent  = ( TSvgTimerEvent* ) aEvent;
    TSvgTimerEvent lLastEventTime(lEngineTime);

	// this is treated as
	if(lGetTimerEvent)
		{
		// send the last timer event.
		timerEvent = &lLastEventTime;
		}

  //   if( (!iOverrideTime) )
        {
        // This condition would not be satisfied for animations having one begin time
        // E.g begin='5s' moves on to check if iAnimStatus is NotActive
        if ( (iBeginTimeIndex+1 < iAnimTime->BeginTimesCount() ) && ( timerEvent->Time() >= iAnimTime->GetBeginTime( iBeginTimeIndex + 1 )))
            {
            
             if ( iAnimRestart == KRestartWhenNotActive && iAnimStatus == KAnimActive )
		        {
		        return ETrue;     // Not start when active and restart="whenNotActive"
		        }
		    if ( iAnimRestart == KRestartNever && iHadBegun )
		        {
		        return ETrue;     // Not start when anim had begun and if restart="never"
		        }
            iBeginTimeIndex++;
            iAnimTime->SetBeginTime( iAnimTime->GetBeginTime( iBeginTimeIndex ));
			// added for setting proper end time
			iAbsoluteBeginTime= iAnimTime->GetBeginTime( iBeginTimeIndex );
            iAnimStatus = KAnimNotActive;

            if (iTargetElement)
            {
            	iTargetElement->iHasAnimationBase = (TInt)this;
        	iTargetElement->SetIsAnimating( EFalse );
        	SetIsAnimating( EFalse );
            }
        	iAnimTime->GetNextEndTime(iAnimTime->BeginTime());

			TUint32 lSimpleDur= SimpleDuration();
		   	if(lSimpleDur != KTimeIndefinite)
   				{
			   	iAnimTime->SetEndTime( lSimpleDur );
	   			}

          	// in case of multiple begin times we need to sort the list again.
        	((CSvgDocumentImpl*)iOwnerDocument)->iInitSortList=ETrue;
            }
        }

    if ( iAnimStatus == KAnimNotActive )
        {
        // Animation element becomes active only if current time exceeds 
        // the anim's begin time and its duration is not indefinite 
        if ( timerEvent->Time() >= iAnimTime->BeginTime() )
            {
          //  iHasEnded = EFalse;
            // start animation
            iAnimStatus = KAnimActive;
			iDoFreeze = EFalse;
            if (iTargetElement)
            {
            	iTargetElement->iHasAnimationBase = (TInt)this;
        	iTargetElement->SetIsAnimating( ETrue );
        	SetIsAnimating( ETrue );
            }
			if ((this->ElemID() == KSvgAnimateElement) || 
				(this->ElemID() == KSvgAudioElement) || 
				(this->ElemID() == KSvgMediaAnimationElement ))
				{
				// this will be used to calculate the from values again
				iHadBegun = EFalse;
				}
			else
				{
				iHadBegun = ETrue;
				}
            iAdditive = iAdditiveOrg;
			iCurrentRepeatCount= 0;
			((CSvgDocumentImpl*)iOwnerDocument)->Engine()->NotifyAnimationStarted();
			
			//Triggers internal begin event so that animations dependent on this would get the events
				TSvgInternalEvent   event   ( ESvgEventBeginEvent, this,iAnimTime->BeginTime() );
	            
	            ((CSvgDocumentImpl*)iOwnerDocument)->Engine()->ProcessEventL( 
	                ( CSvgDocumentImpl* ) OwnerDocument(), &event );

			}
        else
            {
            iAdditive = KAdditivePass;
            }
        }

    if ( iAnimStatus == KAnimActive )
        {
		// if end time less than the simple duration

			// DurationTime() shall return the duration set in svg
			// E.g  begin="0s"  dur="10s" repeatCount='3' returns DurationTime() 10000, 
			// repeating of the animation would be handled while checking done 
			// before setting iAnimStatus to KAnimFinished. (see if ( iAnimStatus == KAnimEnd ))
		  if ( iAnimTime->DurationTime() != KTimeIndefinite
	               && timerEvent->Time() >= iAnimTime->BeginTime() + iAnimTime->DurationTime() )
	            {
	            // end animation
	            iAnimStatus = KAnimEnd;
		        if (iTargetElement)
		            {
		            iTargetElement->iHasAnimationBase = (TInt)this;
                	iTargetElement->SetIsAnimating( EFalse );
                	SetIsAnimating( EFalse );
                	
	                }
	            }
	       	// any endtime got from simpleDuration, EndTime values in svg content, event based endtimes 
	       	// E.g  begin="0s"  dur="10s" repeatCount='3' returns EndTime() 30000, 
	        if ( timerEvent->Time() >= iAnimTime->EndTime() )
	            {
	            // end animation
	            iAnimStatus = KAnimEnd;
	            if (iTargetElement)
	                {
	                iTargetElement->iHasAnimationBase = (TInt)this;
        	        iTargetElement->SetIsAnimating( EFalse );
                	SetIsAnimating( EFalse );
	        	    }
	            }

        }

    if ( iAnimStatus == KAnimEnd )
        {

						//Check if animation is finished
						// Check 1. repeatcount reached
						//			 2. Repeatduration valid and reached
						//			 3. Endtime reached
            if (( ( iCurrentRepeatCount >= iRepeatCount )||
            
                ( ( iAnimTime->RepeatDurationTime() != KTimeIndefinite )&&
                ( iRepeatCount == KRepeatCountMax )&& 
                ( timerEvent->Time() >= iAbsoluteBeginTime + iAnimTime->RepeatDurationTime() ) ) )
               
               || timerEvent->Time() >= iAnimTime->EndTime()
                )
                {
                iEndTimeIndex++;
				TInt32 LEndTIme = lCurTime;

                iAnimTime->SetEndTime(KTimeIndefinite);
                iAnimStatus = KAnimFinished;

                if (iTargetElement)
                    {
                    iTargetElement->iHasAnimationBase = (TInt)this;
                	iTargetElement->SetIsAnimating( EFalse );
                	SetIsAnimating( EFalse );
        	        }

                iOverrideTime = EFalse;

                // if its begin depends on its end than it might create a problem.
                if(!iIsEndEventSent || IsSelfDependentForBegin())
	                {
					((CSvgDocumentImpl*)iOwnerDocument)->Engine()->NotifyAnimationEnded();
					TSvgInternalEvent   event   ( ESvgEventEndEvent, this,LEndTIme );
					TSvgInternalEvent   event1   ( ESvgEventRepeatEvent, this,LEndTIme );
					//repeatCount Event is also generated
			        ((CSvgDocumentImpl*)iOwnerDocument)->Engine()->ProcessEventL( 
			            ( CSvgDocumentImpl* ) OwnerDocument(), &event1 );
			        ((CSvgDocumentImpl*)iOwnerDocument)->Engine()->ProcessEventL( 
			            ( CSvgDocumentImpl* ) OwnerDocument(), &event );
					// Update presentation should be called only if the animation gets finished
					// Check for the multiple begin values to make sure that the animation ended completely.
					if ( (iBeginTimeIndex+1 >= iAnimTime->BeginTimesCount() ) || ( iAnimTime->GetBeginTime( iBeginTimeIndex + 1 ) <= timerEvent->Time()))
						{
						((CSvgDocumentImpl*)iOwnerDocument)->Engine()->UpdatePresentation(1);
						}
					return ETrue; 	// so that draw happens if begin depends on its own end event
		            }
		           else
		            {
		            iIsEndEventSent = EFalse;
		            }

                }
                
            else
                {
                	// E.g  begin="0s"  dur="10s" repeatCount='3' 
                	//in such case if curtime < 30s and iAnimStatus is End then animation would repeat
                	iCurrentRepeatCount++;
                if ( iAccumulate == KAccumSum )
                    {
                    aAnimElement->SetAccumulateValues();
                    }
                aAnimElement->ResetAnimationL();
                iAnimTime->ResetBeginTime( iAnimTime->BeginTime() +
                                           iAnimTime->DurationTime() );
                iAnimStatus = KAnimActive;

                if (iTargetElement)
                    {
                    iTargetElement->iHasAnimationBase = (TInt)this;
                	iTargetElement->SetIsAnimating( ETrue );
                	SetIsAnimating( ETrue );
        	        }

                iOverrideTime = EFalse;
					// Generates repeatEvent 
					//Triggers internal repeat event so that animations dependent on this would get the events
			
					TInt32 lRepeatTime = iAbsoluteBeginTime + iCurrentRepeatCount*iAnimTime->DurationTime();
						
					TSvgInternalEvent   event   ( ESvgEventRepeatEvent, this,lRepeatTime );
		            ((CSvgDocumentImpl*)iOwnerDocument)->Engine()->ProcessEventL( 
		                ( CSvgDocumentImpl* ) OwnerDocument(), &event );
                }
        }

       // do not redraw.
       if(iAnimStatus == KAnimNotActive || (iAnimStatus == KAnimFinished && iDoFreeze))
	       {
	       // this will avoid going in to the draw again.
	       return EFalse;
	       }
      return ETrue;

    }

//
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgAnimationBase::SetFillValueL()
    {
    switch ( iDataType )
        {
        case KSvgTypeOpacity:
        case KSvgTypeLength:
        iTargetElement->GetAttributeFloat( iAttrId, iPropFloat );
        if ( ( iFill == KAnimFillFreeze ) && iNoFrom )
            {
            iFromFloat = iPropFloat;
            }
        break;

        case KSvgTypeList:
        case KSvgTypePath:
        break;

        case KSvgTypeColor:
        if ( iAttributeFlag && !iCurrentRepeatCount )
            {
            iTargetElement->GetAttributeIntL( iAttrId, iPropInt );

            }
        break;
        case KSvgTypeTextAnchor:
        case KSvgTypeGradientUnits:
        case KSvgTypeSpreadMethod:
        case KSvgTypeInteger:
        case KSvgTypeDisplay:
        case KSvgTypeVisibility:
        if ( iAttributeFlag )
            {
            iTargetElement->GetAttributeIntL( iAttrId, iPropInt );
            }
        break;

		// viewBox
        case KSvgTypeViewBox:
			{
			if(iTargetElement->ElemID() == KSvgSvgElement)
				{
				((CSvgSvgElementImpl*)iTargetElement)->GetViewBox( iOrgViewBox );
				if ( ( iFill == KAnimFillFreeze ) && iNoFrom )
					{
					iFromViewBox = iOrgViewBox;
					}
				}
			}
			 break;

        default:
        break;
        }
    }


//
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgAnimationBase::SetToOriginalL()
    {
    switch ( iDataType )
        {
        case KSvgTypeOpacity:
        case KSvgTypeLength:
        iTargetElement->SetAttributeFloatL( iAttrId, iPropFloat );
        break;
        case KSvgTypeList:
        case KSvgTypePath:
            iTargetElement->SetAttributePathL( iAttrId, iOrgPath );
        break;
        case KSvgTypeTextAnchor:
        case KSvgTypeGradientUnits:
        case KSvgTypeSpreadMethod:
        case KSvgTypeColor:
        case KSvgTypeInteger:
        case KSvgTypeDisplay:
        case KSvgTypeVisibility:
        if ( iAttributeFlag )
            {
            iTargetElement->SetAttributeIntL( iAttrId, iPropInt );
            }
        break;

		//viewBox
        case KSvgTypeViewBox:
			{
		    if(iTargetElement->ElemID() == KSvgSvgElement)
				{
				((CSvgSvgElementImpl*)iTargetElement)->SetViewBoxL( iOrgViewBox );
				}
			}
			break;

        default:
        break;
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgAnimationBase::SetToInitialValueL()
    {
    switch ( iDataType )
        {
        case KSvgTypeOpacity:
        case KSvgTypeLength:
            iTargetElement->SetAttributeFloatL( iAttrId, iPropFloat );
        break;
        case KSvgTypeList:
        case KSvgTypePath:
             iTargetElement->SetAttributePathL( iAttrId, iOrgPath );
        break;
        case KSvgTypeColor:
        if ( iAttributeFlag )
            {
            iTargetElement->SetAttributeIntL( iAttrId, iPropInt );
            }
        else
            {
            if( iAttrId == KCSS_ATTR_VISIBILITY ||
                iAttrId == KCSS_ATTR_FILL ||
                iAttrId == KCSS_ATTR_STROKE )
                {
                iTargetElement->RemoveAttribute(iAttrId);
                }

            }
        break;
        case KSvgTypeTextAnchor:
        case KSvgTypeGradientUnits:
        case KSvgTypeSpreadMethod:
        case KSvgTypeInteger:
        case KSvgTypeDisplay:
        case KSvgTypeVisibility:
        if ( iAttributeFlag )
            {
            iTargetElement->SetAttributeIntL( iAttrId, iPropInt );
            }
        else
            {
            if( iAttrId == KCSS_ATTR_VISIBILITY ||
                iAttrId == KCSS_ATTR_FILL ||
                iAttrId == KCSS_ATTR_STROKE )
                {
                iTargetElement->RemoveAttribute(iAttrId);
                }

            }
        break;

		//viewBox
        case KSvgTypeViewBox:
			{
		    if(iTargetElement->ElemID() == KSvgSvgElement)
				{
				((CSvgSvgElementImpl*)iTargetElement)->SetViewBoxL( iOrgViewBox );
				}
			}
		break;

        default:
            ;
        }
    }
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgAnimationBase::SetToEndValueL()
    {
    switch ( iDataType )
        {
        case KSvgTypeOpacity:
        case KSvgTypeLength:
            if( iAdditive == KAdditiveSum && !iNoFrom && (this->ElemID() == KSvgAnimateElement ))
            {
                TFloatFixPt lCurValue;
                iTargetElement->GetAttributeFloat( iAttrId, lCurValue );
                iTargetElement->SetAttributeFloatL(iAttrId, lCurValue +  iEndFloat );
            }
            else
            {
                iTargetElement->SetAttributeFloatL( iAttrId, iEndFloat );
            }
        break;
        case KSvgTypeList:
        case KSvgTypePath:
            {
            iTargetElement->SetAttributePathL( iAttrId, iEndPath );
            }
        break;
		case KSvgTypeTextAnchor:
        case KSvgTypeGradientUnits:
        case KSvgTypeSpreadMethod:
        case KSvgTypeInteger:
        case KSvgTypeDisplay:
        case KSvgTypeVisibility:
			{
        if ( iAttributeFlag )
            {
            iTargetElement->SetAttributeIntL( iAttrId, iEndInt );
            }
			}
        break;

        case KSvgTypeColor:
			{
			// fix for accum_addit_7
			if( iAttributeFlag )
				{
				if( iAdditive == KAdditiveSum && !iNoFrom  )
					{

					TInt32 lCurValue;
					iTargetElement->GetAttributeIntL( iAttrId, lCurValue );


				   lCurValue = AddColor( iEndInt, lCurValue );
					iTargetElement->SetAttributeIntL( iAttrId, lCurValue );
					}

				else
					{
					iTargetElement->SetAttributeIntL( iAttrId, iEndInt );
					}
				}
			}
        break;

		//viewBox
        case KSvgTypeViewBox:
			{
		     if( iAdditive == KAdditiveSum && !iNoFrom && (this->ElemID() == KSvgAnimateElement ))
				{
				TGfxRectangle2D  lCurValue;
				if(iTargetElement->ElemID() == KSvgSvgElement)
					{
					((CSvgSvgElementImpl*)iTargetElement)->GetViewBox( lCurValue );
					}

				lCurValue.iX += iEndViewBox.iX ;
				lCurValue.iY += iEndViewBox.iY ;
				lCurValue.iWidth += iEndViewBox.iWidth ;
				lCurValue.iHeight += iEndViewBox.iHeight ;


				// set the vlue to the target element.
				 if(iTargetElement->ElemID() == KSvgSvgElement)
					{
					((CSvgSvgElementImpl*)iTargetElement)->SetViewBoxL( lCurValue );
					}
				}

  			 else
				{
				if(iTargetElement->ElemID() == KSvgSvgElement)
					{
					((CSvgSvgElementImpl*)iTargetElement)->SetViewBoxL( iEndViewBox );
					}
				}
			}
        break;

        default:
        break;
        }
    }



// ---------------------------------------------------------------------------
// Initialization before start of animation
// ---------------------------------------------------------------------------
void CSvgAnimationBase::InitAnimationL()
    {
    // 1. Create keytimes if no keytime but values available
    // 2. Set animation time according to calcMode
    //
    // Also, add target element to event receiver list for mouse event
    //Set the First Begin Time



    if( iAnimTime->BeginTimesCount() > 0 )
        {
        iAnimTime->SetBeginTime( iAnimTime->GetBeginTime(0));
        iAbsoluteBeginTime = iAnimTime->GetBeginTime(0);
        }


    if( iAnimTime->RepeatDurationTime() != KTimeIndefinite )
        {
         iAnimTime->CalculateAnimTimes();
        }




    iOverrideTime = ETrue;

    switch ( iDataType )
        {
        case KSvgTypeOpacity:
        case KSvgTypeLength:
        if ( iNoFrom )
            {
            iOrgFromFloat = iFromFloat = iPropFloat;
            }
        iAnimTime->CreateKeyTime( iValuesFloat->Count() );
        iAnimTime->PrepareTimeL( iValuesFloat );
        break;
        case KSvgTypeList:
        case KSvgTypePath:
            {
           
            if ( iOrgFromPath )
                {
                delete iOrgFromPath;
                iOrgFromPath = NULL;
                }
            iOrgFromPath = CGfxGeneralPath::NewL();

			CGfxGeneralPath* parentPath = NULL;
            iTargetElement->GetAttributePath( iAttrId, parentPath );
            CGfxPathIterator*   itr;
            TGfxAffineTransform affineTransform;
            
            if (parentPath != NULL)
            {
            parentPath->GetPathIteratorL( &affineTransform, itr );
            CleanupStack::PushL( itr );

            iOrgFromPath->AppendL( itr );
            CleanupStack::PopAndDestroy( 1 ); // destroy itr
			}
            // Set keytime

			iValuesFloat->Reset();
            TInt lCount = iValuesPath->Count();
            for ( TInt i = 0; i < lCount ; i++ )
                {
                User::LeaveIfError(iValuesFloat->Append( TFloatFixPt( i ) )); // create dummy scalar array
                }
            iAnimTime->CreateKeyTime( iValuesFloat->Count() );
            iAnimTime->PrepareTimeL( iValuesFloat );
            }
        break;

        case KSvgTypeColor:
            {
            // Set keytime
            iValuesFloat->Reset();
            // Compute the colour distance in iValuesFloat
            FindColorDistanceL();
            iAnimTime->CreateKeyTime( iValuesFloat->Count() );
            iAnimTime->PrepareTimeL( iValuesFloat );
            }
        break;
        case KSvgTypeTextAnchor:
        case KSvgTypeGradientUnits:
        case KSvgTypeSpreadMethod:
        case KSvgTypeVisibility:
        case KSvgTypeDisplay:
        case KSvgTypeInteger:
        iAnimTime->CreateKeyTimeForEnumeration( iValuesInt->Count() );
        iAnimTime->PrepareTimeL( ( RArray<TFloatFixPt>* ) iValuesInt );
        break;

       // viewBox
        case KSvgTypeViewBox:
			{
		    if ( iNoFrom )
			    {
				 iOrgFromViewBox = iFromViewBox = iOrgViewBox;
				}
        // Set keytime
			iValuesFloat->Reset();
            TInt lCount = iValuesViewBox->Count();
            for ( TInt i = 0; i < lCount ; i++ )
                User::LeaveIfError(iValuesFloat->Append( TFloatFixPt( i ) )); // create dummy scalar array
            iAnimTime->CreateKeyTime( iValuesFloat->Count() );
            iAnimTime->PrepareTimeL( iValuesFloat );
			}
			break;

        default:
        // Text
			break;
        }

     TUint32 lSimpleDuration = SimpleDuration();
     if( lSimpleDuration != KTimeIndefinite)
            {
            iAnimTime->SetEndTime(lSimpleDuration);
			iAnimTime->SetModifiedEndTime(lSimpleDuration);
            }

	 if(this->ElemID() == KSvgAnimateColorElement)

        {

        // this Is to check that the animateColor is not applied to anything
        // other than color.

        switch(iDataType )
            {
            case KSvgTypeColor:
            break;
            default :
            ((CSvgDocumentImpl*)iOwnerDocument)->RemoveFromEventReceiverList(this );
            ((CSvgDocumentImpl*)iOwnerDocument)->Engine()->UpdatePresentation(1);
            break;
            }

        }

    // Add to event receiver list
    // Also need to look end event...
    CSvgElementImpl* targetElement = NULL;


		// this is for forward reference.
	 	// first find the count this can be more than one
		// for multiple BeginTimes.
	 	TInt lCount = iEventList->Count();
	 	for (TInt i=0; i< lCount; i++)
	 		{

			if ( (iEventList->operator[](i)).iTargetElement != NULL )
			{
			((CSvgDocumentImpl*)iOwnerDocument)->AddToEventReceiverListL(
			                                     (iEventList->operator[](i)).iTargetElement,
			                                     (iEventList->operator[](i)).iEvent,
			                                     KSvgEventMaskExternalUI );
			}
			
	 		// check that this is not NULL;
	 		// NULL means Begin time is absolute.
		 	if(iEventList->operator[](i).iReferenceElemeId != NULL)
			 	{

			 	// find the element.
			 	targetElement = ( CSvgElementImpl * )
		                      ((CSvgDocumentImpl*)iOwnerDocument)->GetElementById(
		                         iEventList->operator[](i).iReferenceElemeId->Des());
				if(targetElement != NULL)
					{

					// Add to Event receiver list.
					((CSvgDocumentImpl*)iOwnerDocument)->AddToEventReceiverListL(
					targetElement ,(iEventList->operator[](i)).iEvent, KSvgEventMaskExternalUI );

					// update the pointer to the target element.
					iEventList->operator[](i).iTargetElement = targetElement;
					iTargetElement->iHasAnimationBase = (TInt)this;
					}

				delete  iEventList->operator[](i).iReferenceElemeId;
				iEventList->operator[](i).iReferenceElemeId = NULL;

				}

			}

	// add the end sync element so that it receives mouse events

	 if( iEndTimeList->Count() > 0 )
        {

		// this is for forward reference.
	 	// first find the count this can be more than one
		// for multiple BeginTimes.
	 	TInt lCount2 = iEndTimeList->Count();
	 	for (TInt i=0; i< lCount2; i++)
	 		{
	 		
	 		if ( (iEndTimeList->operator[](i)).iTargetElement != NULL )
			{
			((CSvgDocumentImpl*)iOwnerDocument)->AddToEventReceiverListL(
			                                     (iEndTimeList->operator[](i)).iTargetElement,
			                                     (iEndTimeList->operator[](i)).iEvent,
			                                     KSvgEventMaskExternalUI );
			}
			
	 		// check that this is not NULL;
	 		// NULL means Begin time is absolute.
		 	if(iEndTimeList->operator[](i).iReferenceElemeId != NULL)
			 	{

			 	// find the element.
			 	targetElement = ( CSvgElementImpl * )
		                      ((CSvgDocumentImpl*)iOwnerDocument)->GetElementById(
		                         iEndTimeList->operator[](i).iReferenceElemeId->Des());
				if(targetElement != NULL)
					{

					// Add to Event receiver list.
					((CSvgDocumentImpl*)iOwnerDocument)->AddToEventReceiverListL(
					                                     targetElement ,
					                                     (iEndTimeList->operator[](0)).iEvent,
					                                     KSvgEventMaskExternalUI );

					// update the pointer to the target element.
					iEndTimeList->operator[](i).iTargetElement = targetElement;
					iTargetElement->iHasAnimationBase = (TInt)this;
					}

				delete  iEndTimeList->operator[](i).iReferenceElemeId;
				iEndTimeList->operator[](i).iReferenceElemeId = NULL;

				}

			}

		}

        }

//
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgAnimationBase::SetAccumulateValues()
    {
   /* switch ( iDataType )
        {
        case KSvgTypeOpacity:
        case KSvgTypeLength:
        case KSvgTypeList:
        case KSvgTypePath:
        break;

        case KSvgTypeColor:
        break;

        case KSvgTypeInteger:
        break;

        default:
        // Descriptor
        ;
        }*/
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TBool CSvgAnimationBase::IsElementAttrs( const TDesC& aAttrName )
    {
    TInt pos = ((CSvgDocumentImpl*)iOwnerDocument)->SchemaData()->GetSVGTAttributeId(aAttrName);

    if ( pos == KErrNotFound)
        {
        // Did not find the attribute name
        return EFalse;
        }
    else
        {
        iAttrId= (TUint16) pos;
        if(iAttrId <= KSvgCoordAttrEndIndex || iAttrId == KAtrFy || iAttrId == KAtrFx )
            {
            iDataType = KSvgTypeLength;
            }
        else
            {
            switch(iAttrId)
                {
                case KAtrGradientUnits:
                iDataType = KSvgTypeGradientUnits;
                break;
                case KAtrSpreadMethods:
                iDataType = KSvgTypeSpreadMethod;
                break;

                case KAtrStopColor:
                iDataType= KSvgTypeColor;
                break;

                case KAtrPoints:
                case KAtrD:
                iDataType = KSvgTypePath;
                break;

                case KAtrOffset: // Always need to be between 0 - 1 , thus treating same as opacity.
                case KAtrStopOpacity:
                iDataType= KSvgTypeOpacity;
                break;

				case KAtrViewBox:
                iDataType= KSvgTypeViewBox;
                break;

                default:
                return EFalse;
                }
            }
        }

    return ETrue;
    }



// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TBool CSvgAnimationBase::IsPresentationAttrs( const TDesC& aAttrName )
    {
    TInt            ret;
    ret = ((CSvgDocumentImpl*)iOwnerDocument)->SchemaData()->GetPresentationAttributeId( aAttrName);
    if ( ret  == KErrNotFound )
        {
        // Did not find the attribute name
        return EFalse;
        }
    else
        {
        switch ( ret )
            {
            case KCSS_ATTR_FILL:
            // fill
            iAttrId = KCSS_ATTR_FILL;
            iDataType = KSvgTypeColor;
            break;
            case KCSS_ATTR_STROKE:
            // stroke
            iAttrId = KCSS_ATTR_STROKE;
            iDataType = KSvgTypeColor;
            break;
            case KCSS_ATTR_STROKEWIDTH:
            // stroke-width
            iAttrId = KCSS_ATTR_STROKEWIDTH;
            iDataType = KSvgTypeLength;
            break;
            case KCSS_ATTR_STROKE_DASHOFFSET:
            iAttrId = KCSS_ATTR_STROKE_DASHOFFSET;
            iDataType = KSvgTypeLength;
            break;
            case KCSS_ATTR_VISIBILITY:
            // visibility
            iAttrId = KCSS_ATTR_VISIBILITY;
            iDataType = KSvgTypeVisibility;
            break;
            case KCSS_ATTR_COLOR:
            // color
            iAttrId = KCSS_ATTR_COLOR;
            iDataType = KSvgTypeColor;
            break;
            case KCSS_ATTR_FONTSIZE:
            // font-size
            iAttrId = KCSS_ATTR_FONTSIZE;
            iDataType = KSvgTypeLength;
            break;
            case KCSS_ATTR_DISPLAY:
            // display
            iAttrId = KCSS_ATTR_DISPLAY;
            iDataType = KSvgTypeDisplay;
            break;
            case KCSS_ATTR_TEXTANCHOR:
            iAttrId = KCSS_ATTR_TEXTANCHOR;
            iDataType = KSvgTypeTextAnchor;
            break;
            case KCSS_ATTR_FILL_OPACITY:
            iAttrId = KCSS_ATTR_FILL_OPACITY;
            iDataType = KSvgTypeOpacity;
            break;
            case KCSS_ATTR_STROKE_OPACITY:
            iAttrId = KCSS_ATTR_STROKE_OPACITY;
            iDataType = KSvgTypeOpacity;
            break;
            case KCSS_ATTR_GROUP_OPACITY:
            iAttrId = KCSS_ATTR_GROUP_OPACITY;
            iDataType = KSvgTypeOpacity;
            break;
            default:
            break;
            }

        }

    return ETrue;
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgAnimationBase::GetAttrValueL()
    {
    iAttributeFlag = 1;
    switch ( iDataType )
        {
        case KSvgTypeLength:
        case KSvgTypeNumber:
            iTargetElement->GetAttributeFloat( iAttrId, iPropFloat );
        	iTargetElement->GetAttributeFloat( iAttrId, iOrgFloat );
        break;
        case KSvgTypeOpacity:
            iTargetElement->GetAttributeFloat( iAttrId, iPropFloat );
        iTargetElement->GetAttributeFloat( iAttrId, iOrgFloat );
        break;
        case KSvgTypeList:
        case KSvgTypePath:
            {
            // copy parent -> iOrgPath
            CGfxGeneralPath*parentPath = NULL;
           
			if ( iOrgPath )
			{
			delete iOrgPath;
			iOrgPath = NULL;
			}

			if ( iEndPath )
			{
			delete iEndPath;
			iEndPath = NULL;
			}
            
            iOrgPath = CGfxGeneralPath::NewL();
            iEndPath = CGfxGeneralPath::NewL();

            iTargetElement->GetAttributePath( iAttrId, parentPath );
            
            if(parentPath != NULL)
            {
            CGfxPathIterator*   itr;
            TGfxAffineTransform affineTransform;
            parentPath->GetPathIteratorL( &affineTransform, itr );
            CleanupStack::PushL( itr );
            iOrgPath->AppendL( itr );
            CleanupStack::PopAndDestroy( 1 ); // destroy itr

            iTargetElement->GetAttributePath( iAttrId, parentPath );
            CGfxPathIterator*   itr1;
            TGfxAffineTransform affineTransform1;
            parentPath->GetPathIteratorL( &affineTransform1, itr1 );
            CleanupStack::PushL( itr1 );
            iEndPath->AppendL( itr1 );
            CleanupStack::PopAndDestroy( 1 ); // destroy itr 
            }
            }
        break;
        case KSvgTypeColor:
            {
            TInt32  col;

            if ( iTargetElement->GetAttributeIntL( iAttrId, col ) ==
                 KErrNoAttribute )
                {
                iPropInt = 0;
                iAttributeFlag = 0;
                }
            else
                {
				// convert the color to openVG color
                iPropInt = col;
                iAttributeFlag = 1;
                }
            iTargetElement->GetAttributeIntL( iAttrId, iOrgInt );
            }
        break;
        case KSvgTypeTextAnchor:
        case KSvgTypeGradientUnits:
        case KSvgTypeSpreadMethod:
        case KSvgTypeInteger:
            iTargetElement->GetAttributeIntL( iAttrId, iPropInt );
        iTargetElement->GetAttributeIntL( iAttrId, iOrgInt );
        break;
        case KSvgTypeVisibility:
            iTargetElement->GetAttributeIntL( iAttrId, iOrgInt );
            if ( iTargetElement->GetAttributeIntL( iAttrId, iPropInt ) == KErrNoAttribute )
            {
                iPropInt = 0;
            iAttributeFlag = 0;
            }
        else
            {
            iAttributeFlag = 1;
            }
        break;
        case KSvgTypeDisplay:
            iTargetElement->GetAttributeIntL( iAttrId, iPropInt );
        iTargetElement->GetAttributeIntL( iAttrId, iOrgInt );
        break;


			// viewBox
        case KSvgTypeViewBox:
			{
			if(iTargetElement->ElemID() == KSvgSvgElement)
				{
				((CSvgSvgElementImpl*)iTargetElement)->GetViewBox( iOrgViewBox );
				}
			}
		break;

        default:
            iTargetElement->GetAttributeIntL( iAttrId, iOrgInt );
            iTargetElement->GetAttributeIntL( iAttrId, iPropInt );
        break;
        }
    }


void CSvgAnimationBase::ResetAttrValueL()
    {

    if (!iTargetElement)
    {
    	return;
    }

    switch ( iDataType )
        {
        case KSvgTypeLength:
        case KSvgTypeNumber:
            iTargetElement->SetAttributeFloatL( iAttrId, iOrgFloat );
            break;
        case KSvgTypeOpacity:
            iTargetElement->SetAttributeFloatL( iAttrId, iOrgFloat );
            break;
        case KSvgTypeColor:
            iTargetElement->SetAttributeIntL( iAttrId, iOrgInt );
            break;
        case KSvgTypeTextAnchor:
        case KSvgTypeGradientUnits:
        case KSvgTypeSpreadMethod:
        case KSvgTypeInteger:
            iTargetElement->SetAttributeIntL( iAttrId, iOrgInt );
            break;
        case KSvgTypeVisibility:
            iTargetElement->SetAttributeIntL( iAttrId, iOrgInt );
            break;
        case KSvgTypeDisplay:
            iTargetElement->SetAttributeIntL( iAttrId, iOrgInt );
            break;
        default:
            iTargetElement->SetAttributeIntL( iAttrId, iOrgInt );
            break;
        }
    }


//
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgAnimationBase::SetTarget( CSvgElementImpl* aTarget )
    {
		if (aTarget)
		{
    iTargetSet = ETrue;
    // Before anything else set the target to parent
    iTargetElement = aTarget;
    iTargetElement->iHasAnimationBase = (TInt)this;
		}
    }

//
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgAnimationBase::SetBeginByEventL( CSvgAnimationBase* aAnimElement,
                                         TInt32 aBeginTime, TBool aAddTime )
    {
    if ( iAnimRestart == KRestartWhenNotActive && iAnimStatus == KAnimActive )
        {
        return;     // Not start when active and restart="whenNotActive"
        }
    if ( iAnimRestart == KRestartNever && iHadBegun )
        {
        return;     // Not start when anim had begun and if restart="never"
        }


    aAnimElement->ResetAnimationL();

    if ( aAddTime )
    	{
	    iAnimTime->AddBeginTime( aBeginTime );
    	}
    else
		{
    	iAnimTime->ResetBeginTime( aBeginTime );
		}

    iAnimStatus = KAnimNotActive;

    if (iTargetElement)
        {
        iTargetElement->iHasAnimationBase = (TInt)this;
       	iTargetElement->SetIsAnimating( EFalse );
       	SetIsAnimating( EFalse );
        }
    iOverrideTime = ETrue;
    TInt32 lCurTime = ((CSvgDocumentImpl*)(iOwnerDocument))->CurrentTime();
    iAnimTime->SetNextBeginTime(lCurTime);
    iAbsoluteBeginTime = iAnimTime->BeginTime();

	if(iAnimTime->GetOriginalEndTime() == KTimeIndefinite)
		{
		iAnimTime->SetEndTime(KTimeIndefinite);
		}

    TUint32 lSimpleDuration = SimpleDuration();
    if( lSimpleDuration != KTimeIndefinite)
       {
       iAnimTime->SetEndTime(lSimpleDuration);
       }

    ((CSvgDocumentImpl*)iOwnerDocument)->AddEventBeginTime( aAnimElement, iAbsoluteBeginTime, aAnimElement->iTargetElement );
    ((CSvgDocumentImpl*)iOwnerDocument)->iInitSortList=ETrue;


    }


//
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgAnimationBase::SetEndByEvent( CSvgAnimationBase* /* aAnimElement */,
                                       TInt32 aEndTime )
    {
    iAnimTime->AddEndTime( aEndTime );

    iAnimTime->GetNextEndTime(iAnimTime->BeginTime());
	TUint32 lSimpleDur= SimpleDuration();
   	if(lSimpleDur != KTimeIndefinite)
   		{
	   	iAnimTime->SetEndTime( lSimpleDur );
	   	}

  	iIsEndEventSent = EFalse;
    }

// ---------------------------------------------------------------------------
// Blending methods aAlpha must be [0 - 255]
// ---------------------------------------------------------------------------
TFloatFixPt CSvgAnimationBase::BlendFloat( TInt32 aAlpha, TFloatFixPt aV1, TFloatFixPt aV2 )
    {
	#ifdef SVG_FLOAT_BUILD
    TFloatFixPt percent = ((float)aAlpha)/255.0f;
    TFloatFixPt percentValue = ( aV2 - aV1 ) * percent;
    return aV1 + percentValue;
	#else
    TUint32 v1  = aV1.RawData() >> 8;
    TUint32 v2  = aV2.RawData() >> 8;
    return TFloatFixPt( BlendInt( aAlpha, v1, v2 ) << 8, ETrue );
#endif
    }

// ---------------------------------------------------------------------------
// Blending methods aAlpha must be [0 - 255]
// ---------------------------------------------------------------------------
TInt32 CSvgAnimationBase::BlendInt( TInt32 aAlpha, TInt32 aV1, TInt32 aV2 )
    {
    return ( (aV2 - aV1) * (((TReal32)aAlpha)/((TReal32)255)) ) + aV1 ;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TUint32 CSvgAnimationBase::BlendColor( TInt32 aAlpha, // Interpolation factor
                                                      //  between [0-255]
        TUint32 aV1, // Initial Colour value in RGB format
        TUint32 aV2 ) // Final Colour value
    {
    TUint32 r, g, b; 
    TInt32 c1, c2; 

    // aAlpha is a value between 0-255 which indicates current interpolated
    // value of the animation.
    // Since colour value is divided by 256 (for optimisation - bit shift 
    //  by 8 bits) alpha is set to 256 when it is 255. 
    if( 255 == aAlpha ) 
        {
        aAlpha = 256;
        }

    // Compute Red component
    c1 = aV2 & KRmask; 
    c2 = aV1 & KRmask; 
    r = ( ( ( c1 - c2 ) * aAlpha + c2 * 256 ) >> 8 ) & KRmask; 

    // Compute Green Component
    c1 = aV2 & KGmask; 
    c2 = aV1 & KGmask; 
    g = ( ( ( c1 - c2 ) * aAlpha + c2 * 256 ) >> 8 ) & KGmask; 

    // Compute Blue component
    c1 = aV2 & KBmask; 
    c2 = aV1 & KBmask; 
    b = ( ( ( c1 - c2 ) * aAlpha + c2 * 256 ) >> 8 ) & KBmask; 

    // Combine r,g,b and return
    return r | g | b; 
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TUint32 CSvgAnimationBase::AddColor( TUint32 aV1, TUint32 aV2 )
    {
    TInt32 r, g, b;
    TInt32 r1, r2, g1, g2, b1, b2;
    TUint sign1, sign2;

    // WARNING, spare high order bits store sign of 8 bit components
    // on input values only the use is limited in scope to
    // SetAccumulateValues method
    // return value is unsigned & clamped
    // MSB better be blank for normal use

    sign1 = (TInt32)( aV1 & 0xff000000) >> 24;
    sign2 = (TInt32)( aV2 & 0xff000000) >> 24;
	r1 = (aV1 & 0x00ff0000) >> 16;
	r2 = (aV2 & 0x00ff0000) >> 16;
	g1 = (aV1 & 0x0000ff00) >> 8;
	g2 = (aV2 & 0x0000ff00) >> 8;
	b1 = (aV1 & 0x000000ff);
	b2 = (aV2 & 0x000000ff);

    if(sign1 & 0x04)
        {
        r1 *= -1;
        }
    if(sign1 & 0x02)
        {
        g1 *= -1;
        }
    if(sign1 & 0x01)
        {
        b1 *= -1;
        }
    if(sign2 & 0x04)
        {
        r2 *= -1;
        }
    if(sign2 & 0x02)
        {
        g2 *= -1;
        }
    if(sign2 & 0x01)
        {
        b2 *= -1;
        }

    r = ( ( r1 + r2 ) );
    g = ( ( g1 + g2 ) );
    b = ( ( b1 + b2 ) );

        // clamp for overflow & underflow
    if ( r > 255 )
        {
        r = 255;
        }
    if ( g > 255 )
        {
        g = 255;
        }
    if ( b > 255 )
        {
        b = 255;
        }

    if ( r < 0 )
        {
        r = 0;
        }
    if ( g < 0 )
        {
        g = 0;
        }

    if ( b < 0 )
        {
        b = 0;
        }
    return ( ( ( r & 0xFF ) << 16 ) | ( ( g & 0xFF ) << 8 ) | ( b & 0xFF ) );
        }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TUint32 CSvgAnimationBase::SubtractColor( TUint32 aV1, TUint32 aV2 )
    {
    TInt32 r, g, b;
    TInt32 r1, r2, g1, g2, b1, b2;
    TUint sign =  0x0;

    // WARNING, spare high order bits store sign of 8 bit components
    // the use is limited in scope to SetAccumulateValues method
    // the commented code would handle signed input but this isn't
    // used by calling code
    // output components are not clamped
	r1 = (aV1 & 0x00ff0000) >> 16;
	r2 = (aV2 & 0x00ff0000) >> 16;
	g1 = (aV1 & 0x0000ff00) >> 8;
	g2 = (aV2 & 0x0000ff00) >> 8;
	b1 = (aV1 & 0x000000ff);
	b2 = (aV2 & 0x000000ff);

    r = ( ( r1 - r2 ) );
    g = ( ( g1 - g2 ) );
    b = ( ( b1 - b2 ) );

    if ( r < 0 )
        {
        sign = 0x04;
        r *= -1;
        }

    if ( g < 0 )
        {
        sign |= 0x02;
        g *= -1;
        }

    if ( b < 0 )
        {
        sign |= 0x01;
        b *= -1;
        }
    return ( ( ( r & 0xFF ) << 16 ) | ( ( g & 0xFF ) << 8 ) | ( b & 0xFF ) );

    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt32      CSvgAnimationBase::GetNumberOfEvents(const TDesC& aValue)
    {
    TInt32 result=1;
    TStringTokenizer tkn ( aValue, KSEMICOLON );
    while( tkn.HasMoreTokens()  )
        {
        tkn.NextToken( );
        result++;
        }
    return result;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------

TInt32       CSvgAnimationBase::Duration()
    {
    return (TInt32)iAnimTime->DurationTime();
    }


void CSvgAnimationBase::SaveBeginTimeToList(TInt aTimeInMilliseconds)
    {
    iAnimTime->AddToInitialList(aTimeInMilliseconds);
    }
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------

void CSvgAnimationBase::ReInitializeAnimation()
    {


	iHadBegun= EFalse;
    iBeginTimeIndex = 0 ;
    iCurrentRepeatCount= 0;
    iInitDone = EFalse;
    iOverrideTime = EFalse;
    iAnimStatus = KAnimNotActive;

    if (iTargetElement)
        {
        iTargetElement->iHasAnimationBase = (TInt)this;
       	iTargetElement->SetIsAnimating( EFalse );
       	SetIsAnimating( EFalse );
        }

			//JSR226 Change in question
//    	if (iOwnerDocument && ((CSvgDocumentImpl*)iOwnerDocument)->Engine() && iTargetElement )
//    	{
    		//hope these are parrallel vectors
    		//for (TInt i=0; i < iAnimTime->BeginTimesCount(); i++)
        	//{
//        		if ( (iAnimTime->BeginTime() <= ((CSvgDocumentImpl*)iOwnerDocument)->Engine()->CurrentTIme() )
//        		  && ( iAnimTime->EndTime() >= ((CSvgDocumentImpl*)iOwnerDocument)->Engine()->CurrentTIme() ) )
//        		{
//        			iTargetElement->iAnimating = ETrue;
//        			iAnimating = ETrue;
//        		}
        	//}
//    	}
			//--end of change in question
//
	if(iAdditiveOrg == KAdditiveReplace)
		{
		iAdditive= KAdditivePass;
		}


	if (iAnimTime)
		{
		if (iIsUserSeek)
			iAnimTime->ReInitializeForSeek();
       else
			iAnimTime->ReInitialize();
		
	if( iAnimTime->BeginTimesCount()== 0 )
        {
         if(iIsBeginSet ) // begin on events
			  {
			  if(!iIsUserSeek)// for DOM Reusage only
				{
				iAnimTime->SetBeginTime(KTimeIndefinite);
				iAbsoluteBeginTime = KTimeIndefinite ;
				}
			  else
				{
				// if begin depends on event and repeat is there,
				//then iAbsoluteBeginTime != iBeginTime.
				if((TUint32)iAbsoluteBeginTime != iAnimTime->BeginTime() )
					iAnimTime->SetBeginTime(iAbsoluteBeginTime);
				}

			}
		 else
			{
            iAnimTime->SetBeginTime(0);
	    	 iAbsoluteBeginTime = 0 ;
			}
        }

     }
    }

// ---------------------------------------------------------------------------
// Functions used by Decoder
// ---------------------------------------------------------------------------


void CSvgAnimationBase::SetAttributeId(const TUint16 aAtrId)
    {
    iAttrId= aAtrId;
    }

void CSvgAnimationBase::SetAttributeType(const TUint8 aAtrType)
    {
    iDataType= (TUint16) aAtrType;
    }

void CSvgAnimationBase::SetBeginTimeList(RArray<TInt32>*& aList)
    {
    if (iAnimTime)
        {
        iAnimTime->SetBeginTimeList(aList);
        }
    }

void CSvgAnimationBase::SetBeginTime(TInt32 aTime)
    {
    if (iAnimTime)
        {
        iAnimTime->SetBeginTime(aTime);
        }
    }

void CSvgAnimationBase::SetAbsoluteBeginTime(TInt32 aTime)
    {
    iAbsoluteBeginTime= aTime;
    iIsBeginSet = ETrue;
    }

void CSvgAnimationBase::SetEventList(CArrayFixFlat<TEventListItem>*& aEventList)
    {
    if (iEventList)
        {
        iEventList->Reset();
        delete iEventList;
        iEventList= NULL;
        }
    iEventList= aEventList;
    }

void CSvgAnimationBase::SetEndTimeList(CArrayFixFlat<TEventListItem>*& aTimeList)
    {
    if (iEndTimeList)
        {
        iEndTimeList->Reset();
        delete iEndTimeList;
        iEndTimeList= NULL;
        }
    iEndTimeList= aTimeList;
    }

void CSvgAnimationBase::SetDurationTime(TInt32 aTime)
    {
    if (iAnimTime)
        {
        iAnimTime->SetDurationTime(aTime);
        }
    }

void CSvgAnimationBase::SetFromFlag()
    {
    iNoFrom= EFalse;
    }

void CSvgAnimationBase::SetFromInt(TInt32 aValue)
    {
    iFromInt= aValue;
    iOrgFromInt= aValue;
    }

void CSvgAnimationBase::SetFromFloat(TFloatFixPt aValue)
    {
    iFromFloat= aValue;
    iOrgFromFloat= aValue;
    }

void CSvgAnimationBase::SetFromViewBox(TGfxRectangle2D aValue)
    {
    iFromViewBox= aValue;
    iOrgFromViewBox= aValue;
    }

void CSvgAnimationBase::SetFromPathL(CGfxGeneralPath*& aPath)
    {
    if (iFromPath)
        {
        delete iFromPath;
        iFromPath= NULL;
        }
    iFromPath= aPath;

    if ( iOrgFromPath )
       {
       delete iOrgFromPath;
       iOrgFromPath = NULL;
       }

    iOrgFromPath = CGfxGeneralPath::NewL();
    iOrgFromPath->SetPointTypeArrayL(aPath->PointTypeArray());
    iOrgFromPath->SetPointCoordsArrayL(aPath->PointCoordsArrayAll());
    }

void CSvgAnimationBase::SetToFlag()
    {
    iHaveTo= ETrue;
    }

void CSvgAnimationBase:: SetToInt(TInt32 aValue)
    {
    iToInt= aValue;
    }

void CSvgAnimationBase:: SetToFloat(TFloatFixPt aValue)
    {
    iToFloat= aValue;
    }

void CSvgAnimationBase::SetToPath(CGfxGeneralPath*& aPath)
    {
    iToPath= aPath;
    }

void CSvgAnimationBase::SetToViewBox(TGfxRectangle2D aValue)
    {
    iToViewBox= aValue;
    }

CSvgElementImpl*& CSvgAnimationBase::GetTargetElem()
    {
        return iTargetElement;
    }

void CSvgAnimationBase::SetAnimFreeze()
    {
    iFill= KAnimFillFreeze;
    }

void CSvgAnimationBase::SetValuesFlag()
    {
    iHaveValues = ETrue;
    }

void CSvgAnimationBase::SetIntValuesArray( RArray<TInt32>*& aArray)
    {
    if (iValuesInt)
        {
        iValuesInt->Close();
        delete iValuesInt;
        }
        iValuesInt= aArray;
    }

void CSvgAnimationBase::SetFloatValuesArray( RArray<TFloatFixPt>*& aArray)
    {
    if (iValuesFloat)
        {
        iValuesFloat->Close();
        delete iValuesFloat;
        }

        iValuesFloat= aArray;
    }

void CSvgAnimationBase::SetViewBoxValuesArray( RArray<TGfxRectangle2D>*& aArray)
    {
    if (iValuesViewBox)
        {
        iValuesViewBox->Close();
        delete iValuesViewBox;
        }

        iValuesViewBox= aArray;
    }

void CSvgAnimationBase::SetPathValuesArray( RPointerArray<CGfxGeneralPath>*& aArray)
    {
    if (iValuesPath)
        {
        iValuesPath->Close();
        delete iValuesPath;
        }

        iValuesPath= aArray;
    }

void CSvgAnimationBase::ResetFloatValuesArray()
    {
    if (iValuesFloat)
        {
        iValuesFloat->Reset();
        }
    }

void CSvgAnimationBase::SetRepeatCount(TReal32 aCount)
    {
        // fix for repeatCount
        if(aCount <= 0)
            {
            iRepeatCount = 1;
            }
        else
            {
            iRepeatCount= aCount;
            }
    iRepeatCountSet = ETrue;

    }

void CSvgAnimationBase::SetRepeatDuration(TUint32 aDur)
    {
    if(!iRepeatCountSet)
        {
        iRepeatCount = KRepeatCountMax;
        }

    if (iAnimTime)
        {
        iAnimTime->SetRepeatDurationTime(aDur);
        }
    }


void  CSvgAnimationBase::SetEndTime(TInt32 aTime)
    {
    if(iAnimTime)
        {
        iAnimTime->SetEndTime(aTime);
        }
    }

void CSvgAnimationBase::SetRestartMode(TRestartMode aMode)
    {
    iAnimRestart= aMode;
    }

void CSvgAnimationBase::SetAccumulate(TAccumulate aValue)
    {
    iAccumulate= aValue;
    }

void CSvgAnimationBase::SetAdditive(TAdditive aValue)
    {
    iAdditive= aValue;
    iAdditiveOrg= aValue;
    }

void CSvgAnimationBase::SetCalcMode(TAnimCalcMode aMode)
    {
    if (iAnimTime)
        {
        iAnimTime->SetCalMode(aMode);
        }
    }

void CSvgAnimationBase::SetKeyTimeArray(RArray<CSvgAnimTimeController::TKeyTime>*& aArray)
    {
    if (iAnimTime)
        {
        iAnimTime->SetKeyTimeArray(aArray);
        }
    }

void  CSvgAnimationBase::SetAnimTimeArray(RArray<TUint32>*& aArray)
    {
    if (iAnimTime)
        {
        iAnimTime->SetAnimTimeArray(aArray);
        }
    }

void CSvgAnimationBase::SetByFlag()
    {
    iHaveBy= ETrue;
    }

TBool CSvgAnimationBase::  GetFromFlag()
    {
    return iNoFrom;
    }

TUint16 CSvgAnimationBase::GetAttributeId()
    {
    return iAttrId;
    }

void CSvgAnimationBase::SetKeyTimeFlag()
    {
    iKeyTimesPresent= ETrue;
    }

TInt  CSvgAnimationBase::GetAttributeFloat( const TInt aNameId,
                                                       TFloatFixPt& aValue )
    {
 switch(aNameId)
    {
     case KAtrAttributeName:
            {
            aValue = (TFloatFixPt)(TInt)(iAttrId);
            break;
    }
    case KAtrBegin:
            {
            aValue = (TInt32)iAnimTime->GetBeginTime( iBeginTimeIndex );
            break;
            }
    case KAtrDur:
    {
            aValue = (TFloatFixPt)(TInt) (iAnimTime->DurationTime());
            break;
    }
    case KAtrRepeatCount:
    {
            aValue = (TInt)iRepeatCount;
            break;
            }
    case KAtrRepeatDur:
        {
            aValue = (TInt32) iAnimTime->RepeatDurationTime();
        break;
            }
    case KAtrEnd:
            {
            aValue = (TInt32)iAnimTime->EndTime();
            break;
            }
     case KAtrFrom:
         {
             if(! iNoFrom)
                {
                 switch(iDataType)
                    {   // Length float.
                      case KSvgTypeOpacity:
                      case KSvgTypeLength:
            {
                          aValue = iFromFloat;
            break;
            }
                      default: break;
                     }
                }
         }
     break;
     case KAtrTo:
            {
             if(iHaveTo)
                {
                 switch(iDataType)
                    {   // Length float.
                      case KSvgTypeOpacity:
                      case KSvgTypeLength:
            {
                          aValue = iToFloat;
            break;
            }
                      default: break;

                    }
                 }
             }
     break;
     case KAtrBy:
            {
            if(iHaveBy)
                {
                 switch(iDataType)
                    {   // Length float.
                      case KSvgTypeOpacity:
                      case KSvgTypeLength:
                          {
                          aValue = iToFloat - iFromFloat;
                          break;
                          }
                      default:
            break;
            }
                 }
            }
         break;
     default:
     	//this is for the JSR because it wants the original value returned instead of the animated
    	//version that element would return if it gets down to the default of this case statement
    	if (aNameId == iAttrId)
				{
					aValue = iOrgFloat;
					return KErrNone;
				}
			else
			{
     	return CSvgElementImpl::GetAttributeFloat( aNameId,aValue );
    	}
    }
    return KErrNone;
            }

TInt CSvgAnimationBase::SetAttributeFloatL( const TInt aNameId,
                                                        const TFloatFixPt aValue )
{
    switch(aNameId)
    {
        case KAtrAttributeName:
            {
            iAttrId = (TUint16)(TInt32)aValue;
            break;
            }
        case KAtrBegin:
            {
            iAnimTime->SetBeginTime( aValue );
            break;
            }
        case KAtrDur:
            {
            iAnimTime->SetDurationTime( aValue );
            break;
            }

    case KAtrRepeatCount:
            {
            if((TInt32)aValue <= 0)
                {
                iRepeatCount = 1;
                }
            else
                {
                iRepeatCount= (TUint16) (TInt32)aValue;
                }
            iRepeatCountSet = ETrue;
            break;
            }

        case KAtrRepeatDur:

            {
            iAnimTime->SetRepeatDurationTime( (TInt32)aValue );
            break;
            }
        case KAtrEnd:
            {
            iAnimTime->SetEndTime( aValue );
            break;
            }
     case KAtrFrom:
            {

                 switch(iDataType)
                    {   // Length float.
                      case KSvgTypeOpacity:
                      case KSvgTypeLength:
                          {
                          iFromFloat = aValue;
                          break;
                          }
                      default:
                          break;
                    }

            }
     break;
     case KAtrTo:
            {

                 switch(iDataType)
                    {   // Length float.
                      case KSvgTypeOpacity:
                      case KSvgTypeLength:
                          {
                          iToFloat=  aValue ;
                          break;
                          }
                      default:
                      break;
                      // color and integer values.

                    }

             }
     break;
     case KAtrBy:
            {
            switch(iDataType)
                {   // Length float.
                  case KSvgTypeOpacity:
                  case KSvgTypeLength:
                      {
                      iToFloat  =  iFromFloat + aValue;
                      break;
                      }
                    default:
                    break;
                }

            }
         break;
     default:
     {
		//JSR226 change in question.  This is so you can set the original value from the JSR226 side.
     	if (aNameId == iAttrId)
		{
			iOrgFloat = aValue;
		}

		return CSvgElementImpl::SetAttributeFloatL( aNameId,aValue );
     }
        }
    return KErrNone;

    }

TInt CSvgAnimationBase::SetAttributeIntL( const TInt aNameId,
                                                      const TInt32 aValue )
    {
    switch(aNameId)
        {
    case KAtrFrom:
        {
            switch(iDataType)
                {
                case KSvgTypeTextAnchor:
                case KSvgTypeColor:
                case KSvgTypeVisibility:
                case KSvgTypeGradientUnits:
                case KSvgTypeSpreadMethod:
                case KSvgTypeDisplay:
                    iFromInt = aValue;
                    iNoFrom = EFalse;
                break;

                default:return KErrNotFound;
                }
        }
        break;
    case KAtrTo:
        {
            switch(iDataType)
                {
                // only these are supported as Int
                case KSvgTypeTextAnchor:
                case KSvgTypeColor:
                case KSvgTypeVisibility:
                case KSvgTypeGradientUnits:
                case KSvgTypeSpreadMethod:
                case KSvgTypeDisplay:
                    iToInt = aValue;
                    iHaveTo = ETrue;
                break;
                default:return KErrNotFound;
                }
        }
        break;
    case KAtrBy:
            {
            // By is not supported.
            if(iDataType ==  KSvgTypeColor)
                {
                iToInt = (TInt32 ) AddColor( iFromInt, (TInt32)aValue);
                break;
                }
            }
        break;
        case KAtrAdditive:
            {
            iAdditive = (TUint8)aValue;
            iAdditiveOrg = (TUint8)aValue;
            break;
            }

        case KAtrAccumulate:
            {
            iAccumulate = (TUint8)aValue;
            break;
            }

        case KAtrCalcMode:
            {
            iAnimTime->SetCalMode( (TUint8)aValue );
            break;
            }

        case KAtrRestart:
            {
            iAnimRestart = (TUint8) aValue;
            break;
            }

        // this is confusing but in case of animation
        // elements the style attributes really do not matter.
        case KCSS_ATTR_FILL:
            {
            iFill = (TUint8) aValue;
            break;
            }

    default:
        return CSvgElementImpl::SetAttributeIntL( aNameId, aValue );
            }
 return KErrNone;
}

TInt CSvgAnimationBase::GetAttributeIntL( const TInt aNameId,
                                                     TInt32& aValue )
{
switch(aNameId)
    {
    case KAtrFrom:
        {
            if(!iNoFrom)
                {
                switch(iDataType)
                    {
                    case KSvgTypeTextAnchor:
                    case KSvgTypeColor:
                    case KSvgTypeVisibility:
                    case KSvgTypeGradientUnits:
                    case KSvgTypeSpreadMethod:
                    case KSvgTypeDisplay:
                        aValue = iFromInt;
                    break;

                    default:return KErrNotFound;
                    }
                }
        }
        break;
    case KAtrTo:
        {
            if(iHaveTo)
                {
                switch(iDataType)
                    {
                    case KSvgTypeTextAnchor:
                    case KSvgTypeColor:
                    case KSvgTypeVisibility:
                    case KSvgTypeGradientUnits:
                    case KSvgTypeSpreadMethod:
                    case KSvgTypeDisplay:
                        aValue = iToInt;
                    break;
                    default:return KErrNotFound;
                    }
                }
        }
        break;
    case KAtrBy:
            {
            // By is not supported.
            if(iDataType == KSvgTypeColor)
                {
                aValue = (TInt32 ) SubtractColor( iToInt, iFromInt );
                }
            }
        break;
    case KAtrAdditive:
            {
            aValue = iAdditive ;
            break;
            }

    case KAtrAccumulate:
            {
            aValue = iAccumulate ;
            break;
            }
    case KAtrCalcMode:
            {
            aValue = iAnimTime->CalMode();
            break;
            }
    case KAtrRestart:
            {
            aValue =  iAnimRestart ;
            break;
            }
    case KCSS_ATTR_FILL:
            {
            aValue =  iFill ;
            break;
            }
        default:
            return CSvgElementImpl::GetAttributeIntL( aNameId, aValue );
        }
    return KErrNone;
    }

void CSvgAnimationBase::ResetL(MSvgEvent* aEvent, CSvgAnimationBase* aAnimElement)
	{

	TSvgTimerEvent* timerEvent  			= ( TSvgTimerEvent* ) aEvent;
	TInt32     lCurrentTime 				= timerEvent->Time();
	iAnimTime->Reset();
	TInt32 lBeginTime 						=  iAbsoluteBeginTime;
	
	//If Stop was called Dynamic endtime list(iEndTimeList) was reset to iInitialEndTimesList
	// This needs recalculation of iEndTime inside animTimeController
	TInt32 lSimpleDur						= SimpleDuration();
	if( lSimpleDur != KTimeIndefinite)
        {
        iAnimTime->SetEndTime(lSimpleDur);
        }
	
	TInt32 lDuration 						= iAnimTime->DurationTime();

	// reset back to the original values.
	aAnimElement->ResetReferenceElementL();

	// Set the Additive;
	iAdditive = iAdditiveOrg;

	if(lCurrentTime >= lBeginTime && lCurrentTime < lSimpleDur)
		{
		// set the new status.
		iAnimStatus = KAnimActive;
		iHadBegun = ETrue;

		// calculate the CurrentRepeatCount.
		if(lDuration> 0)
			{
			iCurrentRepeatCount = (TUint16)((lCurrentTime - lBeginTime ) / lDuration);

			// check it if it is bigger than the allowed repeatCount.
			if(iCurrentRepeatCount > iRepeatCount)
				{
				// truncate the repeatCount
				iCurrentRepeatCount = (TUint16)iRepeatCount;
				}
			}
		else
			{
			// No duration means repeatCount is ignored.
			iCurrentRepeatCount = 0;
			}

		if(aAnimElement->ElemID()== KSvgAnimateTransformElement ||aAnimElement->ElemID()== KSvgAnimateMotionElement )
			{
			if(aAnimElement->ElemID() == KSvgAnimateMotionElement)
				{
				aAnimElement->ResetAnimationL();
				}
			aAnimElement->SetAccumulateValuesForSetMediaTime();
			}
		// set the begin time.
		iAnimTime->SetBeginTime(lBeginTime + (iCurrentRepeatCount*lDuration));

		// Send the begin event.
		((CSvgDocumentImpl*)iOwnerDocument)->Engine()->NotifyAnimationStarted();
		TSvgInternalEvent   Beginevent   ( ESvgEventBeginEvent, this, lBeginTime );
		((CSvgDocumentImpl*)iOwnerDocument)->Engine()->ProcessEventL( 
		    ( CSvgDocumentImpl* ) OwnerDocument(), &Beginevent );


		// Send Repeat Events.
		if(iAnimTime->DurationTime() != KTimeIndefinite)
			{
			TUint16 lTotalRepeatEvents = (TUint16)iCurrentRepeatCount;
			for(TUint16 i= 0; i < lTotalRepeatEvents; i++)
				{
				TUint32 lDur = (TUint32)iAnimTime->DurationTime();
				 TInt lCount = (TUint16)(i+1);
	             TInt32 lRepeatTime = lBeginTime + (TInt32)(lDur*lCount);
				 TSvgInternalEvent   Repeatevent   ( ESvgEventRepeatEvent, this, lRepeatTime );
				((CSvgDocumentImpl*)iOwnerDocument)->Engine()->ProcessEventL( 
				    ( CSvgDocumentImpl* ) OwnerDocument(), &Repeatevent );
				}
			}
		}
	else if(lCurrentTime >= lSimpleDur)
		{
		// made active to calculate the freeze value.
		iAnimStatus 			= KAnimActive;
		iHadBegun 				= ETrue;
		TBool lAcuumulate 		= EFalse;
		if(lDuration> 0)
			{
			iCurrentRepeatCount = (TUint16)((lCurrentTime - lBeginTime ) / lDuration);

			// check below is for the correct repeatCount setting this is
			// necessary for accumulate values calculation.
			if(iCurrentRepeatCount >= iRepeatCount || iRepeatCount == KRepeatCountMax)
				{
				// this means that the repeatCount was not specified so it is not supposed to send the
				// repeatEvents.
				if(iRepeatCount == 1)
					{
					iCurrentRepeatCount = 0;
					}
				else
					{
					if(iRepeatCountSet)
						{
						// repeatCount was specified.
						if(iRepeatCount != KRepeatCountMax)
							{
							// repeatCount is not "indefinite"
							iCurrentRepeatCount = (TUint16) iRepeatCount;
							}
						}
					else
						{
						// what if repeatDuration was specified.
						if(KTimeIndefinite != iAnimTime->RepeatDurationTime())
							{
							// repeatDuration is not "indefinite"
							iCurrentRepeatCount = (iAnimTime->RepeatDurationTime()) / lDuration;
							}
						}
					lAcuumulate = ETrue;
					}
				}
			}
		else
			{
			iCurrentRepeatCount = 0;
			}
		//take the last begin time so that accumulation happens correctly.
		// if "end" needs to be checked
		TInt32 lEnd 				= iAnimTime->GetOriginalEndTime();
		TInt   count				=0;
		TBool  lChangeRepeatCount 	= EFalse;
		if(KTimeIndefinite != lEnd)
			{
			for(; count <= iCurrentRepeatCount; count++)
				{
				// check is the repaetCount is more than the valid range.
				// depending on the end time.
				if((lBeginTime+(count*lDuration)) < lEnd)
					{

				 	}
				else
					{
					// original end time must be grater than the begin time to be valid.
					if(lEnd > lBeginTime)
						{
						// repeatCount needs to be changed.
						lChangeRepeatCount = ETrue;
						}
					break;

					}
				}
			}
		if(lChangeRepeatCount)
			{
			if(count >0)
				{
				iCurrentRepeatCount = count - 1;
				}
			else
				{
				iCurrentRepeatCount = 0;
				}
			}
		iAnimTime->SetBeginTime(lBeginTime + (iCurrentRepeatCount)*lDuration);

		// Decrement the repeatCount; this is because the accumulate will give
		//additional one repeatCouont for seeking beyond the end time.
		TReal32 RealRepeatCount = iCurrentRepeatCount;
		if(iCurrentRepeatCount > 0 && ( RealRepeatCount == iRepeatCount || iRepeatCount == KRepeatCountMax))
			{
			iCurrentRepeatCount--;
			}

		// check for animateTransforms and animateMotion for iAccumulate
		// which means that these need to accumulate separately.
		if(aAnimElement->ElemID()== KSvgAnimateTransformElement ||aAnimElement->ElemID()== KSvgAnimateMotionElement )
			{
			if(iAccumulate == KAccumSum && lAcuumulate)
				{
				// if it is animateMotion reset the current path indicaters.
				if(aAnimElement->ElemID() == KSvgAnimateMotionElement)
					{
					aAnimElement->ResetAnimationL();
					}
				// accumulate.
				aAnimElement->SetAccumulateValuesForSetMediaTime();
				}
			}

		// send the begin
		((CSvgDocumentImpl*)iOwnerDocument)->Engine()->NotifyAnimationStarted();
		TSvgInternalEvent   Beginevent   ( ESvgEventBeginEvent, this, lBeginTime );
		((CSvgDocumentImpl*)iOwnerDocument)->Engine()->ProcessEventL( 
		    ( CSvgDocumentImpl* ) OwnerDocument(), &Beginevent );
		// send the begin, repeat, and end Events.
		if(iAnimTime->DurationTime() != KTimeIndefinite)
			{
			TUint16 lTotalRepeatEvents = (TUint16)iCurrentRepeatCount;
			for(TUint16 i= 0; i < lTotalRepeatEvents; i++)
				{
				TUint32 lDur = (TUint32)iAnimTime->DurationTime();
				 TInt lCount = (TUint16)(i+1);
	             TInt32 lRepeatTime = lBeginTime + (TInt32)(lDur*lCount);
				 TSvgInternalEvent   Repeatevent   ( ESvgEventRepeatEvent, this, lRepeatTime );
				((CSvgDocumentImpl*)iOwnerDocument)->Engine()->ProcessEventL( 
				    ( CSvgDocumentImpl* ) OwnerDocument(), &Repeatevent );
				}
			}

		// send end events.
		if(lSimpleDur != (TInt32)KTimeIndefinite)
			{
			((CSvgDocumentImpl*)iOwnerDocument)->Engine()->NotifyAnimationEnded();
			TSvgInternalEvent   Endevent   ( ESvgEventEndEvent, this, lSimpleDur );
			((CSvgDocumentImpl*)iOwnerDocument)->Engine()->ProcessEventL( 
			    ( CSvgDocumentImpl* ) OwnerDocument(), &Endevent );
			TSvgTimerEvent   lEndTime1(lSimpleDur);
			iIsEndEventSent = ETrue;
			// calculate the freeze values.
			if(iFill == KAnimFillFreeze)
				{
				// if it has to freeze than calculate the final values.
				aAnimElement->AnimProcL(&lEndTime1);

				// this is to avoid it going in the AnimProcL again. Which will change
				// the freeze values.
				// THIS MUST BE DONE ONLY WHEN THE repeatCount is fractional.
				TReal32 lFractReapeatDur = 0;
				if(KTimeIndefinite != iAnimTime->RepeatDurationTime())
					{
					// this is to check whether this matches a fractional repeatCount.
					lFractReapeatDur = iAnimTime->RepeatDurationTime();
					lFractReapeatDur = (lFractReapeatDur - (lDuration*(iCurrentRepeatCount+1))) ;
					}
				if((iRepeatCount - TInt(iRepeatCount)>0 || lFractReapeatDur > 0 )
					|| ((iAnimTime->BeginTime() + iAnimTime->DurationTime()) > lSimpleDur &&
					iAnimTime->BeginTime()!= lSimpleDur))
				 	{
				 	// do not go to AnimProcL only once.
					iDoFreeze = ETrue;
					}
				}
			else
				{
				// this should be ok . But how do we get the initial value.
				aAnimElement->SetToInitialValueL();
				// do not go to animProcL;
				iDoFreeze = ETrue;
				}
			}
		}
	}

void  CSvgAnimationBase::SetOriginalValues_DOMReuse()
	{

	if(iAnimTime)
		{
		iAnimTime->SetOriginalValues_DOMReuse();
		}
	}
CGfxGeneralPath*  CSvgAnimationBase::GetPathAttribute(TInt aAttributeId)
{
if(iDataType == KSvgTypePath)
	{
	switch(aAttributeId)
		{
		case KAtrTo:	return iToPath;
		case KAtrFrom: return iOrgFromPath;
		default :
			return NULL;
		}
	}
return NULL;
}
void CSvgAnimationBase::SetPathAttribute(TInt aAttributeId, CGfxGeneralPath* aPathHandle)
{
 if(iDataType == KSvgTypePath)
	{
	switch(aAttributeId)
		{
		case KAtrTo:
			{
				if(iToPath == aPathHandle)
					{
					return;
					}
			delete iToPath;
			iToPath = aPathHandle;
			}
			break;
		case KAtrFrom:
			{

				if(iOrgFromPath == aPathHandle)
					{
					return;
					}

			delete iOrgFromPath;
			iOrgFromPath = aPathHandle;

			// clone it .
			delete iFromPath;
			iFromPath = NULL;

			/************************************/
			TRAPD(error ,iFromPath = CGfxGeneralPath::NewL());
			if(error == KErrNone)
				{
				RArray<TUint32>* lTypes = aPathHandle->PointTypeArray();
                RArray<TFloatFixPt>* lCoords = aPathHandle->PointCoordsArrayAll();
                TRAP_IGNORE( iFromPath->SetPointTypeArrayL(lTypes) );
                TRAP_IGNORE( iFromPath->SetPointCoordsArrayL(lCoords) );
                }
			/************************************/

			}
			break;
		default :
			break;
		}
	}

}


TUint8 CSvgAnimationBase::GetAnimStatus()
{
	return iAnimStatus;
}

// added for forward referencing
TInt  CSvgAnimationBase::SetRefElemById(const TDesC& aName)
	{
	//If iTargetElement is Set to Null means that some junk Href was given
	 iTargetElement = ( CSvgElementImpl*)((CSvgDocumentImpl*)iOwnerDocument)->GetElementById( aName );

     if(iTargetElement==NULL)
     	{
     	return KErrNotFound;
     	}
     else
     	{
    iTargetElement->iHasAnimationBase = (TInt)this;
		TRAPD(lError,GetAttrValueL());
		if ( lError != KErrNone )
		   {
		   // ignore trap error
	       }
		return KErrNone;
		}
	}

void CSvgAnimationBase::CopyL(CSvgAnimationBase* newElement)
	{
	if(newElement)
	{

	// copy this pointer.
	this->iAnimTime->CopyL(newElement->iAnimTime);

	if(iEndPath)
	{
	newElement->iEndPath = CGfxGeneralPath::NewL();

    RArray<TUint32>* lTypes = this->iEndPath->PointTypeArray();
    RArray<TFloatFixPt>* lCoords = this->iEndPath->PointCoordsArrayAll();
    newElement->iEndPath->SetPointTypeArrayL(lTypes);
    newElement->iEndPath->SetPointCoordsArrayL(lCoords);
    }
	if(iOrgPath)
	{
	newElement->iOrgPath = CGfxGeneralPath::NewL();

    RArray<TUint32>* lTypes = this->iOrgPath->PointTypeArray();
    RArray<TFloatFixPt>* lCoords = this->iOrgPath->PointCoordsArrayAll();
    newElement->iOrgPath->SetPointTypeArrayL(lTypes);
    newElement->iOrgPath->SetPointCoordsArrayL(lCoords);
    }
	if(iFromPath)
	{
	newElement->iFromPath = CGfxGeneralPath::NewL();

    RArray<TUint32>* lTypes = this->iFromPath->PointTypeArray();
    RArray<TFloatFixPt>* lCoords = this->iFromPath->PointCoordsArrayAll();
    newElement->iFromPath->SetPointTypeArrayL(lTypes);
    newElement->iFromPath->SetPointCoordsArrayL(lCoords);
    }
	if(iToPath)
	{
	newElement->iToPath = CGfxGeneralPath::NewL();

    RArray<TUint32>* lTypes = this->iToPath->PointTypeArray();
    RArray<TFloatFixPt>* lCoords = this->iToPath->PointCoordsArrayAll();
    newElement->iToPath->SetPointTypeArrayL(lTypes);
    newElement->iToPath->SetPointCoordsArrayL(lCoords);
    }
	if(iOrgFromPath)
	{
	newElement->iOrgFromPath = CGfxGeneralPath::NewL();

    RArray<TUint32>* lTypes = this->iOrgFromPath->PointTypeArray();
    RArray<TFloatFixPt>* lCoords = this->iOrgFromPath->PointCoordsArrayAll();
    newElement->iOrgFromPath->SetPointTypeArrayL(lTypes);
    newElement->iOrgFromPath->SetPointCoordsArrayL(lCoords);
    }
	if(iValuesFloat)
		{

		TInt lCount = iValuesFloat->Count();
		newElement->iValuesFloat->Reset();
		for(TInt i=0; i<lCount; i++)
            {
            newElement->iValuesFloat->AppendL(iValuesFloat->operator[](i));
			}
		}
	if(iValuesInt)
		{
		TInt lCount = iValuesInt->Count();
		newElement->iValuesInt->Reset();
		for(TInt i=0; i<lCount; i++)
			{
			User::LeaveIfError(newElement->iValuesInt->Append(iValuesInt->operator[](i)));
			}
		}

	//iValuesPath
    if(iValuesPath)
		{
		TInt lCount = iValuesPath->Count();
		newElement->iValuesPath->Reset();
		for(TInt i=0; i<lCount; i++)
			{
			CGfxGeneralPath* lOrgPath = (this->iValuesPath)->operator[](i);
			RArray<TUint32>* lTypes = lOrgPath->PointTypeArray();
			RArray<TFloatFixPt>* lCoords = lOrgPath->PointCoordsArrayAll();

            CGfxGeneralPath* lClonePath = CGfxGeneralPath::NewL();
            CleanupStack::PushL(lClonePath);
            lClonePath->SetPointTypeArrayL(lTypes);
            lClonePath->SetPointCoordsArrayL(lCoords);
            newElement->iValuesPath->AppendL((CGfxGeneralPath*)lClonePath);
            CleanupStack::Pop(lClonePath);
			}
		}

	//iValuesViewBox
	if(iValuesViewBox)
		{
		TInt lCount = iValuesViewBox->Count();
		newElement->iValuesViewBox->Reset();
		for(TInt i=0; i<lCount; i++)
			{
			User::LeaveIfError(newElement->iValuesViewBox->Append(iValuesViewBox->operator[](i)));
			}
		}

	newElement->iEndOffset = iEndOffset;
	newElement->iRepeatCount = iRepeatCount;
	newElement->iRepeatEndValue = iRepeatEndValue;
	newElement->iAbsoluteBeginTime = iAbsoluteBeginTime;
	newElement->iAttributeFlag = iAttributeFlag;
	newElement->iEndInt = iEndInt;
	newElement->iPropInt = iPropInt;
	newElement->iOrgInt = iOrgInt;
	newElement->iFromInt = iFromInt;
	newElement->iToInt = iToInt;
	newElement->iOrgFromInt = iOrgFromInt;
	newElement->iIsBeginSet = iIsBeginSet;
	newElement->iPropFloat = iPropFloat;
	newElement->iOrgFloat = iOrgFloat;
	newElement->iFromFloat = iFromFloat;
	newElement->iToFloat = iToFloat;
	newElement->iOrgFromFloat = iOrgFromFloat;
	newElement->iEndFloat = iEndFloat;
	newElement->iBeginTimeIndex = iBeginTimeIndex;
	//newElement->iEndAccessKeyCode = iEndAccessKeyCode;
	newElement->iCurrentRepeatCount = iCurrentRepeatCount;
	newElement->iAttrId = iAttrId;
	newElement->iDataType = iDataType;
	newElement->iInitDone = iInitDone;
	newElement->iTargetSet = iTargetSet;
	newElement->iNoFrom = iNoFrom;
	newElement->iHaveTo = iHaveTo;
	newElement->iHaveBy = iHaveBy;
	newElement->iHaveValues = iHaveValues;
	newElement->iAnimStatus = iAnimStatus;

	//viewBox
	newElement->iOrgViewBox = iOrgViewBox;
	newElement->iFromViewBox = iFromViewBox;
	newElement->iToViewBox = iToViewBox;
	newElement->iOrgFromViewBox = iOrgFromViewBox;
	newElement->iEndViewBox = iEndViewBox;

//	newElement->iEndReferenceEvent = iEndReferenceEvent;
	//////////////////////////////
	newElement->iAnimStatus = iAnimStatus;
	newElement->iFill = iFill;
	newElement->iHadBegun = iHadBegun;
	newElement->iAnimRestart = iAnimRestart;
	newElement->iAccumulate = iAccumulate;
	newElement->iAdditive = iAdditive;
	newElement->iAdditiveOrg = iAdditiveOrg;
	newElement->iOverrideTime = iOverrideTime;
	newElement->iRepeatCountSet = iRepeatCountSet;
	newElement->iDoFreeze = iDoFreeze;
	newElement->iKeyTimesPresent = iKeyTimesPresent;
	newElement->iCheckFirstAnim = iCheckFirstAnim;

	newElement->iIsUserSeek = EFalse;




	if(iEventList)
		{
		TInt lCount = iEventList->Count();


		newElement->iEventList->Reset();
		for(TInt i=0;i<lCount;i++ )
			{
			newElement->iEventList->AppendL(iEventList->operator[](i));
			(newElement->iEventList->operator[](i)).iReferenceElemeId = NULL;
			if(iEventList->operator[](i).iReferenceElemeId)
				{
				(newElement->iEventList->operator[](i)).iReferenceElemeId = (iEventList->operator[](i).iReferenceElemeId)->AllocL();
				}
			else
				{
				(newElement->iEventList->operator[](i)).iReferenceElemeId = NULL;
				}
			}
		}


	if(iEndTimeList)
		{
		TInt lCount = iEndTimeList->Count();
		newElement->iEndTimeList->Reset();
		for(TInt i=0;i<lCount;i++ )
			{
			newElement->iEndTimeList->AppendL(iEndTimeList->operator[](i));
			(newElement->iEndTimeList->operator[](i)).iReferenceElemeId = NULL;
			if(iEndTimeList->operator[](i).iReferenceElemeId)
				{
				(newElement->iEndTimeList->operator[](i)).iReferenceElemeId = (iEndTimeList->operator[](i).iReferenceElemeId)->AllocL();
				}
			else
				{
				(newElement->iEndTimeList->operator[](i)).iReferenceElemeId = NULL;
				}
			}
		}
	}

	}
// Call stack on Stop and Play
// CSvgEngineInterfaceImpl::Replay
// CSvgEngineImpl::SetMediaTime
// CSvgEngineImpl::SeekEngine
// CSvgTimeContainer::UserSeek
// CSvgDocumentImpl::Reset
// CSvgEventHandler::Reset	
// CSvgEventHandler::DeactivateAnimations 
// CSvgAnimateElementImpl::DeactivateAnimation
void CSvgAnimationBase::DeactivateAnimation(CSvgAnimationBase* aElement)
{
	if(!aElement)return;

	iDoFreeze = EFalse;

	// this should work for multiple begins.
	if(iEventList->Count()> 0)
		{
		if(iEventList->operator[](0).iEvent != ESvgEventNone || iEventList->Count() > 1)
			{
			// if it is not absolute.
			// reset this to initials.
			iAbsoluteBeginTime = KTimeIndefinite;
			iHadBegun= EFalse;
			iBeginTimeIndex = 0 ;
			iCurrentRepeatCount= 0;
			iInitDone = EFalse;
			iOverrideTime = EFalse;
			iAnimStatus = KAnimNotActive;
			iAnimTime->SetBeginTime(KTimeIndefinite);

			// reset the time related stuffs.
			iAnimTime->ReInitialize();

			// this is necessary.
			if(iAdditiveOrg == KAdditiveReplace)
				{
				iAdditive= KAdditivePass;
				}
			// this is a virtual function.
			aElement->ReInitializeAnimation();
			}
		}

	// Set the end times to indefinite.
	if(iEndTimeList->Count()> 0   )
		{
		if(iEndTimeList->operator[](0).iEvent != ESvgEventNone )
			{
			iAnimTime->SetEndTimesIndefinite();
			}
		}
}

TBool CSvgAnimationBase::ParseViewBoxValue(const TDesC& aValue, TGfxRectangle2D& aRect)
	{

	TLex input( aValue );
	TReal32 vBoxX=0;
	TReal32 vBoxY=0;
	TReal32 vBoxW=0;
	TReal32 vBoxH=0;

    input.SkipSpace();
    if (input.Val( vBoxX, '.' ) != KErrNone)
       	 vBoxX= 0;

    if( input.Peek() == ',' )
       	input.Inc();
    input.SkipSpace();
    if( input.Peek() == ',' )
       	input.Inc();

    if (input.Val( vBoxY, '.' ) != KErrNone)
       	 vBoxY= 0;


    if( input.Peek() == ',' )
       	input.Inc();
    input.SkipSpace();
    if( input.Peek() == ',' )
       	input.Inc();

    if (input.Val( vBoxW, '.' ) != KErrNone)
      	 vBoxW= 0;

    if( input.Peek() == ',' )
       	input.Inc();
    input.SkipSpace();
    if( input.Peek() == ',' )
       	input.Inc();

    if (input.Val( vBoxH, '.' ) != KErrNone)
       	 vBoxH= 0;

     aRect= TGfxRectangle2D( vBoxX, vBoxY, vBoxW, vBoxH );
	 return ETrue;
	}
void CSvgAnimationBase::CheckForEndTimesAndFreezeL(CSvgAnimationBase* aElement)
{
	if(iAnimStatus == KAnimFinished) // KAnimFinished
        {
      	    if(!iDoFreeze)
      	    	{
      	    	iDoFreeze = ETrue;
      	    	return;
      	    	}
  		    if ( iFill == KAnimFillFreeze )
		        {
		        aElement->SetToEndValueL();
		        }

        }
}

// function for decoder
void CSvgAnimationBase::AddEndTime( TInt32 aEndTime )
    {
     if(iAnimTime)
		 iAnimTime->AddEndTime(aEndTime);
    }

void CSvgAnimationBase::SetAccumulateValuesForSetMediaTime()
{
}

TBool CSvgAnimationBase::IsSelfDependentForBegin()
{

	/*********************************************************************

	 This is used only after setmediaTime is done on the svg DOCUMENT.
	 This function is to tell whether the elements begin depends on its end.

	*********************************************************************/

	if(iEventList->Count()> 0)
		{
		TInt lCount = iEventList->Count();
		for(TInt i=0; i< lCount; i++)
			{
			if(iEventList->operator[](i).iTargetElement == this &&
			   iEventList->operator[](i).iEvent == ESvgEventEndEvent)
				{
				return ETrue;
				}
			}
		}

	return EFalse;
}
 void CSvgAnimationBase::SetFromValuesL()
{
	/*
	* this is to make sure that the iOrg values do not  get modified once set.
	*/
	 switch ( iDataType )
        {
        case KSvgTypeOpacity:
        case KSvgTypeLength:
	        {
	        // This is to handle all cases, was only fill="freeze" cases.
	        if (iNoFrom)
	            {
	            TFloatFixPt    lTempValue;
		        iTargetElement->GetAttributeFloat( iAttrId, lTempValue );
	            iFromFloat = lTempValue;
	            }
	        }
        break;

        case KSvgTypeList:
        case KSvgTypePath:

        	{

        	if (iNoFrom )
	            {
	            if ( iFromPath )
                    {
                    delete iFromPath;
                    iFromPath = NULL;
                    }
                CGfxGeneralPath*parentPath;
	            iFromPath = CGfxGeneralPath::NewL();
	            iTargetElement->GetAttributePath( iAttrId, parentPath );
	            CGfxPathIterator*   itr;
	            TGfxAffineTransform affineTransform;
	            parentPath->GetPathIteratorL( &affineTransform, itr );
	            CleanupStack::PushL( itr );
	            iFromPath->AppendL( itr );
	            CleanupStack::PopAndDestroy( 1 ); // destroy itr
             	}

        	}
        	break;
		/*
		 *   The above algorithm can be applicable to color as well.
		 *   Results are not same as adobe.
		 */
        case KSvgTypeColor:

        break;

        // following are all strings data types.
        case KSvgTypeTextAnchor:
        case KSvgTypeGradientUnits:
        case KSvgTypeSpreadMethod:
        case KSvgTypeInteger:
        case KSvgTypeDisplay:
        case KSvgTypeVisibility:
        	if (iAttributeFlag)
        	if ( iNoFrom )
	            {
	            iTargetElement->GetAttributeIntL( iAttrId, iFromInt);
	            }
        break;

		// viewBox
        case KSvgTypeViewBox:
			{
			if(iTargetElement->ElemID() == KSvgSvgElement)
				{
				if (iNoFrom)
					{
					TGfxRectangle2D   lTempVierwBox;
					((CSvgSvgElementImpl*)iTargetElement)->GetViewBox( lTempVierwBox );
					iFromViewBox = lTempVierwBox;
					}
				}
			}
			 break;

        default:
        break;
        }
}


TInt32 CSvgAnimationBase::GetAbsoluteBeginTime()
    {
    return iAbsoluteBeginTime;
    }

TInt32 CSvgAnimationBase::GetEndTime()
    {
    return iAnimTime->EndTime();
    }


void CSvgAnimationBase::CheckBeginTime()
    {

    if (iAnimTime->BeginTimesCount() == 0 && iAnimTime->BeginTime() == 0)
        {
        iAnimTime->AddBeginTime(0);
        iAnimTime->AddToInitialList(0);
        }
    }

void CSvgAnimationBase::StoreRepeatId( const TDesC& aValue, TBool aRepeatWasInBeginAtt )
{
	if (iRepeatId)
	{
		delete iRepeatId;
		iRepeatId = NULL;
	}

    iRepeatInBegin = aRepeatWasInBeginAtt;
    TRAPD( error, iRepeatId = aValue.AllocL() );
    if ( error != KErrNone )
        {
        #ifdef _DEBUG
        RDebug::Printf("CSvgAnimationBase::StoreRepeatId: iRepeatId = aValue.AllocL() leaves");
        #endif
        }
}

void CSvgAnimationBase::NotifyAnimationsRepeatingOnThisOneL( const TDesC* aId )
{
	// Get THhe Engine's Current Media Time.
	CSvgDocumentImpl* lDocument = ((CSvgDocumentImpl*)iOwnerDocument);

    if ( !aId || aId->Length() == 0 )
    return;

    TInt32 lEngineTime = ((CSvgDocumentImpl*)iOwnerDocument)->Engine()->CurrentTIme();

    TInt lAnimationCount = lDocument->iSvgAnimations.Count();
    for (TInt i=0; i < lAnimationCount; i++)
    {
    	CSvgAnimationBase* lAnimatingElement = lDocument->iSvgAnimations[i];
    	if ( lAnimatingElement->iRepeatId && (lAnimatingElement->iRepeatId->CompareF(*aId) == 0 ) )
    	{
    		if (lAnimatingElement->iRepeatInBegin)
    		{
    			lAnimatingElement->SetBeginByEventL( lAnimatingElement,lEngineTime, ETrue );
    		}
    		else
    		{
    			//it must have been in an end attribute
    			lAnimatingElement->SetEndByEvent(lAnimatingElement, lEngineTime );
    		}

    	}
    }
}

// ---------------------------------------------------------------------------
// void CSvgAnimationBase::FindColorDistanceL() 
//  This function computes the distance between colours for the purpose 
//  of animateColor element. Colours are treated as a 3D point 
//  with r, g, b acting as axes. It modifies the iValuesFloat array and each 
//  index contains the cummulative length(from start colour) of the segment 
//  with same index
// ---------------------------------------------------------------------------
void CSvgAnimationBase::FindColorDistanceL() 
    {
    TInt lcount = iValuesInt->Count(); 

    //Fix for MLIO-743FRR:check whether "values" attribute is present
    if ( lcount > 0 )
        {
	    iValuesFloat->AppendL( 0 ); 
        }
        
    TFloatFixPt len = 0; 

    for(TInt i = 1; i < lcount; i++) 
        { 
        TInt c1 = (*iValuesInt)[i-1]; 
        TInt c2 = (*iValuesInt)[i]; 
        TInt r1 = c1 & 0x00FF0000; 
        r1 = r1 >> 16; 
        TInt g1 = c1 & 0x0000FF00; 
        g1 = g1 >> 8; 
        TInt b1 = c1 & 0x000000FF; 
        
        TInt r2 = c2 & 0x00FF0000; 
        r2 = r2 >> 16; 
        TInt g2 = c2 & 0x0000FF00; 
        g2 = g2 >> 8; 
        TInt b2 = c2 & 0x000000FF; 
        
        TInt dr = r1 - r2; 
        TInt dg = g1 - g2; 
        TInt db = b1 - b2; 
        
        TFloatFixPt length = TFloatFixPt::Sqrt( dr * dr + dg * dg + db * db ); 
        //TFloatFixPt length = Math::Sqrt( dr * dr + dg * dg + db * db ); 
        len += length; 
        iValuesFloat->AppendL( len ); 
        } 
    } 

TBool CSvgAnimationBase::IsFillFreeze()
    {
        if ( iFill == KAnimFillFreeze )
            return ETrue;
        else
            return EFalse;
    }
void CSvgAnimationBase::ResetTimes()
    {
	iAnimTime->ResetBeginTime();
    iAnimTime->ResetEndTime();
    }

// End of file
