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
 * Interface for a base service provider
 *
 */

#include "smfprovider.h"

/**
 * Constructor
 */
SmfProvider::SmfProvider()
	{
	
	}

/**
 * Destructor
 */
SmfProvider::~SmfProvider()
	{
	
	}

/**
 * Localizable name of the service 
 * @return service name
 */
QString SmfProvider::serviceName() const
	{
	return m_serviceName;
	}

/**
 * Logo of the service
 * @return logo image of the service
 */
QImage SmfProvider::serviceIcon() const
	{
	return m_serviceIcon;
	}

/**
 * Readable service description
 * @return service description
 */
QString SmfProvider::description() const
	{
	return m_description;
	}

/*
 * Website of the service
 * @return service url 
 */
QUrl SmfProvider::serviceUrl() const
	{
	return m_serviceUrl;
	}

/**
 * URL of the application providing this service
 * @return application url
 */
QUrl SmfProvider::applicationUrl() const  
	{
	return m_appUrl;
	}

/**
 * List of interfaces that this provider support
 * @return List of supported interface names
 */
QList<QString> SmfProvider::supportedInterfaces() const
	{
	return m_serviceTypes;
	}

/**
* List of languages supported by this service provider
* @return a QStringList of languages supported by this service 
* provider in 2 letter ISO 639-1 format.
*/
QStringList SmfProvider::supportedLanguages() const
	{
	return m_supportedLanguages;
	}

/**
 * Sets Localizable name of the service 
 * @param name service name
 */
void SmfProvider::setServiceName(QString& name)
	{
	m_serviceName = name;
	}

/**
 * Sets Logo of the service
 * @param image service icon
 */
void SmfProvider::setServiceIcon(QImage& image)
	{
	m_serviceIcon = image;
	}

/**
 * Sets Readable service description
 * @param desc description text
 */
void SmfProvider::setDescription(QString& desc)
	{
	m_description = desc;
	}

/*
 * Sets Website of the service
 * @param name service url
 */
void SmfProvider::setServiceUrl(QUrl& url)
	{
	m_serviceUrl = url;
	}

/**
 * Sets URL of the application providing this service
 * @param url application url
 */
void SmfProvider::setApplicationUrl(QUrl& url)
	{
	m_appUrl = url;
	}

/**
 * Sets list of interfaces that this provider supports
 * @param types List of supported interface names
 */
void SmfProvider::setSupportedInterfaces( QStringList& types)
	{
	m_serviceTypes = types;
	}


/**
* Sets the list of languages supported by this service provider
* @param lang A QStringList of languages supported by this service 
* provider in 2 letter ISO 639-1 format.
*/
void SmfProvider::setSupportedLanguages( QStringList& lang )
	{
	m_supportedLanguages = lang;
	}

/**
 * Method for Externalization. Writes the SmfProvider object to 
 * the stream and returns a reference to the stream.
 * @param out Stream to be written
 * @param base The SmfProvider object to be externalized
 * @return reference to the written stream
 */
QDataStream &operator<<(QDataStream& out, const SmfProvider& base)
	{
	out<<base.m_serviceName;
	out<<base.m_serviceIcon;
	out<<base.m_description;
	out<<base.m_serviceUrl;
	out<<base.m_appUrl;
	out<<base.m_serviceTypes;
	out<<base.m_supportedLanguages;
	return out; 
	}

/**
 * Method for Internalization. Reads a SmfProvider object from 
 * the stream and returns a reference to the stream.
 * @param in Stream to be read
 * @param base The SmfProvider object to be internalized
 * @return reference to the stream
 */
QDataStream &operator>>(QDataStream& in, SmfProvider& base)
	{
	in>>base.m_serviceName;
	in>>base.m_serviceIcon;
	in>>base.m_description;
	in>>base.m_serviceUrl;
	in>>base.m_appUrl;
	in>>base.m_serviceTypes;
	in>>base.m_supportedLanguages;
	return in; 
	}
