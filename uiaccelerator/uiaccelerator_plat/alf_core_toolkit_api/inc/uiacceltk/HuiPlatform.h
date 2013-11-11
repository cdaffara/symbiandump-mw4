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
* Description:   Platform specific #definitions for the toolkit.
*
*/

 

#ifndef __HUIPLATFORM_H__
#define __HUIPLATFORM_H__

#if !defined(SERIES_80) && !defined(SERIES_60)
#define SERIES_60           // S60 is the default.
#endif

/*
 * Series 80 definitions.
 */
#ifdef SERIES_80

#include <coecntrl.h>
#include <eikapp.h>
#include <eikdoc.h>
#include <eikappui.h>
#include <eikon.hrh>

#define C_HUI_APPLICATION   CEikApplication
#define C_HUI_DOCUMENT      CEikDocument
#define C_HUI_APP_UI        CEikAppUi
#define C_HUI_CONTROL       CCoeControl

#endif

/*
 * Series 90 definitions.
 */
#ifdef SERIES_90

#include <eikbctrl.h>
#include <eikapp.h>
#include <eikdoc.h>
#include <eikappui.h>
#include <eikon.hrh>

#define C_HUI_APPLICATION   CEikApplication
#define C_HUI_DOCUMENT      CEikDocument
#define C_HUI_APP_UI        CEikAppUi
#define C_HUI_CONTROL       CEikBorderedControl

#endif

/*
 * Series 60 definitions.
 */
#ifdef SERIES_60

#include <coecntrl.h>
#include <aknapp.h>
#include <AknDoc.h>
#include <aknappui.h>
#include <avkon.hrh>

#define C_HUI_APPLICATION   CAknApplication
#define C_HUI_DOCUMENT      CAknDocument
#define C_HUI_APP_UI        CAknAppUi
#define C_HUI_CONTROL       CCoeControl

#endif


#endif
