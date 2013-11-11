/*
 * SettingsInternalCRKeys.h
 *
 *  Created on: Mar 30, 2010
 *      Author: kavadapu
 */

#ifndef SETTINGSCONSTANTS_H_
#define SETTINGSCONSTANTS_H_

//#define __FOR_SYMBIAN_CR_USAGE__

#ifdef __FOR_SYMBIAN_CR_USAGE__
	#include <e32base.h>
	//UID of the Setting Application
	static const TUid KCRUidSettings = { 0xE6313AF5 };
	//Settings Key value to set/identify the Plugin Name
	const TUint32 KSettingsUIPluginNameKey = 0x01;
	//Settings Key value to set/identify the Plugin Status
	const TUint32 KSettingsUIPluginStausKey = 0x02;
	//Settings Key value used to set/identify the Authentication Expiry Limit
	const TUint32 KSettingsUIAuthExpLimitKey = 0x03;
	//Settings key value used to set/identify the Max. Data Transfer Limit value
	const TUint32 KSettingsUIMaxDataTransferLimitKey = 0x04;
	//Settings key value used to set/identify the Roaming status Value.
	const TUint32 KSettingsUIRoamingStatusKey = 0x05;
	//Settings key value used to set/identify the Upload file type value.
	const TUint32 KSettingsUIUploadFileTypeKey = 0x06;
#else
	#include "qsettings.h"
	
	//Settings Key value to set/identify the Plugin Name
	const QString KSettingsUIPluginNameKey = "PluginName";
	//Settings Key value to set/identify the Plugin Status
	const QString KSettingsUIPluginStausKey = "PluginStatus";
	//Settings Key value used to set/identify the Authentication Expiry Limit
	const QString KSettingsUIAuthExpLimitKey = "AuthExpLmt";
	//Settings key value used to set/identify the Max. Data Transfer Limit value
	const QString KSettingsUIMaxDataTransferLimitKey = "MaxDataTrLmt";
	//Settings key value used to set/identify the Roaming status Value.
	const QString KSettingsUIRoamingStatusKey = "RoamingStatus";
	//Settings key value used to set/identify the Upload file type value.
	const QString KSettingsUIUploadFileTypeKey = "UploadFileType";
#endif

#endif /* SETTINGSCONSTANTS_H_ */
