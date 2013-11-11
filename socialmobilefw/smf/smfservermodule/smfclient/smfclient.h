/**
 * Copyright (c) 2010 Sasken Communication Technologies Ltd.
 * All rights reserved.
 * This component and the accompanying materials are made available
 * under the terms of the "Eclipse Public License v1.0" 
 * which accompanies  this distribution, and is available
 * at the URL "http://www.eclipse.org/legal/epl-v10.html"
 *
 * Initial Contributors:
 * Chandradeep Gandhi, Sasken Communication Technologies Ltd - Initial contribution
 *
 * Contributors:
 * Manasij Roy, Nalina Hariharan
 * 
 * Description:
 * The SmfClient class provides a set of APIs to get list of service provider
 * who implement the given interface. Calling GetServices of the instance of this
 * class is mandatory before using any other interface.
 *
 */

#ifndef SMFCLIENT_H
#define SMFCLIENT_H

#include <QObject>

#include "smfclientglobal.h"
#include "smfglobal.h"

//Forward declarations
class SmfProvider;
class SmfClientPrivate;

/**
 * @ingroup smf_client_group 
 * Interface for a base service provider. Other service provider classes contains 
 * implementation of this base class so that each has access to service provider 
 * information.
 * All of the functionality described here should be implemented by a service
 * specific plug-in object.
 */
class  SMFCLIENT_EXPORT SmfClient
	{
public:
	/**
	 * Constructs SmfClient
	 */
	SmfClient();
	
	/**
	 * Destructor
	 */
	~SmfClient();
	
	/**
	 * Method to get the list of services for a given service (interface name)
	 * @param serviceName The name of the service 
	 * @return Service provider's list (ownership of memory is transferred 
	 * to the calling party)
	 */
	QList<SmfProvider>* GetServices(const QString& serviceName);
	
	// ToDo :- More overloaded APIs will be provided later
	//QList<SmfProvider>* GetServices(const QString& serviceName, const QString& providerName);
	//QList<SmfProvider>* GetServices(const QString& serviceName, const SmfProvider& provider);
  
	/**
	 * This method returns the error message for the mentioned argument error code
	 * @param aErrorCode The SmfError code for which the error string is required
	 * @returns a QString object containing the required error message
	 */
	QString errorString ( const SmfError &aErrorCode ) const;

private:
	/**
	 * Private impl wrapper.
	 */
	SmfClientPrivate* m_private;
	
	/**
	 * List of SmfProvider to be returned to the requested code
	 */
	QList<SmfProvider>* m_providerList;
  
	};

#endif // SMFCLIENT_H
