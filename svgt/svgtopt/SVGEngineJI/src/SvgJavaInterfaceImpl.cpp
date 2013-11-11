/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  SVG Engine source file
 *
*/

#include <SvgJavaInterfaceImpl.h>
#include "SVGErrorImpl.h"
#include "SVGjsrconstants.h"

//utility classes only can be removed in final version
#include <hal.h>
#include <hal_data.h>
#include <imageconversion.h>

// ***********************************************************************
// Java error
// ***********************************************************************

// ==========================================================================
// Two-phase constructor
// ==========================================================================
TInt CJavaError::NewL( CSvgErrorImpl& aError )
    {
    CJavaError* self = new ( ELeave ) CJavaError();
    CleanupStack::PushL( self );
    self->ConstructL( aError );
    CleanupStack::Pop();
    MJavaError* selfInterface = self;
	return ( reinterpret_cast<TUint>( selfInterface ) >> 2 );
    }

// ==========================================================================
// Destructor
// ==========================================================================
CJavaError::~CJavaError()
    {
    if(iDescription)
		{
		delete iDescription;
		iDescription = NULL;
		}
    }

// ==========================================================================
// return Etrue if an error is indicated by this object.
// ==========================================================================
TBool CJavaError::HasError() const
{
    return iErrorCode != KErrNone;
}

// ==========================================================================
// return Etrue if the error indicated by this object is only a warning.
// ==========================================================================
TBool CJavaError::IsWarning() const
{
    return iIsWarning;
}

// ==========================================================================
// Get the error code.
// ==========================================================================
TInt CJavaError::ErrorCode() const
{
    return iErrorCode;
}

// ==========================================================================
// Get the error description.
// ==========================================================================
TDesC8& CJavaError::Description()
{
    return *iDescription;
}

// ==========================================================================
// Second phase of construction
// ==========================================================================
void CJavaError::ConstructL( CSvgErrorImpl& aError )
{
	// Error code
    switch( aError.ErrorCode() )
    	{
    	case ESvgInvalidAttributeValue:
		case ESvgUnknown:
    		{
    		iErrorCode = KJavaIOException;
    		break;
			}
    	case KErrNoMemory:
    		{
    		iErrorCode = KErrNoMemory;
    		break;
    		}
    	default: iErrorCode = KJavaIOException;
    	}
	// Warning
	iIsWarning = aError.IsWarning();
	// Description
    iDescription = HBufC8::NewL( aError.Description().Length() + 1);
    TPtr8 ptr = iDescription->Des();
	ptr.Copy( aError.Description() );
    ptr.ZeroTerminate();
	}

// ==========================================================================
// Constructor
// ==========================================================================
CJavaError::CJavaError()
: iErrorCode( KErrNone ),
  iIsWarning( EFalse ),
  iDescription( NULL )
{
}
// ***********************************************************************
// SVG Engine
// ***********************************************************************

EXPORT_C CSvgJavaInterfaceImpl::CSvgJavaInterfaceImpl(): iTempDoc( NULL )
{
}

EXPORT_C CSvgJavaInterfaceImpl* CSvgJavaInterfaceImpl::NewL( TFontSpec& aFontSpec )
{
    CSvgJavaInterfaceImpl* self = new ( ELeave ) CSvgJavaInterfaceImpl();
    CleanupStack::PushL( self );
    self->ConstructL(aFontSpec);
    CleanupStack::Pop();
    return self;
}

EXPORT_C CSvgJavaInterfaceImpl* CSvgJavaInterfaceImpl::NewL()
{
    CSvgJavaInterfaceImpl* self = new ( ELeave ) CSvgJavaInterfaceImpl();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
}

void CSvgJavaInterfaceImpl::ConstructL( TFontSpec& aFontSpec )
{
    CSvgEngineInterfaceImpl::ConstructL(aFontSpec);
    iTempDoc = (CSvgDocumentImpl*)SvgDocumentCreateEmpty();

    AddListener( (MSvgMouseListener*)this, ESvgMouseListener );

    //TO BE ADDED IN THE FUTURE FOR TEXT AREA AND PROGRESSIVE RENDERING
    //AddListener( (MSvgTextAreaListener*)this, ESvgTextAreaListener );
    //AddListener( (MSvgTextListener*)this, ESvgTextListener );
    //AddListener( (MSvgLoadingListener*)this, ESvgLoadingListener );
}

void CSvgJavaInterfaceImpl::ConstructL()
{
    CSvgEngineInterfaceImpl::ConstructL();
    iTempDoc = (CSvgDocumentImpl*)SvgDocumentCreateEmpty();

    AddListener( (MSvgMouseListener*)this, ESvgMouseListener );

    //TO BE ADDED IN THE FUTURE FOR TEXT AREA AND PROGRESSIVE RENDERING
    //AddListener( (MSvgTextAreaListener*)this, ESvgTextAreaListener );
    //AddListener( (MSvgTextListener*)this, ESvgTextListener );
    //AddListener( (MSvgLoadingListener*)this, ESvgLoadingListener );
}

EXPORT_C CSvgJavaInterfaceImpl::~CSvgJavaInterfaceImpl()
{
	if (iTempDoc)
	{
		DestroyDocument( iTempDoc );
		iTempDoc = NULL;
	}
}

/**
 * Create an SvgEngine instance.
 */
EXPORT_C SvgEngineHandle CSvgJavaInterfaceImpl::SvgEngineCreate()
{
    CSvgEngineImpl *lSvgEngine = NULL;
    TRAPD(error,lSvgEngine = SvgEngineNewL());
    if(error == KErrNone)
        {
        return (SvgEngineHandle)lSvgEngine;
        }
    return 0;
}
/**
 * Request to set render quality.
 */
EXPORT_C void CSvgJavaInterfaceImpl::SvgEngineSetRenderQuality( SvgEngineHandle aEngineHandle, TInt aQuality )
{
	SetRenderQuality( aQuality, aEngineHandle );	
}

/**
 * Request to render the SVG document.
 */
EXPORT_C void CSvgJavaInterfaceImpl::SvgEngineRenderDocument( SvgEngineHandle aEngineHandle, SvgDocumentHandle aDocumentHandle, TInt aSurfaceHandle, TInt aSurfaceMaskHandle, TReal32 aCurrentTime )
{
    CSvgEngineInterfaceImpl::iFileIsLoaded = ETrue;

    SetDocument( (CSvgEngineImpl*)aEngineHandle, (CSvgDocumentImpl*)aDocumentHandle );

    // GfxContext creation
    TRAPD(error, SetGdiContextL( (CSvgEngineImpl*)aEngineHandle, (CFbsBitmap*)aSurfaceHandle ) );
    if ( error != KErrNone )
       {
       // ignore trap error
       }

	//this udpates things like Viewport with whatever preserveAspectRatio and widths are set
	//InitializeEngine((CSvgEngineImpl*)aEngineHandle);

	if ( ((CFbsBitmap*)aSurfaceMaskHandle) != NULL)
	GenerateMask((CFbsBitmap*)aSurfaceMaskHandle);

    RenderFrame( (CSvgEngineImpl*)aEngineHandle, (TUint)(aCurrentTime * 1000) );

	//utility functions   
    /*//TDisplayMode lDisplay = ((CFbsBitmap*)aSurfaceHandle)->DisplayMode();
    
   	//call it like this:
	TBuf<KMaxName> myText;
	myText.Copy(_L("Andrew's Test Bitmap"));

	ConvertBitmapToFileL((CFbsBitmap*)aSurfaceHandle, myText);
	*/
}

/*
 * M2G: SvgEngineRenderDocument() overloaded to pass the CSvgtBitmap object to SvgEngine
 */
EXPORT_C void CSvgJavaInterfaceImpl::SvgEngineRenderDocument( 
    SvgEngineHandle aEngineHandle, SvgDocumentHandle aDocumentHandle, 
    SvgtBitmapHandle aSurfaceHandle, SvgtBitmapHandle aSurfaceMaskHandle, 
    TReal32 aCurrentTime ) 
    {
    CSvgEngineInterfaceImpl::iFileIsLoaded = ETrue;

    SetDocument( (CSvgEngineImpl*)aEngineHandle, (CSvgDocumentImpl*)aDocumentHandle );

    // GfxContext creation
    TRAPD(error, SetGdiContextL( (CSvgEngineImpl*)aEngineHandle, (CSvgtBitmap*)aSurfaceHandle ) );
    if ( error != KErrNone )
       {
       // ignore trap error
       }

    //this udpates things like Viewport with whatever preserveAspectRatio and widths are set
    //InitializeEngine((CSvgEngineImpl*)aEngineHandle);

    if ( ((CSvgtBitmap*)aSurfaceMaskHandle) != NULL)
    GenerateMask((CSvgtBitmap*)aSurfaceMaskHandle);

    RenderFrame( (CSvgEngineImpl*)aEngineHandle, (TUint)(aCurrentTime * 1000) );
    }

/**
 *
 */
EXPORT_C void CSvgJavaInterfaceImpl::SvgEngineRenderDocumentL( 
		SvgEngineHandle aEngineHandle, SvgDocumentHandle aDocumentHandle, 
		TInt aSurfaceHandle, const TPoint& aAnchor, const TRect& aRenderArea, TReal32 aCurrentTime, TReal32 aAlpha ) __SOFTFP
	{
    CSvgEngineInterfaceImpl::iFileIsLoaded = ETrue;
    SetDocument( (CSvgEngineImpl*)aEngineHandle, (CSvgDocumentImpl*)aDocumentHandle );

	CFbsBitmap* target = reinterpret_cast<CFbsBitmap*>( aSurfaceHandle );	
	CFbsBitmap* source = new ( ELeave ) CFbsBitmap();

	User::LeaveIfError( source->Create( 
		TSize( SvgDocumentGetViewportWidth( aDocumentHandle ), 
		       SvgDocumentGetViewportHeight( aDocumentHandle ) ),
			   EColor16MA ) ); 
	
    // GfxContext creation
    TRAPD(error, SetGdiContextL( (CSvgEngineImpl*)aEngineHandle, source ) );
    if ( error != KErrNone )
       {
       // ignore trap error
       }

    RenderFrame( (CSvgEngineImpl*)aEngineHandle, (TUint)(aCurrentTime * 1000) );

	source->LockHeap();
	target->LockHeap();

	// Source 
	TUint32* sourceBuffer = source->DataAddress();
	const TInt sourceBufferWidth( source->SizeInPixels().iWidth );
	
	// Target 
	TUint32* targetBuffer = target->DataAddress();
	const TInt targetBufferWidth( target->SizeInPixels().iWidth );

	// Set source buffer x position
	if( aAnchor.iX < aRenderArea.iTl.iX ) 
		{ 
		sourceBuffer += ( aRenderArea.iTl.iX - aAnchor.iX ); 
		}
	// Set source buffer y position
	if( aAnchor.iY < aRenderArea.iTl.iY ) 
		{ 
		sourceBuffer += sourceBufferWidth * ( aAnchor.iY - aRenderArea.iTl.iY ); 
		}

	// Set target buffer x position
	targetBuffer += aRenderArea.iTl.iX;
	// Set target buffer y position
	targetBuffer += targetBufferWidth * aRenderArea.iTl.iY;

	TInt heightIndex = 0;
	TInt widthIndex = 0;
	// If fully opaque
	if( aAlpha == 1.0f )
		{
		// Line loop
		for( heightIndex = 0; heightIndex < aRenderArea.Height(); heightIndex++ )
			{
			// Pixel loop
			for( widthIndex = 0; widthIndex < aRenderArea.Width(); widthIndex++ )
				{
				// If alpha channel is not zero then do pixel coping
				if( ( 0xff000000 & *sourceBuffer ) != 0x0 ) 
					{ 
					*targetBuffer = *sourceBuffer;
					// Set the alpha value 0xFF 	
					*targetBuffer |= 0xff000000;
					}
				// Next pixel
				sourceBuffer++;
				targetBuffer++;
				}
			sourceBuffer = sourceBuffer - aRenderArea.Width() + sourceBufferWidth;
			targetBuffer = targetBuffer - aRenderArea.Width() + targetBufferWidth;
			}
		}
	// Partly opaque
	else
		{

		TUint alpha = STATIC_CAST( TUint8, ( aAlpha * 255 ) );
		TUint antiAlpha = 255 - alpha;

		TUint8* sourcePtr = NULL;
		TUint8* targetPtr = NULL;
		for( heightIndex = 0; heightIndex < aRenderArea.Height(); heightIndex++ )
			{
			// Pixel loop
			for( widthIndex = 0; widthIndex < aRenderArea.Width(); widthIndex++ )
				{
				// If alpha channel is not zero then do alpha plending
				if( ( 0xff000000 & *sourceBuffer ) != 0x0 ) 
					{ 
					targetPtr = REINTERPRET_CAST( TUint8*, targetBuffer );
					sourcePtr = REINTERPRET_CAST( TUint8*, sourceBuffer );
					// Blue component
					*targetPtr = ( TUint8 )( ( ( TUint )( *sourcePtr ) * alpha + 
											   ( TUint )( *targetPtr ) * antiAlpha ) / 255 ); 
     				sourcePtr++;
					targetPtr++;
					// Green component
					*targetPtr = ( TUint8 )( ( ( TUint )( *sourcePtr ) * alpha + 
											   ( TUint )( *targetPtr ) * antiAlpha ) / 255 ); 
     				sourcePtr++;
					targetPtr++;
					// Red component
					*targetPtr = ( TUint8 )( ( ( TUint )( *sourcePtr ) * alpha + 
											   ( TUint )( *targetPtr ) * antiAlpha ) / 255 ); 
     				// Alpha component
					sourcePtr++;
					targetPtr++;
					*targetPtr = *sourcePtr;
					// Set the alpha value 0xFF	
					*targetPtr |= 0xff000000;
					}
				// Next pixel
				sourceBuffer++;
				targetBuffer++;
				}
			// Next line
			sourceBuffer = sourceBuffer - aRenderArea.Width() + sourceBufferWidth;
			targetBuffer = targetBuffer - aRenderArea.Width() + targetBufferWidth;
			}
		}
		source->UnlockHeap();
		target->UnlockHeap();
		delete source;

	}

