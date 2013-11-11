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
 * The Plugin util class provides the information required for the 
 * plugins from Smf framework 
 *
 */

#include <smfpluginutil.h>


/**
 * Constructor
 */
SmfPluginUtil::SmfPluginUtil ( )
	{
	m_jsonParser = new QJson::Parser;
	}
	
/**
 * Destructor
 */
SmfPluginUtil::~SmfPluginUtil ( )
	{
	if(m_jsonParser)
		delete m_jsonParser;
	}
	

/**
* Read JSON string from the I/O Device and converts it to a QVariant object
* @param io Input output device
* @param ok if a conversion error occurs, *ok is set to false; otherwise *ok is set to true.
* @returns a QVariant object generated from the JSON string
*/
QVariant SmfPluginUtil::parse ( QIODevice* io, bool* ok )
	{
	return m_jsonParser->parse(io, ok);
	}

/**
* This is a method provided for convenience.
* @param jsonData data containing the JSON object representation
* @param ok if a conversion error occurs, *ok is set to false; otherwise *ok is set to true.
* @returns a QVariant object generated from the JSON string
* @sa errorString
* @sa errorLine
*/
QVariant SmfPluginUtil::parse ( const QByteArray& jsonData, bool* ok )
	{
	return m_jsonParser->parse(jsonData, ok);
	}

/**
* This method returns the error message tha ocuured during last parsing
* @returns a QString object containing the error message of the last parse operation
*/
QString SmfPluginUtil::errorString ( ) const
	{
	return m_jsonParser->errorString();
	}

/**
* This method returns line number where the last QJson parsing error occurred
* @returns the line number where the error occurred
 */
int SmfPluginUtil::errorLine ( ) const
	{
	return m_jsonParser->errorLine();
	}

/**
 * Method called by plugins to get the OAuth Keys.
 * @param aKeys [out] The OAuth keys
 * @param aRegToken The plugin's registration token
 * @param aValidity The validity of the keys
 * @param aPluginID The ID of the plugin that requests for 
 * the OAuth keys
 */
void SmfPluginUtil::getAuthKeys( SmfAuthParams &aKeys, 
		const QString aRegToken, 
		const QDateTime& aValidity,
		const QString aPluginID )
	{
	aKeys.clear();
	
	// Invalid arguments
	if( (0 == aRegToken.size()) || 
		(0 == aPluginID.size()) || 
		(aValidity.isNull()) )
		return;
	
	// Call Credential manager function to get the keys passing the registration token and the plugin
	SmfCredMgrClient csmClient;
	csmClient.AuthDataSet(aRegToken,aValidity,aKeys);
	}

/**
 * Method called by plugins to get the Nonce value
 * @param aNonceString [out] The Nonce string
 * @param aLength Lenth of the NONCE tobe generated
 */
void SmfPluginUtil::getNonce( QString &aNonceString, const qint64 aLength )
	{
	aNonceString.clear();
	
	// Invalid argument
	if(aLength < 0)
		return;
	
	// Call Credential manager function to get the nonce string
	SmfCredMgrClient csmClient;
	aNonceString = csmClient.GenerateNONCE(aLength);
	}

/**
 * Method called by plugins to generate a parameters string required to 
 * access Protected Resources using OAuth authorization.
 * @param aRequestUrl The request URL
 * @param aOperation The type of http operation
 * @param aToken The access token
 * @param aTokenSecret The token secret
 * @param aSignatureMethod The signature method to be used
 * @param aParams A map of parameters to its values
 * @param aMode The mode of creation of the request
 * @return The cretd parameter string
 */
QByteArray SmfPluginUtil::createParameterString( const QString &aRequestUrl, 
		QNetworkAccessManager::Operation aOperation, 
		const QByteArray &aToken, 
		const QByteArray &aTokenSecret, 
		const SmfSignatureMethod aSignatureMethod, 
		const QMultiMap<QByteArray, QByteArray> &aParams, 
		const SmfParsingMode aMode)
	{
	Q_UNUSED(aRequestUrl)
	Q_UNUSED(aOperation)
	Q_UNUSED(aToken)
	Q_UNUSED(aTokenSecret)
	Q_UNUSED(aSignatureMethod)
	Q_UNUSED(aParams)
	Q_UNUSED(aMode)
	return QByteArray();
	//// Call Credential manager function to create and sign the parameter string
	}

