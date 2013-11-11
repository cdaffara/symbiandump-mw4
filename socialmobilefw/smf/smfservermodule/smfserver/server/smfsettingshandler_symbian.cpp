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
 * 
 * Description:
 * Settings Repository class of SMF
 */

#ifdef __FOR_SYMBIAN_CR_USAGE__

#include <e32base.h>
#include "smfsettingshandler_symbian.h"
#include "SettingsConstants.h"

// Standard construction sequence
CSettingsRepository* CSettingsRepository::NewL()
    {
   	 CSettingsRepository* self = CSettingsRepository::NewLC();
     CleanupStack::Pop( self );
     return self;
    }

CSettingsRepository* CSettingsRepository::NewLC()
    {
     CSettingsRepository* self = new ( ELeave ) CSettingsRepository;
     CleanupStack::PushL( self );
     self->ConstructL();
     return self;
    }
CSettingsRepository::CSettingsRepository()
    {

    }

CSettingsRepository::~CSettingsRepository()
    {
	 if(iRepository)
		{
			delete iRepository;
			iRepository = NULL;
		}
    }

void CSettingsRepository::ConstructL()
    {
   	  User::LeaveIfNull(iRepository= CRepository::NewL(KCRUidSettings));
    }
void CSettingsRepository::SetPluginDetails(QString& aPluginName,QString& aPluginStatus)
	{
	TPtrC pluginNameSymbian(static_cast<const TUint16*>(aPluginName.utf16()), aPluginName.length());
	TPtrC pluginStatusSymbian(static_cast<const TUint16*>(aPluginStatus.utf16()), aPluginStatus.length());
	SetPluginDetails(pluginNameSymbian,pluginStatusSymbian);
	}

void CSettingsRepository::SetAuthExpirationValue(QString& aVal)
	{
	TPtrC valSymbian(static_cast<const TUint16*>(aVal.utf16()), aVal.length());
	SetAuthExpirationValue(valSymbian);
	}

void CSettingsRepository::SetMaxDataTransferLimit(QString& aVal)
	{
	TPtrC valSymbian(static_cast<const TUint16*>(aVal.utf16()), aVal.length());
	SetMaxDataTransferLimit(valSymbian);
	}

void CSettingsRepository::SetRoamingStatusforDataTransfer(QString& aStatus)
	{
	TPtrC valSymbian(static_cast<const TUint16*>(aStatus.utf16()), aStatus.length());
	SetRoamingStatusforDataTransfer(valSymbian);
	}

void CSettingsRepository::SetUploadFileType(QString& aFileType)
	{
	TPtrC valSymbian(static_cast<const TUint16*>(aFileType.utf16()), aFileType.length());
	SetRoamingStatusforDataTransfer(valSymbian);
	}

void CSettingsRepository::GetPluginDetails(QMap<QString,QString>& aDetails) 
	{
	TBuf<KMaxSettingValue> name ;
	TBuf<KMaxSettingValue> details;
	GetPluginDetailsInternal(name,details);

	QString nameQt = QString::fromUtf16((name.Ptr()),name.Length());
	QString statusQt = QString::fromUtf16((details.Ptr()),details.Length());
	aDetails.insert(nameQt,statusQt);

	}

QString CSettingsRepository::GetAuthExpirationValue() const
	{
	TBuf<125> valSymbian;
	GetAuthExpirationValueInternal(valSymbian);
	QString valQt = QString::fromUtf16((valSymbian.Ptr()),valSymbian.Length());
	return valQt;
	}

QString CSettingsRepository::GetMaxDataTransferLimit()const
	{

	TBuf<125> buf;
	GetMaxDataTransferLimitInternal(buf);
	QString str;
	str = QString::fromUtf16((buf.Ptr()),buf.Length());
	return str;

	}

