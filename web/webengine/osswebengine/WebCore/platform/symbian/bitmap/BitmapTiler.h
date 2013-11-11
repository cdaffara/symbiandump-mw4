/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __BITMAPTILER_H__
#define __BITMAPTILER_H__

#include <e32base.h>
#include <e32def.h>

class CMaskedBitmap;
class TSrcScanLine;
class TSrcScanLineMasked;
class TSrcScanLineAlpha;

class TFastWrapUtil
    {
    public:
        TFastWrapUtil()         {}
        TFastWrapUtil( const TPoint& aPt, TInt aWidth, TInt aHeight ) 
            : iOrig( aPt ), iDelta( aPt ), iSrcWidth( aWidth ), iSrcHeight( aHeight )  {}
        void Set( const TPoint& aPt, TInt aWidth, TInt aHeight ) { iOrig = aPt; iDelta = aPt; iSrcWidth = aWidth, iSrcHeight = aHeight; }
        inline void NextLine();
        inline void JumpLines( TInt aSteps );
        inline void operator++()                { if( ++iDelta.iX == iSrcWidth )  iDelta.iX = iOrig.iX; }
        inline const TPoint& operator*() const  { return iDelta; }
        inline void operator+=( TInt aSteps );
        
    private:
        TPoint iOrig;
        TPoint iDelta;
        TInt iSrcWidth, iSrcHeight;
    };

class CScanLineFetcher : public CBase
    {
    public:
        CScanLineFetcher( CMaskedBitmap* aImage, TInt aStartPos, TInt aStartLine );
        
        virtual ~CScanLineFetcher()         {}
        virtual void Fetch( TSrcScanLine& ) = 0;
        virtual void Fetch( TSrcScanLineMasked& ) = 0;
        virtual void Fetch( TSrcScanLineAlpha& aScanLine ) = 0;

        void operator++();
        void operator+=( TInt aSteps );
		void operator=( TInt aLine )		{ iLine = aLine; }        
    
    protected:
        CMaskedBitmap*      iImage;
        TInt                iLine;
        TInt                iOffsetX;
        TFastWrapUtil       iWrap;

        // scanline data
        TUint16*            iBmpLine;
        TUint16*            iMskLine;
        TInt                iBmpLineSize;
        TInt                iMskLineSize;
    };

class CCompressedScanLineFetcher : public CScanLineFetcher
    {
    public:
        static CCompressedScanLineFetcher* NewL( CMaskedBitmap* aImage, TInt aStartPos, TInt aStartLine );
        
        CCompressedScanLineFetcher( CMaskedBitmap* aImage, TInt aStartPos, TInt aStartLine );
        ~CCompressedScanLineFetcher();
        void ConstructL();
                
        // composite scanlines
        void Fetch( TSrcScanLine& );
        void Fetch( TSrcScanLineMasked& );
        void Fetch( TSrcScanLineAlpha& aScanLine );
    };

class CFastScanLineFetcher : public CScanLineFetcher
    {
    public:
        static CFastScanLineFetcher* NewL( CMaskedBitmap* aImage, TInt aStartPos, TInt aStartLine );

        CFastScanLineFetcher( CMaskedBitmap* aImage, TInt aStartPos, TInt aStartLine );
        ~CFastScanLineFetcher();
        void ConstructL();
        
        // composite scanlines
        void Fetch( TSrcScanLine& );
        void Fetch( TSrcScanLineMasked& );
        void Fetch( TSrcScanLineAlpha& aScanLine );
    };    

// a 16-bit scanline
class TSrcScanLine
    {
    public:
        TSrcScanLine()
            : iSrcLine( 0 ), iPos( 0 )          {}
        void Set( TUint16* aLine, TInt aStart ) { iSrcLine = aLine; iPos = aStart; }                
        inline void operator++()                { iPos++; }
        inline void operator+=( TInt aSteps )   { iPos+=aSteps; }
		inline void operator=( TInt aPos )		{ iPos = aPos; }

        inline void Mix( TUint16* aTarget );
        inline void Mix( TUint32* aTarget );
        
    protected:
        TUint16*    iSrcLine;
        TInt        iPos;
    };

// a 16-bit 2-bit masked scanline
class TSrcScanLineMasked
    {
    public:
        TSrcScanLineMasked() 
                : iSrcLine(0), iSrcMask(0), iPos(0)             {}
        void Set( TUint16* aLine, TUint16* aMask, TInt aStart ) { iSrcLine = aLine; iSrcMask = aMask; iPos = aStart; }
        inline void operator++()                                { iPos++; }
        inline void operator+=( TInt aStep )                    { iPos+=aStep; }
		inline void operator=( TInt aPos )						{ iPos = aPos; }
        inline void Mix( TUint16* aTarget );
        inline void Mix( TUint32* aTarget );
        
    protected:
        TUint16* iSrcLine;
        TUint16* iSrcMask;
        TInt     iPos;
    };

// a 16-bit 8-bit alpha scanline
class TSrcScanLineAlpha
    {
    public:
        TSrcScanLineAlpha()
            : iSrcLine(0), iAlpha(0), iPos(0)                   {}
        void Set( TUint16* aLine, TUint8* aAlpha, TInt aStart)  { iSrcLine = aLine; iAlpha = aAlpha; iPos = aStart; }
        inline void operator++()                                { iPos++; }
        inline void operator+=( TInt aStep )                    { iPos += aStep; }
        inline void operator=( TInt aPos )                      { iPos = aPos; }
        inline void Mix( TUint16* aTarget );
        inline void Mix( TUint32* aTarget );

    protected:
        TUint16* iSrcLine;
        TUint8*  iAlpha;
        TInt     iPos;
    };

template <class S, typename T>
class TScanLineTiler
    {
    public:
        TScanLineTiler( TInt aTrgWidth, TInt aOffsetX )
            : iTrgWidth( aTrgWidth ), iOffsetX( aOffsetX ) { }
        void MixLine( T& aTarget, S& aSrcLine, TFastWrapUtil& aWrap );
    private:
        TInt iTrgWidth;
        TInt iOffsetX;        
    };    

template <class S, typename T>
void TileBitmap( T& aTarget, TInt aStride, CScanLineFetcher& aFetcher, TScanLineTiler<S, T>& aTiler,
                TInt aTrgHeight, TInt aSrcWidth, TInt aSrcHeight, const TPoint& aOffset );                

// inline functions
#include "BitmapTiler.inl"

#endif //!__BITMAPTILER_H__
