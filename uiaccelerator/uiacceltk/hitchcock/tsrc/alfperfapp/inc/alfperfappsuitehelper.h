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

#ifndef __ALFPERFAPPSUITEHELPER_H__
#define __ALFPERFAPPSUITEHELPER_H__

// INCLUDES
#include <alf/AlfVisual.h>
#include <alf/AlfGradientBrush.h>
#include <alf/AlfBorderBrush.h>
#include <alf/AlfBrushArray.h>
#include <alf/AlfDropShadowBrush.h>
#include <alf/AlfMetric.h>

// CLASS DECLARATION
class TAlfPerfAppSuiteHelper
	{
	public:
		static void AddBorderBrushL( 
				CAlfVisual* aVisual,
				const TRgb& aColor,
				TInt aThicknessWidth = 1,
				TInt aThicknessHeight = 1 );
		
		static void AddGradientBrushL( 
				CAlfVisual* aVisual,
				const TRgb& aColor1,
				const TRgb& aColor2,
				CAlfGradientBrush::TDirection aDirection = CAlfGradientBrush::EDirectionDown );
		
		static void AddGradientBrushL( 
				CAlfVisual* aVisual,
				const TRgb& aColor );		
		
		static void AddDropShadowBrushL(
				CAlfVisual* aVisual,
				const TRgb& aColor,
				TInt aDepth	);
	};

#endif // __ALFPERFAPPSUITEHELPER_H__

// end of file

