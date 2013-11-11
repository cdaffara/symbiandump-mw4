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
* Description:   Panic header for Alfred
*
*/



#ifndef C_ALFPANIC_H
#define C_ALFPANIC_H

#include <e32def.h>
#include "alfpanicreasons.h"

GLREF_C void Panic(TAlfPanic aPanic);

#endif // C_ALFPANIC_H
