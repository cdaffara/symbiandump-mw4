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


#ifndef _INC_CSVGUSEELEMENTIMPL_
#define _INC_CSVGUSEELEMENTIMPL_

//#include "SVGStructuralElementImpl.h"
#include "SVGElementImpl.h"
#include "GfxFloatFixPt.h"
#include "SVGSchemaData.h"


class   CSvgDocumentImpl;


/**
 * Class description goes here.
 *
 *  @lib SVGEngine.lib
 *  @since 1.0
 */
class CSvgUseElementImpl : public CSvgElementImpl
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
        static CSvgUseElementImpl*NewL(  const TUint8 aElemID,
                                      CSvgDocumentImpl* aDoc );

		/**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        static CSvgUseElementImpl*NewLC(  const TUint8 aElemID,
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
        virtual                     ~CSvgUseElementImpl();

        // From SVG DOM: Empty


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

        // SVG Implementation: Empty


        // From MXmlElement API
		/**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
		void						GetBBox( TGfxRectangle2D& aBbox );
		void                        GetBoundingBox(TGfxRectangle2D& aBbox);
		void						GetUnscaledBBox( TGfxRectangle2D& aBbox );
		
		TInt						SetAttributeFloatL( const TInt aNameId,
											          const TFloatFixPt aValue );
		TInt GetAttributeFloat( const TInt aNameId,
                                             TFloatFixPt& aValue );
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

		TInt 						SetAttributeDesL( const TInt aNameId,
                                        const TDesC& aValue );
                                        
        TInt 						GetAttributeDes( const TInt aNameId, TPtrC16& aValue );
                                        
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
		/**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
		*/	
		void CopyL( CSvgUseElementImpl* aDestElement );

		void Print( TBool aIsEncodeOn );
		
		/**
         * Return wether an svg element is displayable, such as <rect>, <circle>, or
         * an abstract element, such as <g>, <animate>.
         *
         * @since 1.0
         * @param None
         * @return ETrue if element is viewable.
         */
        TBool               IsViewable();
        
        CSvgElementImpl*    GetReferenceElementL();
    private:

        TFloatFixPt                      iX;
        //TBool                       iXSet;

        TFloatFixPt                      iY;
        //TBool                       iYSet;

        TFloatFixPt                      iWidth;

        TFloatFixPt                      iHeight;

        CSvgElementImpl*            ireferenceEl;


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
                                    CSvgUseElementImpl( CSvgDocumentImpl* aDoc );


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
//        void                        DeepClone( MXmlElement* );


	public:
        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                        SetReferenceElementL(  );

		TInt						SetRefElemById(const TDesC& aName);
		
		CSvgUseElementImpl*			iUseElementThatUsesThisUse;
		
		HBufC* iUseHRef;
    };

#endif
