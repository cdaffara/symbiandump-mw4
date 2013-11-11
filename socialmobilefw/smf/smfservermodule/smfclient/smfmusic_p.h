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
 * Private implementation of music related services
 * 
 */

#ifndef SMFMUSICPRIVATE_H_
#define SMFMUSICPRIVATE_H_

#include <qcontactgeolocation.h>
#include <smfglobal.h>
#include <smfobserver.h>
#include <smfevent.h>
#include <smfsubtitle.h>
#include <smfmusicprofile.h>
#include <smflyrics.h>
#include <smfplaylist.h>
#include <smftrackinfo.h>
#include <smflocation.h>
#include <smfprovider.h>
#include <smfartists.h>
#include <smfalbum.h>

#ifdef Q_OS_SYMBIAN
	class CSmfClientSymbian;	
#else
	class SmfClientQt;
#endif
class SmfMusicService;
class SmfMusicSearch;
class SmfPlaylistService;
class SmfLyricsService;
class SmfMusicFingerPrint;
class SmfMusicRating;
class SmfComment;
class SmfContact;


class SmfMusicServicePrivate : public smfObserver
	{
	Q_OBJECT
public:
	/**
	 * Constructor
	 */
	SmfMusicServicePrivate(SmfMusicService* aMusicService);
	
	/**
	 * Destructor
	 */
	~SmfMusicServicePrivate();
	
	/**
	 * Gets self profile information (e.g. music likings, feavorites etc) asynchronously.
	 * userMusicInfoAvailable() signal is emitted with SmfMusicProfile when the info is arrived
	 * @return SmfError. SmfNoError if success, else appropriate error code
	 */
	SmfError userMusicInfo ( );

	/**
	 * Asynchronously searches information about artisits in this service. All information 
	 * in SmfArtists is not required, however more available the better 
	 * searchArtistInfoAvailable() signal is emitted when the info is arrived.
	 * @param artist The search criteria
	 * @param pageNum Page number to download, SMF_FIRST_PAGE denotes fresh query.
	 * @param perPage Item per page, default is SMF_ITEMS_PER_PAGE
	 * @return SmfError, SmfNoError if success, else appropriate error value
	 */
	SmfError searchArtist ( SmfArtists artist, int pageNum = SMF_FIRST_PAGE,
						int perPage = SMF_ITEMS_PER_PAGE );

	/**
	 * Asynchronously searches information about albums in this service. All information 
	 * in SmfAlbum is not required, however more available the better 
	 * searchAlbumInfoAvailable() signal is emitted when the info is arrived.
	 * @param album The search criteria
	 * @param pageNum Page number to download, SMF_FIRST_PAGE denotes fresh query.
	 * @param perPage Item per page, default is SMF_ITEMS_PER_PAGE
	 * @return SmfError, SmfNoError if success, else appropriate error value
	 */
	SmfError searchAlbum ( SmfAlbum album, int pageNum = SMF_FIRST_PAGE,
						int perPage = SMF_ITEMS_PER_PAGE );
	
	/**
	 * Asynchronously searches information about events in this service. All information 
	 * in SmfEvents is not required, however more available the better 
	 * searchEventsInfoAvailable() signal is emitted when the info is arrived.
	 * @param event The search criteria
	 * @param pageNum Page number to download, SMF_FIRST_PAGE denotes fresh query.
	 * @param perPage Item per page, default is SMF_ITEMS_PER_PAGE
	 * @return SmfError, SmfNoError if success, else appropriate error value
	 */
	SmfError searchEvents ( SmfEvent event, int pageNum = SMF_FIRST_PAGE,
						int perPage = SMF_ITEMS_PER_PAGE );
	
	/**
	 * Asynchronously searches information about venue in this service. All information 
	 * in SmfVenue is not required, however more available the better 
	 * searchVenueInfoAvailable() signal is emitted when the info is arrived.
	 * @param location The search criteria
	 * @param pageNum Page number to download, SMF_FIRST_PAGE denotes fresh query.
	 * @param perPage Item per page, default is SMF_ITEMS_PER_PAGE
	 * @return SmfError, SmfNoError if success, else appropriate error value
	 */
	SmfError searchVenue ( SmfLocation location, int pageNum = SMF_FIRST_PAGE,
						int perPage = SMF_ITEMS_PER_PAGE );

	/**
	 * Asynchronously searches information about other service users for a particular venue
	 * searchUserInfoAvailable() signal is emitted with SmfMusicProfileList when the info is arrived.
	 * When the list is big user can specify the page number and per page item data.
	 * If not supplied by the user default values are used.
	 * @param location The search criteria
	 * @param pageNum Page number to download, SMF_FIRST_PAGE denotes fresh query.
	 * @param perPage Item per page, default is SMF_ITEMS_PER_PAGE
	 * @return SmfError, SmfNoError if success, else appropriate error value
	 */
	SmfError searchUser ( SmfLocation venue, int pageNum = SMF_FIRST_PAGE,
						int perPage = SMF_ITEMS_PER_PAGE);
	
	/**
	 * Posts currently playing track.
	 * Success can be checked by checking the signal postFinished()
	 * @param track Track to post
	 * @return SmfError, SmfNoError if success, else appropriate error value
	 */
	SmfError postCurrentPlaying ( SmfTrackInfo track);
	
	/**
	 * Rate a music via this API 
	 * Success can be checked by checking the signal postFinished()
	 * @param track Track on which rating should be posted
	 * @param rate Rating value
	 * @return SmfError, SmfNoError if success, else appropriate error value 
	 */
	SmfError postRating ( SmfTrackInfo track, SmfMusicRating rate);

	/**
	 * Comment on a track via this API 
	 * Success can be checked by checking the signal postFinished()
	 * @param track Track on which comment should be posted
	 * @param comment The comemnt to be posted
	 * @return SmfError, SmfNoError if success, else appropriate error value  
	 */
	SmfError postComments ( SmfTrackInfo track, SmfComment comment);

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
	
public: // from smfObserver
	/**
	 * To notify availibility of asynchronous requests.
	 * @param result Requested result, before using must check error param.
	 * @param opcode Requested opcode, for which the result has arrived.
	 * @param error Error 
	 */
	void resultsAvailable(QByteArray result,SmfRequestTypeID opcode, SmfError error);
	
private:
	SmfMusicService* m_musicService;
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


//SmfMusicSearch
class SmfMusicSearchPrivate : public smfObserver
	{
	Q_OBJECT
public:
	/**
	 * Constructor
	 */
	SmfMusicSearchPrivate(SmfMusicSearch* aMusicSearch);
	
	/**
	 * Constructor
	 */
	~SmfMusicSearchPrivate();
	
public:
	/**
	 * Searches for music recommendations similar to a particulartrack asynchronously.
	 * The signal trackSearchAvailable() is emitted with SmfTrackInfoList
	 * once its arrived.
	 * When the list is big user can specify the page number and per page item data.
	 * If not supplied by the user default values are used.
	 * @param track The track for which similar recommendations need to be fetched.
	 * @param pageNum Page number to download, SMF_FIRST_PAGE denotes fresh query.
	 * @param perPage Item per page, default is SMF_ITEMS_PER_PAGE
	 * @return SmfNoError if success, else appropriate error value
	 */
	SmfError recommendations ( SmfTrackInfo track, int pageNum = SMF_FIRST_PAGE,
			int perPage = SMF_ITEMS_PER_PAGE);
	
	/**
	 * Searches for tracks similar to a given track asynchronously.
	 * The signal trackSearchAvailable() is emitted with SmfTrackInfoList
	 * once its arrived.
	 * When the list is big user can specify the page number and per page item data.
	 * If not supplied by the user default values are used.
	 * @param track The search criteria, similar tracks are searched
	 * @param pageNum Page number to download, SMF_FIRST_PAGE denotes fresh query.
	 * @param perPage Item per page, default is SMF_ITEMS_PER_PAGE
	 * @return SmfNoError if success, else appropriate error value
	 */
	SmfError tracksSimilar ( SmfTrackInfo track, int pageNum = SMF_FIRST_PAGE,
			int perPage = SMF_ITEMS_PER_PAGE);

	/**
	 * Searches for tracks for a given album asynchronously.
	 * The signal trackSearchAvailable() is emitted with SmfTrackInfoList
	 * once its arrived.
	 * When the list is big user can specify the page number and per page item data.
	 * If not supplied by the user default values are used.
	 * @param album The album containing the id
	 * @param pageNum Page number to download, SMF_FIRST_PAGE denotes fresh query.
	 * @param perPage Item per page, default is SMF_ITEMS_PER_PAGE
	 * @return SmfNoError if success, else appropriate error value
	 */
	SmfError tracksOfAlbum ( SmfAlbum album, int pageNum = SMF_FIRST_PAGE,
			int perPage = SMF_ITEMS_PER_PAGE);
	
	/**
	 * Searches for tracks for a given artist(s) asynchronously.
	 * The signal trackSearchAvailable() is emitted with SmfTrackInfoList
	 * once its arrived.
	 * When the list is big user can specify the page number and per page item data.
	 * If not supplied by the user default values are used.
	 * @param artists The artists list
	 * @param pageNum Page number to download, SMF_FIRST_PAGE denotes fresh query.
	 * @param perPage Item per page, default is SMF_ITEMS_PER_PAGE
	 * @return SmfNoError if success, else appropriate error value
	 */
	SmfError tracksOfArtist ( SmfArtists artists, int pageNum = SMF_FIRST_PAGE,
			int perPage = SMF_ITEMS_PER_PAGE);
	
	/**
	 * Searches for a track having similar finger print asynchronously.
	 * The signal trackSearchAvailable() is emitted with SmfTrackInfoList
	 * once its arrived.
	 * @param signature The search criteria,signature to be searched for
	 * @param pageNum Page number to download, SMF_FIRST_PAGE denotes fresh query.
	 * @param perPage Item per page, default is SMF_ITEMS_PER_PAGE
	 * @return SmfNoError if success, else appropriate error value
	 */
	SmfError trackInfo ( SmfMusicFingerPrint signature, int pageNum = SMF_FIRST_PAGE,
			int perPage = SMF_ITEMS_PER_PAGE);
	
	/**
	 * Search information about where to buy this song from asynchronously.
	 * The signal storeSearchAvailable() is emitted with SmfProviderList once its arrived.
	 * @param track The search criteria for stores
	 * @param pageNum Page number to download, SMF_FIRST_PAGE denotes fresh query.
	 * @param perPage Item per page, default is SMF_ITEMS_PER_PAGE
	 * @return SmfNoError if success, else appropriate error value
	 */
	SmfError stores ( SmfTrackInfo track, int pageNum = SMF_FIRST_PAGE, 
			int perPage = SMF_ITEMS_PER_PAGE);
	
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
	SmfMusicSearch *m_musicSearch;
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


class SmfPlaylistServicePrivate : public smfObserver
	{
	Q_OBJECT

public:
	/**
	 * Constructs SmfPlaylistService with base provider info
	 */
	SmfPlaylistServicePrivate(SmfPlaylistService* aPlayLstSrvc);
	
	/**
	 * Destructor
	 */
	~SmfPlaylistServicePrivate();

public:
	/**
	 * Gets the list playlists for the logged-in user asynchronously.
	 * The signal playlistsListAvailable() signal is emitted with
	 * SmfPlaylistList once its arrived .
	 * When the list is big user can specify the page number and per page item data.
	 * If not supplied by the user default values are used.
	 * @param pageNum Page number to download, SMF_FIRST_PAGE denotes fresh query.
	 * @param perPage Item per page, default is SMF_ITEMS_PER_PAGE
	 * @return SmfNoError if success, else appropriate error value
	 */
	SmfError playlists ( int pageNum = SMF_FIRST_PAGE,
			int perPage = SMF_ITEMS_PER_PAGE);
	
	/**
	 * Gets the list playlists for the given user asynchronously.
	 * The signal playlistsListAvailable() signal is emitted with
	 * SmfPlaylistList once its arrived.
	 * When the list is big user can specify the page number and per page item data.
	 * If not supplied by the user default values are used.
	 * @param user User for which to get the playlists
	 * @param pageNum Page number to download, SMF_FIRST_PAGE denotes fresh query.
	 * @param perPage Item per page, default is SMF_ITEMS_PER_PAGE
	 * @return SmfNoError if success, else appropriate error value
	 */
	SmfError playlistsOf ( SmfContact* user, int pageNum = SMF_FIRST_PAGE,
			int perPage = SMF_ITEMS_PER_PAGE);
	
	/**
	 * Upload currently playing track to a playlist. Signal
	 * playlistUpdated() can be checked for success value
	 * @param plst The playlist to be added in
	 * @param tracks The list of tracks to uploaded
	 * @return SmfNoError if success, else appropriate error value
	 */
	SmfError addToPlaylist ( SmfPlaylist plst, SmfTrackInfoList* tracks );
	
	/**
	 * Upload currently playing playlist . Signal
	 * playlistUpdated() can be checked for success value
	 * @param plst The playlist to be uploaded
	 * @return SmfNoError if success, else appropriate error value
	 */
	SmfError postCurrentPlayingPlaylist ( SmfPlaylist plst );
	
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
	SmfPlaylistService* m_playlstSrvc;
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



/**
 * provides service ("org.symbian.smf.client.music.lyrics")
 */
class SmfLyricsServicePrivate : public smfObserver
	{
	Q_OBJECT

public:
	/**
	 * Constructs SmfLyricsService with base provider info.
	 */
	SmfLyricsServicePrivate(SmfLyricsService* aLyricsSrvc);
	
	/**
	 * Destructor
	 */
	~SmfLyricsServicePrivate();

public:
	/**
	 * Get the lyrics lists asynchrnously, it fetches texts without time info.
	 * lyricsAvailable() notification comes SmfLyricsList with when the data is available
	 * @param track Track for which lyrics needs to be fetched.
	 * @param pageNum Page number to download, SMF_FIRST_PAGE denotes fresh query.
	 * @param perPage Item per page, default is SMF_ITEMS_PER_PAGE
	 * @return SmfNoError if success, else appropriate error value
	 */
	SmfError lyrics ( SmfTrackInfo track, int pageNum = SMF_FIRST_PAGE,
			int perPage = SMF_ITEMS_PER_PAGE);
	
	/**
	 * Get the lyrics lists asynchrnously, it fetches texts with time info.
	 * Subtitle search filter can be applied
	 * subtitleAvailable() notification comes SmfSubtitleList with when the data is available
	 * @param track Track for which subtitle needs to be fetched.
	 * @param filter Subtitle search filter
	 * @param pageNum Page number to download, SMF_FIRST_PAGE denotes fresh query.
	 * @param perPage Item per page, default is SMF_ITEMS_PER_PAGE
	 * @return SmfNoError if success, else appropriate error value
	 */
	SmfError subtitles ( SmfTrackInfo track, SmfSubtitleSearchFilter filter,
			int pageNum = SMF_FIRST_PAGE, int perPage = SMF_ITEMS_PER_PAGE);
	
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
	SmfLyricsService *m_lyricsSrvc;
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

#endif
