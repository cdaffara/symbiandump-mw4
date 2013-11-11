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



/* Includes */
#include <eikenv.h>
#include <e32cmn.h>

//Unsorted
#include <alf/alfenv.h> 
#include <alf/alftexture.h>
#include <alf/alfdisplay.h>
#include <alf/alfevent.h>
#include <alf/alfimageloaderutil.h>
#include <aknnotewrappers.h> 
#include <aknutils.h>
#include <StringLoader.h>

//Layouts
#include <alf/alfgridlayout.h>
#include <alf/alfviewportlayout.h>

//Visuals
#include <alf/alftextvisual.h>
#include <alf/alfimagevisual.h>
#include <alf/alfvisual.h>

//Brushes
#include <alf/alfborderbrush.h>
#include <alf/alfimagebrush.h>
#include <alf/alfdropshadowbrush.h>
#include <alf/alfbrush.h>
#include <alf/alfbrusharray.h>
	
//Own
#include "alfexstickerscontrol.h"
#include "alfexstickerlayout.h"
#include <alfexstickers.rsg>

//TEMP
#include <alf/alfroster.h>
#include <alf/alftransformation.h>
#include <alf/alfdecklayout.h>

//Literals

//Constants
const TInt KStickerBitmapCount = 4;

CAlfExStickersControl* CAlfExStickersControl::NewL(CAlfEnv& aEnv, RFs* aFs)
	{
	CAlfExStickersControl* self = new (ELeave) CAlfExStickersControl;
	CleanupStack::PushL(self);
	self->ConstructL(aEnv, aFs);
	CleanupStack::Pop(self);
	return self;
	}


CAlfExStickersControl* CAlfExStickersControl::NewLC(CAlfEnv& aEnv, RFs* aFs)
    {
    CAlfExStickersControl* self = new (ELeave) CAlfExStickersControl;
    CleanupStack::PushL(self);
    self->ConstructL(aEnv, aFs);
    return self;
    }


CAlfExStickersControl::CAlfExStickersControl(): iDraggingVisual(EFalse)
    {
    }


void CAlfExStickersControl::ConstructL(CAlfEnv& aEnv, RFs* aFs)
    {
    //Set UI Accelerator Toolkit environment object into iEnv class variable
    iEnv = &aEnv;
    //Set handle to file server session
    iFs=aFs;
        
    //Create new control for visuals
    CAlfControl::ConstructL(*iEnv);
    
    //Get the size of the main pane, save it to iLayoutsize
    AknLayoutUtils::LayoutMetricsSize(AknLayoutUtils::EMainPane, iLayoutsize);    

    //Assign size for the applicaton, Screen height by Screen width*2
    iVirtualSize.iWidth  = iLayoutsize.iWidth*2;
    iVirtualSize.iHeight = iLayoutsize.iHeight;

    //Create a viewPort layout to act as the root layout visual.
    CreateViewportL();
    
    //Initialize the texture manager
    SetTextureManagerL();
    
    //Set image for the application background
    LoadBackgroundImageL();
    
    //Create iPlainLayout and add it to the iViewPort
    iPlainLayout = CAlfLayout::AddNewL(*this,iViewPort);
 
    //Create the example stickers
    CreateStickersL();
    
}
    

CAlfExStickersControl::~CAlfExStickersControl()
    {
    for(TInt current=0;current < iImageLoaderUtilArray.Count();current++)
        {
        delete iImageLoaderUtilArray[current];
        }
    iImageLoaderUtilArray.Close();
    }
    

