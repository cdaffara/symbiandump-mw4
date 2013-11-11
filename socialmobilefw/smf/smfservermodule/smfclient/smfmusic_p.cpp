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

#include <qdebug.h>
#include <smfmusicrating.h>
#include <smfcomment.h>
#include <smfmusicfingerprint.h>
#include <smfcontact.h>

#include "smfmusic.h"
#include "smfmusic_p.h"
#ifdef Q_OS_SYMBIAN
	#include "smfclientsymbian.h"
#else
	#include "smfclientqt.h"
#endif


//SmfMusicServicePrivate start
SmfMusicServicePrivate::SmfMusicServicePrivate(SmfMusicService* aMusicService)
		:m_musicService(aMusicService)
	{
#ifdef Q_OS_SYMBIAN
	//private impl for symbian
	m_SmfClientPrivate = CSmfClientSymbian::NewL(this);
#else
	//TODO:- Use private impl for other platforms or else Qt impl
#endif
	}

SmfMusicServicePrivate::~SmfMusicServicePrivate()
	{
	if(m_SmfClientPrivate)
		{
		delete m_SmfClientPrivate;
		m_SmfClientPrivate = NULL;
		}
	}

SmfError SmfMusicServicePrivate::userMusicInfo()
	{
	SmfError err = SmfNoError;
	//We need to pass Opcode and SmfProvider serialized into bytearray 
	SmfProvider* m_baseProvider = m_musicService->getProvider();
	m_serializedDataToServer.clear();
	QDataStream write(&m_serializedDataToServer,QIODevice::WriteOnly);
	write<<*m_baseProvider;
	
	QString intfName(musicServiceInterface);
	int maxalloc = SmfMusicProfileMaxSize;
	
	//call private impl's send method
	m_SmfClientPrivate->sendRequest(m_serializedDataToServer, intfName, 
			SmfMusicGetUserInfo, maxalloc);
	return err;
	}

SmfError SmfMusicServicePrivate::searchArtist ( SmfArtists artist, int pageNum, int perPage )
	{
	SmfError err = SmfNoError;
	//We need to pass Opcode and SmfProvider serialized into bytearray 
	SmfProvider* m_baseProvider = m_musicService->getProvider();
	m_serializedDataToServer.clear();
	QDataStream write(&m_serializedDataToServer,QIODevice::WriteOnly);
	write<<*m_baseProvider;
	
	QByteArray dataToPlugins;
	QDataStream streamToPlugin(&dataToPlugins, QIODevice::WriteOnly);
	m_argFlag = 1;
	streamToPlugin<<m_argFlag;
	streamToPlugin<<artist;
	streamToPlugin<<m_argFlag;
	streamToPlugin<<pageNum;
	streamToPlugin<<m_argFlag;
	streamToPlugin<<perPage;
	
	write<<dataToPlugins;
	
	QString intfName(musicServiceInterface);
	int maxalloc = SmfMusicProfileMaxSize*perPage;
	
	//call private impl's send method
	m_SmfClientPrivate->sendRequest(m_serializedDataToServer, intfName,
			SmfMusicSearchArtist, maxalloc);
	return err;
	}

SmfError SmfMusicServicePrivate::searchAlbum ( SmfAlbum album, int pageNum, int perPage )
	{
	SmfError err = SmfNoError;
	//We need to pass Opcode and SmfProvider serialized into bytearray 
	SmfProvider* m_baseProvider = m_musicService->getProvider();
	m_serializedDataToServer.clear();
	QDataStream write(&m_serializedDataToServer,QIODevice::WriteOnly);
	write<<*m_baseProvider;
	
	QByteArray dataToPlugins;
	QDataStream streamToPlugin(&dataToPlugins, QIODevice::WriteOnly);
	m_argFlag = 1;
	streamToPlugin<<m_argFlag;
	streamToPlugin<<album;
	streamToPlugin<<m_argFlag;
	streamToPlugin<<pageNum;
	streamToPlugin<<m_argFlag;
	streamToPlugin<<perPage;
	
	write<<dataToPlugins;
	
	QString intfName(musicServiceInterface);
	int maxalloc = SmfMusicProfileMaxSize*perPage;
	
	//call private impl's send method
	m_SmfClientPrivate->sendRequest(m_serializedDataToServer, intfName,
			SmfMusicSearchAlbum, maxalloc);
	return err;
	}


SmfError SmfMusicServicePrivate::searchEvents ( SmfEvent event, int pageNum, int perPage )
	{
	SmfError err = SmfNoError;
	//We need to pass Opcode and SmfProvider serialized into bytearray 
	SmfProvider* m_baseProvider = m_musicService->getProvider();
	m_serializedDataToServer.clear();
	QDataStream write(&m_serializedDataToServer,QIODevice::WriteOnly);
	write<<*m_baseProvider;
	
	QByteArray dataToPlugins;
	QDataStream streamToPlugin(&dataToPlugins, QIODevice::WriteOnly);
	m_argFlag = 1;
	streamToPlugin<<m_argFlag;
	streamToPlugin<<event;
	streamToPlugin<<m_argFlag;
	streamToPlugin<<pageNum;
	streamToPlugin<<m_argFlag;
	streamToPlugin<<perPage;
	
	write<<dataToPlugins;
	
	QString intfName(musicServiceInterface);
	int maxalloc = SmfMusicProfileMaxSize*perPage;
	
	//call private impl's send method
	m_SmfClientPrivate->sendRequest(m_serializedDataToServer, intfName,
			SmfMusicSearchEvent, maxalloc);
	return err;
	}

