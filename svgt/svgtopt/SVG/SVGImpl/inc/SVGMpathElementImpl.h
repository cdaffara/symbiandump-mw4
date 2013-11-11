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


#ifndef __INC_CSVGMPATHELEMENTIMPL__
#define __INC_CSVGMPATHELEMENTIMPL__

#include "SVGElementImpl.h"



class   CGfxGeneralMpath;
class   CSvgDocumentImpl;


/**
 * Class description goes here.
 *
 *  @lib SVGEngine.lib
 *  @since 1.0
 */
class CSvgMpathElementImpl : public CSvgElementImpl
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
        static CSvgMpathElementImpl*NewL(  const TUint8 aElemID,
                                      CSvgDocumentImpl* aDoc );

		/**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        static CSvgMpathElementImpl*NewLC(  const TUint8 aElemID,
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
                                    ~CSvgMpathElementImpl();

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
        // From MXmlElementOpt API


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

		void                        SetAttributePathByIdL(const TDesC& aId);

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

        CGfxGeneralPath*            iShape;
		HBufC*						iParsedHref;
		TBool						iNeedTarget;



        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
                                    CSvgMpathElementImpl( CSvgDocumentImpl* aDoc );

    public:

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        MXmlElement*                    CloneL(MXmlElement* aParentElement);

		void							Print( TBool aIsEncodeOn );
		
    protected:

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                            CopyL( CSvgMpathElementImpl* aDestElement );

    };

#endif /* _INC_CSVGMPATHELEMENT_ */
