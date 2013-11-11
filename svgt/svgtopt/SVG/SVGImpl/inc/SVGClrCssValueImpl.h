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


#ifndef __INC_CCLRCSSVALUEIMPL__
#define __INC_CCLRCSSVALUEIMPL__


#include "SVGCssValue.h"

#include "SVGColor.h"



/**
 * Class description goes here.
 *
 *  @lib SVGEngine.lib
 *  @since 1.0
 */

class CClrCssValueImpl : public CCssValue

    {
    public:

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
                                    ~CClrCssValueImpl();


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
                      CClrCssValueImpl( );

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TInt                        SetValueL( const TDesC &aValue );
        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TInt                        SetValueL( const TInt &aValue );

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TSvgColor*                   Value();

		void 						SetL(CClrCssValueImpl* aValueToMimic);
		
         /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */

         TInt                       CloneRGBValueL(const TInt& aValue);



		 TBool						IsEqual(CCssValue* aValue);
		 
		 void						Print();
		 
    private:





        TSvgColor*                   iValue;
    };

#endif /* __INC_CCLRCSSVALUEIMPL__ */