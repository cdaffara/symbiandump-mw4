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
* Description:   ?Description
*
*/



#include "uiacceltk/huimetric.h"  // Class definition
#include "uiacceltk/HuiUtil.h"

EXPORT_C THuiMetric::THuiMetric(TReal32 aMagnitude, TInt aUnit, TInt aReferenceTextStyleId) __SOFTFP
    : iMagnitude(aMagnitude),
      iUnit(aUnit),
      iReferenceTextStyleId(aReferenceTextStyleId)
    {    
    }

EXPORT_C THuiMetric::THuiMetric(TInt aPixels)
    : iMagnitude(aPixels),
      iUnit(EHuiUnitPixel),
      iReferenceTextStyleId(EHuiTextStyleNormal)
    {    
    }

EXPORT_C THuiXYMetric::THuiXYMetric()
    {
    }
    
EXPORT_C THuiXYMetric::THuiXYMetric(const THuiMetric& aMetric)
    : iX(aMetric),
      iY(aMetric)
    {
    }
        
EXPORT_C THuiXYMetric::THuiXYMetric(const THuiMetric& aX,  const THuiMetric& aY)
    : iX(aX),
      iY(aY)
    {   
    }

EXPORT_C THuiBoxMetric::THuiBoxMetric()
    {
    }

EXPORT_C THuiBoxMetric::THuiBoxMetric(const THuiMetric& aMetric)
        : iLeft(aMetric), 
          iRight(aMetric),
          iTop(aMetric),
          iBottom(aMetric)
    {
    }

EXPORT_C THuiBoxMetric::THuiBoxMetric(const THuiXYMetric& aMetric)
        : iLeft(aMetric.iX),
          iRight(aMetric.iX),
          iTop(aMetric.iY),
          iBottom(aMetric.iY)
    {
    }

EXPORT_C THuiBoxMetric::THuiBoxMetric(const THuiXYMetric& aTopLeft, 
        const THuiXYMetric& aBottomRight)
        : iLeft(aTopLeft.iX),
          iRight(aBottomRight.iX),
          iTop(aTopLeft.iY),
          iBottom(aBottomRight.iY)
    {
    }

EXPORT_C THuiBoxMetric::THuiBoxMetric(const THuiMetric& aLeft, const THuiMetric& aRight,
        const THuiMetric& aTop, const THuiMetric& aBottom)
    : iLeft(aLeft), 
      iRight(aRight), 
      iTop(aTop), 
      iBottom(aBottom)
    {
    }
        
