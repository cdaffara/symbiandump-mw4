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


#ifndef __HWATRANSEFFECTUIDS__
#define __HWATRANSEFFECTUIDS__

#include <e32std.h>

enum HWATRANSEFFECTCOMMANDS
    {
    // for convenience, introduce these in same order (and ordinals as client defs)
    EFxCmdApplicationStarting,
    EFxCmdApplicationClosing,
    EFxCmdApplicationActivated,
    EFxCmdApplicationDeactivated,
    EFxCmdApplicationEmbeddedStarting,
    EFxCmdApplicationEmbeddedClosing
 
 
    ETfxCmdRegisterControl = 100,
    ETfxCmdRegisterVisual
    // ...
    
    };