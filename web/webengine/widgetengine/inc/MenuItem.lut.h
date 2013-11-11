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

/* Automatically generated from MenuItem.cpp using create_hash_table. DO NOT EDIT ! */

#include "lookup.h"

namespace KJS {

static const struct HashEntry TMenuItemTableEntries[] = {
   { 0, 0, 0, 0, 0 },
   { "remove", JSMenuItem::Remove, DontDelete|Function, 1, 0 }/* 4093363626 */ ,
   { "append", JSMenuItem::Append, DontDelete|Function, 1, &TMenuItemTableEntries[5] }/* 113617182 */ ,
   { 0, 0, 0, 0, 0 },
   { "onSelect", JSMenuItem::OnSelect, DontDelete|ReadOnly, 0, 0 }/* 1371654329 */ ,
   { "setDimmed", JSMenuItem::SetDim, DontDelete|Function, 1, &TMenuItemTableEntries[6] }/* 721647082 */ ,
   { "toString", JSMenuItem::ToString, DontDelete|Function, 0, 0 }/* 150235932 */ 
};

const struct HashTable TMenuItemTable = { 2, 7, TMenuItemTableEntries, 5 };

} // namespace