SmfError SmfMusicServicePrivate::searchVenue ( SmfLocation location, int pageNum, int perPage )
	{
	SmfError err = SmfNoError;
	//We need to pass Opcode and SmfProvider serialized into bytearray 
	SmfProvider* m_baseProvider = m_musicService->getProvider();
	m_serializedDataToServer.clear();
	QDataStream write(&m_serializedDataToServer,QIODevice::WriteOnly);
	write<<*m_baseProvider;
	
	QByteArray dataToPlugins;
	QDataStream streamToPlugin(&dataToPlugins, QIODevice::WriteOnly);
	m_argFlag = 1;
	streamToPlugin<<m_argFlag;
	streamToPlugin<<location;
	streamToPlugin<<m_argFlag;
	streamToPlugin<<pageNum;
	streamToPlugin<<m_argFlag;
	streamToPlugin<<perPage;
	
	write<<dataToPlugins;
	
	QString intfName(musicServiceInterface);
	int maxalloc = SmfMusicProfileMaxSize*perPage;
	
	//call private impl's send method
	m_SmfClientPrivate->sendRequest(m_serializedDataToServer, intfName,
			SmfMusicSearchVenue, maxalloc);
	return err;
	}
	
SmfError SmfMusicServicePrivate::searchUser(SmfLocation venue, int pageNum, int perPage)
	{
	SmfError err = SmfNoError;
	//We need to pass Opcode and SmfProvider serialized into bytearray 
	SmfProvider* m_baseProvider = m_musicService->getProvider();
	m_serializedDataToServer.clear();
	QDataStream write(&m_serializedDataToServer,QIODevice::WriteOnly);
	write<<*m_baseProvider;
	
	QByteArray dataToPlugins;
	QDataStream streamToPlugin(&dataToPlugins, QIODevice::WriteOnly);
	m_argFlag = 1;
	streamToPlugin<<m_argFlag;
	streamToPlugin<<venue;
	streamToPlugin<<m_argFlag;
	streamToPlugin<<pageNum;
	streamToPlugin<<m_argFlag;
	streamToPlugin<<perPage;
	
	write<<dataToPlugins;
	
	QString intfName(musicServiceInterface);
	int maxalloc = SmfMusicProfileMaxSize*perPage;
	
	//call private impl's send method
	m_SmfClientPrivate->sendRequest(m_serializedDataToServer, intfName,
	SmfMusicSearchUser, maxalloc);
	return err;
	}

SmfError SmfMusicServicePrivate::customRequest ( const int& operationId, QByteArray* customData )
	{
	SmfError err = SmfNoError;
	//We need to pass Opcode and SmfProvider serialized into bytearray 
	SmfProvider* m_baseProvider = m_musicService->getProvider();
	m_serializedDataToServer.clear();
	QDataStream write(&m_serializedDataToServer,QIODevice::WriteOnly);
	write<<*m_baseProvider;
	
	QByteArray dataToPlugins;
	QDataStream streamToPlugin(&dataToPlugins, QIODevice::WriteOnly);
	
	m_argFlag = 1;
	streamToPlugin<<m_argFlag;
	streamToPlugin<<operationId;
	if(customData)
		{
		streamToPlugin<<m_argFlag;
		streamToPlugin<<*customData;
		}
	else
		{
		m_argFlag = 0;
		streamToPlugin<<m_argFlag;
		}
	
	write<<dataToPlugins;
	
	QString intfName(musicServiceInterface);
	//ToDo:- How much size to allocate for custo data? keeping SmfMusicProfileMaxSize for now
	int maxAlloc = SmfMusicProfileMaxSize;
	
	//call private impl's send method
	m_SmfClientPrivate->sendRequest(m_serializedDataToServer, intfName,
			SmfMusicServiceCustomRequest, maxAlloc);
	return err;
	}

SmfError SmfMusicServicePrivate::postCurrentPlaying(SmfTrackInfo track) 
	{
	SmfError err = SmfNoError;
	//We need to pass Opcode and SmfProvider serialized into bytearray 
	SmfProvider* m_baseProvider = m_musicService->getProvider();
	m_serializedDataToServer.clear();
	QDataStream write(&m_serializedDataToServer,QIODevice::WriteOnly);
	write<<*(m_baseProvider);
	
	QByteArray dataToPlugins;
	QDataStream streamToPlugin(&dataToPlugins, QIODevice::WriteOnly);
	
	m_argFlag = 1;
	streamToPlugin<<m_argFlag;
	streamToPlugin<<track;
	
	write<<dataToPlugins;
	
	QString intfName(musicServiceInterface);
	//TODO:-revisit all the maxalloc
	int maxalloc = 1000;
	
	//call private impl's send method
	m_SmfClientPrivate->sendRequest(m_serializedDataToServer, intfName,
			SmfMusicPostCurrentPlaying, maxalloc);
	return err;
	}

