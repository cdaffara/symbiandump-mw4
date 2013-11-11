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
* Description:  SVG Implementation source file
 *
*/


#ifndef CSVGRADIALGRADIENTELEMENTIMPL_H
#define CSVGRADIALGRADIENTELEMENTIMPL_H

#include "SVGGradientElementImpl.h"

/**
 * Class description goes here.
 *
 *  @lib SVGEngine.lib
 *  @since 1.0
 */
class CSvgRadialGradientElementImpl : public CSvgGradientElementImpl
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
        static CSvgRadialGradientElementImpl* NewL( const TUint8 aElemID,
                                          CSvgDocumentImpl* aDoc );

        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        MGfxPaint *PaintAttribute();

        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        static CSvgRadialGradientElementImpl* NewLC( const TUint8 aElemID,
                                           CSvgDocumentImpl* aDoc );


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        virtual                     ~CSvgRadialGradientElementImpl();

        // From SVG DOM

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
        TInt SetAttributeL( const TDesC& aName, const TDesC& aValue );

		void Print( TBool aIsEncodeOn );
		
    private:
        TGfxRadialGradientPaint iGfxRadialPaint;
       
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
                                    CSvgRadialGradientElementImpl( CSvgDocumentImpl* aDoc );
    };

#endif /* CSVGRADIALGRADIENTELEMENTIMPL_H */
