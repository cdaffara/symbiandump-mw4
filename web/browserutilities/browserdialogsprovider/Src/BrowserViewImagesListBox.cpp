/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*      View images popup dialog listbox
*      
*
*/


// INCLUDES
#include "BrowserViewImagesListBox.h"
#include "BrowserDialogsProviderConstants.h"

#include <gulicon.h>
#include <e32math.h>
#include <AknIconArray.h>
#include <Oma2Agent.h>
using namespace ContentAccess;
//-----------------------------------------------------------------------------
//  CBrowserViewImagesListBox::CBrowserViewImagesListBox
// 
//  EPOC first phase Constructor - must not leave
//-----------------------------------------------------------------------------
//
CBrowserViewImagesListBox::CBrowserViewImagesListBox( )
                        : CAknDoubleLargeGraphicPopupMenuStyleListBox(), 
                        iFormat(), 
                        iCodec(), 
                        iDRMOutputBuf(NULL),
                        iImageIndex(0)
    {
    iRetryingConvert = EFalse;
    }


//-----------------------------------------------------------------------------
//  CBrowserViewImagesListBox::~CBrowserViewImagesListBox
// 
//  Destructor
//-----------------------------------------------------------------------------
//
CBrowserViewImagesListBox::~CBrowserViewImagesListBox()
    {
    delete [] iDRMOutputBuf;
    delete iConverter;
    delete iCurrentBitmap;
    }

//-----------------------------------------------------------------------------
//  CBrowserViewImagesListBox::ConstructL( 
//                                CCoeControl& aParent, 
//                                CArrayFixFlat<TBrCtlImageCarrier>& aImages )
// 
//  Second Phase constructor
//-----------------------------------------------------------------------------
//
void CBrowserViewImagesListBox::ConstructL( 
                                CCoeControl& aParent, 
                                CArrayFixFlat<TBrCtlImageCarrier>& aImages )
    {
    iImages = &aImages;
    
    CAknDoubleLargeGraphicPopupMenuStyleListBox::ConstructL( &aParent, 0 );

    iConverter = CMdaImageDescToBitmapUtility::NewL( *this );

    iIcons = new( ELeave) CAknIconArray( 5 );       // iIcons must not be deleted in destructor! 

    // create placeholder icon
    CGulIcon* icon = CreatePlaceholderIconL();
    CleanupStack::PushL(icon);
    iIcons->AppendL(icon);
    CleanupStack::Pop();    // icon

    ItemDrawer()->ColumnData()->SetIconArray( iIcons );   // iIcons NOT DELETED IN DESTRUCTOR!!!

    // ---------------------------------------------------------------------------------
    // items

    iItemList = STATIC_CAST(CDesCArray*, Model()->ItemTextArray());     // MUST NOT BE DELETED IN DESTRUCTOR

    TBuf<KMaxAltTextLength> buf;
    TInt cstart = 0;
    TInt cend = 0;
    
    // append items with placeholder thumbnail
    for(TInt i=0; i<iImages->Count(); i++)
        {
        buf = _L("0\t");
        TBrCtlImageCarrier& img = iImages->At(i);
        
        if( img.AltText().Length() > 0 )
            {            
            buf.Append(img.AltText().Left( buf.MaxLength() - buf.Length() ) );
            }
        else
            {
            //get filename w/o extension from url
            cstart = img.Url().LocateReverse('/') + 1;
            cend   = img.Url().LocateReverse('.');
            if ( cend <= cstart ) cend = img.Url().Length();
                {
                buf.Append( img.Url().Mid 
                    ( cstart, 
                            ( cend - cstart ) < ( buf.MaxLength() - buf.Length() ) 
                                ?
                            ( cend - cstart ) 
                                :
                            ( buf.MaxLength() - buf.Length() )
            	        ) 
                    );
                }
            }
            
        iItemList->AppendL(buf);
        }
        
    ItemDrawer()->ColumnData()->EnableMarqueeL( ETrue );
    }


//-----------------------------------------------------------------------------
//  CBrowserViewImagesListBox::CurrentlySelectedImage()
//-----------------------------------------------------------------------------
//
const TBrCtlImageCarrier& CBrowserViewImagesListBox::CurrentlySelectedImage()
    {
    return iImages->At( CurrentItemIndex() );
    }


//-----------------------------------------------------------------------------
//  CBrowserViewImagesListBox::CreatePlaceholderIconL()
//-----------------------------------------------------------------------------
//
CGulIcon* CBrowserViewImagesListBox::CreatePlaceholderIconL()
    {
    CFbsBitmap* bitmap = new(ELeave) CFbsBitmap;
    CleanupStack::PushL(bitmap);
    if(KErrNone != bitmap->Create(TSize(42,32), EColor16M))
        User::Leave(KErrNoMemory);

    // create icon
    CGulIcon* icon = CGulIcon::NewL(bitmap); // bitmap is owned, no mask used
    CleanupStack::Pop();    // bitmap
    return icon;
    }


