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


#ifndef CSVGELLIPSEELEMENTIMPL_H
#define CSVGELLIPSEELEMENTIMPL_H

#include "SVGElementImpl.h"
#include "GfxEllipse2D.h"


class   CSvgDocumentImpl;


/**
 * Class description goes here.
 *
 *  @lib SVGEngine.lib
 *  @since 1.0
 */
class CSvgEllipseElementImpl : public CSvgElementImpl
    {
    public: // Constructor/deconstructor


		/**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        static CSvgEllipseElementImpl*NewL(  const TUint8 aElemID,
                                      CSvgDocumentImpl* aDoc );

		/**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        static CSvgEllipseElementImpl*NewLC(  const TUint8 aElemID,
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
        virtual                         ~CSvgEllipseElementImpl();

    public: // New functions
        // From SVG DOM


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TFloatFixPt                          Rx();


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TFloatFixPt                          Ry();


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TFloatFixPt                          Cx();


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TFloatFixPt                          Cy();

        // SVG Implementation


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                            SetRx( TFloatFixPt aRx );


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                            SetRy( TFloatFixPt aRy );


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                            SetCx( TFloatFixPt aCx );


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                            SetCy( TFloatFixPt aCy );

        // Utility methods

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        MXmlElement*                    CloneL(MXmlElement* aParentElement);

        /**
         * Return wether an svg element is displayable, such as <rect>, <circle>, or
         * an abstract element, such as <g>, <animate>.
         *
         * @since 1.0
         * @param None
         * @return ETrue if element is viewable.
         */
        TBool               IsViewable() { return ETrue; }

    public: // From base classes

        // From CSvgElementImpl


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TBool                           DrawL( CGfx2dGc* aGc,
                                               CSvgElementImpl* aElement );

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                            GetBBox( TGfxRectangle2D& aBbox );

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual void                    GetUnscaledBBox( TGfxRectangle2D& /*aBbox*/ );

        // From MXmlElement API


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TInt                            SetAttributeL( const TDesC& aName,
                                                       const TDesC& aValue );


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
        TInt                            SetAttributeFloatL( const TInt aNameId,
                                                            const TFloatFixPt aValue );
		
		void							Print( TBool aIsEncodeOn );
    
    protected: // New functions


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                            CopyL( CSvgEllipseElementImpl* aDestElement );

    protected: // From base classes

    private:

        /**
        * C++ default constructor.
        */
                                        CSvgEllipseElementImpl( CSvgDocumentImpl* aDoc );


    private: // Data

        TGfxEllipse2D                   iEllipse;

    };

#endif
