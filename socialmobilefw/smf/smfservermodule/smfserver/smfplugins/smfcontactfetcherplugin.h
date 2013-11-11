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
 * Interface specification for fetching contacts
 *
 */

#ifndef SMFCONTACTFETCHERPLUGIN_H_
#define SMFCONTACTFETCHERPLUGIN_H_

#include <smfpluginbase.h>
#include <smfcontact.h>
#include <smfgroup.h>
#include <smflocation.h>


/**
 * @ingroup smf_plugin_group
 * Interface specification for fetching contacts. This class provides 
 * basic functionality to allow applications to obtain list of
 * friends, followers, groups of a user in a social networking service.
 *
 * All of the functionality described here should be implemented by a service
 * specific plug-in.
 */
class SmfContactFetcherPlugin : public SmfPluginBase
	{
public:
	/**
	 * Destructor
	 */
	virtual ~SmfContactFetcherPlugin( ) {}
	
	/**
	 * Method to get the list of friends
	 * @param aRequest [out] The request data to be sent to network
	 * @param aPageNum [in] The page to be extracted
	 * @param aItemsPerPage [in] Number of items per page
	 * @return Appropriate value of the enum SmfPluginError.
	 * Plugin error if any, else SmfPluginErrNone for success
	 */
	virtual SmfPluginError friends( SmfPluginRequestData &aRequest,
			const int aPageNum = SMF_FIRST_PAGE, 
			const int aItemsPerPage = SMF_ITEMS_PER_PAGE ) = 0;
	
	/**
	 * Method to get the list of followers
	 * @param aRequest [out] The request data to be sent to network
	 * @param aPageNum [in] The page to be extracted
	 * @param aItemsPerPage [in] Number of items per page
	 * @return Appropriate value of the enum SmfPluginError.
	 * Plugin error if any, else SmfPluginErrNone for success
	 */
	virtual SmfPluginError followers( SmfPluginRequestData &aRequest,
			const int aPageNum = SMF_FIRST_PAGE, 
			const int aItemsPerPage = SMF_ITEMS_PER_PAGE ) = 0;
	
	/**
	 * Method to search for a contact (name or service ID should be provided)
	 * @param aRequest [out] The request data to be sent to network
	 * @param aContact [in] contact to be searched
	 * @param aPageNum [in] The page to be extracted
	 * @param aItemsPerPage [in] Number of items per page
	 * @return Appropriate value of the enum SmfPluginError.
	 * Plugin error if any, else SmfPluginErrNone for success
	 */
	virtual SmfPluginError search( SmfPluginRequestData &aRequest,
			const SmfContact &aContact,
			const int aPageNum = SMF_FIRST_PAGE, 
			const int aItemsPerPage = SMF_ITEMS_PER_PAGE ) = 0;
	
	/**
	 * Method to search for contacts (friends) who are near the user.
	 * Proximity defines accuracy level
	 * @param aRequest [out] The request data to be sent to network
	 * @param aLocation [in] The location search criteria
	 * @param aProximity [in] location search boundary
	 * @param aPageNum [in] The page to be extracted
	 * @param aItemsPerPage [in] Number of items per page
	 * @return Appropriate value of the enum SmfPluginError.
	 * Plugin error if any, else SmfPluginErrNone for success
	 */
	virtual SmfPluginError searchNear( SmfPluginRequestData &aRequest,
			const SmfLocation &aLocation,
			const SmfLocationSearchBoundary &aProximity,
			const int aPageNum = SMF_FIRST_PAGE, 
			const int aItemsPerPage = SMF_ITEMS_PER_PAGE ) = 0;
	
	/**
	 * Method to get the list of groups
	 * @param aRequest [out] The request data to be sent to network
	 * @param aPageNum The page to be extracted
	 * @param aItemsPerPage Number of items per page
	 * @return Appropriate value of the enum SmfPluginError.
	 * Plugin error if any, else SmfPluginErrNone for success
	 */
	virtual SmfPluginError groups( SmfPluginRequestData &aRequest,
			const int aPageNum = SMF_FIRST_PAGE, 
			const int aItemsPerPage = SMF_ITEMS_PER_PAGE ) = 0;
	
	/**
	 * Method to search for a contact in a group
	 * @param aRequest [out] The request data to be sent to network
	 * @param aGroup [in] the group in which to search
	 * @param aContact [in] The contact to be searched, 
	 * default (NULL) is the self contact.
	 * @param aPageNum [in] The page to be extracted
	 * @param aItemsPerPage [in] Number of items per page
	 * @return Appropriate value of the enum SmfPluginError.
	 * Plugin error if any, else SmfPluginErrNone for success
	 */
	virtual SmfPluginError searchInGroup( SmfPluginRequestData &aRequest,
			const SmfGroup &aGroup,
			SmfContact *aContact = NULL,
			const int aPageNum = SMF_FIRST_PAGE, 
			const int aItemsPerPage = SMF_ITEMS_PER_PAGE ) = 0;
	
	/**
	 * Customised method for SmfContactFetcherPlugin interface
	 * @param aRequest [out] The request data to be sent to network
	 * @param aOperation [in] The operation type (should be known between 
	 * the client interface and the plugin)
	 * @param aData [in] The data required to form the request (The type 
	 * of data should be known between client and the plugin)
	 * @return Appropriate value of the enum SmfPluginError.
	 * Plugin error if any, else SmfPluginErrNone for success
	 */
	virtual SmfPluginError customRequest( SmfPluginRequestData &aRequest, 
			const int &aOperation, QByteArray *aData ) = 0;
	};

Q_DECLARE_INTERFACE( SmfContactFetcherPlugin, "org.symbian.smf.plugin.contact.fetcher/v0.2" );

#endif /* SMFCONTACTFETCHERPLUGIN_H_ */
