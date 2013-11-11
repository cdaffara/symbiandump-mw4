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
* Settings Repository class of SMF
*/

#include "SettingsRepository.h"

#ifdef __FOR_SYMBIAN_CR_USAGE__


#include <e32base.h>
#include "SettingsRepository.h"

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
RArray<TBuf<KMaxSettingValue> >& CSettingsRepository::GetPluginDetails() const
	{
		TBuf<KMaxSettingValue> Value;
		RArray<TBuf<KMaxSettingValue> > PluginDetails;

		User::LeaveIfError(iRepository->Get(KSettingsUIPluginNameKey,Value));
		PluginDetails.AppendL(Value);
		
		User::LeaveIfError(iRepository->Get(KSettingsUIPluginStausKey,Value));
		PluginDetails.AppendL(Value);
		
		return PluginDetails;
	}
/*
 * Method to Get the Auth Expiry Value from Repository
 */
TDesC& CSettingsRepository::GetAuthExpirationValue() const
	{
		TBuf<KMaxSettingValue> Value;
		User::LeaveIfError(iRepository->Get(KSettingsUIAuthExpLimitKey,Value));
		return Value;
	}
/*
 * Method to Get the Max. Data Transfer Limit from Repository
 */
TDesC& CSettingsRepository::GetMaxDataTransferLimit() const
	{
		TBuf<KMaxSettingValue> Value;
		User::LeaveIfError(iRepository->Get(KSettingsUIMaxDataTransferLimitKey,Value));
		return Value;
	}
/*
 * Method to Get the Roaming Status for Data Transfer from Repository
 */
TDesC& CSettingsRepository::GetRoamingStatusforDataTransfer() const
	{
		TBuf<KMaxSettingValue> Value;
		User::LeaveIfError(iRepository->Get(KSettingsUIRoamingStatusKey,Value));
		return Value;
	}
/*
 * Method to Get the Upload File Type from Repository
 */
TDesC& CSettingsRepository::GetUploadFileType() const
	{
		TBuf<KMaxSettingValue> Value;
		User::LeaveIfError(iRepository->Get(KSettingsUIUploadFileTypeKey,Value));
		return Value;
	}


#endif
