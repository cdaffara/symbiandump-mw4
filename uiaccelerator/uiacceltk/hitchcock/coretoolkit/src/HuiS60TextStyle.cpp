/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   A Text style encapsulates a unique text style 
*                in Hitchcock UI Toolkit usage.
*
*/



#include <AknUtils.h>
#include <AknLayoutFont.h>
#include <AknFontSpecification.h>

#include "HuiS60TextStyle.h"


EXPORT_C THuiS60TextStyle::THuiS60TextStyle(TInt aFontStyleId, TInt aParentId)
    : THuiTextStyle(aFontStyleId, aParentId)
    {
    Init();
    }

void THuiS60TextStyle::Init()
    {
    if (!CCoeEnv::Static())
        {
        return;
        }
		
    TFontSpec fs; 
    // note that we can use CAknLayoutFont::AsCAknLayoutFontOrNull to downcast
    const CAknLayoutFont* font = AknLayoutUtils::LayoutFontFromId(iFontStyleId, NULL);
    fs = font->FontSpecInTwips();
    
    // Define everything that is found from the TFontSpec
    iLocalDefinitionFlags |= KTextSizeDefined;
    iLocalDefinitionFlags |= KStrokeWeightDefined;
    iLocalDefinitionFlags |= KPostureDefined;
    iLocalDefinitionFlags |= KFontDefined;

    iFont.SetFontSpec(fs); 
    THuiFont huiFont(0, fs);
    
    // transfer the text pane height between the different font specs
    TAknFontSpecification aknFs = font->FontSpecification();
    huiFont.SetTextPaneHeight(aknFs.TextPaneHeight());
    
    SetFont(huiFont);
    }
