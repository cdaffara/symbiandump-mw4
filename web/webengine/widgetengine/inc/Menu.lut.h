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

/* Automatically generated from Menu.cpp using create_hash_table. DO NOT EDIT ! */

#include "lookup.h"

namespace KJS {

static const struct HashEntry MenuTableEntries[] = {
   { "remove", JSMenu::Remove, DontDelete|Function, 1, &MenuTableEntries[12] }/* 4093363626 */ ,
   { "getMenuItemByName", JSMenu::GetMenuItemByName, DontDelete|Function, 1, &MenuTableEntries[11] }/* 4200074033 */ ,
   { "setRightSoftkeyLabel", JSMenu::SetRightSoftKeyLabel, DontDelete|Function, 2, &MenuTableEntries[14] }/* 553073479 */ ,
   { 0, 0, 0, 0, 0 },
   { 0, 0, 0, 0, 0 },
   { "location", JSMenu::Location, DontDelete|ReadOnly, 0, 0 }/* 290291248 */ ,
   { 0, 0, 0, 0, 0 },
   { "onShow", JSMenu::OnShow, DontDelete|ReadOnly, 0, 0 }/* 4016841220 */ ,
   { "append", JSMenu::Append, DontDelete|Function, 1, 0 }/* 113617182 */ ,
   { "setLeftSoftkeyLabel", JSMenu::SetLeftSoftKeyLabel, DontDelete|Function, 2, 0 }/* 174051557 */ ,
   { "getMenuItemById", JSMenu::GetMenuItemById, DontDelete|Function, 1, &MenuTableEntries[13] }/* 599604224 */ ,
   { "clear", JSMenu::Clear, DontDelete|Function, 0, 0 }/* 3385053211 */ ,
   { "toString", JSMenu::ToString, DontDelete|Function, 0, 0 }/* 150235932 */ ,
   { "hideSoftkeys", JSMenu::HideSoftkeys, DontDelete|Function, 0, 0 }/* 2183114163 */ ,
   { "showSoftkeys", JSMenu::ShowSoftkeys, DontDelete|Function, 0, 0 }/* 4150746789 */ 
};

const struct HashTable MenuTable = { 2, 15, MenuTableEntries, 11 };

} // namespace