/**
 * Request to destroy the given engine by handle.
 */
EXPORT_C void CSvgJavaInterfaceImpl::SvgEngineDestroy( SvgEngineHandle aEngineHandle )

{
    DestroyEngine( (CSvgEngineImpl *)aEngineHandle );
}

/*
* Sets the animation back to time 0 and then starts the internal engine timer
*/
EXPORT_C void CSvgJavaInterfaceImpl::SvgEngineStart( SvgEngineHandle aEngineHandle )
{
	Start((CSvgEngineImpl*)aEngineHandle);
}

/*
* Stops the internal SVG engine timer
*/
EXPORT_C void CSvgJavaInterfaceImpl::SvgEngineStop( SvgEngineHandle aEngineHandle )
{
	Stop((CSvgEngineImpl*)aEngineHandle);
}

/*
* Resumes the internal SVG engine timer
*/
EXPORT_C void CSvgJavaInterfaceImpl::SvgEngineResume( SvgEngineHandle aEngineHandle )
{
	Resume((CSvgEngineImpl*)aEngineHandle);
}


// *****************************************************************************
// *** SVG Rendering Surface Functions
// *****************************************************************************
/**
 * Create a Svg Rendering Surface.
 */

 // SVG Engine creates the buffer for the surface according width and height. Currently, application gives us the buffer.

EXPORT_C SvgSurfaceHandle CSvgJavaInterfaceImpl::SvgRenderingSurfaceCreate( TInt aWidth, TInt aHeight )
{
    // First create the frame buffer with aWidth and aHeight
    TSize size(aWidth, aHeight);

    // Can't use ELeave because this function can't leave
    // Using ELeave but didn't handle (no trap or propagate to parents) causes more confusing.
    // Check for NULL is good enough.
    // Ignore CodeScanner error (Code Review Guide)
    CFbsBitmap *lFrameBuffer = new CFbsBitmap();

    // Has same effect as using ELeave
    if ( !lFrameBuffer )
        {
        return NULL;
        }

    TInt lErrorCode = KErrNone;
	TInt displayColors = 256*256;
	HAL::Get( HALData::EDisplayColors, displayColors );
	if ( displayColors == 256*256*256 )
    	{
   		lErrorCode = lFrameBuffer->Create( size, EColor16MU );
	    }
	else
	    {
   	 	lErrorCode = lFrameBuffer->Create( size, EColor64K );
	    }

    if(lErrorCode != KErrNone)
        {
        return NULL;
        }

    return (SvgSurfaceHandle) lFrameBuffer;

}
/**
 * Destroy a Svg Rendering Surface.
 */

EXPORT_C  void CSvgJavaInterfaceImpl::SvgRenderingSurfaceDestroy( SvgSurfaceHandle aSurface )
{
    if(aSurface)
        {
        delete (CFbsBitmap*)aSurface;
        return;
        }
}
/**
 * Get a pointer to the Svg Rendering surface.
 */

EXPORT_C TUint* CSvgJavaInterfaceImpl::SvgRenderingSurfaceGetBuffer( SvgSurfaceHandle aSurface )
{
    return (TUint*)aSurface;
}
/**
 * Get the width of the Svg Rendering Surface.
 */
EXPORT_C TInt CSvgJavaInterfaceImpl::SvgRenderingSurfaceGetWidth( SvgSurfaceHandle aSurface )
{
    if(aSurface)
        {
        return ((CFbsBitmap *)aSurface)->SizeInPixels().iWidth;
        }
    else
        {
        return 0;
        }
}
/**
 * Get the height of the Svg Rendering Surface
 */
EXPORT_C TInt CSvgJavaInterfaceImpl::SvgRenderingSurfaceGetHeight( SvgSurfaceHandle aSurface )
{
    if(aSurface)
        {
        return ((CFbsBitmap *)aSurface)->SizeInPixels().iHeight;
        }
    else
        {
        return 0;
        }
}

// ***********************************************************************
// SVG Document
// ***********************************************************************

/**
 * Create an empty svg document.
 */
EXPORT_C SvgDocumentHandle CSvgJavaInterfaceImpl::SvgDocumentCreateEmpty()
{
    CSvgDocumentImpl *lSvgDoc = NULL;
    TRAPD(error,lSvgDoc = SvgDocumentNewL());
    if(error == KErrNone)
        {
        }

     //sets up the root element
    InitRootElement(lSvgDoc);

    return ((SvgDocumentHandle)lSvgDoc);
}

/**
 * Create a svg document by parsing the given STRING (not file).
 */
EXPORT_C SvgDocumentHandle CSvgJavaInterfaceImpl::SvgDocumentCreateL( const TPtrC16& aString )
{
    CSvgDocumentImpl *lSvgDoc = NULL;
    TRAPD(error ,lSvgDoc = SvgDocumentNewL());
    if(error != KErrNone)
        {
        	#ifdef _DEBUG
        	RDebug::Printf("JavaInterfaceImpl: Document Failed to Create");
        	#endif

        return NULL;
        }

    TRAPD( error2, FillDocumentL( lSvgDoc, aString ) );
    if(error2 != KErrNone)
        {
        // error is set in iSvgError
        }

    if( iSvgError && iSvgError->HasError() )
    	{
    		if( lSvgDoc != NULL )
        	{
				DestroyDocument(lSvgDoc);
				lSvgDoc = NULL;
        	}

		User::Leave( CJavaError::NewL( *iSvgError ) );
    	}

    return  ((SvgDocumentHandle)lSvgDoc);
}

/**
 * Destroy the given svg document (by handle).
 */
EXPORT_C void CSvgJavaInterfaceImpl::SvgDocumentDestroy( SvgDocumentHandle aDocumentHandle )
{
    DestroyDocument( (CSvgDocumentImpl*)aDocumentHandle );
}

/**
 * Svg Document RequestCompleted
 */
 // Returns the data from a resource handler to be attached to
 // the elements with the corresponding uri
EXPORT_C TInt CSvgJavaInterfaceImpl::SvgDocumentRequestCompleted( SvgDocumentHandle aDocument, const TPtrC16& aUri, const TPtrC8& aData)
{
    if ( aUri.Length() > 0 && aDocument != NULL)
    {
        AddExternalData((CSvgDocumentImpl*)aDocument, aUri, aData, ETrue, aData.Size());
       
        return 1;
    }
    return 0;
}

/**
 * Svg Document GetExternalListItem.
 */
 // Returns a character ptr to a URI for the element at (index) in
 // the external elements list (any item that has a uri or image link)

EXPORT_C TInt CSvgJavaInterfaceImpl::SvgDocumentGetExternalListItem(SvgDocumentHandle aDocumentHandle, TInt aIndex, TPtrC16& aItem)
{
    //returns the uri associated with the element(at index) that is requires an external resource

    if (aIndex < GetExternalListSize( reinterpret_cast< CSvgDocumentImpl* >( aDocumentHandle ) ) )
    {
	  TRAPD(error1, GetExternalListItemL( 
				( reinterpret_cast< CSvgDocumentImpl* >( aDocumentHandle ) ), aIndex, aItem ));
		
      return error1; 	    
    }
    
	return KErrNotFound;		
}

/**
 * Svg Document get external list item.
 */
 // What is the purpose of this api
EXPORT_C TInt CSvgJavaInterfaceImpl::SvgDocumentGetExternalListSize( SvgDocumentHandle aDocumentHandle )
{
    //the total number of external elements requested in the document i.e. the number of uris

    return (GetExternalListSize((CSvgDocumentImpl*)aDocumentHandle));
}

/**
 * Set Media time for the specified document.
 */
EXPORT_C  void CSvgJavaInterfaceImpl::SvgDocumentSetMediaTime( SvgDocumentHandle aDocumentHandle, TReal32 seconds ) __SOFTFP
{
    if(aDocumentHandle)
        {
        // this means that the current time for the document needs to be changed.
        TUint32 lTimeInMilliSeconds = (TUint32)(seconds * 1000);
        SvgSetMediaTime((CSvgDocumentImpl*)aDocumentHandle , lTimeInMilliSeconds);
        }
}

/**
 * Get Media time for the specified document.
 */
EXPORT_C TReal32 CSvgJavaInterfaceImpl::SvgDocumentGetMediaTime( SvgDocumentHandle aDocumentHandle ) __SOFTFP
{
    if(aDocumentHandle)
        {
        TReal32 ltime = SvgGetMediaTime((CSvgDocumentImpl*) aDocumentHandle );
        // convert in to seconds.
        if(ltime)
            {
            return (ltime / 1000);
            }
         // media time is 0
        }
    return 0;
}

/**
 * Get the viewport width for the given document.
 */
EXPORT_C TInt CSvgJavaInterfaceImpl::SvgDocumentGetViewportWidth( SvgDocumentHandle aDocumentHandle)
{
    return ( aDocumentHandle) ? GetViewportWidth( (CSvgDocumentImpl*)aDocumentHandle ) : 0;
}

/**
 * Get the viewport height for the given document.
 */
EXPORT_C TInt CSvgJavaInterfaceImpl::SvgDocumentGetViewportHeight( SvgDocumentHandle aDocumentHandle )
{
    return ( aDocumentHandle) ? GetViewportHeight( (CSvgDocumentImpl*)aDocumentHandle ) : 0;
}

/**
 * Set the viewport width for the given document.
 */
EXPORT_C void CSvgJavaInterfaceImpl::SvgDocumentSetViewportWidth( SvgDocumentHandle aDocumentHandle, TInt aWidth )
{
    if ( aDocumentHandle ) SetViewportWidth( (CSvgDocumentImpl*)aDocumentHandle, aWidth );
}

/**
 * Set the viewport height for the given document.
 */
EXPORT_C void CSvgJavaInterfaceImpl::SvgDocumentSetViewportHeight( SvgDocumentHandle aDocumentHandle, TInt aHeight )
{
    if ( aDocumentHandle ) SetViewportHeight( (CSvgDocumentImpl*)aDocumentHandle, aHeight );
}

EXPORT_C void CSvgJavaInterfaceImpl::SvgDocumentViewportInit( SvgDocumentHandle aDocumentHandle )
{
    if ( aDocumentHandle ) ViewportInit( (CSvgDocumentImpl*)aDocumentHandle );
}

