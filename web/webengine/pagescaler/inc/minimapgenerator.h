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



#ifndef CMINIMAPGENERATOR_H
#define CMINIMAPGENERATOR_H

//  INCLUDES

#include <e32base.h>

#include "minimapscaler.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

class CMinimap;
class CFbsBitGc;
class CFbsBitmap;
class CFbsBitmapDevice;
class CWindowGc;
class CMinimapGenerator;
class CMinimapOOMCollector;

// CLASS DECLARATION


/**
*
*
*  @lib webkit.dll
*  @since 3.1
*/
class CMinimapGenerator : public CBase, MMinimapScalerCallback
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CMinimapGenerator* NewL(CMinimap& aMinimap);

        /**
        * Destructor.
        */
        virtual ~CMinimapGenerator();

    public: // New functions


        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        void SetKeepBitmap(TBool aKeepBitmaps);


        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        void Invalidate();

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        void UpdateL(TBool aScrolling=EFalse);

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        void ScrollL();

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        void Clear();


        /**
        * @since 3.1
        * @param
        * @return
        */
        void Draw(CFbsBitGc& aGc, const TRect& aTo) const;
        void Draw(CWindowGc& aGc, const TRect& aTo) const;

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        void SetKeepsBitmaps(TBool aKeepBitmaps);

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        TBool KeepsBitmaps() const;

        /**
        *
        * @since now
        * @param
        * @return
        */
        void DrawColoringMask(CFbsBitGc& aGc, const TRect& aMinimapViewportOnDoc) const;
        void DrawColoringMask(CWindowGc& aGc, const TRect& aMinimapViewportOnDoc) const;

         /**
        *
        * @since now
        * @param
        * @return
        */
        void VisitArea(const TRect &aArea);

        /**
        * @since 3.1
        * @param
        * @return the scaled copy of the page
        */
        CFbsBitmap* ScaledPage() const;


        /**
        * @since 3.1
        * @param
        * @return size of the document being scaled
        */
        TSize DocSize() const { return iDocSize; }

    private: // Functions from MMinimapScalerCallback

        void ScalingCompletedL(CFbsBitmap& aResult, const TRect& aTargetRect);

    public:

        TBool StartAsyncBufferUpdateL();

    private:

        TBool FastBufferUpdateL();
        TBool CheckAndCreateBitmapsL();
    public:
        void DeleteUnscaledBitmap();
        void DeleteBufferBitmap();

#ifdef __OOM__
        TBool IsCollectingMemory();
        void CollectMemory();
#endif

    private:        
        void InvalidateBuffers();
        void CalcBufferPosition();
        TRect BufferRect() const;
        template<class GC> void DrawT(GC& aGc, const TRect& aTo) const;
        template<class GC> void DrawColoringMaskT(GC& aGc, const TRect& aMinimapViewportOnDoc) const;

    private:

        /**
        * C++ constructor.
        */
        CMinimapGenerator(CMinimap& aMinimap);

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();


    private:    // Data

        CMinimap* iMinimap; // not owned
        CMinimapScaler* iScaler; // owned

        CFbsBitmap* iUnscaledBitmap; //owned
        CFbsBitmapDevice* iUnscaledBitmapDevice; //owned
        CFbsBitGc* iUnscaledBitmapGc; //owned
        CFbsBitmap* iBufferBitmap; //owned
        CFbsBitmapDevice* iBufferBitmapDevice; //owned
        CFbsBitGc* iBufferBitmapGc; //owned

        CIdle* iAsyncUpdateStarted; // owned

        RRegion iValidLQRegion;
        RRegion iValidHQRegion;

        TPoint iBufferPos;

        TSize iDocSize;

        TBool iKeepsBitmaps;

        TInt iLastHQUpdatePos;

        /**
        * Maps the visited regions. Used to control mmap coloring.
        */
        RRegion iVisitedRegion;
        
        CMinimapOOMCollector* iOOMCollector;
    };

#endif      // CMINIMAPGENERATOR_H

// End of File
