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
 * Interface specification for events
 * Note: This class has dependencies on QtMobility project
 *
 */


#ifndef SMFEVENTSFETCHERPLUGIN_H_
#define SMFEVENTSFETCHERPLUGIN_H_

#include <smfpluginbase.h>
#include <smfevent.h>
#include <smflocation.h>
#include <smfcontact.h>

using namespace QtMobility;

/**
 * @ingroup smf_plugin_group
 * Interface specification for events
 *
 * All of the functionality described here should be implemented by a service
 * specific plug-in.
 * 
 * Note: This class has dependencies on QtMobility project
 */
class SmfEventsFetcherPlugin : public SmfPluginBase
	{
public:
	
	/**
	 * Destructor
	 */
	virtual ~SmfEventsFetcherPlugin( ) {}
	
	/**
	 * Method to get the events on the specified location
	 * @param aRequest [out] The request data to be sent to network
	 * @param aLocation [in] Location of the event
	 * @param aPageNum [in] The page to be extracted
	 * @param aItemsPerPage [in] Number of items per page
	 * @return Appropriate value of the enum SmfPluginError.
	 * Plugin error if any, else SmfPluginErrNone for success
	 */
	virtual SmfPluginError events( SmfPluginRequestData &aRequest,
			const SmfLocation &aLocation,
			const int aPageNum = SMF_FIRST_PAGE, 
			const int aItemsPerPage = SMF_ITEMS_PER_PAGE ) = 0;
	
	/**
	 * Method to get the venues on the specified location
	 * @param aRequest [out] The request data to be sent to network
	 * @param aLocation [in] Location of the venue
	 * @param aPageNum [in] The page to be extracted
	 * @param aItemsPerPage [in] Number of items per page
	 * @return Appropriate value of the enum SmfPluginError.
	 * Plugin error if any, else SmfPluginErrNone for success
	 */
	virtual SmfPluginError venues( SmfPluginRequestData &aRequest,
			const SmfLocation &aLocation,
			const int aPageNum = SMF_FIRST_PAGE, 
			const int aItemsPerPage = SMF_ITEMS_PER_PAGE ) = 0;
	
	/**
	 * Method to get the list of attendees for an event
	 * @param aRequest [out] The request data to be sent to network
	 * @param aEvent [in] The event for which attendees should be fetched
	 * @param aPageNum [in] The page to be extracted
	 * @param aItemsPerPage [in] Number of items per page
	 * @return Appropriate value of the enum SmfPluginError.
	 * Plugin error if any, else SmfPluginErrNone for success
	 */
	virtual SmfPluginError attendees( SmfPluginRequestData &aRequest,
			const SmfEvent &aEvent, 
			const int aPageNum = SMF_FIRST_PAGE,
			const int aItemsPerPage = SMF_ITEMS_PER_PAGE ) = 0;
	/**
	 * Method to post event(s)
	 * @param aRequest [out] The request data to be sent to network
	 * @param aEventList [in] The list of events to be posted
	 * @return Appropriate value of the enum SmfPluginError.
	 * Plugin error if any, else SmfPluginErrNone for success
	 */
	virtual SmfPluginError postEvents( SmfPluginRequestData &aRequest,
			const QList<SmfEvent> &aEventList ) = 0;
	
	/**
	 * Customised method for SmfEventsFetcherPlugin interface
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

Q_DECLARE_INTERFACE( SmfEventsFetcherPlugin, "org.symbian.smf.plugin.events.fetcher/v0.2" );

#endif /* SMFEVENTSFETCHERPLUGIN_H_ */
