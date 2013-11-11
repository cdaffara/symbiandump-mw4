/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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

#include "Gfx2dGcOpenVG.h"
#include "GfxRoundRectangle2D.h"
#include "SVGMediaAnimationElementImpl.h"
#include "SVGDocumentImpl.h"
#include "SVGEngineImpl.h"
#include "SVGSchemaData.h"
#include "SVGEventHandler.h"

_LIT( KXlink, "xlink:href" );
_LIT( KFocusable, "focusable" );
_LIT(KInitialVisibility, "initialVisibility");
_LIT( KSvgFile, ".svg");
_LIT( KSvgbFile, ".svgb");
_LIT( KSvgzFile, ".svgz");
_LIT(msg, "Fail to open file for reading: ");
_LIT( KErrorMsg, "Failed to Load Svg Content: Read File Size Error." );
_LIT( KMemoryErrorMsg, "Failed to Load Svg Content: Out of memory");
_LIT( KReadErrorMsg, "Failed to Load Svg Content: Read File Data Error");
_LIT(KEngineFailed, "Failed to Create Svg Document: Out of Memory");
_LIT( KTmpDur, "dur" );
_LIT( KTmpDurMedia, "media" );

CSvgMediaAnimationElementImpl* CSvgMediaAnimationElementImpl::NewL(const TUint8
                                               aElemID,CSvgDocumentImpl* aDoc )
    {
    CSvgMediaAnimationElementImpl* self = CSvgMediaAnimationElementImpl::NewLC
                                                                (aElemID,aDoc);
    CleanupStack::Pop();
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgMediaAnimationElementImpl* CSvgMediaAnimationElementImpl::NewLC(const TUint8
                                              aElemID, CSvgDocumentImpl* aDoc )
    {
    CSvgMediaAnimationElementImpl* self = new ( ELeave ) CSvgMediaAnimationElementImpl( aDoc );
    CleanupStack::PushL( self );
    self->ConstructL(  aElemID,aDoc );
    return self;
    }

// -----------------------------------------------------------------------------
// CSvgMediaAnimationElementImpl::SetSyncBehavior
// Set the Synchronised behaviour for the animation element
// -----------------------------------------------------------------------------
//        
void CSvgMediaAnimationElementImpl::SetSyncBehavior( const TDesC& aValue )
    {
    CSvgMediaElementBase::SetSyncBehavior( aValue );
    // If document is created then set the sync value to the time container
    if ( iNewDocument )
        {
        iNewDocument->SetTCSyncBehavior( iSyncBehavior );
        }
    }

// -----------------------------------------------------------------------------
// CSvgMediaAnimationElementImpl::SetSyncTolerance
// Set the Synchronised Tolerance for the animation element
// -----------------------------------------------------------------------------
//        
void CSvgMediaAnimationElementImpl::SetSyncTolerance( const TDesC& aValue )
    {
    CSvgMediaElementBase::SetSyncTolerance( aValue );
    
    // If document is created then set the sync value to the time container
    if ( iNewDocument )
        {
        iNewDocument->SetTCSyncTolerance( iSyncTolerance );
        }
    }

// -----------------------------------------------------------------------------
// CSvgMediaAnimationElementImpl::SetSyncMaster
// Set the animation element as Synchronised Master
// -----------------------------------------------------------------------------
//        
void CSvgMediaAnimationElementImpl::SetSyncMaster( const TDesC& aValue )
    {
    CSvgMediaElementBase::SetSyncMaster( aValue );

    // If document is created then set the sync value to the time container
    if ( iNewDocument )
        {
        iNewDocument->SetTCSyncMaster( iSyncMasterConfig );
        }
    }
    
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgMediaAnimationElementImpl::ConstructL(  const TUint8 aElemID,
                                       CSvgDocumentImpl*  /* aDoc */ )
    {
	CSvgMediaElementBase::ConstructL( aElemID );
	iViewBoxImpl = CSvgFitToViewBoxImpl::NewL();
	iIsFocusable = ETrue;
	iDurMediaTime = ETrue;
    iSvgError = CSvgErrorImpl::NewL();
    if(!iSvgTransformable)
        iSvgTransformable = CSvgTransformableImpl::NewL();
    iChildSvgName = HBufC::NewL( 100 );
	#ifdef SVG_FLOAT_BUILD
    iX = ( 0 );
    iY = ( 0 );
	#else
    iX.operator = ( 0 );
    iY.operator = ( 0 );
	#endif
	
	#ifdef SVG_FLOAT_BUILD
   
    iScale = ( 0 );
	#else
    iScale.operator = ( 0 );
	#endif
	iIsOriginalRootTrSet = EFalse;
	if (OwnerDocument())
	{
	((CSvgDocumentImpl*)OwnerDocument())->AddInternalMouseListener( this );
	     ((CSvgDocumentImpl*)iOwnerDocument)->AddToEventReceiverListL(
                                               this, KSvgEventMaskExternalUI );
	}
	
    }

// ---------------------------------------------------------------------------
//  Constructor
// ---------------------------------------------------------------------------

CSvgMediaAnimationElementImpl::CSvgMediaAnimationElementImpl
                               ( CSvgDocumentImpl* aDoc ):CSvgMediaElementBase
                               ( aDoc ),iAnimationStatus(EPlayerStateIdle),iInitialVisibility(EAnimationNotVisible)
                          

    {
    iAttrId = KAtrMediaAnimationId;
	}
	
// ---------------------------------------------------------------------------
// perform a deep clone of this object
// ---------------------------------------------------------------------------
MXmlElement* CSvgMediaAnimationElementImpl::CloneL(MXmlElement* aParentElement)
    {
    CSvgMediaAnimationElementImpl* newElement = CSvgMediaAnimationElementImpl::
    NewL( this->ElemID(), ((CSvgDocumentImpl*)iOwnerDocument) );

    CleanupStack::PushL(newElement);
    newElement->iParentNode = aParentElement;
    // copy everything over
    this->CopyL(newElement);
    CleanupStack::Pop();
    return newElement;
    }
	
// ==========================================================================
// perform a deep copy of this object
// ==========================================================================
void CSvgMediaAnimationElementImpl::CopyL( CSvgMediaAnimationElementImpl* aDestElement )
    {
    if(aDestElement)
		{
	    // copy stuff from superclass
	    this->CSvgElementImpl::CopyL(aDestElement);
	    this->CSvgMediaElementBase::CopyL(aDestElement);
	    aDestElement->iWidth = iWidth;
        aDestElement->iHeight = iHeight;
        aDestElement->iInitialVisibility = iInitialVisibility;
        aDestElement->iDurMediaTime = iDurMediaTime;
        aDestElement->iX = iX;
        aDestElement->iY = iY;
        aDestElement->iScale = iScale;
        aDestElement->iOriginalRootTr = iOriginalRootTr;
        aDestElement->iIsOriginalRootTrSet = iIsOriginalRootTrSet;
        aDestElement->iAnimationStatus = iAnimationStatus;
        aDestElement->iTargetElement = ( CSvgElementImpl * ) ParentNode();
		aDestElement->iTargetSet = ETrue;
		TInt handle = 0;
        TInt engine = 0;
        TPtr    tPtr    = this->iChildSvgName->Des();
        aDestElement->PrepareDomChildSvg(tPtr,handle,engine);
		
	    }

    }
	
// ---------------------------------------------------------------------------
//  Destructor
// ---------------------------------------------------------------------------
CSvgMediaAnimationElementImpl::~CSvgMediaAnimationElementImpl()
    {
        
    delete iViewBoxImpl;
    delete iSvgError;
    delete iNewDocument;
    iSvgMouseEnteredElements.Close(); 
    delete iChildSvgName;        
    }

  
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgMediaAnimationElementImpl::SetAttributeL( const TDesC& aName,
                                          const TDesC& aValue )
    {
     CSvgElementImpl::SetAttributeL(aName,aValue);
            //added for the bug if no begin is given
    if ( !iTargetSet )
			{
			// default is parent element
			iTargetElement = ( CSvgElementImpl * ) ParentNode();
			iTargetSet = ETrue;
			}

    // Encapsulates both viewBox and preserveAspectRatio attributes.
    if ( iViewBoxImpl->SetViewBoxL( aName, aValue ) )
        {
                if ( iNewDocument && iNewDocument->RootElement() )
                    {
                        MSvgPreserveAspectRatio* lAspectRatio;
                        iViewBoxImpl->GetPreserveAspectRatio(lAspectRatio);
                        
                        if ( lAspectRatio )
                            {
                            ((CSvgSvgElementImpl *)
                             (iNewDocument->RootElement()))->SetPreserveAspectRatioL
                             (lAspectRatio->GetAlign(), lAspectRatio->GetMeetOrSlice());                
                            }    
                    }
                
                return KErrNone;
        }
    
    if ( aName == KXlink )
        {
            if ( aValue.Length() != 0 )
            {
            iChildSvgName = iChildSvgName->ReAlloc(aValue.Length());
            TPtr    tPtr    = iChildSvgName->Des();
            tPtr.Copy( aValue );
            TInt length = tPtr.Length();
            TPtrC tPtrCSvg = tPtr.Right(4);
            TPtrC tPtrCSvgX = tPtr.Right(5);
            
            if ( tPtrCSvg.Find(KSvgFile) == KErrNotFound && 
                            tPtrCSvgX.Find(KSvgbFile) == KErrNotFound  
                            && tPtrCSvgX.Find(KSvgzFile) == KErrNotFound  )
                {
                return KErrGeneral;
                }
            else
                {
                TInt handle = 0;
                TInt engine = 0;
                TPtrC fileName;
                PrepareDomChildSvg(tPtr,handle,engine);
               
                // Check if error occured
                if ( !iSvgError->HasError() )
                    {
                    // If document is valid then set parameters of child svg
                    // element from animation element 
                    
                    if ( iNewDocument && iNewDocument->RootElement() )
                        {
                        ((CSvgSvgElementImpl *)
                         (iNewDocument->RootElement()))->iWidthInPercentage = EFalse;
                        ((CSvgSvgElementImpl *)
                         (iNewDocument->RootElement()))->iHeightInPercentage = EFalse;
                        ((CSvgSvgElementImpl *)
                         (iNewDocument->RootElement()))->SetWidth((TFloatFixPt)iWidth);
                        ((CSvgSvgElementImpl *)
                         (iNewDocument->RootElement()))->SetHeight((TFloatFixPt)iHeight);
                       
                  // Set the Sync Values from animation element to time container
                        // Sync Behaviour
                        iNewDocument->SetTCSyncBehavior( iSyncBehavior );
                        // Sync Tolerance
                        iNewDocument->SetTCSyncTolerance( iSyncTolerance );
                        // Sync Mastership
                        iNewDocument->SetTCSyncMaster( iSyncMasterConfig );
                        
                        MSvgPreserveAspectRatio* lAspectRatio;
                        iViewBoxImpl->GetPreserveAspectRatio(lAspectRatio);
                        
                        if ( lAspectRatio )
                            {
                            ((CSvgSvgElementImpl *)
                             (iNewDocument->RootElement()))->
                             SetPreserveAspectRatioL(lAspectRatio->GetAlign(), 
                             lAspectRatio->GetMeetOrSlice());                
                            }
                   
                        }
                    }
                 else  
                    {
                      return KErrGeneral;
                    }
                }
              }
		}
	
    else if ( aName == KFocusable )
        {
            if ( aValue == _L("true") )
                iIsFocusable = ETrue;
            if ( aValue == _L("false") )
                 iIsFocusable = EFalse;
        }
    else if ( aName == KInitialVisibility )    
        {
                if ( aValue == _L("always") )
                {
                    iInitialVisibility =  EAnimationVisible;
                }
        }
    else if ( aName == KTmpDur && aValue != KTmpDurMedia )
           {
           iDurMediaTime = EFalse;
           return CSvgMediaElementBase::SetAttributeL(aName, aValue);
           }
    else 
	    {
   	    return CSvgMediaElementBase::SetAttributeL(aName, aValue);
	    }

	return KErrNone;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt  CSvgMediaAnimationElementImpl::SetAttributeDesL( const TInt aNameId,
											   const TDesC& aValue )
	{
	return CSvgMediaElementBase::SetAttributeDesL( aNameId, aValue );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgMediaAnimationElementImpl::GetAttributeFloat(const TInt aNameId,
                                                TFloatFixPt& aValue )
    {
     switch ( aNameId )
     {
     case KAtrX:
	   aValue = iX ;
	   break;

     case KAtrY:
	   aValue = iY ;
	   break;

     case KAtrWidth:
	   aValue = iWidth ;
	   break;

     case KAtrHeight:
	   aValue = iHeight ;
	   break;
     default:
       return CSvgMediaElementBase::GetAttributeFloat( aNameId, aValue );
     }
        
     return KErrNone;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgMediaAnimationElementImpl::SetAttributeFloatL( const TInt aNameId,
                                               TFloatFixPt aValue )
    {
      switch ( aNameId )
      {
      case KAtrX:
          iX = aValue;
          SetInitialCTM();
          break;

      case KAtrY:
          iY = aValue;
          SetInitialCTM();
          break;
          
      case KAtrWidth:
          iWidth = aValue;
          break;

      case KAtrHeight:
          iHeight= aValue;
          break;
      
      default:
         return CSvgMediaElementBase::SetAttributeFloatL( aNameId, aValue );
        }
      return KErrNone;
    }

// ---------------------------------------------------------------------------
// From MSvgEventReceiver
// ---------------------------------------------------------------------------

TBool CSvgMediaAnimationElementImpl::ReceiveEventL( MSvgEvent* aEvent )
    {
    if( CSvgElementImpl::IsSVGEnginePaused())
    {
    	return EFalse;
    }
    
    if ( !iNewDocument )
        {
        return EFalse;
        }
    // Process animation element's events first    
    TBool lSelfRedrawReqd = CSvgMediaElementBase::ReceiveEventProcL( aEvent, 
        this );
    
    CSvgElementImpl::ReceiveEventL(aEvent);    
    // Only External UI events are sent to the child document
    if ( ( aEvent->EventMask() & KSvgEventMaskExternalUI ) )
        {
        TInt lEvtTime = 0;
        // Save the time as the ProcessEvent modifies the time of the event
        // to the document's current time for keypress events
        if ( aEvent->EventType() == ESvgEngineEventKeyPress )
            {
            lEvtTime = ( ( MSvgUiKeyEvent * ) aEvent )->Time();
            }
        
        // Send the events to the child document
        ( ( ( CSvgDocumentImpl* )
            iOwnerDocument )->Engine() )->ProcessEventL( iNewDocument, 
            aEvent, EFalse );
  
        // Restore the time, so that when the event is propogated to 
        // further elements in the DOM tree, the time is in sync with
        // respect to current document time. 
        if ( aEvent->EventType() == ESvgEngineEventKeyPress )
            {
            ( ( MSvgUiKeyEvent * ) aEvent )->SetTime( lEvtTime );
            }
            
        }
    return lSelfRedrawReqd;
     
  
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------

void CSvgMediaAnimationElementImpl::ResetAnimationL()
    {
  	// Check if document is valid
	if ( !iNewDocument || !iNewDocument->RootElement() )
	    {
	    return;
	    }

        if (( iAnimStatus == KAnimActive ) ||
		( iAnimStatus == KAnimFinished) ||
		( iAnimStatus == KAnimEnd))
		{
		if (( iAnimationStatus == EPlayerStatePlaying ) ||
			( iAnimationStatus == EPlayerStateStop ))
			{
			// if previously audio is still playing, and asked to play again...
	            iNewDocument->SetCurrentTime( 0 );
                
                   iNewDocument->iAnimationResetNeeded = ETrue;
                
                if(this->iNewDocument && this->iNewDocument->iEventHandler)
                    {
                    this->iNewDocument->iEventHandler->SetCurrentTime(0);
                    }
            	
            	 TSvgTimerEvent timeEvent(0);
            	
            	
            	 iNewDocument->Reset( &timeEvent );
        	// Save the animation start tick so that
        	// child document's tick can be correctly offset
	    	iBeginTick = ( ( CSvgDocumentImpl* )
	    	    iOwnerDocument )->TimeContainer()->GetCurTick(); 
	    	iAnimationStatus = EPlayerStatePlaying;
			}
		}
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgMediaAnimationElementImpl::Reset( MSvgEvent* aEvent )
	{

	iIsUserSeek = ETrue;

	// call the animation base function.
	TSvgTimerEvent* timerEvent  = ( TSvgTimerEvent* ) aEvent;


	if ( (TInt32)timerEvent->Time() < iAbsoluteBeginTime )
		{
		// let it come to initial position.
		((CSvgDocumentImpl*)iOwnerDocument)->iInitialDrawFlag = ETrue;
		ReInitializeAnimation();
		// Seek to the initial position means that 
		// animation should also be drawn in initial position
    	if ( iNewDocument && iNewDocument->RootElement() )
    	    {
            iNewDocument->SetCurrentTime( 0 );
            iNewDocument->iAnimationResetNeeded = ETrue;

            if ( this->iNewDocument && this->iNewDocument->iEventHandler )
                {
                this->iNewDocument->iEventHandler->SetCurrentTime(0);
                }

            TSvgTimerEvent timeEvent(0);
            iNewDocument->Reset( &timeEvent );    	    
    	    }
		return;
		}
	TRAPD(error,CSvgMediaElementBase::ResetL( aEvent, this));

		if ( error != KErrNone )
		{
			// error processing not processed
			return;
		}
	}  

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------

TBool CSvgMediaAnimationElementImpl::DoAnimProcL(MSvgEvent* aEvent)
    {
        if ( !iNewDocument )
        {
            return EFalse;
        }
    return this->AnimProcL((MSvgTimerEvent*)aEvent);
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------

TBool CSvgMediaAnimationElementImpl::IsViewable()
    {
     if ( iInitialVisibility == EAnimationVisible )
        {
        return ETrue;
        }
        return EFalse;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgMediaAnimationElementImpl::Print( TBool /*aIsEncodeOn */)
    {
	    // No implementation.
    }  
       
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TBool CSvgMediaAnimationElementImpl::DrawL( CGfx2dGc* aGc, 
                                           CSvgElementImpl* /* aElement */)
    {
    // Check if document is valid
	if ( !iNewDocument || !iNewDocument->RootElement() || (TInt)iWidth==0 || 
	                                                      (TInt)iHeight==0 )
	    {
	    return EFalse;
	    }

    // if iInitialVisibility status is EAnimationInitialised then draw won't happen
    if ( iInitialVisibility == EAnimationNotVisible || 
                            iInitialVisibility == EAnimationVisible )
        {
        
            // visibility is set to initialised state to get font before actual draw
            if ( iInitialVisibility == EAnimationNotVisible )
                {
                    iInitialVisibility = EAnimationInitialised;    
                }
            
            TReal32 opacity = 1;
            TGfxRoundRectangle2D        Rectangle;
            TGfxAffineTransform aTr = GetCTM();
            ((CSvgSvgElementImpl *)(iNewDocument->RootElement()))->SetTransformList
                                                                   (iOriginalRootTr);
            ((CSvgElementImpl*)iNewDocument->RootElement())->UpdateCTM();   
            
            iNewDocument->iIsRendering = ETrue; 
            ((CGfx2dGcOpenVG*)aGc)->BindToMediaImageL(iScale * iWidth,iScale * iHeight);
            (iNewDocument->Engine())->DrawElementsL((CSvgElementImpl*)
                                      (iNewDocument->RootElement()));
            iNewDocument->iIsRendering = EFalse; 
            TGfxAffineTransform tMatrix;
            tMatrix = aTr;
            
            #ifdef SVG_FLOAT_BUILD
            tMatrix.Scale( ( TFloatFixPt( 1 ) / ( iScale ) ), 
                             ( TFloatFixPt( 1 ) / ( iScale ) ) );
            #else
            tMatrix.Scale( ( TFloatFixPt( 0x10000,ETrue ) / ( iScale ) ), 
                             ( TFloatFixPt( 0x10000,ETrue ) / ( iScale ) ) );
            #endif
            
            // draw happens only if iInitialVisibility status is EAnimationVisible
            // i.e. animation element has started its animation
            
            ((CGfx2dGcOpenVG*)aGc)->UnbindFromMediaImageL(tMatrix,opacity,150,150,iInitialVisibility);
            
            tMatrix.Concatenate(iOriginalRootTr);
            ((CSvgSvgElementImpl *)(iNewDocument->RootElement()))->SetTransformList
                                                                   (tMatrix);
            ((CSvgElementImpl*)iNewDocument->RootElement())->UpdateCTM();   
             return ETrue;
        }
        
      return EFalse;  
    }

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------    
void CSvgMediaAnimationElementImpl::SetInitialCTM()
    {
 
       TGfxAffineTransform aTr(1,0,0,1,iX,iY);
       SetTransformList(aTr);
    }

// ---------------------------------------------------------------------------
// CSvgMediaAnimationElementImpl::GetChildDocument()
// Accessor function for obtaining the child SVG document
// ---------------------------------------------------------------------------
CSvgDocumentImpl* CSvgMediaAnimationElementImpl::GetChildDocument()
    {
        if ( iNewDocument )
         return iNewDocument;
        else 
          return NULL;
    }
    
// -----------------------------------------------------------------------------
// CSvgMediaAnimationElementImpl::SetPreserveAspectRatioL
// From CSvgElementImpl
// -----------------------------------------------------------------------------
//        
void CSvgMediaAnimationElementImpl::SetPreserveAspectRatioL( 
    TSvgPreserveAspectAlignType aAlign,
    TSvgMeetOrSliceType aMeetSlice )
    {
    iViewBoxImpl->SetPreserveAspectRatioL( aAlign, aMeetSlice);
    }
    
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgMediaAnimationElementImpl::SetWidth(TReal32 aValue)
    {
      iWidth = aValue;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgMediaAnimationElementImpl::SetHeight(TReal32 aValue)
    {
     iHeight = aValue;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------    
MSvgError* CSvgMediaAnimationElementImpl::PrepareDomChildSvg
                           (const TDesC& aFileName,TInt& aHandle, TInt aEngine)
    {
         // Create session to open file
    RFs session;
    CSvgDocumentImpl::OpenSession( session, *iSvgError );
    if ( iSvgError->HasError() )
    {
        return iSvgError;
    }

    // Open file handle
    RFile fileHandle;
    TInt openError = 0;
    
    if ( ((CSvgDocumentImpl*)iOwnerDocument)->Engine() == NULL  )
       {
       return iSvgError;
       }
    if ( (((CSvgDocumentImpl*)iOwnerDocument)->Engine())->iRequestObserver == NULL) 
       {
       return iSvgError;	
       }
    
    openError = (((CSvgDocumentImpl*)iOwnerDocument)->Engine())->iRequestObserver
   									->FetchImage( aFileName, session,fileHandle  );
   
    if ( openError != KErrNone )
       {
       CSvgDocumentImpl::PrepareError( *iSvgError, ESvgUnknown, openError,
                                        msg, aFileName );
       fileHandle.Close();
       session.Close();
       return iSvgError;                                 
       }
     else
       {
       PrepareDomChildSvg( fileHandle, aHandle, aEngine );
       }
        
     // If iDurMediaTime is ETrue then it means dur has been set as sometime
     // else dur was not there and defauly media duration is set.
     // iDurMediaTime is set to ETrue by default.
        if ( iDurMediaTime  )
           {
           TUint32 dur =  iNewDocument->AnimationDuration();
           if ( dur == 0xFFFFFFFF )
              {
              fileHandle.Close();
    		  session.Close();
    		  return iSvgError;
              }
           dur /=  1000; 
           TBuf<10> number;
           number.Num(dur); 
           CSvgMediaElementBase::SetAttributeL(KTmpDur, number);    	
           }
        
        
     
    fileHandle.Close();
    session.Close();
    return iSvgError;
    }
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------    
 MSvgError* CSvgMediaAnimationElementImpl::PrepareDomChildSvg
                          ( RFile& aFileHandle, TInt& aHandle, TInt aEngine)
    {
        // reset to start of file
    TInt zero = 0;
    aFileHandle.Seek( ESeekStart, zero );

    TInt fileSize = 0;
    TInt sizeError = aFileHandle.Size( fileSize );
    if ( sizeError != KErrNone )
    {
        
        CSvgDocumentImpl::PrepareError( *iSvgError, ESvgUnknown, sizeError,
                                        KErrorMsg, KNullDesC );
        return iSvgError;
    }

    HBufC8* byteArray = NULL;
    TRAPD( error, byteArray = HBufC8::NewL( fileSize ) );
    if ( error != KErrNone )
    {
        
        CSvgDocumentImpl::PrepareError( *iSvgError, ESvgNoMemory, error,
                                        KMemoryErrorMsg, KNullDesC );
        return iSvgError;
    }

    TPtr8 des = byteArray->Des();
    TInt readError = aFileHandle.Read( des );
    if ( readError != KErrNone )
    {
        
        CSvgDocumentImpl::PrepareError( *iSvgError, ESvgNoMemory, error,
                                        KReadErrorMsg, KNullDesC );
        delete byteArray;
        return iSvgError;
    }

    PrepareDomChildSvg( *byteArray, aHandle, aEngine );
    delete byteArray;
    return iSvgError;
    
    }
    
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------  
    
MSvgError* CSvgMediaAnimationElementImpl::PrepareDomChildSvg(const TDesC8& aByteData, 
    TInt& aHandle, TInt /* aEngine */)  
    {
    aHandle = 0;

    // Clear Error
    iSvgError->SetErrorCode( ESvgNoError );


    // CSvgDocumentImpl* newDocument = NULL;
    CSvgSvgElementImpl* lRoot = (CSvgSvgElementImpl* )((CSvgDocumentImpl* )
                                            iOwnerDocument)->RootElement();
    
    // Get the default sync behaviour from SVG element
    TSvgSyncBehaviour lRootSB = lRoot->SyncBehaviorDefault();
    
    // Get the default tolerance from SVG element
    TInt32 lRootST = lRoot->SyncToleranceDefault();
    
    // Indicate ETrue while creating document to indicate that
    // document has a parent
    CSvgBitmapFontProvider *tempBitmapFontProvider = 
            ((CSvgDocumentImpl*)iOwnerDocument)->GetBitmapFontProvider();
            
    TRAPD( error, iNewDocument = CSvgDocumentImpl::NewL( tempBitmapFontProvider,
            ETrue, lRootSB, lRootST ) );
    
    // Check for error
    if ( error != KErrNone )
        {
        #ifdef _DEBUG
        RDebug::Print(KEngineFailed);
        #endif //_DEBUG
        CSvgDocumentImpl::PrepareError( *iSvgError, ESvgNoMemory, error,
                                        KEngineFailed, KNullDesC );
        return iSvgError;
        }

    // Set appropriate flags to documents
    iNewDocument->SetWasPrepared( !iIsLoadingRequest );
    iNewDocument->SetDRMMode( iDrmEnabled );
    iNewDocument->SetThumbNailMode( iIsThumbNailMode );
  

    iNewDocument->SetEngine(((CSvgDocumentImpl*)iOwnerDocument)->Engine());
    iNewDocument->SetLoadingListeners(((CSvgDocumentImpl*)iOwnerDocument)
                                                 ->GetLoadingListeners());

    // Parse document
    iNewDocument->Load( aByteData, *iSvgError );

    #ifdef _DEBUG
    if ( iNewDocument->GetError() && iNewDocument->GetError()->HasError() )
        {
        RDebug::Printf("Load warning: ");
        RDebug::RawPrint( iNewDocument->GetError()->Description() );
        }
    #endif

    iFileIsLoaded = !iSvgError->HasError() || iSvgError->IsWarning();

    // Parsing error or out of memory
    if ( !iFileIsLoaded )
        {
        return iSvgError;
        }

    aHandle = (TInt)iNewDocument;

    // Add the animation element as an observer on the child time 
    // container
    iNewDocument->TimeContainer()->SetTcObserver( this );

    return iSvgError;
    }
    
// -----------------------------------------------------------------------------
// CSvgMediaAnimationElementImpl::ParentTimeContainerTick
// From CSvgMediaElementBase
// -----------------------------------------------------------------------------
//
void CSvgMediaAnimationElementImpl::ParentTimeContainerTick( 
    TSvgTick /* aTick */) // Current tick information 
    {
        //empty implementation
    }
       

// -----------------------------------------------------------------------------
// CSvgMediaAnimationElementImpl::GetEntityCurrentTime
// From CSvgMediaElementBase
// -----------------------------------------------------------------------------
//        
void CSvgMediaAnimationElementImpl::GetEntityCurrentTime( TUint32& 
            aEntityCurTime ) // Current Entity Time in msecs. 
    {
    if ( iNewDocument )
        {
        // Access time container from child document
        CSvgTimeContainer* lChildTC = iNewDocument->TimeContainer();
            
        if ( lChildTC )
            {
            lChildTC->GetEntityCurrentTime( aEntityCurTime );        
            // Adjust the time from the timed entity to compensate
            // the BeginTick's parent time container tick
            aEntityCurTime += iBeginTick.iParentTcTick;
            }
        }
    }

// -----------------------------------------------------------------------------
// CSvgMediaAnimationElementImpl::SetCurSyncMasterStatus
// From CSvgMediaElementBase
// -----------------------------------------------------------------------------
void CSvgMediaAnimationElementImpl::SetCurSyncMasterStatus( 
    TBool aSyncMasterStatus ) //  Indicates whether the element is currrently 
                              // Sync Master.
    {
    // Set the current sync master status
    CSvgMediaElementBase::SetCurSyncMasterStatus( aSyncMasterStatus );
    if ( iNewDocument )
        {
        // Access time container from child document
        CSvgTimeContainer* lChildTC = iNewDocument->TimeContainer();
            
        if ( lChildTC )
            {
            lChildTC->SetCurSyncMasterStatus( aSyncMasterStatus );        
            }    
        }
    }

// -----------------------------------------------------------------------------
// CSvgMediaAnimationElementImpl::CanGenerateTick
// From CSvgMediaElementBase
// -----------------------------------------------------------------------------
TBool CSvgMediaAnimationElementImpl::CanGenerateTick()
    {
    if ( iNewDocument )
        {
        // Access time container from child document
        CSvgTimeContainer* lChildTC = iNewDocument->TimeContainer();
            
        if ( lChildTC )
            {
            return ( lChildTC->CanGenerateTick() );
            }
        }
    return EFalse; 
    }

// -----------------------------------------------------------------------------
// CSvgMediaAnimationElementImpl::ResyncTimedEntity
// From CSvgMediaElementBase
// -----------------------------------------------------------------------------
void CSvgMediaAnimationElementImpl::ResyncTimedEntity( 
            TUint32 /* aSynctime */ ) // Time for resync in msecs.
    {
    // Empty Implementation
    }    
// -----------------------------------------------------------------------------
// CSvgMediaAnimationElementImpl::PauseTimedEntity
// From CSvgMediaElementBase
// -----------------------------------------------------------------------------
void CSvgMediaAnimationElementImpl::PauseTimedEntity()
    {
    iIsSyncPauseIssued = ETrue;
    if ( iNewDocument && iAnimationStatus == EPlayerStatePlaying )
        {
        iAnimationStatus = EPlayerStatePaused;
        }
    }

// -----------------------------------------------------------------------------
// CSvgMediaAnimationElementImpl::ResumeTimedEntity
// From CSvgMediaElementBase
// -----------------------------------------------------------------------------
void CSvgMediaAnimationElementImpl::ResumeTimedEntity()
    {
    iIsSyncPauseIssued =  EFalse;
    if ( iNewDocument && iAnimationStatus == EPlayerStatePaused )
    	{
    	// The child time container recieves the event from the parent time
    	// container directly.
	    iAnimationStatus = EPlayerStatePlaying;
    	}    
    }

// -----------------------------------------------------------------------------
// CSvgMediaAnimationElementImpl::StopTimedEntity
// From CSvgMediaElementBase
// -----------------------------------------------------------------------------
void CSvgMediaAnimationElementImpl::StopTimedEntity()
    {
    if ( iNewDocument )
    	{
    	iAnimationStatus = EPlayerStateStop;
    	}    
    }
    

//From MSvgTimedEntityInterface
TSvgObjectType CSvgMediaAnimationElementImpl::ObjectType()
    {
	return ESvgMediaAnimationElement;	
    }


//returns the child time container of the element 
//used in timecontainer
CSvgTimeContainer* CSvgMediaAnimationElementImpl::GetChildTimeContainer()
    {
    //Check for valid document and get time container from documentimpl
    if ( iNewDocument )
        {
        return iNewDocument->TimeContainer();
        }
    else
        {
        return NULL;    
        }
    }
// -----------------------------------------------------------------------------
// CSvgMediaAnimationElementImpl::TimeContainerReady
// From MSvgTimeContainerObserver
// -----------------------------------------------------------------------------
void CSvgMediaAnimationElementImpl::TimeContainerReady()
    {
    // Access time container from owner document
    CSvgTimeContainer* lOwnerTC = ((CSvgDocumentImpl* )
        iOwnerDocument)->TimeContainer();
        
    if ( lOwnerTC )
        {
        lOwnerTC->TimedEntityReady( this );        
        }
    }

// -----------------------------------------------------------------------------
// CSvgMediaAnimationElementImpl::TimeContainerNotReady
// From MSvgTimeContainerObserver
// -----------------------------------------------------------------------------
void CSvgMediaAnimationElementImpl::TimeContainerNotReady()
    {
    // Access time container from owner document
    CSvgTimeContainer* lOwnerTC = ((CSvgDocumentImpl* )
        iOwnerDocument)->TimeContainer();
        
    if ( lOwnerTC )
        {
        lOwnerTC->TimedEntityNotReady( this );        
        }    
    }
    
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------

TBool CSvgMediaAnimationElementImpl::AnimProcL( MSvgTimerEvent* /* aEvent */ )
    {
#ifdef _DEBUG    
	RDebug::Print(_L("%d %d"), iAnimStatus, IsAnimating());
#endif
	// Check if document is valid
	if ( !iNewDocument || !iNewDocument->RootElement() )
	    {
	    return EFalse;
	    }
    if ( iAnimationStatus == EPlayerProhibit )
        return EFalse;
    if ( iAnimTime->DurationTime() == KTimeIndefinite || iAnimTime->DurationTime() == 0 )
        {
         return EFalse;
        }
	
    if ( iAnimStatus != KAnimActive )
        {
        if ( iAnimationStatus == EPlayerStatePlaying )
    		{
             //stop child svg animation based on freeze/remove value.
            if( iFill == KAnimFillRemove )
                {
                    iNewDocument->SetCurrentTime( 0 );
                
                   iNewDocument->iAnimationResetNeeded = ETrue;
                 
                    if  (iNewDocument && iNewDocument->iEventHandler )
                    {
                    iNewDocument->iEventHandler->SetCurrentTime(0);
                    }
            	
            	    TSvgTimerEvent timeEvent(0);
            	
            	
            	    iNewDocument->Reset( &timeEvent );
                }
            
        	iAnimationStatus = EPlayerStateStop;
        	return EFalse;
    		}
        }
    else //iAnimStatus == KAnimActive
        {
        if ( !iHadBegun )
            {
            iHadBegun = ETrue;
            // animation element can be drawn now
            iInitialVisibility = EAnimationVisible;
            // this is a restart from begin list.
            if ( (iAnimationStatus != EPlayerStateIdle) )
//            	(iAnimationStatus != EPlayerStatePlayInit))
            	{
            	
            	//child svg animataion should begin from 0 again, seek it to zero.
            	
            	// Remember the current time as beginning of the animation.
            	
                   iNewDocument->SetCurrentTime(0);
                
                   iNewDocument->iAnimationResetNeeded = ETrue;
                
                if ( this->iNewDocument && this->iNewDocument->iEventHandler )
                    {
                    this->iNewDocument->iEventHandler->SetCurrentTime(0);
                    }
            	
            	 TSvgTimerEvent timeEvent(0);
            	
            	
            	 iNewDocument->Reset( &timeEvent );
            	
            	// Save the animation start tick so that
            	// child document's tick can be correctly offset
		    	iBeginTick = ( ( CSvgDocumentImpl* )
		    	    iOwnerDocument )->TimeContainer()->GetCurTick(); 
		    	iAnimationStatus = EPlayerStatePlaying;
		    	
            	}
            return ETrue;
            }
            
            
        if ( iAnimationStatus == EPlayerStateIdle ) // stopped
            {
            //child svg animataion should begin from 0 i.e for the first time
            // Remember the current time as beginning of the animation.
             iNewDocument->SetCurrentTime( 0 );
                
                   iNewDocument->iAnimationResetNeeded = ETrue;
                
                if ( this->iNewDocument && this->iNewDocument->iEventHandler )
                    {
                    this->iNewDocument->iEventHandler->SetCurrentTime(0);
                    }
            	
            	 TSvgTimerEvent timeEvent(0);
            	
            	
            	 iNewDocument->Reset( &timeEvent );
            	            
            	// Save the animation start tick so that
            	// child document's tick can be correctly offset
		    	iBeginTick = ( ( CSvgDocumentImpl* )
		    	    iOwnerDocument )->TimeContainer()->GetCurTick(); 
       		    iAnimationStatus = EPlayerStatePlaying;
           	    return ETrue;
            }
            
            if ( iAnimationStatus == EPlayerStatePlaying )
                {

                // child svg active and in running state so pass the tick
                // to the child svg's time container
                // Subtract the reference begin tick from the current tick 
                // before giving the tick.
                TSvgTick lChildTick = ( ( CSvgDocumentImpl* )
		    	    iOwnerDocument )->TimeContainer()->GetCurTick(); 
                
                lChildTick.iParentTcTick -= iBeginTick.iParentTcTick;
                lChildTick.iRealTimeTick -= iBeginTick.iRealTimeTick;
                lChildTick.iParentTcTick -= iNegativeBeginTime;
                lChildTick.iRealTimeTick -= iNegativeBeginTime;
                iNewDocument->TimeContainer()->ParentTimeContainerTick
                                               ( lChildTick );
                }
             if ( iAnimationStatus == EPlayerStateStop )
        	{
            	
        	       iNewDocument->SetCurrentTime( 0 );
                   iNewDocument->iAnimationResetNeeded = ETrue;
                
                if ( this->iNewDocument && this->iNewDocument->iEventHandler )
                    {
                    this->iNewDocument->iEventHandler->SetCurrentTime(0);
                    }
            	
            	 TSvgTimerEvent timeEvent(0);
            	
            	
            	 iNewDocument->Reset( &timeEvent );
            	            
            	// Save the animation start tick so that
            	// child document's tick can be correctly offset
		    	iBeginTick = ( ( CSvgDocumentImpl* )
		    	    iOwnerDocument )->TimeContainer()->GetCurTick(); 
       		    iAnimationStatus = EPlayerStatePlaying;
           	    return ETrue;   
        	}    
        }


    return ETrue;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------

void CSvgMediaAnimationElementImpl::InitAnimationL()
    {
        if ( !iNewDocument )
        {
            return;
        }
    CSvgMediaElementBase::InitAnimationL();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgMediaAnimationElementImpl::GetPreserveAspectRatio(
                                    MSvgPreserveAspectRatio*& aAspectRatio )
    {
    iViewBoxImpl->GetPreserveAspectRatio( aAspectRatio );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TFloatFixPt CSvgMediaAnimationElementImpl::X()
    {
    return iX;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TFloatFixPt CSvgMediaAnimationElementImpl::Y()
    {
    return iY;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TFloatFixPt CSvgMediaAnimationElementImpl::Width()
    {
    return iWidth;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TFloatFixPt CSvgMediaAnimationElementImpl::Height()
    {
    return iHeight;
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------    
 void CSvgMediaAnimationElementImpl::GetBBox( TGfxRectangle2D& aBbox )
    {
        TGfxRoundRectangle2D        aRectangle;   
         
        aRectangle.iX = 0;
        aRectangle.iY = 0;
        aRectangle.iHeight = iHeight;
        aRectangle.iWidth =  iWidth;
        if ( !(TInt32)iHeight || !(TInt32)iWidth || iInitialVisibility != EAnimationVisible  )
            {
               aRectangle.iHeight = aRectangle.iWidth = 0;
            }
        aRectangle.GetBounds( GetCTM(), aBbox );
    }
    
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------    
void CSvgMediaAnimationElementImpl::GetFourPointBBox
                                    (TSvgFourPointRect& aFourPointBbox)
{
	const TGfxAffineTransform& ctm = GetCTM();
    TGfxRoundRectangle2D        aRectangle;   
    aRectangle.iX = 0;
    aRectangle.iY = 0;
    aRectangle.iHeight = iHeight;
    aRectangle.iWidth =  iWidth;
        
    if ( !(TInt32)iHeight || !(TInt32)iWidth )
    {
     aRectangle.iHeight = aRectangle.iWidth = 0;
    }
	TSvgFourPointRect lFourPointBox(aRectangle.iX, aRectangle.iY, aRectangle.iWidth, aRectangle.iHeight);

    TGfxPoint2D lPoint1, lPoint2, lPoint3, lPoint4;

    lFourPointBox.GetPoints(lPoint1, lPoint2, lPoint3, lPoint4);

    ctm.Transform(&lPoint1, &lPoint1, 1);
    ctm.Transform(&lPoint2, &lPoint2, 1);
    ctm.Transform(&lPoint3, &lPoint3, 1);
    ctm.Transform(&lPoint4, &lPoint4, 1);

    aFourPointBbox.SetRectPoints(lPoint1, lPoint2, lPoint3, lPoint4);
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgMediaAnimationElementImpl::GetUnscaledBBox( TGfxRectangle2D& aBbox )
    {
    TGfxAffineTransform identityTx;
    TGfxRoundRectangle2D        aRectangle;   
    aRectangle.iX = iX;
    aRectangle.iY = iY;
    aRectangle.iHeight = iHeight;
    aRectangle.iWidth =  iWidth;
    aRectangle.GetBounds( identityTx, aBbox );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------    
TBool CSvgMediaAnimationElementImpl::MouseEntered( 
                            RPointerArray<CSvgElementImpl>& /* aElements */,
                            TInt aX, TInt aY )
    
    {
    if ( !iNewDocument )
        {
        return EFalse;
        }
        
    TInt listenerCount = (((CSvgDocumentImpl*)iOwnerDocument)->Engine())->
                                                     MouseListenerCount();

    if ( iNewDocument )
        {
        listenerCount += iNewDocument->MouseInternalListenerCount();
        }
    if ( listenerCount == 0 )
        {
        return EFalse;
        }
    // Get list of elements containing pointer (bounding-box)
    RPointerArray<CSvgElementImpl> viewableElements;
    RPointerArray<CSvgElementImpl> enteringElements;
    RPointerArray<CSvgElementImpl> stillInsideElements;
    (((CSvgDocumentImpl*)iOwnerDocument)->Engine())->GetViewableElementsAtPoint
                                    (iNewDocument, viewableElements, aX, aY );

    // check for mouse entering of elements
    TInt viewableEleCnt = viewableElements.Count();
    for ( TInt i = 0; i < viewableEleCnt; i++ )
        {
        // If an element is in the view-list but NOT in the entered-list
        // then, it's copied to the entered-list
        // otherwise, ignore (still inside)
        if ( iSvgMouseEnteredElements.Find( viewableElements[i] ) 
             == KErrNotFound )
            {
            enteringElements.Append( viewableElements[i] );
            }
        else
            {
            stillInsideElements.Append( viewableElements[i] );
            }
        }

    // check for mouse exiting of elements
    RPointerArray<CSvgElementImpl> exitingElements;
    TInt svgMouseEnteredEleCnt = iSvgMouseEnteredElements.Count();
    for ( TInt i = 0; i < svgMouseEnteredEleCnt; i++ )
        {
        // If an element is in the entered-list but not the view-list
        // then, it's copied to the exiting list
        // otherwise, ignore (
        if ( viewableElements.Find( iSvgMouseEnteredElements[i] ) 
             == KErrNotFound )
            {
            exitingElements.Append( iSvgMouseEnteredElements[i] );
            }
        }

    // Remove exiting elements from entered list
    TInt exitingEleCnt = exitingElements.Count();
    for ( TInt i = 0; i < exitingEleCnt; i++ )
        {
        TInt index = iSvgMouseEnteredElements.Find( exitingElements[i] );
        if ( index != KErrNotFound )
            {
            iSvgMouseEnteredElements.Remove( index );
            }
        }

    // Add entering elements to entered-list
    TInt enteringEleCnt = enteringElements.Count();
    for ( TInt i = 0; i < enteringEleCnt; i++ )
        {
        iSvgMouseEnteredElements.Append( enteringElements[i] );
        }

    // Notify exiting
    if ( exitingElements.Count() > 0 )
        {
        (((CSvgDocumentImpl*)iOwnerDocument)->Engine())->NotifyMouseExited
                                                ( exitingElements, aX, aY );
        if ( iNewDocument )
            {
            iNewDocument->NotifyInternalMouseExited( exitingElements, aX, aY );
            }
        }

    // Notify entering
    if ( enteringElements.Count() > 0 )
        {
        (((CSvgDocumentImpl*)iOwnerDocument)->Engine())->NotifyMouseEntered
                                                ( enteringElements, aX, aY );
        if ( iNewDocument )
            {
            iNewDocument->NotifyInternalMouseEntered( enteringElements, aX, aY );
            }
        }

    // Notify moved
    if ( stillInsideElements.Count() > 0 )
        {
        (((CSvgDocumentImpl*)iOwnerDocument)->Engine())->NotifyMouseMoved
                                            ( stillInsideElements, aX, aY );
        if ( iNewDocument )
            {
            iNewDocument->NotifyInternalMouseMoved( stillInsideElements, aX, aY );
            }
        }

    exitingElements.Close();
    enteringElements.Close();
    stillInsideElements.Close();
    viewableElements.Close();
    
    
    return ETrue;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------    
TBool CSvgMediaAnimationElementImpl::MouseExited( 
    RPointerArray<CSvgElementImpl>& /* aElements */,
    TInt aX, TInt aY )
    {
     if ( !iNewDocument )
        {
        return EFalse;
        }
        
    TInt listenerCount = (((CSvgDocumentImpl*)iOwnerDocument)->Engine())->
                                                    MouseListenerCount();

    if ( iNewDocument )
    {
        listenerCount += iNewDocument->MouseInternalListenerCount();
    }
    if ( listenerCount == 0 )
        {
        return EFalse;
        }
    // Get list of elements containing pointer (bounding-box)
    RPointerArray<CSvgElementImpl> viewableElements;
    RPointerArray<CSvgElementImpl> enteringElements;
    RPointerArray<CSvgElementImpl> stillInsideElements;
    (((CSvgDocumentImpl*)iOwnerDocument)->Engine())->GetViewableElementsAtPoint
                                    (iNewDocument, viewableElements, aX, aY );

    // check for mouse entering of elements
    TInt viewableEleCnt = viewableElements.Count();
    for ( TInt i = 0; i < viewableEleCnt; i++ )
        {
        // If an element is in the view-list but NOT in the entered-list
        // then, it's copied to the entered-list
        // otherwise, ignore (still inside)
        if ( iSvgMouseEnteredElements.Find( viewableElements[i] ) 
             == KErrNotFound )
            {
            enteringElements.Append( viewableElements[i] );
            }
        else
            {
            stillInsideElements.Append( viewableElements[i] );
            }
        }

    // check for mouse exiting of elements
    RPointerArray<CSvgElementImpl> exitingElements;
    TInt svgMouseEnteredEleCnt = iSvgMouseEnteredElements.Count();
    for ( TInt i = 0; i < svgMouseEnteredEleCnt; i++ )
        {
        // If an element is in the entered-list but not the view-list
        // then, it's copied to the exiting list
        // otherwise, ignore (
        if ( viewableElements.Find( iSvgMouseEnteredElements[i] ) 
             == KErrNotFound )
            {
            exitingElements.Append( iSvgMouseEnteredElements[i] );
            }
        }

    // Remove exiting elements from entered list
    TInt exitingEleCnt = exitingElements.Count();
    for ( TInt i = 0; i < exitingEleCnt; i++ )
        {
        TInt index = iSvgMouseEnteredElements.Find( exitingElements[i] );
        if ( index != KErrNotFound )
            {
            iSvgMouseEnteredElements.Remove( index );
            }
        }

    // Add entering elements to entered-list
    TInt enteringEleCnt = enteringElements.Count();
    for ( TInt i = 0; i < enteringEleCnt; i++ )
        {
        iSvgMouseEnteredElements.Append( enteringElements[i] );
        }

    // Notify exiting
    if ( exitingElements.Count() > 0 )
        {
        (((CSvgDocumentImpl*)iOwnerDocument)->Engine())->NotifyMouseExited
                                                ( exitingElements, aX, aY );
        if ( iNewDocument )
            {
            iNewDocument->NotifyInternalMouseExited( exitingElements, aX, aY );
            }
        }

    // Notify entering
    if ( enteringElements.Count() > 0 )
        {
        (((CSvgDocumentImpl*)iOwnerDocument)->Engine())->NotifyMouseEntered
                                                ( enteringElements, aX, aY );
        if ( iNewDocument )
            {
            iNewDocument->NotifyInternalMouseEntered( enteringElements, aX, aY );
            }
        }

    // Notify moved
    if ( stillInsideElements.Count() > 0 )
        {
        (((CSvgDocumentImpl*)iOwnerDocument)->Engine())->NotifyMouseMoved
                                            ( stillInsideElements, aX, aY );
        if ( iNewDocument )
            {
            iNewDocument->NotifyInternalMouseMoved( stillInsideElements, aX, aY );
            }
        }

    exitingElements.Close();
    enteringElements.Close();
    stillInsideElements.Close();
    viewableElements.Close();
    
    
    return ETrue;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------    
TBool CSvgMediaAnimationElementImpl::MouseMoved( 
    RPointerArray<CSvgElementImpl>& /*aElements */,
                                TInt aX, TInt aY )
    {
    if( !iNewDocument ) 
        {
        return EFalse;
        }
        
    // Process for Mouse enter/exit
    TInt listenerCount = (((CSvgDocumentImpl*)iOwnerDocument)->Engine())
                                                   ->MouseListenerCount();

    if ( iNewDocument )
    {
        listenerCount += iNewDocument->MouseInternalListenerCount();
    }
    if ( listenerCount == 0 )
        {
        return EFalse;
        }
    // Get list of elements containing pointer (bounding-box)
    RPointerArray<CSvgElementImpl> viewableElements;
    RPointerArray<CSvgElementImpl> enteringElements;
    RPointerArray<CSvgElementImpl> stillInsideElements;
    (((CSvgDocumentImpl*)iOwnerDocument)->Engine())->GetViewableElementsAtPoint
                                    (iNewDocument, viewableElements, aX, aY );

    // check for mouse entering of elements
    TInt viewableEleCnt = viewableElements.Count();
    for ( TInt i = 0; i < viewableEleCnt; i++ )
        {
        // If an element is in the view-list but NOT in the entered-list
        // then, it's copied to the entered-list
        // otherwise, ignore (still inside)
        if ( iSvgMouseEnteredElements.Find( viewableElements[i] ) 
             == KErrNotFound )
            {
            enteringElements.Append( viewableElements[i] );
            }
        else
            {
            stillInsideElements.Append( viewableElements[i] );
            }
        }

    // check for mouse exiting of elements
    RPointerArray<CSvgElementImpl> exitingElements;
    TInt svgMouseEnteredEleCnt = iSvgMouseEnteredElements.Count();
    for ( TInt i = 0; i < svgMouseEnteredEleCnt; i++ )
        {
        // If an element is in the entered-list but not the view-list
        // then, it's copied to the exiting list
        // otherwise, ignore (
        if ( viewableElements.Find( iSvgMouseEnteredElements[i] ) 
             == KErrNotFound )
            {
            exitingElements.Append( iSvgMouseEnteredElements[i] );
            }
        }

    // Remove exiting elements from entered list
    TInt exitingEleCnt = exitingElements.Count();
    for ( TInt i = 0; i < exitingEleCnt; i++ )
        {
        TInt index = iSvgMouseEnteredElements.Find( exitingElements[i] );
        if ( index != KErrNotFound )
            {
            iSvgMouseEnteredElements.Remove( index );
            }
        }

    // Add entering elements to entered-list
    TInt enteringEleCnt = enteringElements.Count();
    for ( TInt i = 0; i < enteringEleCnt; i++ )
        {
        iSvgMouseEnteredElements.Append( enteringElements[i] );
        }

    // Notify exiting
    if ( exitingElements.Count() > 0 )
        {
        (((CSvgDocumentImpl*)iOwnerDocument)->Engine())->NotifyMouseExited
                                                ( exitingElements, aX, aY );
        if ( iNewDocument )
            {
            iNewDocument->NotifyInternalMouseExited( exitingElements, aX, aY );
            }
        }

    // Notify entering
    if ( enteringElements.Count() > 0 )
        {
        (((CSvgDocumentImpl*)iOwnerDocument)->Engine())->NotifyMouseEntered
                                                ( enteringElements, aX, aY );
        if ( iNewDocument )
            {
            iNewDocument->NotifyInternalMouseEntered( enteringElements, aX, aY );
            }
        }

    // Notify moved
    if ( stillInsideElements.Count() > 0 )
        {
        (((CSvgDocumentImpl*)iOwnerDocument)->Engine())->NotifyMouseMoved
                                            ( stillInsideElements, aX, aY );
        if ( iNewDocument )
            {
            iNewDocument->NotifyInternalMouseMoved( stillInsideElements, aX, aY );
            }
        }

    exitingElements.Close();
    enteringElements.Close();
    stillInsideElements.Close();
    viewableElements.Close();
    

    return ETrue;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TBool CSvgMediaAnimationElementImpl::MousePressed( 
    RPointerArray<CSvgElementImpl>& /* aElements */,
    TInt aX, TInt aY )
    {
    
    if( !iNewDocument )
        {
        return EFalse;            
        }

        
     TInt listenerCount = (((CSvgDocumentImpl*)iOwnerDocument)->Engine())
                                                    ->MouseListenerCount();
        
      
    if (iNewDocument)
    {
        listenerCount += iNewDocument->MouseInternalListenerCount();
    }
    if ( listenerCount == 0 )
        {
           // return 0;
           return EFalse;
        }
    RPointerArray<CSvgElementImpl> viewableElements;
    // TInt topClickedElement = 0;

    (((CSvgDocumentImpl*)iOwnerDocument)->Engine())->GetViewableElementsAtPoint
                                    (iNewDocument,viewableElements, aX, aY );
    if ( viewableElements.Count() > 0 )
        {
        (((CSvgDocumentImpl*)iOwnerDocument)->Engine())->NotifyMousePressed
                                                ( viewableElements, aX, aY );
        if ( iNewDocument)
            {
            iNewDocument->NotifyInternalMousePressed( viewableElements, aX, aY );
            }
        }
    viewableElements.Close();

    //return topClickedElement;
   
   
    return ETrue;
    }
    
TBool CSvgMediaAnimationElementImpl::MouseReleased( 
    RPointerArray<CSvgElementImpl>& /* aElements */,
    TInt aX, TInt aY )
    {
    // Process for MouseRelease
    if ( !iNewDocument )
        {
        return EFalse;    
        }
    
    TInt listenerCount = (((CSvgDocumentImpl*)iOwnerDocument)->Engine())->
                                                        MouseListenerCount();

    if ( iNewDocument )
    {
        listenerCount += iNewDocument->MouseInternalListenerCount();
    }
    if ( listenerCount == 0 )
        {
        return ETrue;
        }
    RPointerArray<CSvgElementImpl> viewableElements;
    (((CSvgDocumentImpl*)iOwnerDocument)->Engine())->GetViewableElementsAtPoint
                                    (iNewDocument, viewableElements, aX, aY );
    if ( viewableElements.Count() > 0 )
        {
        (((CSvgDocumentImpl*)iOwnerDocument)->Engine())->NotifyMouseReleased
                                                ( viewableElements, aX, aY );
        if ( iNewDocument )
            {
            iNewDocument->NotifyInternalMouseReleased( viewableElements, aX, aY );
            }
        }
    viewableElements.Close();
    return ETrue;
    }                
 
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgMediaAnimationElementImpl::UpdateCTM()
    {
    if ( !iNewDocument )
    {
    return;
    }
        
    CSvgElementImpl::UpdateCTM();
    TGfxAffineTransform aTr = GetCTM();
          
    TGfxPoint2D ep( 1, 0 ), org( 2, 0 );
    aTr.Transform( &ep, & ep, 1 );
    aTr.Transform( &org, & org, 1 );
    ep.iX -= org.iX;
    ep.iY -= org.iY;
    iScale = TFloatFixPt::Sqrt( ep.iX * ep.iX + ep.iY * ep.iY );
   
    TGfxAffineTransform tMatrix;
    tMatrix = aTr;

    #ifdef SVG_FLOAT_BUILD
    tMatrix.Scale( ( TFloatFixPt( 1 ) / ( iScale ) ), ( TFloatFixPt( 1 ) / ( iScale ) ) );
    #else
    tMatrix.Scale( ( TFloatFixPt( 0x10000,ETrue ) / ( iScale ) ), ( TFloatFixPt( 0x10000,ETrue ) / ( iScale ) ) );
    #endif
    
    if ( !iIsOriginalRootTrSet )
    {
       MSvgTransformList* aTransformList;
       ((CSvgSvgElementImpl *)(iNewDocument->RootElement()))->GetTransform
                                                          (aTransformList);
        iOriginalRootTr = aTransformList->GetItem(0);
        iIsOriginalRootTrSet = ETrue;
    }
   
    iNewDocument->DocumentTransform(tMatrix);
    tMatrix.Concatenate(iOriginalRootTr);
   
    ((CSvgSvgElementImpl *)(iNewDocument->RootElement()))->SetTransformList
                                                            (tMatrix);
   
    TFloatFixPt  newWidth =        iScale * iWidth; 
    TFloatFixPt  newHeight =     iScale * iHeight; 
   

    ((CSvgSvgElementImpl *)(iNewDocument->RootElement()))->SetWidth(newWidth);
    ((CSvgSvgElementImpl *)(iNewDocument->RootElement()))->SetHeight(newHeight);
   
    ((CSvgElementImpl*)iNewDocument->RootElement())->UpdateCTM();
        
         
    }
    
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------    
void CSvgMediaAnimationElementImpl::DeactivateAnimation()
    {
        CSvgMediaElementBase::DeactivateAnimation(this);
    }
        
// End of file