//-----------------------------------------------------------------------------
//  CBrowserViewImagesListBox::Fit()
//-----------------------------------------------------------------------------
//
TSize CBrowserViewImagesListBox::Fit( const TSize& aSrc, const TSize& aTgt )
    {
    TSize result1(aTgt);
    TSize result2(aTgt);

    TInt dA( aTgt.iWidth - aSrc.iWidth );
    TInt dB( aTgt.iHeight - aSrc.iHeight );
    TReal tmp = aSrc.iHeight + ((TReal)(aSrc.iHeight * dA) / (TReal)aSrc.iWidth);
    TReal res;
    Math::Round( res, tmp, 0);
    result1.iHeight = (TInt)res;
    tmp = aSrc.iWidth + ((TReal)(aSrc.iWidth * dB) / (TReal)aSrc.iHeight);
    Math::Round( res, tmp, 0);
    result2.iWidth = (TInt)res;

    TSize result;
    if(result1.iWidth <= aTgt.iWidth && result1.iHeight <= aTgt.iHeight)
        result = result1;
    else
        result = result2;

    return result;    
    }


//-----------------------------------------------------------------------------
//  CBrowserViewImagesListBox::CreateIconL(
//                                      CFbsBitmap* aBitmap, TBool aShrinkIt )
//-----------------------------------------------------------------------------
//
CGulIcon* CBrowserViewImagesListBox::CreateIconL(
                                        CFbsBitmap* aBitmap, TBool aShrinkIt)
    {
    CGulIcon* icon = NULL;

    // create icon
    if(aShrinkIt)
        {
        CFbsBitmap* bmp = new(ELeave)CFbsBitmap;
        CleanupStack::PushL(bmp);
        User::LeaveIfError(bmp->Create(TSize(42,32), EColor16M));
    
        // create bitmap device
        CFbsBitmapDevice* dev = CFbsBitmapDevice::NewL(bmp);
        CleanupStack::PushL(dev);
    
        // create graphics context for bitmap device
        CGraphicsContext* ctx = NULL;
        User::LeaveIfError( dev->CreateContext(ctx) );
        CleanupStack::PushL(ctx);
    
        // calculate aspect ratio
        TSize targetSize = Fit(aBitmap->SizeInPixels(), bmp->SizeInPixels());
        // copy bitmap to temporary bitmap
        ctx->DrawBitmap(TRect(TPoint(0,0), targetSize), aBitmap, TRect(TPoint(0,0), aBitmap->SizeInPixels()));

        CleanupStack::PopAndDestroy(2);   // ctx, dev

        icon = CGulIcon::NewL(bmp); // bmp is owned, no mask used
        CleanupStack::Pop();  // bmp
        delete aBitmap;
        } 
    else 
        {
        icon = CGulIcon::NewL(aBitmap); // bitmap is owned, no mask used
        }

    return icon;
    }


//-----------------------------------------------------------------------------
//  CBrowserViewImagesListBox::StartConversionL()
//-----------------------------------------------------------------------------
//
void CBrowserViewImagesListBox::StartConversionL()
    {
    // if we are out of index, stop
    if (iImageIndex >= iImages->Count())
        {
        // zero non-owned pointers
        iCurrentBitmap = NULL;
        iIcons = NULL;
        iItemList = NULL;

        // delete converter because it is not needed anymore
        delete iConverter;
        iConverter = NULL;
        return;
        }

    // begin open
    TBrCtlImageCarrier& img = iImages->At( iImageIndex );
    
    if ( img.ImageType() == EImageTypeWbmp )
        {
        iConverter->OpenL( img.RawData(), &iFormat, &iCodec );
        }
    else if ( img.ImageType() == EImageTypeOta )
        {
        iConverter->OpenL( img.RawData(), &iFormat, &iCodec );
        }
    else
        {
        iConverter->OpenL( img.RawData() );        
        }
    // MiuoOpenComplete will be called next
    }


//-----------------------------------------------------------------------------
//  CBrowserViewImagesListBox::MiuoConvertComplete( TInt aError )
//-----------------------------------------------------------------------------
//
void CBrowserViewImagesListBox::MiuoConvertComplete( TInt aError )
    {
    TRAPD(err, MiuoConvertCompleteL(aError));
    if(err!=KErrNone)
        {
        DeleteCurrentBitmap();
        TRAP(err, ConvertNextImageL());
        }
    }


