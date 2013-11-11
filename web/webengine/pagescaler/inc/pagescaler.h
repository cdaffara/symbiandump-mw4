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
*
*/



#ifndef PAGESCALER_H
#define PAGESCALER_H

//  INCLUDES

#include <e32base.h>
#include <gdi.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

class CWindowGc;
class CFbsBitGc;
class CFbsBitmap;
class CCoeControl;


// CLASS DECLARATION

/**
*
*
*  @lib webkit.dll
*  @since 3.1
*/
class MPageScalerCallback
    {
    public:
        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        virtual void DrawDocumentPart(CFbsBitGc& aGc, CFbsBitmap* aBitmap, const TRect& aDocumentAreaToDraw) = 0;

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        virtual TRect DocumentViewport()  = 0;

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        virtual void ScaledPageChanged(const TRect& aArea, TBool aFullScreen, TBool aScroll) = 0;

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        virtual TSize DocumentSize() = 0;

        /**
        *
        * @since 3.2
        * @param
        * @return
        */
        virtual TBool TouchScrolling() = 0;
        
        
        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        virtual CCoeControl& PageControlView() = 0;
    };

// CLASS DECLARATION

/**
*
*
*  @lib webkit.dll
*  @since 3.1
*/
class CPageScaler : public CBase
    {
    public:

        enum TPosition
            {
            ETopLeft,
            ETopRight,
            EBottomLeft,
            EBottomRight
            };

    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        IMPORT_C static CPageScaler* NewL(MPageScalerCallback& aCallback, TDisplayMode aMode, TBool aLowQuality);

        /**
        * Destructor.
        */
        virtual ~CPageScaler();

    public:

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        virtual void SetVisibleUntil(TTimeIntervalMicroSeconds32 aUntil);

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        virtual void SetVisible(TBool aVisible);

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        virtual TBool Visible() const;

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        virtual void SetZoomOutLevel(TInt aPercent);

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        virtual TInt ZoomOutLevel() const;

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        virtual void SetMaxSize(TSize aSize);

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        virtual TSize MaxSize() const;

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        virtual void SetTransparency(TInt aPercent);

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        virtual TInt Transparency() const;

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        virtual TRect Rect() const;

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        virtual void SetRelativePosition(TInt aXDist, TInt aYDist, TPosition aPosition);

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        virtual TRect IndicatorRect() const;

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        virtual void DocumentStarted();

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        virtual void DocumentChangedL();

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        virtual void DocumentCompleted();

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        virtual void DocumentViewportMoved();

        /**
        * @since 3.1
        * @param
        * @return
        */
        virtual void Draw(CWindowGc& aGc, const TRect& aRect) const;

        /**
        * @since 3.1
        * @param
        * @return the scaled copy of the page
        */
        virtual CFbsBitmap* ScaledPage() const;

        /**
        * @since 3.1
        * @return
        */
        virtual TRect ContainerRect() const;

        /**
        * @since 3.1
        * @return
        */
        virtual void SetContainerRect(const TRect& aRect);

        /**
        * @since 3.1
        * @return
        */
        virtual TBool FullScreenMode() const;

        /**
        * @since 3.1
        * @return
        */
        virtual void SetFullScreenMode(TBool aFullScreen);

        /**
        * @since 3.1
        * @return
        */
        IMPORT_C static TBool HasOverlaySupport();


    protected:

        /**
        * C++ default constructor.
        */
        CPageScaler();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
    };


#endif      // PAGESCALER_H

// End of File
