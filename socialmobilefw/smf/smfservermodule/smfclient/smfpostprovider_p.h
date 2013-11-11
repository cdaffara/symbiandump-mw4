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
 * Private implementation of SmfPostProvider
 *
 */

#ifndef SMFPOSTPROVIDER_P_H_
#define SMFPOSTPROVIDER_P_H_

#include <QVector>
#include <QTextFormat>
#include <smfclientglobal.h>
#include <smfglobal.h>
#include <smfobserver.h>
#include <smfpost.h>

// Forward declaration
class SmfProvider;
class SmfContact;
class SmfLocation;
class SmfPostProvider;
#ifdef Q_OS_SYMBIAN
	class CSmfClientSymbian;
#else
	class SmfClientQt;
#endif

/**
 * Data size limits
 */
/**
 * Single Post-1 MB
 */
const int KSmfPostMaxSize = 1000000;


/**
 * Private Qt wrapper
 * Provided to accomodate diff facilities in diff platforms.
 * Uses Symbian client-server for Symbian
 * QDbus for linux
 * QLocalserver for rest of the platforms
 * Note:- only symbian client-server impl is provided at the moment
 **/
class SmfPostProviderPrivate: public smfObserver
	{
	Q_OBJECT

public:
	/**
	 * Constructor
	 */
	SmfPostProviderPrivate(SmfPostProvider* postProvider=0);
	
	/**
	 * Destructor
	 */
	~SmfPostProviderPrivate();
	
public:
	/**
	 * returns maximum no of chars (unicode) that service provider can post without truncation
	 * negative value means no limit
	 */
	qint32 maxCharsInPost();
	
	/**
	 * returns maximum no of items that can be returned in a single query to getPosts
	 * negative value means feature not supported.
	 */
	qint32 maxItems();

	/**
	 * returns all the formatting of posts that this service provider supports.
	 * May return 0 items to mean only QString is supported.
	 */
	QVector<QTextFormat> supportedFormats();
	
	/**
	 * returns whether this SP supports Appearence @see SmfAppearenceInfo
	 */
	bool supportsAppearence();

	/**
	 * Gets the posts asynchronously. The signal postsAvailable()with SmfPostList is emitted
	 * once the post lists are available
	 * @param user user's contact in this SP, omit for self contact
	 * @param pageNum Page number to download, SMF_FIRST_PAGE denotes fresh query.
     * @param perPage Item per page, default is SMF_ITEMS_PER_PAGE
     * @return SmfError. SmfNoError if success, else appropriate error code 
	 * @see postsAvailable()
	 */
	SmfError posts(SmfContact* user = 0,int pageNum=SMF_FIRST_PAGE,int perPage=SMF_ITEMS_PER_PAGE);
	
	/**
	 * Updates a post to own area, the success of the post can be checked with signal
	 * updatePostFinished() signal
	 * @param postData data to be posted
	 * @param location location data
	 * @return SmfError. SmfNoError if success, else appropriate error code
	 */
	SmfError post(SmfPost& postData,SmfLocation& location) ; 
	
	/**
	 * Updates the last post to own area with new data, the success of the post can be checked with signal
	 * updatePostFinished() signal
	 * @param postData edited/new data to be posted
	 * @param location location data
	 * @return SmfError. SmfNoError if success, else appropriate error code
	 */
	SmfError updatePost(SmfPost& postData); 
	
	/**
	 * Updates a post to a particular Smf contact. the success of the post can be checked with signal
	 * updatePostFinished() signal.
	 * @param postData data to be posted
	 * @param contact contact to which the post is to be directed
	 * @param location location data
	 * @return SmfError. SmfNoError if success, else appropriate error code
	 */
	SmfError postDirected(SmfPost& postData,SmfContact& contact,SmfLocation* location=0);
	
	/**
	 * Method to post a comment on a post.
	 * @param aTarget Post on which comment has to be posted
	 * @param aComment comment to be posted
	 * @param aLocation location data
	 * @return SmfError. SmfNoError if success, else appropriate error code
	 */
	SmfError commentOnAPost( SmfPost &aTarget, SmfPost &aComment, SmfLocation *aLocation = NULL);
	
	/**
	 * Posts appearance info of the user.e.g. appear offline, busy, do-not-disturb
	 * @param appearence user appearance
	 * @see SmfPresenceInfo
	 * @return SmfError. SmfNoError if success, else appropriate error code 
	 */
	SmfError postAppearence(SmfAppearenceInfo appearence, const QString &status); 
	
	/**
	 * Share /a contact's post to user's friends and followers (e.g. retweet in twitter, share on facebook)
	 * emits updatePostFinished() signal when done.
	 * @param postData data to be posted
	 * @param contact contact to which the post belonged
	 * @param bool whether user changed items within the post
	 * @return SmfError. SmfNoError if success, else appropriate error code
	 */
	SmfError sharePost(SmfPost& postData,SmfContact& contact,bool edited);
	
	/**
	 * Request for a custom operation.
	 * @param operationId OperationId
	 * @param customData Custom data to be sent
	 * @return SmfError. SmfNoError if success, else appropriate error code
	 * Note:-Interpretation of operationId and customData is upto the concerned
	 * plugin and client application. service provider should provide some
	 * serializing-deserializing utilities for these custom data
	 */
	SmfError customRequest(const int& operationId,QByteArray* customData);
	
    /**
     * Cancels a request generated due to the call to any API which results 
     * into http request. Might return error if no request is currently pending.
     * Please note that there can be only one request pending at any point of time
     * @return Appropriate SmfError value
     */
	SmfError cancelRequest ();
   
public:	//From smfobserver 
	/**
	 * To notify availibility of asynchronous requests.
	 * @param result Requested result, before using must check error param.
	 * @param opcode Requested opcode, for which the result has arrived.
	 * @param error Error
	 */
	void resultsAvailable(QByteArray result,SmfRequestTypeID opcode,SmfError error);

private:
	SmfPostProvider* m_postProvider;
	QByteArray m_serializedDataToServer;	//serialized xtra info, order of serialization follows order of param
	quint8 m_argFlag;
#ifdef Q_OS_SYMBIAN
	CSmfClientSymbian* m_SmfClientPrivate;
	friend class CSmfClientSymbian;
#else
	SmfClientQt* m_SmfClientPrivate;
	friend class SmfClientQt;
#endif
	};

#endif /* SMFPOSTPROVIDER_P_H_ */
