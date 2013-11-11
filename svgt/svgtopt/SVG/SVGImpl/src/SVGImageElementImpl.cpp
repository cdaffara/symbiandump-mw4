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

#include "SVGImageElementImpl.h"
#include "SVGDocumentImpl.h"
#include "SVGEngineImpl.h"
#include "SVGSchemaData.h"

#include "GfxAffineTransform.h"

#include <imcvcodc.h>

//#include <caf/caf.h>

const TInt  KErrNoAttributeSet  = -1;
#define KSVGWAITFORIMAGEDOWNLOAD 1

#define PERIODIC_DELAY    300000
#define PERIODIC_INTERVAL 1000000

_LIT( KSvg,  ".svg" );
_LIT( KSvgb, ".svgb" );



// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgImageElementImpl* CSvgImageElementImpl::NewL(  const TUint8 aElemID,
                                                  CSvgDocumentImpl* aDoc )
    {
    CSvgImageElementImpl*   self    = new ( ELeave )
                                      CSvgImageElementImpl( aDoc );
    CleanupStack::PushL( self );
    self->ConstructL(  aElemID, aDoc );
    CleanupStack::Pop();

    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgImageElementImpl* CSvgImageElementImpl::NewLC(  const TUint8 aElemID,
                                                   CSvgDocumentImpl* aDoc )
    {
    CSvgImageElementImpl*   self    = new ( ELeave )
                                      CSvgImageElementImpl( aDoc );
    CleanupStack::PushL( self );
    self->ConstructL(  aElemID,aDoc );

    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgImageElementImpl::ConstructL(  const TUint8 aElemID,
                                       CSvgDocumentImpl* /* aDoc */ )
    {
    CSvgElementImpl::InitializeL(  aElemID );


    iUsedImage = EFalse;
//    iBitmap = new ( ELeave ) CFbsBitmap();
    iReqAttrFlag=KSVG_IMAGE_ELEMFLAG;

    iSvgStyleProperties = new(ELeave) RPointerArray<CCssValue>(KCSS_MAX_ATTR);
    User::LeaveIfError( iSvgStyleProperties->Append( NULL ) );
    iSvgStyleProperties->Remove( 0 );
    iSvgTransformable = CSvgTransformableImpl::NewL();
    iSvgUriReferenceImpl = CSvgUriReferenceImpl::NewL();

    iBitmapLoadCalled = EFalse;

    iImgRect.iHeight = TFloatFixPt(-1);
    iImgRect.iWidth = TFloatFixPt(-1);

    iIsBase64 = EFalse;
    }



// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgImageElementImpl::~CSvgImageElementImpl()
    {
    // Reset the notifiers first.
    NotifyResetReference();

    // Check whether there this element refers to an image element.
    // This would be the case when use is used to refer to a parent 
    // image element.
    if ( iRefElement )
        {
        iRefElement->RemoveImageElementListener( this );
        iRefElement = NULL;
        }
        
    if ( iImageLoader )
        {
        delete iImageLoader;
        iImageLoader = NULL;
        }

    if ( !iUsedImage )
        {
        delete iBitmap;
        delete iMask;
        }
    if ( !iUsedImage && iPar )
        {
        if ( iPar )
            {
            delete iPar;
            iPar = NULL;
            }
        }

    if ( iImageData )
        {
        delete iImageData;
        iImageData = NULL;
        }

    if ( iSvgUriReferenceImpl )
        {
        delete iSvgUriReferenceImpl;
        iSvgUriReferenceImpl = NULL;
        }


    if ( iSvgStyleProperties )
        {
        iSvgStyleProperties->Close();
        delete iSvgStyleProperties;
        iSvgStyleProperties = NULL;
        }

    iImageElementListeners.Close();

    if ( iSessionConnected )
        {
        iSession.Close();
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgImageElementImpl::GetBBox( TGfxRectangle2D& aBbox )
    {
    iImgRect.GetBounds( GetCTM(), aBbox );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgImageElementImpl::GetUnscaledBBox( TGfxRectangle2D& aBbox )
    {
    TGfxAffineTransform identityTx;
    iImgRect.GetBounds( identityTx, aBbox );
    }

// *******************************************************
// From SVG DOM

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TFloatFixPt CSvgImageElementImpl::X()
    {
    return iImgRect.iX;
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TFloatFixPt CSvgImageElementImpl::Y()
    {
    return iImgRect.iY;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TFloatFixPt CSvgImageElementImpl::Width()
    {
    	if (iImgRect.iWidth == TFloatFixPt(-1))
    	{
    		return TFloatFixPt(0);
    	}

    	return iImgRect.iWidth;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TFloatFixPt CSvgImageElementImpl::Height()
    {
    	if (iImgRect.iHeight == TFloatFixPt(-1) )
    	{
    		return TFloatFixPt(0);
    	}

    	return iImgRect.iHeight;
    }

// *******************************************************
// SVG Implementation


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgImageElementImpl::SetWidth( TFloatFixPt aWidth )
    {
    iRenderImage = !(aWidth <= TFloatFixPt(0));
    
    iImgRect.iWidth = aWidth;

    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgImageElementImpl::SetHeight( TFloatFixPt aHeight )
    {
    iRenderImage = !(aHeight <= TFloatFixPt(0));
    
    iImgRect.iHeight = aHeight;
    
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgImageElementImpl::SetParL( const TDesC& aName )
    {
    iPar = aName.AllocL();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
const TDesC& CSvgImageElementImpl::Par()
    {
    if ( iPar )
        return *iPar;
    else
        return KNullDesC;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgImageElementImpl::SetUriL( const TDesC& aUri )
    {
    // Indicate that the Uri is set. This is used to decide whether 
    // to indicate an error if xlink:href attribute was not specified
    // on the image element.
    iIsUriSet = ETrue;
    _LIT( KXlinkHref, "xlink:href" );
    SetXlinkAttributeL(KXlinkHref, aUri);
    CheckRequiredAttributes();

	if ( Href().Length() == 0)
	{
		//empty URI string...do nothing
		//JSR 226 tests start out with empty string
		return;
	}

	//We have to be careful in this method searching the entire string
	//because we can get the entire
	//base64 encoded image coming through this call

    // Verify xlink for JavaInterface
    CSvgDocumentImpl* document = (CSvgDocumentImpl*)OwnerDocument();
    //if ( document && aUri.Length() > 0)
    if ( document )
        {
        // Check for local URI (#uri)
        //if ( aUri.Locate( '#' ) != KErrNotFound || aUri.Length() == 0)
          if ( Href().Left(1) == _L("#") )
            {
            document->SetError( KErrNotFound, _L( "Usage of local URI in <image>: " ), aUri );
            }
          else
            {
            _LIT( KHttp, "http://" );
            _LIT( KData, "data:");

            if ( ( Href().Length() >= 7 ) && ( Href().Left( 7 ) == KHttp ) )
                {
                //using an http address for the image
                return;
                }
            else if ( ( Href().Length() >= 5 ) && ( Href().Left( 5 ) == KData ) )
            	{
            	//this is a base 64 image
            	iIsBase64 = ETrue;
            	return;
            	}
            else
                {
                _LIT( KJpgFile, ".jpg");
                _LIT( KPngFile, ".png");
                _LIT( KSvgFile, ".svg");

                // Check for relative URI
          		if ( aUri.Locate( '/' ) == KErrNotFound)
          		{
          			//plain image used...but is this relative?!?
          			//does JSR 226 need an error thrown on this?
          			//i.e. image.jpg
          			if (aUri.Right(4) == KJpgFile)
          			{
          				//this is a plain image.jpg
          				return;
          			}
          			else if (aUri.Right(4) == KPngFile)
          			{
          				//this is a plain image.png file
          				return;
          			}
          			else if (aUri.Right(4) == KSvgFile)
          			{
          				//this is a filename.svg file
          				document->SetError(KErrNotFound, _L("Usage of SVG file in xlink-href:"), aUri);
          			}
          			return;
          		}
                /*if ( aUri.Find( KFile ) != 1 || !TChar( aUri[0] ).IsAlpha() )
                    {
                    document->SetError( KErrNotFound, _L( "Usage of relative URI in <image>: " ), aUri );
                    }
                else
                	{
                		//this is an absolute image
                		//i.e. c:/folder/image.jpg
                	}*/
                }
            }
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgImageElementImpl::LoadUriL()
    {
    if ( Href().Length() == 0 )
        {
        #ifdef _DEBUG
        RDebug::Printf("CSvgImageElementImpl::LoadUriL()--iUri is NULL or empty.");
        #endif
        return;
        }
    iBitmapLoadCalled = ETrue;
    iSessionConnected = EFalse;

    // Cannot continue without SvgEngine instance or
    // MSvgRequestObserver is not set

    CSvgDocumentImpl* document = (CSvgDocumentImpl*)OwnerDocument();
    if ( document == NULL )
        {
        #ifdef _DEBUG
        RDebug::Printf("CSvgImageElementImpl::LoadUriL--document handle null");
        #endif
        return;
        }

    // Request
    if ( ( Href().Length() >= 4 && Href().Right( 4 ) == KSvg ) ||
         ( Href().Length() >= 5 && Href().Right( 5 ) == KSvgb ) )
        {
        iIsSvgFile = ETrue;
        }

    // Don't call LoadUriL if LoadingListener exists and LoadingListener::WillAssignImageData returns ETrue
    if ( document )
        {
        const RPointerArray<MSvgLoadingListener>* listeners = document->GetLoadingListeners();
        if ( listeners && listeners->Count() > 0 )
            {
            if ( (*listeners)[0]->WillAssignImageData() )
                {
                (*listeners)[0]->ImageDataReference( Href() );
                return;
                }
            }
        }

    // Embedded image
    if ( ( Href().Length() >= 5 ) && ( Href().Left( 5 ) == _L( "data:" ) ) )
        {
        ProcessEncodedImageL( Href() );
        return;
        }

    iBitmapOrgReady = EFalse;
    iBitmapReady = EFalse;

    CSvgEngineImpl* engine  = document->Engine();
    if ( engine == NULL || engine->iRequestObserver == NULL )
        {
        #ifdef _DEBUG
        RDebug::Printf("CSvgImageElementImpl::LoadUriL--engine handle null");
        #endif
        return;
        }
    
    // Connect session
    RFile fileHandle;
    if ( iSession.Connect() != KErrNone )
        {
        return;
        }
    iSessionConnected = ETrue;
    TInt status = engine->iRequestObserver->FetchImage( Href(), iSession, fileHandle );
    if ( status == KErrNone )
        {
        // prevent division by zero (DrawL) when image fails to load
        CFbsBitmap* aBitmap = new ( ELeave ) CFbsBitmap();
        CleanupStack::PushL( aBitmap );
        TInt lCreateErr = aBitmap->Create( TSize( 1, 1 ), EColor64K );
        if ( lCreateErr != KErrNone )
            {
            // Error in creation, destroy the bitmap and return
            CleanupStack::PopAndDestroy( aBitmap );
            return;
            }
        // Ownership is about to be tranferred to Image Element
        CleanupStack::Pop( aBitmap );
        TInt lDecodeErr = StartImageDecoding( fileHandle, aBitmap, ETrue );
        if ( lDecodeErr != KErrNone )
            {
            // No error Processing
            }
        }
    else
        {
        iSession.Close();
        iSessionConnected = EFalse;
        }
    }

void CSvgImageElementImpl::PrepareImageFromSvgFile(const TDesC8& aImageData)
    {
    CSvgDocumentImpl* document = (CSvgDocumentImpl*)OwnerDocument();
    if ( !document )
        {
        return;
        }

    CSvgEngineImpl* engine  = document->Engine();
    if ( !engine )
        {
        return;
        }
    TSize bitmapSize( (TInt32)Width(), (TInt32)Height() );
    if ( !iBitmap )
        {
        iBitmap = new CFbsBitmap();
        }
    if(!iMask)
	    {
		iMask = new CFbsBitmap();        	
	    }
    if(iMask)
        {
		iMask->Create( bitmapSize, EGray256);        	
        }
    // Can't use ELeave because this function can't leave
    // Using ELeave but didn't handle (no trap or propagate to parents) causes more confusing.
    // Check for NULL is good enough.
    // Ignore CodeScanner error (Code Review Guide)
    if (iBitmap)
        {
	    iBitmap->Create( bitmapSize, EColor16MU );
        
        TBool preserveRatio = !( iPar != NULL && *iPar == _L( "none" ) );
        // RenderFileToBuffer if uri is a svg file
        TInt err = engine->RenderFileToBuffer( aImageData, iBitmap,iMask, preserveRatio );
        ImageLoadingCompleted(err);
        }
    iSession.Close();
    }
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------

void CSvgImageElementImpl::ProcessEncodedImageL( const TDesC& aXlinkHrefValue )
    {
    // find positions for ';' and ',' to determine encoding, mimetype
    TInt startIndex = aXlinkHrefValue.Locate( ';' );
    TInt endIndex = aXlinkHrefValue.Locate( ',' );

    // mimetype is after 'data:' : xlink:href="data:image/png;base64,
    // ignore mimetype extraction, decode will detect mimetype from decoded data.

    if ( startIndex != KErrNotFound && endIndex != KErrNotFound &&
         startIndex < endIndex )
        {
        // extract encoding type
        TPtrC encoding( aXlinkHrefValue.Left( endIndex ).Right( endIndex - startIndex - 1 ) );
        // handle Base64 encoding
        _LIT( KEncodingBase64, "base64" );
        if ( encoding == KEncodingBase64 )
            {
            // find index of first character after white-space
            TInt index = endIndex + 1;
            while ( index < aXlinkHrefValue.Length() && TChar( aXlinkHrefValue[index] ).IsSpace() )
                index++;

            // must be 8-bit
            TInt length = aXlinkHrefValue.Length() - index;
            HBufC8* encoded = HBufC8::NewLC( length );
            encoded->Des().Copy( aXlinkHrefValue.Right( length ) );

            // Assign to member variable to destroy after image is process.
            iImageData = HBufC8::NewL( length );
            TPtr8 decodedDes = iImageData->Des();

            // decode
            TImCodecB64 encodingBase64;
            TInt decodeError = encodingBase64.Decode( *encoded, decodedDes );
            CleanupStack::PopAndDestroy( encoded );

            if ( decodeError == KErrNone )
                {
                // prevent division by zero (DrawL) when image fails to load
                CFbsBitmap* lBitmap = new ( ELeave ) CFbsBitmap();
                CleanupStack::PushL( lBitmap );
                TInt lCreateErr = lBitmap->Create( TSize( 1, 1 ), EColor64K );
                if ( lCreateErr != KErrNone )
                    {
                    // Error creating bitmap, delete bitmap and return
                    CleanupStack::PopAndDestroy( lBitmap );
                    return;
                    }
                // Ownership of aBitmap is about to be transferred to image
                // element
                CleanupStack::Pop( lBitmap );
                // If bitmap was already present, release it
                if ( iBitmap )
                    {
                	delete iBitmap;
                	iBitmap = NULL;
                    }
                iBitmap = lBitmap;
                TInt lImgDecErr = StartImageDecoding( *iImageData,  
                        ETrue ); // ETrue means Synchronous Loading
                if ( lImgDecErr != KErrNone )
                    {
                    // No Error Processing
                    }
                }
            }
        }
    }

// *******************************************************
// From MXmlElement

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgImageElementImpl::SetAttributeL( const TDesC& aName,
                                          const TDesC& aValue )
    {
    _LIT( KTmpUri, "xlink:href" );
    _LIT( KTmpPreserveAspectRatio, "preserveAspectRatio" );

    if ( this->SetXlinkAttributeL( aName, aValue ) )
        {
        if ( aName == KTmpUri )
            {
            SetUriL( aValue );
            CheckRequiredAttributes();
            }
        return KErrNone;
        }


    CSvgElementImpl::SetAttributeL(aName,aValue);

    if ( aName == KTmpPreserveAspectRatio )
        {
        SetParL( aValue );
        }

    return KErrNoAttributeSet;
    }



// From MXmlElementOpt

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgImageElementImpl::GetAttributeFloat( const TInt aNameId,
                                              TFloatFixPt& aValue )
    {
    switch ( aNameId )
        {
        case KAtrX:
        case KAtrRefX:
        aValue = X();
        break;
        case KAtrY:
        case KAtrRefY:
        aValue = Y();
        break;
        case KAtrHeight:
        aValue = Height();
        break;
        case KAtrWidth:
        aValue = Width();
        break;
        default:
        return CSvgElementImpl::GetAttributeFloat( aNameId, aValue );
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgImageElementImpl::SetAttributeFloatL( const TInt aNameId,
                                               const TFloatFixPt aValue )
    {
    switch ( aNameId )
        {
        case KAtrX:
        iImgRect.iX = aValue;
        break;
        case KAtrY:
        iImgRect.iY = aValue;
        break;

        case KAtrHeight:
        SetHeight( aValue );
        CheckRequiredAttributes();
        break;

        case KAtrWidth:
        SetWidth( aValue );
        CheckRequiredAttributes();
        break;

        default:
        return CSvgElementImpl::SetAttributeFloatL( aNameId, aValue );
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgImageElementImpl::SetAttributeDesL( const TInt aNameId,
                                             const TDesC& aValue )
    {
    switch ( aNameId )
        {
        case KAtrXlinkhref:
        SetUriL( aValue );
        CheckRequiredAttributes();
        break;
        default:
        return CSvgElementImpl::SetAttributeDesL( aNameId, aValue );
        }
    return KErrNone;
    }

void CSvgImageElementImpl::ImageLoadingCompleted( TInt aError )
    {
 	iBitmapReady = aError != KErrCancel;    
    if ( aError == KErrNone )
        {
        
	    PrepareImageWithMask();
        iBitmapOrgReady = ETrue;
        // Notify clones that the bitmap is ready.
   
   
        NotifyImageDecoded();
        
   		// The redraw needs to be done once the image element is decoded, 
   		// so that the image is rendered in place in the content.
   		// If the decoding is done asynchronously, the client does not know
   		// when the decoding was completed hence it cannot perform a redraw()
   		// In such a scenario, the image element itself triggers a redraw.

    	if(iImageLoader && !(iImageLoader->IsSynchronousLoading()))
    		{
    		CSvgDocumentImpl* document = (CSvgDocumentImpl*)OwnerDocument();
    		if(document)
    			{
    			CSvgEngineImpl* engine  =document->Engine();
        		if(engine)
        			{
        			engine->ImageLoadingCompleted(aError);
        			}
    			}	
    		}
         } // if ( aError == KErrNone )
    CSvgDocumentImpl* document = (CSvgDocumentImpl*)OwnerDocument();
    if(document)
        {
        document->ImageLoadingCompleted(aError);    
        }

    
   
   
    if ( iSessionConnected )
        {
        iSession.Close();
        iSessionConnected = EFalse;
        }
    }

// ---------------------------------------------------------------------------
// void CSvgImageElementImpl::PrepareImageWithMask()
//  Combines mask information into main bitmap for use with OpenVG
// ---------------------------------------------------------------------------
void CSvgImageElementImpl::PrepareImageWithMask()
    {
    if ( iBitmap && iMask )
        {
        TSize size = iMask->SizeInPixels();
        // If size of bitmap and mask do not match, cannot process mask
        if ( size != iBitmap->SizeInPixels() )
            {
            return;
            }
        iBitmap->LockHeap();
        iMask->LockHeap();
        
        TUint32* bitmapData = (TUint32*)iBitmap->DataAddress();
        TUint8* maskData = (TUint8*)iMask->DataAddress();
        
        TInt lBmpSLLen = CFbsBitmap::ScanLineLength( size.iWidth,
        iBitmap->DisplayMode() ); 
        
        TInt lMaskSLLen = CFbsBitmap::ScanLineLength( size.iWidth, 
                iMask->DisplayMode() );
                
        TInt count = size.iWidth * size.iHeight;
        TUint32* lBitmapCurPtr = bitmapData; 
        TUint8* lMaskCurPtr = maskData; 
        
        for ( TInt lIndex = 1; lIndex < count; lIndex++ )
            {
             // Converting from ARGB to RGBA8888
            *lBitmapCurPtr = ( *lBitmapCurPtr & 0x00FFFFFF ) << 8 | *lMaskCurPtr;
            // Check if one row is complete
            if ( lIndex % size.iWidth == 0 )
                {
                // One row of pixels is complete. Now reposition the 
                // lBitmapCurPtr, lMaskCurPtr to point to beginning of new row. 
                // Current row is given by (index/widthMask) 
                // 
                // In order to get offset to first pixel, we need to 
                // perform following 
                //     BasePtr + Current row * (row width in bytes) 
                lBitmapCurPtr =(TUint32*)((TUint8*)bitmapData + ( ( lIndex / size.iWidth ) 
                        * lBmpSLLen ));
                lMaskCurPtr = maskData + ( ( lIndex  / size.iWidth ) 
                        * lMaskSLLen ); 
                } 
          else 
                { 
                // Still within the row, keep incrementing 
                lBitmapCurPtr++; 
                lMaskCurPtr++; 
                } 
            }
        iMask->UnlockHeap();
        iBitmap->UnlockHeap();
        }
    }

// *******************************************************
// From CSvgElementImpl

// perform a deep clone of this object
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
MXmlElement* CSvgImageElementImpl::CloneL(MXmlElement* aParentElement)
    {
    CSvgImageElementImpl* newElement = CSvgImageElementImpl::NewL(  this->ElemID(), ((CSvgDocumentImpl*)iOwnerDocument) );

    CleanupStack::PushL(newElement);
    newElement->iParentNode = aParentElement;
    // copy everything over
    this->CopyL(newElement);
    CleanupStack::Pop();
    // Add the cloned element to image decode notification list. 
    // This is done to ensure that the image is decoded only once.
    AddImageElementListener( newElement );
    newElement->SetReferenceElement( this );
    return newElement;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TBool CSvgImageElementImpl::DrawL( CGfx2dGc* aGc, CSvgElementImpl* aElement )
    {
    // Set Gc
    this->DrawShapeL( aGc, aElement );

    if ( iBitmap == NULL )
        return ETrue;
    TSize imageSize = iBitmap->SizeInPixels(); //.
    if ( imageSize.iWidth == 0 || imageSize.iHeight == 0 )
        return ETrue;

    const TGfxAffineTransform& currentTM = GetCTM();

    aGc->SetTransform( currentTM );

    if ( iBitmapReady && iRenderImage && iBitmap )
        {
        aGc->DrawImage( iBitmap, iImgRect, iMask != NULL );
        }
    else if ( iBitmapOrgReady && iRenderImage )
        {
        aGc->SetForegroundColor( TGfxColor( 0xaaaaaa ) );
        aGc->SetPaint( NULL );
        aGc->DrawL( &iImgRect );
        }
    return ETrue;
    }

// *******************************************************
// Private

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgImageElementImpl::CSvgImageElementImpl( CSvgDocumentImpl* aDoc ) :  
    iRenderImage ( ETrue ),
    iIsUriSet( EFalse ),
    iImageDecodingDone(EFalse),
    iBitmapReady( EFalse )
    {
    SetOwnerDocument(aDoc);
    iBitmap = NULL;
    iMask   = NULL;
    }

// ---------------------------------------------------------------------------
// perform a deep copy of this object
// ---------------------------------------------------------------------------
void CSvgImageElementImpl::CopyL( CSvgImageElementImpl* aDestElement )
    {
    if(aDestElement)
        {
          // copy stuff from superclass
        this->CSvgElementImpl::CopyL(aDestElement);

        // copy items special to CSvgImageElementImpl
        aDestElement->iUsedImage      = ETrue;          // this is not original image
        delete aDestElement->iBitmap;                  // delete old
        aDestElement->iBitmap         = this->iBitmap; // borrow new
        aDestElement->iImgRect        = this->iImgRect;
        aDestElement->iPar            = this->iPar;
        aDestElement->iBitmapOrgReady = ETrue;
        aDestElement->iBitmapReady    = ETrue;
        aDestElement->iRenderImage    = ETrue;
		aDestElement->iIsBase64 = this->iIsBase64;

        // copy the reference to idoc (CSvgDocumentImpl)
        aDestElement->iOwnerDocument = this->iOwnerDocument;
        }


    }

/********************* Helping Class **************************/

// ---------------------------------------------------------------------------
// Two-phased constructor
// ---------------------------------------------------------------------------
CSvgImageElementImpl::CSvgImageLoaderUtil* CSvgImageElementImpl::CSvgImageLoaderUtil::NewL( 
    MSVGImageLoadingObserver* aObserver,
    RFile& aFileHandle,
    TBool aSyncLoading )
    {
    CSvgImageLoaderUtil* self = new ( ELeave ) CSvgImageLoaderUtil( );
    CleanupStack::PushL( self );
    self->ConstructL( aObserver, aFileHandle, aSyncLoading );
    CleanupStack::Pop();
    return self;
    }

// ---------------------------------------------------------------------------
// Two-phased constructor
// ---------------------------------------------------------------------------
CSvgImageElementImpl::CSvgImageLoaderUtil* CSvgImageElementImpl::CSvgImageLoaderUtil::NewL( 
    MSVGImageLoadingObserver* aObserver,
    const TDesC8& aEncodedData,
    TBool aSyncLoading )
    {
    CSvgImageLoaderUtil* self = new ( ELeave ) CSvgImageLoaderUtil( );
    CleanupStack::PushL( self );
    self->ConstructL( aObserver, aEncodedData, aSyncLoading );
    CleanupStack::Pop();
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
CSvgImageElementImpl::CSvgImageLoaderUtil::~CSvgImageLoaderUtil()
    {
    Cancel();
    if(iImageDecoder)
        {
        delete iImageDecoder;
        iImageDecoder = NULL;
        }
    if ( iActiveSchedulerWait )
        {
        delete iActiveSchedulerWait;
        }
    if(iImageByteData)
    	{
    	delete iImageByteData;
    	iImageByteData=NULL;
    	}
    }

// ---------------------------------------------------------------------------
// Set the display mode (color mode), the default value is EColor64K.
// ---------------------------------------------------------------------------
void CSvgImageElementImpl::CSvgImageLoaderUtil::SetDisplayMode( TDisplayMode aDisplayMode )
    {
    iDisplayMode = aDisplayMode;
    }

// ---------------------------------------------------------------------------
// Returns the FrameInfo of Image
// ---------------------------------------------------------------------------
TFrameInfo CSvgImageElementImpl::CSvgImageLoaderUtil::GetFrameInfo()
    {
	return iImageDecoder->FrameInfo(); 
    }


// ---------------------------------------------------------------------------
// Request asynchronously for the image, sized to the image size.
// ---------------------------------------------------------------------------
void CSvgImageElementImpl::CSvgImageLoaderUtil::RequestForImageL( CFbsBitmap& aBitmap,
                                                                  CFbsBitmap& aMask,
                                                                  TBool       aHasAlpha,
                                                                  TFrameInfo  aFrameInfo )
    {
    if ( iImageDecoder != NULL )
        {
        if ( aHasAlpha )
            {
            User::LeaveIfError( aMask.Create( aFrameInfo.iOverallSizeInPixels, EGray256 ) );
            User::LeaveIfError( aBitmap.Create( aFrameInfo.iOverallSizeInPixels, EColor16MU ) );
            iImageDecoder->Convert( &iStatus, aBitmap, aMask, 0 );
            }
        else
            {
            User::LeaveIfError( aBitmap.Create( aFrameInfo.iOverallSizeInPixels, EColor64K ) );
            iImageDecoder->Convert( &iStatus, aBitmap, 0 );
            }

        SetActive();
        if ( iActiveSchedulerWait )
            {
            iActiveSchedulerWait->Start();
            }
        }
    else
        {
        iObserver->ImageLoadingCompleted( KErrNotFound );
        }
    }

// ---------------------------------------------------------------------------
// Override method from CActive, called upon successful completion of request
// ---------------------------------------------------------------------------
void CSvgImageElementImpl::CSvgImageLoaderUtil::RunL()
    {
    if ( iActiveSchedulerWait )
        {
        iActiveSchedulerWait->AsyncStop();
        }
    if ( iObserver )
        {
        iObserver->ImageLoadingCompleted( iStatus.Int() );
        }
    }

// ---------------------------------------------------------------------------
// Override method from CActive, called upon a cancel of request
// ---------------------------------------------------------------------------
void CSvgImageElementImpl::CSvgImageLoaderUtil::DoCancel()
    {
    if ( iImageDecoder != NULL )
        {
        iImageDecoder->Cancel();
        }
    
    // must delete decoder immediate or exception occurs in destructor.
    delete iImageDecoder;
    iImageDecoder = NULL;
    iObserver->ImageLoadingCompleted( KErrCancel );
    }

// ---------------------------------------------------------------------------
// Override method from CActive, called upon an error from request
// ---------------------------------------------------------------------------
TInt CSvgImageElementImpl::CSvgImageLoaderUtil::RunError( TInt aError )
    {
    // must delete decoder immediate or exception occurs in destructor.
    delete iImageDecoder;
    iImageDecoder = NULL;
    iObserver->ImageLoadingCompleted( aError );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
CSvgImageElementImpl::CSvgImageLoaderUtil::CSvgImageLoaderUtil() : CActive( EPriorityNormal )
    {
    SetDisplayMode( EColor64K );
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// Second phase Construction
// ---------------------------------------------------------------------------
void CSvgImageElementImpl::CSvgImageLoaderUtil::ConstructL( 
    MSVGImageLoadingObserver* aObserver, 
    RFile& aFileHandle,
    TBool aSyncLoading )
    {
    iObserver = aObserver;
    iIsSyncLoading=aSyncLoading;
    
    // Reset to start of file
    TInt zero = 0;
    aFileHandle.Seek( ESeekStart, zero );

    TInt fileSize = 0;
    TInt sizeError = aFileHandle.Size( fileSize );
    if ( sizeError != KErrNone )
    	{
      
     	return;
       	}

 	
	iImageByteData = HBufC8::New ( fileSize );

	 if (!iImageByteData)
    	{
        
       	return;
       	}
	
	TPtr8 binaryBufferPtr = iImageByteData->Des();
	
	// Read the image data into a buffer.
	TInt readError = aFileHandle.Read(binaryBufferPtr, fileSize);
	if ( readError != KErrNone )
    	{
        delete iImageByteData;
        iImageByteData = NULL;
        return;
    	}
    // Unused parameter	
    RFs session;
    iImageDecoder = CImageDecoder::DataNewL( session, *iImageByteData, CImageDecoder::TOptions( CImageDecoder::EAllowGeneratedMask | 
        CImageDecoder::EOptionAlwaysThread )); 
    iImageDecoder->SetDecoderThreadPriority( EPriorityNormal );
	
    if ( aSyncLoading )
        {
        iActiveSchedulerWait = new (ELeave) CActiveSchedulerWait();
        }
    }

void CSvgImageElementImpl::CSvgImageLoaderUtil::ConstructL( MSVGImageLoadingObserver* aObserver,
                                                            const TDesC8& aEncodedData,
                                                            TBool aSyncLoading )
    {
    iObserver = aObserver;
    iIsSyncLoading=aSyncLoading;
    // Unused parameter
    RFs session;
    iImageDecoder = CImageDecoder::DataNewL( session, aEncodedData, CImageDecoder::TOptions( CImageDecoder::EAllowGeneratedMask | 
        CImageDecoder::EOptionAlwaysThread ));
    iImageDecoder->SetDecoderThreadPriority( EPriorityNormal );
    if ( aSyncLoading )
        {
        iActiveSchedulerWait = new (ELeave) CActiveSchedulerWait();
        }
    }

TInt CSvgImageElementImpl::GetAttributeDes( const TInt aNameId,
                                                     TPtrC16& aValue )
{
 if(aNameId == KAtrPreserveAspectRatio)
     {
     aValue.Set( Par() );
     return KErrNone;
     }
 // added for updation of Get/Set
 if( aNameId == KAtrXlinkhref )
    {
     aValue.Set( Href() );
     return KErrNone;
    }
 return CSvgElementImpl::GetAttributeDes( aNameId, aValue );

}

// MSvgImageDecodingRequester method
TInt CSvgImageElementImpl::StartImageDecoding( const TDesC8& aImageData,
                                               TBool aSyncLoading )
    {
    if ( iBitmap != NULL )
        {
        
        if( iIsSvgFile )
            {
        	// SVG files are decoded by engine and do not follow the ImageDecoder Path
        	PrepareImageFromSvgFile(aImageData);             
            return KErrNone;
            }
    	if( iImageDecodingDone )
    	    {
    	    CSvgDocumentImpl* document = (CSvgDocumentImpl*)OwnerDocument();
    	    if(document)
    	        {
    	        TInt incCnt = document->GetImageElementsCount() + 1;
        	    document->SetImageElementsCount(incCnt);
                }
            
    	    }
    	else
    	    {
    	    iImageDecodingDone = ETrue;
    	    }
        // Check if the iImageLoader already exists for the ImageElement
        TInt err = KErrNone;
        
        if( iImageLoader == NULL )
        {
        	TRAP(err, iImageLoader = CSvgImageLoaderUtil::NewL( this, 
            aImageData,
            aSyncLoading ));  // EFalse means Asynchronous decoding	
        }
        else
        {
        	// Cancel the currently active request if the request is pending.
        	if( iImageLoader)
        	{
        		delete iImageLoader;
        		iImageLoader = NULL;
        	}
        	
        	TRAP(err, iImageLoader = CSvgImageLoaderUtil::NewL( this, 
            aImageData,
            aSyncLoading ));  // EFalse means Asynchronous decoding	
        }
        
        if ( err != KErrNone || !iImageLoader )
            {
            #ifdef _DEBUG
            RDebug::Printf("CSvgImageElementImpl::StartImageDecoding failed");
            #endif
            ImageLoadingCompleted(err);
            return err;
            }

        TFrameInfo lFrameInfo( iImageLoader->GetFrameInfo() );
		TBool      lHasAlpha = (lFrameInfo.iFlags & TFrameInfo::ETransparencyPossible) != 0;
       
        if( lHasAlpha )
            {
        	if( iMask )
        	    {
        		delete iMask;
        		iMask = NULL;
        	    }
        	
        	iMask = new (ELeave) CFbsBitmap();
            }
        
        TRAPD(err2, iImageLoader->RequestForImageL( *iBitmap, *iMask, lHasAlpha, lFrameInfo ));
        if ( err2 != KErrNone )
            {
            #ifdef _DEBUG
            RDebug::Printf("CSvgImageElementImpl::StartImageDecoding RequestForImageL() failed");
            #endif
            ImageLoadingCompleted(err2);
            return err2;
            }
        }
    return KErrNone;
    }


TInt CSvgImageElementImpl::StartImageDecoding( RFile& aImageFile,
                                               CFbsBitmap* aBitmap,
                                               TBool aSyncLoading )
    {
    if ( iBitmap )
        {
        // Destroy the existing bitmap
        delete iBitmap;
        }
    // Take ownership of the bitmap
    iBitmap = aBitmap;

    if ( iBitmap != NULL )
        {
        if( iIsSvgFile )
            {
               	
        	TInt length;
        	aImageFile.Size(length);
        	
        	HBufC8 *aImageData = HBufC8::NewL( length );
           	TPtr8 des = aImageData->Des();
        	
        	TInt readError = aImageFile.Read( des );
        	if ( readError != KErrNone )
        		{
            	delete aImageData;
        		return KErrGeneral;
        		}
        	else
        		{
        		// SVG files are decoded by engine and do not follow the ImageDecoder Path
        		PrepareImageFromSvgFile(des);             
            	return KErrNone;
        		}        	
            }
    	if( iImageDecodingDone )
    	    {
    	    CSvgDocumentImpl* document = (CSvgDocumentImpl*)OwnerDocument();
    	    if(document)
    	        {
    	        TInt incCnt = document->GetImageElementsCount() + 1;
        	    document->SetImageElementsCount(incCnt);
                }
            
    	    }
    	else
    	    {
    	    iImageDecodingDone = ETrue;
    	    }
        
        TInt err = KErrNone;
        // Check if the iImageLoader already exists for the ImageElement
        if( iImageLoader == NULL )
            {
        	TRAP( err, iImageLoader = CSvgImageLoaderUtil::NewL( this, 
            aImageFile,
            aSyncLoading ));  // EFalse means Asynchronous decoding	
            }
        else
            {
        	// Cancel the currently active request if the request is pending.
        	if( iImageLoader)
        	   {
        	   delete iImageLoader;
        	   iImageLoader = NULL;
        	   }
        	
        	TRAP(err, iImageLoader = CSvgImageLoaderUtil::NewL( this, 
            aImageFile,
            aSyncLoading ));  // EFalse means Asynchronous decoding	
            }
        
        if ( err != KErrNone || !iImageLoader )
            {
            #ifdef _DEBUG
            RDebug::Printf("CSvgImageElementImpl::StartImageDecoding failed");
            #endif
            ImageLoadingCompleted(err);
            return err;
            }
        // Get the Frame info and based on it create the iMask if required.
        TFrameInfo lFrameInfo( iImageLoader->GetFrameInfo() );
		TBool      lHasAlpha = (lFrameInfo.iFlags & TFrameInfo::ETransparencyPossible) != 0;
       
        if( lHasAlpha )
            {
        	if( iMask )
        	    {
        		delete iMask;
        		iMask = NULL;
        	    }
        	
        	iMask = new (ELeave) CFbsBitmap();
            }
            
            
        TRAPD(err2, iImageLoader->RequestForImageL( *iBitmap, *iMask, lHasAlpha, lFrameInfo ));
        
        if ( err2 != KErrNone )
            {
            #ifdef _DEBUG
            RDebug::Printf("CSvgImageElementImpl::StartImageDecoding RequestForImageL() failed");
            #endif
            ImageLoadingCompleted(err);
            return err2;
            }
        }
    return KErrNone;
    }
// ---------------------------------------------------------------------------
// CSvgImageElementImpl::SetReferenceElement
//  Informs the image element that it is referring to a particular image 
//  element.
// ---------------------------------------------------------------------------
void CSvgImageElementImpl::SetReferenceElement( CSvgImageElementImpl* 
    aImageElement )
    {
    iRefElement = aImageElement;
    }

// ---------------------------------------------------------------------------
// CSvgImageElementImpl::GetReferenceElement
//  Get Function for reference element
// ---------------------------------------------------------------------------
CSvgImageElementImpl* CSvgImageElementImpl::ReferenceElement()
    {
    return iRefElement;
    }

// ---------------------------------------------------------------------------
// CSvgImageElementImpl::AddImageElementListener
//  Adds an image element listener - used in the case of cloning of image 
//  element.
// ---------------------------------------------------------------------------
void CSvgImageElementImpl::AddImageElementListener( 
    MSvgImageElementListener* aListener )
    {
    if ( iImageElementListeners.Find( aListener ) != KErrNotFound )
        {
        return;
        }
    if ( iImageElementListeners.Append( aListener ) != KErrNone )
        {
        #ifdef _DEBUG
        RDebug::Printf("iImageElementListeners.Append failed");
        #endif
        }
    }

// ---------------------------------------------------------------------------
// CSvgImageElementImpl::RemoveImageElementListener
//  Removes an image element listener - used in the case of cloning of image 
//  element.
// ---------------------------------------------------------------------------
void CSvgImageElementImpl::RemoveImageElementListener( 
    MSvgImageElementListener* aListener )
    {
    TInt index = iImageElementListeners.Find( aListener );
    if ( index == KErrNotFound )
        {
        return;
        }
    iImageElementListeners.Remove( index );
    }

// ---------------------------------------------------------------------------
// CSvgImageElementImpl::BitmapDecoded
//  This is the callback implementation for updating the decoded image 
//  and mask from referenced image element.
// ---------------------------------------------------------------------------
void CSvgImageElementImpl::BitmapDecoded( CFbsBitmap* aBitmap, 
    CFbsBitmap* aMask )
    {
    // Save the pointers to the decoded bitmap in this element,
    // this can be used in subsequent draw.
    // Note: Both the bitmap and mask are not owned by this element.
    // It is owned by the reference element.
    iBitmap = aBitmap;
    iMask = aMask;

    iBitmapReady = iBitmap != NULL;
    }

// ---------------------------------------------------------------------------
// CSvgImageElementImpl::ResetReference
//  This is the callback implementation for resetting reference element 
//  pointer
// ---------------------------------------------------------------------------
void CSvgImageElementImpl::ResetReference()
    {
    // Stop referring to the reference element.
    iRefElement = NULL;
    }

// ---------------------------------------------------------------------------
// CSvgImageElementImpl::NotifyImageDecoded
//  Notify listeners that image has been decoded.
// ---------------------------------------------------------------------------
void CSvgImageElementImpl::NotifyImageDecoded()
    {
    TInt imageEleListenersCnt = iImageElementListeners.Count();
    for ( TInt i = 0; i < imageEleListenersCnt; i++ )
        {
        iImageElementListeners[i]->BitmapDecoded( iBitmap, iMask );
        }
}

// ---------------------------------------------------------------------------
// CSvgImageElementImpl::ResetReference
//  Notify listeners to reset their reference element pointers
// ---------------------------------------------------------------------------
void CSvgImageElementImpl::NotifyResetReference()
    {
    TInt imageEleListenersCnt = iImageElementListeners.Count();
    for ( TInt i = 0; i < imageEleListenersCnt; i++ )
        {
        iImageElementListeners[i]->ResetReference();
        }
}

void CSvgImageElementImpl::Print( TBool aIsEncodeOn )
{
	if (!aIsEncodeOn)
	{
		#ifdef _DEBUG
		RDebug::Printf("<image x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\" xlink:href=\"hmm\" \\>", (int)X(), (int)Y(), (int)Width(), (int)Height()/*, Href()*/);
		#endif
	}
}

CFbsBitmap* CSvgImageElementImpl::GetBitmap()
    {
    return iBitmap;
    }

void CSvgImageElementImpl::SetBitmap( CFbsBitmap* aBitmap )    
    {
    if ( iBitmap )
        {
    	delete iBitmap;
    	iBitmap = NULL;
        }
	iBitmap = aBitmap;
    }

void CSvgImageElementImpl::AssignImageData( HBufC8* aData )
    {
    if(iImageData)
        {
        delete iImageData;
        iImageData = NULL;
        }
    iImageData = aData;
    }

TInt CSvgImageElementImpl::CheckRequiredAttributes()
{
	if ( !( iImgRect.iHeight >= TFloatFixPt(0) ) )
	{
		//need a height
		iReqAttrFlag = KAtrHeight;
		return iReqAttrFlag;
	}

	if ( !( iImgRect.iWidth >= TFloatFixPt(0)) )
	{
		iReqAttrFlag = KAtrWidth;
		return iReqAttrFlag;
	}

	
	if ( !iIsUriSet )
	{
		iReqAttrFlag = KAtrXlinkhref;
		return iReqAttrFlag;
	}

	iReqAttrFlag = 0;
	
	// check if the diplay property is turned off programatically
	if( WasTurnedOff() )
	{
	    _LIT(KInline, "inline");
	    // turn it on. this means all the required attributes Are present and
	    // hence we can render it.
	    CSvgElementImpl::SetPropertyL(KCSS_ATTR_DISPLAY, KInline);
	    SetTurnOff( EFalse );
	}
	
	return 0;
}
// ---------------------------------------------------------------------------
// CSvgImageElementImpl::IsSynchronousLoading
// Returns whether the image decoding was synchronous or not
// ---------------------------------------------------------------------------
TBool CSvgImageElementImpl::CSvgImageLoaderUtil::IsSynchronousLoading()
	{
	return iIsSyncLoading;
	}