/**
 * Get the viewport width units for the given document.
 */
EXPORT_C TInt CSvgJavaInterfaceImpl::SvgDocumentGetViewportWidthUnits( SvgDocumentHandle aDocumentHandle )
{
   return ( aDocumentHandle ) ? GetViewportUnits( (CSvgDocumentImpl*)aDocumentHandle ) : 0;
}

/**
 * Get the viewport height units for the given document.
 */
EXPORT_C TInt CSvgJavaInterfaceImpl::SvgDocumentGetViewportHeightUnits( SvgDocumentHandle aDocumentHandle)
{
    return ( aDocumentHandle) ? GetViewportUnits( (CSvgDocumentImpl*)aDocumentHandle ) : 0;
}

/**
 * Get the root element for the given document.
 */
EXPORT_C SvgElementHandle CSvgJavaInterfaceImpl::SvgDocumentGetRootElement( SvgDocumentHandle aDocumentHandle )
{
    return (SvgElementHandle)GetRootElement( (CSvgDocumentImpl*)aDocumentHandle );
}

/**
 * Get the svg element identified by the given id-string.
 */
EXPORT_C SvgElementHandle CSvgJavaInterfaceImpl::SvgDocumentGetElementById( SvgDocumentHandle aDocumentHandle, const TPtrC16& aID )
{
    if ( aID.Length() > 0 )
    {
        SvgElementHandle elementHandle =
            (SvgElementHandle)GetElementById( (CSvgDocumentImpl*)aDocumentHandle, aID );

        return elementHandle;
    }
    else
    {
        return 0;
    }
}

EXPORT_C TInt CSvgJavaInterfaceImpl::SvgDocumentGetNumberOfIds(SvgDocumentHandle aDocumentHandle )
{
    return GetNumberOfIds((CSvgDocumentImpl*)aDocumentHandle);
}

EXPORT_C TInt CSvgJavaInterfaceImpl::SvgDocumentGetId(SvgDocumentHandle aDocumentHandle, TInt index, TPtrC16& aId)
{
    TDesC* myId = GetId((CSvgDocumentImpl*)aDocumentHandle, (TInt)index);
    if ( myId == NULL )
        return NULL;
    
    aId.Set( *myId );
	
	return KErrNone;
}

/**
 * Set the begin animation time for the given element.
 */
EXPORT_C void CSvgJavaInterfaceImpl::SvgDocumentBeginElementAt( SvgDocumentHandle aDocumentHandle,
                                    SvgElementHandle aElementHandle,
                                    TReal32 aOffsetTime ) __SOFTFP
{
    if(aDocumentHandle)
        {
        if(aElementHandle)
            {
            // should their be any conversion between the time. ie possible
            // multiplication. The below conversion is done thinking that the
            // aOffsetTime value represents the time in seconds.
            TUint32 lTime = (TUint32)(aOffsetTime * 1000);
            SvgBeginElementAt( (CXmlElementImpl*)aElementHandle , lTime, (CSvgDocumentImpl*)aDocumentHandle );
            }
        }
}
/**
 * Set the end animation time for the given element.
 */
EXPORT_C void CSvgJavaInterfaceImpl::SvgDocumentEndElementAt( SvgDocumentHandle aDocumentHandle,
                                  SvgElementHandle aElementHandle,
                                  TReal32 aOffsetTime ) __SOFTFP
{
    if(aDocumentHandle)
        {
        if(aElementHandle)
            {
            // should their be any conversion between the time. ie possible
            // multiplication. The below conversion is done thinking that the
            // aOffsetTime value represents the time in seconds.
            TUint32 lTime = (TUint32)(aOffsetTime * 1000);
            SvgEndElementAt((CXmlElementImpl*)aElementHandle , lTime, (CSvgDocumentImpl*)aDocumentHandle );
            }
        }
}

/**
 * Set the given element to have the focus.
 */
EXPORT_C void CSvgJavaInterfaceImpl::SvgDocumentFocusOn( SvgDocumentHandle aDocumentHandle,
                             SvgElementHandle aElementHandle )
 {
    if(aDocumentHandle && aElementHandle)
        {
        SetFocusElement((CXmlElementImpl*)aElementHandle,(CSvgDocumentImpl*)aDocumentHandle);
        DispatchFocusInEvent((CSvgDocumentImpl*)aDocumentHandle,(CSvgElementImpl *)aElementHandle);
        }
 }
 
 
 /**
 * Unset the focus from the given element passed as parameter.
 */
EXPORT_C void CSvgJavaInterfaceImpl::SvgDocumentFocusOut( SvgDocumentHandle aDocumentHandle,
                             SvgElementHandle aElementHandle)
 {
    if(aDocumentHandle && aElementHandle)
        {
       	DispatchFocusOutEvent((CSvgDocumentImpl*)aDocumentHandle,(CSvgElementImpl *)aElementHandle);
        }
 }

/**
 * Get the element having the focus.
 */
EXPORT_C SvgElementHandle CSvgJavaInterfaceImpl::SvgDocumentGetFocus( SvgDocumentHandle aDocumentHandle )
{
    if(aDocumentHandle)
        {
        return (SvgElementHandle)GetFocusedElement((CSvgDocumentImpl*)aDocumentHandle);
        }
    else
        {
        return 0;
        }
}

/**
 * Activate the element that has the focus.
 */
EXPORT_C void CSvgJavaInterfaceImpl::SvgDocumentActivate( SvgDocumentHandle aDocumentHandle )
{
    if(aDocumentHandle)
        {
        SvgActivateAnimation((CSvgDocumentImpl*)aDocumentHandle);
        }
}

/**
 * Request a mouse event at the given coordinate.
 */
EXPORT_C SvgElementHandle CSvgJavaInterfaceImpl::SvgDocumentDispatchMouseEvent( SvgDocumentHandle aDocumentHandle,
                                         TInt aMouseX, TInt aMouseY )
{
    if(aDocumentHandle)
        {
        // Dispatch of all mouse events.
        return (DispatchMouseEventsAt((CSvgDocumentImpl*)aDocumentHandle, aMouseX, aMouseY, (MSvgMouseListener*)this));
        }

        return 0;
}

/**
 * Check if document has animation.
 */
EXPORT_C TInt CSvgJavaInterfaceImpl::SvgDocumentHasAnimation( SvgDocumentHandle aDocumentHandle )
{
    if(aDocumentHandle)
        {
       	 	TBool lValue = SvgHasAnimation((CSvgDocumentImpl*)aDocumentHandle);

        	if(lValue)
        	{
        		return 1;
        	}
        }
        
    return 0;
}

/**
 * Check if element has animation.
 */
EXPORT_C TInt CSvgJavaInterfaceImpl::SvgElementHasAnimation( SvgElementHandle aElementHandle )
{
    if(aElementHandle)
        {
       	 	SvgElementHandle aFirstChildElement = SvgElementGetFirstElementChild(aElementHandle);
					
					while (aFirstChildElement)
					{
						if ( IsAnimationElemId( GetElementType((CXmlElementImpl*)aFirstChildElement)) )
						{
							return 1;
						}
						
						aFirstChildElement = SvgElementGetNextElementSibling( aFirstChildElement );
					}
        }

    return 0;
}

// ***********************************************************************
// SVG Element
// ***********************************************************************

/**
 * Create a new svg element of the given tag string and append to the given
 * svg element parent.
 */


EXPORT_C SvgElementHandle CSvgJavaInterfaceImpl::SvgElementCreate( SvgAttrType aType )
{
	TInt lElementId = SvgGetElementTypeMappingJSRtoSVG(aType);

    CXmlElementImpl* newChild = NULL;

        TRAPD( error, newChild = CreateElementL( iTempDoc, lElementId ) );
        if(error != KErrNone)
        {
           return 0;
        }

        //AppendChild( GetRootElement( iTempDoc ), newChild);
        // Alan
        TRAPD(err3, InitSvgStylePropertiesWithNullL((CSvgElementImpl*)newChild));
        if ( err3 != KErrNone )
        {
           return NULL;
        }

        return (SvgElementHandle)newChild;
}

/**
 * Destroy svg element of the given handle.
 */
EXPORT_C void  CSvgJavaInterfaceImpl::SvgElementDestroy( SvgElementHandle aElementHandle )
{
		if(aElementHandle )
    {
    CXmlElementImpl *lParentNode =
        (CXmlElementImpl*)GetParentElement( (CXmlElementImpl*)aElementHandle );
    if(lParentNode)
        {
        RemoveChild( (CXmlElementImpl*)lParentNode, (CXmlElementImpl*)aElementHandle );
        }
    DestroyElement( (CXmlElementImpl*)aElementHandle );
    }
}

/**
 * Get the parent element for the given svg element.
 */
EXPORT_C SvgElementHandle CSvgJavaInterfaceImpl::SvgElementGetParent( SvgElementHandle aElementHandle )
{
    return ( aElementHandle ) ? (SvgElementHandle)GetParentElement( (CXmlElementImpl*)aElementHandle ) : NULL;
}
/**
 * Get the first child element for the given svg element.
 */
EXPORT_C SvgElementHandle CSvgJavaInterfaceImpl::SvgElementGetFirstElementChild( SvgElementHandle aElementHandle )
{
    return ( aElementHandle ) ? (SvgElementHandle)GetFirstChild( (CXmlElementImpl*)aElementHandle ) : NULL;
}

/**
 * Get the next sibling element for the given svg element.
 */
EXPORT_C SvgElementHandle CSvgJavaInterfaceImpl::SvgElementGetNextElementSibling( SvgElementHandle aElementHandle )
{
    return ( aElementHandle ) ? (SvgElementHandle)GetNextSibling( (CXmlElementImpl*)aElementHandle ) : NULL;
}

/**
 * Append the given child element to the given svg element.
 */
EXPORT_C void CSvgJavaInterfaceImpl::SvgElementAppendChild( SvgElementHandle aElementHandle,
                             SvgElementHandle aChildElementHandle )
{
    if ( aElementHandle )
        {
        AppendChild( (CXmlElementImpl*)aElementHandle,
                                  (CXmlElementImpl*)aChildElementHandle, ETrue );
        }
}
/**
 * Remove the given child element from the given svg element.
 */
EXPORT_C SvgElementHandle CSvgJavaInterfaceImpl::SvgElementRemoveChild( SvgElementHandle aElementHandle,
                                 SvgElementHandle aChildElementHandle )
{
    if(aElementHandle)
        {
        RemoveChild( (CXmlElementImpl*)aElementHandle,
                     (CXmlElementImpl*)aChildElementHandle );
        }
     return aChildElementHandle;
}
/**
 * Add the given child element to the given svg element, before
 * the given reference element.
 */
EXPORT_C void CSvgJavaInterfaceImpl::SvgElementInsertBefore( SvgElementHandle aElementHandle,
                              SvgElementHandle aChildElementHandle,
                              SvgElementHandle aReferenceElementHandle )
{
    if ( aReferenceElementHandle )
    {
        CXmlElementImpl* lTempChild = GetFirstChild( (CXmlElementImpl*)aElementHandle );

    	if (lTempChild == (CXmlElementImpl*)aReferenceElementHandle)
    	{
    		//the first child happens to be the reference child to insert before

    		//set the parent up for the new child
    		SetParentElement( (CXmlElementImpl*)aChildElementHandle,
                              (CXmlElementImpl*)aElementHandle );

           	SetFirstChildElement( (CXmlElementImpl*)aChildElementHandle,
           							(CXmlElementImpl*)aElementHandle );

           	//set the new child up in front of the reference sibling
            SetNextSibling( (CXmlElementImpl*)aChildElementHandle, lTempChild  );

            return;
    	}

        while ( lTempChild && GetNextSibling( lTempChild ) !=
                (CXmlElementImpl*)aReferenceElementHandle )
        {
            lTempChild = GetNextSibling( lTempChild );
        }
        if ( lTempChild != NULL )
        {
            SetParentElement( (CXmlElementImpl*)aChildElementHandle,
                              (CXmlElementImpl*)aElementHandle );
            SetNextSibling( lTempChild, (CXmlElementImpl*)aChildElementHandle );
            SetNextSibling( (CXmlElementImpl*)aChildElementHandle,
                            (CXmlElementImpl*)aReferenceElementHandle );
        }
    }
}