TBool CAlfExStickersControl::OfferEventL(const TAlfEvent& aEvent)
    {
    //petjarve:This needs to be cleaned up
    TBool visual = EFalse;
    
    //Different user input cases
    //If the event was pointer down event
    if (aEvent.IsPointerEvent() && aEvent.PointerDown())
        {
        iHasBeenDragged = EFalse;
        CAlfVisual* onVisual = aEvent.Visual();
        
        //Find out if sticker (text visual) was interacted
        for (int j = 0; j < KStickerCount; ++j) 
        	{
        	if (aEvent.Visual() == iStickerArray[j]->GetVisual())
        		{
        		iSelectedDeck = iStickerArray[j];
        		visual = ETrue;
        		}
        	}
        
        //Interaction was on a visual
        if (visual)
            {
            iOnVisual = onVisual;
            iPlainLayout->MoveVisualToFront(*iSelectedDeck);

            iDraggingVisual = ETrue;
            }
        
        //Interaction was on background -> move it
        else
            {
            ;
            }   
            
        iStartDisplayPoint = aEvent.PointerEvent().iPosition;
        iStartVisualPos = onVisual->Pos().ValueNow();
            
        Display()->Roster().AddPointerEventObserver(EAlfPointerEventReportDrag, *this);
                
        return ETrue;    
        }
    
    //If the event was a "drag" event
    else if(aEvent.IsPointerEvent() && aEvent.PointerEvent().iType == TPointerEvent::EDrag)
        {
        iHasBeenDragged = ETrue;
        //If we are dragging sticker
        if (iDraggingVisual && iSelectedDeck)
            {
            iDraggedPosition = 
                aEvent.PointerEvent().iPosition
                - iStartDisplayPoint+iViewPos;
            
            iSelectedDeck->SetPos(iDraggedPosition, 50);
            }

        //Else we are dragging the background
        else
            {
            TAlfRealPoint temp = (iStartDisplayPoint-(aEvent.PointerEvent().iPosition));

            if(temp.iX <0) temp.iX=0;
            if(temp.iY <0) temp.iY=0;
            
            if (temp.iY  > iVirtualSize.iHeight - iLayoutsize.iHeight)
	            {
	            temp.iY =iVirtualSize.iHeight- iLayoutsize.iHeight ;
	            }
	            
	        if (temp.iX  > iVirtualSize.iWidth - iLayoutsize.iWidth)
	            {
	            temp.iX=iVirtualSize.iWidth- iLayoutsize.iWidth ;
	            }    
            
            iViewPos=temp;
            iViewPort->SetViewportPos(iViewPos, 0);   
            }    
        
        return ETrue;
        }

    //If the event was simple tap event, ie. not a drag
    else if (aEvent.PointerUp() && iHasBeenDragged == EFalse)
    	{
    	//petjarve: Possibly unneeded check
    	if (iStartDisplayPoint == aEvent.PointerEvent().iPosition && iSelectedDeck)
    		{
    		iSelectedDeck->ChangeStickerSizeL();
    		
	    	return ETrue;
    		}
    	return ETrue;
    	}
    
    else if( aEvent.PointerUp() )
        {
        Display()->Roster().RemovePointerEventObserver(EAlfPointerEventReportDrag, *this);
        iDraggingVisual = EFalse;
        return ETrue;
        }
        
    return EFalse;
    }


void CAlfExStickersControl::CreateViewportL()
	{
	//Create a viewPort layout to act as the root layout visual.
    iViewPort = CAlfViewportLayout::AddNewL(*this); 

    //Set inital viewport position value to top left corner
    iViewPos = TAlfRealPoint(0, 0);
    
    //Set Viewport sizes, both virtual and non virtual and position (top left corner)
    iViewPort->SetVirtualSize(TAlfRealSize(iVirtualSize), 0);
    iViewPort->SetViewportSize(TAlfRealSize(iLayoutsize), 0);
    iViewPort->SetViewportPos(iViewPos, 0);
    
    //Enable scrolling of the Viewport
    iViewPort->EnableScrollingL();

	}


void CAlfExStickersControl::SetTextureManagerL()
	{
	//First create path to our textures
    User::LeaveIfError( iFs->CreatePrivatePath(EDriveC));
	TChar driveChar;
    iFs->DriveToChar( EDriveC,driveChar);
    TDriveName driveName;
    driveName.Append(driveChar);
    driveName.Append(KDriveDelimiter);
       
    iFs->PrivatePath(iPrivateFolderPath);
    iPrivateFolderPath.Insert(0,driveName);

    //Set texture manager path
    Env().TextureManager().SetImagePathL(iPrivateFolderPath);
	}


