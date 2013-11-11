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
 * The SmfPluginBase class is the base class for all plugins.
 *
 */

#ifndef SMFPLUGINBASE_H_
#define SMFPLUGINBASE_H_

#include <smfproviderbase.h>
#include <smfglobal.h>

class SmfPluginUtil;

/**
 * @ingroup smf_plugin_group
 * The SmfPluginBase class is the base class for all plugins 
 */
class SmfPluginBase
	{
public:
	/**
	 * Destructor
	 */
	virtual ~SmfPluginBase( ) {}
	
	/**
	 * The first method to be called in the plugin that implements this interface.
	 * If this method is not called, plugin may not behave as expected.
	 */
	virtual void initialize( ) = 0;

	/**
	 * Method to get the provider information
	 * @return Instance of SmfProviderBase
	 */
	virtual SmfProviderBase* getProviderInfo( ) = 0;
	
	/**
	 * Method to get the result for a network request.
	 * @param aOperation [in] The type of operation to be requested
	 * @param aTransportResult [in] The result of transport operation
	 * @param aReply [in] The QByteArray instance containing the network response.
	 * The plugins should delete this instance once they have read the 
	 * data from it.
	 * @param aResult [out] An output parameter to the plugin manager.If the 
	 * return value is SmfSendRequestAgain, QVariant will be of type 
	 * SmfPluginRequestData.
	 * 
	 * For SmfActivityFetcherPlugin: If the last operation was selfActivities(), 
	 * friendsActivities() or filtered(), aResult will be of type QList<SmfActivityEntry>
	 * 
	 * For SmfContactFetcherPlugin: If last operation was friends() or followers() 
	 * or search() or searchInGroup() or searchNear(), aResult will be of type 
	 * QList<SmfContact>. If last operation was groups(), aResult will be of 
	 * type QList<SmfGroup>
	 * 
	 * For SmfEventsFetcherPlugin: If the last operation was events(), aResult will 
	 * be of type QList<SmfEvent>. If the last operation was venues(), aResult 
	 * will be of type QList<SmfLocation>. If the last operation was attendees(), 
	 * aResult will be of type QList<SmfContact>. If the last operation was 
	 * postEvents(), aResult will be of type bool.
	 * 
	 * For SmfGalleryPlugin: If last operation was albums, aResult will be of 
	 * type QList<SmfPictureAlbum>. If the last operation was pictures(), aResult 
	 * will be of type QList<SmfPicture>. If last operation was description(), 
	 * aResult will be of type QString. If last operation was upload() or 
	 * postComment(), aResult will be of type bool.
	 * 
	 * For SmfLyricsServicePlugin: If last operation was lyrics(), aResult will 
	 * be of type QList<SmfLyrics>. If last operation was subtitles(), aResult will 
	 * be of type QList<SmfSubtitle>.
	 * 
	 * For SmfMusicSearchPlugin: If last operation was recommendations() or 
	 * tracksSimilar() or tracksOfAlbum() or tracksOfArtist() or trackInfo(), 
	 * aResult will be of type QList<SmfTrackInfo>. If last operation was 
	 * stores(), aResult will be of type QList<SmfProvider>.
	 * 
	 * For SmfMusicServicePlugin: If last operation was userMusicInfo(), aResult 
	 * will be of type SmfMusicProfile. If last operation was searchArtist(), 
	 * aResult will be of type QList<SmfArtists>. If last operation was searchAlbum(), 
	 * aResult will be of type QList<SmfAlbum>. If last operation was searchEvents(), 
	 * aResult will be of type QList<SmfEvent>. If last operation was searchVenue(), 
	 * aResult will be of type QList<Smfocation>. If last operation was searchUser(), 
	 * aResult will be of type QList<SmfMusicProfile>. If last operation was 
	 * postCurrentPlaying() or postRating() or postComments(), aResult will be of 
	 * type bool.
	 * 
	 * For SmfPlaylistServicePlugin: If last operation was playlists() or 
	 * playlistsOf(), aResult will be of type QList<SmfPlaylist>. If last 
	 * operation was addToPlaylist() or postCurrentPlayingPlaylist(), 
	 * aResult will be of type bool.
	 * 
	 * For SmfPostProviderPlugin: If last operation was retrieve(), aResult will be 
	 * of type QList<SmfPost>. If last operation was post() or updatePost() or 
	 * postDirected() or commentOnAPost() or postAppearence() or sharePost(), 
	 * aResult will be of type bool
	 * 
	 * @param aRetType [out] Appropriate value of the enum SmfPluginRetType
	 * @param aPageResult [out] The SmfResultPage structure variable
	 * @return Appropriate value of the enum SmfPluginError.
	 * Plugin error if any, else SmfPluginErrNone for success
	 */
	virtual SmfPluginError responseAvailable( 
			const SmfRequestTypeID aOperation,
			const SmfTransportResult &aTransportResult, 
			QByteArray *aReply, 
			QVariant* aResult, 
			SmfPluginRetType &aRetType,
			SmfResultPage &aPageResult ) = 0;
	
	};

Q_DECLARE_INTERFACE( SmfPluginBase, "org.symbian.smf.plugin.smfpluginbase/v0.2" );

#endif /* SMFPLUGINBASE_H_ */