/**
 * Request element type.
 */
EXPORT_C TInt CSvgJavaInterfaceImpl::SvgElementGetType( SvgElementHandle aElementHandle )
{
    return ( !aElementHandle ) ? 0 :
        SvgGetElementTypeMappingSVGtoJSR( GetElementType( (CXmlElementImpl*)aElementHandle ) );
}

/**
 * Get a string attribute from the given element.
 * returning 16 bit characters now instead of 8 bit ones that the original JSR used...
 */

EXPORT_C TInt CSvgJavaInterfaceImpl::SvgElementGetStringAttribute( SvgElementHandle aElementHandle,
                                        SvgAttrType aAttributeName, TPtrC16& aStrAttribute )
{
	 if(aElementHandle)
   {
	   		TInt lSvgAttrId =
				SvgGetAttributeTypeMappingJSRtoSVG(aAttributeName);
	  		if(lSvgAttrId != KSvgUnknownAttribute)
      		{
	        		return ( GetElementDesAttribute( ( CSvgElementImpl* )aElementHandle, lSvgAttrId, aStrAttribute ) );
      		}
		}
   	return KErrNotFound;
}

/**
 * Set a string attribute on the given element.
 */
EXPORT_C void CSvgJavaInterfaceImpl::SvgElementSetStringAttribute( SvgElementHandle aElementHandle,
                                        SvgAttrType aAttributeName,
                                        const TPtrC16& aAttributeValue )
{

if(aElementHandle)
    {
    TInt lSvgAttrId = SvgGetAttributeTypeMappingJSRtoSVG(aAttributeName);
    if(lSvgAttrId != KSvgUnknownAttribute)
        {
        // This will not work with the animation elements.
            SetElementDesAttribute((CSvgElementImpl*)aElementHandle,lSvgAttrId, aAttributeValue);
        }
    }
return ;
}

/**
 * Get a color attribute from the given element.
 * The values are copied into the color object given.
 */
EXPORT_C TInt CSvgJavaInterfaceImpl::SvgElementGetColorAttribute( SvgElementHandle aElementHandle,
                                   SvgAttrType aAttribute,
                                   TInt* aRedValue, TInt* aGreenValue, TInt* aBlueValue )
{
    if(aElementHandle)
    {
        TInt lSvgAttrId = SvgGetAttributeTypeMappingJSRtoSVG(aAttribute);
        // find if it is a valid attribute.
        if(lSvgAttrId != KSvgUnknownAttribute)
        {
            TInt32 lColorValue;

            // now the rgb values need to be fused. for this we take the
            // least significant 8 bits of each integer.

            lColorValue = GetElementColorAttribute((CSvgElementImpl*) aElementHandle,lSvgAttrId);

            // extract the 8-bits corressponding to the specific color attribute.
            // the color is in BGR format so

			//KGfxColorNull == 0x1ffffff
            if (lColorValue == 0x1ffffff)
            {
            	  *aBlueValue = -1; //Initialize BGR values
            	  *aGreenValue = -1;
            	  *aRedValue = -1;
            }

			//KSvgCurrentColor = 0x2ffffff
						else if (lColorValue == 0x2ffffff)
						{
							*aBlueValue = 0; //Initialize BGR values
            	*aGreenValue = 0;
            	*aRedValue = 0;
						}

			//KErrNoAttribute
			else if (lColorValue == -10 && (aAttribute == AT_FILL)  )
			{
				*aBlueValue = 0;
				*aGreenValue = 0;
				*aRedValue = 0;
			}
			else if (lColorValue == -10 && (aAttribute == AT_STROKE) )
			{
				*aBlueValue = -1;
				*aGreenValue = -1;
				*aRedValue = -1;
			}

			else if (lColorValue == -10 && (aAttribute == AT_COLOR) )
			{
				*aBlueValue = 0;
				*aGreenValue = 0;
				*aRedValue = 0;
			}
            else if (lColorValue != KInvalidEnumAttribute)
            {
                *aRedValue = ( lColorValue >> 16 ) & 0x0000ff;
                *aGreenValue = ( lColorValue >> 8 ) & 0x0000ff;
                *aBlueValue = lColorValue & 0x0000ff;
            }
            return 0;
        }
    }
    return -1;
}

/**
 * Set a color attribute for the given element.
 */
EXPORT_C void CSvgJavaInterfaceImpl::SvgElementSetColorAttribute( SvgElementHandle aElementHandle,
                                   SvgAttrType aAttribute,
                                   TInt aRedValue, TInt aGreenValue, TInt aBlueValue )

{
    if(aElementHandle)
        {
        TInt lSvgAttrId = SvgGetAttributeTypeMappingJSRtoSVG(aAttribute);
        // find if it is a valid attribute.
        if(lSvgAttrId != KSvgUnknownAttribute)
            {
            TInt32 lColorValue;

            // now the rgb values need to be fused. for this we take the
            // least significant 8 bits of each integer.

            TInt32 red = aRedValue & 0x0000ff;
            TInt32 green = aGreenValue & 0x0000ff;
            TInt32 blue = aBlueValue & 0x0000ff;

            lColorValue = (((red << 16) | green << 8) | blue);


            SetElementColorAttribute((CSvgElementImpl*) aElementHandle,lSvgAttrId,lColorValue);
            }
        }
}

/**
 * Get a float attribute value.
 */
EXPORT_C TReal32 CSvgJavaInterfaceImpl::SvgElementGetFloatAttribute( SvgElementHandle aElementHandle,
                                    SvgAttrType aAttribute ) __SOFTFP

{
	if(aElementHandle)
    {
     TInt lSvgAttrId = SvgGetAttributeTypeMappingJSRtoSVG(aAttribute);
     // The following function can not be called for the animation elements
     		if(lSvgAttrId != KSvgUnknownAttribute)
     		{
        	TReal32 lValue = GetElementFloatAttribute((CSvgElementImpl*)aElementHandle,lSvgAttrId);

        	return lValue;
      	}
    }
	return KInvalidFloatAttribute;
}

/**
 * Set a float attribute value.
 */
EXPORT_C void CSvgJavaInterfaceImpl::SvgElementSetFloatAttribute( SvgElementHandle aElementHandle,
                                   SvgAttrType aAttribute,
                                   TReal32 aFloatValue ) __SOFTFP
{
	if(aElementHandle)
    {
     TInt lSvgAttrId = SvgGetAttributeTypeMappingJSRtoSVG(aAttribute);
     // The following function can not be called for the animation elements
     if(lSvgAttrId != KSvgUnknownAttribute)
        {
        SetElementFloatAttribute((CSvgElementImpl*)aElementHandle,lSvgAttrId, aFloatValue);
        }
    }
	return;
}

/**
 * Set enum attribute value.
 */
EXPORT_C void  CSvgJavaInterfaceImpl::SvgElementSetEnumAttribute( SvgElementHandle aElementHandle,
                                     SvgAttrType aAttribute, short aValue)
{
   /**********************************************************
    // this function needs mapping of the enumeration values.
    // preserve aspect ratio
    // Zoom And Pan
    // Paint ? is it fill or stroke.
    // Fonts
    // Font_Style
    // Font_weight
    // Font_stretch
    // Text_Anchor
    // Font_Size
    // Fill_Rule
    // Display
    // Visibility
    // Color-rendering          // This is not supported in the SVGEngine.
    // Strokes
    // accumulate
    // additive
    // calc_mode
    // fill
    // restart
    // type (animate Transform)
    // attributeType
    ********************************************************/
    if(aElementHandle)
        {
        //TInt32 lValue = 0;
        
        TInt lSvgAttrId = SvgGetAttributeTypeMappingJSRtoSVG(aAttribute);
        if(lSvgAttrId != KSvgUnknownAttribute)
            {
			TInt lValue = SvgEnumerationMappingJSRtoSVG(lSvgAttrId , (TInt)aValue);
			/*
            lValue = SvgEnumerationMappingJSRtoSVG(lSvgAttrId , (TInt)aValue);
            
            if ( lValue == KErrNotFound && aValue == ATTRIBUTE_INHERIT ) 
                lValue = KSVGAttributeInherit;*/
            if(lSvgAttrId == KCSS_ATTR_FONTSIZE)
                {
                if(lValue != KErrNotFound)
                    {
                    // font is internally considered to be a float.

                    SetElementFloatAttribute((CSvgElementImpl*)aElementHandle,lSvgAttrId, (TReal32)lValue);
                    return;
                    }
                return;
                }
            if(lValue == KErrNotFound)
                {
                TBuf<40> lValueDes;
                TInt lResult = SvgEnumerationtoStringMappingJSRtoSVG(lSvgAttrId , aValue ,lValueDes);
                if(lResult == KErrNotFound)
                    {
                    // this is not valid Attribute.
                    return;
                    }
                else
                    {
                    SetElementDesAttribute((CSvgElementImpl*)aElementHandle,lSvgAttrId,lValueDes );
                    return;
                    }
                // Call for the string Attribute.
                }
            SetEnumAttribute((CSvgElementImpl*)aElementHandle,lSvgAttrId,lValue);
            }
        }
}

/**
 * Get enum attribute value.
 */
EXPORT_C short CSvgJavaInterfaceImpl::SvgElementGetEnumAttribute( SvgElementHandle aElementHandle,
                                     SvgAttrType aAttribute )
{
        if(aElementHandle)
        {
        TInt lSvgAttrId = SvgGetAttributeTypeMappingJSRtoSVG(aAttribute);
        if(lSvgAttrId != KSvgUnknownAttribute)
            {
            // following swicth is for all the attributes for which constants
            // are specified in Svgjsrconstants.h
            TInt32 lValue = KErrNone;
            if( lSvgAttrId == KCSS_ATTR_FILLRULE ||
            lSvgAttrId == KCSS_ATTR_STROKE_LINECAP ||
            lSvgAttrId == KCSS_ATTR_STROKE_LINEJOIN )
                {
                TPtrC16 lValue;
                GetElementDesAttribute((CSvgElementImpl*)aElementHandle,lSvgAttrId,lValue);
                TInt lResult = SvgStringtoEnumerationMappingSVGtoJSR(lSvgAttrId , lValue );
                return ( short )lResult;
                }
            else
                {
                if(lSvgAttrId == KCSS_ATTR_FONTSIZE)
                    {
                   // if(lValue != KErrNotFound)
                    //    {
                        // font is internally considered to be a float.
                        TReal32 lValue = GetElementFloatAttribute((CSvgElementImpl*)aElementHandle,lSvgAttrId);
                        return short (lValue) ;
                    //    }
                    //return KInvalidEnumAttribute;
                    }
                TInt lResult = GetEnumAttribute((CSvgElementImpl*)aElementHandle, lSvgAttrId , lValue);
                if(lResult == KErrNotFound)
                    {
                    return KInvalidEnumAttribute;
                    }
                else
                    {
                     TInt value = SvgEnumerationMappingSVGtoJSR(lSvgAttrId , lValue);
                    return (short) value;
                    }
                }
            }
        }
        return 0;
}

/**
 * Get the rectangle attribute value.
 */
EXPORT_C TInt CSvgJavaInterfaceImpl::SvgElementGetRectAttribute( SvgElementHandle aElementHandle,
                                  SvgAttrType /*aRectAttribute*/,
                                  TReal32* aX, TReal32* aY, TReal32* aWidth, TReal32* aHeight ) __SOFTFP
{
   if ( aElementHandle )
   {
        if (!GetRectAttribute( (CXmlElementImpl*) aElementHandle, aX,
                                                    aY, aWidth, aHeight ))
        {
					return -1;
				}
				
        if ( aX != NULL )
        {
            return 1;
        }
    }
   return -1;
}

/**
 * Set the rectangle attribute value.
 */
EXPORT_C void CSvgJavaInterfaceImpl::SvgElementSetRectAttribute( SvgElementHandle aElementHandle,
                                  SvgAttrType /*aRectAttribute*/,
                                  TReal32 aX, TReal32 aY, TReal32 aWidth, TReal32 aHeight ) __SOFTFP
{
   if ( aElementHandle ) SetRectAttribute( (CXmlElementImpl*) aElementHandle, aX,
                                                     aY, aWidth, aHeight );
}

