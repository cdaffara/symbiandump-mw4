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
 * Interface specification for music services
 *
 */


#ifndef SMFMUSICSERVICEPLUGIN_H_
#define SMFMUSICSERVICEPLUGIN_H_

#include <smfpluginbase.h>
#include <smfmusicprofile.h>
#include <smflocation.h>
#include <smftrackinfo.h>
#include <smfmusicrating.h>
#include <smfcomment.h>

/**
 * @ingroup smf_plugin_group
 * Interface specification for music services. This class provides basic 
 * functionality to allow application to search for a user or check for
 * user information etc.
 *
 * All of the functionality described here should be implemented by a service
 * specific plug-in.
 */
class SmfMusicServicePlugin : public SmfPluginBase
	{
public:
	
	/**
	 * Destructor
	 */
	virtual ~SmfMusicServicePlugin( ) {}
	
	/**
	 * Method to get self profile information
	 * @param aRequest [out] The request data to be sent to network
	 * @return Appropriate value of the enum SmfPluginError.
	 * Plugin error if any, else SmfPluginErrNone for success
	 */
	virtual SmfPluginError userMusicInfo( SmfPluginRequestData &aRequest ) = 0;
	
	/**
	 * Method to search information about artists. All information 
	 * in SmfArtists is not required, however more available the better
	 * @param aRequest [out] The request data to be sent to network
	 * @param aArtist The artist which is the search criteria
	 * @param aPageNum The page to be extracted
	 * @param aItemsPerPage Number of items per page
	 * @return Appropriate value of the enum SmfPluginError.
	 * Plugin error if any, else SmfPluginErrNone for success
	 */
	virtual SmfPluginError searchArtist( SmfPluginRequestData &aRequest,
			const SmfArtists &aArtist,
			const int aPageNum = SMF_FIRST_PAGE, 
			const int aItemsPerPage = SMF_ITEMS_PER_PAGE ) = 0;
	
	/**
	 * Method to search information about album. All information 
	 * in SmfAlbum is not required, however more available the better
	 * @param aRequest [out] The request data to be sent to network
	 * @param aAlbum [in] The album which is the search criteria
	 * @param aPageNum [in] The page to be extracted
	 * @param aItemsPerPage [in] Number of items per page
	 * @return Appropriate value of the enum SmfPluginError.
	 * Plugin error if any, else SmfPluginErrNone for success
	 */
	virtual SmfPluginError searchAlbum( SmfPluginRequestData &aRequest,
			const SmfAlbum &aAlbum,
			const int aPageNum = SMF_FIRST_PAGE, 
			const int aItemsPerPage = SMF_ITEMS_PER_PAGE ) = 0;
	
	/**
	 * Method to search information about events. All information 
	 * in SmfEvent is not required, however more available the better
	 * @param aRequest [out] The request data to be sent to network
	 * @param aEvent [in] The event which is the search criteria
	 * @param aPageNum [in] The page to be extracted
	 * @param aItemsPerPage [in] Number of items per page
	 * @return Appropriate value of the enum SmfPluginError.
	 * Plugin error if any, else SmfPluginErrNone for success
	 */
	virtual SmfPluginError searchEvents( SmfPluginRequestData &aRequest,
			const SmfEvent &aEvent,
			const int aPageNum = SMF_FIRST_PAGE, 
			const int aItemsPerPage = SMF_ITEMS_PER_PAGE ) = 0;
	
	/**
	 * Method to search information about venue. All information 
	 * in SmfLocation is not required, however more available the better
	 * @param aRequest [out] The request data to be sent to network
	 * @param aVenue [in] The venue which is the search criteria
	 * @param aPageNum [in] The page to be extracted
	 * @param aItemsPerPage [in] Number of items per page
	 * @return Appropriate value of the enum SmfPluginError.
	 * Plugin error if any, else SmfPluginErrNone for success
	 */
	virtual SmfPluginError searchVenue( SmfPluginRequestData &aRequest,
			const SmfLocation &aVenue,
			const int aPageNum = SMF_FIRST_PAGE, 
			const int aItemsPerPage = SMF_ITEMS_PER_PAGE ) = 0;
	
	/**
	 * Method to search information about other service users for a 
	 * particular place
	 * @param aRequest [out] The request data to be sent to network
	 * @param aPlace [in] The place which is the search criteria
	 * @param aPageNum [in] The page to be extracted
	 * @param aItemsPerPage [in] Number of items per page
	 * @return Appropriate value of the enum SmfPluginError.
	 * Plugin error if any, else SmfPluginErrNone for success
	 */
	virtual SmfPluginError searchUser( SmfPluginRequestData &aRequest,
			const SmfLocation &aPlace,
			const int aPageNum = SMF_FIRST_PAGE, 
			const int aItemsPerPage = SMF_ITEMS_PER_PAGE ) = 0;
	
	/**
	 * Method to post the currently playing track
	 * @param aRequest [out] The request data to be sent to network
	 * @param aTrack [in] The current playing track, that should be posted
	 * @return Appropriate value of the enum SmfPluginError.
	 * Plugin error if any, else SmfPluginErrNone for success
	 */
	virtual SmfPluginError postCurrentPlaying( 
			SmfPluginRequestData &aRequest,
			const SmfTrackInfo &aTrack ) = 0;
	
	/**
	 * Method to post the rating on a track
	 * @param aRequest [out] The request data to be sent to network
	 * @param aTrack [in] The track on which rating should be posted
	 * @param aRating [in] The rating values
	 * @return Appropriate value of the enum SmfPluginError.
	 * Plugin error if any, else SmfPluginErrNone for success
	 */
	virtual SmfPluginError postRating( 
			SmfPluginRequestData &aRequest, 
			const SmfTrackInfo &aTrack, 
			const SmfMusicRating &aRating ) = 0;
	
	/**
	 * Method to post comment on a track
	 * @param aRequest [out] The request data to be sent to network
	 * @param aTrack [in] The track on which comment should be posted
	 * @param aComment [in] The comment content
	 * @return Appropriate value of the enum SmfPluginError.
	 * Plugin error if any, else SmfPluginErrNone for success
	 */
	virtual SmfPluginError postComments( 
			SmfPluginRequestData &aRequest, 
			const SmfTrackInfo &aTrack, 
			const SmfComment &aComment ) = 0;
	
	/**
	 * Customised method for SmfMusicServicePlugin interface
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

Q_DECLARE_INTERFACE( SmfMusicServicePlugin, "org.symbian.smf.plugin.music.service/v0.2" );

#endif /* SMFMUSICSERVICEPLUGIN_H_ */