SmfError SmfMusicServicePrivate::postRating(SmfTrackInfo track, SmfMusicRating rate) 
	{
	SmfError err = SmfNoError;
	//We need to pass Opcode and SmfProvider serialized into bytearray 
	SmfProvider* m_baseProvider = m_musicService->getProvider();
	m_serializedDataToServer.clear();
	QDataStream write(&m_serializedDataToServer,QIODevice::WriteOnly);
	write<<*(m_baseProvider);
	
	QByteArray dataToPlugins;
	QDataStream streamToPlugin(&dataToPlugins, QIODevice::WriteOnly);
	
	m_argFlag = 1;
	streamToPlugin<<m_argFlag;
	streamToPlugin<<track;
	streamToPlugin<<m_argFlag;
	streamToPlugin<<rate;
	
	write<<dataToPlugins;

	QString intfName(musicServiceInterface);
	//TODO:-revisit all the maxalloc
	int maxalloc = 1000;
	
	//call private impl's send method
	m_SmfClientPrivate->sendRequest(m_serializedDataToServer, intfName,
	SmfMusicPostRating, maxalloc);
	return err;
	}

SmfError SmfMusicServicePrivate::postComments(SmfTrackInfo track, SmfComment comment) 
	{
	SmfError err = SmfNoError;
	//We need to pass Opcode and SmfProvider serialized into bytearray 
	SmfProvider* m_baseProvider = m_musicService->getProvider();
	m_serializedDataToServer.clear();
	QDataStream write(&m_serializedDataToServer,QIODevice::WriteOnly);
	write<<*(m_baseProvider);
	
	QByteArray dataToPlugins;
	QDataStream streamToPlugin(&dataToPlugins, QIODevice::WriteOnly);
	
	m_argFlag = 1;
	streamToPlugin<<m_argFlag;
	streamToPlugin<<track;
	streamToPlugin<<m_argFlag;
	streamToPlugin<<comment;
	
	write<<dataToPlugins;
	
	QString intfName(musicServiceInterface);
	//TODO:-revisit all the maxalloc
	int maxalloc = 1000;
	
	//call private impl's send method
	m_SmfClientPrivate->sendRequest(m_serializedDataToServer, intfName,
			SmfMusicPostComment, maxalloc);
	return err;
	}


SmfError SmfMusicServicePrivate::cancelRequest()
	{
	qDebug()<<"Inside SmfMusicServicePrivate::cancelRequest()";
	QByteArray notused;
	QByteArray retData = m_SmfClientPrivate->sendSyncRequest(notused,SmfCancelRequest,1000, notused);
	
	//De-serialize it into SmfError
	QDataStream reader(&retData,QIODevice::ReadOnly);
	int val;
	reader>>val;
	SmfError error = (SmfError) val;
	return error;
	}

void SmfMusicServicePrivate::resultsAvailable(QByteArray result, SmfRequestTypeID opcode, SmfError error)
	{
	//note:- "result" is serialized and we need to de-serialize it as per opcode
	//Order of serialization Error value followed by data
	
	QDataStream reader(&result,QIODevice::ReadOnly);
	
	//Now de-serialize it based on opcode
	switch(opcode)
		{
		case SmfMusicGetUserInfo:
			{
			SmfMusicProfile* m_profile = new SmfMusicProfile;
			reader>>*m_profile;

			emit m_musicService->userMusicInfoAvailable(m_profile,error);
			break;
			}
			
		case SmfMusicSearchArtist:
			{
			SmfArtistsList *m_List = new SmfArtistsList;
			reader>>*m_List;
			
			//ToDo :- not incorporating paging now
			SmfResultPage page;

			emit m_musicService->searchArtistInfoAvailable(m_List, error, page);
			break;
			}
			
		case SmfMusicSearchAlbum:
			{
			SmfAlbumList *m_List = new SmfAlbumList;
			reader>>*m_List;
			
			//ToDo :- not incorporating paging now
			SmfResultPage page;

			emit m_musicService->searchAlbumInfoAvailable(m_List, error, page);
			break;
			}
			
		case SmfMusicSearchEvent:
			{
			SmfEventList *m_List = new SmfEventList;
			reader>>*m_List;
			
			//ToDo :- not incorporating paging now
			SmfResultPage page;

			emit m_musicService->searchEventsInfoAvailable(m_List, error, page);
			break;
			}
		case SmfMusicSearchVenue:
			{
			SmfLocationList *m_List = new SmfLocationList;
			reader>>*m_List;
			
			//ToDo :- not incorporating paging now
			SmfResultPage page;

			emit m_musicService->searchVenueInfoAvailable(m_List, error, page);
			break;
			}
				
		case SmfMusicSearchUser:
			{
			SmfMusicProfileList* m_profileList = new SmfMusicProfileList;
			reader>>*m_profileList;
			
			//ToDo :- not incorporating paging now
			SmfResultPage page;

			emit m_musicService->searchUserInfoAvailable(m_profileList, error, page);
			break;
			}
			
		case SmfMusicPostCurrentPlaying:
		case SmfMusicPostRating:
		case SmfMusicPostComment:
			{
			emit m_musicService->postfinished(error);
			break;
			}
			
		case SmfMusicServiceCustomRequest:
   			{
   			int operationId;
   			QByteArray *data = new QByteArray;
   			reader>>operationId;
   			reader>>*data;
   			qDebug()<<"operationId = "<<operationId;
   			qDebug()<<"data size = "<<data->size();
   			emit m_musicService->customDataAvailable(operationId, data);
   			break;
   			}
   			
		default:
			User::Panic(_L("Music Service Private = "),opcode);
		}
	}
//SmfMusicServicePrivate start


//SmfMusicSearchPrivate start
SmfMusicSearchPrivate::SmfMusicSearchPrivate(SmfMusicSearch *aMusicSearch)
		: m_musicSearch(aMusicSearch)
	{
#ifdef Q_OS_SYMBIAN
	//private impl for symbian
	m_SmfClientPrivate = CSmfClientSymbian::NewL(this);
#else
	//TODO:- Use private impl for other platforms or else Qt impl
#endif
	}

