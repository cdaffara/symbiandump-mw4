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

#ifndef __ALFPERFAPPSUITEEPGCONTROL_H__
#define __ALFPERFAPPSUITEEPGCONTROL_H__

// INCLUDES
#include <alf/alfcontrol.h>
#include "alfperfappsuiteflags.h"

// FORWARD DECLARATIONS
class CAlfLayout;
class CAlfPerfAppSuiteTestCaseScroll;

// CLASS DECLARATIONS
class TProgram
	{
	public:
		/**
		 * C++ constructor.
		 */		
		TProgram();
		
		/**
		 * C++ constructor.
		 * @param aName Name of the program.
		 * @param aStart Start time of the program.
		 * @param aEnd End time of the program.
		 */		
		TProgram( const TDesC& aName, const TDesC& aStart, const TDesC& aEnd );
		
	public:
		
		/**
		 * Calculates the duration of the program.
		 * @return duration of the program.
		 */
		TInt DurationInMinutes();
		
	public:
		/**
		 * Name of the program.
		 */
		TBuf<100> iName;
		
		/**
		 * Start time.
		 */
		TTime iStart;
		
		/**
		 * End time.
		 */
		TTime iEnd;
	};

class CAlfPerfAppSuiteEPGControl : public CAlfControl, public MAlfTextureLoadingCompletedObserver
    {
	private: // Enums
		
		/**
		 * Textstyles used in EGP.
		 */
		enum TTextStyle
			{
			ETextStyleTitle = 0,
			ETextStyleTime,
			ETextStyleProgram,
			ENumberOfTextStyles
			};
	
	public: // Construction and desctruction
		
		/**
		 * C++ constuctor.
		 * @param aScrollControl is used as observer
		 */ 
		CAlfPerfAppSuiteEPGControl(CAlfPerfAppSuiteTestCaseScroll* aScrollControl);
		
		/**
		 * Destructor.
		 */
		~CAlfPerfAppSuiteEPGControl();
		
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
		void AddRowL( 
				CAlfLayout* aParentLayout,
				TAlfImage& aChannelImage,
				RArray<TProgram>& aPrograms,
				const TTime& aTimeWindowStart,
				const TTime& aTimeWindowEnd,
				TInt aTextStyle,
				TBool aSetBorders = ETrue
				);
		
		void AddProgramsL( 
				CAlfLayout* aParentLayout,
				RArray<TProgram>& aPrograms,
				const TTime& aTimeWindowStart,
				const TTime& aTimeWindowEnd,
				TInt aTextStyle,
				TBool aSetBorders
				);		
	   
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

#endif // __ALFPERFAPPSUITEEPGCONTROL_H__

// end of file
