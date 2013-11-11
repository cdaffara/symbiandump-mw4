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
 * The Transport Manager class initializes the transport component of Smf 
 * framework
 * Note: This class has dependencies on QtMobility project
 *
 */

#ifndef SMFTRANSPORTMANAGER_H_
#define SMFTRANSPORTMANAGER_H_

// Include files
#include "smfserverglobal.h"

// Qt mobility classes
#include <qnetworkconfigmanager.h>
#include <qsysteminfo.h>

using namespace QtMobility;

/**
 * The Transport Manager class initializes the transport component of Smf 
 * framework
 * 
 * Note: This class has dependencies on QtMobility project
 */
class SmfTransportManager : public QObject
	{
	Q_OBJECT
	
public:
	/**
	 * Method to get the instance of SmfTransportManager class
	 * @return The instance of SmfTransportManager class
	 */
	static SmfTransportManager* getInstance ( );
	
	/**
	 * Destructor
	 */
	~SmfTransportManager ( );
	
public slots:
	/**
	 * Method to initialize the transport component before 
	 * executing a web query
	 * @return SmfTransportInitializeResult
	 */
	SmfTransportInitializeResult initializeTransport ( );
	
	/*
	 * This slot is called whenever a new network configuration is added to the system.
	 * @param aConfig The new configuration
	 */
	void configurationAdded ( const QNetworkConfiguration &aConfig );

	/*
	 * This slot is called when the state of the aConfig changes.
	 * @param aConfig The changed configuration
	 */
	void configurationChanged ( const QNetworkConfiguration &aConfig );

	/*
	 * This slot is called when a configuration is about to be removed from the system.
	 * The removed configuration is invalid but retains name and identifier.
	 * @param aConfig The to be removed configuration
	 */
	void configurationRemoved ( const QNetworkConfiguration &aConfig );
	
private:
	/* Method to get network availability.
	 * @return True if online, else false.
	 */
	bool getNetworkAvailabilty ( );
	
	/*
	 * Method that checks if the phone is in home network or in roaming.
	 * @param aStatus [out] An output parameter indicating the current network 
	 * status as SmfNetworkStatus
	 */
	void getCurrentNetworkStatus ( SmfNetworkStatus &aStatus );
	
private:
	/**
	 * Constructor with default argument
	 */
	SmfTransportManager ( );
	
	/**
	 * Copy Constructor
	 * @param aOther The reference object
	 */
	SmfTransportManager ( const SmfTransportManager &aOther );
	
private:
	/**
	 * The single instance of SmfTransportManager
	 */
	static SmfTransportManager* m_myInstance;
	
	/*
	 * Member variable - QNetworkConfigurationManager instance
	 */
	QNetworkConfigurationManager *m_netwConfigMngr;
	
	/*
	 * Member variable - QSystemNetworkInfo instance
	 */
	QSystemNetworkInfo *m_systemInfo;
	
	};

#endif /* SMFTRANSPORTMANAGER_H_ */