SmfMusicSearchPrivate::~SmfMusicSearchPrivate()
	{
	if(m_SmfClientPrivate)
		{
		delete m_SmfClientPrivate;
		m_SmfClientPrivate = NULL;
		}
	}

SmfError SmfMusicSearchPrivate::recommendations(SmfTrackInfo track, int pageNum, int perPage)
	{
	SmfError err = SmfNoError;
	//We need to pass Opcode and SmfProvider serialized into bytearray
	m_serializedDataToServer.clear();
	SmfProvider* m_baseProvider = m_musicSearch->getProvider();
	QDataStream write(&m_serializedDataToServer,QIODevice::WriteOnly);
	write<<*m_baseProvider;
	
	QByteArray dataToPlugins;
	QDataStream streamToPlugin(&dataToPlugins, QIODevice::WriteOnly);
	
	m_argFlag = 1;
	streamToPlugin<<m_argFlag;
	streamToPlugin<<track;
	streamToPlugin<<m_argFlag;
	streamToPlugin<<pageNum;
	streamToPlugin<<m_argFlag;
	streamToPlugin<<perPage;
	
	write<<dataToPlugins;
		
	QString intfName(musicSearchInterface);
	//TODO:-revisit all the maxalloc
	int maxalloc = SmfMusicTracksMaxSize*perPage;
	
	//call private impl's send method
	m_SmfClientPrivate->sendRequest(m_serializedDataToServer, intfName,
			SmfMusicGetRecommendations, maxalloc);
	return err;
	}

SmfError SmfMusicSearchPrivate::tracksSimilar ( SmfTrackInfo track, int pageNum, int perPage )
	{
	SmfError err = SmfNoError;
	//We need to pass Opcode and SmfProvider serialized into bytearray
	m_serializedDataToServer.clear();
	SmfProvider* m_baseProvider = m_musicSearch->getProvider();
	QDataStream write(&m_serializedDataToServer,QIODevice::WriteOnly);
	write<<*m_baseProvider;
	
	
	QByteArray dataToPlugins;
	QDataStream streamToPlugin(&dataToPlugins, QIODevice::WriteOnly);
	
	m_argFlag = 1;
	streamToPlugin<<m_argFlag;
	streamToPlugin<<track;
	streamToPlugin<<m_argFlag;
	streamToPlugin<<pageNum;
	streamToPlugin<<m_argFlag;
	streamToPlugin<<perPage;
	
	write<<dataToPlugins;
	
	QString intfName(musicSearchInterface);
	//TODO:-revisit all the maxalloc
	int maxalloc = SmfMusicTracksMaxSize*perPage;
	
	//call private impl's send method
	m_SmfClientPrivate->sendRequest(m_serializedDataToServer, intfName,
			SmfMusicGetTracksSimilar, maxalloc);
	return err;
	}

SmfError SmfMusicSearchPrivate::tracksOfAlbum ( SmfAlbum album, int pageNum, int perPage )
	{
	SmfError err = SmfNoError;
	//We need to pass Opcode and SmfProvider serialized into bytearray
	m_serializedDataToServer.clear();
	SmfProvider* m_baseProvider = m_musicSearch->getProvider();
	QDataStream write(&m_serializedDataToServer,QIODevice::WriteOnly);
	write<<*m_baseProvider;

	QByteArray dataToPlugins;
	QDataStream streamToPlugin(&dataToPlugins, QIODevice::WriteOnly);
	
	m_argFlag = 1;
	streamToPlugin<<m_argFlag;
	streamToPlugin<<album;
	streamToPlugin<<m_argFlag;
	streamToPlugin<<pageNum;
	streamToPlugin<<m_argFlag;
	streamToPlugin<<perPage;
	
	write<<dataToPlugins;
		
	QString intfName(musicSearchInterface);
	//TODO:-revisit all the maxalloc
	int maxalloc = SmfMusicTracksMaxSize*perPage;
	
	//call private impl's send method
	m_SmfClientPrivate->sendRequest(m_serializedDataToServer, intfName,
			SmfMusicGetTracksOfAlbum, maxalloc);
	return err;
	}

SmfError SmfMusicSearchPrivate::tracksOfArtist ( SmfArtists artists, int pageNum, int perPage )
	{
	SmfError err = SmfNoError;
	//We need to pass Opcode and SmfProvider serialized into bytearray
	m_serializedDataToServer.clear();
	SmfProvider* m_baseProvider = m_musicSearch->getProvider();
	QDataStream write(&m_serializedDataToServer,QIODevice::WriteOnly);
	write<<*m_baseProvider;
	
	QByteArray dataToPlugins;
	QDataStream streamToPlugin(&dataToPlugins, QIODevice::WriteOnly);
	
	m_argFlag = 1;
	streamToPlugin<<m_argFlag;
	streamToPlugin<<artists;
	streamToPlugin<<m_argFlag;
	streamToPlugin<<pageNum;
	streamToPlugin<<m_argFlag;
	streamToPlugin<<perPage;
	
	write<<dataToPlugins;
		
	QString intfName(musicSearchInterface);
	//TODO:-revisit all the maxalloc
	int maxalloc = SmfMusicTracksMaxSize*perPage;
	
	//call private impl's send method
	m_SmfClientPrivate->sendRequest(m_serializedDataToServer, intfName,
			SmfMusicGetTracksOfArtist, maxalloc);
	return err;
	}

