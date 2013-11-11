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


#ifndef _INC_CSVGHKERNELEMENTIMPL_
#define _INC_CSVGHKERNELEMENTIMPL_

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
class CSvgHkernElementImpl : public CSvgElementImpl
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
        static CSvgHkernElementImpl*NewL(  const TUint8 aElemID,
                                      CSvgDocumentImpl* aDoc );

		/**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        static CSvgHkernElementImpl*NewLC(  const TUint8 aElemID,
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
        virtual                     ~CSvgHkernElementImpl();

        // SVG Implementation:


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TFloatFixPt                      GetK();

		/**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
		TInt  SetAttributeDesL( const TInt aNameId, const TDesC& aValue );

		TInt                        SetAttributeFloatL( const TInt aNameId,
                                                        const TFloatFixPt aValue );


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
        TInt                        GetAttributeDes( const TInt aNameId,
                                                     TPtrC16& aValue );
        
        void Print( TBool aIsEncodeOn );
        

    private:

        TFloatFixPt                      iK;

        HBufC*                      iG1;

        HBufC*                      iG2;

        HBufC*                      iU1;

        HBufC*                      iU2;




        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
                                    CSvgHkernElementImpl( CSvgDocumentImpl* aDoc );
    };

#endif /* SVG_FONTS_INCLUDE */

#endif
