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



#ifndef HUIFXVG10BEVELFILTER_H_
#define HUIFXVG10BEVELFILTER_H_

#include "HuiFxVg10FilterBase.h"

class CHuiFxVg10BevelFilter : public CHuiFxVg10FilterBase
    {
public:
    static CHuiFxVg10BevelFilter* NewL();
    CHuiFxVg10BevelFilter *CloneL() const;
protected:
    void ConstructL();
    void DrawEffect(CHuiFxEngine& aEngine, VGImage aTargetImage, VGImage aSourceImage, TInt aWidth, TInt aHeight, TBool aHasSurface);
private:
    TReal32             iOutlineX;
    TReal32             iOutlineY;
    TReal32             iSteepness;
/*    TReal32             iRed;
    TReal32             iGreen;
    TReal32             iBlue;
*/    };

#endif /*HUIFXVG10BEVELFILTER_H_*/