SmfError SmfMusicSearchPrivate::trackInfo(SmfMusicFingerPrint signature, int pageNum, int perPage)
	{
	SmfError err = SmfNoError;
	//We need to pass Opcode and SmfProvider serialized into bytearray 
	SmfProvider* m_baseProvider = m_musicSearch->getProvider();
	m_serializedDataToServer.clear();
	QDataStream write(&m_serializedDataToServer,QIODevice::WriteOnly);
	write<<*m_baseProvider;

	QByteArray dataToPlugins;
	QDataStream streamToPlugin(&dataToPlugins, QIODevice::WriteOnly);
	
	m_argFlag = 1;
	streamToPlugin<<m_argFlag;
	streamToPlugin<<signature;
	streamToPlugin<<m_argFlag;
	streamToPlugin<<pageNum;
	streamToPlugin<<m_argFlag;
	streamToPlugin<<perPage;
	
	write<<dataToPlugins;
	
	QString intfName(musicSearchInterface);
	//TODO:-revisit all the maxalloc
	int maxalloc = SmfMusicTracksMaxSize*perPage;
	
	//call private impl's send method
	m_SmfClientPrivate->sendRequest(m_serializedDataToServer, intfName, 
			SmfMusicGetTrackInfo, maxalloc);
	return err;
	}

SmfError SmfMusicSearchPrivate::stores(SmfTrackInfo track, int pageNum, int perPage)
	{
	SmfError err = SmfNoError;
	//We need to pass Opcode and SmfProvider serialized into bytearray 
	SmfProvider* m_baseProvider = m_musicSearch->getProvider();
	m_serializedDataToServer.clear();
	QDataStream write(&m_serializedDataToServer,QIODevice::WriteOnly);
	write<<*(m_baseProvider);

	QByteArray dataToPlugins;
	QDataStream streamToPlugin(&dataToPlugins, QIODevice::WriteOnly);
	
	m_argFlag = 1;
	streamToPlugin<<m_argFlag;
	streamToPlugin<<track;
	streamToPlugin<<m_argFlag;
	streamToPlugin<<pageNum;
	streamToPlugin<<m_argFlag;
	streamToPlugin<<perPage;
	
	write<<dataToPlugins;
	
	QString intfName(musicSearchInterface);
	/** @TODO:-revisit all the maxalloc */
	int maxalloc = 1000*perPage;
	
	//call private impl's send method
	m_SmfClientPrivate->sendRequest(m_serializedDataToServer, intfName,
			SmfMusicGetStores, maxalloc);
	return err;
	}

SmfError SmfMusicSearchPrivate::customRequest ( const int& operationId, QByteArray* customData )
	{
	SmfError err = SmfNoError;
	//We need to pass Opcode and SmfProvider serialized into bytearray 
	SmfProvider* m_baseProvider = m_musicSearch->getProvider();
	m_serializedDataToServer.clear();
	QDataStream write(&m_serializedDataToServer,QIODevice::WriteOnly);
	write<<*m_baseProvider;

	QByteArray dataToPlugins;
	QDataStream streamToPlugin(&dataToPlugins, QIODevice::WriteOnly);
	
	m_argFlag = 1;
	streamToPlugin<<m_argFlag;
	streamToPlugin<<operationId;
	if(customData)
		{
		streamToPlugin<<m_argFlag;
		streamToPlugin<<*customData;
		}
	else
		{
		m_argFlag = 0;
		streamToPlugin<<m_argFlag;
		}
	
	write<<dataToPlugins;
	
	QString intfName(musicSearchInterface);
	/** @TODO:-revisit all the maxalloc */
	int maxalloc = 10000;
	
	//call private impl's send method
	m_SmfClientPrivate->sendRequest(m_serializedDataToServer, intfName,
			SmfMusicSearchCustomRequest, maxalloc);
	return err;
	}

SmfError SmfMusicSearchPrivate::cancelRequest()
	{
	qDebug()<<"Inside SmfMusicSearchPrivate::cancelRequest()";
	QByteArray notused;
	QByteArray retData = m_SmfClientPrivate->sendSyncRequest(notused,SmfCancelRequest,1000, notused);
	
	//De-serialize it into SmfError
	QDataStream reader(&retData,QIODevice::ReadOnly);
	int val;
	reader>>val;
	SmfError error = (SmfError) val;
	return error;
	}


void SmfMusicSearchPrivate::resultsAvailable(QByteArray result, SmfRequestTypeID opcode, SmfError error)
	{
	//note:- "result" is serialized and we need to de-serialize it as per opcode
	//Order of serialization Error value followed by data
	
	QDataStream reader(&result,QIODevice::ReadOnly);
	
	//Now de-serialize it based on opcode
	switch(opcode)
		{
		case SmfMusicGetRecommendations:
		case SmfMusicGetTracksSimilar:
		case SmfMusicGetTracksOfAlbum:
		case SmfMusicGetTracksOfArtist:
		case SmfMusicGetTrackInfo:
				{
				SmfTrackInfoList* m_trackInfoList = new SmfTrackInfoList;
				reader>>*(m_trackInfoList);
				
				//ToDo :- not incorporating paging now
				SmfResultPage page;
	
				emit m_musicSearch->trackSearchAvailable(m_trackInfoList,error,page);
				break;
				}
				
		case SmfMusicGetStores:
			{
			SmfProviderList* m_providers = new SmfProviderList;
			reader>>*(m_providers);
			
			//ToDo :- not incorporating paging now
			SmfResultPage page;

			emit m_musicSearch->storeSearchAvailable(m_providers,error,page);
			break;
			}

		case SmfMusicSearchCustomRequest:
   			{
   			int operationId;
   			QByteArray *data = new QByteArray;
   			reader>>operationId;
   			reader>>*data;
   			qDebug()<<"operationId = "<<operationId;
   			qDebug()<<"data size = "<<data->size();
   			emit m_musicSearch->customDataAvailable(operationId, data);
   			break;
   			}

		default:
			User::Panic(_L("Music Search Private = "),opcode);
		}
	}
