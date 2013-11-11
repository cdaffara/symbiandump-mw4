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


#ifndef _INC_CSVGTEXTELEMENTIMPL_
#define _INC_CSVGTEXTELEMENTIMPL_

#include "SVGElementImpl.h"
#include "SVGLangSpaceImpl.h"
#include "SVGFourPointRect.h"
#include "SVGListener.h"
#include "GfxPoint2D.h"




class CSvgErrorImpl;

class   CSvgDocumentImpl;
class   MXmlDocument;

enum TTextAnchor
{
    EStartAnchor = 0,
    EMiddleAnchor,
    EEndAnchor
};

/**
 * Class description goes here.
 *
 *  @lib SVGEngine.lib
 *  @since 1.0
 */
class CSvgTextElementImpl : public CSvgElementImpl,
							public MSvgMouseListener
    {
    public:

        // Constructor/deconstructor



        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        static CSvgTextElementImpl*NewL(  const TUint8 aElemID,
                                      CSvgDocumentImpl* aDoc );

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        static CSvgTextElementImpl*NewLC(  const TUint8 aElemID,
                                       CSvgDocumentImpl* aDoc );

    private:
        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                            ConstructL(  const TUint8 aElemID  );



    public:

        void UpdateCurrentScaledFont();

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
                                    ~CSvgTextElementImpl();
        // From SVG DOM

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TInt32                      NumberOfChars();

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                        GetX( CArrayFix<TFloatFixPt>* aX );

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                        GetY( CArrayFix<TFloatFixPt>* aY );

        // SVG Implementation

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TBool                       IsNumberAttributeValid( const TDesC& aValue );


        void                        SetTextL( const TDesC& aText ); // This is temporary API until CDATA is implemented

        TDesC&                      GetText();

		TBool			 			LoadExternalSVGFontL( const TDesC& aText );
		
        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                        SetX( CArrayFix<TFloatFixPt>* aX );

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                        SetY( CArrayFix<TFloatFixPt>* aX );


        void                        SetXYArray(TUint16 aAtrId, CArrayFixFlat<TFloatFixPt>*& aX );



        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                        SetTextDecoration( const TDesC& aValue );

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                        SetTextAnchor( const TDesC& aValue );


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                        SetFontFamilyL( const TDesC& aValue );

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                        SetRotateL( const TDesC& aValue );

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */



        /**
         * Using generic function instead of SetArrayXL or SetArrrayYL
         *
         * @since 1.0
         * aValue - the string containing the coordinates
         * aIsX - Flag to determine whether it is X or Y coordinate values
         */

        void                        SetArrayL( const TDesC& aValue , const TBool aIsX);



        // From MXmlElement API


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TInt                        SetAttributeL( const TDesC& aName,
                                                   const TDesC& aValue );
        // From MXmlElementOpt API


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TInt                        GetAttributeFloat( const TInt aNameId,
                                                       TFloatFixPt& aValue );

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TInt                        SetAttributeFloatL( const TInt aNameId,
                                                        const TFloatFixPt aValue );


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TInt                        GetAttributeDes( const TInt aNameId,
                                                     TPtrC16& aValue );

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TInt                        SetAttributeDesL( const TInt aNameId,
                                                      const TDesC& aValue );

        // From CSvgElementImpl


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        MXmlElement*                CloneL(MXmlElement* aParentElement);


		void					CopyL( CSvgTextElementImpl* aDestElement );


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TBool                       DrawL( CGfx2dGc* aGc,
                                           CSvgElementImpl* aElement );

		
		
        /**
         * Scale the stroke width in order to nullify the effect of scaling
         * appiled on the glyph
         *
         * @since 3.2
         * @param aGc- Graphics context
         * @return none
         */
		void ScaleStrokeWidth(CGfx2dGc* aGc);
		
		
//		void	GetCurrentFontScaled(CFont*& aFont, TFontSpec& aFontSpec);
				
        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                        GetBBox( TGfxRectangle2D& aBbox );
		void						GetFourPointBBox( TSvgFourPointRect& aFourPointBBox );
		void 						GetBBoxForSVGText( TGfxRectangle2D& aBbox );
		void 						GetBBoxForSVGText( TSvgFourPointRect& aFourPointBbox,const TGfxAffineTransform& aTransform);	
		void						GetBBoxForSystemText( TGfxRectangle2D& aBbox );
        void 						GetBBoxForSystemText( TSvgFourPointRect& aFourPointBbox );
        void                        CreateSVGFontL();
        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual void                GetUnscaledBBox( TGfxRectangle2D& aBbox );

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        //Local Function(s)

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TBool                       IsWithinUnicodeRange( const TDesC& aUnicodeRange,
                                                          const TDesC& aCharacter );

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                        SetUKernPairsL( const TDesC& aU1,
                                                   const TDesC& aU2 );


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                        SetGKernPairsL( const TDesC& aG1,
                                                   const TDesC& aG2 );


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TBool                       IsKerningRequired( const TDesC& aPrevGlyphName,
                                                       const TDesC& aCurrGlyphName,
                                                       const TDesC& aPrevUnicode,
                                                       const TDesC& aCurrUnicode);

        // function for decoding
        void                      SetRotateArray(CArrayFixFlat<TReal32>*& aRotate);
        void                      FreeFontData();

        /**
         * Return the font size for this text element.
         *
         * @since 1.0
         * @param
         * @return
         */
        TFloatFixPt  FontSize();

        TFloatFixPt TextAdvance( const TDesC& aText, TInt aIndex = 0);
        TFloatFixPt Ascent();
        TFloatFixPt Descent();
        TTextAnchor TextAnchor();
		void  CacheGlyphsForText();
		TBool IsMostBasicLatin();
		TBool HasFont();
		void SetFont(CFont* aBitmapFont = NULL, CSvgElementImpl* aSVGFont = NULL);
	
		void SetEditable( const TDesC& aValue );
		
		void Print( TBool aIsEncodeOn );
		const CFont* Font() const;
		
        /**
         * Return wether an svg element is displayable, such as <rect>, <circle>, or
         * an abstract element, such as <g>, <animate>.
         *
         * @since 1.0
         * @param None
         * @return ETrue if element is viewable.
         */
        TBool               IsViewable() { return ETrue; }

		TBool 				IsEditable();

        /**
         * Notified when the mouse pointer enters a visible svg element.
         *
         * @since 1.0
         * param : aElements -- SVG elements containing the mouse point.
         * param : aX -- x coordinate of mouse pointer.
         * param : aY -- y coordinate of mouse pointer.
         * @return : For future use.  Value is ignored.
         */
        TBool MouseEntered( RPointerArray<CSvgElementImpl>& aElements,
                                    TInt aX, TInt aY );

        /**
         * Notified when the mouse pointer exits a visible svg element.
         *
         * @since 1.0
         * param : aElements -- SVG elements containing the mouse point.
         * param : aX -- x coordinate of mouse pointer.
         * param : aY -- y coordinate of mouse pointer.
         * @return : For future use.  Value is ignored.
         */
        TBool MouseExited( RPointerArray<CSvgElementImpl>& aElements,
                                   TInt aX, TInt aY );

        /**
         * Notified when the mouse pointer exits a visible svg element.
         *
         * @since 1.0
         * param : aElements -- SVG elements containing the mouse point.
         * param : aX -- x coordinate of mouse pointer.
         * param : aY -- y coordinate of mouse pointer.
         * @return : For future use.  Value is ignored.
         */
        TBool MouseMoved( RPointerArray<CSvgElementImpl>& aElements,
                          TInt aX, TInt aY );


        /**
         * Notified when the mouse pointer is pressed down on visible svg element.
         *
         * @since 1.0
         * param : aElements -- SVG elements containing the mouse point.
         * param : aX -- x coordinate of mouse pointer.
         * param : aY -- y coordinate of mouse pointer.
         * @return : For future use.  Value is ignored.
         */
        TBool MousePressed( RPointerArray<CSvgElementImpl>& aElements,
                                    TInt aX, TInt aY );

        /**
         * Notified when the mouse pointer is released on on visible svg element.
         *
         * @since 1.0
         * param : aElements -- SVG elements containing the mouse point.
         * param : aX -- x coordinate of mouse pointer.
         * param : aY -- y coordinate of mouse pointer.
         * @return : For future use.  Value is ignored.
         */
        TBool MouseReleased( RPointerArray<CSvgElementImpl>& aElements,
                                    TInt aX, TInt aY );
        
        inline TInt FileOffset() const
            {
            return iOffset;
            }
            
        inline void SetFileOffset(TInt aOffset)
            {
            if(! FileOffset())
            iOffset = aOffset;
            };
            
        inline void SetOrgLength(TInt aOrgLength)
            {
            if(!GetOrgLength())
            iOrgLength = aOrgLength;
            };
            
        inline TInt GetOrgLength()            
            {
            return iOrgLength;
            }
        TFloatFixPt GetTotalTextAdvance(const TFloatFixPt& alK,const TFloatFixPt& alUnitsPerEmInverse,const TFloatFixPt& alFontSize,const TFloatFixPt& alMissingGlyphHorzAdvX,const TFloatFixPt& alFontHorzAdvX);
            
        CFont*                      iBitmapFont;
        TFontSpec					iBitmapFontSpec; //New
                                    
    private:

    void GetScaledFont( TFloatFixPt aHeight,
                                          const TDesC& aTypefaceName,
                                          CFont*& aFont,
                                          TFontSpec& aFontSpec );

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
                                    CSvgTextElementImpl( CSvgDocumentImpl* aDoc );
         
         TBool 						FetchSvgFontL(const TDesC& aUri, CSvgDocumentImpl* aNewFontDoc);
		 //TBool 						LoadDefaultSvgFont(CSvgDocumentImpl* aNewFontDoc);
									

private:
        // Text element specific instance variables
        HBufC*                      iText;
        TFloatFixPt						iScale;
        TGfxPoint2D                 iPoint;
        TReal                       iLetterSpacing;
        TReal                       iWordSpacing;
        TReal                       iRotate;
        CDesCArrayFlat*             iFamilies;
        CArrayFixFlat<TFloatFixPt>*      iArrayX;
        CArrayFixFlat<TFloatFixPt>*      iArrayY;
        CArrayFixFlat<TReal32>*     iArrayRotate;
        TInt8                       iTextAnchor;
        TInt8                       iTextDecoration;
		
		RPointerArray<CSvgElementImpl> iGlyphElements; 
		
        HBufC*                      iG1;
        HBufC*                      iG2;

        HBufC*                      iU1;
        HBufC*                      iU2;
        
        TBool						iNeedToCacheGlyphs;
	    TBool						iUseDefaultSVGFont;
	    TBool						iTriedLoadingSVGFonts;
  
        CWsScreenDevice*            iWsScreenDevice; //New
        CSvgElementImpl*			iSVGFont;
		
		CSvgElementImpl*			iFontFaceElement;
		CSvgElementImpl*			iMissingGlyphElement;
		CSvgElementImpl*			iHkernElement;
		
		TGfxRectangle2D 			iSVGBbox;
		TBool						iEditable;

public:	
		TInt                        iOffset;
		TInt                        iOrgLength;	
		TSize						iBoundingBox;	
		CFont*						iFont;
		
		CGfx2dGc*					iGfx2dGc;
    };

#endif /* _INC_CSVGTEXTELEMENT_3C61A6080051_INCLUDED */
