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


#ifndef __INC_MSVGTESTS__
#define __INC_MSVGTESTS__

#include <e32base.h>
#include <badesca.h>


/**
 * Class description goes here.
 *
 *  @lib SVGEngine.lib
 *  @since 1.0
 */
class MSvgTests
    {
    public:


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        virtual void    GetRequiredFeatures( const CDesCArrayFlat*& aRequiredFeatures ) = 0;


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        virtual void    GetRequiredExtensions( const CDesCArrayFlat*& aRequiredExtensions ) = 0;


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        virtual TBool    GetSystemLanguage( const CDesCArrayFlat*& aSystemLanguage ) = 0;


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        virtual TBool   HasExtension( const TDesC& aExtension ) = 0;


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        virtual TBool   HasFeature( const TDesC& aFeature ) = 0;
        
        virtual TBool	HasAnyTests() = 0;
    };

#endif /* __INC_MSVGTESTS__ */