//-----------------------------------------------------------------------------
//  CBrowserViewImagesListBox::ConvertNextImageL()
//-----------------------------------------------------------------------------
//
void CBrowserViewImagesListBox::ConvertNextImageL()
    {
    // close converter
    iConverter->Close();    
    iImageIndex++;
    StartConversionL();    
    }


//-----------------------------------------------------------------------------
//  CBrowserViewImagesListBox::CreateCurrentBitmapL( TBool aUseBitmapSize )
//-----------------------------------------------------------------------------
//
TInt CBrowserViewImagesListBox::CreateCurrentBitmapL( TBool aUseBitmapSize )
    {
    DeleteCurrentBitmap();  // just in case
    iCurrentBitmap = new(ELeave) CFbsBitmap;

    TInt err = KErrNone;
    if( aUseBitmapSize )
        {
        err = iCurrentBitmap->Create(iFrameInfo.iOverallSizeInPixels, EColor16M);
        }
    else
        {
        // Store the size of the thumbnail
        err = iCurrentBitmap->Create( TSize(42,32), EColor16M);
        }

    return err;
    }


//-----------------------------------------------------------------------------
//  CBrowserViewImagesListBox::MiuoOpenComplete(TInt aError)
//-----------------------------------------------------------------------------
//
void CBrowserViewImagesListBox::MiuoOpenComplete(TInt aError)
    {
    TRAPD(err, MiuoOpenCompleteL(aError));
    if(err!=KErrNone)
        {
        // just in case, delete iCurrentBitmap if MiuoOpenCompleteL left
        // after allocating the bitmap
        DeleteCurrentBitmap();        

        // try to convert next thumbnail image until all have been tried
        // if this fails, placeholders will still remain
        TRAP(err, ConvertNextImageL());
        }
    }


//-----------------------------------------------------------------------------
//  CBrowserViewImagesListBox::MiuoOpenCompleteL( TInt aError )
//-----------------------------------------------------------------------------
//
void CBrowserViewImagesListBox::MiuoOpenCompleteL( TInt aError )
    {
    if(aError==KErrNone)
        {
        iConverter->FrameInfo(0, iFrameInfo);
        // create new current bitmap for conversion
        TInt err = CreateCurrentBitmapL();
        if(err!=KErrNone)
            {
            // unable to create bitmap, so we won't transfer its ownership to iIcons
            // -> delete
            DeleteCurrentBitmap();
            // try next image if available
            ConvertNextImageL();
            }
        else 
            {
            // start converter, MiuoConvertComplete will be called next
            // if this leaves, iCurrentBitmap will remain allocated, but deleted in MiuoOpenComplete
            iConverter->ConvertL(*iCurrentBitmap);
            }
        }
    else 
        {
        TInt error(KErrNone);
        
        // error occurred, we decide to try next image. placeholder
        // will remain in its place
        // if this leaves, iCurrentBitmap is not allocated
             
        // input buffers for image conversion
        TBrCtlImageCarrier& img = iImages->At( iImageIndex );
        iImageDataPtr.Set( (const TUint8*) img.RawData().Ptr(),
                                                img.RawData().Size() );
            
        
        TInt inputBufLen = iImageDataPtr.Size() + 1; // Input buffer length
        
        HBufC8* bufInput = HBufC8::NewL( inputBufLen + 1 );
        CleanupStack::PushL( bufInput );
        TPtr8 ptrInput( NULL,0,0 );
        ptrInput.Set( bufInput->Des() );
        ptrInput.Append( EView );
        ptrInput.Append( (const TUint8 *) iImageDataPtr.Ptr(), iImageDataPtr.Size() );
        
        // output buffer for image conversion
        if ( iDRMOutputBuf )
            {
            delete [] iDRMOutputBuf;
            iDRMOutputBuf = NULL;
            }
        TInt outputBufLen = iImageDataPtr.Size() + 256; // Output buffer length
        iDRMOutputBuf = new (ELeave) TUint8[outputBufLen];
        TPtr8 ptrOutput( iDRMOutputBuf, 0, outputBufLen );
        
        
        // Do we have a DRM image?
        TAgent agentDRM;
        TRequestStatus status;
        
        ContentAccess::CManager* manager = CManager::NewLC();

        RArray<ContentAccess::TAgent> agents;
       //Add agents to cleanup stack
		CleanupClosePushL(agents);
 
        manager->ListAgentsL( agents );
        for ( TInt i = 0; i < agents.Count(); i++ )
		    {
    		if ( agents[i].Name().Compare( KOmaDrm2AgentName ) == 0)
    			{
    			 agentDRM = agents[i];
    			 break;
    			}
    		}
    	
    	//Cleanup the agents
        CleanupStack::PopAndDestroy(/*agents*/);
	
    	// convert the DRM image (asynchronous)
        manager->AgentSpecificCommand(  agentDRM, 
                                        EBufferContainsOma1Dcf, 
									    iImageDataPtr,
									    ptrOutput, 
									    status );
        // wait for it to finish
        User::WaitForRequest(status);
        
        // Conversion completed ok (if not image is skipped)
        if ( status == KErrNone )
            {
            //Now decrypt the buffer.
		    error = manager->AgentSpecificCommand(   
		                                agentDRM,
		                                EDecryptOma1DcfBuffer,
										ptrInput,
									    ptrOutput);
            if ( !error )
                {                
                iImageDataPtr.Set( (const TUint8*) ptrOutput.Ptr(), 
                                                    ptrOutput.Size() );
                delete iConverter;
                iConverter = NULL;
                TRAP( error, iConverter = CMdaImageDescToBitmapUtility::NewL( *this ); );
                
                // Decrypted ok (if not image is skipped)
                if ( !error )
                    {
                    TRAP_IGNORE( iConverter->OpenL( iImageDataPtr ) );
                    }
                }
            }
        CleanupStack::PopAndDestroy(); // bufInput
        CleanupStack::Pop(); //manager        
        }
    }


