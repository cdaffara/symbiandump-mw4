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


#ifndef  SVGLINEARGRADIENTELEMENTIMPL_H
#define SVGLINEARGRADIENTELEMENTIMPL_H

#include "SVGGradientElementImpl.h"

/**
 * Class description goes here.
 *
 *  @lib SVGEngine.lib
 *  @since 1.0
 */
class CSvgLinearGradientElementImpl : public CSvgGradientElementImpl
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
        static CSvgLinearGradientElementImpl* NewL( const TUint8 aElemID,
                                          CSvgDocumentImpl* aDoc );


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        static CSvgLinearGradientElementImpl* NewLC( const TUint8 aElemID,
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
        virtual                     ~CSvgLinearGradientElementImpl();

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
		TInt						SetAttributeIntL( const TInt aNameId,
                                        const TInt32 aValue );
		TInt						GetAttributeIntL( const TInt aNameId, TInt32& aValue );


        // From MXmlElement API
        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        TInt SetAttributeL( const TDesC& aName,
                                                   const TDesC& aValue );

		void Print( TBool aIsEncodeOn );
		
    private:
        TGfxLinearGradientPaint iGfxLinearPaint;
    
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
                                    CSvgLinearGradientElementImpl( CSvgDocumentImpl* aDoc );
		/*
         * To add support for % values for x1,x2,y1,y2 
         */
        TBool IsValidValue( const TDesC& aValue );

    };

#endif /* SVGLINEARGRADIENTELEMENTIMPL_H */
