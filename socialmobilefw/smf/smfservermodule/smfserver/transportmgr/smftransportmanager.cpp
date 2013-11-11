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

// Include files
#include "smftransportmanager.h"
#include "smftransportmanagerutil.h"

// Static data initialisation
SmfTransportManager* SmfTransportManager::m_myInstance = NULL;

/**
 * Method to get the instance of SmfTransportManager class
 * @return The instance of SmfTransportManager class
 */
SmfTransportManager* SmfTransportManager::getInstance ( )
	{
	if(NULL == m_myInstance)
		m_myInstance = new SmfTransportManager( );
	return m_myInstance;
	}

/**
 * Constructor with default argument
 * @param aParent The parent object
 */
SmfTransportManager::SmfTransportManager ( )
	{
	m_systemInfo = new QSystemNetworkInfo(this);
	
	m_netwConfigMngr = new QNetworkConfigurationManager(this);
	
	// Register for monitoring changes in IAPs (network configurations)
	connect(m_netwConfigMngr, SIGNAL(configurationAdded(const QNetworkConfiguration &)), 
			this, SLOT(configurationAdded(const QNetworkConfiguration &)));
	
	connect(m_netwConfigMngr, SIGNAL(configurationChanged(const QNetworkConfiguration &)), 
			this, SLOT(configurationChanged(const QNetworkConfiguration &)));
	
	connect(m_netwConfigMngr, SIGNAL(configurationRemoved(const QNetworkConfiguration &)), 
			this, SLOT(configurationRemoved(const QNetworkConfiguration &)));
	}


/**
 * Destructor
 */
SmfTransportManager::~SmfTransportManager ( )
	{
	if(m_netwConfigMngr)
		delete m_netwConfigMngr;
	
	if(m_systemInfo)
		delete m_systemInfo;
	
	if(m_myInstance)
		delete m_myInstance;
	
	m_myInstance = NULL;
	}


/**
 * Method to initialize the transport component before 
 * executing a web query
 * @return SmfTransportInitializeResult
 */
SmfTransportInitializeResult SmfTransportManager::initializeTransport ( )
	{
	qDebug()<<"Inside SmfTransportManager::initializeTransport()";
	SmfTransportInitializeResult retVal = SmfTransportInitNetworkNotAvailable;
	
	if(getNetworkAvailabilty())
		{
		qDebug()<<"getNetworkAvailabilty() returned true";
		retVal = SmfTransportInitNoError;
		
		SmfNetworkStatus status;
		getCurrentNetworkStatus(status);
		qDebug()<<"getCurrentNetworkStatus() returned = "+QString::number(status);
		switch(status)
			{
			// homenetwork
			case SmfNetworkConnectedHome:
			case SmfNetworkConnected:
				break;

			// roaming
			case SmfNetworkConnectedRoaming:
///////////// check with CSM for usage while roaming
//// if enabled do nothing(i.e, retVal will be SmfTransportInitNoError)
//// else retVal = SmfTransportInitRoamingNetworkUsageNotEnabled;
/*				if(!iSmfServer->getNetworkUsageWhileRoamingSetting())
					retVal = SmfTransportInitRoamingNetworkUsageNotEnabled;*/
				break;
						
			// unknown state
			case SmfNetworkStateNotKnown:
			// network not available
			case SmfNetworkNotConnected:
			default :
				retVal = SmfTransportInitNetworkNotAvailable;
			}
		}
	else
		qDebug()<<"getNetworkAvailabilty() returned false!!!";
	
	qDebug()<<"Return value of SmfTransportManager::initializeTransport() = "<<retVal;
	return retVal;
	}


/* Method to get network availability.
 * @return True if online, else false.
 */
