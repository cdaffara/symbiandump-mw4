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


#ifndef __INC_CSVGPATHELEMENTIMPL__
#define __INC_CSVGPATHELEMENTIMPL__

#include "SVGElementImpl.h"
#include "GfxGeneralPath.h"

class   CSvgDocumentImpl;



/**
 * Class description goes here.
 *
 *  @lib SVGEngine.lib
 *  @since 1.0
 */
class CSvgPathElementImpl : public CSvgElementImpl
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
        static CSvgPathElementImpl*NewL(  const TUint8 aElemID,
                                      CSvgDocumentImpl* aDoc );

		/**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        static CSvgPathElementImpl*NewLC(  const TUint8 aElemID,
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
                                    ~CSvgPathElementImpl();

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
		CGfxGeneralPath* GetPathAttribute(TInt aAttributeId);
		void SetPathAttribute(TInt aAttributeId, CGfxGeneralPath* aPathHandle);

        /**
         * Return wether an svg element is displayable, such as <rect>, <circle>, or
         * an abstract element, such as <g>, <animate>.
         *
         * @since 1.0
         * @param None
         * @return ETrue if element is viewable.
         */
        TBool               IsViewable() { return ETrue; }

		CGfxGeneralPath*            iShape;

    private:

        



        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
                                    CSvgPathElementImpl( CSvgDocumentImpl* aDoc );

    public:

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        MXmlElement*                CloneL(MXmlElement* aParentElement);

		void						Print( TBool aIsEncodeOn );
		
    protected:

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                        CopyL( CSvgPathElementImpl* aDestElement );

    };

#endif /* _INC_CSVGPATHELEMENT_3C61A657011D_INCLUDED */
