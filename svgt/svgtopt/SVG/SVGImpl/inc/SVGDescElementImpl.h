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


#ifndef _INC_CSVGDESCELEMENTIMPL_
#define _INC_CSVGDESCELEMENTIMPL_

#include "SVGElementImpl.h"
#include "GfxFloatFixPt.h"

class   CSvgDocumentImpl;


/**
 * Class description goes here.
 *
 *  @lib SVGEngine.lib
 *  @since 1.0
 */
class CSvgDescElementImpl : public CSvgElementImpl
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
        static CSvgDescElementImpl* NewL( const TUint8 aElemID,
                                          CSvgDocumentImpl* aDoc );


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        static CSvgDescElementImpl* NewLC( const TUint8 aElemID,
                                           CSvgDocumentImpl* aDoc );


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual                     ~CSvgDescElementImpl();

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
        MXmlElement*        CloneL(MXmlElement* aParentElement);

				void 								CopyL( CSvgDescElementImpl* aDestElement );
				
        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TBool                       DrawL( CGfx2dGc* aGc,
                                           CSvgElementImpl* aElement );
                                           
        void 														SetTextL( const TDesC& aText );
        
        void												Print( TBool aIsEncodeOn );
        
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
                                    CSvgDescElementImpl( CSvgDocumentImpl* aDoc );
                                    
        
        
        HBufC*                      iText;
    };

#endif
