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
* Description:   UIAccelTK domain Central Repository keys.
*
*/




#ifndef UIACCELERATORTOOLKITDOMAINCRKEYS_H
#define UIACCELERATORTOOLKITDOMAINCRKEYS_H

//  INCLUDES
#include <e32std.h>

/** UIAccelTK Settings API */
/** Provides access to the UI accelerator toolkit settings */
const TUid KCRUidUIAccelTK = {0x10283344};

/**
 * Key for default framerate (frames per second).
 * Possible values: 0 - 999999 fps
 */
const TUint32 KUIAccelTKDefaultFrameRate = 0x00000000;

/**
 * Key for maximum CPU usage for alfredserver.exe in precentage.
 * Possible values: 0 - 100 % 
 */	
const TUint32 KUIAccelTKMaxCpuUsage = 0x00000001;

/**
 * Key for used renderer. See huirenderer.h for more details.
 * Possible values: 
 * 0 = Automatic selection
 * 1 = Symbian bitgdi 
 * 2 = OpenVG 1.0 (Not implemented)
 * 3 = OpenGL ES 1.0 
 * 4 = OpenGL ES 1.1
 */	
const TUint32 KUIAccelTKRenderer = 0x00000002;

/**
 * Key for maximum cache size (in kilobytes) allocated for resources.
 * Possible values: 0 - 999999 KB (0 = no limit)
 */	
const TUint32 KUIAccelTKMaxResourceCache = 0x00000003;

/**
 * Key for effect complexity.
 * Possible values: 1 - 10 (1 = simpliest, 10 = most complex) 
 *
 * The lowest 8 bits (decimal 0-255) are reserved for the complexity factor.
 * The rest are for RnD purposes:
 * Flag     Purpose
 * 0x0100   If set, the dirty regoins will be drawn. Not set by default.
 * 0x0200   If set, draws outline to all visuals
 * 0x0400   If set, draws outline to the visuals, whose EAlfVisualFlagEnableDebugMode flag is set
 * 0x0800   If set, enables memory usage calculation by textures. It writes info messages  
 *          describing memory consumption and total number of textures along with average bit depth
 */
const TUint32 KUIAccelTKEffectsComplexityFactor = 0x00000004;

/**
 * Key for setting up the HW driver.
 * Possible values: TBD 
 *
 * The lowest bit defines Antialising.
 * 0 = antialiasing off (fast rendering)
 * 1 = antialiasing on (multisampling etc.)
 */
const TUint32 KUIAccelTKHWConfigurationFlags = 0x00000005;

#endif      // UIACCELERATORTOOLKITDOMAINCRKEYS_H

// End of File
