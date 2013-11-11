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


#ifndef _INC_CSVGSTYLEELEMENTIMPL_
#define _INC_CSVGSTYLEELEMENTIMPL_

#include "SVGElementImpl.h"
#include "GfxFloatFixPt.h"



class   CSvgDocumentImpl;


/**
 * Class description goes here.
 *
 *  @lib SVGEngine.lib
 *  @since 1.0
 */
class CSvgStyleElementImpl : public CSvgElementImpl
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
        static CSvgStyleElementImpl*NewL(  const TUint8 aElemID,
                                      CSvgDocumentImpl* aDoc );

		/**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        static CSvgStyleElementImpl*NewLC(  const TUint8 aElemID,
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
        virtual                     ~CSvgStyleElementImpl();

        // SVG Implementation:

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                        SetTypeL( const TDesC& aValue );


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                        SetMediaL( const TDesC& aValue );


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                        SetTitleL( const TDesC& aValue );


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                        SetStyleL( const TDesC& aValue );

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

		TInt                       SetAttributeDesL( const TInt aNameId,
			                               const TDesC& aValue );


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

        HBufC*                      iType;

        HBufC*                      iMedia;

        HBufC*                      iTitle;

        HBufC*                      iStyle;




        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
                                    CSvgStyleElementImpl( CSvgDocumentImpl* aDoc );
    };

#endif