/**
 * Get the matrix attribute value.
 */
EXPORT_C TInt CSvgJavaInterfaceImpl::SvgElementGetMatrixAttribute( SvgElementHandle aElementHandle,
                                    SvgAttrType aMatrixAttribute,
                                    TReal32* aAVal, TReal32* aBVal, TReal32* aCVal,
                                    TReal32* aDVal, TReal32* aEVal, TReal32* aFVal ) __SOFTFP
{
   if ( aElementHandle == NULL )
   {
       return -1;
   }
   
   if( aMatrixAttribute == AT_TRANSFORM) // request to get transform for the element
   {
      aMatrixAttribute = SvgGetAttributeTypeMappingJSRtoSVG(aMatrixAttribute);
   	  GetMatrixAttribute( (CXmlElementImpl*) aElementHandle, aMatrixAttribute, aAVal,
                           aBVal, aCVal, aDVal, aEVal, aFVal );
   }
   else // request for CTM
   {
       GetMatrixAttribute( (CXmlElementImpl*) aElementHandle, aAVal,
                           aBVal, aCVal, aDVal, aEVal, aFVal );
   }
   

   return 0;
}

/**
 * Set the matrix attribute value.
 */
EXPORT_C void CSvgJavaInterfaceImpl::SvgElementSetMatrixAttribute( SvgElementHandle aElementHandle,
                                    SvgAttrType /*aMatrixAttribute*/,
                                    TReal32 aAVal, TReal32 aBVal, TReal32 aCVal,
                                    TReal32 aDVal, TReal32 aEVal, TReal32 aFVal ) __SOFTFP
{
   if ( aElementHandle ) SetMatrixAttribute( (CXmlElementImpl*) aElementHandle, aAVal,
                                                      aBVal, aCVal, aDVal, aEVal, aFVal );
}

/**
 * Get the path attribute value.
 */
EXPORT_C TInt CSvgJavaInterfaceImpl::SvgElementGetPathAttribute( SvgElementHandle aElementHandle,
                                                        SvgAttrType aPathAttribute )
{
    //  The attributeId must be one of the following.
    //  KAtrD, KAtrTo, KAtrFrom, KAtrBy, KAtrPath
    TInt lSvgAttrId = SvgGetAttributeTypeMappingJSRtoSVG(aPathAttribute);

    return (TInt)GetPathAttribute((CXmlElementImpl*) aElementHandle, lSvgAttrId);
}

/**
 * Get the bounding box rectangle attribute value.
 */
EXPORT_C void CSvgJavaInterfaceImpl::SvgElementGetBBox( SvgElementHandle aElementHandle,
                            SvgAttrType /*aAttributeType*/,
                            TReal32* aX, TReal32* aY, TReal32* aWidth, TReal32* aHeight ) __SOFTFP
{
    if(aElementHandle)
        {
        TReal32 lX ,lY,lWidth,lHeight;
        GetElementUnScaledBoundingBox((CSvgElementImpl*)aElementHandle,lX ,lY,lWidth,lHeight);
        *aX = lX;
        *aY = lY;
        *aWidth = lWidth;
        *aHeight = lHeight;
        }
}

/**
 * Add event listener to the given element.
 * Note: need to replace void* with specific class
 */
EXPORT_C void CSvgJavaInterfaceImpl::SvgElementAddEventListener( SvgElementHandle aElementHandle,
                                        TInt /*aListener*/, SvgAttrType /*aType*/ )
{
    if(aElementHandle)
        {
        // by default we have added the KEventExternalUI as the mask type.

        // aListener how do you convert this to the SVGEngien's event Mask.
        const TUint8 aEventMask = KSvgEventMaskExternalUI;
        AddToEventReceiverList((CXmlElementImpl*) aElementHandle, aEventMask);
        }
}

/**
 * Remove event listener from the given element.
 * Note: need to replace void* with specific class
 */
EXPORT_C void CSvgJavaInterfaceImpl::SvgElementRemoveEventListener( SvgElementHandle aElementHandle,
                                        TInt /*aListener*/, SvgAttrType /*aType*/ )
{
    if(aElementHandle)
        {
        RemoveFromEventReceiverList((CXmlElementImpl*)aElementHandle);
        }
}

/**
 * Check if element is active.
 */
EXPORT_C TInt CSvgJavaInterfaceImpl::SvgElementIsActive( SvgElementHandle aElementHandle )
{
    CSvgElementImpl* hElement = (CSvgElementImpl*)aElementHandle;

    return (IsElementActive( hElement ));
}

/**
 * Check removable element.
 * any element that doesnt have an id and its children dont have ids
*/
EXPORT_C TInt CSvgJavaInterfaceImpl::SvgElementCheckRemoveable( SvgElementHandle aElementHandle )
{
    CSvgElementImpl* hElement = (CSvgElementImpl*)aElementHandle;

    return (IsRemoveable(hElement));
}

// ***********************************************************************
// SVG Path
// ***********************************************************************

/**
 * Create an empty path
 */
EXPORT_C SvgPathHandle CSvgJavaInterfaceImpl::SvgPathCreate()
{
    return (SvgPathHandle) SvgCreatePath();
}

/**
 * Destroy the specified path path
 */
EXPORT_C void CSvgJavaInterfaceImpl::SvgPathDestroy( SvgPathHandle aPathHandle )
{
 	if(aPathHandle)
    {
    SvgDestroyPath((CGfxGeneralPath*)aPathHandle);
    }
}

/**
 * Get the segment count for the given path.
 */
EXPORT_C TInt CSvgJavaInterfaceImpl::SvgPathGetSegmentCount( SvgPathHandle aPathHandle )
{
    if(aPathHandle)
        {
        return GetSegmentCount((CGfxGeneralPath*)aPathHandle);
        }
    return 0;
}

/**
 * Get the segment type for the given path.
 */
EXPORT_C TInt CSvgJavaInterfaceImpl::SvgPathGetSegmentType(SvgPathHandle aPathHandle, TInt aSegmentIndex )
{
    if(aPathHandle)
        {

        TInt segmenttype = GetSegmentType((CGfxGeneralPath*)aPathHandle, aSegmentIndex + 1);
        // map this to the corressponding segment type for the jsr.

        return MapSegmentType(segmenttype );
        }
    return 0;
}

/**
 * Get the segment parameter for the given path.
 */
EXPORT_C TReal32 CSvgJavaInterfaceImpl::SvgPathGetSegmentParameter( SvgPathHandle aPathHandle,
                                   TInt aSegmentIndex,
                                   TInt aSegmentParameterIndex ) __SOFTFP
{
    if(aPathHandle)
        {
        return (TReal32)GetSegmentParameter((CGfxGeneralPath*)aPathHandle,aSegmentIndex + 1,aSegmentParameterIndex + 1);
        }
    return 0;
}

 /**
 * Add a move-to command to the given path.
 */
EXPORT_C void CSvgJavaInterfaceImpl::SvgPathAddMoveTo( SvgPathHandle aPathHandle, TReal32 aX, TReal32 aY ) __SOFTFP
{
    if(aPathHandle)
        {
         ADDMoveTo((CGfxGeneralPath*)aPathHandle,aX,aY);
        }
}

/**
 * Add a line-to command to the given path.
 */
EXPORT_C void CSvgJavaInterfaceImpl::SvgPathAddLineTo( SvgPathHandle aPathHandle, TReal32 aX, TReal32 aY ) __SOFTFP
{
    if(aPathHandle)
        {
         ADDLineTo((CGfxGeneralPath*)aPathHandle, aX, aY);
        }
}

/**
 * Add a quad-to command to the given path.
 */
EXPORT_C void CSvgJavaInterfaceImpl::SvgPathAddQuadTo( SvgPathHandle aPathHandle,
                        TReal32 aX1, TReal32 aY1,
                        TReal32 aX2, TReal32 aY2) __SOFTFP
{
    if(aPathHandle)
        {
         ADDQuadTo((CGfxGeneralPath*)aPathHandle, aX1, aY1, aX2, aY2);
        }
}

/**
 * Add a curve-to command to the given path.
 */
EXPORT_C void CSvgJavaInterfaceImpl::SvgPathAddCurveTo( SvgPathHandle aPathHandle,
                        TReal32 aX1, TReal32 aY1,
                        TReal32 aX2, TReal32 aY2,
                        TReal32 aX3, TReal32 aY3 ) __SOFTFP
{
  if(aPathHandle)
        {
         ADDCurveTo((CGfxGeneralPath*)aPathHandle, aX1, aY1, aX2, aY2, aX3, aY3);
        }
}

/**
 * Send a close-path command to the given path.
 */
EXPORT_C void CSvgJavaInterfaceImpl::SvgPathAddClose( SvgPathHandle aPathHandle )
{
    ADDCloseTo((CGfxGeneralPath*)aPathHandle);
}

/**
 * Update path info.
 */
EXPORT_C void CSvgJavaInterfaceImpl::SvgElementUpdatePath( SvgElementHandle aElementHandle, SvgPathHandle aPathHandle )
{
    //this element handle may not be necessary
    CSvgElementImpl* myPathElement = (CSvgElementImpl*)aElementHandle;

    UpdatePath(aPathHandle, myPathElement);
}

EXPORT_C TInt  CSvgJavaInterfaceImpl::SvgElementElementInDOM ( TInt hDocument, TInt
                                        hElement )
{
		if(hDocument && hElement)
    {
    	TBool aresult= SVGElementInDom((CSvgDocumentImpl*)hDocument, (CXmlElementImpl*)hElement);
    
    	if(aresult)
        {
        return 1;
        }
    }
    
return 0;
}

/*
* This functions checks if the element is  child of a useElement.
*
*/

EXPORT_C TInt  CSvgJavaInterfaceImpl::SvgElementIsUsed ( TInt hElement )
{
    if(hElement)
        {
        if( GetElementType( GetParentElement( (CXmlElementImpl*) hElement )) == KSvgUseElement)
            {
            return 1;
            }
        return 0;
        }
    else
        {
        return 0;
        }
}

/*
* This functions returns the pointer to the element which uses the current element.
*
*/
EXPORT_C TInt CSvgJavaInterfaceImpl::SvgElementGetUsedFromElement ( TInt hElement )
{
    // this function gets the actual element which was cloned so that it can
    // be used in a use element.
    if(hElement)
        {
        // get the parent <use> elements xlink:href
        return (int) SVGElementGetUsedElement((CXmlElementImpl*)hElement);
        }
    return 0;
}
/*
* This functions returns the pointer to the element which uses the current element.
*
*/
EXPORT_C void CSvgJavaInterfaceImpl::SvgElementGetScreenBBox( TInt aElementHandle, TReal32* x,
                                                            TReal32* y, TReal32* w, TReal32* h ) __SOFTFP
    {
    if(aElementHandle)
        {
        TReal32 lX ,lY,lWidth,lHeight;
        GetElementBoundingbox((CSvgElementImpl*)aElementHandle,lX ,lY,lWidth,lHeight);
        *x = lX;
        *y = lY;
        *w = lWidth;
        *h = lHeight;
        }
    }

EXPORT_C void CSvgJavaInterfaceImpl::SvgElementSetPathAttribute( SvgElementHandle aElementHandle,
                                      SvgAttrType aPathAttribute , SvgPathHandle aPathHandle)
{
    if(aElementHandle)
        {
        if(aPathHandle)
            {
            TInt lSvgAttrId = SvgGetAttributeTypeMappingJSRtoSVG(aPathAttribute);
            SetPathAttribute((CSvgElementImpl*)(CXmlElementImpl*)aElementHandle,  lSvgAttrId, (CGfxGeneralPath*)aPathHandle);
            }
        }
}

TBool CSvgJavaInterfaceImpl::MouseEntered( RPointerArray<CSvgElementImpl>& /*aElements*/, TInt /*aX*/, TInt /*aY*/ )
{
	return ETrue;
}

TBool CSvgJavaInterfaceImpl::MouseExited( RPointerArray<CSvgElementImpl>& /*aElements*/, TInt /*aX*/, TInt /*aY*/ )
{
	return ETrue;
}

