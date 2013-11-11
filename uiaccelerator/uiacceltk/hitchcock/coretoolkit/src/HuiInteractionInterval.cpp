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
* Description:   THuiInteractionInterval is a utility class that can be used 
*                as specify intervals that adapt to user input.
*
*/



#include "uiacceltk/HuiInteractionInterval.h"  // Class definition
#include "uiacceltk/HuiStatic.h"


EXPORT_C THuiInteractionInterval::THuiInteractionInterval(TReal32 aScalar) __SOFTFP
        : iScalar(aScalar), iLastTimeMs(0)
    {
    }


EXPORT_C void THuiInteractionInterval::SetScalar(TReal32 aScalar) __SOFTFP
    {
    iScalar = aScalar;
    }
    
    
EXPORT_C TReal32 THuiInteractionInterval::Scalar() const __SOFTFP
    {
    return iScalar;
    }
    

EXPORT_C TInt THuiInteractionInterval::Interval(TInt aIntervalTime)
    {
    TUint32 now = CHuiStatic::MilliSecondsSinceStart();
    TUint32 elapsed = now - iLastTimeMs;
    
    if(iScalar * elapsed < aIntervalTime)
        {
        // The last movement is probably still going on.
        aIntervalTime = TInt(elapsed * iScalar);
        }
        
    iLastTimeMs = now;
    
    return aIntervalTime;
    }
