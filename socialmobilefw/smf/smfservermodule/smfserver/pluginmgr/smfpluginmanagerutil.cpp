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
 * The Plugin Manager util class performs all plugin functional 
 * specific functionalities
 *
 */

// plugin interfaces
#include <QVector>
#include <QTextFormat>
#include <smfactivityfetcherplugin.h>
#include <smfcontactfetcherplugin.h>
#include <smfeventsfetcherplugin.h>
#include <smfpostproviderplugin.h>
#include <smflyricsserviceplugin.h>
#include <smfmusicsearchplugin.h>
#include <smfmusicserviceplugin.h>
#include <smfplaylistserviceplugin.h>
#include <smfgalleryplugin.h>
#include <smfprovider.h>

#include "smfpluginmanagerutil.h"
#include "smfpluginmanager.h"

// Static data initialisation
SmfPluginManagerUtil* SmfPluginManagerUtil::m_myInstance = NULL;


/**
 * Method to get the instance of SmfPluginManagerUtil class
 * @param aParent The parent object
 * @return The instance of SmfPluginManagerUtil class
 */
SmfPluginManagerUtil* SmfPluginManagerUtil::getInstance ( QObject *aParent )
	{
	if(NULL == m_myInstance)
		m_myInstance = new SmfPluginManagerUtil( aParent );
	return m_myInstance;
	}


/**
 * Constructor with default argument
 * @param aParent The parent object
 */
SmfPluginManagerUtil::SmfPluginManagerUtil ( QObject *aParent )
	{
	Q_UNUSED(aParent)
	}


/**
 * Destructor
 */
SmfPluginManagerUtil::~SmfPluginManagerUtil ( )
	{
	if(m_myInstance)
		delete m_myInstance;
	}


/**
 * Method called by Plugin Manager to create a web query.
 * This method calls the respective plugin methods for creation of 
 * web queries using the aOperation and aInputData parameters.
 * @param aInstance Instance of the loaded plugin that perform this operation
 * @param aOperation The type of operation to be performed
 * @param aInputData The data required to create the web query
 * @param aReqData [out] The request data created by the 
 * plugin (don't consider for synchronous requests)
 * @param aResult [out] SmfError, The result of the operation
 * It can be :-
 * SmfPluginNoError (if plugin has created the request successfully)
 * SmfPluginUnknownPluginService (if plugin service is not known or unsupported)
 * SmfPluginRequestCreationFailed (if request creation has failed)
 * @param aOutputData [out] The output data to be filled by the 
 * plugins (for synchronous request only), don't consider for asynchronous requests
 */
void SmfPluginManagerUtil::createRequest ( QObject* aInstance, 
		const SmfRequestTypeID &aOperation, 
		QByteArray &aInputData,
		SmfPluginRequestData &aReqData,
		SmfError &aResult,
		QByteArray &aOutputData )
	{
	qDebug()<<"SmfPluginManagerUtil::createRequest()";
	SmfPluginError pluginErrorVal = SmfPluginErrServiceNotSupported;
	
	// Find the type of service required
	switch(aOperation)
		{
		case SmfActivitySelfActivity:
		case SmfActivityFriendsActivities:
		case SmfActivityFiltered:
		case SmfActivityCustomRequest:
			// Authorise the plugin and call create request methods
			pluginErrorVal = createActivityFetcherRequest(aInstance, aOperation, 
					aInputData, aReqData);
			break;
			
		case SmfContactGetFriends:
		case SmfContactGetFollowers:
		case SmfContactSearch:
		case SmfContactSearchNear:
		case SmfContactGetGroups:
		case SmfContactSearchInGroup:
		case SmfContactCustomRequest:
			// Authorise the plugin and call create request methods
			pluginErrorVal = createContactFetcherRequest(aInstance, aOperation, 
					aInputData, aReqData);
			break;
			
		case SmfContactRetrievePosts:
		case SmfContactPost:
		case SmfContactUpdatePost:
		case SmfContactPostDirected:
		case SmfContactCommentOnAPost:
		case SmfContactPostAppearence:
		case SmfContactSharePost:
		case SmfContactPostCustomRequest:
			// Authorise the plugin and call create request methods
			pluginErrorVal = createContactPostRequest(aInstance, aOperation, 
					aInputData, aReqData);
			break;
			
		case SmfPostGetMaxCharsInPost:
		case SmfPostGetMaxItems:
		case SmfPostGetSupportedFormats:
		case SmfPostGetAppearanceSupport:
			pluginErrorVal = createSyncRequest(aInstance, aOperation, 
					aInputData, aOutputData);
			break;
			
		case SmfMusicGetLyrics:
		case SmfMusicGetSubtitle:
		case SmfMusicLyricsCustomRequest:
			// Authorise the plugin and call create request methods
			pluginErrorVal = createMusicLyricsRequest(aInstance, aOperation, 
					aInputData, aReqData);
			break;
	
		case SmfEventsGetEvents:
		case SmfEventsGetVenues:
		case SmfEventsGetEventAttendees:
		case SmfEventsPostEvents:
		case SmfEventsCustomRequest:
			// Authorise the plugin and call create request methods
			pluginErrorVal = createEventsFetcherRequest(aInstance, aOperation, 
					aInputData, aReqData);
			break;
			
		case SmfMusicGetRecommendations:
		case SmfMusicGetTracksSimilar:
		case SmfMusicGetTracksOfAlbum:
		case SmfMusicGetTracksOfArtist:
		case SmfMusicGetTrackInfo:
		case SmfMusicGetStores:
		case SmfMusicSearchCustomRequest:
			// Authorise the plugin and call create request methods
			pluginErrorVal = createMusicSearchRequest(aInstance, aOperation, 
					aInputData, aReqData);
			break;
			
		case SmfMusicGetUserInfo:
		case SmfMusicSearchArtist:
		case SmfMusicSearchAlbum:
		case SmfMusicSearchEvent:
		case SmfMusicSearchVenue:
		case SmfMusicSearchUser:
		case SmfMusicPostCurrentPlaying:
		case SmfMusicPostRating:
		case SmfMusicPostComment:
		case SmfMusicServiceCustomRequest:
			// Authorise the plugin and call create request methods
			pluginErrorVal = createMusicServiceRequest(aInstance, aOperation, 
					aInputData, aReqData);
			break;
			
		case SmfMusicGetPlaylists:
		case SmfMusicGetPlaylistsOfUser:
		case SmfMusicAddToPlaylist:
		case SmfMusicPostCurrentPlayingPlaylist:
		case SmfMusicPlaylistCustomRequest:
			// Authorise the plugin and call create request methods
			pluginErrorVal = createMusicPlaylistRequest(aInstance, aOperation, 
					aInputData, aReqData);
			break;
			
		case SmfPictureGetAlbums:
		case SmfPictureGetPictures:
		case SmfPictureDescription:
		case SmfPictureUpload:
		case SmfPictureMultiUpload:
		case SmfPicturePostComment:
		case SmfPictureCustomRequest:
			// Authorise the plugin and call create request methods
			pluginErrorVal = createGalleryRequest(aInstance, aOperation, 
					aInputData, aReqData);
			break;
			
		default:
			qDebug()<<"Unknown service type!!!";
			aResult = SmfPMPluginUnknownPluginService;
		}
	qDebug()<<"return value = "<<pluginErrorVal;
	
	// Convert the SmfPluginError error type to SmfError type
	convertPluginErrorType(pluginErrorVal, aResult);
	}


/**
 * Method to create a web query to fetch activities
 * @param aPlugin The instance of the loaded plugin that performs the 
 * contact fetch operation.
 * @param aOperation The type of operation to be performed
 * @param aInputData The data required to create the web query
 * @param aReqData [out] The request data created by the plugin
 * @return SmfPluginError
 */
