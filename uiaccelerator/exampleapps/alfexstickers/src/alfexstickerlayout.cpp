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

#include <alf/alfenv.h> 
#include <alf/alftexture.h>
#include <StringLoader.h>
#include <alf/alfroster.h>
#include <alf/alftransformation.h>


//Events
#include <alf/alfevent.h>

//Layouts
#include <alf/alfdecklayout.h>

//Visuals
#include <alf/alftextvisual.h>

//Brushes
#include <alf/alfborderbrush.h>
#include <alf/alfimagebrush.h>
#include <alf/alfdropshadowbrush.h>
#include <alf/alfbrush.h>
#include <alf/alfbrusharray.h>
	
//Own
#include "alfexstickerlayout.h"
#include "alfexstickerscontrol.h"
#include <alfexstickers.rsg>

//TEMP
#include <alf/alfcontrol.h>

//Literals


//Constants
const TInt KXCoordinate = 30;
const TInt KYCoordinate = 30;


//Factory functions are not currently used
CAlfExStickerLayout* CAlfExStickerLayout::NewL(CAlfExStickersControl& aControl, CAlfEnv& aEnv)
    {
    CAlfExStickerLayout* self = new (ELeave) CAlfExStickerLayout;
    CleanupStack::PushL(self);
    self->ConstructL(aControl, aEnv);
    CleanupStack::Pop();
    return self;
    }


CAlfExStickerLayout::CAlfExStickerLayout()
    {
    }


void CAlfExStickerLayout::ConstructL(CAlfExStickersControl& aControl, CAlfEnv& aEnv)
    {
    iControl = &aControl;
    iEnv = &aEnv;
    }
    

CAlfExStickerLayout::~CAlfExStickerLayout()
    {
    }
    

void CAlfExStickerLayout::SetStickerPlaceL(CAlfExStickersControl* aControl)
	{
	// Create TAlfRealRect which contains TextVisual placement (KXCoordinate, KYCoordinate)
	// and TextVisual size (KStickerWidth, KStickerHeight).
    TAlfRealRect rect(TRect(TPoint(KXCoordinate, KYCoordinate), TSize(KStickerWidth, KStickerHeight)));
    
    //Use SetRect function to set the afore mentioned placement and size
    SetRect(rect);
    
    //Get parent control
    CAlfControl* control = aControl->GetControl();
    
    //Add new text visual into the deck layout
    iTextVisual = CAlfTextVisual::AddNewL(*control, this);
    
    //Set the text visual to fill the same space as the deck layout
    iTextVisual->SetRect(TAlfRealRect(TRect(TPoint(0, 0), TSize(KStickerWidth, KStickerHeight))));
    
    //Set sticker size to small
    iStickerIsLarge = ETrue;
	}


void CAlfExStickerLayout::SetStickerTextContentL()
	{
	//Function is used to set text inside the text visual and to format it
	//Set example text to sticker
	iTextVisual->SetTextL(*StringLoader::LoadLC(R_ALFEXSTICKERS_DEFAULT_TEXT));
    CleanupStack::PopAndDestroy(1);
    
	//Set text to wrap so that the lines are broken to the visual's content area.
	iTextVisual->SetWrapping(CAlfTextVisual::ELineWrapBreak);
	
	//Set text color to black
	iTextVisual->SetColor(KRgbBlack);
	
	//Set line spacing
	iTextVisual->EnableShadow(EFalse);
	
	//Set predefined style for the text
	iTextVisual->SetTextStyle( EAlfTextStyleSmall );
	
	//Align the text content to visuals center, in both axis
	iTextVisual->SetAlign(EAlfAlignHCenter, EAlfAlignVCenter);
	
	//Add padding between text and visual border, 10 pixels to every side
	iTextVisual->SetPadding(TAlfBoxMetric(10, 10, 10, 10));
	}


void CAlfExStickerLayout::SetStickerDropShadowBrushL()
	{
	//First we must enable brushes
	//NOTE: Before adding brushes to visual, EnableBrushesL() needs to be invoked or the application crashes
	EnableBrushesL();	
	
	//Create Drop Shadow Brush, second parameter is the shadow size in pixels
	CAlfDropShadowBrush* dropBrush = CAlfDropShadowBrush::NewLC(Env(), 5);
    
    //Append the brush to the visual
	Brushes()->AppendL(dropBrush, EAlfHasOwnership);
    
    CleanupStack::Pop(1, dropBrush); 
	}


void CAlfExStickerLayout::SetStickerImageBrushL(CAlfTexture& aTexture)
	{
	//NOTE: Before adding brushes to visual, EnableBrushesL() needs to be invoked or the application crashes
	
	//Create image brush using the texture from loaded above
	CAlfImageBrush* imageBrush = CAlfImageBrush::NewLC(Env(), TAlfImage(aTexture));
	
    //Set the image brush to background layer
	imageBrush->SetLayer(EAlfBrushLayerBackground);
	
	//Append the brush to the visual
	Brushes()->AppendL(imageBrush, EAlfHasOwnership);
	
	CleanupStack::Pop(1, imageBrush);
	}


CAlfVisual* CAlfExStickerLayout::GetVisual() {
	{
	//Function return the text visual for the control, needed in event handling
	return iTextVisual;
	}
}


void CAlfExStickerLayout::ChangeStickerSizeL()
	{
	//Transformations need to be enabled or else the application crashes on scaling
	iTextVisual->EnableTransformationL();
	
	if (iStickerIsLarge)
		{
	    //Timed values for scaling
	    TAlfTimedValue x;
		TAlfTimedValue y;
		
		//Set starting state scale
		x.SetValueNow(1.0);
		y.SetValueNow(1.0);
		
		//Set scale target, halve the size and do it in 1000ms
		x.SetTarget(0.5, 1000);
		y.SetTarget(0.5, 1000);
		
		//Resize the deck
		SetSize(TAlfRealSize(150, 150), 1000);
		
		//Scale the text visual
		iTextVisual->Transformation().Scale(x, y);
				
		iStickerIsLarge = EFalse;
		}
	else if (!iStickerIsLarge)
		{
	    //Timed values for scaling
		TAlfTimedValue x;
		TAlfTimedValue y;
		
		//Set starting state scale
		x.SetValueNow(1.0);
		y.SetValueNow(1.0);
		
		//Set scale target, double the size and do it in 1000ms
		x.SetTarget(2.0, 1000);
		y.SetTarget(2.0, 1000);
		
		//Scale the text visual
		iTextVisual->Transformation().Scale(x, y);
		
		//Resize the deck
		SetSize(TAlfRealSize(300, 300), 1000);

		iStickerIsLarge = ETrue;
		}
	}

