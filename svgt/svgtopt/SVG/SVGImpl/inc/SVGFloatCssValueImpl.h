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



#ifndef __INC_CFLOATCSSVALUEIMPL__
#define __INC_CFLOATCSSVALUEIMPL__


#include "SVGCssValue.h"

#include "GfxFloatFixPt.h"



/**
 * Class description goes here.
 *
 *  @lib SVGEngine.lib
 *  @since 1.0
 */



class CFloatCssValueImpl : public CCssValue

    {
    public:

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
                                    ~CFloatCssValueImpl();

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TInt                        SetValueL( const TDesC& aValue );

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TInt                        SetValueL( const TFloatFixPt& aValue );

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TFloatFixPt                       Value();

       /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
                                    CFloatCssValueImpl();

       /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
                                    CFloatCssValueImpl( float aValue );
		
		
		TBool						IsEqual(CCssValue* aValue);
		void						Print();
		
    private:




        TFloatFixPt                      iValue;
    };

#endif /* __INC_CFLOATCSSVALUEIMPL__ */