SmfPluginError SmfPluginManagerUtil::createActivityFetcherRequest ( QObject *aPlugin, 
		const SmfRequestTypeID &aOperation, 
		QByteArray &aInputData,
		SmfPluginRequestData &aReqData )
	{
	qDebug()<<"SmfPluginManagerUtil::createActivityFetcherRequest()";
	
	// typecast instance to the activity - fetcher type, here SmfActivityFetcherPlugin
	SmfActivityFetcherPlugin *plugin = qobject_cast<SmfActivityFetcherPlugin *>(aPlugin);
	SmfPluginError pluginErrorVal = SmfPluginErrServiceNotSupported;
	
	if(plugin)
		{
		QDataStream stream(aInputData);
		int pageNum, itemPerPage;
		quint8 flag = 0;

		switch(aOperation)
			{
			case SmfActivitySelfActivity:
				qDebug()<<"Plugin Operation requested : selfactivities()";
				stream>>flag;
				stream>>pageNum;
				stream>>flag;
				stream>>itemPerPage;
				pluginErrorVal = plugin->selfActivities(aReqData, pageNum, itemPerPage);
				break;
				
			case SmfActivityFriendsActivities:
				{
				qDebug()<<"Plugin Operation requested : friendsActivities()";
				SmfContact contact;
				stream>>flag;
				stream>>contact;
				stream>>flag;
				stream>>pageNum;
				stream>>flag;
				stream>>itemPerPage;
				pluginErrorVal = plugin->friendsActivities(aReqData, contact, pageNum, itemPerPage);
				break;
				}
				
			case SmfActivityFiltered:
				{
				qDebug()<<"Plugin Operation requested : filtered()";
				QList<SmfActivityObjectType> list;
				QList<int> intList;
				stream>>flag;
				stream>>intList;
				list.clear();
				foreach(int val, intList)
					{
					SmfActivityObjectType type = (SmfActivityObjectType)val;
					list.append(type);
					}
				
				stream>>flag;
				stream>>pageNum;
				stream>>flag;
				stream>>itemPerPage;
				pluginErrorVal = plugin->filtered(aReqData, list, pageNum, itemPerPage);
				break;
				}
				
			case SmfActivityCustomRequest:
				{
				qDebug()<<"Plugin Operation requested : customRequest()";
				int operationType;
				stream>>flag;
				stream>>operationType;
				QByteArray data;
				stream>>flag;
				if(flag)
					stream>>data;
				pluginErrorVal = plugin->customRequest(aReqData, operationType, &data);
				break;
				}
				
			default:
				// do nothing, unknown service
				qDebug()<<"No API found for this operation type!!!";
			}
		}
	else
		{
		qDebug()<<"Plugin instance couldn't be typecasted!!!";
		pluginErrorVal = SmfPluginErrRequestNotCreated; 
		}
	
	return pluginErrorVal;
	}


/**
 * Method to create a web query to fetch contact details.
 * @param aPlugin The instance of the loaded plugin that performs the 
 * contact fetch operation.
 * @param aOperation The type of operation to be performed
 * @param aInputData The data required to create the web query
 * @param aReqData [out] The request data created by the plugin
 * @return SmfPluginError, it can be :-
 * SmfPluginErrNone (if request is created successfully) or
 * SmfPluginErrServiceNotSupported (if the service requested is not known or unsupported) or
 * SmfPluginErrInvalidArguments (if the arguments are invalid) or 
 * SmfPluginErrRequestNotCreated (if request could not be created) 
 */
SmfPluginError SmfPluginManagerUtil::createContactFetcherRequest ( QObject *aPlugin, 
		const SmfRequestTypeID &aOperation, 
		QByteArray &aInputData,
		SmfPluginRequestData &aReqData )
	{
	qDebug()<<"SmfPluginManagerUtil::createContactFetcherRequest()";
	
	// typecast instance to the contact - fetcher type, here SmfContactFetcherPlugin
	SmfContactFetcherPlugin *plugin = qobject_cast<SmfContactFetcherPlugin *>(aPlugin);
	SmfPluginError pluginErrorVal = SmfPluginErrServiceNotSupported;
	
	if(plugin)
		{
		QDataStream stream(aInputData);
		int pageNum, itemPerPage;
		quint8 flag = 0;

		switch(aOperation)
			{
			case SmfContactGetFriends:
				qDebug()<<"Plugin Operation requested : friends()";
				stream>>flag;
				stream>>pageNum;
				stream>>flag;
				stream>>itemPerPage;
				pluginErrorVal = plugin->friends(aReqData, pageNum, itemPerPage);
				break;
				
			case SmfContactGetFollowers:
				qDebug()<<"Plugin Operation requested : followers()";
				stream>>flag;
				stream>>pageNum;
				stream>>flag;
				stream>>itemPerPage;
				pluginErrorVal = plugin->followers(aReqData, pageNum, itemPerPage);
				break;
				
			case SmfContactSearch:
				{
				qDebug()<<"Plugin Operation requested : search()";
				SmfContact searchContact;
				stream>>flag;
				if(flag)
					stream>>searchContact;
				stream>>flag;
				stream>>pageNum;
				stream>>flag;
				stream>>itemPerPage;
				pluginErrorVal = plugin->search(aReqData, searchContact, pageNum, itemPerPage);
				break;
				}
				
			case SmfContactSearchNear:
				{
				qDebug()<<"Plugin Operation requested : searchNear()";
				SmfLocation location;
				stream>>flag;
				if(flag)
					stream>>location;
				stream>>flag;
				int i;
				stream>>i;
				SmfLocationSearchBoundary proximity = (SmfLocationSearchBoundary)i;
				stream>>flag;
				stream>>pageNum;
				stream>>flag;
				stream>>itemPerPage;
				pluginErrorVal = plugin->searchNear(aReqData, location, proximity, pageNum, itemPerPage);
				break;
				}
				
			case SmfContactGetGroups:
				{
				qDebug()<<"Plugin Operation requested : groups()";
				stream>>flag;
				stream>>pageNum;
				stream>>flag;
				stream>>itemPerPage;
				pluginErrorVal = plugin->groups(aReqData, pageNum, itemPerPage);
				break;
				}
				
			case SmfContactSearchInGroup:
				{
				qDebug()<<"Plugin Operation requested : searchInGroup()";
				SmfGroup group;
				SmfContact contact;
				bool arg = false;
				stream>>flag;
				stream>>group;
				stream>>flag;
				if(flag)
					{
					arg = true;
					stream>>contact;
					}
				stream>>flag;
				stream>>pageNum;
				stream>>flag;
				stream>>itemPerPage;
				if(arg)
					pluginErrorVal = plugin->searchInGroup(aReqData, group, &contact, pageNum, itemPerPage);
				else
					pluginErrorVal = plugin->searchInGroup(aReqData, group, NULL, pageNum, itemPerPage);
				break;
				}
				
			case SmfContactCustomRequest:
				{
				qDebug()<<"Plugin Operation requested : customRequest()";
				int operationType;
				stream>>flag;
				stream>>operationType;
				QByteArray data;
				stream>>flag;
				if(flag)
					stream>>data;
				pluginErrorVal = plugin->customRequest(aReqData, operationType, &data);
				break;
				}
				
			default:
				// do nothing, unknown service
				qDebug()<<"No API found for this operation type!!!";
			}
		}
	else
		{
		qDebug()<<"Plugin instance couldn't be typecasted!!!";
		pluginErrorVal = SmfPluginErrRequestNotCreated; 
		}
	
	return pluginErrorVal;
	}
	

/**
 * Method to create a web query to do post operation on contacts 
 * (like scraps or messages)
 * @param aPlugin The instance of the loaded plugin that performs the 
 * contact fetch operation.
 * @param aOperation The type of operation to be performed
 * @param aInputData The data required to create the web query
 * @param aReqData [out] The request data created by the plugin
 * @return SmfPluginError, it can be :-
 * SmfPluginErrNone (if request is created successfully) or
 * SmfPluginErrServiceNotSupported (if the service requested is not known or unsupported) or
 * SmfPluginErrInvalidArguments (if the arguments are invalid) or 
 * SmfPluginErrRequestNotCreated (if request could not be created) 
 */
