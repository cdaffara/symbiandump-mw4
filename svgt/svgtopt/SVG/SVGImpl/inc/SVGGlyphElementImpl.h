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


#ifndef _INC_CSVGGLYPHELEMENTIMPL_
#define _INC_CSVGGLYPHELEMENTIMPL_

#include "SVGElementImpl.h"
#include "GfxFloatFixPt.h"

#include "GfxGeneralPath.h"




#ifdef SVG_FONTS_INCLUDE

class   CSvgDocumentImpl;


/**
 * Class description goes here.
 *
 *  @lib SVGEngine.lib
 *  @since 1.0
 */
class CSvgGlyphElementImpl : public CSvgElementImpl
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
        static CSvgGlyphElementImpl*NewL(  const TUint8 aElemID,
                                      CSvgDocumentImpl* aDoc );

		/**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        static CSvgGlyphElementImpl*NewLC(  const TUint8 aElemID,
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
        virtual                     ~CSvgGlyphElementImpl();

        // From SVG DOM

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                        GetUnicode( TDes& aUnicode );


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TFloatFixPt                      GetHorzAdvX();

        // SVG Implementation


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                        SetUnicodeL( const TDesC& aUnicode );


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                        SetGlyphNameL( const TDesC& aGlyphName );


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                        SetLangCodeL( const TDesC& aLangCode );


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                        SetPath( const TDesC& aPath );


		 /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TInt						SetAttributeFloatL(	const TInt aNameId,
														const TFloatFixPt aValue );

		/**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
		TInt						SetAttributeDesL( const TInt aNameId, const TDesC& aValue );


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
        TInt                        GetAttributePath( const TInt aNameId,
                                                      CGfxGeneralPath*& aValue );

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TInt                        SetAttributePathL( const TInt aNameId,
                                                       CGfxGeneralPath* aValue );

		TInt                        SetAttributePathRef( const TInt aNameId,
                                                       CGfxGeneralPath*& aValue );


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TInt                        GetAttributeDes( const TInt aNameId,
                                                     TPtrC16& aValue );

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
        // From CSvgElementImpl


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        MXmlElement*                CloneL(MXmlElement* aParentElement);


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
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                        GetBBox( TGfxRectangle2D& aBbox );

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual void GetUnscaledBBox( TGfxRectangle2D& /*aBbox*/ );

        CGfxGeneralPath* GetPathAttribute(TInt aAttributeId);
        void SetPathAttribute(TInt aAttributeId, CGfxGeneralPath* aPathHandle);

		void Print( TBool aIsEncodeOn );
		
    private:

        HBufC*                      iUnicode;

        HBufC*                      iGlyphName;

        TFloatFixPt                      iHorzAdvX;

        HBufC*                      iLangCode;

        //    TInt16 iPathLength;

        CGfxGeneralPath*            iShape;

		TBool	iOwnedPath;


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
                                    CSvgGlyphElementImpl( CSvgDocumentImpl* aDoc );
    };

#endif /* SVG_FONTS_INCLUDE */

#endif /* _INC_CSVGGLYPHELEMENTIMPL_ */
