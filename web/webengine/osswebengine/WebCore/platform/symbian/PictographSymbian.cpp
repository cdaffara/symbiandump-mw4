/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
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


// INCLUDE FILES
#include "config.h"
#include "PictographSymbian.h"
#include "WebCoreGraphicsContext.h"
#include <featmgr.h>

// DEFINES
using namespace WebCore;

// CONSTANTS

// LOCAL FUNCTION PROTOTYPES

//-------------------------------------------------------------------------------
// 
// 
//-------------------------------------------------------------------------------
PictographSymbian::PictographSymbian()
{
    if (FeatureManager::FeatureSupported(KFeatureIdJapanesePicto))
        {                             
        iPictographInterface = NULL;
        //This parameter is not used.
        CCoeControl* coeemptyctrlnotused = NULL;        
        TRAP_IGNORE(iPictographInterface = CAknPictographInterface::NewL(*coeemptyctrlnotused,*this));
        }
    else
        {
        iPictographInterface = NULL;
        }        
}

//-------------------------------------------------------------------------------
// 
// 
//-------------------------------------------------------------------------------
PictographSymbian::~PictographSymbian()
{
    delete iPictographInterface;
}

//-------------------------------------------------------------------------------
// DrawPictographsInText
// Draw Pictographs in text given the TPoint
//-------------------------------------------------------------------------------
void PictographSymbian::DrawPictographsInText(WebCoreGraphicsContext* aContext, CBitmapContext& aGc, const CFont& aFont, const TDesC& aText, const TPoint& aPosition)
    {
    if (iPictographInterface)
        {
        iContext = aContext;
        iPictographInterface->Interface()->DrawPictographsInText(aGc, aFont, aText, aPosition);
        }
    }

//-------------------------------------------------------------------------------
// DrawPictographsInText
// Draw Pictographs in text given the offset, alignment and left margin
//-------------------------------------------------------------------------------
void PictographSymbian::DrawPictographsInText(WebCoreGraphicsContext* aContext, CBitmapContext& aGc, 
                                                const CFont& aFont, const TDesC& aText,
                                                const TRect& aBox, TInt aBaselineOffset, 
                                                CGraphicsContext::TTextAlign aAlignment ,TInt aLeftMargin )
    {
    if (iPictographInterface)
        {
        iContext = aContext;
        iPictographInterface->Interface()->DrawPictographsInText(aGc, aFont, aText, aBox, aBaselineOffset, aAlignment, aLeftMargin);
        }
    }


//-------------------------------------------------------------------------------
// DrawPictographArea
// Callback function called by the Pictograph Drawer Interface
//-------------------------------------------------------------------------------
void PictographSymbian::DrawPictographArea()
    {
    if (iContext)
        {
        iContext->view().draw(*iContext, iContext->view().visibleRect());
        iContext->view().invalidateRect(iContext->view().visibleRect(), ETrue);            
        }
    }



// End of File