TBool CSvgJavaInterfaceImpl::MouseMoved( RPointerArray<CSvgElementImpl>& /*aElements*/, TInt /*aX*/, TInt /*aY*/ )
{
	return ETrue;
}

TBool CSvgJavaInterfaceImpl::MousePressed( RPointerArray<CSvgElementImpl>& /*aElements*/, TInt /*aX*/, TInt /*aY*/ )
{
	return ETrue;
}

TBool CSvgJavaInterfaceImpl::MouseReleased( RPointerArray<CSvgElementImpl>& /*aElements*/, TInt /*aX*/, TInt /*aY*/ )
{
	return ETrue;
}

/**
* This maps the element ids form JSR specific to SVG specific
*/
TInt CSvgJavaInterfaceImpl::SvgGetElementTypeMappingJSRtoSVG( SvgAttrType aType )
{

switch(aType)
    {
    case 0: return KSvgAElement;

    case 1: return KSvgAnimateElement;

    case 2: return KSvgAnimateColorElement;

    case 3: return KSvgAnimateMotionElement;

    case 4: return KSvgAnimateTransformElement;

    case 5: return KSvgCircleElement;

    case 6: return KSvgDefsElement;

    case 7: return KSvgDescElement;

    case 8: return KSvgEllipseElement;

    case 9: return KSvgFontElement;

    case 10: return KSvgFontfaceElement;

    case 11: return KSvgFontfacenameElement;

    case 12: return KSvgFontfacesrcElement;

    case 13: return KSvgForeignObjectElement;

    case 14: return KSvgGElement;

    case 15: return KSvgGlyphElement;

    case 16: return KSvgHkernElement;

    case 17: return KSvgImageElement;

    case 18: return KSvgLineElement;

    case 19: return KSvgMetadataElement;

    case 20: return KSvgMissingglyphElement;

    case 21: return KSvgMpathElement;

    case 22: return KSvgPathElement;

    case 23: return KSvgPolygonElement;

    case 24: return KSvgPolylineElement;

    case 25: return KSvgRectElement;

    case 26: return KSvgSetElement;

    case 27: return KSvgSvgElement;

    case 28: return KSvgSwitchElement;

    case 29: return KSvgTextElement;

    case 30: return KSvgTitleElement;

    case 31: return KSvgUseElement;

    default: return KSvgUnknownElement;
    }
}

/**
* Tells if the element id is an animation or not.
*/
TBool CSvgJavaInterfaceImpl::IsAnimationElemId( const TInt aElementId )
{
	if ( aElementId == KSvgAnimateElement || 
			 aElementId == KSvgAnimateColorElement ||
			 aElementId == KSvgAnimateMotionElement ||
			 aElementId == KSvgAnimateTransformElement)
	{
		return ETrue;
	}		 	
	
	return EFalse;
}

/**
* This maps the element ids form SVG specific to JSR specific
*/
SvgAttrType  CSvgJavaInterfaceImpl::SvgGetElementTypeMappingSVGtoJSR( const TInt aElementId)
{
    switch(aElementId)
    {
    case KSvgAElement: return 0;

    case KSvgAnimateElement: return 1;

    case KSvgAnimateColorElement: return 2;

    case KSvgAnimateMotionElement: return 3;

    case KSvgAnimateTransformElement: return 4;

    case KSvgCircleElement: return 5;

    case KSvgDefsElement: return 6;

    case KSvgDescElement: return 7;

    case KSvgEllipseElement: return 8;

    case KSvgFontElement: return 9;

    case KSvgFontfaceElement: return 10;

    case KSvgFontfacenameElement: return 11;

    case KSvgFontfacesrcElement: return 12;

    case KSvgForeignObjectElement: return 13;

    case KSvgGElement: return 14;

    case KSvgGlyphElement: return 15;

    case KSvgHkernElement: return 16;

    case KSvgImageElement: return 17;

    case KSvgLineElement: return 18;

    case KSvgMetadataElement: return 19;

    case KSvgMissingglyphElement: return 20;

    case KSvgMpathElement: return 21;

    case KSvgPathElement: return 22;

    case KSvgPolygonElement: return 23;

    case KSvgPolylineElement: return 24;

    case KSvgRectElement: return 25;

    case KSvgSetElement: return 26;

    case KSvgSvgElement: return 27;

    case KSvgSwitchElement: return 28;

    case KSvgTextElement: return 29;

    case KSvgTitleElement: return 30;

    case KSvgUseElement: return 31;

    default: return KSvgUnknownElement;
    }
}

/**
* This maps the Attribute ids form SVG specific to JSR specific
*/

SvgAttrType  CSvgJavaInterfaceImpl::SvgGetAttributeTypeMappingSVGtoJSR( const TInt aElementId)
{
    switch(aElementId)
    {
    case KAtrAccumulate:return AT_ACCUMULATE;
    case KAtrAdditive:return AT_ADDITIVE;
    case KAtrAlphabetic:return AT_ALPHABETIC;
    case KAtrAscent:return AT_ASCENT;
    case KAtrAttributeName:return AT_ATTRIBUTENAME;
    case KAtrBaseProfile:return AT_BASEPROFILE;
    case KAtrBegin:return AT_BEGIN;
    case KAtrBy:return AT_BY;
    case KAtrCalcMode:return AT_CALCMODE;
    case KCSS_ATTR_COLOR:return AT_COLOR;
    case KCSS_ATTR_COLORRENDERING:return AT_COLORRENDERING;
    case KAtrCx:return AT_CX;
    case KAtrCy:return AT_CY;
    case KAtrD:return AT_D;
    case KAtrDescent:return AT_DESCENT ;
    case KCSS_ATTR_DISPLAY:return AT_DISPLAY;
    case KAtrDur:return AT_DUR;
    case KAtrEnd:return AT_END;
    case KCSS_ATTR_FILL:return AT_FILL;
    case KCSS_ATTR_FILLRULE:return AT_FILLRULE;
    case KCSS_ATTR_FONTFAMILY:return AT_FONTFAMILY;
    case KCSS_ATTR_FONTSIZE:return AT_FONTSIZE;
    case KCSS_ATTR_FONTSTYLE:return AT_FONTSTYLE;
    case KCSS_ATTR_FONTWEIGHT:return AT_FONTWEIGHT;
    case KAtrFrom:return AT_FROM;
    case KAtrG1:return AT_G1;
    case KAtrG2:return AT_G2;
    case KAtrGlyphName:return AT_GLYPHNAME;
    case KAtrHeight:return AT_HEIGHT;
    case KAtrHorizAdvX:return AT_HORIZADVX;
    case KAtrHorizOriginX:return AT_HORIZORIGINX;
    case KAtrId:return AT_ID;
    case KAtrK:return AT_K;
    case KAtrKeySplines:return AT_KEYSPLINES;
    case KAtrKeyTimes:return AT_KEYTIMES;
    case KAtrLang:return AT_LANG;
    case KAtrOverlinePosition:return AT_OVERLINEPOSITION;
    case KAtrOverlineThickness:return AT_OVERLINETHICKNESS;
    case KAtrPath:return AT_PATH;
    case KAtrPathLength:return AT_PATHLENGTH;
    case KAtrPoints:return AT_POINTS;
    case KAtrPreserveAspectRatio:return AT_PRESERVEASPECTRATIO;
    case KAtrR:return AT_R;
    case KAtrRepeatCount:return AT_REPEATCOUNT;
    case KAtrRepeatDur:return AT_REPEATDUR;
    case KAtrRequiredExtensions:return AT_REQUIREDEXTENSIONS;
    case KAtrRequiredFeatures:return AT_REQUIREDFEATURES;
    case KAtrRestart:return AT_RESTART;
    case KAtrRotate:return AT_ROTATE;
    case KAtrRx :return AT_RX;
    case KAtrRy:return AT_RY;
    case KAtrStrikethroughPosition:return AT_STRIKETHROUGHPOSITION;
    case KAtrStrikethroughThickness:return AT_STRIKETHROUGHTHICKNESS;
    case KCSS_ATTR_STROKE:return AT_STROKE;
    case KCSS_ATTR_STROKE_DASHARRAY:return AT_STROKEDASHARRAY;
    case KCSS_ATTR_STROKE_DASHOFFSET:return AT_STROKEDASHOFFSET;
    case KCSS_ATTR_STROKE_LINECAP:return AT_STROKELINECAP;
    case KCSS_ATTR_STROKE_LINEJOIN:return AT_STROKELINEJOIN;
    case KCSS_ATTR_STROKE_MITERLIMIT:return AT_STROKEMITERLIMIT;
    case KCSS_ATTR_STROKEWIDTH:return AT_STROKEWIDTH;
    case KAtrStyle:return AT_STYLE;
    case KAtrCdata:return AT_STRING;
    case KAtrSystemLanguage:return AT_SYSTEMLANGUAGE;
    case KCSS_ATTR_TEXTANCHOR:return AT_TEXTANCHOR;
    case KCSS_ATTR_TEXTDECORATION:return AT_TEXTDECORATION;
    case KAtrTo:return AT_TO;
    case KAtrTransform :return AT_TRANSFORM;
    case KAtrType: return AT_TYPE;
    case KAtrU1:return AT_U1;
    case KAtrU2:return AT_U2;
    case KAtrUnderlinePosition:return AT_UNDERLINEPOSITION;
    case KAtrUnderlineThickness:return AT_UNDERLINETHICKNESS;
    case KAtrUnicode:return AT_UNICODE;
    case KAtrUnicodeRange:return AT_UNICODERANGE;
    case KAtrUnitsPerEm:return AT_UNITSPEREM;
    case KAtrValues:return AT_VALUES;
    case KAtrVersion:return AT_VERSION;
    case KAtrViewBox:return AT_VIEWBOX;
    case KCSS_ATTR_VISIBILITY:return  AT_VISIBILITY;
    case KAtrWidth: return AT_WIDTH;
    case KAtrX: return AT_X;
    case KAtrX1:return AT_X1;
    case KAtrX2:return AT_X2;
    case KAtrXlinkactuate:return AT_XLINKACTUATE;
    case KAtrXlinkarcrole:return AT_XLINKARCROLE;
    case KAtrXlinkhref: return AT_XLINKHREF;
    case KAtrXlinkrole: return AT_XLINKROLE;
    case KAtrXlinkshow: return AT_XLINKSHOW;
    case KAtrXlinktitle:return AT_XLINKTITLE;
    case KAtrXlinktype:return AT_XLINKTYPE;
    case KAtrXmlLang: return AT_XMLLANG;
    case KAtrXmlBase: return AT_XMLBASE;
    case KAtrXmlSpace:return AT_XMLSPACE;
    case KAtrY: return AT_Y;
    case KAtrY1:return AT_Y1;
    case KAtrY2:return AT_Y2;
    case KAtrZoomAndPan: return AT_ZOOMANDPAN;
    default: return KSvgUnknownAttribute;
    }
}

