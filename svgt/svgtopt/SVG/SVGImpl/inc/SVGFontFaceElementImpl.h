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


#ifndef _INC_CSVGFONTFACEELEMENTIMPL_
#define _INC_CSVGFONTFACEELEMENTIMPL_

#include "SVGElementImpl.h"
#include "GfxFloatFixPt.h"





#ifdef SVG_FONTS_INCLUDE

class   CSvgDocumentImpl;


/**
 * Class description goes here.
 *
 *  @lib SVGEngine.lib
 *  @since 1.0
 */
class CSvgFontFaceElementImpl : public CSvgElementImpl
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
        static CSvgFontFaceElementImpl*NewL(  const TUint8 aElemID,
                                      CSvgDocumentImpl* aDoc );

		/**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        static CSvgFontFaceElementImpl*NewLC(  const TUint8 aElemID,
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

       /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */

		TInt							SetAttributeFloatL(		const TInt aNameId,
																const TFloatFixPt aValue );



        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual                         ~CSvgFontFaceElementImpl();

        // From SVG DOM: Empty


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        const TDesC*                          GetFontFamily(  );


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TFloatFixPt                          GetAscent();


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TFloatFixPt                          GetDescent();


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TFloatFixPt                          GetUnitsPerEm();


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TFloatFixPt                          GetAlphabetic();

        // SVG Implementation:


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                            SetFontFamilyL( const TDesC& aFontFamily );

		
		/**
         * Removes the font-family from hash map, called in case the 
         * font-face has font-face-src 
         *
         * @since 3.2 
         * @param: aFontFamily-- font-family name
         * @return none
         */
        
		void RemoveFontFamily(const TDesC& aFontFamily);
        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                            SetUnicodeRangeL( const TDesC& aUnicodeRange );


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                            SetAscent( TFloatFixPt aAscent );


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                            SetDescent( TFloatFixPt aDescent );


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                            SetUnitsPerEm( TFloatFixPt aUnitsPerEm );


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                            SetAlphabetic( TFloatFixPt aAlphabetic );


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                            SetUnderlinePosition( TFloatFixPt aUnderlinePosition );


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                            SetUnderlineThickness( TFloatFixPt aUnderlineThickness );


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                            SetOverlinePosition( TFloatFixPt aOverlinePosition );


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                            SetOverlineThickness( TFloatFixPt aOverlineThickness );


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                            SetStrikethroughPosition( TFloatFixPt aStrikethroughPosition );


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                            SetStrikethroughThickness( TFloatFixPt aStrikethroughThickness );

        // From MXmlElement API

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TBool                           IsAttributeValid( const TDesC& aValue );

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TInt                            SetAttributeL( const TDesC& aName,
                                                       const TDesC& aValue );
        // From CSvgElementImpl


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
        TBool                           DrawL( CGfx2dGc* aGc,
                                               CSvgElementImpl* aElement );

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
        TInt                            GetAttributeDes( const TInt aNameId,
                                                         TPtrC16& aValue );
		void Print( TBool aIsEncodeOn );
		
    private:

        TFloatFixPt                          iAscent;

        TFloatFixPt                          iDescent;

        TFloatFixPt                          iUnitsPerEm;

        TFloatFixPt                          iAlphabetic;

        TFloatFixPt                          iUnderlinePosition;

        TFloatFixPt                          iUnderlineThickness;

        TFloatFixPt                          iOverlinePosition;

        TFloatFixPt                          iOverlineThickness;

        TFloatFixPt                          iStrikethroughPosition;

        TFloatFixPt                          iStrikethroughThickness;

        HBufC*                          iFontFamily;

        HBufC*                          iUnicodeRange;





        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
                                        CSvgFontFaceElementImpl( CSvgDocumentImpl* aDoc );
    };

#endif /* SVG_FONTS_INCLUDE */

#endif
