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
 * Private Qt wrapper for SmfContactFetcher
 * 
 */

#ifndef SMFCLIENTPRIVATE_H_
#define SMFCLIENTPRIVATE_H_

#include <smfprovider.h>
#include <smfclientglobal.h>
#include <smfglobal.h>
#include <smfobserver.h>
#include <smflocation.h>
#include <smfgroup.h>

// Forward Declaration
class SmfProvider;
class SmfContact;
class SmfContactFetcher;
#ifdef Q_OS_SYMBIAN
	class CSmfClientSymbian;
#else
	class SmfClientQt;
#endif


/**
 * Private Qt wrapper
 * Provided to accomodate diff facilities in diff platforms.
 * Uses Symbian client-server for Symbian
 * QDbus for linux
 * QLocalserver for rest of the platforms
 * Note:- only symbian client-server impl is provided at the moment
 **/
class SmfContactFetcherPrivate : public smfObserver
	{
	Q_OBJECT

public:
	/**
	 * Constructor
	 * @param contactFetcher The SmfContactFetcher instance
	 */
	SmfContactFetcherPrivate ( SmfContactFetcher* contactFetcher = 0 );
	
	/**
	 * Destructor
	 */
	~SmfContactFetcherPrivate();
	//Note we need to expose the methods later through some macro
	
public:
	/**
	 * Get the friend listing asynchronously. The friendsListAvailable() signal 
	 * is emitted with SmfContactList once data is arrived. When the list is big,
	 * user can specify the page number and per page item data. If not supplied 
	 * by the user default values are used. 
	 * @param pageNum Page number to download, SMF_FIRST_PAGE denotes fresh query.
	 * @param perPage Item per page, default is SMF_ITEMS_PER_PAGE
	 * @return SmfError. SmfNoError if success, else appropriate error code
	 */
	SmfError friends ( int pageNum = SMF_FIRST_PAGE, int perPage = SMF_ITEMS_PER_PAGE );

	/**
	 * Get the list of followers asynchronously. The followersListAvailable() signal
	 * is emitted with SmfContactList once data is arrived. Please note that some
	 * service may not support followers/fans - FALSE is returned if not supported.
	 * When the list is big user can specify the page number and per page item data.
	 * If not supplied by the user default values are used.
	 * @param pageNum Page number to download, SMF_FIRST_PAGE denotes fresh query.
	 * @param perPage Item per page, default is SMF_ITEMS_PER_PAGE
	 * @return SmfError. SmfNoError if success, else appropriate error code
	 */
	SmfError followers ( int pageNum = SMF_FIRST_PAGE, int perPage = SMF_ITEMS_PER_PAGE );

	/**
	 * Searches for a contact The searchContactFinished() signal
	 * is emitted with SmfContactList once data is arrived.
	 * When the list is big user can specify the page number and per page item data.
	 * If not supplied by the user default values are used.
	 * @param contact The contact to be searched. The serach criteria must be 
	 * set as one of its fields.
	 * @param pageNum Page number to download, SMF_FIRST_PAGE denotes fresh query.
	 * @param perPage Item per page, default is SMF_ITEMS_PER_PAGE
	 * @return SmfError. SmfNoError if success, else appropriate error code
	 */
	SmfError search ( SmfContact* contact, int pageNum = SMF_FIRST_PAGE,
					int perPage = SMF_ITEMS_PER_PAGE );
	
	/**
	 * Searches for a contacts (friends) who are near the user. The signal 
	 * searchNearFinished() is emitted with SmfContactList once data is arrived.
	 * Proximity defines accuracy level. When the list is big user can specify 
	 * the page number and per page item data. If not supplied by the user 
	 * default values are used.
	 * @param location The location information
	 * @param proximity The search boundary criteria
	 * @param pageNum Page number to download, SMF_FIRST_PAGE denotes fresh query.
	 * @param perPage Item per page, default is SMF_ITEMS_PER_PAGE
	 * @return SmfError. SmfNoError if success, else appropriate error code
	 */
	SmfError searchNear ( SmfLocation* location, 
					SmfLocationSearchBoundary proximity,
					int pageNum = SMF_FIRST_PAGE,
					int perPage = SMF_ITEMS_PER_PAGE);
	
	/**
	 * Get the list of groups. The groupListAvailable() signal is emitted with 
	 * SmfGroupList once data is arrived. False might be returned if this service 
	 * doesn't support any mode of grouping (very rare). When the list is big, 
	 * user can specify the page number and per page item data. If not supplied 
	 * by the user default values are used.
	 * @param pageNum Page number to download, SMF_FIRST_PAGE denotes fresh query.
	 * @param perPage Item per page, default is SMF_ITEMS_PER_PAGE
	 * @return SmfError. SmfNoError if success, else appropriate error code
	 */
	SmfError groups ( int pageNum = SMF_FIRST_PAGE, int perPage = SMF_ITEMS_PER_PAGE );

	/**
	 * Searches for Smf Contacts in an Smf group. The signal searchInGroupFinished() 
	 * is emitted with SmfContactList once data is arrived. When the list is big user 
	 * can specify the page number and per page item data. If not supplied by the 
	 * user default values are used.
	 * @param group The group to be searched in
	 * @contact The contact to be searched, default (NULL) is the self contact.
	 * @param pageNum Page number to download, SMF_FIRST_PAGE denotes fresh query.
	 * @param perPage Item per page, default is SMF_ITEMS_PER_PAGE
	 * @return SmfError. SmfNoError if success, else appropriate error code
	 */
	SmfError searchInGroup ( SmfGroup group,
						SmfContact* contact = NULL,
						int pageNum = SMF_FIRST_PAGE,
						int perPage = SMF_ITEMS_PER_PAGE );

	/**
	 * Request for a custom operation. The signal customDataAvailable() is emitted 
	 * when the result is available.
	 * @param operationId OperationId
	 * @param customData Custom data to be sent
	 * @return SmfError. SmfNoError if success, else appropriate error code
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
   
public: //from smfobserver
	/**
	 * To notify availibility of asynchronous requests.
	 * @param result Requested result, before using must check error param.
	 * @param opcode Requested opcode, for which the result has arrived.
	 * @param error Error
	 */
	void resultsAvailable ( QByteArray result,
			SmfRequestTypeID opcode, SmfError error );
	
private:
	SmfContactFetcher* m_contactFetcher;
	QByteArray m_dataSerialized;	//serialized byte array of provider and data
	//QByteArray m_xtraInfoSerialized;	//serialized xtra info, order of serialization follows order of param
	quint8 m_argFlag;
#ifdef Q_OS_SYMBIAN
	CSmfClientSymbian* m_SmfClientPrivate;
	friend class CSmfClientSymbian;
#else
	SmfClientQt* m_SmfClientPrivate;
	friend class SmfClientQt;
#endif
	
	};

QDataStream &operator<<( QDataStream &aDataStream, const SmfError &err );
QDataStream &operator>>( QDataStream &aDataStream, SmfError &err );

#endif /* SMFCLIENTPRIVATE_H_ */
