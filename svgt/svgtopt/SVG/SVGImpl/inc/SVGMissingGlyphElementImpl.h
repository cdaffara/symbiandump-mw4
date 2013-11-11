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


#ifndef _INC_CSVGMISSINGGLYPHELEMENTIMPL_
#define _INC_CSVGMISSINGGLYPHELEMENTIMPL_

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
class CSvgMissingGlyphElementImpl : public CSvgElementImpl
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
        static CSvgMissingGlyphElementImpl*NewL(  const TUint8 aElemID,
                                      CSvgDocumentImpl* aDoc );

		/**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        static CSvgMissingGlyphElementImpl*NewLC(  const TUint8 aElemID,
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
        virtual                             ~CSvgMissingGlyphElementImpl();

        // From SVG DOM:


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TFloatFixPt                              GetHorzAdvX();

        // SVG Implementation:


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                                SetPath( const TDesC& aPath );



		/**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
         TInt								SetAttributeFloatL( const TInt aNameId, const TFloatFixPt aValue );

        // From MXmlElement API


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TInt                                SetAttributeL( const TDesC& aName,
                                                           const TDesC& aValue );

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
        TBool                               DrawL( CGfx2dGc* aGc,
                                                   CSvgElementImpl* aElement );
        // From MXmlElementOpt API

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TInt                                GetAttributeFloat( const TInt aNameId,
                                                               TFloatFixPt& aValue );


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TInt                                GetAttributePath( const TInt aNameId,
                                                              CGfxGeneralPath*& aValue );

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TInt                                SetAttributePathL( const TInt aNameId,
                                                               CGfxGeneralPath* aValue );

		TInt                                SetAttributePathRef( const TInt aNameId,
                                                       CGfxGeneralPath*& aValue );

		CGfxGeneralPath* GetPathAttribute(TInt aAttributeId);
		void SetPathAttribute(TInt aAttributeId, CGfxGeneralPath* aPathHandle);
		
		void Print( TBool aIsEncodeOn );
		
    private:

        TFloatFixPt                              iHorzAdvX;

        //    TInt16 iPathLength;

        CGfxGeneralPath*                    iShape;





        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
                                            CSvgMissingGlyphElementImpl( CSvgDocumentImpl* aDoc );
    };

#endif /* SVG_FONTS_INCLUDE */

#endif /* #ifndef _INC_CSVGMISSINGGLYPHElementIMPL_ */
