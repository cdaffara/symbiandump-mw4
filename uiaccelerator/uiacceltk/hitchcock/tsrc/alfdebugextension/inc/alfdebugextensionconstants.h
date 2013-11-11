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
* Description:  
*
*/


#ifndef ALFDEBUGEXTENSIONCONSTANTS_H
#define ALFDEBUGEXTENSIONCONSTANTS_H


// All the components in this factory
enum 
    {
    EAlfDebugExtensionCreateDebug
    };

// Commands for this plugin
enum
    {
    EAlfDebugCmdSetTimeFactor,
    EAlfDebugCmdGetTimeFactor,
    EAlfDebugCmdGetFrameCount,
    EAlfDebugCmdGetFrameRate,
    EAlfDebugCmdShowFrameRate,
    EAlfDebugCmdShowServerHeap,
    EAlfDebugCmdMeasure
    };
    
// constats for commands
const TInt KAlfDebugHideFraweRate               = 0;
const TInt KAlfDebugShowFraweRateAfterDrawLoop  = -1;
const TInt KAlfDebugShowFraweRateWhenQueried    = -2;

#endif // ALFDEBUGEXTENSIONCONSTANTS_H
