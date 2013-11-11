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
* Description:  SVG Implementation header file
 *
*/


#ifndef _INC_CSVGIMAGEELEMENTIMPL_
#define _INC_CSVGIMAGEELEMENTIMPL_

#include <imageconversion.h>

#include "SVGElementImpl.h"
#include "GfxRectangle2D.h"
#include "SVGRequestObserver.h"

#include "SVGImageLoadingObserver.h" // MSVGImageLoadingObserver

class   CFbsBitmap;
class   CSvgDocumentImpl;
class   MXmlDocument;

/**
 * This class is for internal use to receive notification from other
 * <image> elements.  This case is when an <image> element is Cloned
 * by a <use> element
 */
class MSvgImageElementListener
{
    public:
        /**
         * Inform the listener that the image is decoded and update its
         * bitmap pointers
         *
         * @since s60 3.0
         * @return : None.
         */
        virtual void BitmapDecoded( CFbsBitmap* aBitmap, CFbsBitmap* aMask ) = 0;
        
        /**
         * Inform the listener to reset its reference to the referred element
         * @since s60 3.1
         * @return : None.
         */
        virtual void ResetReference() = 0;
};

/**
 * Class description goes here.
 *
 *  @lib SVGEngine.lib
 *  @since 1.0
 */
class CSvgImageElementImpl : public CSvgElementImpl,
                             public MSVGImageLoadingObserver,
                             public MSvgImageElementListener

    {
    public:

        /**
         * Helper class to load images asynchronously.  To load an image:
         * 1.  Instantiate CSvgImageLoaderUtil with NewL/NewLC, passing
         *     a MSVGImageLoadingObserver, which implements the 'ImageLoadingCompleted'
         *     method, and a file name.  This method is called by the CSvgImageLoaderUtil
         *     object when loading is complete.  The error code passed-in will indicate
         *     a successful load or not.
         * 2.  Set the output mode with 'SetDisplayMode' the default is EColor64K, see
         *     TDisplayMode for more options.
         * 3.  Call one of the 'RequestForImageL' method.
         *
         *  @lib SVGEngine.lib
         *  @since 1.0
         */
         class CSvgImageLoaderUtil : public CActive
            {
            public:
                /**
                 * Constructor
                 *
                 * @since 1.0
                 * @param aObserver : callback object when image loading completes.
                 * @param aFileHandle : file handle of image to load.
                 * @param aSyncLoading : Perform Synchronous loading(ETrue) or not
                 * @return
                 */
                static CSvgImageLoaderUtil* NewL( MSVGImageLoadingObserver* aObserver,
                                                  RFile& aFileHandle,
                                                  TBool aSyncLoading );

                /**
                 * Constructor
                 *
                 * @since 1.0
                 * @param aObserver : callback object when image loading completes.
                 * @param aEncodedData : encode 8-bit data buffer
                 * @param aSyncLoading : Perform Synchronous loading(ETrue) or not
                 * @return
                 */
                static CSvgImageLoaderUtil* NewL( MSVGImageLoadingObserver* aObserver,
                                                  const TDesC8& aEncodedData,
                                                  TBool aSyncLoading );

                /**
                 * Destructor
                 *
                 * @since 1.0
                 * @return
                 */
                ~CSvgImageLoaderUtil();

                /**
                 * Set the output mode, the default value is EColor64K.
                 *
                 * @since 1.0
                 * @param aDisplayMode : output mode.
                 * @return
                 */
                void SetDisplayMode( TDisplayMode aDisplayMode );

                /**
                 * Request for the image and size the bitmap to the size of the image.
                 *
                 * @since 1.0
                 * @param aBitmap    : bitmap to store the pixels
                 * @param aMask      : bitmap to store the Mask pixels
                 * @param aHasAlpha  : Alpha value indicator, Used to create Mask
                 * @param aFrameInfo : TFrameInfo structure
                 * @return
                 */
                void RequestForImageL( CFbsBitmap& aBitmap,
                                       CFbsBitmap& aMask,
                                       TBool       aHasAlpha,
                                       TFrameInfo  aFrameInfo );

                /**
                 * From CActive Handles cancelling of asynchronous requests
                 *
                 * @param aBitmap : bitmap to store the pixels
                 * @since 2.0
                 */
                void DoCancel();

                /**
                 * From CActive Called when asynchronous request is completed
                 *
                 * @since 2.0
                 */
                void RunL();

                /**
                 * From CActive Called when asynchronous request is completed
                 *
                 * @since 2.0
                 * @param aError : error code
                 */
                TInt RunError( TInt aError );

                /**
         		 * Return wether image decoding is synchronous or asynchronous.
                 * @since 3.1
                 * @param None
                 * @return ETrue if image decoding is synchrounous else EFalse
                 */
                 TBool               IsSynchronousLoading();
                 
                /**
                 * Returns the FrameInfo of Image. Function is called once the Decoder is
                 * created.
                 * @since 3.2
                 * @param None
                 * @return TFrameInfo structure.
                 */
                 
                 TFrameInfo          GetFrameInfo();
            private:
                /**
                 * Constructor
                 *
                 * @since 1.0
                 */
                CSvgImageLoaderUtil();

                /**
                 * Construct this object with leavable calls.
                 *
                 * @since 1.0
                 * @param aObserver : callback object when image loading completes.
                 * @param aFileHandle : file handle of image to load.
                 * @param aSyncLoading : Perform Synchronous loading(ETrue) or not
                 */
                void ConstructL( MSVGImageLoadingObserver* aObserver, 
                    RFile& aFileHandle,
                    TBool aSyncLoading );

                /**
                 * Phase two of construction.
                 *
                 * @since 1.0
                 * @param aObserver : callback object when image loading completes.
                 * @param aEncodedData : encode 8-bit data buffer
                 * @param aSyncLoading : Perform Synchronous loading(ETrue) or not
                 */
                void ConstructL( MSVGImageLoadingObserver* aObserver,
                                 const TDesC8& aEncodedData,
                                 TBool aSyncLoading );


            private:
                MSVGImageLoadingObserver* iObserver;
                CImageDecoder*  iImageDecoder;
                TDisplayMode iDisplayMode;
                CActiveSchedulerWait* iActiveSchedulerWait;
                TBool iIsSyncLoading;
                HBufC8* iImageByteData;
            };

    public:
        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        MXmlElement*        CloneL(MXmlElement* aParentElement);

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        static CSvgImageElementImpl*NewL(  const TUint8 aElemID,
                                      CSvgDocumentImpl* aDoc );

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        static CSvgImageElementImpl*NewLC(  const TUint8 aElemID,
                                       CSvgDocumentImpl* aDoc );
    private:
        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
         void                            ConstructL(  const TUint8 aElemID,
                                                    CSvgDocumentImpl* aDoc );



    public:

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual                         ~CSvgImageElementImpl();


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        const TDesC&                    Par();

		TInt 	CheckRequiredAttributes();
		
        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                            SetParL( const TDesC& aName );

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                            ImageLoadingCompleted( TInt aErrorStatus );

        // From SVG DOM

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TFloatFixPt                          X();

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TFloatFixPt                          Y();

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TFloatFixPt                          Width();

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TFloatFixPt                          Height();

        // SVG Implementation

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                            SetWidth( TFloatFixPt aWidth );

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                            SetHeight( TFloatFixPt aHeight );

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                            SetUriL( const TDesC& aUri );

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                            LoadUriL();

        // From MXmlElement API


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TInt                            SetAttributeL( const TDesC& aName,
                                                       const TDesC& aValue );


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TBool                           DrawL( CGfx2dGc* aGc,
                                               CSvgElementImpl* aElement );


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                            GetBBox( TGfxRectangle2D& aBbox );

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual void                    GetUnscaledBBox( TGfxRectangle2D& /*aBbox*/ );

        // From MXmlElementOpt API


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TInt                            GetAttributeFloat( const TInt aNameId,
                                                           TFloatFixPt& aValue );

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TInt                            SetAttributeFloatL( const TInt aNameId,
                                                            const TFloatFixPt aValue );


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TInt                            SetAttributeDesL( const TInt aNameId,
                                                          const TDesC& aValue );
        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
                                        CSvgImageElementImpl( CSvgDocumentImpl* aDoc );

        TInt                GetAttributeDes( const TInt aNameId,
                                                     TPtrC16& aValue );

        /**
         * Return wether an svg element is displayable, such as <rect>, <circle>, or
         * an abstract element, such as <g>, <animate>.
         *
         * @since 1.0
         * @param None
         * @return ETrue if element is viewable.
         */
        TBool               IsViewable() { return ETrue; }

        // Adding/Removing listener interested in image decoding notification
        void AddImageElementListener( MSvgImageElementListener* aListener );
        void RemoveImageElementListener( MSvgImageElementListener* aListener );

        // MSvgImageElementListener method
        void BitmapDecoded( CFbsBitmap* aBitmap, CFbsBitmap* aMask );
        void ResetReference();
        
        void Print( TBool aIsEncodeOn );

        CFbsBitmap*                 GetBitmap();        
        
        void SetBitmap( CFbsBitmap* aBitmap );

        // Assigned image data for this object to manage delete
        // after image-decoding or destructing.
        void AssignImageData( HBufC8* aData );

        /**
         * Starts the image decoding. This is the version that accepts 
         * descriptor data of the image content. Image can be SVG/ any format
         * supported by CImageDecoder 
         * @since s60 3.2
         * @param aImageData Image Data Descriptor
         * @param aSyncLoading ETrue=Sync Loading
         * @return TInt errorcode
         */
        TInt StartImageDecoding( const TDesC8& aImageData,
                                 TBool aSyncLoading );
        
        /**
         * Starts the image decoding. This is the version that accepts 
         * file handle of the image content. Image can be SVG/ any format
         * supported by CImageDecoder 
         * @since s60 3.2
         * @param aImageFile Image File handle
         * @param aBitmap Bitmap Pointer
         * @param aSyncLoading ETrue=Sync Loading
         * @return TInt errorcode
         */
        TInt StartImageDecoding( RFile& aImageFile,
                                 CFbsBitmap* aBitmap,
                                 TBool aSyncLoading );
        /**
         * Informs the image element that it is referring to a 
         * particular element. This is used in case an image element
         * is cloned.
         * @since s60 3.2
         * @param aImageElement Image Element Pointer
         * @return None.
         */
        void SetReferenceElement( CSvgImageElementImpl* aImageElement );

        /**
         * Get function for reference element
         * @since s60 3.2
         * @param none 
         * @return Image Element Pointer.
         */
        
        CSvgImageElementImpl* ReferenceElement();
        
    private:
        /**
         * Notify the listeners (MSvgImageElementListener) when 
         * Image decoding is completed
         * @since s60 3.0
         * @param None.
         * @return None. 
         */
        void NotifyImageDecoded();
        /**
         * Notify the listeners (MSvgImageElementListener) to dereference 
         * the reference element. This function can be called for eg.
         * when the reference element is getting deleted.
         * @since s60 3.2
         * @param None.
         * @return None. 
         */
        void NotifyResetReference();
        void PrepareImageWithMask();
    protected:

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                            CopyL( CSvgImageElementImpl* aDestElement );

    void                                PrepareImageFromSvgFile(const TDesC8& aImageData);


 private:
        /**
         * Process an encoded image in the xlink:href attribute value.
         *
         * @since 1.0
         * @param aXlinkHrefValue : the string of the xlink:href attribute value.
         * @return
         */
        void                            ProcessEncodedImageL( const TDesC& aXlinkHrefValue );


    private:
        TGfxRectangle2D                 iImgRect;
        HBufC*                          iPar;
        TBool                           iBitmapOrgReady;
        CSvgImageLoaderUtil*            iImageLoader;
        TBool                           iRenderImage;

        TBool                           iUsedImage;

        HBufC8*                         iImageData;

        RPointerArray<MSvgImageElementListener> iImageElementListeners;
        // Indicates whether the URI for xlink:href attribute is set.
        TBool                           iIsUriSet;
        TBool                           iImageDecodingDone;
    public:
        CFbsBitmap*                     iBitmap;
        CFbsBitmap*                     iMask;
        TBool                           iBitmapReady;
        TBool                           iBitmapLoadCalled;
        TInt                            iDownloadWait;
        TFileName                       iFullPathFileName;
        CPeriodic*                      iImageDownloadTimer;

        RFs                             iSession;
        TBool                           iSessionConnected;
        TBool                           iIsBase64;

        TBool                           iIsSvgFile;
        // Image Reference Element - used in clone scenario
        CSvgImageElementImpl*           iRefElement;
    };




#endif /*_INC_CSVGIMAGEELEMENTIMPL_INCLUDED */
