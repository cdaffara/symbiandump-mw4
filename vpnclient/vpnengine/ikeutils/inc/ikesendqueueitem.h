/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  IKE send queue item
*
*/


#ifndef T_IKESENDQUEUEITEM_H
#define T_IKESENDQUEUEITEM_H

// INCLUDES
#include <in_sock.h>

/**
 *  Item in send queue
 * 
 *  Item contains information needed to send a UDP data. Items are stored
 *  in send queue while they are waiting for sending. 
 *
 *  @lib ikeutils.lib
 */
class TIkeSendQueueItem
	{
public:
	
    /**
     * Constructor.
     *
     * @param aUdpData UDP data
     * @param aDestAddr Destination IP address
     * @param aLocalPort Local port
     * @param aDscp DSCP value
     */
    IMPORT_C TIkeSendQueueItem( HBufC8* aUdpData, 
                                const TInetAddr& aDestAddr, 
                                TInt aLocalPort,
                                TUint8 aDscp );		
	
	/**
	 * Constructor with message identification parameter. Constructs object
	 * which should be used for matching with Match() method.
     * @param aUdpData UDP data
	 */
    IMPORT_C TIkeSendQueueItem( HBufC8* aUdpData );	

    /**
     * Determines whether two objects match. Items match if their
     * message identification is same.
     * @param aItemOne First item
     * @param aItemTwo Second item
     * @return Match or not
     */
    IMPORT_C static TBool Match( const TIkeSendQueueItem& aItemOne,
                                 const TIkeSendQueueItem& aItemTwo );
	
    /**
     * Gets UDP data.
     * @return UDP data.
     */
    IMPORT_C HBufC8* UdpData();		

    /**
     * Gets destination IP address.
     * @return Destination IP address
     */
    IMPORT_C const TInetAddr& DestAddr();								

    /**
     * Gets local port.
     * @return Local port.
     */
    IMPORT_C TInt LocalPort() const;    

    /**
     * Gets DSCP value.
     * @return DSCP value.
     */
    IMPORT_C TUint8 Dscp() const;    
    
private:

	/**
	 * UDP data.
	 * Own.
	 */	
    HBufC8*        iUdpData;
		
	/**
	 * Destination IP address
	 * Own.
	 */	
    TInetAddr      iDestAddr;
	
    /**
     * Local port.
     * Own.
     */ 
    TInt           iLocalPort;
    
    /**
     * DSCP value.
     * Own.
     */ 
    TUint8         iDscp;              
	};


#endif // T_IKESENDQUEUEITEM_H
