/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/



#ifndef WEBCANNEDIMAGES_H
#define WEBCANNEDIMAGES_H

//  INCLUDES
#include <e32base.h>
#include <bitstd.h>

#include <AknsConstants.h>
#include <AknsItemID.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION
class TCannedImageData
    {
    public:
        TCannedImageData(): m_img(0), m_msk(0)       {}
    public:
        CFbsBitmap* m_img;
        CFbsBitmap* m_msk;
    };

/**
*
*  @lib browserView.lib
*  @since 3.0
*/
class WebCannedImages : public CBase
    {

    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static WebCannedImages* NewL();

        /**
        * Destructor.
        */
        virtual ~WebCannedImages();

    public: // New functions

        /**
        * Returns the image represented by the id
        * @since 3.0
        * @param
        * @return
        */
        TCannedImageData& getImage( TInt imgid );

        /**
        * Gets the folder where the canned image is located
        * @since 3.0
        * @param
        * @return
        */
        static HBufC* getImageDirL( );

        /**
        * Loads the canned image
        * @since 3.0
        * @param
        * @return
        */
        static void loadImageDirL( TInt imgid, const TDesC& filename, TCannedImageData& img );

        /**
        * Loads svg icon
        * @since 3.0
        * @param
        * @return
        */
        static void loadSvg( TInt imgid, TAknsItemID aknsid, const TDesC& filename, TCannedImageData& img );

    private:


        /**
        * C++ default constructor.
        */
        WebCannedImages();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    public:

        enum TCannedImageNames
              {
              EImageSquare = 0,
              EImageDisc,
              EImageCircle,
              EImageMissing,
              EImageSelectArrow,
              EImageObject,
              EImageSelectFile,
              EImageMap,
              EImageRadioButtonOff,
              EImageRadioButtonOn,
              EImageRadioButtonOnSel,
              EImageRadioButtonOffSel,
              EImageCheckBoxOff,
              EImageCheckBoxOn,
              EImageCheckBoxOnSel,
              EImageCheckBoxOffSel,
              EImageFingerBitmap,
              EImageArrowBitmap,
              EImageWaitArrowBitmap,
              EImageIBeam,
              EImageSelectMulti,
              EImageSmartLinkPhone,
              EImageSmartLinkEmail,
              EImageUrlCall,
              EImageUrlEmail,
              EImageUrlMm,
              EImageUrlMms,
              EImageUrlPage,
              EImageUrlVideo,
              EImageEscFullScreen,
              KNumberOfCannedImages
              };

    private: // Data
        TCannedImageData m_imgarray[KNumberOfCannedImages];
        HBufC*           m_mbmfile;
        HBufC*           m_svgfile;

    };

#endif      // WebCannedImages_H

// End of File