SmfPluginError SmfPluginManagerUtil::createContactPostRequest ( QObject *aPlugin, 
		const SmfRequestTypeID &aOperation, 
		QByteArray &aInputData,
		SmfPluginRequestData &aReqData )
	{
	qDebug()<<"SmfPluginManagerUtil::createContactPostRequest";
	
	// typecast instance to SmfPostProviderPlugin
	SmfPostProviderPlugin *plugin = qobject_cast<SmfPostProviderPlugin *>(aPlugin);
	SmfPluginError pluginErrorVal = SmfPluginErrServiceNotSupported;	
	
	if(plugin)
		{
		QDataStream stream(aInputData);
		SmfContact contact;
		SmfPost post;
		SmfLocation location;
		int pageNum, itemPerPage;
		quint8 flag = 0;
		switch(aOperation)
			{
			case SmfContactRetrievePosts:
				qDebug()<<"Plugin Operation requested : retrieve()";
				stream>>flag;
				if(flag)
					stream>>contact;
				stream>>flag;
				stream>>pageNum;
				stream>>flag;
				stream>>itemPerPage;
				pluginErrorVal = plugin->retrieve(aReqData, &contact, pageNum, itemPerPage);
				break;
				
			case SmfContactPost:
				qDebug()<<"Plugin Operation requested : post()";
				stream>>flag;
				stream>>post;
				stream>>flag;
				stream>>location;
				pluginErrorVal = plugin->post(aReqData, post, location);
				break;
				
			case SmfContactUpdatePost:
				qDebug()<<"Plugin Operation requested : updatePost()";
				stream>>flag;
				stream>>post;
				pluginErrorVal = plugin->updatePost(aReqData, post);
				break;
				
			case SmfContactPostDirected:
				qDebug()<<"Plugin Operation requested : postDirected()";
				stream>>flag;
				stream>>post;
				stream>>flag;
				stream>>contact;
				stream>>flag;
				if(flag)
					stream>>location;
				qDebug()<<"Post data = "<<post.description();
				qDebug()<<"Contact ID = "<<contact.value("Guid").value<QContactGuid>().guid();
				pluginErrorVal = plugin->postDirected(aReqData, post, contact, &location);
				break;
				
			case SmfContactCommentOnAPost:
				{
				qDebug()<<"Plugin Operation requested : commentOnAPost()";
				SmfPost post2;
				stream>>flag;
				stream>>post;
				stream>>flag;
				stream>>post2;
				stream>>flag;
				if(flag)
					stream>>location;
				pluginErrorVal = plugin->commentOnAPost(aReqData, post, post2, &location);
				break;
				}
				
			case SmfContactPostAppearence:
				{
				qDebug()<<"Plugin Operation requested : postAppearence()";
				QString status;
				int i;
				stream>>flag;
				stream>>i;
				SmfPresenceInfo presence = (SmfPresenceInfo)i;
				stream>>flag;
				stream>>status;
				pluginErrorVal = plugin->postAppearence(aReqData, presence, status);
				break;
				}
				
			case SmfContactSharePost:
				{
				qDebug()<<"Plugin Operation requested : sharePost()";
				bool edited;
				stream>>flag;
				stream>>post;
				stream>>flag;
				stream>>contact;
				stream>>flag;
				stream>>edited;
				pluginErrorVal = plugin->sharePost(aReqData, post, contact, edited);
				break;
				}
				
			case SmfContactPostCustomRequest:
				{
				qDebug()<<"Plugin Operation requested : customRequest()";
				int operationType;
				stream>>flag;
				stream>>operationType;
				QByteArray data;
				stream>>flag;
				if(flag)
					stream>>data;
				pluginErrorVal = plugin->customRequest(aReqData, operationType, &data);
				break;
				}
				
			default:
				// do nothing, unknown service
				qDebug()<<"No API found for this operation type!!!";
			}
		}
	else
		{
		qDebug()<<"Plugin instance couldn't be typecasted!!!";
		pluginErrorVal = SmfPluginErrRequestNotCreated; 
		}
	
	return pluginErrorVal;
	}
	


/**
 * Method to create a web query to do music lyrics or subtitle search
 * @param aPlugin The instance of the loaded plugin that performs the 
 * contact fetch operation.
 * @param aOperation The type of operation to be performed
 * @param aInputData The data required to create the web query
 * @param aReqData [out] The request data created by the plugin
 * @return SmfPluginError, it can be :-
 * SmfPluginErrNone (if request is created successfully) or
 * SmfPluginErrServiceNotSupported (if the service requested is not known or unsupported) or
 * SmfPluginErrInvalidArguments (if the arguments are invalid) or 
 * SmfPluginErrRequestNotCreated (if request could not be created) 
 */
SmfPluginError SmfPluginManagerUtil::createMusicLyricsRequest ( QObject *aPlugin, 
		const SmfRequestTypeID &aOperation, 
		QByteArray &aInputData,
		SmfPluginRequestData &aReqData )
	{
	qDebug()<<"SmfPluginManagerUtil::createMusicLyricsRequest";
	
	// typecast instance to SmfLyricsServicePlugin
	SmfLyricsServicePlugin *plugin = qobject_cast<SmfLyricsServicePlugin *>(aPlugin);
	SmfPluginError pluginErrorVal = SmfPluginErrServiceNotSupported;
	
	if(plugin)
		{
		QDataStream stream(aInputData);
		SmfTrackInfo trackInfo;
		int pageNum, itemPerPage;
		quint8 flag = 0;

		switch(aOperation)
			{
			case SmfMusicGetLyrics:
				qDebug()<<"Plugin Operation requested : lyrics()";
				stream>>flag;
				stream>>trackInfo;
				stream>>flag;
				stream>>pageNum;
				stream>>flag;
				stream>>itemPerPage;
				pluginErrorVal = plugin->lyrics(aReqData, trackInfo, pageNum, itemPerPage);
				break;
				
			case SmfMusicGetSubtitle:
				{
				qDebug()<<"Plugin Operation requested : subtitles()";
				stream>>trackInfo;
				int i;
				stream>>flag;
				stream>>i;
				SmfSubtitleSearchFilter filter = (SmfSubtitleSearchFilter)i;
				stream>>flag;
				stream>>pageNum;
				stream>>flag;
				stream>>itemPerPage;
				pluginErrorVal = plugin->subtitles(aReqData, trackInfo, filter, pageNum, itemPerPage);
				break;
				}
				
			case SmfMusicLyricsCustomRequest:
				{
				qDebug()<<"Plugin Operation requested : customRequest()";
				int operationType;
				stream>>flag;
				stream>>operationType;
				QByteArray data;
				stream>>flag;
				if(flag)
					stream>>data;
				pluginErrorVal = plugin->customRequest(aReqData, operationType, &data);
				break;
				}
				
			default:
				// do nothing, unknown service
				qDebug()<<"No API found for this operation type!!!";
			}
		}
	else
		{
		qDebug()<<"Plugin instance couldn't be typecasted!!!";
		pluginErrorVal = SmfPluginErrRequestNotCreated; 
		}
	
	return pluginErrorVal;
	}


/**
 * Method to create a web query to do event search or event post operation 
 * @param aPlugin The instance of the loaded plugin that performs the 
 * contact fetch operation.
 * @param aOperation The type of operation to be performed
 * @param aInputData The data required to create the web query
 * @param aReqData [out] The request data created by the plugin
 * @return SmfPluginError, it can be :-
 * SmfPluginErrNone (if request is created successfully) or
 * SmfPluginErrServiceNotSupported (if the service requested is not known or unsupported) or
 * SmfPluginErrInvalidArguments (if the arguments are invalid) or 
 * SmfPluginErrRequestNotCreated (if request could not be created) 
 */
SmfPluginError SmfPluginManagerUtil::createEventsFetcherRequest ( QObject *aPlugin, 
		const SmfRequestTypeID &aOperation, 
		QByteArray &aInputData,
		SmfPluginRequestData &aReqData )
	{
	qDebug()<<"SmfPluginManagerUtil::createEventsFetcherRequest";
	
	// typecast instance to SmfEventsFetcherPlugin
	SmfEventsFetcherPlugin *plugin = qobject_cast<SmfEventsFetcherPlugin *>(aPlugin);
	SmfPluginError pluginErrorVal = SmfPluginErrServiceNotSupported;
	
	if(plugin)
		{
		QDataStream stream(aInputData);
		int pageNum, itemPerPage;
		SmfLocation location;
		quint8 flag = 0;
		
		switch(aOperation)
			{
			case SmfEventsGetEvents:
				qDebug()<<"Plugin Operation requested : events()";
				stream>>flag;
				stream>>location;
				stream>>flag;
				stream>>pageNum;
				stream>>flag;
				stream>>itemPerPage;
				pluginErrorVal = plugin->events(aReqData, location, pageNum, itemPerPage);
				break;
				
			case SmfEventsGetVenues:
				qDebug()<<"Plugin Operation requested : venues()";
				stream>>flag;
				stream>>location;
				stream>>flag;
				stream>>pageNum;
				stream>>flag;
				stream>>itemPerPage;
				pluginErrorVal = plugin->venues(aReqData, location, pageNum, itemPerPage);
				break;
				
			case SmfEventsGetEventAttendees:
				{
				qDebug()<<"Plugin Operation requested : attendees()";
				SmfEvent event;
				stream>>flag;
				stream>>event;
				stream>>flag;
				stream>>pageNum;
				stream>>flag;
				stream>>itemPerPage;
				pluginErrorVal = plugin->attendees(aReqData, event, pageNum, itemPerPage);
				break;
				}
				
			case SmfEventsPostEvents:
				{
				qDebug()<<"Plugin Operation requested : postEvents()";
				QList<SmfEvent> list;
				stream>>flag;
				stream>>list;
				pluginErrorVal = plugin->postEvents(aReqData, list);
				break;
				}
				
			case SmfEventsCustomRequest:
				{
				qDebug()<<"Plugin Operation requested : customRequest()";
				int operationType;
				stream>>flag;
				stream>>operationType;
				QByteArray data;
				stream>>flag;
				if(flag)
					stream>>data;
				pluginErrorVal = plugin->customRequest(aReqData, operationType, &data);
				break;
				}
				
			default:
				// do nothing, unknown service
				qDebug()<<"No API found for this operation type!!!";
			}
		}
	else
		{
		qDebug()<<"Plugin instance couldn't be typecasted!!!";
		pluginErrorVal = SmfPluginErrRequestNotCreated; 
		}
	
	return pluginErrorVal;
	}
	


