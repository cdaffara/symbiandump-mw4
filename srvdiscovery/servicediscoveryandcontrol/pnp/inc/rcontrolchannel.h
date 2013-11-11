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


#ifndef __RCONTROLCHANNEL_H__
#define __RCONTROLCHANNEL_H__

#include <e32base.h>

class CControlChannelBase;

/* 
 This class is used to wrap the TDes8 data that is used for transmission
 @publishedPartner
 @prototype
*/
class TControlMessage
	{
public: 
    /** Default constructor */
	IMPORT_C TControlMessage ();

    /** Default destructor */
	IMPORT_C ~TControlMessage ();
    /** 
	Used to Set the data part of control message with data supplied in aData
    @param aData Supplied data
    */
	IMPORT_C void SetMessageDes ( const TDesC8& aData );
    /** 
	Used to Set the data part of control message with data supplied in aData
    @param aData Supplied data
    */
	IMPORT_C void SetMessagePtr ( TDes8& aData );	
    /**
	Used to retrieves the data part of control message
    @return 8 bit non-modifiable descriptor 
    */
	IMPORT_C const TDesC8& MessageDes () const;
    /**
	Used to retrieves the data part of control message
    @return 8 bit modifiable descriptor 
    */
	IMPORT_C TDes8& MessagePtr ();	
    /**
	Used to tell whether the data is the last message in the control data or not
    @param True if the msg is the last message
    */
	IMPORT_C TBool IsLast() const;
    /**	Used to set the last message of the control data */
	IMPORT_C void SetLast();
    /**
	Used to set max length of Control Message
    @param aLen Max length of the msg
    */
	IMPORT_C void SetMaxLength ( TInt aLen );
    /** 
	Used to get max length of the Control Message
    @return Max length of the msg 
    */
	IMPORT_C TInt MaxLength () const;
    /** Used to reset the SetLast flag */
	IMPORT_C void ClearFlags ();
	
private:
	TPtr8 		iPtr;
	TInt 		iMaxLength;
	TUint	 	iFlags;
	};


/** 
 This class is used to send and receive control messages
 @publishedPartner
 @prototype
 */
class RControlChannel
	{
	friend class RPnPServiceDiscovery;
	friend class RPnPServicePublisher;
	
public: 
	
    /** default constructor */
	IMPORT_C RControlChannel ( );
    /**
	Used to receive control messages
    @param aMessage msg wrapped as TControlMessage 
    @param aStatus request status
    */
	IMPORT_C void Recv ( TControlMessage& aMessage, TRequestStatus& aStatus );
    /**
	Used to send control messages
    @param aMessage msg wrapped as TControlMessage 
    @param aStatus request status
    */
	IMPORT_C void Send ( TControlMessage& aMessage, TRequestStatus& aStatus );
    
	/** Used to close the channel */
	IMPORT_C void Close ();	
	
	/** Used to cancel the asynchronous recv */
	IMPORT_C void CancelRecv ();
	
	/** Used to cancel the asynchronous send */
	IMPORT_C void CancelSend ();

private:
    void Attach(CControlChannelBase& aChannelBase);	
    
private:
	CControlChannelBase* iControlChannelImpl;
	};

#endif //__RCONTROLCHANNEL_H__
