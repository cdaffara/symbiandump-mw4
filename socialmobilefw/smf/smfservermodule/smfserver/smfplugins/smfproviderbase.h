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
 * The Provider Base class is the class that has to be contained as a member 
 * in all Plug-in Interfaces.
 *
 */

#ifndef SMFPROVIDERBASE_H_
#define SMFPROVIDERBASE_H_

#include <QImage>
#include <QUrl>
#include <QBuffer>
#include <smfglobal.h>
#include <qdatastream.h>
#include <QSharedData>

/**
 * @ingroup smf_plugin_group
 * The Provider Base class is the class that has to be contained as a member 
 * in all Plug-in Interfaces.
 * 
 * All of plug-ins should contain this class as a member and should also 
 * contain a public method to get instance of this class. 
 */
class SmfProviderBase
	{
public:
	/**
	 * Destructor
	 */
	virtual ~SmfProviderBase( ) {}

	/**
	 * Method to get the Localisable name of the service.
	 * @return The Localisable name of the service.
	 */
	virtual QString serviceName( ) const = 0;
	
	/**
	 * Method to get the Logo of the service
	 * @return The Logo of the service
	 */
	virtual QImage serviceIcon( ) const = 0;
	
	/**
	 * Method to get the Readable service description
	 * @return The Readable service description
	 */
	virtual QString description( ) const = 0;
	
	/**
	 * Method to get the Website of the service
	 * @return The Website of the service
	 */
	virtual QUrl serviceUrl( ) const = 0;
	
	/**
	 * Method to get the URL of the Application providing this service
	 * @return The URL of the Application providing this service
	 */
	virtual QUrl applicationUrl( ) const = 0;
	
	/**
	 * Method to get the Icon of the application
	 * @return The Icon of the application
	 */
	virtual QImage applicationIcon( ) const = 0;
	
	
	/**
	* Method to get the list of interfaces that this provider support
	* @return List of supported Interafces
	*/
	virtual QList<QString> supportedInterfaces( ) const = 0;
	
	/**
	* Method to get the list of languages supported by this service provider
	* @return a QStringList of languages supported by this service 
	* provider in 2 letter ISO 639-1 format.
	*/
	virtual QStringList supportedLanguages( ) const = 0;
	
	
	/**
	 * Method to get the Plugin specific ID
	 * @return The Plugin specific ID
	 */
	virtual QString pluginId( ) const = 0;
	
	/**
	 * Method to get the ID of the authentication application 
	 * for this service
	 * @param aProgram The authentication application name
	 * @param aArguments List of arguments required for authentication app
	 * @param aMode Strting mode for authentication application
	 * @return The ID of the authentication application 
	 */
	virtual QString authenticationApp( QString &aProgram, QStringList & aArguments, 
			QIODevice::OpenModeFlag aMode = QIODevice::ReadWrite ) const = 0;
	
	/**
	 * Method to get the unique registration ID provided by the 
	 * Smf for authorised plugins
	 * @return The unique registration ID/token provided by the Smf for 
	 * authorised plugins
	 */
	virtual QString smfRegistrationId( ) const = 0;
	};

Q_DECLARE_INTERFACE( SmfProviderBase, "org.symbian.smf.plugin.smfproviderbase/v0.2" );

#endif /* SMFPROVIDERBASE_H_ */