bool SmfTransportManager::getNetworkAvailabilty ( )
	{
	qDebug()<<"Inside SmfTransportManager::getNetworkAvailabilty()";

/*	QSystemNetworkInfo::NetworkStatus ret2 = m_systemInfo->networkStatus(QSystemNetworkInfo::GsmMode);
	qDebug()<<"gsm = "<<ret2;
	
	ret2 = m_systemInfo->networkStatus(QSystemNetworkInfo::CdmaMode);
	qDebug()<<"cdma = "<<ret2;
	
	ret2 = m_systemInfo->networkStatus(QSystemNetworkInfo::WcdmaMode);
	qDebug()<<"wcdma = "<<ret2;
	
	ret2 = m_systemInfo->networkStatus(QSystemNetworkInfo::WlanMode);
	qDebug()<<"wlan = "<<ret2;
	
	ret2 = m_systemInfo->networkStatus(QSystemNetworkInfo::EthernetMode);
	qDebug()<<"ethernet = "<<ret2;
	
	ret2 = m_systemInfo->networkStatus(QSystemNetworkInfo::BluetoothMode);
	qDebug()<<"bluetooth = "<<ret2;
	
	ret2 = m_systemInfo->networkStatus(QSystemNetworkInfo::WimaxMode);
	qDebug()<<"wimax = "<<ret2;
	
	bool ret1 = m_netwConfigMngr->isOnline();
	qDebug()<<"m_netwConfigMngr->isOnline() = "<<ret1;*/
		
	bool retVal = false;
	
	// Get all the defined and undefined configurations
	QList<QNetworkConfiguration> list = m_netwConfigMngr->allConfigurations();
	qDebug()<<"list count = "<<list.count();
	
	if(list.count())
		{
		foreach(QNetworkConfiguration conf, list)
			{
			qDebug()<<"Configuration name = "<<conf.name();
			qDebug()<<"Configuration bearer name = "<<conf.bearerName();
			qDebug()<<"Configuration state = "<<conf.state();

			// If the stateflgag is any one other than QNetworkConfiguration::Undefined or 
			// QNetworkConfiguration::Defined, means a defined configuration is available
			if( (QNetworkConfiguration::Discovered == conf.state()) ||
					(QNetworkConfiguration::Active == conf.state()) )
				{
				retVal = true;
				break;
				}
			}
		}

	return retVal;
	}


/*
 * Method that checks if the phone is in home network or in roaming.
 * @param aStatus [out] An output parameter indicating the current network 
 * status as SmfNetworkStatus
 */
void SmfTransportManager::getCurrentNetworkStatus ( 
		SmfNetworkStatus &aStatus )
	{
	QSystemNetworkInfo::NetworkStatus status = m_systemInfo->networkStatus ( QSystemNetworkInfo::GsmMode );
	qDebug()<<"m_systemInfo->networkStatus ( QSystemNetworkInfo::GsmMode ) = "<<status;
	
	switch(status)
		{
		// homenetwork
		case QSystemNetworkInfo::HomeNetwork:
			aStatus = SmfNetworkConnectedHome;
			qDebug()<<"Home network";
			break;
		
		// connected
		case QSystemNetworkInfo::Connected:
			aStatus = SmfNetworkConnected;
			qDebug()<<"network connected";
			break;
		
		// roaming	
		case QSystemNetworkInfo::Roaming:
			aStatus = SmfNetworkConnectedRoaming;
			qDebug()<<"roaming";
			break;
			
		// unknown state			
		case QSystemNetworkInfo::Searching:
		case QSystemNetworkInfo::Busy:
			qDebug()<<"network state unknown";
			aStatus = SmfNetworkStateNotKnown;
			break;

		// network not available
		case QSystemNetworkInfo::UndefinedStatus:
		case QSystemNetworkInfo::NoNetworkAvailable:
		case QSystemNetworkInfo::EmergencyOnly:
		case QSystemNetworkInfo::Denied:
		default :
			qDebug()<<"not connected";
			aStatus = SmfNetworkNotConnected;
		}
	}



/**
 * This slot is called whenever a new network configuration is added to the system.
 * @param aConfig The new configuration
 */
void SmfTransportManager::configurationAdded ( 
		const QNetworkConfiguration &aConfig )
	{
	Q_UNUSED(aConfig)
	SmfTransportManagerUtil::getInstance()->configurationAdded(SmfTransportOpIAPChanged);
	}

/**
 * This slot is called when the state of the aConfig changes.
 * @param aConfig The changed configuration
 */
void SmfTransportManager::configurationChanged ( 
		const QNetworkConfiguration &aConfig )
	{
	if( aConfig == m_netwConfigMngr->defaultConfiguration() )
		SmfTransportManagerUtil::getInstance()->configurationChanged(SmfTransportOpIAPChanged);
	}

/**
 * This slot is called when a configuration is about to be removed from the system.
 * The removed configuration is invalid but retains name and identifier.
 * @param aConfig The to be removed configuration
 */
void SmfTransportManager::configurationRemoved ( 
		const QNetworkConfiguration &aConfig )
	{
	if( aConfig == m_netwConfigMngr->defaultConfiguration() )
		SmfTransportManagerUtil::getInstance()->configurationRemoved(SmfTransportOpIAPChanged);
	}