//SmfMusicSearchPrivate end


//SmfPlaylistServicePrivate start
SmfPlaylistServicePrivate::SmfPlaylistServicePrivate(SmfPlaylistService *aPlayLstSrvc)
		: m_playlstSrvc(aPlayLstSrvc)
	{
#ifdef Q_OS_SYMBIAN
	//private impl for symbian
	m_SmfClientPrivate = CSmfClientSymbian::NewL(this);
#else
	//TODO:- Use private impl for other platforms or else Qt impl
#endif
	}

SmfPlaylistServicePrivate::~SmfPlaylistServicePrivate()
	{
	if(m_SmfClientPrivate)
		{
		delete m_SmfClientPrivate;
		m_SmfClientPrivate = NULL;
		}	
	}

SmfError SmfPlaylistServicePrivate::playlists(int pageNum, int perPage)
	{
	SmfError err = SmfNoError;
	//We need to pass Opcode and SmfProvider serialized into bytearray 
	SmfProvider* m_baseProvider = m_playlstSrvc->getProvider();
	m_serializedDataToServer.clear();
	QDataStream write(&m_serializedDataToServer,QIODevice::WriteOnly);
	write<<*(m_baseProvider);

	QByteArray dataToPlugins;
	QDataStream streamToPlugin(&dataToPlugins, QIODevice::WriteOnly);
	
	m_argFlag = 1;
	streamToPlugin<<m_argFlag;
	streamToPlugin<<pageNum;
	streamToPlugin<<m_argFlag;
	streamToPlugin<<perPage;
		
	write<<dataToPlugins;
	
	QString intfName(playlistServiceInterface);
	//TODO:-revisit all the maxalloc
	int maxalloc = SmfMusicPlaylistMaxSize*perPage;
	
	//call private impl's send method
	m_SmfClientPrivate->sendRequest(m_serializedDataToServer, intfName,
			SmfMusicGetPlaylists, maxalloc);
	return err;
	}

SmfError SmfPlaylistServicePrivate::playlistsOf(SmfContact *user, int pageNum, int perPage)
	{
	SmfError err = SmfNoError;
	//We need to pass Opcode and SmfProvider serialized into bytearray 
	SmfProvider* m_baseProvider = m_playlstSrvc->getProvider();
	m_serializedDataToServer.clear();
	QDataStream write(&m_serializedDataToServer,QIODevice::WriteOnly);
	write<<*(m_baseProvider);
	
	QByteArray dataToPlugins;
	QDataStream streamToPlugin(&dataToPlugins, QIODevice::WriteOnly);
	
	if(user)
		{
		m_argFlag = 1;
		streamToPlugin<<m_argFlag;
		streamToPlugin<<*user;
		}
	else
		{
		m_argFlag = 0;
		streamToPlugin<<m_argFlag;
		}
	m_argFlag = 1;
	streamToPlugin<<m_argFlag;
	streamToPlugin<<pageNum;
	streamToPlugin<<m_argFlag;
	streamToPlugin<<perPage;
		
	write<<dataToPlugins;
		
	QString intfName(playlistServiceInterface);
	//TODO:-revisit all the maxalloc
	int maxalloc = SmfMusicPlaylistMaxSize*perPage;
	
	//call private impl's send method
	m_SmfClientPrivate->sendRequest(m_serializedDataToServer, intfName,
			SmfMusicGetPlaylistsOfUser, maxalloc);
	return err;
	}

SmfError SmfPlaylistServicePrivate::addToPlaylist(SmfPlaylist plst, SmfTrackInfoList *tracks)
	{
	SmfError err = SmfNoError;
	//We need to pass Opcode and SmfProvider serialized into bytearray 
	SmfProvider* m_baseProvider = m_playlstSrvc->getProvider();
	m_serializedDataToServer.clear();
	QDataStream write(&m_serializedDataToServer,QIODevice::WriteOnly);
	write<<*(m_baseProvider);
	
	QByteArray dataToPlugins;
	QDataStream streamToPlugin(&dataToPlugins, QIODevice::WriteOnly);
	
	m_argFlag = 1;
	streamToPlugin<<m_argFlag;
	streamToPlugin<<plst;
	if(tracks)
		{
		streamToPlugin<<m_argFlag;
		streamToPlugin<<*tracks;
		}
	else
		{
		m_argFlag = 0;
		streamToPlugin<<m_argFlag;
		}
		
	write<<dataToPlugins;
		
	QString intfName(playlistServiceInterface);
	//TODO:-revisit all the maxalloc
	int maxalloc = 1000;
	
	//call private impl's send method
	m_SmfClientPrivate->sendRequest(m_serializedDataToServer, intfName,
			SmfMusicAddToPlaylist, maxalloc);
	return err;
	}

