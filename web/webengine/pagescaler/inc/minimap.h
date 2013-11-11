/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef CMINIMAP_H
#define CMINIMAP_H

//  INCLUDES
#include "pagescaler.h"

#include <e32base.h>
#include <gdi.h>
#include <w32std.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

class CWindowGc;
class CMinimapGenerator;
class CFbsBitGc;
class CFbsBitmap;
class CFbsBitmapDevice;
class CMinimapTimer;


// CLASS DECLARATION

/**
*
*
*  @lib pagescaler.dll
*  @since 3.1
*/
class CMinimap : public CPageScaler
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CMinimap* NewL(MPageScalerCallback& aCallback, TDisplayMode aMode, TBool aLowQuality );

        /**
        * Destructor.
        */
        virtual ~CMinimap();

    public: // New functions


        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        void SetVisibleUntil(TTimeIntervalMicroSeconds32 aUntil);

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        void SetVisible(TBool aVisible);

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        inline TBool Visible() const;

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        inline void SetZoomOutLevel(TInt aPercent);

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        inline TInt ZoomOutLevel() const;

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        void SetPercentSize(TInt aWidthPercent, TInt aHeightPercent);

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        void SetTransparency(TInt aPercent);

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        inline TInt Transparency() const;

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        TRect Rect() const;

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        TRect MinimapRect() const;


        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        TRect IndicatorRect() const;

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        void SetRelativePosition(TInt aXDist, TInt aYDist, TPosition aPosition);

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        void DocumentStarted();

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        void DocumentChangedL();

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        void DocumentCompleted();

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        void DocumentViewportMoved();

        /**
        * @since 3.1
        * @param
        * @return
        */
        void Draw(CWindowGc& aGc, const TRect& aRect) const;

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        TRect ViewportOnDocument() const;


        /**
        * @since 3.1
        * @param
        * @return the scaled copy of the page
        */
        CFbsBitmap* ScaledPage() const;

        /**
        * @since 3.1
        * @return
        */
        TRect ContainerRect() const;

        /**
        * @since 3.1
        * @return
        */
        void SetContainerRect(const TRect& aRect);

        /**
        * @since 3.1
        * @return
        */
        TBool FullScreenMode() const;

        /**
        * @since 3.1
        * @return
        */
        void SetFullScreenMode(TBool aFullScreen);


        /**
        * @since 3.1
        * @return
        */
        void UpdateNow();

#ifdef __OOM__
        void DeleteMinimapBitmap();
#endif

        TBool LowQuality() const       { return iLowQuality; }

    protected:

        /**
        * C++ constructor.
        */
        CMinimap(MPageScalerCallback& aCallback, TDisplayMode aMode, TBool aLowQuality);

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:
        friend class CMinimapGenerator;

        TRect FromDocCoords(const TRect& aFrom) const;
        TPoint FromDocCoords(const TPoint& aFrom) const;
        TSize FromDocCoords(const TSize& aFrom) const;
        TRect ToDocCoords(const TRect& aFrom) const;
        TPoint ToDocCoords(const TPoint& aFrom) const;
        TSize ToDocCoords(const TSize& aFrom) const;

        void CheckAndCreateMinimapBitmapsL();
        TBool CheckAndCreateBitmapL(TSize aSize, CFbsBitmap*& aBm, CFbsBitmapDevice*& aDev, CFbsBitGc*& aGc);

       	void InitColoringL(const TSize aSize);

        inline MPageScalerCallback& Callback() const;

        TSize CalcSize() const;
        TRect CalcViewportOnDocument() const;

        void DocumentChangedCbL();
        void UpdateCbTimerCbL();
        void UpdateTimerCbL();
        void VisibilityTimerCbL();

        void UpdateTransparency();

        TRect ViewportOnMinimap() const;

        template<class GC> void DrawT(GC& aGc, const TRect& aRect) const;

        TInt BorderWidth() const;
        
        void ConstructSprite();

    public:

        TBool IsDocumentComplete() const { return iDocumentComplete; }

    private:    // Data

        // callbacks
        MPageScalerCallback* iCallback;

        // generator
        CMinimapGenerator* iGenerator; // owned

        // zoom level of the minimap in percent
        TInt iZoomOutLevel;

        // transparency in percent
        TInt iTransparency;

        // distance from the viewport edges (d<0 => right, bottom)
        TInt iXDist;
        TInt iYDist;

        // is it visible
        TBool iVisible;

        TDisplayMode iDisplayMode;

        CFbsBitmap* iMinimapBitmap; //owned
        CFbsBitmapDevice* iMinimapBitmapDevice; //owned
        CFbsBitGc* iMinimapBitmapGc; //owned
        
        CFbsBitmap* iMinimapMaskBitmap; //owned
        CFbsBitmapDevice* iMinimapMaskBitmapDevice; //owned
        CFbsBitGc* iMinimapMaskBitmapGc; //owned

        // Minimap coloring
        CFbsBitmap *iMinimapColoringBitmap; // owned
        CFbsBitmap *iMinimapColoringBitmapMask; // owned
        CFbsBitmapDevice* iMinimapColoringBitmapMaskDevice; //owned
        CFbsBitGc* iMinimapColoringBitmapMaskGc; //owned


        TRect iViewportOnDocument;

        TBool iNeedsUpdate;

        CMinimapTimer* iUpdateTimer; //owned
        CMinimapTimer* iUpdateCbTimer; //owned
        CMinimapTimer* iVisibilityTimer; //owned

        CMinimapTimer* iFadeTimer; //owned
        TBool iFadeDirection; //owned

        TBool iDocumentComplete;

        TInt iWidthPercent;
        TInt iHeightPercent;

        TRect iContainerRect;

        TBool iFullScreenMode;
        TBool iMinimapUpdating;

        TBool iLowQuality;
        
        RWsSprite iSprite;
    };

#include "minimap.inl"

#endif      // CMINIMAP_H

// End of File
