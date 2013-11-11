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
 * Interface specification for music search services
 *
 */


#ifndef SMFMUSICSEARCHPLUGIN_H_
#define SMFMUSICSEARCHPLUGIN_H_

#include <smfpluginbase.h>
#include <QList>
#include <smfmusicfingerprint.h>
#include <smftrackinfo.h>
#include <smfalbum.h>
#include <smfartists.h>

/**
 * @ingroup smf_plugin_group
 * Interface specification for music search services. This class
 * provides basic functionality to allow applications to search for 
 * tracks, get recommented tracks etc
 *
 * All of the functionality described here should be implemented by a service
 * specific plug-in.
 */
class SmfMusicSearchPlugin : public SmfPluginBase
	{
public:
	
	/**
	 * Destructor
	 */
	virtual ~SmfMusicSearchPlugin( ) {}
	
	/**
	 * Method to get recommended tracks
	 * @param aRequest [out] The request data to be sent to network
	 * @param aTrack [in] The track for which similar recommendations 
	 * need to be fetched.
	 * @param aPageNum [in] The page to be extracted
	 * @param aItemsPerPage [in] Number of items per page
	 * @return Appropriate value of the enum SmfPluginError.
	 * Plugin error if any, else SmfPluginErrNone for success
	 */
	virtual SmfPluginError recommendations( SmfPluginRequestData &aRequest,
			const SmfTrackInfo &aTrack,
			const int aPageNum = SMF_FIRST_PAGE, 
			const int aItemsPerPage = SMF_ITEMS_PER_PAGE ) = 0;
	
	/**
	 * Method to search for tracks similar to a given track
	 * @param aRequest [out] The request data to be sent to network
	 * @param aTrack [in] The track for which similar tracks 
	 * need to be fetched.
	 * @param aPageNum [in] The page to be extracted
	 * @param aItemsPerPage [in] Number of items per page
	 * @return Appropriate value of the enum SmfPluginError.
	 * Plugin error if any, else SmfPluginErrNone for success
	 */
	virtual SmfPluginError tracksSimilar( SmfPluginRequestData &aRequest,
			const SmfTrackInfo &aTrack,
			const int aPageNum = SMF_FIRST_PAGE, 
			const int aItemsPerPage = SMF_ITEMS_PER_PAGE ) = 0;
	
	/**
	 * Method to search for tracks of a given album
	 * @param aRequest [out] The request data to be sent to network
	 * @param aAlbum [in] The album whose tracks need to be fetched.
	 * @param aPageNum [in] The page to be extracted
	 * @param aItemsPerPage [in] Number of items per page
	 * @return Appropriate value of the enum SmfPluginError.
	 * Plugin error if any, else SmfPluginErrNone for success
	 */
	virtual SmfPluginError tracksOfAlbum( SmfPluginRequestData &aRequest,
			const SmfAlbum &aAlbum,
			const int aPageNum = SMF_FIRST_PAGE, 
			const int aItemsPerPage = SMF_ITEMS_PER_PAGE ) = 0;
	
	/**
	 * Method to search for tracks of the given artist(s)
	 * @param aRequest [out] The request data to be sent to network
	 * @param aArtist [in] The artist(s) whose tracks need to be fetched.
	 * @param aPageNum [in] The page to be extracted
	 * @param aItemsPerPage [in] Number of items per page
	 * @return Appropriate value of the enum SmfPluginError.
	 * Plugin error if any, else SmfPluginErrNone for success
	 */
	virtual SmfPluginError tracksOfArtist( SmfPluginRequestData &aRequest,
			const SmfArtists &aArtist,
			const int aPageNum = SMF_FIRST_PAGE, 
			const int aItemsPerPage = SMF_ITEMS_PER_PAGE ) = 0;
	
	/**
	 * Method to get tracks having a similar finger print
	 * @param aRequest [out] The request data to be sent to network
	 * @param aSignature [in] The finger print to be searched for need  
	 * to be fetched.
	 * @param aPageNum [in] The page to be extracted
	 * @param aItemsPerPage [in] Number of items per page
	 * @return Appropriate value of the enum SmfPluginError.
	 * Plugin error if any, else SmfPluginErrNone for success
	 */
	virtual SmfPluginError trackInfo( SmfPluginRequestData &aRequest,
			const SmfMusicFingerPrint &aSignature,
			const int aPageNum = SMF_FIRST_PAGE, 
			const int aItemsPerPage = SMF_ITEMS_PER_PAGE ) = 0;
	
	/**
	 * Method to search information about where to buy this song from
	 * @param aRequest [out] The request data to be sent to network
	 * @param aTrack [in] The track for which stores need to be searched
	 * @param aPageNum [in] The page to be extracted
	 * @param aItemsPerPage [in] Number of items per page
	 * @return Appropriate value of the enum SmfPluginError.
	 * Plugin error if any, else SmfPluginErrNone for success
	 */
	virtual SmfPluginError stores( SmfPluginRequestData &aRequest,
			const SmfTrackInfo &aTrack,
			const int aPageNum = SMF_FIRST_PAGE, 
			const int aItemsPerPage = SMF_ITEMS_PER_PAGE ) = 0;
	
	/**
	 * Customised method for SmfMusicSearchPlugin interface
	 * @param aRequest [out] The request data to be sent to network
	 * @param aOperation [in] The operation type (should be known between 
	 * the client interface and the plugin)
	 * @param aData [in] The data required to form the request (The type 
	 * of data should be known between client and the plugin)
	 * @return Appropriate value of the enum SmfPluginError.
	 * Plugin error if any, else SmfPluginErrNone for success
	 */
	virtual SmfPluginError customRequest( SmfPluginRequestData &aRequest, 
			const int &aOperation, QByteArray *aData ) = 0;
	
	};

Q_DECLARE_INTERFACE( SmfMusicSearchPlugin, "org.symbian.smf.plugin.music.search/v0.2" );

#endif /* SMFMUSICSEARCHPLUGIN_H_ */
