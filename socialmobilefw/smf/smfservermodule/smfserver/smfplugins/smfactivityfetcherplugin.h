/**
 * Copyright (c) 2010 Sasken Communication Technologies Ltd. 
 * All rights reserved.
 * This component and the accompanying materials are made available 
 * under the terms of the "Eclipse Public License v1.0"
 * which accompanies  this distribution, and is available 
 * at the URL "http://www.eclipse.org/legal/epl-v10.html"
 * 
 * Initial Contributors:
 * Chandradeep Gandhi, Sasken Communication Technologies Ltd - 
 *
 * Contributors:
 * Manasij Roy, Nalina Hariharan
 * 
 * Description:
 * Interface specification for plugins that implements social activity related services
 *
 */
#ifndef SMFACTIVITYFETCHERPLUGIN_H_
#define SMFACTIVITYFETCHERPLUGIN_H_

#include <smfpluginbase.h>
#include <smfactions.h>

// Forward declaration
class SmfContact;

/**
 * @ingroup smf_plugin_group
 * Interface specification for fetching social activity list for a user. 
 * This class provides basic functionality to allow applications to obtain list of
 * activities like "marked as favourite", "commented on a picture" etc in a 
 * social networking service.
 *
 * All of the functionality described here should be implemented by a service
 * specific plug-in.
 */
class SmfActivityFetcherPlugin : public SmfPluginBase
	{
public:
	/**
	 * Destructor
	 */
	virtual ~SmfActivityFetcherPlugin( ) {}

	/**
	 * Method to get the list of self activities, e.g. shown in own wall
	 * @param aRequest [out] The request data that plugin generates (to be sent to network)
	 * @param aPageNum [in] The page to be extracted
	 * @param aItemsPerPage [in] Number of items per page
	 * @return Appropriate value of the enum SmfPluginError.
	 * Plugin error if any, else SmfPluginErrNone for success
	 */
	virtual SmfPluginError selfActivities( SmfPluginRequestData &aRequest,
			const int aPageNum = SMF_FIRST_PAGE, 
			const int aItemsPerPage = SMF_ITEMS_PER_PAGE ) = 0;

	/**
	 * Method to get the list of activities for other, e.g. shown in a friends wall
	 * @param aRequest [out] The request data plugin generated (to be sent to network)
	 * @param aContact [in] The contact containing the URI, name or id of the user 
	 * @param aPageNum [in] The page to be extracted
	 * @param aItemsPerPage [in] Number of items per page
	 * @return Appropriate value of the enum SmfPluginError.
	 * Plugin error if any, else SmfPluginErrNone for success
	 */
	virtual SmfPluginError friendsActivities( SmfPluginRequestData &aRequest,
			const SmfContact &aContact,			
			const int aPageNum = SMF_FIRST_PAGE, 
			const int aItemsPerPage = SMF_ITEMS_PER_PAGE ) = 0;

	/**
	 * Method to get list of self activities filtered by activity type, e.g. only photo updates in own wall
	 * @param aRequest [out] The request data plugin generated (to be sent to network)
	 * @param aFilters [in] The list of activity types to be included in result
	 * @param aPageNum [in] The page to be extracted
	 * @param aItemsPerPage[in] Number of items per page
	 * @return Appropriate value of the enum SmfPluginError.
	 * Plugin error if any, else SmfPluginErrNone for success
	 */
	virtual SmfPluginError filtered( SmfPluginRequestData &aRequest,
			QList<SmfActivityObjectType> &aFilters,
			const int aPageNum = SMF_FIRST_PAGE, 
			const int aItemsPerPage = SMF_ITEMS_PER_PAGE ) = 0;
	
	/**
	 * Customised method for SmfActivityFetcherPlugin interface
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

Q_DECLARE_INTERFACE( SmfActivityFetcherPlugin, "org.symbian.smf.plugin.activity.fetcher/v0.2" );

#endif /* SMFACTIVITYFETCHERPLUGIN_H_ */
