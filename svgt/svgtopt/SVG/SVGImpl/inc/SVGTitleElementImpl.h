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


#ifndef _INC_CSVGTITLEELEMENTIMPL_
#define _INC_CSVGTITLEELEMENTIMPL_

#include "SVGElementImpl.h"
#include "GfxFloatFixPt.h"

class   CSvgDocumentImpl;

/**
 * Class description goes here.
 *
 *  @lib SVGEngine.lib
 *  @since 1.0
 */
class CSvgTitleElementImpl : public CSvgElementImpl
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
        static CSvgTitleElementImpl*NewL( const TUint8 aElemID,
                                          CSvgDocumentImpl* aDoc );


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        static CSvgTitleElementImpl*NewLC( const TUint8 aElemID,
                                           CSvgDocumentImpl* aDoc );


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual                     ~CSvgTitleElementImpl();


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TInt                        SetAttributeL( const TDesC& aName,
                                                   const TDesC& aValue );

				TInt 												SetAttributeDesL( const TInt aNameId,
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

				void 								CopyL( CSvgTitleElementImpl* aDestElement );
				
        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TBool                       DrawL( CGfx2dGc* aGc,
                                           CSvgElementImpl* aElement );
                                           
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
        void                        ConstructL( const TUint8 aElemID );


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
                                    CSvgTitleElementImpl( CSvgDocumentImpl* aDoc );
                                    
        
        
        HBufC*                      iText;
    };

#endif
