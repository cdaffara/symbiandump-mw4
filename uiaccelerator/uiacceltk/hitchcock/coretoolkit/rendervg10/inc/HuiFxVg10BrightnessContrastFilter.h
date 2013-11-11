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



#ifndef HUIFXVG10BRIGHTNESSCONTRASTFILTER_H_
#define HUIFXVG10BRIGHTNESSCONTRASTFILTER_H_

#include "HuiFxVg10ColorMatrixFilterBase.h"

class CHuiFxVg10BrightnessContrastFilter: public CHuiFxVg10ColorMatrixFilterBase
    {
public:
    static CHuiFxVg10BrightnessContrastFilter* NewL();
    CHuiFxVg10BrightnessContrastFilter *CloneL() const;
protected:
    void ConstructL();
    void UpdateColorMatrix(void);    
private:
    TReal32 iBrightness;    // from -1.0f to 1.0f 
    TReal32 iContrast;      // from 0.0f to N
    };

#endif /*HUIFXVG10BRIGHTNESSCONTRASTFILTER_H_*/
