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



#include "alf/alftextureautosizeparams.h"


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TAlfTextureAutoSizeParams::TAlfTextureAutoSizeParams() : 
        iSizeLowerThreshold(EMedium),
        iSizeUpperThreshold(EMedium),
        iMinSizeChange(EMedium),
        iDownsizeSettleThreshold(EMedium)
    {    
    };        

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TInt TAlfTextureAutoSizeParams::SizeLowerThreshold() const
    {
    return iSizeLowerThreshold;    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void TAlfTextureAutoSizeParams::SetSizeLowerThreshold(TInt aSizeLowerThreshold)
    {
    iSizeLowerThreshold = aSizeLowerThreshold;    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TInt TAlfTextureAutoSizeParams::SizeUpperThreshold() const
    {
    return iSizeUpperThreshold;    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void TAlfTextureAutoSizeParams::SetSizeUpperThreshold(TInt aSizeLowerThreshold)
    {
    iSizeUpperThreshold = aSizeLowerThreshold;        
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TInt TAlfTextureAutoSizeParams::MinSizeChange() const
    {
    return iMinSizeChange;    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void TAlfTextureAutoSizeParams::SetMinSizeChange(TInt aMinSizeChange)
    {
    iMinSizeChange = aMinSizeChange;    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TInt TAlfTextureAutoSizeParams::DownsizeSettleThreshold() const
    {
    return iDownsizeSettleThreshold;    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void TAlfTextureAutoSizeParams::SetDownsizeSettleThreshold(TInt aDownsizeSettleThreshold)
    {
    iDownsizeSettleThreshold = aDownsizeSettleThreshold;    
    }  
