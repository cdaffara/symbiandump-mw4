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



#ifndef HUIFXPARAMETERCOLLECTION_H_
#define HUIFXPARAMETERCOLLECTION_H_

#include <e32std.h>

class MHuiFxParameter;

class MHuiFxParameterCollection
    {
public:
    // virtual RParameterMap* Parameters() = 0 const;
    virtual MHuiFxParameter* Parameter(const TDesC& aName) = 0;
    // does not take ownership
    //virtual void RegisterParameterL(const TDesC& aName, TRgb* aColor) = 0;
    //virtual void RegisterParameterL(const TDesC& aName, TReal32* aScalar) = 0;
    };

#endif /*HUIFXPARAMETERCOLLECTION_H_*/
