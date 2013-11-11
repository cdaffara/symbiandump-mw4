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


// INCLUDES
#include "alfperfappsuitehelper.h"


// ============================ MEMBER FUNCTIONS ===============================

void TAlfPerfAppSuiteHelper::AddBorderBrushL( 
		CAlfVisual* aVisual,
		const TRgb& aColor,
		TInt aThicknessWidth,
		TInt aThicknessHeight)
	{
	CAlfBorderBrush* brush = CAlfBorderBrush::NewLC( aVisual->Env(), aThicknessWidth, aThicknessHeight, 0, 0);
	aVisual->EnableBrushesL();
	brush->SetColor(aColor);
	brush->SetLayer( EAlfBrushLayerForeground ); // this will be drawn on top of everything
    aVisual->Brushes()->AppendL(brush, EAlfHasOwnership);
    CleanupStack::Pop(brush);	
	}

void TAlfPerfAppSuiteHelper::AddGradientBrushL( 
		CAlfVisual* aVisual,
		const TRgb& aColor1,
		const TRgb& aColor2,
		CAlfGradientBrush::TDirection aDirection )
	{
	CAlfGradientBrush* brush = CAlfGradientBrush::NewLC( aVisual->Env() );
	aVisual->EnableBrushesL();
	brush->SetDirection( aDirection );
	brush->AppendColorL( 0, aColor1 );
	brush->AppendColorL( 1, aColor2 );
    aVisual->Brushes()->AppendL(brush, EAlfHasOwnership);
    CleanupStack::Pop(brush);	
	}

void TAlfPerfAppSuiteHelper::AddGradientBrushL( 
		CAlfVisual* aVisual,
		const TRgb& aColor )
	{
	// This is effectively a solid brush
	CAlfGradientBrush* brush = CAlfGradientBrush::NewLC( aVisual->Env() );
	aVisual->EnableBrushesL();
	brush->SetColor( aColor );
    aVisual->Brushes()->AppendL(brush, EAlfHasOwnership);
    CleanupStack::Pop(brush);	
	}

void TAlfPerfAppSuiteHelper::AddDropShadowBrushL(
		CAlfVisual* aVisual,
		const TRgb& aColor,
		TInt aDepth
		)
	{
	CAlfDropShadowBrush* brush = CAlfDropShadowBrush::NewLC( aVisual->Env(), aDepth );
	aVisual->EnableBrushesL();
	brush->SetColor( aColor );
    aVisual->Brushes()->AppendL(brush, EAlfHasOwnership);
    CleanupStack::Pop(brush);	
	}

// end of file
