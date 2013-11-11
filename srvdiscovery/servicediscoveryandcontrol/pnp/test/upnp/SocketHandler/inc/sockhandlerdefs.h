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


#ifndef __SOCKHANDLERDEFS_H__
#define __SOCKHANDLERDEFS_H__


namespace SockHandler
{	
// @internalAll

	enum TOperation
		{
		EConnect =0,
		EAccept =1,
		EDescSend =2,
		EMBufSend =3,
		EDescSendTo =4,
		EMBufSendTo =5,
		ERecv =6,
		ERecvFrom =7,  
		EIoctl =8, 				// unused enums 9 - 11 ( inclusive )
		EOpenByProtocol =12,  	// Used by CAsyncSocketOpener
		EOpenByName =13	 		// Used by CAsyncSocketOpener
		};

//@internalComponent
typedef TIpcArgs TSocketHandlerParams;
}

#endif // __SOCKHANDLERDEFS_H__
