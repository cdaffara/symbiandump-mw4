/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Master header file for applications. Includes the entire 
*                public API of the Hitchcock User Interface Toolkit.
*
*/



/**
 * @mainpage Hitchcock User Interface Toolkit
 *
 * @section sec1 Introduction
 *
 * The Hitchcock User Interface Toolkit (HUITK) is a library of classes that
 * can be used for creating graphical user interfaces that are primarily 
 * two-dimensional, but can utilize three-dimensional objects and effects, for 
 * instance rotations. HUITK has been designed to be used with hardware-accelerated 
 * graphics, but limited functionality software-only rendering is also supported.
 * HUITK has been developed on Symbian OS 9.1.
 *
 * You are reading the HUITK API documentation. This is a reference manual 
 * of the HUITK API classes. For higher-level developer documentation, see
 * @ref sec_info.
 *
 * @section sec2 Typical Usage
 *
 * Initialization:
 *  - Create environment. Specify the renderer to use, e.g., EHuiRendererGles11.
 *  - Create a display. The most convenient way is to use CHuiDisplayCoeControl.
 *  - Create one or more control groups.
 *  - Create controls and append them to the groups.
 *  - Display one (or more) of the groups.
 *
 * Normal runtime behaviour:
 *  - Action and animation commands can be posted using the environment.
 *  - Controls handle input events in CHuiControl::OfferEventL().
 *  - Controls can manipulate visuals.
 *  - Groups can be shown, hidden, and transformed.
 *  - Display refresh is paused while nothing is happening. See 
 *    CHuiEnv::ContinueRefresh().
 *
 * Cleanup:
 *  - Delete CoeControls owned by the application (including 
 *    CHuiDisplayCoeControl instances).
 *  - Shut down by deleting the environment.
 *
 * @section sec_groups Classes
 *
 * The following classes form the public API of the toolkit. The API 
 * documentation also includes information about internal classes. Note that
 * exported methods are listed under an &quot;Exported API&quot; subsection
 * in each class's description.
 *
 * Core Functionality:
 * - CHuiEnv
 * - CHuiDisplay
 * - CHuiDisplayCoeControl
 * - CHuiControl
 * - CHuiControlGroup
 * - CHuiRoster
 * - CHuiStatic
 * - THuiTimedValue
 * - THuiTimedPoint
 * - THuiCommand (and derived classes such as THuiValueCommand)
 * - THuiEvent
 *
 * Utilities:
 * - HuiUtil (only static methods)
 * - MHuiMappingFunction implementations such as THuiLinearMappingFunction
 * - THuiRealPoint
 * - THuiRealRect
 * - THuiRealSize
 *
 * Graphics (note HuiDrawing.h):
 * - CHuiGc (available in CHuiVisual::Draw() methods)
 * - CHuiTransformation
 * - CHuiCurvePath
 * - CHuiMesh
 * - THuiMaterial
 * - CHuiS60Skin
 * - CHuiTextureManager
 * - CHuiTextureProcessor
 * - CHuiTexture
 * - CHuiAnimatedTexture
 * - THuiTextureHandle
 * - THuiImage (reference to a region of a texture)
 *
 * Visuals and Layouts (note HuiVisuals.h and HuiLayouts.h):
 * - CHuiVisual
 *     - CHuiImageVisual
 *     - CHuiLineVisual
 *     - CHuiMeshVisual
 *     - CHuiTextVisual
 * - CHuiLayout
 *     - CHuiAnchorLayout
 *     - CHuiFlowLayout
 *     - CHuiGridLayout
 *
 * Brushes (note HuiBrushes.h):
 * - CHuiBrushArray (see CHuiVisual::EnableBrushesL())
 * - CHuiBrush
 *     - CHuiBorderBrush
 *     - CHuiDropShadowBrush
 *     - CHuiGradientBrush
 *     - CHuiShadowBorderBrush
 * 
 * Common controls:
 * - CHuiSoftKeyControl 
 *
 * @section sec_info More Information
 *
 * More information is available in the Nokia internal Wiki pages:
 * http://nwiki.nokia.com/NSSProjects/HUIToolkit
 *
 *
 * Copyright &copy; 2006 Nokia.  All rights reserved.
 *
 * This material, including documentation and any related computer programs, 
 * is protected by copyright controlled by Nokia.  All rights are reserved.  
 * Copying, including reproducing, storing, adapting or translating, any or 
 * all of this material requires the prior written consent of Nokia.  This 
 * material also contains confidential information which may not be disclosed 
 * to others without the prior written consent of Nokia.
 */

#include <uiacceltk/HuiCommand.h>
#include <uiacceltk/HuiControl.h>
#include <uiacceltk/HuiControlGroup.h>
#include <uiacceltk/HuiRoster.h>
#include <uiacceltk/HuiDisplay.h>
#include <uiacceltk/HuiDisplayCoeControl.h>
#include <uiacceltk/HuiEnv.h>
#include <uiacceltk/HuiEvent.h>
#include <uiacceltk/HuiPanic.h>
#include <uiacceltk/HuiPlatform.h>
#include <uiacceltk/HuiScheduler.h>
#include <uiacceltk/HuiStatic.h>
#include <uiacceltk/HuiTimedPoint.h>
#include <uiacceltk/HuiTimedValue.h>
#include <uiacceltk/huieventhandler.h>

/* 
 * Utilities 
 */
#include <uiacceltk/HuiMappingFunctions.h>
#include <uiacceltk/HuiObserverArray.h>
#include <uiacceltk/HuiTimeObserver.h>
#include <uiacceltk/HuiOwnedPointer.h>
#include <uiacceltk/HuiInteractionInterval.h>
#include <uiacceltk/HuiRealPoint.h>
#include <uiacceltk/HuiRealRect.h>
#include <uiacceltk/HuiRealSize.h>
#include <uiacceltk/HuiUtil.h>
#include <uiacceltk/HuiImageLoaderUtil.h>

/* 
 * Graphics 
 */
#include <uiacceltk/HuiTransformation.h>
#include <uiacceltk/HuiTextureManager.h>
#include <uiacceltk/HuiTextureProcessor.h>
#include <uiacceltk/HuiTextureIf.h>
#include <uiacceltk/HuiSegmentedTexture.h>
#include <uiacceltk/HuiShadowedTexture.h>
#include <uiacceltk/HuiTexture.h>
#include <uiacceltk/HuiTextureHandle.h>
#include <uiacceltk/HuiCurvePath.h>
#include <uiacceltk/HuiGc.h>
#include <uiacceltk/HuiImage.h>
#include <uiacceltk/HuiMesh.h>
#include <uiacceltk/HuiMaterial.h>
#include <uiacceltk/HuiTextMesh.h>
#include <uiacceltk/HuiSkin.h>
#include <uiacceltk/HuiS60Skin.h>
#include <uiacceltk/HuiAnimatedTexture.h>

/* 
 * Visuals and Layouts 
 */
#include <uiacceltk/HuiDrawing.h>
#include <uiacceltk/HuiVisuals.h>
#include <uiacceltk/HuiLayouts.h>

/* 
 * Brushes 
 */
#include <uiacceltk/HuiBrushes.h>
#include <uiacceltk/HuiBrushArray.h>

/*
 * Common Controls 
 * @ deprecated
 *
 */
#include <uiacceltk/HuiSoftKeyControl.h>
