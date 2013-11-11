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


#ifndef _INC_CSVGFONTELEMENTIMPL_
#define _INC_CSVGFONTELEMENTIMPL_

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
class CSvgFontElementImpl : public CSvgElementImpl
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
        static CSvgFontElementImpl*NewL(  const TUint8 aElemID,
                                      CSvgDocumentImpl* aDoc );

		/**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        static CSvgFontElementImpl*NewLC(  const TUint8 aElemID,
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
        virtual                     ~CSvgFontElementImpl();

        // From SVG DOM:


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TFloatFixPt                      GetHorzOrgX();


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TFloatFixPt                      GetHorzOrgY();


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TFloatFixPt                      GetHorzAdvX();

        // SVG Implementation:

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
        TInt                        SetAttributeL( const TDesC& aName,
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
        TBool                       DrawL( CGfx2dGc* aGc,
                                           CSvgElementImpl* aElement );
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
                                                      
        void						Print( TBool aIsEncodeOn );
        
    private:

        TFloatFixPt                      iHorzOrgX;
        TFloatFixPt                      iHorzOrgY;
        TFloatFixPt                      iHorzAdvX;



        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
                                    CSvgFontElementImpl( CSvgDocumentImpl* aDoc );
    };

#endif /* SVG_FONTS_INCLUDE */

#endif