QString CSettingsRepository::GetRoamingStatusforDataTransfer() const
	{
	TBuf<125> valSymbian ;
	GetRoamingStatusforDataTransferInternal(valSymbian);
	QString valQt;
	valQt = QString::fromUtf16((valSymbian.Ptr()),valSymbian.Length());
	return valQt;
	}

QString CSettingsRepository::GetUploadFileType() const
	{
	TBuf<125> valSymbian;
	GetUploadFileTypeInternal(valSymbian);
	QString valQt = QString::fromUtf16((valSymbian.Ptr()),valSymbian.Length());
	return valQt;
	}
/*

 * Method to set the PluginDetails to Repository
 * Param - @aPluginStatus,holds the Plgugin Status.
 */
void CSettingsRepository::SetPluginDetails(TDesC& aPluginName,TDesC& aPluginStatus)
	{
		User::LeaveIfError(iRepository->Set(KSettingsUIPluginNameKey,aPluginName));
		User::LeaveIfError(iRepository->Set(KSettingsUIPluginStausKey,aPluginStatus));
	}
/*
 * Method to set the Auth Expiry Value to Repository
 * Param - @aVal,Auth Expiry Value
 */
void CSettingsRepository::SetAuthExpirationValue(TDesC& aVal)
	{
		User::LeaveIfError(iRepository->Set(KSettingsUIAuthExpLimitKey,aVal));
	}
/*
 * Method to set the Max. Data Transfer Limit to Repository
 * Param - @aVal,Max. Data Transfer Limit Value
 */
void CSettingsRepository::SetMaxDataTransferLimit(TDesC& aVal)
	{
		User::LeaveIfError(iRepository->Set(KSettingsUIMaxDataTransferLimitKey,aVal));
	}
/*
 * Method to set the Roaming Status for Data Transfer to Repository
 * Param - @aStatus,Rpaming Status value for Data Transfer
 */
void CSettingsRepository::SetRoamingStatusforDataTransfer(TDesC& aStatus)
	{
		User::LeaveIfError(iRepository->Set(KSettingsUIRoamingStatusKey,aStatus));
	}
/*
 * Method to set the Upload File Type to Repository
 * Param - @aStatus,Upload File Type Value
 */
void CSettingsRepository::SetUploadFileType(TDesC& aFileType)
	{
		User::LeaveIfError(iRepository->Set(KSettingsUIUploadFileTypeKey,aFileType));
	}
/*
 * Method to Get the PluginDetails from Repository
 */
void CSettingsRepository::GetPluginDetailsInternal(TDes& aName , TDes& aDetails) const
	{

		User::LeaveIfError(iRepository->Get(KSettingsUIPluginNameKey,aName));
		
		User::LeaveIfError(iRepository->Get(KSettingsUIPluginStausKey,aDetails));
	}
/*
 * Method to Get the Auth Expiry Value from Repository
 */
void CSettingsRepository::GetAuthExpirationValueInternal(TDes& aExp) const
	{
		
		User::LeaveIfError(iRepository->Get(KSettingsUIAuthExpLimitKey,aExp));
		
	}
/*
 * Method to Get the Max. Data Transfer Limit from Repository
 */
void CSettingsRepository::GetMaxDataTransferLimitInternal(TDes& aMax) const
	{
		TBuf<125> Value;
		User::LeaveIfError(iRepository->Get(KSettingsUIMaxDataTransferLimitKey,aMax));
		
	}
/*
 * Method to Get the Roaming Status for Data Transfer from Repository
 */
void CSettingsRepository::GetRoamingStatusforDataTransferInternal(TDes& aRoam) const
	{
		User::LeaveIfError(iRepository->Get(KSettingsUIRoamingStatusKey,aRoam));

	}
/*
 * Method to Get the Upload File Type from Repository
 */
void CSettingsRepository::GetUploadFileTypeInternal(TDes& aFtype) const
	{
		User::LeaveIfError(iRepository->Get(KSettingsUIUploadFileTypeKey,aFtype));
	}

#endif
