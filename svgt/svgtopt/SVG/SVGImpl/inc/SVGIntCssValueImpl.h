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


#ifndef __INC_CINTCSSVALUEIMPL__
#define __INC_CINTCSSVALUEIMPL__


#include "SVGCssValue.h"






/**
 * Class description goes here.
 *
 *  @lib SVGEngine.lib
 *  @since 1.0
 */

class CIntCssValueImpl : public CCssValue

    {
    public:

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
                                    ~CIntCssValueImpl();

          CIntCssValueImpl();


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
        TInt                        SetValueL( const TInt& aValue );

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
         TInt                      Value();





		
		TBool						IsEqual(CCssValue* aValue);

		void						Print();
		
    private:
        TInt32                      iValue;



    };

#endif /* __INC_CINTCSSVALUEIMPL__ */
