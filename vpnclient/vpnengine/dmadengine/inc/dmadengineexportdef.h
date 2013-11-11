/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Device Management Adapter Engine export definitions.
*
*/



#ifndef __DMADENGINEEXPORTDEF_H__
#define __DMADENGINEEXPORTDEF_H__

#ifdef DMAD_ENGINE_EXPORT
    #define DMAD_EXPORT_C EXPORT_C
    #define DMAD_IMPORT_C IMPORT_C
#else
    #define DMAD_EXPORT_C
    #define DMAD_IMPORT_C
#endif

#endif