/**
* This maps the Attribute ids form JSR specific to SVG specific
*/
TInt CSvgJavaInterfaceImpl::SvgGetAttributeTypeMappingJSRtoSVG( SvgAttrType aType )
{
    switch(aType)
    {
    case AT_ACCENTHEIGHT: return KSvgUnknownAttribute;

    case AT_ACCUMULATE: return KAtrAccumulate;

    case AT_ADDITIVE: return KAtrAdditive;

    case AT_ALPHABETIC: return KAtrAlphabetic;

    case AT_ARABICFORM: return KSvgUnknownAttribute;

    case AT_ASCENT: return KAtrAscent;

    case AT_ATTRIBUTENAME: return KAtrAttributeName;

    case AT_ATTRIBUTETYPE: return KSvgUnknownAttribute;

    case AT_BASEPROFILE: return KAtrBaseProfile;

    case AT_BBOX: return KSvgUnknownAttribute;

    case AT_BEGIN: return KAtrBegin;

    case AT_BY: return KAtrBy;

    case AT_CALCMODE: return KAtrCalcMode;

    case AT_CAPHEIGHT: return KSvgUnknownAttribute;

    case AT_COLOR: return KCSS_ATTR_COLOR;

    case AT_COLORRENDERING: return KCSS_ATTR_COLORRENDERING;

    case AT_CX: return KAtrCx;

    case AT_CY: return KAtrCy;

    case AT_D: return KAtrD;

    case AT_DESCENT: return KAtrDescent;

    case AT_DISPLAY: return KCSS_ATTR_DISPLAY;

    case AT_DUR: return KAtrDur;

    case AT_END: return KAtrEnd;

   // it can be the fill corressponding to animation how to handle that.
    case AT_FILL: return KCSS_ATTR_FILL;

    case AT_FILLRULE: return KCSS_ATTR_FILLRULE;

    case AT_FONTFAMILY: return KCSS_ATTR_FONTFAMILY;

    case AT_FONTSIZE: return KCSS_ATTR_FONTSIZE;

    case AT_FONTSTRETCH: return KSvgUnknownAttribute;

    case AT_FONTSTYLE: return KCSS_ATTR_FONTSTYLE;

    case AT_FONTVARIANT: return KSvgUnknownAttribute;

    case AT_FONTWEIGHT: return KCSS_ATTR_FONTWEIGHT;

    case AT_FROM: return KAtrFrom;

    case AT_G1: return KAtrG1;

    case AT_G2: return KAtrG2;

    case AT_GLYPHNAME: return KAtrGlyphName;

    case AT_HANGING: return KSvgUnknownAttribute;

    case AT_HEIGHT: return KAtrHeight;

    case AT_HORIZADVX: return KAtrHorizAdvX;

    case AT_HORIZORIGINX: return KAtrHorizOriginX;

    case AT_ID: return KAtrId;

    case AT_IDEOGRAPHIC: return KSvgUnknownAttribute;

    case AT_K: return KAtrK;

    case AT_KEYPOINTS: return KSvgUnknownAttribute;

    case AT_KEYSPLINES: return KAtrKeySplines;

    case AT_KEYTIMES: return KAtrKeyTimes;

    case AT_LANG: return KAtrLang;

    case AT_MATHEMATICAL: return KSvgUnknownAttribute;

    case AT_MAX: return KSvgUnknownAttribute;

    case AT_MIN: return KSvgUnknownAttribute;

    case AT_NAME: return KSvgUnknownAttribute;

    case AT_ORIGIN: return KSvgUnknownAttribute;

    case AT_OVERLINEPOSITION: return KAtrOverlinePosition;

    case AT_OVERLINETHICKNESS: return KAtrOverlineThickness;

    case AT_PANOSE1: return KSvgUnknownAttribute;

    case AT_PATH: return KAtrPath;

    case AT_PATHLENGTH: return KAtrPathLength;

    case AT_POINTS: return KAtrPoints;

    case AT_PRESERVEASPECTRATIO: return KAtrPreserveAspectRatio;

    case AT_R: return KAtrR;

    case AT_REPEATCOUNT: return KAtrRepeatCount;

    case AT_REPEATDUR:return KAtrRepeatDur;

    case AT_REQUIREDEXTENSIONS: return KAtrRequiredExtensions;

    case AT_REQUIREDFEATURES: return KAtrRequiredFeatures;

    case AT_RESTART: return KAtrRestart;

    case AT_ROTATE: return KAtrRotate;

    case AT_RX: return KAtrRx;

    case AT_RY: return KAtrRy;

    case AT_SLOPE: return KSvgUnknownAttribute;

    case AT_STEMH: return KSvgUnknownAttribute;

    case AT_STEMV: return KSvgUnknownAttribute;

    case AT_STRIKETHROUGHPOSITION: return KAtrStrikethroughPosition;

    case AT_STRIKETHROUGHTHICKNESS: return KAtrStrikethroughThickness;

    case AT_STROKE: return KCSS_ATTR_STROKE;

    case AT_STROKEDASHARRAY: return KCSS_ATTR_STROKE_DASHARRAY;

    case AT_STROKEDASHOFFSET: return KCSS_ATTR_STROKE_DASHOFFSET;

    case AT_STROKELINECAP: return KCSS_ATTR_STROKE_LINECAP;

    case AT_STROKELINEJOIN: return KCSS_ATTR_STROKE_LINEJOIN;

    case AT_STROKEMITERLIMIT: return KCSS_ATTR_STROKE_MITERLIMIT;

    case AT_STROKEWIDTH: return KCSS_ATTR_STROKEWIDTH;

    case AT_STYLE: return KAtrStyle;

		case AT_STRING: return KAtrCdata;

    case AT_SYSTEMLANGUAGE: return KAtrSystemLanguage;

    case AT_TARGET: return KAtrTarget;

    case AT_TEXTANCHOR: return KCSS_ATTR_TEXTANCHOR;
    case AT_TEXTDECORATION: return KCSS_ATTR_TEXTDECORATION;
    case AT_TO: return KAtrTo;

    case AT_TRANSFORM: return KAtrTransform;

    case AT_TYPE: return KAtrType;

    case AT_U1: return KAtrU1;

    case AT_U2: return KAtrU2;

    case AT_UNDERLINEPOSITION: return KAtrUnderlinePosition;

    case AT_UNDERLINETHICKNESS: return KAtrUnderlineThickness;

    case AT_UNICODE: return KAtrUnicode;

    case AT_UNICODERANGE:return KAtrUnicodeRange;

    case AT_UNITSPEREM: return KAtrUnitsPerEm;

    case AT_VALUES: return KAtrValues;

    case AT_VERSION: return KAtrVersion;

    case AT_VIEWBOX: return KAtrViewBox;

    case AT_VISIBILITY: return KCSS_ATTR_VISIBILITY;

    case AT_WIDTH: return KAtrWidth;

    case AT_WIDTHS: return KSvgUnknownAttribute;

    case AT_X: return KAtrX;

    case AT_XHEIGHT: return KSvgUnknownAttribute;

    case AT_X1: return KAtrX1;

    case AT_X2:return KAtrX2;

    case AT_XLINKACTUATE: return KAtrXlinkactuate;

    case AT_XLINKARCROLE: return KAtrXlinkarcrole;

    case AT_XLINKHREF: return KAtrXlinkhref;

    case AT_XLINKROLE: return KAtrXlinkrole;

    case AT_XLINKSHOW: return KAtrXlinkshow;

    case AT_XLINKTITLE: return KAtrXlinktitle;

    case AT_XLINKTYPE: return KAtrXlinktype;

    case AT_XMLLANG: return KAtrXmlLang;

    case AT_XMLBASE: return KAtrXmlBase;

    case AT_XMLSPACE: return KAtrXmlSpace;

    case AT_Y: return KAtrY;

    case AT_Y1: return KAtrY1;

    case AT_Y2: return KAtrY2;

    case AT_ZOOMANDPAN: return KAtrZoomAndPan;

    default: return KSvgUnknownAttribute;
    }
}
/**
* This maps the Enumeration  Mapping form JSR specific to SVG specific
*/
TInt  CSvgJavaInterfaceImpl::SvgEnumerationMappingJSRtoSVG(const TInt aAttributeId, const TInt aJsrEnumValue)
{
    // Most of these are style properties.
    // COLOR_Rendering. this is not supported.
            switch(aAttributeId)
                {
                    // Text Anchor
                case KAtrRotate:
                    {
                        switch(aJsrEnumValue)
                        {
                            // these are defined in animateMotion.cpp
                        case ROTATE_AUTO: return -100;
                        case ROTATE_AUTOREVERSE: return -200;
                        default:
                            return KErrNotFound;
                        }
                    }

                case KAtrType:
                    {
                        switch(aJsrEnumValue)
                        {
                        // these are defined in SchemaData.cpp
                        case TYPE_TRANSLATE: return KSvgTypeTranslate;
                        case TYPE_SCALE: return KSvgTypeScale;
                        case TYPE_ROTATE:return KSvgTypeRotate;
                        case TYPE_SKEWX:return KSvgTypeSkewX;
                        case TYPE_SKEWY: return KSvgTypeSkewY;
                        default:
                            return KErrNotFound;
                        }
                    }

                case KCSS_ATTR_FONTSIZE:
                    {
                        switch(aJsrEnumValue)
                        {
                        case FONT_SIZE_XXSMALL: return 2;
                        case FONT_SIZE_XSMALL: return 4;
                        case FONT_SIZE_SMALL:return 6;
                        case FONT_SIZE_MEDIUM:return 10;
                        case FONT_SIZE_LARGE: return 16;
                        case FONT_SIZE_XLARGE:return 20;
                        case FONT_SIZE_XXLARGE:return 24;
                        default:
                            return KErrNotFound;
                        }
                    }

                case KCSS_ATTR_TEXTANCHOR:
                    {
                        switch(aJsrEnumValue)
                        {
                        case TEXT_ANCHOR_START:
                        // these constants are taken from schemaData.cpp any change will need
                        // to be propagated to here
                            return 0;
                        case TEXT_ANCHOR_MIDDLE:
                            return 1;
                        case TEXT_ANCHOR_END:
                            return 2;
                        default:
                         return KErrNotFound;
                        }

                    }
                    // Font Weight
                case KCSS_ATTR_FONTWEIGHT:
                    {
                        switch(aJsrEnumValue)
                        {
                        case FONT_NORMAL:
                        return 0;
                        case FONT_WEIGHT_BOLD:
                        return 1;
                        case FONT_WEIGHT_BOLDER:
                        return 2;
                        case FONT_WEIGHT_LIGHTER:
                        return 3;
                        case FONT_WEIGHT_100:
                        return 4;
                        case FONT_WEIGHT_200:
                        return 5;
                        case FONT_WEIGHT_300:
                        return 6;
                        case FONT_WEIGHT_400:
                        return 7;
                        case FONT_WEIGHT_500:
                        return 8;
                        case FONT_WEIGHT_600:
                        return 9;
                        case FONT_WEIGHT_700:
                        return 10;
                        case FONT_WEIGHT_800:
                        return 11;
                        case FONT_WEIGHT_900:
                        return 12;
                        default:
                        return KErrNotFound;
                        }

                    }
                    // Font Style
                case KCSS_ATTR_FONTSTYLE:
                        {
                        switch(aJsrEnumValue)
                            {
                            case FONT_NORMAL:
                            return 0;
                            case FONT_STYLE_ITALIC  :
                            return 1;
                            case FONT_STYLE_OBLIQUE  :
                            return 2;
                            default:
                            return KErrNotFound;
                            }

                        }
                    // Spread Method not given in defines.h so probably spreadmethod won't be used.
                case KAtrPreserveAspectRatio:

                        {
                            switch(aJsrEnumValue)
                            {
                            case PAR_NONE:
                            return 0;
                            case PAR_XMIDYMID:
                            return 6;
                            default:
                            return KErrNotFound;

                            }

                        }
                    // TextDecoration
                case KAtrZoomAndPan:
                    {
                        switch(aJsrEnumValue)
                        {
                        case ZPN_MAGNIFY:
                        return 1;

                        case ZPN_DISABLE:
                        return 0;
                        default:
                            return KErrNotFound;
                        }

                    }
                    // Display
                case KCSS_ATTR_DISPLAY:
                    {
                        switch(aJsrEnumValue)
                    {
                    case DISPLAY_NONE:
                    return 16;

                    case DISPLAY_OTHER:
                    return 0;

                    default:
                    return KErrNotFound;

                    }

                    }
                    // VIsibility
                case KCSS_ATTR_VISIBILITY:
                    {
                        switch(aJsrEnumValue)
                        {
                        case VISIBILITY_VISIBLE:
                        return 0;
                        case VISIBILITY_OTHER:
                        return 1;
                        default:
                        return KErrNotFound;

                        }

                    }
                case KAtrAdditive:
                        {
                        switch(aJsrEnumValue)
                            {
                        case ADDITIVE_REPLACE:
                        return 0;
                        case ADDITIVE_SUM:
                        return 1;
                        default : return KErrNotFound;

                            }

                        }
                    case KAtrAccumulate:
                        {
                            switch(aJsrEnumValue)
                            {
                        case ACCUMULATE_NONE:
                        return 0;
                        case ACCUMULATE_SUM:
                        return 1;
                        default : return KErrNotFound;

                            }

                        }
                    case KAtrCalcMode:
                        {
                            switch(aJsrEnumValue)
                            {
                        case CALC_MODE_DISCRETE:
                        return 0;
                        case CALC_MODE_LINEAR:
                        return 1;
                        case CALC_MODE_PACED:
                        return 2;
                        case CALC_MODE_SPLINE:
                        return 3;
                        default: return KErrNotFound;

                            }

                        }
                        // it can not be this attribute.
                        // This fill corresponds to animation.
                    case KCSS_ATTR_FILL:
                            {
                            switch(aJsrEnumValue)
                                {
                                // these are for animation
                                case FILL_REMOVE:
                                return 0;
                                case FILL_FREEZE:
                                return 1;
                                // following is the attribute value for fill "color"
                                // these are for fill - style attributes.
                                case PAINT_NONE:
                                return KSVGColorNone;
                                case PAINT_CURRENT:
                                return KSVGCurrentColor;
                                case PAINT_INHERIT:
                                return KSVGColorInherit;
                                default: return KErrNotFound;
                                }

                        }
                    case KCSS_ATTR_STROKE:
                            {
                            switch(aJsrEnumValue)
                                {
                                // following is the attribute value for fill "color"
                                // these are for fill - style attributes.
                                case PAINT_NONE:
                                return KSVGColorNone;
                                case PAINT_CURRENT:
                                return KSVGCurrentColor;
                                case PAINT_INHERIT:
                                return KSVGColorInherit;
                                default: return KErrNotFound;
                                }

                            }
                    case KAtrRestart:
                        {
                        switch(aJsrEnumValue)
                        {
                        case RESTART_ALWAYS:
                        return 0;
                        case RESTART_NEVER:
                        return 2;
                        case RESTART_WHENNOTACTIVE:
                        return 1;
                        default :
                        return KErrNotFound;
                        }

                        }
                    case KCSS_ATTR_TEXTDECORATION:
                        {
                        switch(aJsrEnumValue)
                            {
                        case TEXT_UNDER_LINE:
                        return 1;
                        case TEXT_OVER_LINE:
                        return 2;
                        case TEXT_LINE_THROUGH:
                        return 3;
                        default:
                        return KErrNotFound;
                            }

                        }
                    default:
                    return KErrNotFound;

                    }
}

