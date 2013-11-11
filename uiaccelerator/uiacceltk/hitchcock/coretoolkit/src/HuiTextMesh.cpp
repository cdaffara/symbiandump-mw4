/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Implementation of CHuiTextMesh. CHuiTextMesh stores a cached 
*                version of a text string.
*
*/


 
#include <AknUtils.h>
#include "alf/alfconstants.h"

#include "uiacceltk/HuiTextMesh.h"
#include "uiacceltk/HuiStatic.h"
#include "uiacceltk/HuiFont.h"
#include "uiacceltk/HuiTexture.h"
#include "uiacceltk/HuiGc.h"
#include "uiacceltk/HuiUtil.h"
#include "uiacceltk/HuiPanic.h"
#include "uiacceltk/HuiEnv.h"
#include "uiacceltk/huitextstylemanager.h"
#include "uiacceltk/huitextstyle.h"
#include "uiacceltk/HuiDisplay.h"
#include "HuiRosterImpl.h"

  
CHuiTextMesh::CHuiTextMesh()
        : iTextStyleId(0),
          iLineSpacing(0),        
          iTextMeshScale(1.0),        
          iLineMode(ELineModeTruncate),
          iMaxLineWidth(KMaxTInt),
          iScaledMaxLineWidth(KMaxTInt),
          iMaxLineCount(KMaxTInt),          
          iRasterizedShadow(EFalse)
    {
    }

    
void CHuiTextMesh::ConstructL()
    {
    // Register as observer
    CHuiStatic::Env().iActionObservers.AppendL(*this);
    RPointerArray<CHuiDisplay> displays = CHuiStatic::Env().Displays();
    for(TInt i = 0; i<displays.Count(); i++)
        {
        displays[i]->iVisibleAreaObservers.AppendL(*this);
        
        if (displays[i]->IsDisplayTypeTvOut())
            {
            displays[i]->iDeletionObservers.AppendL(*this);
            }
        }
    ReleaseFont();
        
    CalculateTvOutScales();
    }
   

CHuiTextMesh::~CHuiTextMesh()
    {
    // Remove observers
    CHuiStatic::Env().iActionObservers.RemoveIfFound(*this);
    RPointerArray<CHuiDisplay> displays = CHuiStatic::Env().Displays();
    for(TInt i = 0; i<displays.Count(); i++)
        {
        displays[i]->iVisibleAreaObservers.RemoveIfFound(*this);
        
        if (displays[i]->IsDisplayTypeTvOut())
            {
            displays[i]->iDeletionObservers.RemoveIfFound(*this);
            }
        }
        
    delete iString;
    iPictographInterface = NULL;
    }
    
    
void CHuiTextMesh::Reset()
    {
    delete iString; 
    iString = 0;
    }


void CHuiTextMesh::SetLineMode(TLineMode aLineMode)
    {
    iLineMode = aLineMode;
    }
    

CHuiTextMesh::TLineMode CHuiTextMesh::LineMode() const
    {
    return iLineMode;
    }


TBool CHuiTextMesh::SetMaxLineWidth(TInt aMaxLineWidth)
    {
    // Note: this do not tell if only scaling has been changed.
    TBool isDifferent = (iMaxLineWidth != aMaxLineWidth);
    iMaxLineWidth = aMaxLineWidth;

    // Max line width has to be scaled to get proper line wrapping
    if(iTextMeshScale != 1 && aMaxLineWidth != KMaxTInt)
        {
        // Scale width with Y-scale to retain correct font proportions
        iScaledMaxLineWidth = HUI_ROUND_FLOAT_TO_INT( aMaxLineWidth * iTextMeshScale );
        } 

    return isDifferent;
    }


TInt CHuiTextMesh::MaxLineWidth() const
    {
    if(iTextMeshScale != 1)
        {
        return iScaledMaxLineWidth;
        } 

    return iMaxLineWidth;
    }


TBool CHuiTextMesh::SetMaxLineCount(TInt aMaxLineCount)
    {
    TBool isDifferent = (iMaxLineCount != aMaxLineCount);
    iMaxLineCount = aMaxLineCount;
    return isDifferent;
    }


TInt CHuiTextMesh::MaxLineCount() const
    {
    return iMaxLineCount;
    }
      

void CHuiTextMesh::SetFontL(const THuiFont& aFont, TBool aRefreshTextures)
    {
    THuiTextStyle* textStyle = CHuiStatic::Env().TextStyleManager().TextStyle(iTextStyleId);
    textStyle->SetFont(aFont);
    
    if(iString && aRefreshTextures)
        {
        BuildL(ETrue);
        }
    }


void CHuiTextMesh::SetTextL(const TDesC& aText, TBool aRasterize)
    {
    delete iString; iString = 0;
    iString = aText.AllocL();

    // Rasterize text into textures.
    BuildL(aRasterize);    
    }

    
const TDesC* CHuiTextMesh::Text() const
    {
    return iString;
    }
    
    
void CHuiTextMesh::EnableRasterizedShadow(const TBool aIsEnabled)
    {
    TBool isChanged = ETrue;
    if ( (iRasterizedShadow && aIsEnabled) || 
         ( !iRasterizedShadow && !aIsEnabled ) )
        {
        isChanged = EFalse;
        }
        
    iRasterizedShadow = aIsEnabled;
        
    if(isChanged && iString)
        {
        // Status of rasterized shadow has changed.
        // Assuming the sharp shadow is prerendered.
        TRAP_IGNORE(BuildL(ETrue))
        }
    }
   
   
