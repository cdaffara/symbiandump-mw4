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

#include "PlatfromIndependentSettings.h"

#ifndef __FOR_SYMBIAN_CR_USAGE__

#include "SettingsConstants.h"

CPFIndSettings::CPFIndSettings()
    {
		iSettings = new QSettings(QSettings::SystemScope,"Sasken","SMF");
    }

CPFIndSettings::~CPFIndSettings()
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
void CPFIndSettings::SetPluginDetails(QString& aPluginName,QString& aPluginStatus)
	{
		iSettings->setValue(KSettingsUIPluginNameKey,aPluginName);
		iSettings->setValue(KSettingsUIPluginStausKey,aPluginStatus);
	}
/*
 * Method to set the Auth Expiry Value to  QSettings
 * Param - @aVal,Auth Expiry Value
 */
void CPFIndSettings::SetAuthExpirationValue(QString& aVal)
	{
		iSettings->setValue(KSettingsUIAuthExpLimitKey,aVal);
	}
/*
 * Method to set the Max. Data Transfer Limit to  QSettings
 * Param - @aVal,Max. Data Transfer Limit Value
 */
void CPFIndSettings::SetMaxDataTransferLimit(QString& aVal)
	{
		iSettings->setValue(KSettingsUIMaxDataTransferLimitKey,aVal);
	}
/*
 * Method to set the Roaming Status for Data Transfer to  QSettings
 * Param - @aStatus,Rpaming Status value for Data Transfer
 */
void CPFIndSettings::SetRoamingStatusforDataTransfer(QString& aStatus)
	{
		iSettings->setValue(KSettingsUIRoamingStatusKey,aStatus);
	}
/*
 * Method to set the Upload File Type to  QSettings
 * Param - @aStatus,Upload File Type Value
 */
void CPFIndSettings::SetUploadFileType(QString& aFileType)
	{
		iSettings->setValue(KSettingsUIUploadFileTypeKey,aFileType);
	}
/*
 * Method to Get the PluginDetails from  QSettings
 */
QStringList CPFIndSettings::GetPluginDetails() const
	{
		QVariant Value;
		QStringList PluginDetails;

		Value = iSettings->value(KSettingsUIPluginNameKey,Value);
		PluginDetails.append(Value.toString());
		
		Value =  iSettings->value(KSettingsUIPluginStausKey,Value);
		PluginDetails.append(Value.toString());
		
		return PluginDetails;
	}
/*
 * Method to Get the Auth Expiry Value from  QSettings
 */
QString CPFIndSettings::GetAuthExpirationValue() const
	{
		QVariant Value;
		Value = iSettings->value(KSettingsUIAuthExpLimitKey,Value);
		return Value.toString();
	}
/*
 * Method to Get the Max. Data Transfer Limit from  QSettings
 */
QString CPFIndSettings::GetMaxDataTransferLimit() const
	{
		QVariant Value;
		Value = iSettings->value(KSettingsUIMaxDataTransferLimitKey,Value);
		return Value.toString();
	}
/*
 * Method to Get the Roaming Status for Data Transfer from QSettings
 */
QString CPFIndSettings::GetRoamingStatusforDataTransfer() const
	{
		QVariant Value;
		Value = iSettings->value(KSettingsUIRoamingStatusKey,Value);
		return Value.toString();
	}
/*
 * Method to Get the Upload File Type from QSettings
 */
QString CPFIndSettings::GetUploadFileType() const
	{
		QVariant Value;
		Value = iSettings->value(KSettingsUIUploadFileTypeKey,Value);
		return Value.toString();
	}

#endif
