/*
 * smfsettingshandler_qt.cpp
 *
 *  Created on: Jul 8, 2010
 *      Author: satishkk
 */

#include "smfsettingshandler_qt.h"
#include "SettingsConstants.h"

SmfSettingsRepositoryQt::SmfSettingsRepositoryQt(QObject *parent)
    {
	Q_UNUSED(parent)
		iSettings = new QSettings(QSettings::SystemScope,"Sasken","SMF");
    }

SmfSettingsRepositoryQt::~SmfSettingsRepositoryQt()
    {
	 if(iSettings)
		{
			delete iSettings;
			iSettings = NULL;
		}
    }
/*

 * Method to set the PluginDetails to  QSettings
 * Param - @aPluginStatus,holds the Plgugin Status.
 */
void SmfSettingsRepositoryQt::SetPluginDetails(QString& aPluginName,QString& aPluginStatus)
	{
	QVariant var1 = QVariant::fromValue(aPluginName);
	QVariant var2 = QVariant::fromValue(aPluginStatus);
		iSettings->setValue(KSettingsUIPluginNameKey,var1);
		iSettings->setValue(KSettingsUIPluginStausKey,var2);
	}
/*
 * Method to set the Auth Expiry Value to  QSettings
 * Param - @aVal,Auth Expiry Value
 */
void SmfSettingsRepositoryQt::SetAuthExpirationValue(QString& aVal)
	{
		iSettings->setValue(KSettingsUIAuthExpLimitKey,aVal);
	}
/*
 * Method to set the Max. Data Transfer Limit to  QSettings
 * Param - @aVal,Max. Data Transfer Limit Value
 */
void SmfSettingsRepositoryQt::SetMaxDataTransferLimit(QString& aVal)
	{
		iSettings->setValue(KSettingsUIMaxDataTransferLimitKey,aVal);
	}
/*
 * Method to set the Roaming Status for Data Transfer to  QSettings
 * Param - @aStatus,Rpaming Status value for Data Transfer
 */
void SmfSettingsRepositoryQt::SetRoamingStatusforDataTransfer(QString& aStatus)
	{
		iSettings->setValue(KSettingsUIRoamingStatusKey,aStatus);
	}
/*
 * Method to set the Upload File Type to  QSettings
 * Param - @aStatus,Upload File Type Value
 */
void SmfSettingsRepositoryQt::SetUploadFileType(QString& aFileType)
	{
		iSettings->setValue(KSettingsUIUploadFileTypeKey,aFileType);
	}
/*
 * Method to Get the PluginDetails from  QSettings
 */
void SmfSettingsRepositoryQt::GetPluginDetails(QMap<QString,QString>& aDetails) const
	{
		QVariant Value1, Value2;

		Value1 = iSettings->value(KSettingsUIPluginNameKey,Value1);
		
		Value2 =  iSettings->value(KSettingsUIPluginStausKey,Value2);
		aDetails.insert(Value1.toString(), Value2.toString());

	}
/*
 * Method to Get the Auth Expiry Value from  QSettings
 */
QString SmfSettingsRepositoryQt::GetAuthExpirationValue() const
	{
		QVariant Value;
		Value = iSettings->value(KSettingsUIAuthExpLimitKey,Value);
		return Value.toString();
	}
/*
 * Method to Get the Max. Data Transfer Limit from  QSettings
 */
QString SmfSettingsRepositoryQt::GetMaxDataTransferLimit() const
	{
		QVariant Value;
		Value = iSettings->value(KSettingsUIMaxDataTransferLimitKey,Value);
		return Value.toString();
	}
/*
 * Method to Get the Roaming Status for Data Transfer from QSettings
 */
QString SmfSettingsRepositoryQt::GetRoamingStatusforDataTransfer() const
	{
		QVariant Value;
		Value = iSettings->value(KSettingsUIRoamingStatusKey,Value);
		return Value.toString();
	}
/*
 * Method to Get the Upload File Type from QSettings
 */
QString SmfSettingsRepositoryQt::GetUploadFileType() const
	{
		QVariant Value;
		Value = iSettings->value(KSettingsUIUploadFileTypeKey,Value);
		return Value.toString();
	}
