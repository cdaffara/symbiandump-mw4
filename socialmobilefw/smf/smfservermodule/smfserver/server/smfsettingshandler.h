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
 * Reads/Writes settings key in a platform independent way
 */
#ifndef SMFSETTINGSHANDLER_H
#define SMFSETTINGSHANDLER_H

#include <QObject>
#include <QStringList>
#include <QMap>
#ifdef __FOR_SYMBIAN_CR_USAGE__
class CSettingsRepository;
#else
class SmfSettingsRepositoryQt;
#endif



class SmfSettingsHandler : public QObject
	{
	Q_OBJECT
public:
	SmfSettingsHandler(QObject* parent=0);
	~SmfSettingsHandler();

public slots:
	/*
	 * Method to set the PluginDetails to Smf repository
	 * Parm - @aPluginName,Name of the plugin
	 * Param - @aPluginStatus,holds the Plgugin Status.
	 */
	void SetPluginDetails(QString& aPluginName,QString& aPluginStatus);
	/*
	 * Method to set the Auth Expiry Value to Repository
	 * Param - @aVal,Auth Expiry Value
	 */
	void SetAuthExpirationValue(QString& aVal);
	/*
	 * Method to set the Max. Data Transfer Limit to Repository
	 * Param - @aVal,Max. Data Transfer Limit Value
	 */
	void SetMaxDataTransferLimit(QString& aVal);
	/*
	 * Method to set the Roaming Status for Data Transfer to Repository
	 * Param - @aStatus,Rpaming Status value for Data Transfer
	 */
	void SetRoamingStatusforDataTransfer(QString& aStatus);
	/*
	 * Method to set the Upload File Type to Repository
	 * Param - @aStatus,Upload File Type Value
	 */
	void SetUploadFileType(QString& aFileType);
	/*
	 * Method to Get the PluginDetails from Repository
	 */
	void GetPluginDetails(QMap<QString,QString>& aDetails ) const;
	/*
	 * Method to Get the Auth Expiry Value from Repository
	 */
	QString GetAuthExpirationValue() const;
	/*
	 * Method to Get the Max. Data Transfer Limit from Repository
	 */
	QString GetMaxDataTransferLimit() const;
	/*
	 * Method to Get the Roaming Status for Data Transfer from Repository
	 */
	QString GetRoamingStatusforDataTransfer() const;
	/*
	 * Method to Get the Upload File Type from Repository
	 */
	QString GetUploadFileType() const;
private:
#ifdef __FOR_SYMBIAN_CR_USAGE__
	CSettingsRepository* m_repository;
#else
	SmfSettingsRepositoryQt* m_repository;
#endif
	};
#endif
