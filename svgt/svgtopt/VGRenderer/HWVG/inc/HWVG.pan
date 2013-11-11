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


#ifndef __HWVG_PAN__
#define __HWVG_PAN__

//  Data Types

enum THWVGPanic
    {
    EHWVGNullPointer
    };

//  Function Prototypes

GLREF_C void Panic(THWVGPanic aPanic);

#endif  // __HWVG_PAN__

