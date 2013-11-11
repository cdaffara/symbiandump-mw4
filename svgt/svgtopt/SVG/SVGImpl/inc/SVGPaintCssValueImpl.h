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


#ifndef __INC_CPAINTCSSVALUEIMPL__
#define __INC_CPAINTCSSVALUEIMPL__


#include "SVGCssValue.h"
#include "SVGElementImpl.h"

#include "SVGMemoryManager.h"

class MGfxPaint;


/**
 * Class description goes here.
 *
 *  @lib SVGEngine.lib
 *  @since 1.0
 */

class CPaintCssValueImpl : public CCssValue

    {
    public:

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
                           ~CPaintCssValueImpl();


                            CPaintCssValueImpl( );





        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TInt                   SetValueL( const TDesC& aValue );

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TInt                   SetValueL( const TInt& aValue );

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        MGfxPaint*                  Value();

        void                  GetColorFromSolidColorElement(CSvgElementImpl* aReferencedElement);

		//void						SetL( CPaintCssValueImpl* aValue );
    /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */

        TInt                          CloneRGBValueL(const TInt& aValue);


        void                    SetUrlflag();
				TBool										GetUrlflag();
				
        void                    SetUrlIdL(const TDesC& aValue);
				HBufC*                  GetUrlId();

		
		TBool						IsEqual(CCssValue* aValue);
		TBool					IsDefaultSet();
		
		void 					SetElement(CSvgElementImpl* aElement);
		CSvgElementImpl*		GetElement();
		
		void						Print();
		
		//this data should all be private
		HBufC* iUrlId;
		MGfxPaint*                  iValue;
		
		//For Gradient Support
        TBool iUrlFlag;
        TInt32 iValueType;   //0 for TGfxColor
                           //1 for TLinearGradientPaint
                           //2 for TRadialGradientPaint
        

        CSvgElementImpl* iSvgElementImpl;
        TBool iDefaultSet;
        
    private: // Data
        // Indicates whether the iValue field is owned by CSvgPaintCssValueImpl
        TBool iIsValueOwned;


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        //void                        ConstructL( const TDesC& aValue, CSvgElementImpl* aSvgElementImpl );


        //void                        ConstructL( CSvgElementImpl* aSvgElementImpl );

    };

#endif /* __INC_CPAINTCSSVALUEIMPL__ */