/**
 * Method to create a web query to do music search operation 
 * @param aPlugin The instance of the loaded plugin that performs the 
 * contact fetch operation.
 * @param aOperation The type of operation to be performed
 * @param aInputData The data required to create the web query
 * @param aReqData [out] The request data created by the plugin
 * @return SmfPluginError, it can be :-
 * SmfPluginErrNone (if request is created successfully) or
 * SmfPluginErrServiceNotSupported (if the service requested is not known or unsupported) or
 * SmfPluginErrInvalidArguments (if the arguments are invalid) or 
 * SmfPluginErrRequestNotCreated (if request could not be created) 
 */
SmfPluginError SmfPluginManagerUtil::createMusicSearchRequest ( QObject *aPlugin, 
		const SmfRequestTypeID &aOperation, 
		QByteArray &aInputData,
		SmfPluginRequestData &aReqData )
	{
	qDebug()<<"SmfPluginManagerUtil::createMusicSearchRequest";
	
	// typecast instance to SmfMusicSearchPlugin
	SmfMusicSearchPlugin *plugin = qobject_cast<SmfMusicSearchPlugin *>(aPlugin);
	SmfPluginError pluginErrorVal = SmfPluginErrServiceNotSupported;
	
	if(plugin)
		{
		QDataStream stream(aInputData);
		int pageNum, itemPerPage;
		SmfTrackInfo trackInfo;
		quint8 flag = 0;

		switch(aOperation)
			{
			case SmfMusicGetRecommendations:
				qDebug()<<"Plugin Operation requested : recommendations()";
				stream>>flag;
				stream>>trackInfo;
				stream>>flag;
				stream>>pageNum;
				stream>>flag;
				stream>>itemPerPage;
				pluginErrorVal = plugin->recommendations(aReqData, trackInfo, pageNum, itemPerPage);
				break;
				
			case SmfMusicGetTracksSimilar:
				qDebug()<<"Plugin Operation requested : tracksSimilar()";
				stream>>flag;
				stream>>trackInfo;
				stream>>flag;
				stream>>pageNum;
				stream>>flag;
				stream>>itemPerPage;
				pluginErrorVal = plugin->tracksSimilar(aReqData, trackInfo, pageNum, itemPerPage);
				break;
				
			case SmfMusicGetTracksOfAlbum:
				{
				qDebug()<<"Plugin Operation requested : tracksOfAlbum()";
				SmfAlbum album;
				stream>>flag;
				stream>>album;
				stream>>flag;
				stream>>pageNum;
				stream>>flag;
				stream>>itemPerPage;
				pluginErrorVal = plugin->tracksOfAlbum(aReqData, album, pageNum, itemPerPage);
				break;
				}
				
			case SmfMusicGetTracksOfArtist:
				{
				qDebug()<<"Plugin Operation requested : tracksOfArtist()";
				SmfArtists artist;
				stream>>flag;
				stream>>artist;
				stream>>flag;
				stream>>pageNum;
				stream>>flag;
				stream>>itemPerPage;
				pluginErrorVal = plugin->tracksOfArtist(aReqData, artist, pageNum, itemPerPage);
				break;
				}
				
			case SmfMusicGetTrackInfo:
				{
				qDebug()<<"Plugin Operation requested : trackInfo()";
				SmfMusicFingerPrint fp;
				stream>>flag;
				stream>>fp;
				stream>>flag;
				stream>>pageNum;
				stream>>flag;
				stream>>itemPerPage;
				pluginErrorVal = plugin->trackInfo(aReqData, fp, pageNum, itemPerPage);
				break;
				}
				
			case SmfMusicGetStores:
				qDebug()<<"Plugin Operation requested : stores()";
				stream>>flag;
				stream>>trackInfo;
				stream>>flag;
				stream>>pageNum;
				stream>>flag;
				stream>>itemPerPage;
				pluginErrorVal = plugin->stores(aReqData, trackInfo, pageNum, itemPerPage);
				break;
				
			case SmfMusicSearchCustomRequest:
				{
				qDebug()<<"Plugin Operation requested : customRequest()";
				int operationType;
				stream>>flag;
				stream>>operationType;
				QByteArray data;
				stream>>flag;
				if(flag)
					stream>>data;
				pluginErrorVal = plugin->customRequest(aReqData, operationType, &data);
				break;
				}
				
			default:
				// do nothing, unknown service
				qDebug()<<"No API found for this operation type!!!";
			}
		}
	else
		{
		qDebug()<<"Plugin instance couldn't be typecasted!!!";
		pluginErrorVal = SmfPluginErrRequestNotCreated; 
		}
	
	return pluginErrorVal;
	}
	

/**
 * Method to create a web query to do music service request operation 
 * like getting self profile or search profiles of other users
 * @param aPlugin The instance of the loaded plugin that performs the 
 * contact fetch operation.
 * @param aOperation The type of operation to be performed
 * @param aInputData The data required to create the web query
 * @param aReqData [out] The request data created by the plugin
 * @return SmfPluginError, it can be :-
 * SmfPluginErrNone (if request is created successfully) or
 * SmfPluginErrServiceNotSupported (if the service requested is not known or unsupported) or
 * SmfPluginErrInvalidArguments (if the arguments are invalid) or 
 * SmfPluginErrRequestNotCreated (if request could not be created) 
 */
SmfPluginError SmfPluginManagerUtil::createMusicServiceRequest ( QObject *aPlugin, 
		const SmfRequestTypeID &aOperation, 
		QByteArray &aInputData,
		SmfPluginRequestData &aReqData )
	{
	qDebug()<<"SmfPluginManagerUtil::createMusicServiceRequest";
	
	// typecast instance to SmfMusicServicePlugin
	SmfMusicServicePlugin *plugin = qobject_cast<SmfMusicServicePlugin *>(aPlugin);
	SmfPluginError pluginErrorVal = SmfPluginErrServiceNotSupported;
	
	if(plugin)
		{
		QDataStream stream(aInputData);
		int pageNum, itemPerPage;
		SmfTrackInfo trackInfo;
		quint8 flag = 0;

		switch(aOperation)
			{
			case SmfMusicGetUserInfo:
				qDebug()<<"Plugin Operation requested : userMusicInfo()";
				pluginErrorVal = plugin->userMusicInfo(aReqData);
				break;
				
			case SmfMusicSearchArtist:
				{
				qDebug()<<"Plugin Operation requested : searchArtist() for Music";
				SmfArtists artist;
				stream>>flag;
				stream>>artist;
				stream>>flag;
				stream>>pageNum;
				stream>>flag;
				stream>>itemPerPage;
				pluginErrorVal = plugin->searchArtist(aReqData, artist, pageNum, itemPerPage);
				break;
				}
				
			case SmfMusicSearchAlbum:
				{
				qDebug()<<"Plugin Operation requested : searchAlbum() for Music";
				SmfAlbum album;
				stream>>flag;
				stream>>album;
				stream>>flag;
				stream>>pageNum;
				stream>>flag;
				stream>>itemPerPage;
				pluginErrorVal = plugin->searchAlbum(aReqData, album, pageNum, itemPerPage);
				break;
				}
				
			case SmfMusicSearchEvent:
				{
				qDebug()<<"Plugin Operation requested : searchEvents() for Music";
				SmfEvent event;
				stream>>flag;
				stream>>event;
				stream>>flag;
				stream>>pageNum;
				stream>>flag;
				stream>>itemPerPage;
				pluginErrorVal = plugin->searchEvents(aReqData, event, pageNum, itemPerPage);
				break;
				}
				
			case SmfMusicSearchVenue:
				{
				qDebug()<<"Plugin Operation requested : searchVenue() for Music";
				SmfLocation place;
				stream>>flag;
				stream>>place;
				stream>>flag;
				stream>>pageNum;
				stream>>flag;
				stream>>itemPerPage;
				pluginErrorVal = plugin->searchVenue(aReqData, place, pageNum, itemPerPage);
				break;
				}
				
			case SmfMusicSearchUser:
				{
				qDebug()<<"Plugin Operation requested : serachNear() for Music";
				SmfLocation place;
				stream>>flag;
				stream>>place;
				stream>>flag;
				stream>>pageNum;
				stream>>flag;
				stream>>itemPerPage;
				pluginErrorVal = plugin->searchUser(aReqData, place, pageNum, itemPerPage);
				break;
				}
				
			case SmfMusicPostCurrentPlaying:
				qDebug()<<"Plugin Operation requested : postCurrentPlaying()";
				stream>>flag;
				stream>>trackInfo;
				pluginErrorVal = plugin->postCurrentPlaying(aReqData, trackInfo);
				break;
				
			case SmfMusicPostRating:
				{
				qDebug()<<"Plugin Operation requested : postRating()";
				stream>>flag;
				stream>>trackInfo;
				SmfMusicRating rating;
				stream>>flag;
				stream>>rating;
				pluginErrorVal = plugin->postRating(aReqData, trackInfo, rating);
				break;
				}
				
			case SmfMusicPostComment:
				{
				qDebug()<<"Plugin Operation requested : postComment()";
				stream>>flag;
				stream>>trackInfo;
				SmfComment comment;
				stream>>flag;
				stream>>comment;
				pluginErrorVal = plugin->postComments(aReqData, trackInfo, comment);
				break;
				}
				
			case SmfMusicServiceCustomRequest:
				{
				qDebug()<<"Plugin Operation requested : customRequest()";
				int operationType;
				stream>>flag;
				stream>>operationType;
				QByteArray data;
				stream>>flag;
				if(flag)
					stream>>data;
				pluginErrorVal = plugin->customRequest(aReqData, operationType, &data);
				break;
				}
				
			default:
				// do nothing, unknown service
				qDebug()<<"No API found for this operation type!!!";
			}
		}
	else
		{
		qDebug()<<"Plugin instance couldn't be typecasted!!!";
		pluginErrorVal = SmfPluginErrRequestNotCreated; 
		}
	
	return pluginErrorVal;
	}
	


