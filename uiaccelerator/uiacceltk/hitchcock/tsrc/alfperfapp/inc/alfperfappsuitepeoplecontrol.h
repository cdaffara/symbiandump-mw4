/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  
*
*/

#ifndef __ALFPERFAPPSUITEPEOPLECONTROL_H__
#define __ALFPERFAPPSUITEPEOPLECONTROL_H__

// INCLUDES
#include <alf/alfcontrol.h>
#include "alfperfappsuiteflags.h"

// FORWARD DECLARATIONS
class CAlfLayout;
class CAlfGridLayout;
class TAlfImage;
class CAlfPerfAppSuiteTestCaseScroll;

// CLASS DECLARATION
class CAlfPerfAppSuitePeopleControl : public CAlfControl, public MAlfTextureLoadingCompletedObserver
    {
	private: // Enums
		
		/**
		 * Textstyles used in people-suite.
		 */		
		enum TTextStyle
			{
			ETextStyleHeader = 0,
			ETextStyleBig,
			ETextStyleSmall,
			ENumberOfTextStyles
			};
		
	public: // Construction and destruction
		
		/**
		 * C++ constructor.
		 * @param aScrollControl is used as observer
		 */		
		CAlfPerfAppSuitePeopleControl(CAlfPerfAppSuiteTestCaseScroll* aScrollControl);
		
		/**
		 * Destructor.
		 */		
		~CAlfPerfAppSuitePeopleControl();
		
		/**
		 * Second-phase constructor.
		 * @param aEnv Alf environment.
		 * @param aVisibleArea Visible screen area.
		 * @param aParentLayout Parent layout.
		 */		
	    virtual void ConstructL( 
	        CAlfEnv& aEnv,
	        const TSize& aVisibleArea,
	        CAlfLayout* aParentLayout );
	    
	private: // New functions

#ifdef ENABLE_TEXTSTYLES
		void CreateTextStylesL();
#endif
		
	    CAlfGridLayout* ConstructListItemLayoutL(
	    		CAlfLayout* aParentLayout,
	    		TInt aHeight,
	    		TBool aSelected = EFalse,
	    		TBool aBorders = ETrue );
	    
	    CAlfGridLayout* ConstructListItemTextLayoutL( 
	    		CAlfLayout* aParentLayout );
	    
	    void ConstructListHeaderL(
	    		CAlfLayout* aParentLayout,
	    		const TAlfImage& aImage,
	    		const TDesC& aText,
	    		TInt aHeight );
	    
	    void ConstructListItemL(
	    		CAlfLayout* aParentLayout,
	    		const TDesC& aName,
	    		const TDesC& aDescription,
	    		TInt aHeight,
	    		TBool aSelected = EFalse );

	    // From base class MAlfTextureLoadingCompletedObserver
	    void TextureLoadingCompleted(
	            CAlfTexture& aTexture,
	            TInt aTextureId,
	            TInt aErrorCode );
	        
	private:
		TSize iVisibleArea;

	    /**
	     * count of the number of loaded textures
	     */
		TInt iLoadedImages;
		
	    /**
	     * Used for callback.
	     * Not owned.
	     */
		CAlfPerfAppSuiteTestCaseScroll* iScrollControl;

#ifdef ENABLE_TEXTSTYLES		
		RArray<TInt> iTextStyles;
#endif
		
    };

#endif // __ALFPERFAPPSUITEPEOPLECONTROL_H__

// end of file
