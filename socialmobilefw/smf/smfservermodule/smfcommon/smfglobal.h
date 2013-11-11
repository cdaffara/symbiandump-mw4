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
 * SMF wide global const and macros, to be shared among SMF components 
 * and SMF clients
 *
 */

#ifndef SMFGLOBAL_H
#define SMFGLOBAL_H

#include <qglobal.h>
#include <QBuffer>
#include <QNetworkRequest>
#include <QNetworkAccessManager>

#ifdef Q_OS_SYMBIAN
class SmfServerSymbianSession;
#else
class SmfServerQtSession;
#endif

/**
 * Exact definition to be decided later
 */
#define SMF_SERVICE_NAME(INTERFACE,INTERFACENAME)

/**
 * Indicates first page for a web query
 */
const int SMF_FIRST_PAGE = 0;

/**
 * Default value for per page item for web queries
 */
const int SMF_ITEMS_PER_PAGE = 10;


/**
 * The enumeration used to denote errors reported by plugin
 * Smf can't continue without handling these errors
 */
enum SmfPluginError
	{
	SmfPluginErrNone = 0, 
	SmfPluginErrTooManyRequest,
	SmfPluginErrRequestQuotaExceeded,
	SmfPluginErrInvalidRequest,
	SmfPluginErrUserNotLoggedIn,
	SmfPluginErrAuthenticationExpired,
	SmfPluginErrPermissionDenied,
	SmfPluginErrInvalidApplication,
	SmfPluginErrServiceUnavailable,
	SmfPluginErrServiceTemporaryUnavailable,
	SmfPluginErrFormatNotSupported, 
	SmfPluginErrDataSizeExceeded,
	SmfPluginErrServiceNotSupported,
	SmfPluginErrInvalidArguments,
	SmfPluginErrRequestNotCreated,
	SmfPluginErrParsingFailed,
	SmfPluginErrNetworkError,
	SmfPluginErrCancelComplete
	};


/**
 * The enumeration used to track the plugin methods return type
 */
enum SmfPluginRetType
	{
	SmfSendRequestAgain = 0,
	SmfRequestComplete,
	SmfRequestError
	};


/**
 * The enumeration used to indicate the type of network operation done
 */
enum SmfRequestTypeID
	{
	SmfGetService,
	SmfCancelRequest,
	SmfContactGetFriends,
	SmfContactGetFollowers,
	SmfContactSearch,
	SmfContactSearchNear,
	SmfContactGetGroups,
	SmfContactSearchInGroup,
	SmfContactCustomRequest,
	SmfContactRetrievePosts,
	SmfContactPost,
	SmfContactUpdatePost,
	SmfContactPostDirected,
	SmfContactCommentOnAPost,
	SmfContactPostAppearence,
	SmfContactSharePost,
	SmfContactPostCustomRequest,
	SmfPostGetMaxCharsInPost,
	SmfPostGetMaxItems,
	SmfPostGetSupportedFormats,
	SmfPostGetAppearanceSupport,
	SmfMusicGetLyrics,
	SmfMusicGetSubtitle,
	SmfMusicLyricsCustomRequest,
	SmfMusicGetRecommendations,
	SmfMusicGetTracksSimilar,
	SmfMusicGetTracksOfAlbum,
	SmfMusicGetTracksOfArtist,
	SmfMusicGetTrackInfo,
	SmfMusicGetStores,
	SmfMusicSearchCustomRequest,
	SmfMusicGetUserInfo,
	SmfMusicSearchArtist,
	SmfMusicSearchAlbum,
	SmfMusicSearchEvent,
	SmfMusicSearchVenue,
	SmfMusicSearchUser,
	SmfMusicPostCurrentPlaying,
	SmfMusicPostRating,
	SmfMusicPostComment,
	SmfMusicServiceCustomRequest,
	SmfMusicGetPlaylists,
	SmfMusicGetPlaylistsOfUser,
	SmfMusicAddToPlaylist,
	SmfMusicPostCurrentPlayingPlaylist,
	SmfMusicPlaylistCustomRequest,
	SmfPictureGetAlbums,
	SmfPictureGetPictures,
	SmfPictureDescription,
	SmfPictureUpload,
	SmfPictureMultiUpload,
	SmfPicturePostComment,
	SmfPictureCustomRequest,
	SmfActivityFriendsActivities,
	SmfActivityFiltered,
	SmfActivitySelfActivity,
	SmfActivityCustomRequest,
	SmfEventsGetEvents,
	SmfEventsGetVenues,
	SmfEventsGetEventAttendees,
	SmfEventsPostEvents,
	SmfEventsCustomRequest,
	SmfRelationCreate,//SmfRelationMngr start
	SmfRelationAssociate,
	SmfRelationRemove,
	SmfRelationSearchById,
	SmfRelationSearchByContact,
	SmfRelationCount,
	SmfRelationGet,
	SmfRelationGetAll,
	SmfRelationGetAllRelations,
	SmfRelationDeleteRelation//SmfRelationMngr end
	
	};