TBool CHuiTextMesh::RasterizedShadow() const
    {
    if ( iVisual )
        {
        return TBool(iVisual->DropShadowHandler());
        }
    return iRasterizedShadow;
    }
     
   
TSize CHuiTextMesh::Extents() const
    {
    TSize extents = iExtents;
    
    if(iTextMeshScale != 1)
        {
        extents.iHeight = iScaledExtents.iHeight;
        extents.iWidth = iScaledExtents.iWidth;
        }
    
    return extents;
    }
    
void CHuiTextMesh::ExpandRectWithShadow(TRect& /*aRect*/) const
    {
    }

void CHuiTextMesh::SetExtents(const TSize& aExtents)
    {
    TBool isChanged = (iExtents != aExtents);
    
    iExtents = aExtents;
    
    if(iTextMeshScale != 1 && isChanged)
        {
        iScaledExtents.iHeight = HUI_ROUND_FLOAT_TO_INT( aExtents.iHeight/iTextMeshScale );
        iScaledExtents.iWidth = HUI_ROUND_FLOAT_TO_INT( aExtents.iWidth/iTextMeshScale );
        }
    }


void CHuiTextMesh::InitPictographsL(CAknPictographInterface* aInterface)
    {
    iPictographInterface = aInterface;    
    }


void CHuiTextMesh::SetTextStyle(TInt aTextStyleId)
    {
    iTextStyleId = aTextStyleId;
    ReleaseFont(); 
    }


TInt CHuiTextMesh::TextStyle() const
	{
	return iTextStyleId;
	}


void CHuiTextMesh::BuildPictographsL()
    {        
    }


void CHuiTextMesh::SetLineSpacing(TInt aLineSpacingInPixels)
    {
    iLineSpacing = aLineSpacingInPixels;
    }
    
    
void CHuiTextMesh::ReleaseFont()
    {
    THuiTextStyle* textStyle = CHuiStatic::Env().TextStyleManager().TextStyle(iTextStyleId);
    textStyle->Font().ReleaseFont();
    }
    

void CHuiTextMesh::CalculateTvOutScales()
    {
    // Calculate the TV Out scales
    RPointerArray<CHuiDisplay> displays = CHuiStatic::Env().Displays();
    for(TInt i = 0; i<CHuiStatic::Env().DisplayCount(); i++)
        {
        if (displays[i]->IsDisplayTypeTvOut())
            {
            // Scale with height to retain correct font proportions
            iTextMeshScale = (TReal32)displays[i]->Size().iHeight / (TReal32)displays[i]->RosterImpl().Rect().Height();
            }
        }
    }
    
    
void CHuiTextMesh::NotifyDisplayVisibleAreaChanged(CHuiDisplay& aDisplay)
    {
    if (aDisplay.IsDisplayTypeTvOut())
        {
        iTextMeshScale = (TReal32)aDisplay.Size().iHeight / (TReal32)aDisplay.RosterImpl().Rect().Height();
        }
        
    ReleaseFont();
    }


void CHuiTextMesh::NotifyDisplayDeletion(CHuiDisplay& aDisplay)
    {
    if (iString && aDisplay.IsDisplayTypeTvOut())
        {
        iTextMeshScale = 1.0;
        ReleaseFont();
        ResetLines();
        ResetPictographLines();
        // Updates the mesh and the extents
        TRAP_IGNORE(BuildL(ETrue));
        }
    }


void CHuiTextMesh::HandleActionL(const THuiActionCommand& aActionCommand)
    {
    if(aActionCommand.Id() == KHuiActionNewTVOutDisplayUid.iUid)
        {
        // find the TvOut display
        RPointerArray<CHuiDisplay> displays = CHuiStatic::Env().Displays();
        for(TInt i = 0; i<displays.Count(); i++)
            {
            if (displays[i]->IsDisplayTypeTvOut())
                {
                // Register as observer
                displays[i]->iVisibleAreaObservers.AppendIfNotFoundL(*this);
                displays[i]->iDeletionObservers.AppendIfNotFoundL(*this);
                
                // calculate new scale factors
                CalculateTvOutScales();
                }
            }
            
        ReleaseFont();
        ResetLines();
        ResetPictographLines();
        // Updates the mesh and the extents
        if(Text() != NULL)
            {
            BuildL(ETrue);
            }
        }
            
    else if ( aActionCommand.Id() == KAknsMessageSkinChange )
        {
        // When the resolution/skin changes and this text visual is not in 
        // the roster, the change notification is not received through the normal
        // route aka CHuiVisual::NotifySkinChangedL() 
        //
        // Setting the iMaxLineWidth into -1 is a hack. It would be better to
        // set the iMeshUpdated into EFalse in the CHuiTextVisual, but this class
        // has no pointer to the owning text visual. The text visual calls the 
        // SetMaxLineWidth function before it draws this mesh so the "changed" 
        // notification is informed through that function call.
        iMaxLineWidth = -1;
        }
    else
        {
        // For PC lint
        }
    }

void CHuiTextMesh::SetRelatedVisual(CHuiVisual* aVisual)
    {
    iVisual = aVisual;
    }

void CHuiTextMesh::UpdateMeshL(const TDesC8& /*aBuffer*/)
    {
    }
