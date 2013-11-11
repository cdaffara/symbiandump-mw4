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


#ifndef __INC_CSTRCSSVALUEIMPL__
#define __INC_CSTRCSSVALUEIMPL__


#include "SVGCssValue.h"



/**
 * Class description goes here.
 *
 *  @lib SVGEngine.lib
 *  @since 1.0
 */


class CStrCssValueImpl : public CCssValue

    {
    public:

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
                                    ~CStrCssValueImpl();




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
         const TDesC&                Value();
    /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
		void 					SetL(CStrCssValueImpl* aValue);

    /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
                                    CStrCssValueImpl();
		
		
		TBool						IsEqual(CCssValue* aValue);
	
		void						Print();
			
    private:





            HBufC*                  iCssText;


    };

#endif /* __INC_CSTRCSSVALUEIMPL__ */
