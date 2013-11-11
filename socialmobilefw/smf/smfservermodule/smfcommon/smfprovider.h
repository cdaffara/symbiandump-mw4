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

#ifndef SMFPROVIDER_H
#define SMFPROVIDER_H

#include <QObject>
#include <QImage>
#include <QUrl>
#include <QStringList>
#include <QMetaType>

#include "smfclientglobal.h"

/**
 * @ingroup smf_client_group 
 * Interface for a base service provider. Other service provider classes contains 
 * implementation of this base class so that each has access to service provider 
 * information.
 * All of the functionality described here should be implemented by a service
 * specific plug-in object.
 */
class SMFCOMMON_EXPORT SmfProvider
	{
	
public:
	/**
	 * Constructor
	 */
	SmfProvider();
	/**
	 * Destructor
	 */
	~SmfProvider();

public:
	/**
	 * Localizable name of the service 
	 * @return service name
	 */
	QString serviceName() const; 
	
	/**
	 * Logo of the service
	 * @return logo image of the service
	 */
	QImage serviceIcon() const; 
	
	/**
	 * Readable service description
	 * @return service description
	 */
	QString description() const; 
	
	/*
	 * Website of the service
	 * @return service url 
	 */
	QUrl serviceUrl() const;
	
	/**
	 * URL of the application providing this service
	 * @return application url
	 */
	QUrl applicationUrl() const;
	
	/**
	 * List of interfaces that this provider support
	 * @return List of supported interface names
	 */
	QList<QString> supportedInterfaces() const;
	
	/**
	 * List of languages supported by this service provider
	 * @return a QStringList of languages supported by this service 
	 * provider in 2 letter ISO 639-1 format.
	 */
	QStringList supportedLanguages() const;
	
	/**
	 * Sets Localizable name of the service 
	 * @param name service name
	 */
	void setServiceName(QString& name); 
	
	/**
	 * Sets Logo of the service
	 * @param image service icon
	 */
	void setServiceIcon(QImage& image) ; 
	
	/**
	 * Sets Readable service description
	 * @param desc description text
	 */
	void setDescription(QString& desc) ; 
	
	/*
	 * Sets Website of the service
	 * @param name service url
	 */
	void setServiceUrl(QUrl& url) ;
	
	/**
	 * Sets URL of the application providing this service
	 * @param url application url
	 */
	void setApplicationUrl(QUrl& url) ; 
	
	/**
	 * Sets list of interfaces that this provider supports
	 * @param types List of supported interface names
	 */
	void setSupportedInterfaces( QStringList& types);
	
	/**
	 * Sets the list of languages supported by this service provider
	 * @param lang A QStringList of languages supported by this service 
	 * provider in 2 letter ISO 639-1 format.
	 */
	void setSupportedLanguages( QStringList& lang );
 
private:
	QString m_serviceName;
	QImage m_serviceIcon;
	QString m_description;
	QUrl m_serviceUrl;
	QUrl m_appUrl;
	QStringList m_serviceTypes;
	QStringList m_supportedLanguages;
	
	friend QDataStream &operator<<(QDataStream &, const SmfProvider&);
	friend QDataStream &operator>>(QDataStream &, SmfProvider&);
	};

/**
 * Method for Externalization. Writes the SmfProvider object to 
 * the stream and returns a reference to the stream.
 * @param aDataStream Stream to be written
 * @param aProvider The SmfProvider object to be externalized
 * @return reference to the written stream
 */
SMFCOMMON_EXPORT QDataStream& operator<<(QDataStream &aDataStream, 
		const SmfProvider &aProvider);

/**
 * Method for Internalization. Reads a SmfProvider object from 
 * the stream and returns a reference to the stream.
 * @param aDataStream Stream to be read
 * @param aProvider The SmfProvider object to be internalized
 * @return reference to the stream
 */
SMFCOMMON_EXPORT QDataStream& operator>>(QDataStream &aDataStream, 
		SmfProvider &aProvider);

typedef QList<SmfProvider> SmfProviderList;

// Make the class SmfProvider known to QMetaType, so that as to register it.
Q_DECLARE_METATYPE(SmfProvider)
Q_DECLARE_METATYPE(QList<SmfProvider>)

#endif // SMFPROVIDER_H
