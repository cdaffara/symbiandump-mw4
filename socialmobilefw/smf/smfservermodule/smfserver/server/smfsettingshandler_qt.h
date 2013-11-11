/*
 * smfsettingshandler_qt.h
 *
 *  Created on: Jul 8, 2010
 *      Author: satishkk
 */

#ifndef SMFSETTINGSHANDLER_QT_H_
#define SMFSETTINGSHANDLER_QT_H_

#include <qsettings.h>
#include <qstringlist.h>

/*!
  @class SmfSettingsRepositoryQt

  @discussion An instance Platform Independent Settings class 
  to set/get setting values to/from symbian repository.
  */
class SmfSettingsRepositoryQt : public QObject
    {
public:


/*!
  @function ~SmfSettingsRepositoryQt

  @discussion Destroy the object and release all memory objects
  */
     ~SmfSettingsRepositoryQt();

	/*!
	  @function SmfSettingsRepositoryQt

	  @discussion Construct the Settings Object
	  */

     SmfSettingsRepositoryQt(QObject *parent = 0);
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
	void GetPluginDetails(QMap<QString,QString>& aDetails) const;
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

#endif /* SMFSETTINGSHANDLER_QT_H_ */
