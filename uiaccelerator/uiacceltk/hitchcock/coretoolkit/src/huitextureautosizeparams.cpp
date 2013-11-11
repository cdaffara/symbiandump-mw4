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
* Description:   Texture autosize params
*
*/



#include "uiacceltk/huitextureautosizeparams.h"

EXPORT_C THuiTextureAutoSizeParams::THuiTextureAutoSizeParams() : 
        iSizeLowerThreshold(EMedium),
        iSizeUpperThreshold(EMedium),
        iMinSizeChange(EMedium),
        iDownsizeSettleThreshold(EMedium)
    {    
    };        

EXPORT_C TInt THuiTextureAutoSizeParams::SizeLowerThreshold() const
    {
    return iSizeLowerThreshold;    
    }

EXPORT_C void THuiTextureAutoSizeParams::SetSizeLowerThreshold(TInt aSizeLowerThreshold)
    {
    iSizeLowerThreshold = aSizeLowerThreshold;    
    }

EXPORT_C TInt THuiTextureAutoSizeParams::SizeUpperThreshold() const
    {
    return iSizeUpperThreshold;    
    }

EXPORT_C void THuiTextureAutoSizeParams::SetSizeUpperThreshold(TInt aSizeLowerThreshold)
    {
    iSizeUpperThreshold = aSizeLowerThreshold;        
    }

EXPORT_C TInt THuiTextureAutoSizeParams::MinSizeChange() const
    {
    return iMinSizeChange;    
    }

EXPORT_C void THuiTextureAutoSizeParams::SetMinSizeChange(TInt aMinSizeChange)
    {
    iMinSizeChange = aMinSizeChange;    
    }

EXPORT_C TInt THuiTextureAutoSizeParams::DownsizeSettleThreshold() const
    {
    return iDownsizeSettleThreshold;    
    }

EXPORT_C void THuiTextureAutoSizeParams::SetDownsizeSettleThreshold(TInt aDownsizeSettleThreshold)
    {
    iDownsizeSettleThreshold = aDownsizeSettleThreshold;    
    }

