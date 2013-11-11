/**
* Copyright (c) 2010 Sasken Communication Technologies Ltd.
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the "{License}"
* which accompanies  this distribution, and is available
* at the URL "{LicenseUrl}".
*
* Initial Contributors:
* Narasimhulu Kavadapu, Sasken Communication Technologies Ltd - Initial contribution
*
* Contributors:
*
* Description:
* Platform Independent Settings Class
*/
#include "SettingsConstants.h"

#ifndef __FOR_SYMBIAN_CR_USAGE__

#ifndef PLATFROMINDEPENDENTSETTINGS_H_
#define PLATFROMINDEPENDENTSETTINGS_H_

#include <qsettings.h>
#include <qstringlist.h>
/*!
  @class CPFIndSettings

  @discussion An instance Platform Independent Settings class 
  to set/get setting values to/from symbian repository.
  */
class CPFIndSettings : public QObject
    {
public:


/*!
  @function ~CPFIndSettings

  @discussion Destroy the object and release all memory objects
  */
     ~CPFIndSettings();

	/*!
	  @function CPFIndSettings

	  @discussion Construct the Settings Object
	  */

	    CPFIndSettings();
	/*
	 * Method to set the PluginDetails to QSettings
	 * Parm - @aPluginName,Name of the plugin
	 * Param - @aPluginStatus,holds the Plgugin Status.
	 */
	void SetPluginDetails(QString& aPluginName,QString& aPluginStatus);
	/*
	 * Method to set the Auth Expiry Value to QSettings
	 * Param - @aVal,Auth Expiry Value
	 */
	void SetAuthExpirationValue(QString& aVal);
	/*
	 * Method to set the Max. Data Transfer Limit to QSettings
	 * Param - @aVal,Max. Data Transfer Limit Value
	 */
	void SetMaxDataTransferLimit(QString& aVal);
	/*
	 * Method to set the Roaming Status for Data Transfer to QSettings
	 * Param - @aStatus,Rpaming Status value for Data Transfer
	 */
	void SetRoamingStatusforDataTransfer(QString& aStatus);
	/*
	 * Method to set the Upload File Type to QSettings
	 * Param - @aStatus,Upload File Type Value
	 */
	void SetUploadFileType(QString& aFileType);
	/*
	 * Method to Get the PluginDetails from QSettings
	 */
	QStringList GetPluginDetails() const;
	/*
	 * Method to Get the Auth Expiry Value from QSettings
	 */
	QString GetAuthExpirationValue() const;
	/*
	 * Method to Get the Max. Data Transfer Limit from QSettings
	 */
	QString GetMaxDataTransferLimit() const;
	/*
	 * Method to Get the Roaming Status for Data Transfer from QSettings
	 */
	QString GetRoamingStatusforDataTransfer() const;
	/*
	 * Method to Get the Upload File Type from QSettings
	 */
	QString GetUploadFileType() const;

private:
    /*!
     Pointer to the QSettings Object
      */
    QSettings*  iSettings;

    };

#endif /* PLATFROMINDEPENDENTSETTINGS_H_ */

#endif