/**
 * Method to create a web query to do music playlist related operation
 * @param aPlugin The instance of the loaded plugin that performs the 
 * contact fetch operation.
 * @param aOperation The type of operation to be performed
 * @param aInputData The data required to create the web query
 * @param aReqData [out] The request data created by the plugin
 * @return SmfPluginError, it can be :-
 * SmfPluginErrNone (if request is created successfully) or
 * SmfPluginErrServiceNotSupported (if the service requested is not known or unsupported) or
 * SmfPluginErrInvalidArguments (if the arguments are invalid) or 
 * SmfPluginErrRequestNotCreated (if request could not be created) 
 */
SmfPluginError SmfPluginManagerUtil::createMusicPlaylistRequest ( QObject *aPlugin, 
		const SmfRequestTypeID &aOperation, 
		QByteArray &aInputData,
		SmfPluginRequestData &aReqData )
	{
	qDebug()<<"SmfPluginManagerUtil::createMusicPlaylistRequest";
	
	// typecast instance to SmfPlaylistServicePlugin
	SmfPlaylistServicePlugin *plugin = qobject_cast<SmfPlaylistServicePlugin *>(aPlugin);
	SmfPluginError pluginErrorVal = SmfPluginErrServiceNotSupported;
	
	if(plugin)
		{
		QDataStream stream(aInputData);
		int pageNum, itemPerPage;
		SmfPlaylist playlist;
		quint8 flag = 0;

		switch(aOperation)
			{
			case SmfMusicGetPlaylists:
				qDebug()<<"Plugin Operation requested : playlists()";
				stream>>flag;
				stream>>pageNum;
				stream>>flag;
				stream>>itemPerPage;
				pluginErrorVal = plugin->playlists(aReqData, pageNum, itemPerPage);
				break;
				
			case SmfMusicGetPlaylistsOfUser:
				{
				qDebug()<<"Plugin Operation requested : playlistsOf()";
				SmfContact user;
				stream>>flag;
				if(flag)
					stream>>user;
				stream>>flag;
				stream>>pageNum;
				stream>>flag;
				stream>>itemPerPage;
				pluginErrorVal = plugin->playlistsOf(aReqData, user, pageNum, itemPerPage);
				break;
				}
				
			case SmfMusicAddToPlaylist:
				{
				qDebug()<<"Plugin Operation requested : addToPlaylist()";
				QList<SmfTrackInfo> list;
				stream>>flag;
				stream>>playlist;
				stream>>flag;
				if(flag)
					stream>>list;
				pluginErrorVal = plugin->addToPlaylist(aReqData, playlist, list);
				break;
				}
				
			case SmfMusicPostCurrentPlayingPlaylist:
				qDebug()<<"Plugin Operation requested : postCurrentPlayingPlaylist()";
				stream>>flag;
				stream>>playlist;
				pluginErrorVal = plugin->postCurrentPlayingPlaylist(aReqData, playlist);
				break;
				
			case SmfMusicPlaylistCustomRequest:
				{
				qDebug()<<"Plugin Operation requested : customRequest()";
				int operationType;
				stream>>flag;
				stream>>operationType;
				QByteArray data;
				stream>>flag;
				if(flag)
					stream>>data;
				pluginErrorVal = plugin->customRequest(aReqData, operationType, &data);
				break;
				}
				
			default:
				// do nothing, unknown service
				qDebug()<<"No API found for this operation type!!!";
			}
		}
	else
		{
		qDebug()<<"Plugin instance couldn't be typecasted!!!";
		pluginErrorVal = SmfPluginErrRequestNotCreated; 
		}
	
	return pluginErrorVal;
	}
	

/**
 * Method to create a web query to do gallery related operation
 * @param aPlugin The instance of the loaded plugin that performs the 
 * contact fetch operation.
 * @param aOperation The type of operation to be performed
 * @param aInputData The data required to create the web query
 * @param aReqData [out] The request data created by the plugin
 * @return SmfPluginError, it can be :-
 * SmfPluginErrNone (if request is created successfully) or
 * SmfPluginErrServiceNotSupported (if the service requested is not known or unsupported) or
 * SmfPluginErrInvalidArguments (if the arguments are invalid) or 
 * SmfPluginErrRequestNotCreated (if request could not be created) 
 */
SmfPluginError SmfPluginManagerUtil::createGalleryRequest ( QObject *aPlugin, 
		const SmfRequestTypeID &aOperation, 
		QByteArray &aInputData,
		SmfPluginRequestData &aReqData )
	{
	qDebug()<<"SmfPluginManagerUtil::createGalleryRequest";
	
	// typecast instance to SmfGalleryPlugin
	SmfGalleryPlugin *plugin = qobject_cast<SmfGalleryPlugin *>(aPlugin);
	SmfPluginError pluginErrorVal = SmfPluginErrServiceNotSupported;
	
	if(plugin)
		{
		QDataStream stream(aInputData);
		SmfPicture picture;
		quint8 flag = 0;
		
		switch(aOperation)
			{
			
			case SmfPictureGetAlbums:
				{
				qDebug()<<"Plugin Operation requested : albums()";
				QStringList names;
				SmfContact contact;
				int pageNum, itemPerPage;
				stream>>flag;
				stream>>names;
				stream>>flag;
				if(flag)
					stream>>contact;
				stream>>pageNum;
				stream>>itemPerPage;
				pluginErrorVal = plugin->albums(aReqData, names, &contact, pageNum, itemPerPage);
				break;
				}
			
			case SmfPictureGetPictures:
				{
				qDebug()<<"Plugin Operation requested : pictures()";
				int pageNum, itemPerPage;
				SmfPictureAlbumList albumList;
				stream>>flag;
				stream>>albumList;
				stream>>flag;
				stream>>pageNum;
				stream>>flag;
				stream>>itemPerPage;
				pluginErrorVal = plugin->pictures(aReqData, albumList, pageNum, itemPerPage);
				break;
				}
				
			case SmfPictureDescription:
				qDebug()<<"Plugin Operation requested : description()";
				stream>>flag;
				stream>>picture;
				pluginErrorVal = plugin->description(aReqData, picture);
				break;
				
			case SmfPictureUpload:
				{
				qDebug()<<"Plugin Operation requested : upload() single";
				SmfPictureAlbum album;
				stream>>flag;
				if(flag)
					stream>>picture;
				stream>>flag;
				if(flag)
					stream>>album;
				pluginErrorVal = plugin->upload(aReqData, picture, &album);
				break;
				}
				
			case SmfPictureMultiUpload:
				{
				qDebug()<<"Plugin Operation requested : upload() Multiple";
				QList<SmfPicture> list;
				SmfPictureAlbum album;
				stream>>flag;
				if(flag)
					stream>>list;
				stream>>flag;
				if(flag)
					stream>>album;
				pluginErrorVal = plugin->upload(aReqData, list, &album);
				break;
				}
				
			case SmfPicturePostComment:
				{
				qDebug()<<"Plugin Operation requested : postComment()";
				SmfComment comment;
				stream>>flag;
				stream>>picture;
				stream>>flag;
				stream>>comment;
				pluginErrorVal = plugin->postComment(aReqData, picture, comment);
				break;
				}
				
			case SmfPictureCustomRequest:
				{
				qDebug()<<"Plugin Operation requested : customRequest()";
				int operationType;
				stream>>flag;
				stream>>operationType;
				QByteArray data;
				stream>>flag;
				if(flag)
					stream>>data;
				pluginErrorVal = plugin->customRequest(aReqData, operationType, &data);
				break;
				}
				
			default:
				// do nothing, unknown service
				qDebug()<<"No API found for this operation type!!!";
			}
		}
	else
		{
		qDebug()<<"Plugin instance couldn't be typecasted!!!";
		pluginErrorVal = SmfPluginErrRequestNotCreated; 
		}
	
	return pluginErrorVal;
	}
	
	
