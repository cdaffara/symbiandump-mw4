/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   AlfRenderStage provides a method for redirect avkon drawing to a graphics accelerator
*
*/

#ifndef CALFRSGC_H_
#define CALFRSGC_H_

#include <graphics/wsgraphicscontext.h>
#include <s32mem.h>
#include <e32std.h>
#include <gdi.h>

class CAlfRsSendBuffer;
class CFbsBitGc;
class CFbsScreenDevice;

const TInt KAlfRsGcMaxClippingRects = 20;

/**
 *  This class serializes commands sent to graphics context by using CAlfRsSendBuffer . 
 *  
 *  @lib alfrenderstage.lib
 *  @since S60 v5.2
 */

NONSHARABLE_CLASS(CAlfGraphicsContext) : public CBase, public MWsGraphicsContext
		{
	public: // New methods

        /**
         * NewL
         * 
         * Constructor
         *
         * @param   aAlfSendBuffer Send buffer
         * @return  New instance of the class
         */
		static CAlfGraphicsContext* NewL( CAlfRsSendBuffer& aAlfSendBuffer );

        /**
         * C++ destructor
         */
		~CAlfGraphicsContext();

        /**
         * ResetDrawCommandCount
         *
         * Each call of command which requires drawing increases iDrawCommandCount. This is used for
         * skipping serialization of draw commands that have no effect on screen. @See  DrawCommandCount.
         */
        void ResetDrawCommandCount() 
            {
            iDrawCommandCount = 0; 
            };

        /**
         * DrawCommandCount
         *
         * @return Amount of effective draw commands since last call of ResetDrawCommand.
         */
        TInt DrawCommandCount()
            {
            return iDrawCommandCount; 
            };
    
#ifdef _OPTIMIZE_WS_COMMANDS_ADVANCED_
        /**
         * CheckDelayedClipping
         * 
         * Assertains if the previous clipping (EAlfSetClipRegion) command can be delayed or skipped.
         * 
         * @param   aClippingRect
         * @return  previous clipping command status 
         */
        TBool CheckDelayedClipping( TRect aClippingRect );
#endif
#ifdef _OPTIMIZE_WS_COMMANDS_BASIC_     
        /**
         * FlushStateL
         * 
         * Flushes all the modified Gc states.
         * 
         * @param aRollbackClippingState contains states that should be rollbacked after flush.
         *        Currently this is relevant only for clipping.
         * 
         */
        void FlushStateL(  TInt& aRollbackClippingState );
#endif
        
	public: // From MWsGraphicsContext
	    
        /**
         * Following commands are serialized
         */

	    void BitBlt(const TPoint& aDestPos, const CFbsBitmap& aSourceBitmap);
		void BitBlt(const TPoint& aDestPos, const CFbsBitmap& aSourceBitmap, const TRect& aSourceRect);
		void BitBltMasked(const TPoint& aDestPos, const CFbsBitmap& aSourceBitmap, const TRect& aSourceRect, const CFbsBitmap& aMaskBitmap, TBool aInvertMask);
		void BitBltMasked(const TPoint& aDestPos, const CFbsBitmap& aSourceBitmap, const TRect& aSourceRect, const CFbsBitmap& aMaskBitmap, const TPoint& aMaskPos);
		void ResetClippingRegion();
		void Clear();
		void Clear(const TRect& aRect);
		void ResetBrushPattern();
		void ResetFont();
		void DrawArc(const TRect& aRect, const TPoint& aStart, const TPoint& aEnd);
		void DrawPie(const TRect& aRect, const TPoint& aStart, const TPoint& aEnd);
		void DrawBitmap(const TRect& aDestRect, const CFbsBitmap& aSourceBitmap);
		void DrawBitmap(const TRect& aDestRect,	const CFbsBitmap& aSourceBitmap, const TRect& aSourceRect);
		void DrawBitmap(const TPoint &aTopLeft, const CFbsBitmap &aSource);
		void DrawBitmapMasked(const TRect& aDestRect, const CFbsBitmap& aSourceBitmap, const TRect& aSourceRect, const CFbsBitmap& aMaskBitmap, TBool aInvertMask);
		void DrawRoundRect(const TRect& aRect, const TSize& aEllipse);
		void DrawPolyLine(const TArray<TPoint>& aPointList);
		void DrawPolyLine(const CArrayFix<TPoint>* aPointList);
		void DrawPolyLine(const TPoint *aPointList, TInt aNumPoints);
		void DrawPolyLineNoEndPoint(const TArray<TPoint>& aPointList);
		void DrawPolyLineNoEndPoint(const CArrayFix<TPoint>* aPointList);
		void DrawPolyLineNoEndPoint(const TPoint *aPointList, TInt aNumPoints);
		void DrawPolygon(const CArrayFix<TPoint>* aPointList, TFillRule aFillRule=EAlternate);
		void DrawPolygon(const TArray<TPoint>& aPointList, TFillRule aFillRule=EAlternate);
		void DrawPolygon(const TPoint *aPointList, TInt aNumPoints, TFillRule aFillRule=EAlternate);
		void DrawEllipse(const TRect& aRect);
		void DrawLine(const TPoint& aStart, const TPoint& aEnd);
		void DrawLineTo(const TPoint& aPoint);
		void DrawLineBy(const TPoint& aVector);
		void DrawRect(const TRect& aRect);

	    void DrawText(const TDesC& aText,const TTextParameters* aParam);
	    void DrawText(const TDesC& aText,const TTextParameters* aParam,const TPoint& aPosition);
	    void DrawText(const TDesC& aText,const TTextParameters* aParam,const TRect& aClipRect);
        void DrawText(const TDesC& aText,const TTextParameters* aParam,const TRect& aClipFillRect,TInt aBaselineOffset,TTextAlign aHrz=ELeft,TInt aMargin=0);
	    void DrawTextVertical(const TDesC& aText,const TTextParameters* aParam,TBool aUp);
        void DrawTextVertical(const TDesC& aText,const TTextParameters* aParam,const TPoint& aPosition,TBool aUp);
        void DrawTextVertical(const TDesC& aText,const TTextParameters* aParam,const TRect& aClipRect,TBool aUp);
        void DrawTextVertical(const TDesC& aText,const TTextParameters* aParam,const TRect& aClipRect,TInt aBaselineOffset,TBool aUp,TTextAlign aVert=ELeft,TInt aMargin=0);
        void DrawTextVertical(const TDesC& aText,const TTextParameters* aParam,const TRect& aClipRect,TInt aBaselineOffset,TInt aTextWidth,TBool aUp,TTextAlign aVert=ELeft,TInt aMargin=0);


		void MoveTo(const TPoint& aPoint);
		void MoveBy(const TPoint& aVector);
		void Plot(const TPoint& aPoint);
		void Reset();
		void SetBrushColor(const TRgb& aColor);
		void SetBrushOrigin(const TPoint& aOrigin);
		void SetBrushStyle(TBrushStyle aBrushStyle);
		void SetClippingRegion(const TRegion& aRegion);
		void SetDrawMode(TDrawMode aDrawMode);
		void SetOrigin(const TPoint& aPoint = TPoint(0, 0));
		void SetPenColor(const TRgb& aColor);
		void SetPenStyle(TPenStyle aPenStyle);
		void SetPenSize(const TSize& aSize);
		void SetTextShadowColor(const TRgb& aColor);
		void SetCharJustification(TInt aExcessWidth, TInt aNumChars);
		void SetWordJustification(TInt aExcessWidth, TInt aNumGaps);
		void SetUnderlineStyle(TFontUnderline aUnderlineStyle);
		void SetStrikethroughStyle(TFontStrikethrough aStrikethroughStyle);
		void SetBrushPattern(const CFbsBitmap& aBitmap);
		void SetBrushPattern(TInt aFbsBitmapHandle);
		void SetFont(const CFont* aFont);
		void CopyRect(const TPoint& aOffset, const TRect& aRect);

		void UpdateJustification(const TDesC& aText,const TTextParameters* aParam);
		void UpdateJustificationVertical(const TDesC& aText,const TTextParameters* aParam,TBool aUp);

		void SetFontNoDuplicate(const CFont* aFont);
		TBool HasBrushPattern() const;
		TBool HasFont() const;
		void InternalizeL(RReadStream& aReadStream);
		void ExternalizeL(RWriteStream& aWriteStream);
		TRgb BrushColor() const;
		TRgb PenColor() const;
		TRgb TextShadowColor() const;
		void CopySettings(const MWsGraphicsContext& aGc);
		void SetFaded(TBool aFaded);
		void SetFadingParameters(TUint8 aBlackMap, TUint8 aWhiteMap);
		void FadeArea(const TRegion *aRegion);
		void MapColors(const TRect &aRect, const TRgb *aColors, TInt aNumPairs=2, TBool aMapForwards=ETrue);
		void SetUserDisplayMode(TDisplayMode aDisplayMode);
		TInt UseFont(TInt aFontHandle);

        /**
         * GetError
         * 
         * Return current error status of MWsGraphicsContext
         *
         * @return 
         */
		TInt GetError()
		    {
		    return KErrNone; // @todo
		    }

		/**
         * Origin
         * 
         * The origin of the GC relative to the screen's origin
         *
         * @return Origin
         */
		TPoint Origin() const;

        /**
         * ClippingRegion
         * 
         * The clipping region currently being used
         *
         * @return Clipping region
         */
		const TRegion& ClippingRegion()
	       {
	       return iClippingRegion;
	       }
		
		/**
		 * Push
		 * 
		 * Saves the state of the GC to an internal buffer.  Several GC states can be saved in a FILO.
		 * Do not restore a GC (using Pop()) that wasn't properly saved!
		 *
		 * @return KErrNone if successful, else one of the system-wide error codes
		 */
		TInt Push()
		    {
		    return KErrNotSupported; // @todo
		    }
		
		/**
		 * Pop
		 * 
		 * Restores the last GC state that was saved.
		 */
		void Pop()
		    {
		    // @todo
		    }

    private: // New methods
        
        /**
         * C++ constructor
         *
         * @param aAlfSendBuffer
         */
        CAlfGraphicsContext( CAlfRsSendBuffer& aAlfSendBuffer );

        /**
         * ConstructL
         *
         * Construct
         */
        void ConstructL( );

        /**
         * DoReset
         * 
         * Reset all context variables to their default values
         */
        void DoReset();
        
	private:

        /**
         * locally cached context variables.
         */
	    TPoint iOrigin;
	
        /**
         * clipping regions are in screen coordinates
         */
	    RRegionBuf<KAlfRsGcMaxClippingRects> iClippingRegion;
	    
	    TRect  iClippingRect;
	    
	    TInt iFontHandle;
	    	    
	    TRgb iTextShadowColor;
	    
	    TBool iBrushPatternSet;
	    
	    TInt iBrushPattern;
	    
	    TRgb iPenColor;
	    
	    TRgb iBrushColor;
	    
	    TSize iPenSize;
	    
	    TInt iDrawMode;
	    
	    TPoint iLinePosition;
	    
	    TBool iBrushUsed;
	    
	    TPoint iBrushOrigin;
        
	    TInt iBrushStyle;
        
	    TInt iCharJustExcess;
        
	    TInt iCharJustNum;
        
	    TInt iWordJustExcess;
        
	    TInt iWordJustNum;
        
	    TPoint iDitherOrigin;
        
	    TInt iDotLength;
        
	    TInt iDotMask;
        
	    TInt iDotParam;
        
	    TInt iDotDirection;
        
	    TInt iShadowMode;
        
	    TInt iStrikethrough;
        
	    TInt iUnderline;
        
	    TInt iUserDisplayMode;
        
	    TInt iPenStyle;
        
	    /**
	     * Not owned.
	     */
		CAlfRsSendBuffer& iSendBuffer;

		/*
		 * Increased whenever command that requires actual drawing is called.
		 */
		TInt  iDrawCommandCount;
		
#ifdef _OPTIMIZE_WS_COMMANDS_ADVANCED_
		
		RRegionBuf<5> iDelayedClippingRegion;
		RRegionBuf<5> iDelayedDrawRegion;

		enum TRollbackStates
		    {
		    ERollbackClippingState = 1 
		    //ERollbackFontState = 2
		    };

		RRegionBuf<20> iPreviousClippingRegion;

#endif
		
#ifdef _OPTIMIZE_WS_COMMANDS_BASIC_		

		TInt iRollback;

		enum TDelayedContextState
            {
            EModifiedPenStyle = 1,
            EModifiedBrushStyle = 2,
            EModifiedPenColor = 4,
            EModifiedBrushColor = 8,
            
            EModifiedClipRect = 16,
            EModifiedClipReset = 32,
            
            EModifiedJustification = 64,
            EModifiedBrushPattern1 = 128,
            EModifiedBrushPattern2 = 256,
            
            EModifiedSetFontNoDuplicate = 512,
            EModifiedResetFont = 1024
            };
		
		TInt iPreviousPenStyle;
		TInt iPreviousBrushStyle;
		TRgb iPreviousPenColor;
		TRgb iPreviousBrushColor;
		TInt iPreviousBrushPattern;
		TInt iPreviousFontHandle;
	    
        TInt32 iDelayedContextState;

#endif

		};
#endif /*CALFRSGC_H_*/
