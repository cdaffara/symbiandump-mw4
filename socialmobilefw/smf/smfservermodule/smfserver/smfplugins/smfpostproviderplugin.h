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
 * Interface specification for getting/posting updates to a social site
 *
 */


#ifndef SMFPOSTPROVIDERPLUGIN_H_
#define SMFPOSTPROVIDERPLUGIN_H_

#include <smfpluginbase.h>
#include <smfcontact.h>
#include <smfpost.h>
#include <smflocation.h>

/**
 * @ingroup smf_plugin_group
 * Interface to get/update the posts to a service provider. This class
 * provides basic functionality to allow applications to obtain list of
 * posts, updates posts or change the presence information to a social 
 * networking service.
 *
 * All of the functionality described here should be implemented by a service
 * specific plug-in.
 */
class SmfPostProviderPlugin : public SmfPluginBase
	{
public:
	
	/**
	 * Destructor
	 */
	virtual ~SmfPostProviderPlugin( ) {}
	
	/**
	 * Method that returns maximum no of chars (unicode) that service 
	 * provider can post without truncation. Negative value means 
	 * no limit
	 * @return Max characters that can be posted without truncation
	 */
	virtual qint32 maxCharsInPost( ) const = 0;
	
	/**
	 * Method that returns maximum no of items that can be returned 
	 * in a single query to getPosts. Negative value means feature 
	 * not supported.
	 * @return Max items that can be returned in a single query
	 */
	virtual qint32 maxItems( ) const = 0;

	/**
	 * Method that returns all the formatting of posts that this 
	 * service provider supports. May return 0 items to mean 
	 * only QString is supported.
	 * @return Supported formats of posts
	 */
	virtual QVector<QTextFormat> supportedFormats ( ) const = 0;
	
	/**
	 * Method that returns whether this SP supports Appearence
	 * @return Returns true if Appearance is supported, else false. 
	 * @see SmfAppearenceInfo
	 */
	virtual bool supportsAppearence ( ) const = 0;
	
	/**
	 * Method to get the latest posts
	 * @param aRequest [out] The request data to be sent to network
	 * @param aUser [in] The user's contact in this SP, omit for self contact
	 * @param aPageNum [in] The page to be extracted
	 * @param aItemsPerPage [in] Number of items per page
	 * @return Appropriate value of the enum SmfPluginError.
	 * Plugin error if any, else SmfPluginErrNone for success
	 */
	virtual SmfPluginError retrieve( SmfPluginRequestData &aRequest,
			const SmfContact *aUser = NULL,
			const int aPageNum = SMF_FIRST_PAGE, 
			const int aItemsPerPage = SMF_ITEMS_PER_PAGE ) = 0;

	
	/**
	 * Method to update a post to own area.
	 * @param aRequest [out] The request data to be sent to network
	 * @param aPostData [in] The post data to be posted
	 * @param aLocation [in] The location
	 * @return Appropriate value of the enum SmfPluginError.
	 * Plugin error if any, else SmfPluginErrNone for success
	 */
	virtual SmfPluginError post( SmfPluginRequestData &aRequest,
			const SmfPost &aPostData, 
			const SmfLocation &aLocation ) = 0;
	
	/**
	 * Method to update the last post to own area with new data
	 * @param aRequest [out] The request data to be sent to network
	 * @param aPostData [in] The edited/new data to be posted
	 * @return Appropriate value of the enum SmfPluginError.
	 * Plugin error if any, else SmfPluginErrNone for success
	 */
	virtual SmfPluginError updatePost( SmfPluginRequestData &aRequest,
			const SmfPost &aPostData ) = 0;
	
	/**
	 * Method to update a post to a particular contact
	 * @param aRequest [out] The request data to be sent to network
	 * @param aPostData [in] The post data to be posted
	 * @param aContact [in] The contact where the data has to be posted
	 * @param aLocation [in] The location
	 * @return Appropriate value of the enum SmfPluginError.
	 * Plugin error if any, else SmfPluginErrNone for success
	 */
	virtual SmfPluginError postDirected( SmfPluginRequestData &aRequest,
			const SmfPost &aPostData, 
			const SmfContact &aContact, 
			const SmfLocation *aLocation = NULL ) = 0;
	
	
	/**
	 * Method to post a comment on a post.
	 * @param aRequest [out] The request data to be sent to network
	 * @param aTarget [in] Post on which comment has to be posted
	 * @param aComment [in] comment to be posted
	 * @param aLocation [in] location data
	 * @return Appropriate value of the enum SmfPluginError.
	 * Plugin error if any, else SmfPluginErrNone for success
	 */
	virtual SmfPluginError commentOnAPost(SmfPluginRequestData &aRequest,
			const SmfPost &aTarget,
			const SmfPost &aComment, 
			const SmfLocation *aLocation = NULL) = 0;
	
	/**
	 * Method to update the presence information of the user
	 * @param aRequest [out] The request data to be sent to network
	 * @param aAppearence [in] The appearence information
	 * @param aStatus [in] The status string
	 * @return Appropriate value of the enum SmfPluginError.
	 * Plugin error if any, else SmfPluginErrNone for success
	 */
	virtual SmfPluginError postAppearence( SmfPluginRequestData &aRequest,  
			const SmfPresenceInfo &aAppearence,
			const QString &aStatus ) = 0;
	
	/**
	 * Share a contact's post to user's friends and followers
	 * (e.g. retweet in twitter, share on facebook)
	 * @param aRequest [out] The request data to be sent to network
	 * @param aPostData [in] data to be posted
	 * @param aContact [in] contact to which the post belonged
	 * @param aEdited [in] whether user changed items within the post
	 * @return Appropriate value of the enum SmfPluginError.
	 * Plugin error if any, else SmfPluginErrNone for success
	 */
	virtual SmfPluginError sharePost( SmfPluginRequestData &aRequest, 
			const SmfPost &aPostData, 
			const SmfContact &aContact, 
			const bool &aEdited ) = 0;
	
	/**
	 * Customised method for SmfPostProviderPlugin interface
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

Q_DECLARE_INTERFACE( SmfPostProviderPlugin, "org.symbian.smf.plugin.contact.posts/v0.2" );

#endif /* SMFPOSTPROVIDERPLUGIN_H_ */