SmfError SmfPlaylistServicePrivate::postCurrentPlayingPlaylist(SmfPlaylist plst)
	{
	SmfError err = SmfNoError;
	//We need to pass Opcode and SmfProvider serialized into bytearray 
	SmfProvider* m_baseProvider = m_playlstSrvc->getProvider();
	m_serializedDataToServer.clear();
	QDataStream write(&m_serializedDataToServer,QIODevice::WriteOnly);
	write<<*(m_baseProvider);
	
	QByteArray dataToPlugins;
	QDataStream streamToPlugin(&dataToPlugins, QIODevice::WriteOnly);
	
	m_argFlag = 1;
	streamToPlugin<<m_argFlag;
	streamToPlugin<<plst;
		
	write<<dataToPlugins;
	
	QString intfName(playlistServiceInterface);
	//TODO:-revisit all the maxalloc
	int maxalloc = 1000;
	
	//call private impl's send method
	m_SmfClientPrivate->sendRequest(m_serializedDataToServer, intfName,
			SmfMusicPostCurrentPlayingPlaylist, maxalloc);
	return err;
	}

SmfError SmfPlaylistServicePrivate::customRequest ( const int& operationId, QByteArray* customData )
	{
	SmfError err = SmfNoError;
	//We need to pass Opcode and SmfProvider serialized into bytearray 
	SmfProvider* m_baseProvider = m_playlstSrvc->getProvider();
	m_serializedDataToServer.clear();
	QDataStream write(&m_serializedDataToServer,QIODevice::WriteOnly);
	write<<*m_baseProvider;

	QByteArray dataToPlugins;
	QDataStream streamToPlugin(&dataToPlugins, QIODevice::WriteOnly);
	
	m_argFlag = 1;
	streamToPlugin<<m_argFlag;
	streamToPlugin<<operationId;
	if(customData)
		{
		streamToPlugin<<m_argFlag;
		streamToPlugin<<*customData;
		}
	else
		{
		m_argFlag = 0;
		streamToPlugin<<m_argFlag;
		}
	
	write<<dataToPlugins;
	
	QString intfName(playlistServiceInterface);
	//TODO:-revisit all the maxalloc
	int maxalloc = 10000;
	
	//call private impl's send method
	m_SmfClientPrivate->sendRequest(m_serializedDataToServer, intfName,
			SmfMusicPlaylistCustomRequest, maxalloc);
	return err;
	}

SmfError SmfPlaylistServicePrivate::cancelRequest()
	{
	qDebug()<<"Inside SmfPlaylistServicePrivate::cancelRequest()";
	QByteArray notused;
	QByteArray retData = m_SmfClientPrivate->sendSyncRequest(notused,SmfCancelRequest,1000, notused);
	
	//De-serialize it into SmfError
	QDataStream reader(&retData,QIODevice::ReadOnly);
	int val;
	reader>>val;
	SmfError error = (SmfError) val;
	return error;
	}

void SmfPlaylistServicePrivate::resultsAvailable(QByteArray result, SmfRequestTypeID opcode, SmfError error)
	{
	//note:- "result" is serialized and we need to de-serialize it as per opcode
	//Order of serialization Error value followed by data
	
	QDataStream reader(&result,QIODevice::ReadOnly);
	
	//Now de-serialize it based on opcode
	switch(opcode)
		{
		case SmfMusicGetPlaylists:
		case SmfMusicGetPlaylistsOfUser:
			{
			SmfPlaylistList* m_playlistList = new SmfPlaylistList;
			reader>>*(m_playlistList);
			
			//ToDo:-
			SmfResultPage page;
			
			emit m_playlstSrvc->playlistsListAvailable(m_playlistList,error,page);
			break;
			}
			
		case SmfMusicAddToPlaylist:
		case SmfMusicPostCurrentPlayingPlaylist:
			{
			emit m_playlstSrvc->playlistUpdated(error);
			break;
			}
			
		case SmfMusicPlaylistCustomRequest:
   			{
   			int operationId;
   			QByteArray *data = new QByteArray;
   			reader>>operationId;
   			reader>>*data;
   			qDebug()<<"operationId = "<<operationId;
   			qDebug()<<"data size = "<<data->size();
   			emit m_playlstSrvc->customDataAvailable(operationId, data);
   			break;
   			}
		default:
			Q_ASSERT_X(1,"SmfPlaylistServicePrivate::resultsAvailable","unknown opcode");
		}
	}
//SmfPlaylistServicePrivate end



//SmfLyricsServicePrivate start
SmfLyricsServicePrivate::SmfLyricsServicePrivate(SmfLyricsService *aLyricsSrvc)
		: m_lyricsSrvc(aLyricsSrvc)
	{
#ifdef Q_OS_SYMBIAN
	//private impl for symbian
	m_SmfClientPrivate = CSmfClientSymbian::NewL(this);
#else
	//TODO:- Use private impl for other platforms or else Qt impl
#endif
	}

SmfLyricsServicePrivate::~SmfLyricsServicePrivate()
	{
	if(m_SmfClientPrivate)
		{
		delete m_SmfClientPrivate;
		m_SmfClientPrivate = NULL;
		}
	}