/**
 * Method called to create a synchronous plugin request.
 * @param aPlugin The instance of the loaded plugin that performs the 
 * contact fetch operation.
 * @param aOperation The type of operation to be performed
 * @param aInputData The data required to create the web query
 * @param aOutputData [out] The output data to be filled by the plugins
 * @return SmfPluginError
 */
SmfPluginError SmfPluginManagerUtil::createSyncRequest ( QObject *aPlugin, 
		const SmfRequestTypeID &aOperation, 
		QByteArray &aInputData,
		QByteArray &aOutputData )
	{
	qDebug()<<"SmfPluginManagerUtil::createSyncRequest";
	Q_UNUSED(aInputData)
	
	SmfPluginError pluginErrorVal = SmfPluginErrServiceNotSupported;	
	
	if(aPlugin)
		{
		QDataStream outStream(&aOutputData, QIODevice::ReadWrite);
			
		switch(aOperation)
			{
			case SmfPostGetMaxCharsInPost:
				{
				qDebug()<<"Plugin Operation requested : maxCharsInPost()";
				SmfPostProviderPlugin *plugin = NULL;
				plugin = qobject_cast<SmfPostProviderPlugin *>(aPlugin);
				
				if(!plugin)
					{
					qDebug()<<"Plugin instance couldn't be typecasted!!!";
					pluginErrorVal = SmfPluginErrRequestNotCreated;
					break;
					}

				qint32 val = plugin->maxCharsInPost();
				outStream<<val;
				pluginErrorVal = SmfPluginErrNone;
				break;
				}
				
			case SmfPostGetMaxItems:
				{
				qDebug()<<"Plugin Operation requested : maxItems()";
				SmfPostProviderPlugin *plugin = NULL;
				plugin = qobject_cast<SmfPostProviderPlugin *>(aPlugin);
				
				if(!plugin)
					{
					qDebug()<<"Plugin instance couldn't be typecasted!!!";
					pluginErrorVal = SmfPluginErrRequestNotCreated;
					break;
					}

				qint32 val = plugin->maxItems();
				outStream<<val;
				pluginErrorVal = SmfPluginErrNone;
				break;
				}
				
			case SmfPostGetSupportedFormats:
				{
				qDebug()<<"Plugin Operation requested : supportedFormats()";
				SmfPostProviderPlugin *plugin = NULL;
				plugin = qobject_cast<SmfPostProviderPlugin *>(aPlugin);
				
				if(!plugin)
					{
					qDebug()<<"Plugin instance couldn't be typecasted!!!";
					pluginErrorVal = SmfPluginErrRequestNotCreated;
					break;
					}

				QVector<QTextFormat> formats = plugin->supportedFormats();
				outStream<<formats;
				pluginErrorVal = SmfPluginErrNone;
				break;
				}
				
			case SmfPostGetAppearanceSupport:
				{
				qDebug()<<"Plugin Operation requested : maxItems()";
				SmfPostProviderPlugin *plugin = NULL;
				plugin = qobject_cast<SmfPostProviderPlugin *>(aPlugin);
				
				if(!plugin)
					{
					qDebug()<<"Plugin instance couldn't be typecasted!!!";
					pluginErrorVal = SmfPluginErrRequestNotCreated;
					break;
					}

				bool val = plugin->supportsAppearence();
				outStream<<val;
				pluginErrorVal = SmfPluginErrNone;
				break;
				}
				
			default:
				// do nothing, unknown service
				qDebug()<<"No API found for this operation type!!!";
			}
		}
	else
		{
		qDebug()<<"Plugin instance couldn't be typecasted!!!";
		pluginErrorVal = SmfPluginErrRequestNotCreated; 
		}
	
	return pluginErrorVal;
	}



/**
 * Method called by Plugin Manager when network response is available
 * @param aInstance The instance of the loaded plugin that requested 
 * the network operation.
 * @param aOperation The type of operation to be performed
 * @param aTransportResult The result of Transport Operation
 * @param aResponse The network response data
 * @param aResult [out] An output parameter to the plugin manager 
 * containing the data parsed by the plugins
 * @param aRetType [out] The Plugin return value
 * @param aPageResult [out] The page information filled by the plugins
 * @return SmfError, The result of the operation
 */	
SmfError SmfPluginManagerUtil::responseAvailable ( 
		SmfPluginBase *aInstance,
		const SmfRequestTypeID &aOperation,
		const SmfTransportResult &aTransportResult, 
		QByteArray *aResponse,
		QVariant *aResult,
		SmfPluginRetType &aRetType,
		SmfResultPage &aPageResult )
	{
	qDebug()<<"Inside SmfPluginManagerUtil::responseAvailable()";
	
	SmfPluginError pluginRet = SmfPluginErrServiceNotSupported;
	SmfError result = SmfPMPluginUnknownPluginService;

	switch(aOperation)
		{
		case SmfContactGetFriends:
		case SmfContactGetFollowers:
		case SmfContactSearch:
		case SmfContactSearchNear:
		case SmfContactGetGroups:
		case SmfContactSearchInGroup:
		case SmfContactCustomRequest:
		case SmfContactRetrievePosts:
		case SmfContactPost:
		case SmfContactUpdatePost:
		case SmfContactPostDirected:
		case SmfContactCommentOnAPost:
		case SmfContactPostAppearence:
		case SmfContactSharePost:
		case SmfContactPostCustomRequest:
		case SmfMusicGetLyrics:
		case SmfMusicGetSubtitle:
		case SmfMusicLyricsCustomRequest:
		case SmfEventsGetEvents:
		case SmfEventsGetVenues:
		case SmfEventsGetEventAttendees:
		case SmfEventsPostEvents:
		case SmfEventsCustomRequest:
		case SmfMusicGetRecommendations:
		case SmfMusicGetTracksSimilar:
		case SmfMusicGetTracksOfAlbum:
		case SmfMusicGetTracksOfArtist:
		case SmfMusicGetTrackInfo:
		case SmfMusicGetStores:
		case SmfMusicSearchCustomRequest:
		case SmfMusicGetUserInfo:
		case SmfMusicSearchArtist:
		case SmfMusicSearchAlbum:
		case SmfMusicSearchEvent:
		case SmfMusicSearchVenue:
		case SmfMusicSearchUser:
		case SmfMusicPostCurrentPlaying:
		case SmfMusicPostRating:
		case SmfMusicPostComment:
		case SmfMusicServiceCustomRequest:
		case SmfMusicGetPlaylists:
		case SmfMusicGetPlaylistsOfUser:
		case SmfMusicAddToPlaylist:
		case SmfMusicPostCurrentPlayingPlaylist:
		case SmfMusicPlaylistCustomRequest:
		case SmfPictureGetAlbums:
		case SmfPictureGetPictures:
		case SmfPictureDescription:
		case SmfPictureUpload:
		case SmfPictureMultiUpload:
		case SmfPicturePostComment:
		case SmfPictureCustomRequest:
		case SmfActivityFriendsActivities:
		case SmfActivityFiltered:
		case SmfActivitySelfActivity:
		case SmfActivityCustomRequest:
			{
			pluginRet = aInstance->responseAvailable(aOperation, aTransportResult, aResponse, 
					aResult, aRetType, aPageResult );
			
			// When plugin returns no error
			if( SmfPluginErrNone == pluginRet )
				{
				// Request is complete, parsed data available with aResult
				if( SmfRequestComplete == aRetType )
					{
					qDebug()<<"Parsing successful";
					result =  SmfNoError;
					}
				
				// Send the request again
				else if(SmfSendRequestAgain == aRetType )
					{
					qDebug()<<"Send request again";
					result = SmfPMPluginSendRequestAgain;
					}
				else
					convertPluginErrorType(pluginRet, result);
				}

			// Plugin returns error
			else
				{
				qDebug()<<"Plugin returned error!!!";
				convertPluginErrorType(pluginRet, result);
				if(SmfPluginErrorNetworkError == result)
					convertNetworkErrorType(aTransportResult, result);
				}
			
			break;
			}
			
		default:
			// Unknown service, saved data in Plugin manager is corrupted
			qDebug()<<"No operation type found!!!";
		}
	
	return result;
	}


