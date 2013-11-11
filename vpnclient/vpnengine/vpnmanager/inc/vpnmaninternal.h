/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: VPN Manager internal definitions.
*
*/



#ifndef __VPNMANINTERNAL_H__
#define __VPNMANINTERNAL_H__

#define FIRST_ARGUMENT 0
#define SECOND_ARGUMENT 1
#define THIRD_ARGUMENT 2
#define FOURTH_ARGUMENT 3

enum TNextAction
    {
    ENoAction,
    EActivateAndLoad,
    EDeactivateAndUnload
    };



#endif  // __VPNMANINTERNAL_H__