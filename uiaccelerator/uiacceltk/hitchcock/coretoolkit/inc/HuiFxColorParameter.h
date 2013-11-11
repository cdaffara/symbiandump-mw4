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



#ifndef HUIFXCOLORPARAMETER_H_
#define HUIFXCOLORPARAMETER_H_

#include "HuiFxParameter.h"
#include "HuiFxTimeLine.h"

//FORWARD DECLARATIONS
class TRgb;

/**
 *  Animation timeline for color values
 */
typedef RHuiFxTimeLine<TRgb, TUint32> RHuiFxColorTimeLine;

/*
 *  Color parameter
 */
typedef CHuiFxGenericParameter<TRgb, RHuiFxColorTimeLine, EParameterTypeColor> CHuiFxColorParameter;

#endif /*HUIFXCOLORPARAMETER_H_*/
