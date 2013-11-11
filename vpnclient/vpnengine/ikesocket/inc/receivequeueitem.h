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
* Description:  Receive queue item
*
*/


#ifndef T_RECEIVEQUEUEITEM_H
#define T_RECEIVEQUEUEITEM_H

#include <in_sock.h>

/**
 *  Item in receive queue
 * 
 *  Item contains information related to received UDP data. Items are stored
 *  in receive queue while they are waiting to be read by the client. 
 *
 *  @lib ikesocket.lib
 */
class TReceiveQueueItem
	{
public:
	
    /**
     * Constructor.
     *
     * @param aUdpData UDP data
     * @param aSrcAddr Source IP address/port
     * @param aLocalPort Local port
     */
	TReceiveQueueItem( HBufC8* aUdpData, 
					   const TInetAddr& aSrcAddr,
					   const TInt aLocalPort );	
	
    /**
     * Returns UDP data.
     *
     * @return Udp data
     */
	HBufC8* UdpData() const;	
	
    /**
     * Returns source address.
     *
     * @return Source address
     */
	const TInetAddr& SrcAddr() const;		

    /**
     * Returns local port.
     *
     * @return Local port
     */
	TInt LocalPort() const;	

private:

	/**
	 * Udp data
	 * Own.
	 */	
	HBufC8*        iUdpData;
	
	/**
	 * Source address
	 * Own.
	 */	
	TInetAddr      iSrcAddr;
	
	/**
	 * Local port
	 * Own.
	 */	
	TInt           iLocalPort; 		

	};


#endif // T_RECEIVEQUEUEITEM_H