/**
 * Method to serialize the result of parsing (which is done by the 
 * plugins) to QByteArray to be sent to Smf server.
 * @param aOperation The type of operation to be performed
 * @param aResult The data to be serialized
 * @param aDataStream Stream to be written
 */
void SmfPluginManagerUtil::serializeResult ( 
		const SmfRequestTypeID &aOperation, 
		QVariant* aResult,
		QDataStream &aDataStream )
	{
	qDebug()<<"SmfPluginManagerUtil::serializeResult";
	switch(aOperation)
		{
		// FOR ACTIVITY - FETCHER
		case SmfActivitySelfActivity:
		case SmfActivityFriendsActivities:
		case SmfActivityFiltered:
			{
			qDebug()<<"Serializing to : QList<SmfActivityEntry>";
			QList<SmfActivityEntry> activityList;
			if( aResult->canConvert<SmfActivityEntryList>() )
				activityList = aResult->value<SmfActivityEntryList>();
			aDataStream<<activityList;
			break;
			}
		
			
		// FOR CONTACT - FETCHER
		case SmfContactGetFriends:
		case SmfContactGetFollowers:
		case SmfContactSearch:
		case SmfContactSearchNear:
		case SmfContactSearchInGroup:
			{
			qDebug()<<"Serializing to : QList<SmfContact>";
			QList<SmfContact> contactList;
			if( aResult->canConvert<SmfContactList>() )
				contactList = aResult->value<SmfContactList>();
			aDataStream<<contactList;
			break;
			}

		case SmfContactGetGroups:
			{
			qDebug()<<"Serializing to : QList<SmfGroup>";
			QList<SmfGroup> groupList;
			if( aResult->canConvert<SmfGroupList>() )
				groupList = aResult->value<SmfGroupList>();
			aDataStream<<groupList;

			break;
			}
			
		// FOR CONTACT - POSTS
		case SmfContactRetrievePosts:
			{
			qDebug()<<"Serializing to : QList<SmfPost>";
			qDebug()<<"Serialize - retrieveposts() result";
			
			QList<SmfPost> postList;
			if( aResult->canConvert<SmfPostList>() )
				postList = aResult->value<SmfPostList>();
			aDataStream<<postList;
			break;
			}
			
		case SmfContactPost:
		case SmfContactUpdatePost:
		case SmfContactPostDirected:
		case SmfContactCommentOnAPost:
		case SmfContactPostAppearence:
		case SmfContactSharePost:
			{
			qDebug()<<"Serializing to : bool";
			bool value;
			if( QVariant::Bool == aResult->type() )
				value = aResult->toBool();
			aDataStream<<value;
			break;
			}
			
		// FOR MUSIC - LYRICS SERVICE
		case SmfMusicGetLyrics:
			{
			qDebug()<<"Serializing to : QList<SmfLyrics>";
			QList<SmfLyrics> lyricsList;
			if( aResult->canConvert<SmfLyricsList>() )
				lyricsList = aResult->value<SmfLyricsList>();
			aDataStream<<lyricsList;

			break;
			}

		case SmfMusicGetSubtitle:
			{
			qDebug()<<"Serializing to : QList<SmfSubtitle>";
			QList<SmfSubtitle> subtitleList;
			if( aResult->canConvert<SmfSubtitleList>() )
				subtitleList = aResult->value<SmfSubtitleList>();
			aDataStream<<subtitleList;

			break;
			}

		// FOR EVENTS - FETCHER
		case SmfEventsGetEvents:
			{
			qDebug()<<"Serializing to : QList<SmfEvent>";
			QList<SmfEvent> eventList;
			if( aResult->canConvert<SmfEventList>() )
				eventList = aResult->value<SmfEventList>();
			aDataStream<<eventList;

			break;
			}
			
		case SmfEventsGetVenues:
			{
			qDebug()<<"Serializing to : QList<SmfLocation>";
			QList<SmfLocation> venueList;
			if( aResult->canConvert<SmfLocationList>() )
				venueList = aResult->value<SmfLocationList>();
			aDataStream<<venueList;
			break;
			}
			
		case SmfEventsGetEventAttendees:
			{
			qDebug()<<"Serializing to : QList<SmfContact>";
			QList<SmfContact> contactList;
			if( aResult->canConvert<SmfContactList>() )
				contactList = aResult->value<SmfContactList>();
			aDataStream<<contactList;
			break;
			}
		
		case SmfEventsPostEvents:
			{
			qDebug()<<"Serializing to : bool";
			bool value;
			if( QVariant::Bool == aResult->type() )
				value = aResult->toBool();
			aDataStream<<value;
			break;
			}

		// FOR MUSIC - SEARCH
		case SmfMusicGetRecommendations:
		case SmfMusicGetTracksSimilar:
		case SmfMusicGetTracksOfAlbum:
		case SmfMusicGetTracksOfArtist:
		case SmfMusicGetTrackInfo:
			{
			qDebug()<<"Serializing to : QList<SmfTrackInfo>";
			QList<SmfTrackInfo> trackList;
			if( aResult->canConvert<SmfTrackInfoList>() )
				trackList = aResult->value<SmfTrackInfoList>();
			aDataStream<<trackList;

			break;
			}
			
		case SmfMusicGetStores:
			{
			qDebug()<<"Serializing to : QList<SmfProvider>";
			QList<SmfProvider> storeList;
			if( aResult->canConvert<SmfProviderList>() )
				storeList = aResult->value<SmfProviderList>();
			aDataStream<<storeList;
			break;
			}
			
		// FOR MUSIC - SERVICE
		case SmfMusicGetUserInfo:
			{
			qDebug()<<"Serializing to : SmfMusicProfile";
			SmfMusicProfile user;
			if(aResult->canConvert<SmfMusicProfile>())
				user = aResult->value<SmfMusicProfile>();
			aDataStream<<user;
			break;
			}
			
		case SmfMusicSearchArtist:
			{
			qDebug()<<"Serializing to : QList<SmfArtists>";
			QList<SmfArtists> artistsList;
			if( aResult->canConvert<SmfArtistsList>() )
				artistsList = aResult->value<SmfArtistsList>();
			aDataStream<<artistsList;
			break;
			}
			
		case SmfMusicSearchAlbum:
			{
			qDebug()<<"Serializing to : QList<SmfAlbum>";
			QList<SmfAlbum> albumList;
			if( aResult->canConvert<SmfAlbumList>() )
				albumList = aResult->value<SmfAlbumList>();
			aDataStream<<albumList;
			break;
			}
			
		case SmfMusicSearchEvent:
			{
			qDebug()<<"Serializing to : QList<SmfEvent>";
			QList<SmfEvent> eventList;
			if( aResult->canConvert<SmfEventList>() )
				eventList = aResult->value<SmfEventList>();
			aDataStream<<eventList;
			break;
			}
			
		case SmfMusicSearchVenue:
			{
			qDebug()<<"Serializing to : QList<SmfLocation>";
			QList<SmfLocation> venueList;
			if( aResult->canConvert<SmfLocationList>() )
				venueList = aResult->value<SmfLocationList>();
			aDataStream<<venueList;
			break;
			}
			
		case SmfMusicSearchUser:
			{
			qDebug()<<"Serializing to : QList<SmfMusicProfile>";
			QList<SmfMusicProfile> userList;
			if( aResult->canConvert<SmfMusicProfileList>() )
				userList = aResult->value<SmfMusicProfileList>();
			aDataStream<<userList;
			break;
			}
	 
		case SmfMusicPostCurrentPlaying:
		case SmfMusicPostRating:
		case SmfMusicPostComment:
			{
			qDebug()<<"Serializing to : bool";
			bool value;
			if( QVariant::Bool == aResult->type() )
				value = aResult->toBool();
			aDataStream<<value;
			break;
			}
			
			// FOR MUSIC - PLAYLIST SERVICE
		case SmfMusicGetPlaylists:
		case SmfMusicGetPlaylistsOfUser:
			{
			qDebug()<<"Serializing to : QList<SmfPlaylist>";
			QList<SmfPlaylist> playlists;
			if( aResult->canConvert<SmfPlaylistList>() )
				playlists = aResult->value<SmfPlaylistList>();
			aDataStream<<playlists;
			break;
			}
			
		case SmfMusicAddToPlaylist:
		case SmfMusicPostCurrentPlayingPlaylist:
			{
			qDebug()<<"Serializing to : bool";
			bool value;
			if( QVariant::Bool == aResult->type() )
				value = aResult->toBool();
			aDataStream<<value;
			break;
			}
				 
			// FOR PICTURES - GALLERY SERVICES
		case SmfPictureGetAlbums:
			{
			qDebug()<<"Serializing to : QList<SmfPictureAlbum>";
			QList<SmfPictureAlbum> albumList;
			if( aResult->canConvert<SmfPictureAlbumList>() )
				albumList = aResult->value<SmfPictureAlbumList>();
			aDataStream<<albumList;
			break;
			}
			
		case SmfPictureGetPictures:
			{
			qDebug()<<"Serializing to : QList<SmfPicture>";
			QList<SmfPicture> picList;
			if( aResult->canConvert<SmfPictureList>() )
				picList = aResult->value<SmfPictureList>();
			aDataStream<<picList;
			break;
			}
			
		case SmfPictureDescription:
			{
			qDebug()<<"Serializing to : QString";
			QString str;
			if( QVariant::String == aResult->type() )
				str = aResult->toString();
			aDataStream<<str;
			break;
			}
			
		case SmfPictureUpload:
		case SmfPictureMultiUpload:
		case SmfPicturePostComment:
			{
			qDebug()<<"Serializing to : bool";
			bool value;
			if( QVariant::Bool == aResult->type() )
				value = aResult->toBool();
			aDataStream<<value;
			break;
			}
			
			// FOR ALL INTERFACES CUSTOM REQUESTS
		case SmfContactCustomRequest:
		case SmfContactPostCustomRequest:
		case SmfMusicLyricsCustomRequest:
		case SmfEventsCustomRequest:
		case SmfMusicSearchCustomRequest:
		case SmfMusicServiceCustomRequest:
		case SmfMusicPlaylistCustomRequest:
		case SmfPictureCustomRequest:
		case SmfActivityCustomRequest:
			{
			qDebug()<<"Custom Requests are handled as bytearrays in PM now!!!";
			qDebug()<<"Serializing to : QByteArray";
			QByteArray customResult;
			if( QVariant::ByteArray == aResult->type() )
				customResult.append(aResult->toByteArray());
			aDataStream<<customResult;
			break;
			}
			
		default:
			// Unknown service, saved data in Plugin manager is corrupted
			qDebug()<<"No operation type found!!!";
		}
	}


