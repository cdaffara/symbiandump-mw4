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


#ifndef SVGGRADIENTELEMENTIMPL_H
#define SVGGRADIENTELEMENTIMPL_H

#include "SVGElementImpl.h"
#include "SvgStopElementImpl.h"
#include "GfxGradientPaint.h"

//
class   CSvgDocumentImpl;

/**
 * Class description goes here.
 *
 *  @lib SVGEngine.lib
 *  @since 1.0
 */
class CSvgGradientElementImpl : public CSvgElementImpl
    {
    public:
        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        CSvgGradientElementImpl( CSvgDocumentImpl* aDoc );

        /**
         * The function not only adds the element in Stop element array but also
         * adjusts the offset values of all the previously added elements such that
         * offset value.
         *
         * @param : aStopEl -- Pointer to stop element.  
         * @return: none
         */
        void AddStopElementInArray(CSvgStopElementImpl *aStopEl);
        
        /**
         * Adjusts the offset values of all the previously added elements such that
         * each gradient offset value is greater than the previous gradient stop's
         * each gradient offset value is greater than the previous gradient stop's
         * offset value.
         *
         * @param : aStopEl -- Pointer to stop element.  
         * @return: none
         */
        void UpdateOffsetValues(CSvgStopElementImpl *aStopEl);

        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        TInt SetAttributeL( const TDesC& aName,const TDesC& aValue );
		TInt SetAttributeIntL( const TInt aNameId,
                                        const TInt32 aValue );
		TInt GetAttributeIntL(const TInt aNameId,  TInt32&  aValue );

		 void SetGradientTransMatrix(SVGReal aMatrix[2][3]); 

        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        virtual MGfxPaint *PaintAttribute() = 0;

        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        ~CSvgGradientElementImpl();
		
		RPointerArray<CSvgStopElementImpl> *iSvgStopElementArray;
		
    protected:
        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        void ConstructL(const TUint8 aElemID);

    protected:
        
        TGfxGradientPaint* iGradientPaint;

	    RArray<TSvgStopData> 	*iStopElementData;
    };

#endif /* SVGGRADIENTELEMENTIMPL_H */