/**
 * The enumeration used to indicate result of transport to the plugins
 */
enum SmfTransportResult
	{
	SmfTransportOpNoError = 0,
	SmfTransportOpConnectionRefusedError,
	SmfTransportOpRemoteHostClosedError,
	SmfTransportOpHostNotFoundError,
	SmfTransportOpTimeoutError,
	SmfTransportOpOperationCanceledError,
	SmfTransportOpSslHandshakeFailedError,
	SmfTransportOpProxyConnectionRefusedError,
	SmfTransportOpProxyConnectionClosedError,
	SmfTransportOpProxyNotFoundError,
	SmfTransportOpProxyTimeoutError,
	SmfTransportOpProxyAuthenticationRequiredError,
	SmfTransportOpContentAccessDenied,
	SmfTransportOpContentOperationNotPermittedError,
	SmfTransportOpContentNotFoundError,
	SmfTransportOpAuthenticationRequiredError,
	SmfTransportOpContentReSendError,
	SmfTransportOpProtocolUnknownError,
	SmfTransportOpProtocolInvalidOperationError,
	SmfTransportOpUnknownNetworkError,
	SmfTransportOpUnknownProxyError,
	SmfTransportOpUnknownContentError,
	SmfTransportOpProtocolFailure,
	SmfTransportOpUnknownError,
	SmfTransportOpIAPChanged,
	SmfTransportOpCancelled,
	SmfTransportOpUnsupportedContentEncodingFormat,
	SmfTransportOpGzipError
	};


/**
 * The structure used to hold the request created by the plugins
 */
struct SmfPluginRequestData
	{
	/**
	 * Indicates the type of operation performed, like getting Friends 
	 * list, upload image etc
	 */
	SmfRequestTypeID iRequestType;
	
	/**
	 * The QNetworkRequest that has to be filled up by the plugins
	 */
	QNetworkRequest iNetworkRequest;
	
	/**
	 * The data to be posted in case of HTTP POST operation
	 */
	QBuffer *iPostData;
	
	/**
	 * The type of HTTP transaction, like GET, POST etc
	 */
	QNetworkAccessManager::Operation iHttpOperationType;
	
	};

/**
 * The structure used to hold the request created by the plugins
 */
struct SmfResultPage
	{
	/**
	 * The total number of items
	 */
	uint iTotalItems;
	
	/**
	 * The number of items per page
	 */
	uint iItemsPerPage;
	
	/**
	 * The total number of pages
	 */
	uint iTotalPages;
	
	/**
	 * The current fetched page number
	 */
	uint iPageNum;
	};


struct SmfClientAuthID
	{
#ifdef Q_OS_SYMBIAN
	TSecureId pid;
	SmfServerSymbianSession* session;
#else
	QString pid;
	SmfServerQtSession* session;
#endif
	};

/**
 * Smf wide errors
 */
