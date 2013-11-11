/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
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

/* Automatically generated from Widget.cpp using create_hash_table. DO NOT EDIT ! */

#include "lookup.h"

namespace KJS {

static const struct HashEntry WidgetTableEntries[] = {
   { "setNavigationType", JSWidget::setNavigationType, DontDelete|Function, 1, 0 }/* 1921159715 */ ,
   { "openURL", JSWidget::openURL, DontDelete|Function, 1, &WidgetTableEntries[17] }/* 120566279 */ ,
   { "performTransition", JSWidget::performTransition, DontDelete|Function, 0, &WidgetTableEntries[21] }/* 3947827008 */ ,
   { 0, 0, 0, 0, 0 },
   { "onremove", JSWidget::onremove, DontDelete|ReadOnly, 0, 0 }/* 853904071 */ ,
   { 0, 0, 0, 0, 0 },
   { 0, 0, 0, 0, 0 },
   { 0, 0, 0, 0, 0 },
   { 0, 0, 0, 0, 0 },
   { "onshow", JSWidget::onshow, DontDelete|ReadOnly, 0, &WidgetTableEntries[19] }/* 2917788702 */ ,
   { "identifier", JSWidget::identifier, DontDelete|ReadOnly, 0, &WidgetTableEntries[23] }/* 4053269040 */ ,
   { 0, 0, 0, 0, 0 },
   { "onhide", JSWidget::onhide, DontDelete|ReadOnly, 0, 0 }/* 236865857 */ ,
   { "setPreferenceForKey", JSWidget::setPreferenceForKey, DontDelete|Function, 2, 0 }/* 1304460605 */ ,
   { "openApplication", JSWidget::openApplication, DontDelete|Function, 1, &WidgetTableEntries[22] }/* 78708705 */ ,
   { 0, 0, 0, 0, 0 },
   { "prepareForTransition", JSWidget::prepareForTransition, DontDelete|Function, 1, 0 }/* 3355888042 */ ,
   { "preferenceForKey", JSWidget::preferenceForKey, DontDelete|Function, 1, &WidgetTableEntries[18] }/* 2710796932 */ ,
   { "setNavigationEnabled", JSWidget::setNavigationEnabled, DontDelete|Function, 1, &WidgetTableEntries[20] }/* 2136509817 */ ,
   { "onexit", JSWidget::onexit, DontDelete|ReadOnly, 0, 0 }/* 3671980544 */ ,
   { "setDisplayLandscape", JSWidget::setDisplayLandscape, DontDelete|Function, 0, 0 }/* 1431294652 */ ,
   { "setDisplayPortrait", JSWidget::setDisplayPortrait, DontDelete|Function, 0, 0 }/* 4273345218 */ ,
   { "isrotationsupported", JSWidget::isRotationSupported, DontDelete|ReadOnly, 0, 0 }/* 3247979316 */ ,
   { "wrt", JSWidget::wrt, DontDelete|ReadOnly, 0, 0 }/* 1862342003 */ 
};

const struct HashTable WidgetTable = { 2, 24, WidgetTableEntries, 17 };

} // namespace
