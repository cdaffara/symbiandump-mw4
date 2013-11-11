// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//


#ifndef __PNPCONSTANTS_H__
#define __PNPCONSTANTS_H__

/**
This is the Uid which is used to load the underlying pnp interface. New plugins which make use of the
pnp interface should make use of this as the interface id of their implementations in the rss file of the plugin
@publishedPartner
@prototype
*/
const TUid  KPnPInterfaceId = {0x2000D060};

//Error strings
_LIT( KPnPHandleNotOpen , "PnP Handle Not Open");
_LIT( KControlChannelHandleNotOpen , "Control Channel Handle Not Open");


//Panic codes
const TInt KErrPnPNotOpen = -2525 ;
const TInt KErrControlChannelNotOpen = -2526;

#endif // __PNPCONSTANTS_H__ 