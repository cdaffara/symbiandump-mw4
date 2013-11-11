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
 * Private implementation for SmfGallery class 
 *
 */

#ifndef SMFGALLERY_P_H
#define SMFGALLERY_P_H

#include <smfclientglobal.h>
#include <smfglobal.h>
#include <smfobserver.h>
#include <smfpicture.h>
#include <smfpicturealbum.h>

// Forward Declarations
class SmfProvider;
class SmfGallery;
class SmfContact;
class SmfComment;
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
class SmfGalleryPrivate : public smfObserver
	{
	Q_OBJECT

public:
	/**
	 * Constructor
	 */
	SmfGalleryPrivate(SmfGallery* gallery = 0);
	
	/**
	 * Destructor
	 */
	~SmfGalleryPrivate();
  
	/**
	 * Get the album listing asynchronously. The albumsAvailable() signal is 
	 * emitted with SmfPictureAlbumList once the albums have arrived.
	 * When the list is big user can specify the page number and per page item data.
	 * If not supplied by the user default values are used.
	 * @param names the subject or any keywords to be used to filter albums with that name
	 * @param user the user whose albums are requested 
	 * @param pageNum Page number to download, SMF_FIRST_PAGE denotes fresh query.
	 * @param perPage Item per page, default is SMF_ITEMS_PER_PAGE
	 * @return SmfError. SmfNoError if success, else appropriate error code
	 */
	SmfError albums ( QStringList names, SmfContact* user, 
					int pageNum = SMF_FIRST_PAGE,
					int perPage = SMF_ITEMS_PER_PAGE );
		
	/**
	 * Get the picture listing asynchronously. The picturesAvailable() signal is 
	 * emitted with SmfPictureList once the pictures have arrived.
	 * When the list is big user can specify the page number and per page item data.
	 * If not supplied by the user default values are used.
	 * @param albums album(s) whose pictures are being requested
	 * @param pageNum Page number to download, SMF_FIRST_PAGE denotes fresh query.
	 * @param perPage Item per page, default is SMF_ITEMS_PER_PAGE
	 * @return SmfError. SmfNoError if success, else appropriate error code
	 */
	SmfError pictures ( SmfPictureAlbumList &albums, 
					int pageNum = SMF_FIRST_PAGE,
					int perPage = SMF_ITEMS_PER_PAGE );

	/**
	 * Returns a user title/caption for the picture
	 * @return SmfError. SmfNoError if success, else appropriate error code
	 */
	SmfError description ( SmfPicture& picture );

	/**
	 * Upload an image.Implemented as slot to connect to UI controls more easily
	 * uploadFinished() signal is emitted with the success value of the upload
	 * @param image the image to be uploaded
	 * @param album the optional destination album name
	 * @return SmfError. SmfNoError if success, else appropriate error code
	 */
	SmfError upload ( SmfPicture* image, SmfPictureAlbum* album = NULL );

	/**
	 * Upload an list image.Implemented as slot to connect to UI controls more easily
	 * uploadFinished() signal is emitted with the success value of the upload
	 * @param images the list image to be uploaded
	 * @param album the optional destination album name
	 * @return SmfError. SmfNoError if success, else appropriate error code
	 */
	SmfError upload ( SmfPictureList* images, SmfPictureAlbum* album = NULL );

	/**
	 * Posts a comment for an image. uploadFinished() signal is emitted
	 * with success of the post once comment is posted.
	 * @param image Image to comment on
	 * @param comment Comment to post
	 * @return SmfError. SmfNoError if success, else appropriate error code
	 */
	SmfError postComment ( SmfPicture image, SmfComment comment );
	
	/**
	 * Request for a custom operation.
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
	
public:// from smfobserver
	/**
	 * To notify availibility of asynchronous requests.
	 * @param result Requested result, before using must check error param.
	 * @param opcode Requested opcode, for which the result has arrived.
	 * @param error Error
	 */
	void resultsAvailable(QByteArray result,SmfRequestTypeID opcode,SmfError error);
	
private:
	SmfGallery* m_gallery;
	QByteArray m_serializedDataToServer;//serialized byte array of provider+other info to be sent to the server
	quint8 m_argFlag;
#ifdef Q_OS_SYMBIAN
	CSmfClientSymbian* m_SmfClientPrivate;
	friend class CSmfClientSymbian;
#else
	SmfClientQt* m_SmfClientPrivate;
	friend class SmfClientQt;
#endif

	};

#endif // SMFGALLERY_P_H
