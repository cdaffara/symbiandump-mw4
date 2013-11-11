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
 * Music related services
 */

#include <smfmusicrating.h>
#include <smfcomment.h>
#include <smfmusicfingerprint.h>
#include <smfcontact.h>

#include "smfmusic.h"
#include "smfmusic_p.h"


//SmfMusicService start
SmfMusicService::SmfMusicService(SmfProvider *baseProvider)
		: m_baseProvider(baseProvider)
	{
	m_private = new SmfMusicServicePrivate(this);
	}

SmfMusicService::~SmfMusicService()
	{
	if(m_private)
		{
		delete m_private;
		m_private = NULL;
		}
	}

SmfError SmfMusicService::userMusicInfo()
	{
	return m_private->userMusicInfo();
	}

SmfError SmfMusicService::searchArtist ( SmfArtists artist, int pageNum, int perPage)
	{
	return m_private->searchArtist(artist,pageNum,perPage);
	}

SmfError SmfMusicService::searchAlbum ( SmfAlbum album, int pageNum, int perPage)
	{
	return m_private->searchAlbum(album,pageNum,perPage);
	}

SmfError SmfMusicService::searchEvents ( SmfEvent event, int pageNum, int perPage)
	{
	return m_private->searchEvents(event,pageNum,perPage);
	}

SmfError SmfMusicService::searchVenue ( SmfLocation location, int pageNum, int perPage)
	{
	return m_private->searchVenue(location,pageNum,perPage);
	}

SmfError SmfMusicService::searchUser(SmfLocation venue, int pageNum, int perPage)
	{
	return m_private->searchUser(venue,pageNum,perPage);
	}

SmfError SmfMusicService::customRequest(const int& operationId,QByteArray* customData)
	{
	return m_private->customRequest(operationId, customData);
	}

SmfError SmfMusicService::postCurrentPlaying(SmfTrackInfo track)
	{
	return m_private->postCurrentPlaying(track);
	}

SmfError SmfMusicService::postRating(SmfTrackInfo track, SmfMusicRating rate)
	{
	return m_private->postRating(track,rate);
	}

SmfError SmfMusicService::postComments(SmfTrackInfo track, SmfComment comment)
	{
	return m_private->postComments(track,comment);
	}

SmfError SmfMusicService::cancelRequest()
	{
	return m_private->cancelRequest();
	}

SmfProvider* SmfMusicService::getProvider() const
	{
	return m_baseProvider;
	}
//SmfMusicService end


//SmfMusicSearch start
SmfMusicSearch::SmfMusicSearch(SmfProvider *baseProvider)
		:m_baseProvider(baseProvider)
	{
	m_private = new SmfMusicSearchPrivate(this);
	}

SmfMusicSearch::~SmfMusicSearch()
	{
	if(m_private)
		{
		delete m_private;
		m_private = NULL;
		}
	}

SmfError SmfMusicSearch::recommendations(SmfTrackInfo track, int pageNum, int perPage)
	{
	return m_private->recommendations(track,pageNum,perPage);
	}

SmfError SmfMusicSearch::tracksSimilar ( SmfTrackInfo track, int pageNum, int perPage)
	{
	return m_private->tracksSimilar(track,pageNum,perPage);
	}

SmfError SmfMusicSearch::tracksOfAlbum ( SmfAlbum album, int pageNum, int perPage )
	{
	return m_private->tracksOfAlbum(album,pageNum,perPage);
	}

SmfError SmfMusicSearch::tracksOfArtist ( SmfArtists artists, int pageNum, int perPage )
	{
	return m_private->tracksOfArtist(artists,pageNum,perPage);
	}

SmfError SmfMusicSearch::trackInfo(SmfMusicFingerPrint signature, int pageNum, int perPage)
	{
	return m_private->trackInfo(signature,pageNum,perPage);
	}

SmfError SmfMusicSearch::stores(SmfTrackInfo track, int pageNum, int perPage)
	{
	return m_private->stores(track,pageNum,perPage);
	}

SmfError SmfMusicSearch::customRequest(const int& operationId,QByteArray* customData)
	{
	return m_private->customRequest(operationId, customData);
	}

SmfError SmfMusicSearch::cancelRequest()
	{
	return m_private->cancelRequest();
	}

SmfProvider* SmfMusicSearch::getProvider() const
	{
	return m_baseProvider;
	}
//SmfMusicSearch end


//SmfPlaylistService start
SmfPlaylistService::SmfPlaylistService(SmfProvider *baseProvider)
		: m_baseProvider(baseProvider)
	{
	m_private = new SmfPlaylistServicePrivate(this);
	}

SmfPlaylistService::~SmfPlaylistService()
	{
	if(m_private)
		{
		delete m_private;
		m_private = NULL;
		}
	}

SmfError SmfPlaylistService::playlists(int pageNum, int perPage)
	{
	return m_private->playlists(pageNum,perPage);
	}

SmfError SmfPlaylistService::playlistsOf(SmfContact *user, int pageNum, int perPage)
	{
	return m_private->playlistsOf(user,pageNum,perPage);
	}

SmfError SmfPlaylistService::addToPlaylist(SmfPlaylist plst, SmfTrackInfoList *tracks)
	{
	return m_private->addToPlaylist(plst,tracks);
	}

SmfError SmfPlaylistService::postCurrentPlayingPlaylist(SmfPlaylist plst)
	{
	return m_private->postCurrentPlayingPlaylist(plst);
	}

SmfError SmfPlaylistService::customRequest(const int& operationId,QByteArray* customData)
	{
	return m_private->customRequest(operationId, customData);
	}

SmfError SmfPlaylistService::cancelRequest()
	{
	return m_private->cancelRequest();
	}

SmfProvider* SmfPlaylistService::getProvider() const
	{
	return m_baseProvider;
	}
//SmfPlaylistService end


//SmfLyricsService start
SmfLyricsService::SmfLyricsService(SmfProvider *baseProvider)
		:m_baseProvider(baseProvider)
	{
	m_private = new SmfLyricsServicePrivate(this);
	}

SmfLyricsService::~SmfLyricsService()
	{
	if(m_private)
		{
		delete m_private;
		m_private = NULL;
		}
	}

SmfError SmfLyricsService::lyrics(SmfTrackInfo track, int pageNum, int perPage)
	{
	return m_private->lyrics(track,pageNum,perPage);
	}

SmfError SmfLyricsService::subtitles(SmfTrackInfo track, SmfSubtitleSearchFilter filter, int pageNum, int perPage)
	{
	return m_private->subtitles(track,filter,pageNum,perPage);
	}

SmfError SmfLyricsService::customRequest(const int& operationId,QByteArray* customData)
	{
	return m_private->customRequest(operationId, customData);
	}

SmfError SmfLyricsService::cancelRequest()
	{
	return m_private->cancelRequest();
	}

SmfProvider* SmfLyricsService::getProvider() const
	{
	return m_baseProvider;
	}
//SmfLyricsService end
