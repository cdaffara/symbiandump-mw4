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
 * Nalina Hariharan,Satish Kanteti, Rajat Roy
 *
 * Description:
 * Intro Page
 *
 * @ref smf_high_level_design
 */

/**
 * @mainpage SMF (Social Mobile Framework Interfaces)
 * @defgroup smf_high_level_design SMF Interfaces and Usage
 *
 * Social Mobile Framework is a application level framework which allows Qt Applications
 * use the common web APIs to perform social web activities.
 *
 * Product name is "SMF"
 *
 * For details please refer to http://developer.symbian.org/wiki/index.php/Social_Mobile_Framework
 *
 * @section smf_interfaces SMF Interfaces
 *
 * Native applications can use SMF, via Client APIs.
 * Service Providers can provide smf plugins, via Plugin APIs.
 * Both would use Common APIs for common data classes
 * Service Providers can optionaliy use Credential Manager APIs
 *
 * Here is the list of public interfaces ( exported to /epoc32/include ) -
 * 		@subsection smf_client_interfaces Client Interfaces
 * 				- Client Applications (e.g. Media Player, Contacts, Game) uses these APIs to access SMF. see @ref smf_client_group
 *
 * 		@subsection smf_plugin_interfaces Plugin Interfaces
 * 				- Service Providers (e.g. Facebook, Flickr, last.fm) implements these APIs to provide service to SMF - see @ref smf_plugin_group
 *
 * 		@subsection smf_common_interfaces Common Interfaces
 * 				- Both plugins and smfclient uses these for representing data - see @ref smf_common_group
 *
 * 		@subsection smf_credmgr_interfaces CredMgr Interfaces
 * 				- Credential Manager provides APIs for digital signing, storing and retrieving keys, etc - see @ref smf_credmgr_group
 *			\ep
 *
 *
 *
 *
 *	@section smf_client_usage Client Usage Example
 *		- / example / DemoGUI / HomeView.cpp shows how to use the SMF client APIs in applications, e.g. Client can define suitable Models using lists returned by these APIs.
 *				- how to fetch a list of contacts from a remote server e.g. Facebook, see @ref HomeView::getFriends()
 *				- how to get a list of posts from a remote server e.g. Facebook, see @ref HomeView::getPosts()
 *				- how to get a list of images from a remote gallery e.g. Flickr.com, see @ref HomeView::getAlbums()
 *				- how to list all the activities that has happened recently, see @ref HomeView::getActivities()
 *
 *		@section smf_sample_plugin Sample Plugin From Service provider
 *		- This shows how to write a plugin for extending SMF service.
 *				- how to create request to be sent to a remote server for fetching contacts e.g.Facebook.com, see @ref FBContactFetcherPlugin::friends
 *				- how to implement the fetching of posts or comment,   @ref FBPostProviderPlugin::getPosts()
 *				- how to parse the response that arrived from Facebook.com for fetching activities, see @ref FBActivityFetcherPlugin::responseAvailable()
 *				- how to use JSON parsing from SmfPluginUtil, see @ref FBPostProviderPlugin::responseAvailable()
 * 	- Authentication Application also needs to be provided for these plugins.
 *
 *		@section smf_custom_api Custom API in SMF
 *		- SMF strives to provide service API for generic "common" social operations. However this doesn't mean that developers have to
 *		use other propriatery APIs to access service specific "differentiating" features. Service Providers can provide plugins which support
 *		APIs like @ref SmfGalleryPlugin::customRequest(). A separate library would also be provided by service providers to help SMF clients
 * 	use this custom	API
 *
 *
 *
 *		@section smf_dependency Dependency
 *		- SMF uses Qt 4.6.2 on Symbian^3  ( Nokia_Symbian3_SDK_v0.8 to be specific )
 *		- SMF uses QtMobility 1.0 for mainly contacts information, see QtMobility project.
 *		- SMF uses QJson as utility library, courtesy Flavio Castelli , see http://gitorious.org/qjson/qjson
 *
 *		@section smf_notes Notes:
 *		- SMF defines the social activities with inspiration from http://activitystrea.ms/
 *		- Other such open standards would soon find their usage in SMF, one such candidate is http://xspf.org/
 *
   */
/**
 * @defgroup smf_common_group Common Data Classes
 */
/**
 * @defgroup smf_client_group Client Classes
 */
/**
 * @defgroup smf_plugin_group Plugin Classes
 */
/**
 * @defgroup smf_credmgr_group CredMgr Classes
 */