SmfError SmfLyricsServicePrivate::lyrics(SmfTrackInfo track, int pageNum, int perPage)
	{
	SmfError err = SmfNoError;
	//We need to pass Opcode and SmfProvider serialized into bytearray 
	SmfProvider* m_baseProvider = m_lyricsSrvc->getProvider();
	m_serializedDataToServer.clear();
	QDataStream write(&m_serializedDataToServer,QIODevice::WriteOnly);
	write<<*(m_baseProvider);

	QByteArray dataToPlugins;
	QDataStream streamToPlugin(&dataToPlugins, QIODevice::WriteOnly);
	
	m_argFlag = 1;
	streamToPlugin<<m_argFlag;
	streamToPlugin<<track;
	streamToPlugin<<m_argFlag;
	streamToPlugin<<pageNum;
	streamToPlugin<<m_argFlag;
	streamToPlugin<<perPage;
	
	write<<dataToPlugins;
		
	QString intfName(lyricsServiceInterface);
	//TODO:-revisit all the maxalloc
	int maxalloc = SmfMusicLyricsMaxSize*perPage;
	
	//call private impl's send method
	m_SmfClientPrivate->sendRequest(m_serializedDataToServer, intfName,
			SmfMusicGetLyrics, maxalloc);
	return err;
	}

SmfError SmfLyricsServicePrivate::subtitles(SmfTrackInfo track, SmfSubtitleSearchFilter filter, 
		int pageNum, int perPage)
	{
	SmfError err = SmfNoError;
	//We need to pass Opcode and SmfProvider serialized into bytearray 
	SmfProvider* m_baseProvider = m_lyricsSrvc->getProvider();
	QDataStream write(&m_serializedDataToServer,QIODevice::WriteOnly);
	write<<*(m_baseProvider);

	QByteArray dataToPlugins;
	QDataStream streamToPlugin(&dataToPlugins, QIODevice::WriteOnly);
	
	m_argFlag = 1;
	streamToPlugin<<m_argFlag;
	streamToPlugin<<track;
	streamToPlugin<<m_argFlag;
	streamToPlugin<<filter;
	streamToPlugin<<m_argFlag;
	streamToPlugin<<pageNum;
	streamToPlugin<<m_argFlag;
	streamToPlugin<<perPage;
	
	write<<dataToPlugins;
		
	QString intfName(lyricsServiceInterface);
	//TODO:-revisit all the maxalloc
	int maxalloc = SmfMusicLyricsMaxSize*perPage;
	
	//call private impl's send method
	m_SmfClientPrivate->sendRequest(m_serializedDataToServer, intfName,
			SmfMusicGetSubtitle, maxalloc);
	return err;
	}

SmfError SmfLyricsServicePrivate::customRequest ( const int& operationId, QByteArray* customData )
	{
	SmfError err = SmfNoError;
	//We need to pass Opcode and SmfProvider serialized into bytearray 
	SmfProvider* m_baseProvider = m_lyricsSrvc->getProvider();
	m_serializedDataToServer.clear();
	QDataStream write(&m_serializedDataToServer,QIODevice::WriteOnly);
	write<<*m_baseProvider;

	QByteArray dataToPlugins;
	QDataStream streamToPlugin(&dataToPlugins, QIODevice::WriteOnly);
	
	m_argFlag = 1;
	streamToPlugin<<m_argFlag;
	streamToPlugin<<operationId;
	if(customData)
		{
		streamToPlugin<<m_argFlag;
		streamToPlugin<<*customData;
		}
	else
		{
		m_argFlag = 0;
		streamToPlugin<<m_argFlag;
		}
	
	write<<dataToPlugins;
	
	QString intfName(lyricsServiceInterface);
	//TODO:-revisit all the maxalloc
	int maxalloc = 10000;
	
	//call private impl's send method
	m_SmfClientPrivate->sendRequest(m_serializedDataToServer, intfName,
			SmfMusicLyricsCustomRequest, maxalloc);
	return err;
	}

SmfError SmfLyricsServicePrivate::cancelRequest()
	{
	qDebug()<<"Inside SmfLyricsServicePrivate::cancelRequest()";
	QByteArray notused;
	QByteArray retData = m_SmfClientPrivate->sendSyncRequest(notused,SmfCancelRequest,1000, notused);
	
	//De-serialize it into SmfError
	QDataStream reader(&retData,QIODevice::ReadOnly);
	int val;
	reader>>val;
	SmfError error = (SmfError) val;
	return error;
	}

void SmfLyricsServicePrivate::resultsAvailable(QByteArray result, SmfRequestTypeID opcode, SmfError error)
	{
	//note:- "result" is serialized and we need to de-serialize it as per opcode

	QDataStream reader(&result,QIODevice::ReadOnly);
	
	//Now de-serialize it based on opcode
	switch(opcode)
		{
		case SmfMusicGetLyrics:
			{
			SmfLyricsList* m_lyricsList = new SmfLyricsList;
			reader>>*(m_lyricsList);
			
			// ToDo :-
			SmfResultPage page;
			
			emit m_lyricsSrvc->lyricsAvailable(m_lyricsList,error,page);
			break;
			}
			
		case SmfMusicGetSubtitle:
			{
			SmfSubtitleList* m_subList = new SmfSubtitleList;
			reader>>*(m_subList);
			
			// ToDo :-
			SmfResultPage page;
			
			emit m_lyricsSrvc->subtitleAvailable(m_subList,error,page);
			break;
			}
			
		case SmfMusicLyricsCustomRequest:
   			{
   			int operationId;
   			QByteArray *data = new QByteArray;
   			reader>>operationId;
   			reader>>*data;
   			qDebug()<<"operationId = "<<operationId;
   			qDebug()<<"data size = "<<data->size();
   			emit m_lyricsSrvc->customDataAvailable(operationId, data);
   			break;
   			}
   			
		default:
			Q_ASSERT_X(1,"SmfLyricsServicePrivate::resultsAvailable","unknown opcode");
		}
	}

//SmfLyricsServicePrivate end