//-----------------------------------------------------------------------------
//  CBrowserViewImagesListBox::MiuoConvertCompleteL( TInt aError )
//-----------------------------------------------------------------------------
//
void CBrowserViewImagesListBox::MiuoConvertCompleteL(TInt aError)
    {
    if ( !aError )
        {
        // create icon out of the converted bitmap
        // if retrying convert, we have original size bitmap so shrink it into thumbnail icon
        CGulIcon* icon = CreateIconL(iCurrentBitmap, iRetryingConvert);
        iCurrentBitmap = NULL;      // ownership transferred to icon
        iRetryingConvert = EFalse;  // no more retrying
        CleanupStack::PushL(icon);
        iIcons->AppendL(icon);
        CleanupStack::Pop();        // icon now owned by iIcons

        // append item
        TBuf<KMaxAltTextLength> buf;
        TInt cstart = 1;
        TInt cend = 1;
        buf = _L("");
        buf.AppendNum(iIcons->Count()-1);       // 0 is always the placeholder
        buf.Append(_L("\t"));

        TBrCtlImageCarrier& img = iImages->At(iImageIndex);

        if ( img.AltText().Length() > 0 )
            {
            buf.Append( img.AltText().Left( buf.MaxLength() - buf.Length() ) );
            }
        else
            {
            //get filename w/o extension from url
            cstart = img.Url().LocateReverse('/') + 1;
            cend   = img.Url().LocateReverse('.');
            if ( cend <= cstart ) cend = img.Url().Length();
                {
                buf.Append( img.Url().Mid
                    ( cstart,
                        ( cend - cstart ) < ( buf.MaxLength() - buf.Length() )
                            ?
                        ( cend - cstart )
                            :
                        ( buf.MaxLength() - buf.Length() )
                	    ) 
                    );
                }
            }
            
        // replace item (having placeholder icon) with real thumbnail
        TRAPD(err, iItemList->InsertL(iImageIndex,buf));
        // if succeeded, delete the old one that has moved next to the new item
        // in case error occurred, the placeholder will remain.
        if(err==KErrNone)
            iItemList->Delete(iImageIndex+1);

        // redraw list
        HandleItemAdditionL();

        // open next image and convert it
        ConvertNextImageL();
        }
    else
        {
        if( aError == KErrArgument )
            {
            // retry this image
            // iCurrentBitmap has been allocated when MiuoConvertComplete 
            // is called, so delete it
            DeleteCurrentBitmap();
            CreateCurrentBitmapL(ETrue);        // use original image size
            iRetryingConvert = ETrue;
            iConverter->ConvertL(*iCurrentBitmap);      // retry conversion
            }
        else
            {
            DeleteCurrentBitmap();
            // convert next image (anyway)
            ConvertNextImageL();
            }
        }
		}


//-----------------------------------------------------------------------------
//  CBrowserViewImagesListBox::DeleteCurrentBitmap()
//-----------------------------------------------------------------------------
//
void CBrowserViewImagesListBox::DeleteCurrentBitmap()
    {
    delete iCurrentBitmap;
    iCurrentBitmap = NULL;
    }



//-----------------------------------------------------------------------------
//  CBrowserViewImagesListBox::DeleteCurrentBitmap()
//
// not used - comes from abstract base class
//-----------------------------------------------------------------------------
//
void CBrowserViewImagesListBox::MiuoCreateComplete(TInt /* aError */)
    {
    }


// End of File
