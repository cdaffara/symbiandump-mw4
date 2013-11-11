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


#ifndef _INC_CSVGRECTELEMENTIMPL_
#define _INC_CSVGRECTELEMENTIMPL_

#include "SVGElementImpl.h"
#include "GfxFloatFixPt.h"
#include "GfxRoundRectangle2D.h"



class   CSvgDocumentImpl;
class   TSvgFourPointRect;

/**
 * Class description goes here.
 *
 *  @lib SVGEngine.lib
 *  @since 1.0
 */
class CSvgRectElementImpl : public CSvgElementImpl
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
        static CSvgRectElementImpl*NewL(  const TUint8 aElemID,
                                      CSvgDocumentImpl* aDoc );

		/**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        static CSvgRectElementImpl*NewLC(  const TUint8 aElemID,
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
        virtual                     ~CSvgRectElementImpl();

        // From SVG DOM


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TFloatFixPt                      X();


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TFloatFixPt                      Y();


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TFloatFixPt                      Width();


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TFloatFixPt                      Height();


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TFloatFixPt                      Rx();


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TFloatFixPt                      Ry();

        // SVG Implementation


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

		void						GetFourPointBBox( TSvgFourPointRect& aFourPointBBox);
        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual void                GetUnscaledBBox( TGfxRectangle2D& /*aBbox*/ );

         /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
       void                        SetRectValues(TFloatFixPt aX, TFloatFixPt aY, TFloatFixPt aWidth, TFloatFixPt aHeight);

        /**
         * Return wether an svg element is displayable, such as <rect>, <circle>, or
         * an abstract element, such as <g>, <animate>.
         *
         * @since 1.0
         * @param None
         * @return ETrue if element is viewable.
         */
        TBool               IsViewable() { return ETrue; }

    private:

        TGfxRoundRectangle2D        iRectangle;



        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
                                    CSvgRectElementImpl( CSvgDocumentImpl* aDoc );

    public:

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        MXmlElement*                    CloneL(MXmlElement* aParentElement);

		void						Print( TBool aIsEncodeOn );
		
    protected:

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                            CopyL( CSvgRectElementImpl* aDestElement );

    };

#endif /* _INC_CSVGLINEELEMENT_3C61A6080051_INCLUDED */
