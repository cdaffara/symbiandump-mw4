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



#include "alf/alfmetric.h"  // Class definition

EXPORT_C TAlfMetric::TAlfMetric(TReal32 aMagnitude, TInt aUnit, TInt aReferenceTextStyleId) __SOFTFP
    :
    iMagnitude(aMagnitude),
    iUnit(aUnit),
    iReferenceTextStyleId(aReferenceTextStyleId)
    {
    }

EXPORT_C TAlfMetric::TAlfMetric(TInt aPixels)
    : 
    iMagnitude(aPixels),
    iUnit(EAlfUnitPixel),
    iReferenceTextStyleId(EAlfTextStyleNormal)
    {    
    }

EXPORT_C TAlfXYMetric::TAlfXYMetric()
    {
    }
    
EXPORT_C TAlfXYMetric::TAlfXYMetric(const TAlfMetric& aMetric)
    : iX(aMetric),
      iY(aMetric)
    {
    }
        
EXPORT_C TAlfXYMetric::TAlfXYMetric(const TAlfMetric& aX,  const TAlfMetric& aY)
    :
    iX(aX),
    iY(aY)
    {
    
    }

EXPORT_C TAlfBoxMetric::TAlfBoxMetric()
    {
    }

EXPORT_C TAlfBoxMetric::TAlfBoxMetric(const TAlfMetric& aMetric)
        : iLeft(aMetric), 
          iRight(aMetric),
          iTop(aMetric),
          iBottom(aMetric)
    {
    }

EXPORT_C TAlfBoxMetric::TAlfBoxMetric(const TAlfXYMetric& aMetric)
        : iLeft(aMetric.iX),
          iRight(aMetric.iX),
          iTop(aMetric.iY),
          iBottom(aMetric.iY)
    {
    }

EXPORT_C TAlfBoxMetric::TAlfBoxMetric(const TAlfXYMetric& aTopLeft, 
        const TAlfXYMetric& aBottomRight)
        : iLeft(aTopLeft.iX),
          iRight(aBottomRight.iX),
          iTop(aTopLeft.iY),
          iBottom(aBottomRight.iY)
    {        
    }


EXPORT_C TAlfBoxMetric::TAlfBoxMetric(
    const TAlfMetric& aLeft, 
    const TAlfMetric& aRight,
    const TAlfMetric& aTop, 
    const TAlfMetric& aBottom)
    : 
    iLeft(aLeft), 
    iRight(aRight), 
    iTop(aTop), 
    iBottom(aBottom)
    {
    }
        
