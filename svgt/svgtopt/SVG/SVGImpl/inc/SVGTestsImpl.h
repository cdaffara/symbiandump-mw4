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


#ifndef __INC_CSVGTESTSIMPL__
#define __INC_CSVGTESTSIMPL__

#include <e32base.h>
#include <badesca.h>
#include "SVGTests.h"

/// Implements SvgTests interface. Elements implementing this interface should
/// provide wrappers. Note that not all SVG elements implement this interface
/// in SVG DOM.


/**
 * Class description goes here.
 *
 *  @lib SVGEngine.lib
 *  @since 1.0
 */
class CSvgTestsImpl : public CBase, public MSvgTests
    {
    public:


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        static CSvgTestsImpl*   NewL();


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        static CSvgTestsImpl*   NewLC();


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
                                ~CSvgTestsImpl();

        // From MSvgTests


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        void                    GetRequiredFeatures( const CDesCArrayFlat*& aRequiredFeatures );


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        void                    GetRequiredExtensions( const CDesCArrayFlat*& aRequiredExtensions );


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        TBool                   GetSystemLanguage( const CDesCArrayFlat*& aSystemLanguage );


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        TBool                   HasExtension( const TDesC& aExtension );


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        TBool                   HasFeature( const TDesC& aFeature );

		TBool 					HasAnyTests();
		
        // Implementation

		/**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
		TBool SetSystemLanguageAttribute( const TDesC& aValue  );
		
		/**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
         
		TBool SetRequiredExtensionAttribute( const TDesC& aValue  );
        
        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
         
        TBool SetRequiredFeatureAttribute( const TDesC& aValue  );
        
        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        TBool                   SetTestAttributeL( const TDesC& aPropertyName,
                                                   const TDesC& aValue );


		    // functions for encoding

	    void                    SetRequiredFeatures(  CDesCArrayFlat*& aRequiredFeatures );


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        void                    SetRequiredExtensions(  CDesCArrayFlat*& aRequiredExtensions );


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        void                    SetSystemLanguage(  CDesCArrayFlat*& aSystemLanguage );


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        CSvgTestsImpl*          CloneL();

    private:


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        void                    ConstructL();

        CDesCArrayFlat*         iRequiredFeatures;

        CDesCArrayFlat*         iRequiredExtensions;

        CDesCArrayFlat*         iSystemLanguage;
        
        TBool					iSystemLanguageEmpty;
    };

#endif /* __INC_CSVGTESTSIMPL__ */