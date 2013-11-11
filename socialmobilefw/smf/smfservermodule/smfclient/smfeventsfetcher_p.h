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
 * Nalina Hariharan
 *
 * Description:
 * Private implementation of events related services
 *
 */

#ifndef SMFEVENTSFETCHER_P_H_
#define SMFEVENTSFETCHER_P_H_

#include <smfglobal.h>
#include <smfobserver.h>
#include <smfevent.h>
#include <smflocation.h>
#include <smfprovider.h>

class SmfEventsFetcher;

#ifdef Q_OS_SYMBIAN
	class CSmfClientSymbian;	
#else
	class SmfClientQt;
#endif

class SmfEventsFetcherPrivate : public smfObserver
	{
	Q_OBJECT

public:
	/**
	 * Constructs SmfEventsFetcher with base provider info
	 */
	SmfEventsFetcherPrivate(SmfEventsFetcher* aEventFetcher);
	
	/**
	 * Destructor
	 */
	~SmfEventsFetcherPrivate();

public:
	/**
	 * Gets list of events in a particular venue asynchronously.
	 * eventsAvailable() signal is emitted with SmfEventsList once its arrived.
	 * When the list is big user can specify the page number and per page item data.
	 * If not supplied by the user default values are used.
	 * @param pageNum Page number to download, SMF_FIRST_PAGE denotes fresh query.
	 * @param perPage Item per page, default is SMF_ITEMS_PER_PAGE
	 * @return SmfNoError if success, else appropriate error value
	 */
	SmfError events ( SmfLocation venue, int pageNum = SMF_FIRST_PAGE,
			int perPage = SMF_ITEMS_PER_PAGE);
	
	/**
	 * Gets list of venues of a particular location asynchronously.
	 * venuesAvailable() signal is emitted with SmfLocationList once its arrived.
	 * When the list is big user can specify the page number and per page item data.
	 * If not supplied by the user default values are used.
	 * @param pageNum Page number to download, SMF_FIRST_PAGE denotes fresh query.
	 * @param perPage Item per page, default is SMF_ITEMS_PER_PAGE
	 * @return SmfNoError if success, else appropriate error value 
	 */
	SmfError venues ( SmfLocation location, int pageNum = SMF_FIRST_PAGE,
			int perPage = SMF_ITEMS_PER_PAGE);
	
	
	/**
	 * Gets a list of attendees to the event
	 * attendeesAvailable() signal is emitted with SmfContacts for the users attending this event
	 * @param event 
	 * @param pageNum Page number to download, SMF_FIRST_PAGE denotes fresh query.
	 * @param perPage Item per page, default is SMF_ITEMS_PER_PAGE
	 * @return SmfNoError if success, else appropriate error value
	 */
	SmfError attendees(SmfEvent event, int pageNum = SMF_FIRST_PAGE,
			int perPage = SMF_ITEMS_PER_PAGE); 
	
	/**
	 * Updates events. Might not be supported by all service provider.
	 * eventsUpdated() signal can be checked for success value.
	 * @param SmfEventsList List of events to be posted
	 * @return SmfNoError if success, else appropriate error value
	 */
	SmfError postEvents ( SmfEventList events );
	
	/**
	 * Request for a custom operation. The signal customDataAvailable() is emitted 
	 * when the result is available.
	 * @param operationId OperationId
	 * @param customData Custom data to be sent
	 * @return SmfNoError if success, else appropriate error value
	 * Note:-Interpretation of operationId and customData is upto the concerned
	 * plugin and client application. service provider should provide some
	 * serializing-deserializing utilities for these custom data
	 */
	SmfError customRequest ( const int& operationId, QByteArray* customData );
	
    /**
     * Cancels a request generated due to the call to any API which results 
     * into http request. Might return error if no request is currently pending.
     * Please note that there can be only one request pending at any point of time
     * @return Appropriate SmfError value
     */
	SmfError cancelRequest ();
	
public: //From smfObserver
	/**
	 * To notify availibility of asynchronous requests.
	 * @param result Requested result, before using must check error param.
	 * @param opcode Requested opcode, for which the result has arrived.
	 * @param error Error 
	 */
	void resultsAvailable(QByteArray result,SmfRequestTypeID opcode, SmfError error);

private:
	SmfEventsFetcher* m_eventFetcher;
	//serialized byte array of provider+other info to be sent to the server
	//The order:- SmfProvider then params in order of their appearance in fn
	QByteArray m_serializedDataToServer;
#ifdef Q_OS_SYMBIAN
	CSmfClientSymbian* m_SmfClientPrivate;
	friend class CSmfClientSymbian;
#else
	SmfClientQt* m_SmfClientPrivate;
	friend class SmfClientQt;
#endif
	quint8 m_argFlag;
	
	};

#endif /* SMFEVENTSFETCHER_P_H_ */
