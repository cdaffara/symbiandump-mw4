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



#ifndef HUIFXVG10HSLFILTER_H_
#define HUIFXVG10HSLFILTER_H_

#include "HuiFxVg10ColorMatrixFilterBase.h"

class CHuiFxVg10HSLFilter: public CHuiFxVg10ColorMatrixFilterBase
    {
public:
    static CHuiFxVg10HSLFilter* NewL();
    CHuiFxVg10HSLFilter *CloneL() const;
protected:
    void ConstructL();
    void UpdateColorMatrix(void);
private:
    VGfloat iHue;           // from 0.0f to 360.f   neutral: 0.0f
    VGfloat iSaturation;    // from 0.0f to N       neutral: 1.0f
    VGfloat iLightness;     // from -1.0f to 1.0f   neutral: 0.0f
    };

#endif /*HUIFXVG10HSLFILTER_H_*/
