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


#ifndef _INC_CSVGMETADATAELEMENTIMPL_
#define _INC_CSVGMETADATAELEMENTIMPL_

#include "SVGElementImpl.h"
#include "GfxFloatFixPt.h"

class   CSvgDocumentImpl;


/**
 * Class description goes here.
 *
 *  @lib SVGEngine.lib
 *  @since 1.0
 */
class CSvgMetadataElementImpl : public CSvgElementImpl
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
        static CSvgMetadataElementImpl* NewL( const TUint8 aElemID,
                                              CSvgDocumentImpl* aDoc );


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        static CSvgMetadataElementImpl* NewLC( const TUint8 aElemID,
                                               CSvgDocumentImpl* aDoc );


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual                         ~CSvgMetadataElementImpl();


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        CSvgElementImpl*                SvgElementImpl();

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

				TInt 												SetAttributeDesL( const TInt aNameId,
                                            const TDesC& aValue );
        
        TInt 												GetAttributeDes( const TInt aNameId, TPtrC16& aValue );
                                        
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
        MXmlElement*        CloneL(MXmlElement* aParentElement);
        
        void 								CopyL( CSvgMetadataElementImpl* aDestElement );
        
		void Print( TBool aIsEncodeOn );
		
				void 														SetTextL( const TDesC& aText );
				
    private:

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                            ConstructL( const TUint8 aElemID );


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
                                        CSvgMetadataElementImpl( CSvgDocumentImpl* aDoc );
        
        
        
        
        HBufC*                      iText;
        
    };

#endif
