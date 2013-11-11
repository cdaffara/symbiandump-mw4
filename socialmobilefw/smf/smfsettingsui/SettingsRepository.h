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

#include "SettingsConstants.h"

#ifdef __FOR_SYMBIAN_CR_USAGE__

#ifndef SETTINGSREPOSITORY_H_
#define SETTINGSREPOSITORY_H_

const TInt KMaxSettingValue = 50;

#include <centralrepository.h>
//#include <BADESCA.H> 
/*!
  @class CSettingsRepository

  @discussion An instance Settings Repository class 
  to set/get setting values to/from symbian repository.
  */
class CSettingsRepository : public CBase
    {
public:

/*!
  @function NewL

  @discussion Create a CSettingsRepository object
  @result a pointer to the created instance of CSettingsRepository
  */
    static CSettingsRepository* NewL();

/*!
  @function NewLC

  @discussion Create a CSettingsRepository object
  @result a pointer to the created instance of CSettingsRepository
  */
    static CSettingsRepository* NewLC();


/*!
  @function ~CSettingsRepository

  @discussion Destroy the object and release all memory objects
  */
     ~CSettingsRepository();


public:
	

	/*!
	  @function CSettingsRepository

	  @discussion Perform the first phase of two phase construction
	  */

	    CSettingsRepository();
	/*
	 * Method to set the PluginDetails to Symbian Repository
	 * Parm - @aPluginName,Name of the plugin
	 * Param - @aPluginStatus,holds the Plgugin Status.
	 */
	void SetPluginDetails(TDesC& aPluginName,TDesC& aPluginStatus);
	/*
	 * Method to set the Auth Expiry Value to Repository
	 * Param - @aVal,Auth Expiry Value
	 */
	void SetAuthExpirationValue(TDesC& aVal);
	/*
	 * Method to set the Max. Data Transfer Limit to Repository
	 * Param - @aVal,Max. Data Transfer Limit Value
	 */
	void SetMaxDataTransferLimit(TDesC& aVal);
	/*
	 * Method to set the Roaming Status for Data Transfer to Repository
	 * Param - @aStatus,Rpaming Status value for Data Transfer
	 */
	void SetRoamingStatusforDataTransfer(TDesC& aStatus);
	/*
	 * Method to set the Upload File Type to Repository
	 * Param - @aStatus,Upload File Type Value
	 */
	void SetUploadFileType(TDesC& aFileType);
	/*
	 * Method to Get the PluginDetails from Repository
	 */
	RArray<TBuf<KMaxSettingValue> >& GetPluginDetails() const;
	/*
	 * Method to Get the Auth Expiry Value from Repository
	 */
	TDesC& GetAuthExpirationValue() const;
	/*
	 * Method to Get the Max. Data Transfer Limit from Repository
	 */
	TDesC& GetMaxDataTransferLimit() const;
	/*
	 * Method to Get the Roaming Status for Data Transfer from Repository
	 */
	TDesC& GetRoamingStatusforDataTransfer() const;
	/*
	 * Method to Get the Upload File Type from Repository
	 */
	TDesC& GetUploadFileType() const;

private:

/*!
  @function ConstructL

  @discussion  Perform the second phase construction of a CSettingsRepository object
  */
    void ConstructL();

    /*!
     Pointer to the Centra Repository class object.
      */
   CRepository*  iRepository;

    };


#endif /* SETTINGSREPOSITORY_H_ */

#endif