enum SmfError
	{
	SmfNoError = 0,
	SmfInvalidInterface,						//1
	SmfNoAuthorizedPlugin,						//2
	SmfClientAuthFailed,						//3
	SmfPMPluginNotFound,						//4
	SmfPMPluginNotLoaded,						//5
	SmfPMPluginLoadError,						//6
	SmfPMPluginNotAuthorised,					//7
	SmfPMPluginRequestCreationFailed,			//8
	SmfPMPluginUnknownPluginService,			//9
	SmfPMPluginUnknownHttpService,				//10
	SmfPMPluginRequestSendingFailed,			//11
	SmfPMPluginSOPCheckFailed,					//12
	SmfPMPluginSendRequestAgain,				//13
	SmfPluginErrorTooManyRequest,				//14
	SmfPluginErrorRequestQuotaExceeded,			//15
	SmfPluginErrorInvalidRequest,				//16
	SmfPluginErrorUserNotLoggedIn,				//17
	SmfPluginErrorAuthenticationExpired,		//18
	SmfPluginErrorPermissionDenied,				//19
	SmfPluginErrorInvalidApplication,			//20
	SmfPluginErrorServiceUnavailable,			//21
	SmfPluginErrorServiceTemporaryUnavailable,	//22
	SmfPluginErrorFormatNotSupported, 			//23
	SmfPluginErrorDataSizeExceeded ,			//24
	SmfPluginErrorInvalidArguments,				//25
	SmfPluginErrorParsingFailed,				//26
	SmfPluginErrorNetworkError,					//27
	SmfPluginErrorCancelComplete,				//28
	SmfTMConnectionRefusedError,				//29
	SmfTMRemoteHostClosedError,					//30
	SmfTMHostNotFoundError,						//31
	SmfTMTimeoutError,							//32
	SmfTMOperationCanceledError,				//33
	SmfTMSslHandshakeFailedError,				//34
	SmfTMProxyConnectionRefusedError,			//35
	SmfTMProxyConnectionClosedError,			//36
	SmfTMProxyNotFoundError,					//37
	SmfTMProxyTimeoutError,						//38
	SmfTMProxyAuthenticationRequiredError,		//39
	SmfTMContentAccessDenied,					//40
	SmfTMContentOperationNotPermittedError,		//41
	SmfTMContentNotFoundError,					//42
	SmfTMAuthenticationRequiredError,			//43
	SmfTMContentReSendError,					//44
	SmfTMProtocolUnknownError,					//45
	SmfTMProtocolInvalidOperationError,			//46
	SmfTMUnknownNetworkError,					//47
	SmfTMUnknownProxyError,						//48
	SmfTMUnknownContentError,					//49
	SmfTMProtocolFailure,						//50
	SmfTMUnknownError,							//51
	SmfTMIAPChanged,							//52
	SmfTMCancelled,								//53
	SmfTMUnsupportedContentEncodingFormat,		//54
	SmfTMInitNetworkNotAvailable ,				//55
	SmfTMInitRoamingNetworkUsageNotEnabled,		//56
	SmfTMGzipMemoryError,						//57
	SmfTMGzipStreamError,						//58
	SmfTMGzipDataError,							//59
	SmfMemoryAllocationFailure,					//60
	SmfDbOpeningError,                          //61
	SmfDbQueryExecutonError,					//61
	SmfDbContactNotExist,						//63
	SmfErrItemNotInRelation,                    //64
	SmfErrInvalidRelation,                      //65  
	SmfUnknownError								//66

	};


/**
 * Smf wide panics
 */
enum SmfPanic
	{
	SmfRequestPending		//to allow one outstanding request per session
	};

//interface names
const QString contactFetcherInterface("org.symbian.smf.plugin.contact.fetcher\0.2");
const QString postProviderInterface("org.symbian.smf.plugin.contact.posts\0.2");
const QString galleryInterface("org.symbian.smf.plugin.gallery\0.2");
const QString musicServiceInterface("org.symbian.smf.plugin.music.service\0.2");
const QString musicSearchInterface("org.symbian.smf.client.music.search\0.2");
const QString playlistServiceInterface("org.symbian.smf.plugin.music.playlist\0.2");
const QString musicEventServiceInterface("org.symbian.smf.plugin.music.events\0.2");
const QString lyricsServiceInterface("org.symbian.smf.plugin.music.lyrics\0.2");
const QString activityFetcherInterface("org.symbian.smf.plugin.activity.fetcher\0.2");
const QString eventFetcherInterface("org.symbian.smf.plugin.events.fetcher\0.2");

#endif /* SMFGLOBAL_H_ */