/**
 * Method to convert SmfPluginError Error to the type SmfError 
 * @param aPluginError The Error code returned by the plugin
 * @param aSMFError [out] The Smf specific common error code
 */
void SmfPluginManagerUtil::convertPluginErrorType( 
		const SmfPluginError &aPluginError,  
		SmfError &aSMFError )
	{
	switch(aPluginError)
		{
		case SmfPluginErrNone:
			aSMFError = SmfNoError;
			break;
			
		case SmfPluginErrTooManyRequest:
			aSMFError = SmfPluginErrorTooManyRequest;
			break;
			
		case SmfPluginErrRequestQuotaExceeded:
			aSMFError = SmfPluginErrorRequestQuotaExceeded;
			break;
			
		case SmfPluginErrInvalidRequest:
			aSMFError = SmfPluginErrorInvalidRequest;
			break;
			
		case SmfPluginErrUserNotLoggedIn:
			aSMFError = SmfPluginErrorUserNotLoggedIn;
			break;
			
		case SmfPluginErrAuthenticationExpired:
			aSMFError = SmfPluginErrorAuthenticationExpired;
			break;
			
		case SmfPluginErrPermissionDenied:
			aSMFError = SmfPluginErrorPermissionDenied;
			break;
			
		case SmfPluginErrInvalidApplication:
			aSMFError = SmfPluginErrorInvalidApplication;
			break;
			
		case SmfPluginErrServiceUnavailable:
			aSMFError = SmfPluginErrorServiceUnavailable;
			break;
			
		case SmfPluginErrServiceTemporaryUnavailable:
			aSMFError = SmfPluginErrorServiceTemporaryUnavailable;
			break;
			
		case SmfPluginErrFormatNotSupported:
			aSMFError = SmfPluginErrorFormatNotSupported;
			break;
			
		case SmfPluginErrDataSizeExceeded:
			aSMFError = SmfPluginErrorDataSizeExceeded;
			break;
			
		case SmfPluginErrServiceNotSupported:
			aSMFError = SmfPMPluginUnknownPluginService;
			break;
			
		case SmfPluginErrInvalidArguments:
			aSMFError = SmfPluginErrorInvalidArguments;
			break;
			
		case SmfPluginErrRequestNotCreated:
			aSMFError = SmfPMPluginRequestCreationFailed;
			break;
			
		case SmfPluginErrParsingFailed:
			aSMFError = SmfPluginErrorParsingFailed;
			break;
			
		case SmfPluginErrNetworkError:
			aSMFError = SmfPluginErrorNetworkError;
			break;
			
		case SmfPluginErrCancelComplete:
			aSMFError = SmfPluginErrorCancelComplete;
			break;
			
		default:
			aSMFError = SmfUnknownError;
		}
	}

/**
 * Method to convert SmfTransportResult Error to the type SmfError 
 * @param aTMError The Error code returned by the TM
 * @param aSMFError [out] The Smf specific common error code
 */
void SmfPluginManagerUtil::convertNetworkErrorType( 
		const SmfTransportResult &aTMError,  
		SmfError &aSMFError )
	{
	switch(aTMError)
		{
		case SmfTransportOpNoError:
			aSMFError = SmfNoError;
			break;
			
		case SmfTransportOpConnectionRefusedError:
			aSMFError = SmfTMConnectionRefusedError;
			break;
			
		case SmfTransportOpRemoteHostClosedError:
			aSMFError = SmfTMRemoteHostClosedError;
			break;
			
		case SmfTransportOpHostNotFoundError:
			aSMFError = SmfTMHostNotFoundError;
			break;
			
		case SmfTransportOpTimeoutError:
			aSMFError = SmfTMTimeoutError;
			break;
			
		case SmfTransportOpOperationCanceledError:
			aSMFError = SmfTMOperationCanceledError;
			break;
			
		case SmfTransportOpSslHandshakeFailedError:
			aSMFError = SmfTMSslHandshakeFailedError;
			break;
			
		case SmfTransportOpProxyConnectionRefusedError:
			aSMFError = SmfTMProxyConnectionRefusedError;
			break;
			
		case SmfTransportOpProxyConnectionClosedError:
			aSMFError = SmfTMProxyConnectionClosedError;
			break;
			
		case SmfTransportOpProxyNotFoundError:
			aSMFError = SmfTMProxyNotFoundError;
			break;
			
		case SmfTransportOpProxyTimeoutError:
			aSMFError = SmfTMProxyTimeoutError;
			break;
			
		case SmfTransportOpProxyAuthenticationRequiredError:
			aSMFError = SmfTMProxyAuthenticationRequiredError;
			break;
			
		case SmfTransportOpContentAccessDenied:
			aSMFError = SmfTMContentAccessDenied;
			break;
			
		case SmfTransportOpContentOperationNotPermittedError:
			aSMFError = SmfTMContentOperationNotPermittedError;
			break;
			
		case SmfTransportOpContentNotFoundError:
			aSMFError = SmfTMContentNotFoundError;
			break;
			
		case SmfTransportOpAuthenticationRequiredError:
			aSMFError = SmfTMAuthenticationRequiredError;
			break;
			
		case SmfTransportOpContentReSendError:
			aSMFError = SmfTMContentReSendError;
			break;
			
		case SmfTransportOpProtocolUnknownError:
			aSMFError = SmfTMProtocolUnknownError;
			break;
			
		case SmfTransportOpProtocolInvalidOperationError:
			aSMFError = SmfTMProtocolInvalidOperationError;
			break;
			
		case SmfTransportOpUnknownNetworkError:
			aSMFError = SmfTMUnknownNetworkError;
			break;
			
		case SmfTransportOpUnknownProxyError:
			aSMFError = SmfTMUnknownProxyError;
			break;
			
		case SmfTransportOpUnknownContentError:
			aSMFError = SmfTMUnknownContentError;
			break;
			
		case SmfTransportOpProtocolFailure:
			aSMFError = SmfTMProtocolFailure;
			break;
			
		case SmfTransportOpUnknownError:
			aSMFError = SmfTMUnknownError;
			break;
			
		case SmfTransportOpIAPChanged:
			aSMFError = SmfTMIAPChanged;
			break;
			
		case SmfTransportOpCancelled:
			aSMFError = SmfTMCancelled;
			break;
			
		case SmfTransportOpUnsupportedContentEncodingFormat:
			aSMFError = SmfTMUnsupportedContentEncodingFormat;
			break;
			
		default:
			aSMFError = SmfUnknownError;
		}
	}

