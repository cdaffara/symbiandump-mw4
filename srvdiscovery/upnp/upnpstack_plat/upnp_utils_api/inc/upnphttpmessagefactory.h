/** @file
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies  this distribution, and is available 
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Declares HttpMessageFactory class.
*
*/


#ifndef C_RUPNPHTTPMESSAGEFACTORY_H
#define C_RUPNPHTTPMESSAGEFACTORY_H

// INCLUDES
#include <e32base.h>
#include "upnphttpmessage.h"


// ================= MEMBER FUNCTIONS =======================
/*! 
  \brief HTTP Message Factory. Used to create standard HTTP messages.
 
  */

class RUpnpHttpMessageFactory
	{
public:

	/**
	Constructor function; Creates HTTP get message, which is used normally to get content from remote servers.
	@param aDestination Destination of the message.
	@param aPath path to file in Http server.
	@return new Http message.
	**/
	IMPORT_C static CUpnpHttpMessage* HttpGetL(TInetAddr& aDestination, const TDesC8& aPath);

	/**
	Constructor function; Creates HTTP get message, which is used normally to get content from remote servers.
	@param aDestination Destination of the message.
	@param aHeadPath Start path to file in Http server.
	@param aTailPath End path to file in Http server.
	@return new Http message.
	**/
	IMPORT_C static CUpnpHttpMessage* HttpGetL(TInetAddr& aDestination, const TDesC8& aHeadPath, const TDesC8& aTailPath);

	/**
	Constructor function; Creates HTTP get message, which is used normally to get content from remote servers.
	@param aUrl Url to file.
	@return new Http message.
	**/
	IMPORT_C static CUpnpHttpMessage* HttpGetL(const TDesC8& aUrl);

	/**
	Constructor function; Creates HTTP get message, which is used normally to get content from remote servers.
	@param aDestination Destination of the message.
	@param aPath path to file in Http server.
	@return new Http message.
	**/
	IMPORT_C static CUpnpHttpMessage* HttpGetL(const TDesC8& aDestination, const TDesC8& aPath);
	
	/**
	Constructor function; Creates HTTP HEAD message.
	@param aUrl Url to file/target/service.
	@return new Http message.
	**/
	IMPORT_C static CUpnpHttpMessage* HttpHeadL(const TDesC8& aUrl);

	/**
	Constructor function; Creates HTTP HEAD message.
	@param aDestination Destination of the message.
	@param aPath path to file in Http server.
	@return new Http message.
	**/
	IMPORT_C static CUpnpHttpMessage* HttpHeadL(const TDesC8& aDestination, const TDesC8& aPath);
	
	/**
	Constructor function; Creates HTTP HEAD message.
	@param aDestination Destination of the message.
	@param aPath path to file in Http server.
	@return new Http message.
	**/
	IMPORT_C static CUpnpHttpMessage* HttpHeadL( TInetAddr& aDestination, const TDesC8& aPath);

	/**
	Constructor function; Creates HTTP POST message.
	@param aUrl Url to file/target/service.
	@return new Http message.
	**/
	IMPORT_C static CUpnpHttpMessage* HttpPostL(const TDesC8& aUrl);

	/**
	Constructor function; Creates HTTP POST message.
	@param aDestination Destination of the message.
	@param aPath path to file in Http server.
	@return new Http message.
	**/
	IMPORT_C static CUpnpHttpMessage* HttpPostL(const TDesC8& aDestination, const TDesC8& aPath);

	/**
	Constructor function; Creates HTTP POST message.
	@param aDestination Destination of the message.
	@param aPath path to file in Http server.
	@return new Http message.
	**/
	IMPORT_C static CUpnpHttpMessage* HttpPostL(const TInetAddr& aDestination, const TDesC8& aPath);

	/**
	Creates a HTTP OK reply message, Code 200.
	@param aMessage Message in which should be replied.
	@return new Http message.
	**/
	IMPORT_C static CUpnpHttpMessage* HttpResponseOkL(CUpnpHttpMessage* aMessage);

	/**
	Creates a HTTP OK reply message, Code 200.
	@param aDestination Destination of the reply message.
	@return new Http message.
	**/
	IMPORT_C static CUpnpHttpMessage* HttpResponseOkL(const TInetAddr& aDestination);

	/**
	Creates a HTTP reply message.
	@param aMessage Message in which should be replied.
	@param aStatus Number of status code.
	@return new Http message.
	**/
	IMPORT_C static CUpnpHttpMessage* Http11ResponseL( CUpnpHttpMessage *aMessage, TInt aStatus );

	/**
	Creates a HTTP reply message.
	@param aDestination Destination of the reply message.
	@param aStatus Number of status code.
	@return new Http message.
	**/
	IMPORT_C static CUpnpHttpMessage* Http11ResponseL( const TInetAddr& aDestination, TInt aStatus );

	/**
	Creates a HTTP Error message.
	@param aMessage Message in which should be replied.
	@param aError Number of error code.
	@return new Http message.
	**/
	IMPORT_C static CUpnpHttpMessage* HttpResponseErrorL(CUpnpHttpMessage* aMessage, TInt aError);

	/**
	Creates a HTTP Error message.
	@param aAddr Destination of the error message.
	@param aError Number of error code.
	@return new Http message.
	**/
	IMPORT_C static CUpnpHttpMessage* HttpResponseErrorL(const TInetAddr& aAddr, TInt aError);

	/**
	Creates a HTTP Error message with UPnp error codes.
	@param aMessage Message in which should be replied.
	@param aError Number of error code.
	@return new Http message.
	**/
	IMPORT_C static CUpnpHttpMessage* UpnpResponseErrorL(CUpnpHttpMessage* aMessage, TUpnpErrorCode aError);

	/**
	Creates a HTTP Error message with UPnp error codes.
	@param aDestination Destination of the error message.
	@param aError Number of error code.
	@return new Http message.
	**/
	IMPORT_C static CUpnpHttpMessage* UpnpResponseErrorL(const TInetAddr& aDestination, TUpnpErrorCode aError);
	
private:
	
	/**
	Sets Host HTTP header to given message basing on given destinstion address.
	@param aMessage Message for which the Host header will be set.
	@param aDestination Destination address.
	**/
	static void SetHostHeaderL( CUpnpHttpMessage* aMessage, const TInetAddr& aDestination );
};


#endif // C_RUPNPHTTPMESSAGEFACTORY_H