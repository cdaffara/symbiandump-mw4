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


#ifndef __INC_CVECTORCSSVALUEIMPL__
#define __INC_CVECTORCSSVALUEIMPL__


#include "SVGCssValue.h"

#include "GfxFloatFixPt.h"


/**
 * Class description goes here.
 *
 *  @lib SVGEngine.lib
 *  @since 1.0
 */

class CVectorCssValueImpl : public CCssValue

    {
    public:

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
                                        ~CVectorCssValueImpl();


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TInt                            SetValueL( const TDesC& aValue );

        void                                  SetValueL( CArrayFix<TFloatFixPt>*& aValue );

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                            Value( CArrayFix<TFloatFixPt>*& );

		CArrayFix<TFloatFixPt>*      Value( );
		

		void  					CloneValueL( CArrayFix<TFloatFixPt>*& aValue );
                                CVectorCssValueImpl(CArrayFix<TFloatFixPt>* aValue );

		
		TBool						IsEqual(CCssValue* aValue);
	
		void	Print();
			

    /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
                                        CVectorCssValueImpl( );
    private:




        CArrayFix<TFloatFixPt>*              iValue;
    };

#endif /* __INC_CVECTORCSSVALUEIMPL__ */