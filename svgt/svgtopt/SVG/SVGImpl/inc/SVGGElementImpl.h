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


#ifndef _INC_CSVGGELEMENTIMPL_
#define _INC_CSVGGELEMENTIMPL_

#include "SVGElementImpl.h"
#include "GfxFloatFixPt.h"




class   CSvgDocumentImpl;


/**
 * Class description goes here.
 *
 *  @lib SVGEngine.lib
 *  @since 1.0
 */
class CSvgGElementImpl : public CSvgElementImpl
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
        static CSvgGElementImpl*NewL(  const TUint8 aElemID,
                                      CSvgDocumentImpl* aDoc );

		/**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        static CSvgGElementImpl*NewLC(  const TUint8 aElemID,
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
        virtual                 ~CSvgGElementImpl();

        // From MXmlElement API


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TInt                    SetAttributeL( const TDesC& aName,
                                               const TDesC& aValue );

        // From CSvgElementImpl


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TBool                   DrawL( CGfx2dGc* aGc,
                                       CSvgElementImpl* aElement );


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void GetBBox( TGfxRectangle2D& aBbox );

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual void GetUnscaledBBox( TGfxRectangle2D& /*aBbox*/ );

        // From MXmlElementOpt API


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TInt                GetAttributeFloat( const TInt aNameId,
                                               TFloatFixPt& aValue );


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        // Utilities (not exported)

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        static void         GetGroupBounding( TGfxRectangle2D& aBbox,
                                              CSvgElementImpl* aStartElement );
        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        static void         GetGroupBoundingUnscaled( TGfxRectangle2D& aBbox,
                                                      CSvgElementImpl* aStartElement );
    private:

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        static void         GetGroupBoundingInternal( TFloatFixPt& aXmin,
                                                      TFloatFixPt& aYmin,
                                                      TFloatFixPt& aXmax,
                                                      TFloatFixPt& aYmax,
                                                      CSvgElementImpl* aStartElement );
        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        static void         GetGroupBoundingInternalUnscaled( TFloatFixPt& aXmin,
                                                              TFloatFixPt& aYmin,
                                                              TFloatFixPt& aXmax,
                                                      TFloatFixPt& aYmax,
                                                      CSvgElementImpl* aStartElement );
    private:




        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
                            CSvgGElementImpl( CSvgDocumentImpl* aDoc );

    public:


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        MXmlElement*        CloneL(MXmlElement* aParentElement);

		void				Print( TBool aIsEncodeOn );
		
    protected:


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                CopyL( CSvgGElementImpl* aDestElement );


    };

#endif