void CAlfExStickersControl::LoadBackgroundImageL()
	{
    //Experimental background image for the viewport
	//Load the board texture
	TFileName filename;
	filename.Append(iPrivateFolderPath);
	filename.Append(*StringLoader::LoadLC(R_ALFEXSTICKERS_BOARD_TEXTURE));
	CleanupStack::PopAndDestroy(1);
	
	//Create image loader util. This object must exist through life time of texture. 
	CAlfImageLoaderUtil* imageLoaderUtil = new(ELeave) CAlfImageLoaderUtil;
	iImageLoaderUtilArray.AppendL(imageLoaderUtil); 
	
	//Create board texture from SVG file.
	CAlfTexture& board = CreateSVGTextureL(filename, iVirtualSize, imageLoaderUtil);
	
    //Create new Image Visual and add it to the iViewPort
    CAlfImageVisual* image = CAlfImageVisual::AddNewL(*this, iViewPort); 
    
    //Set scaling to cover the whole visual/layout
    image->SetScaleMode(CAlfImageVisual::EScaleFitHeight);
    
    //Set board texture into image visual
    image->SetImage(TAlfImage(board));
	}


CAlfTexture& CAlfExStickersControl::CreateSVGTextureL(const TFileName& aFileName, 
        TSize& aSize, CAlfImageLoaderUtil* aImageLoaderUtil)
    {
    //Set size of the SVG image. Must be called before creating image loader.
    aImageLoaderUtil->SetSize(aSize, EAspectRatioNotPreserved);
    
    //Use image loader util to create SVG image loader. 
    MAlfBitmapProvider* provider = aImageLoaderUtil->CreateSVGImageLoaderL(aFileName);
            
    //Use texture manager to create the texture with given bitmap provider.
    return Env().TextureManager().CreateTextureL(KAlfAutoGeneratedTextureId, 
            provider, EAlfTextureFlagDefault);

    }


void CAlfExStickersControl::CreateStickersL()
	{
	//Create example stickers for the application
	for (int i = 0; i < KStickerCount; ++i) 
    	{
    	iStickerArray[i] = static_cast<CAlfExStickerLayout*>(CAlfExStickerLayout::AddNewL(*this, iPlainLayout));
    	iStickerArray[i]->SetStickerPlaceL(this);
    	iStickerArray[i]->SetStickerTextContentL();
    	iStickerArray[i]->SetStickerDropShadowBrushL();
    	LoadStickerImageL(i);
    	
    	if (i==0)
    		iStickerArray[i]->ChangeStickerSizeL();
    	}
	}


void CAlfExStickersControl::LoadStickerImageL(TInt aIndex)
    {
    //This makes bitmap selection to proceed sequently within cycles.
    TInt bitmapID = aIndex % KStickerBitmapCount;
            
    TFileName filename;
    filename.Append(iPrivateFolderPath);
    filename.Append(*StringLoader::LoadLC(R_ALFEXSTICKERS_STICKER_TEXTURE_BLUE + bitmapID));
    CleanupStack::PopAndDestroy(1);
    
    //Create image loader util. This object must exist through life time of texture. 
    CAlfImageLoaderUtil* imageLoaderUtil = new(ELeave) CAlfImageLoaderUtil;
    iImageLoaderUtilArray.AppendL(imageLoaderUtil);
    
    TSize stickerSize(KStickerWidth, KStickerHeight);
    
    //Create sticker texture
    CAlfTexture& stickerTexture = CreateSVGTextureL(filename, stickerSize, imageLoaderUtil);
    
    //Assign texture to image brush
    iStickerArray[aIndex]->SetStickerImageBrushL(stickerTexture);
    }


CAlfControl* CAlfExStickersControl::GetControl()
	{
	//Returns control to child visuals who need it
	return this;
	}

