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

#ifndef SMFPLUGINMANAGERUTIL_H_
#define SMFPLUGINMANAGERUTIL_H_

#include "smfpluginbase.h"
#include "smfserverglobal.h"

// Forward declaration
class SmfPluginManager;

/**
 * The Plugin Manager util class performs all plugin functional 
 * specific functionalities
 */
class SmfPluginManagerUtil : public QObject
	{
	Q_OBJECT
	
public:
	/**
	 * Method to get the instance of SmfPluginManagerUtil class
	 * @param aParent The parent object
	 * @return The instance of SmfPluginManagerUtil class
	 */
	static SmfPluginManagerUtil* getInstance ( QObject *aParent = 0 );
	
	/**
	 * Destructor
	 */
	~SmfPluginManagerUtil ( );
	
private:
	/**
	 * Constructor with default argument
	 * @param aParent The parent object
	 */
	SmfPluginManagerUtil ( QObject *aParent = 0 );
	
private:
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
	void createRequest ( QObject* aInstance, 
			const SmfRequestTypeID &aOperation, 
			QByteArray &aInputData,
			SmfPluginRequestData &aReqData,
			SmfError &aResult,
			QByteArray &aOutputData );
	
	/**
	 * Method to create a web query to fetch activities
	 * @param aPlugin The instance of the loaded plugin that performs the 
	 * contact fetch operation.
	 * @param aOperation The type of operation to be performed
	 * @param aInputData The data required to create the web query
	 * @param aReqData [out] The request data created by the plugin
	 * @return SmfPluginError
	 */
	SmfPluginError createActivityFetcherRequest ( QObject *aPlugin, 
			const SmfRequestTypeID &aOperation, 
			QByteArray &aInputData,
			SmfPluginRequestData &aReqData );

	/**
	 * Method to create a web query to fetch contact details.
	 * @param aPlugin The instance of the loaded plugin that performs the 
	 * contact fetch operation.
	 * @param aOperation The type of operation to be performed
	 * @param aInputData The data required to create the web query
	 * @param aReqData [out] The request data created by the plugin
	 * @return SmfPluginError
	 */
	SmfPluginError createContactFetcherRequest ( QObject *aPlugin, 
			const SmfRequestTypeID &aOperation, 
			QByteArray &aInputData,
			SmfPluginRequestData &aReqData );
	
	/**
	 * Method to create a web query to do post operation on contacts 
	 * (like scraps or messages)
	 * @param aPlugin The instance of the loaded plugin that performs the 
	 * contact fetch operation.
	 * @param aOperation The type of operation to be performed
	 * @param aInputData The data required to create the web query
	 * @param aReqData [out] The request data created by the plugin
	 * @return SmfPluginError
	 */
	SmfPluginError createContactPostRequest ( QObject *aPlugin, 
			const SmfRequestTypeID &aOperation, 
			QByteArray &aInputData,
			SmfPluginRequestData &aReqData );

	/**
	 * Method to create a web query to do music lyrics or subtitle search
	 * @param aPlugin The instance of the loaded plugin that performs the 
	 * contact fetch operation.
	 * @param aOperation The type of operation to be performed
	 * @param aInputData The data required to create the web query
	 * @param aReqData [out] The request data created by the plugin
	 * @return SmfPluginError
	 */
	SmfPluginError createMusicLyricsRequest ( QObject *aPlugin, 
			const SmfRequestTypeID &aOperation, 
			QByteArray &aInputData,
			SmfPluginRequestData &aReqData );


	/**
	 * Method to create a web query to do event search or event post operation 
	 * @param aPlugin The instance of the loaded plugin that performs the 
	 * contact fetch operation.
	 * @param aOperation The type of operation to be performed
	 * @param aInputData The data required to create the web query
	 * @param aReqData [out] The request data created by the plugin
	 * @return SmfPluginError
	 */
	SmfPluginError createEventsFetcherRequest ( QObject *aPlugin, 
			const SmfRequestTypeID &aOperation, 
			QByteArray &aInputData,
			SmfPluginRequestData &aReqData );

	/**
	 * Method to create a web query to do music search operation 
	 * @param aPlugin The instance of the loaded plugin that performs the 
	 * contact fetch operation.
	 * @param aOperation The type of operation to be performed
	 * @param aInputData The data required to create the web query
	 * @param aReqData [out] The request data created by the plugin
	 * @return SmfPluginError
	 */
	SmfPluginError createMusicSearchRequest ( QObject *aPlugin, 
			const SmfRequestTypeID &aOperation, 
			QByteArray &aInputData,
			SmfPluginRequestData &aReqData );


	/**
	 * Method to create a web query to do music service request operation 
	 * like getting self profile or search profiles of other users
	 * @param aPlugin The instance of the loaded plugin that performs the 
	 * contact fetch operation.
	 * @param aOperation The type of operation to be performed
	 * @param aInputData The data required to create the web query
	 * @param aReqData [out] The request data created by the plugin
	 * @return SmfPluginError
	 */
	SmfPluginError createMusicServiceRequest ( QObject *aPlugin, 
			const SmfRequestTypeID &aOperation, 
			QByteArray &aInputData,
			SmfPluginRequestData &aReqData );


	/**
	 * Method to create a web query to do music playlist related operation
	 * @param aPlugin The instance of the loaded plugin that performs the 
	 * contact fetch operation.
	 * @param aOperation The type of operation to be performed
	 * @param aInputData The data required to create the web query
	 * @param aReqData [out] The request data created by the plugin
	 * @return SmfPluginError
	 */
	SmfPluginError createMusicPlaylistRequest ( QObject *aPlugin, 
			const SmfRequestTypeID &aOperation, 
			QByteArray &aInputData,
			SmfPluginRequestData &aReqData );

	/**
	 * Method to create a web query to do gallery related operation
	 * @param aPlugin The instance of the loaded plugin that performs the 
	 * contact fetch operation.
	 * @param aOperation The type of operation to be performed
	 * @param aInputData The data required to create the web query
	 * @param aReqData [out] The request data created by the plugin
	 * @return SmfPluginError
	 */
	SmfPluginError createGalleryRequest ( QObject *aPlugin, 
			const SmfRequestTypeID &aOperation, 
			QByteArray &aInputData,
			SmfPluginRequestData &aReqData );
	
	/**
	 * Method called to create a synchronous plugin request.
	 * @param aPlugin The instance of the loaded plugin that performs the 
	 * contact fetch operation.
	 * @param aOperation The type of operation to be performed
	 * @param aInputData The data required to create the web query
	 * @param aOutputData [out] The output data to be filled by the plugins
	 * @return SmfPluginError
	 */
	SmfPluginError createSyncRequest ( QObject *aPlugin, 
			const SmfRequestTypeID &aOperation, 
			QByteArray &aInputData,
			QByteArray &aOutputData );
	
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
	SmfError responseAvailable ( SmfPluginBase *aInstance,
			const SmfRequestTypeID &aOperation,
			const SmfTransportResult &aTransportResult, 
			QByteArray *aResponse,
			QVariant *aResult,
			SmfPluginRetType &aRetType,
			SmfResultPage &aPageResult );
	
	/**
	 * Method to serialize the result of parsing (which is done by the 
	 * plugins) to QByteArray to be sent to Smf server.
	 * @param aOperation The type of operation to be performed
	 * @param aResult The data to be serialized
	 * @param aDataStream Stream to be written
	 */
	void serializeResult ( const SmfRequestTypeID &aOperation, 
			QVariant* aResult,
			QDataStream &aDataStream );
	
	/**
	 * Method to convert SmfPluginError Error to the type SmfError 
	 * @param aPluginError The Error code returned by the plugin
	 * @param aSMFError [out] The Smf specific common error code
	 */
	void convertPluginErrorType( const SmfPluginError &aPluginError,  
			SmfError &aSMFError );
	
	/**
	 * Method to convert SmfTransportResult Error to the type SmfError 
	 * @param aTMError The Error code returned by the TM
	 * @param aSMFError [out] The Smf specific common error code
	 */
	void convertNetworkErrorType( const SmfTransportResult &aTMError,  
			SmfError &aSMFError );

	friend class SmfPluginManager;
	
private:
	/**
	 * The single instance of SmfPluginManagerUtil
	 */
	static SmfPluginManagerUtil* m_myInstance;
	
	};

#endif /* SMFPLUGINMANAGERUTIL_H_ */
