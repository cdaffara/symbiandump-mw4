/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  IKE plugin definitions
*
*/


#ifndef IKEPLUGINDEFS_H
#define IKEPLUGINDEFS_H

#include <e32uid.h>

typedef TUid TIkePluginId;

const TUid KIkePluginUid1 = { KDynamicLibraryUidValue };
const TUid KIkePluginUid2 = { KSharedLibraryUidValue };

const TUid KIkeV1PluginUid3 = { 0x10206994 };  
const TUid KIkeV2PluginUid3 = { 0x10206993 };

const TInt KIkeV1( 1 );
const TInt KIkeV2( 2 );
const TInt KIkePluginMaxCount( 2 );

#endif // IKEPLUGINDEFS_H