/**
* This maps the Enumeration from JSR to SVG. SVG enumerated value here is string type.
*/
TInt  CSvgJavaInterfaceImpl::SvgEnumerationtoStringMappingJSRtoSVG(const TInt aAttributeId, short aJsrEnumValue, TDes& aValue)
{
    // all of these are style properties.
    switch(aAttributeId)
    {
    case KCSS_ATTR_FILLRULE:
        switch(aJsrEnumValue)
            {
            case FILL_RULE_EVENODD:
                {
                aValue = _L("evenodd");
                }
            break;
            case FILL_RULE_NONZERO:
                {
                    aValue = _L("nonzero");
                }
            break;
            default:
                return KErrNotFound;

            }
        break;
        case KCSS_ATTR_STROKE_LINECAP:
            switch(aJsrEnumValue)
            {
            case STROKE_LINECAP_BUTT:
                {
                    aValue = _L("butt");
                }
                break;
            case STROKE_LINECAP_ROUND:
                {
                    aValue = _L("round");
                }
                break;
            case STROKE_LINECAP_SQUARE:
                {
                    aValue = _L("square");
                }
                break;
            default : return KErrNotFound;
            }
        break;
        case KCSS_ATTR_STROKE_LINEJOIN:
            switch(aJsrEnumValue)
            {
            case STROKE_LINEJOIN_MITER:
                {
                    aValue = _L("miter");
                }
                break;
            case STROKE_LINEJOIN_ROUND:
                {
                    aValue = _L("round");
                }
                break;
            case STROKE_LINEJOIN_BEVEL:
                {
                    aValue = _L("bevel");
                }
                break;
            default : return KErrNotFound;
            }
            break;
            default: return KErrNotFound;

    }
    return KErrNone;
}
/**
* This maps the Enumeration from SVG to JSR. SVG enumerated value here is string type.
*/
TInt CSvgJavaInterfaceImpl::SvgStringtoEnumerationMappingSVGtoJSR(const TInt aAttributeId , TPtrC16 aValue)
{
    TInt attribVal = KErrNotFound;
    
    switch(aAttributeId)
    {
    case KCSS_ATTR_FILLRULE:
            {
            if(aValue == _L("evenodd"))
                attribVal = FILL_RULE_EVENODD;
            else if(aValue == _L("nonzero"))
                attribVal = FILL_RULE_NONZERO;
            }
            break;
    case KCSS_ATTR_STROKE_LINECAP:
            {
            if(aValue == _L("butt"))
                attribVal = STROKE_LINECAP_BUTT;
            else if(aValue == _L("round"))
                attribVal = STROKE_LINECAP_ROUND;
            else if(aValue == _L("square"))
                attribVal = STROKE_LINECAP_SQUARE;
            }
            break;
    case KCSS_ATTR_STROKE_LINEJOIN:
            {
            if(aValue == _L("miter"))
                attribVal = STROKE_LINEJOIN_MITER;
            else if(aValue == _L("round"))
                attribVal = STROKE_LINEJOIN_ROUND;
            else if(aValue == _L("bevel"))
                attribVal = STROKE_LINEJOIN_BEVEL;
            }
            break;
    default: 
        attribVal = KErrNotFound;
    
    }
    return attribVal;
}
/**
* This maps the Enumeration from SVG to JSR.
*/
TInt  CSvgJavaInterfaceImpl::SvgEnumerationMappingSVGtoJSR(const TInt aAttributeId, TInt32 aSvgEnumValue)
{
            switch(aAttributeId)
                {

                case KAtrRotate:
                    {
                        switch(aSvgEnumValue)
                        {
                        case -100: return ROTATE_AUTO;
                        case -200: return ROTATE_AUTOREVERSE;
                        default:
                            return KErrNotFound;
                        }
                    }

                case KAtrType:
                    {
                        switch(aSvgEnumValue)
                {
                        case KSvgTypeTranslate: return TYPE_TRANSLATE;
                        case KSvgTypeScale: return TYPE_SCALE;
                        case KSvgTypeRotate:return TYPE_ROTATE;
                        case KSvgTypeSkewX:return TYPE_SKEWX;
                        case KSvgTypeSkewY: return TYPE_SKEWY;
                        default:
                            return KErrNotFound;
                        }
                    }

                case KCSS_ATTR_TEXTANCHOR:
                    {
                        switch(aSvgEnumValue)
                        {
                        case 0:
                        // these constants are taken from schemaData.cpp any change will need
                        // to be propagated to here
                        return TEXT_ANCHOR_START;
                        case 1:
                            return TEXT_ANCHOR_MIDDLE;
                        case 2:
                            return TEXT_ANCHOR_END;
                        default:
                         return KErrNotFound;
                        }

                    }
                    // Font Weight
                case KCSS_ATTR_FONTWEIGHT:
                    {
                        switch(aSvgEnumValue)
                        {
                        case 0:
                        return FONT_NORMAL;
                        case 1:
                        return FONT_WEIGHT_BOLD;
                        case 2:
                        return FONT_WEIGHT_BOLDER;
                        case 3:
                        return FONT_WEIGHT_LIGHTER;
                        case 4:
                        return FONT_WEIGHT_100;
                        case 5:
                        return FONT_WEIGHT_200;
                        case 6:
                        return FONT_WEIGHT_300;
                        case 7:
                        return FONT_WEIGHT_400;
                        case 8:
                        return FONT_WEIGHT_500;
                        case 9:
                        return FONT_WEIGHT_600;
                        case 10:
                        return FONT_WEIGHT_700;
                        case 11:
                        return FONT_WEIGHT_800;
                        case 12:
                        return FONT_WEIGHT_900;
                        default:
                        return KErrNotFound;
                        }

                    }
                    // Font Style
                case KCSS_ATTR_FONTSTYLE:
                    {
                    switch(aSvgEnumValue)
                        {
                        case 0:
                        return FONT_NORMAL;
                        case 1:
                        return FONT_STYLE_ITALIC;
                        case 2:
                        return FONT_STYLE_OBLIQUE  ;
                        default:
                        return KErrNotFound;
                        }

                    }
                // preserve aspect ratio
                case KAtrPreserveAspectRatio:

                    {
                        switch(aSvgEnumValue)
                        {
                        case 0:
                        return PAR_NONE;
                        case 6:
                        return PAR_XMIDYMID;
                        }

                    }
                    // Zoom and Pan
                case KAtrZoomAndPan:
                    {
                        switch(aSvgEnumValue)
                        {
                        case 1:
                        return ZPN_MAGNIFY;

                        case 0:
                        return ZPN_DISABLE;
                        }

                    }
                    // Display
                case KCSS_ATTR_DISPLAY:
                    {
                        switch(aSvgEnumValue)
                    {
                    case 16:
                    return DISPLAY_NONE;

                    case 0:
                    return DISPLAY_OTHER;

                    default:
                    return KErrNotFound;

                    }

                    }
                    // VIsibility
                case KCSS_ATTR_VISIBILITY:
                    {
                        switch(aSvgEnumValue)
                        {
                        case 0:
                        return VISIBILITY_VISIBLE;
                        case 1:
                        return VISIBILITY_OTHER;
                        default:
                        return KErrNotFound;

                        }

                    }
                    // additive
                case KAtrAdditive:
                        {
                        switch(aSvgEnumValue)
                            {
                            case 1:
                            return ADDITIVE_SUM;
                                default : return ADDITIVE_REPLACE;

                            }

                        }
                    // accumulate
                    case KAtrAccumulate:
                        {
                            switch(aSvgEnumValue)
                            {
                        case 0:
                        return ACCUMULATE_NONE;
                        case 1:
                        return ACCUMULATE_SUM;
                        default : return KErrNotFound;

                            }

                        }
                // calc-mode
                    case KAtrCalcMode:
                        {
                            switch(aSvgEnumValue)
                            {
                        case 0:
                        return CALC_MODE_DISCRETE;
                        case 1:
                        return CALC_MODE_LINEAR;
                        case 2:
                        return CALC_MODE_PACED;
                        case 3:
                        return CALC_MODE_SPLINE;
                        default: return KErrNotFound;

                            }

                        }
                        // it can not be this attribute.
                        // This fill corresponds to animation.
                // fill attribute
                    case KCSS_ATTR_FILL:
                        {
                            switch(aSvgEnumValue)
                            {
                        case 0:
                        return FILL_REMOVE;
                        case 1:
                        return FILL_FREEZE;
                        default: return KErrNotFound;
                            }

                        }
                // restart attribute.
                    case KAtrRestart:
                        {
                            switch(aSvgEnumValue)
                            {
                        case 0:
                        return RESTART_ALWAYS;
                        case 2:
                        return RESTART_NEVER;
                        case 1:
                        return RESTART_WHENNOTACTIVE;
                        default :
                        return KErrNotFound;
                            }

                        }
                // text decoration.
                    case KCSS_ATTR_TEXTDECORATION:
                        {
                        switch(aSvgEnumValue)
                            {
                        case 1:
                        return TEXT_UNDER_LINE;
                        case 2:
                        return TEXT_OVER_LINE;
                        case 3:
                        return TEXT_LINE_THROUGH;
                        default:
                        return KErrNotFound;
                            }

                        }
                    default:
                    return KErrNotFound;

                    }
}

/*
* This functions checks if an element is in a specified document.
*
*/

TInt CSvgJavaInterfaceImpl::MapSegmentType(TInt aSvgSegmentType)
{
    // the following is a limitation since in GFx2d.dll we do not store the
    // relative or absolute nature of a command.
    // currently we return the absolute.

    switch(aSvgSegmentType)
        {
        case KSvgSegClose:
                {
                return PATH_COMMAND_Z;
                }
        case KSvgSegLineTo:
                {
                    return PATH_COMMAND_L;
                }
        case KSvgSegQuadTo:
                {
                    return PATH_COMMAND_Q;
                }
        case KSvgSegCubicTo:
                {
                    return PATH_COMMAND_C;
                }
        case KSvgSegMoveTo:
                {
                    return PATH_COMMAND_M;
                }
        case KSvgVertical:
                {
                    return PATH_COMMAND_V;
                }
        case KSvgHorizontal:
                {
                    return PATH_COMMAND_H;
                }
        default :
            return 0;
        }
}
