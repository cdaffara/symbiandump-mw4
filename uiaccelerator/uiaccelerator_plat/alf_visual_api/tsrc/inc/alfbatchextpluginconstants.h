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


#ifndef ALFBATCHEXTPLUGINCONSTANTS_H
#define ALFBATCHEXTPLUGINCONSTANTS_H

#include <alf/alftimedvalue.h>

const TInt KAlfBatchExtPluginImplementationId = 0x0DDDDDDE;

// All the components in this factory
enum 
    {
    EAlfBatchExtPluginCreateExtension
    };

// Commands for this plugin
enum
    {
    EAlfBatchExtensionCmdReset,
    EAlfBatchExtensionCmdGetCount,
    EAlfBatchExtensionCmdGetIndex,
    EAlfBatchExtensionCmdInteger,
    EAlfBatchExtensionCmdReal,
    EAlfBatchExtensionCmdBufferMin,
    EAlfBatchExtensionCmdBufferMax = EAlfBatchExtensionCmdBufferMin + 2048,
    EAlfBatchExtensionCmdNotSupported = 0xFFFF
    };

#endif // ALFBATCHEXTPLUGINCONSTANTS_H
