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
 * The SmfClient class provides a set of APIs to get list of service provider
 * who implement the given interface. Calling GetServices of the instance of this
 * class is mandatory before using any other interface.
 *
 */

#include <qdebug.h>

#include "smfclient.h"
#include "smfclient_p.h"

SmfClient::SmfClient()
	{
	//Create instance of the private wrapper
	m_private = new SmfClientPrivate(this);
	m_providerList = NULL;
	}

SmfClient::~SmfClient()
	{
	if(m_private)
		{
		delete m_private;
		m_private = NULL;
		}
	}

QList<SmfProvider>* SmfClient::GetServices(const QString& serviceName)
	{
	qDebug()<<"Inside SmfClient::GetServices()";
	
	if(m_providerList)
		{
		delete m_providerList;
		m_providerList = NULL;
		}
	//Getting the data synchronously
	m_providerList = m_private->GetServices(serviceName);
	qDebug()<<"After m_private->GetServices";
	
	return m_providerList;
	}


/**
* This method returns the error message for the mentioned argument error code
* @param aErrorCode The SmfError code for which the error string is required
* @returns a QString object containing the required error message
*/
QString SmfClient::errorString ( const SmfError &aErrorCode ) const
	{
	QString str;
	str.clear();
	
	switch(aErrorCode)
		{
		case SmfNoError:								//0
			str.append("No Error");
			break;
			
		case SmfInvalidInterface:						//1
		    str.append("Smf Error : Invalid interface name");
			break;
			
		case SmfNoAuthorizedPlugin:						//2
		    str.append("Smf Error : No Authorised plugins are available");
			break;
			
		case SmfClientAuthFailed:						//3
		    str.append("Smf Error : Smf Client authorisation failed");
			break;
			
		case SmfPMPluginNotFound:						//4
		    str.append("Smf Error : Plugin not found");
			break;
			
		case SmfPMPluginNotLoaded:						//5
		    str.append("Smf Error : Plugin could not be loaded");
			break;
			
		case SmfPMPluginLoadError:						//6
		    str.append("Smf Error : Plugin Load failed");
			break;
			
		case SmfPMPluginNotAuthorised:					//7
		    str.append("Smf Error : Plugin not authorised");
			break;
			
		case SmfPMPluginRequestCreationFailed:			//8
		    str.append("Smf Error : Plugin request creation failed");
			break;
			
		case SmfPMPluginUnknownPluginService:			//9
		    str.append("Smf Error : Requested Plugin service not supported");
			break;
			
		case SmfPMPluginUnknownHttpService:				//10
		    str.append("Smf Error : Unknown http service requested by plugin");
			break;
			
		case SmfPMPluginRequestSendingFailed:			//11
		    str.append("Smf Error : Request created by plugin could not be send");
			break;
			
		case SmfPMPluginSOPCheckFailed:					//12
		    str.append("Smf Error : Same Origin Policy violation by plugin");
			break;
			
		case SmfPMPluginSendRequestAgain:				//13
		    str.append("Smf Error : Send request again");
			break;
			
		case SmfPluginErrorTooManyRequest:				//14
		    str.append("Plugin Error: Too many requests");
			break;
			
		case SmfPluginErrorRequestQuotaExceeded:		//15
		    str.append("Plugin Error: Request Quota Exceeded");
			break;
			
		case SmfPluginErrorInvalidRequest:				//16
		    str.append("Plugin Error: Invalid Request");
			break;
			
		case SmfPluginErrorUserNotLoggedIn:				//17
		    str.append("Plugin Error: User not logged in");
			break;
			
		case SmfPluginErrorAuthenticationExpired:		//18
		    str.append("Plugin Error: Authentication Expired");
			break;
			
		case SmfPluginErrorPermissionDenied:			//19
		    str.append("Plugin Error: Permission denied");
			break;
			
		case SmfPluginErrorInvalidApplication:			//20
		    str.append("Plugin Error: Request Quota Exceeded");
			break;
			
		case SmfPluginErrorServiceUnavailable:			//21
			str.append("Plugin Error: Service unavailable");
			break;
			
		case SmfPluginErrorServiceTemporaryUnavailable:	//22
		    str.append("Plugin Error: Service temporarily unavailable");
			break;
			
		case SmfPluginErrorFormatNotSupported: 			//23
		    str.append("Plugin Error: Format not supported");
			break;
			
		case SmfPluginErrorDataSizeExceeded :			//24
		    str.append("Plugin Error: Data Size exceeded");
			break;
			
		case SmfPluginErrorInvalidArguments:			//25
		    str.append("Plugin Error: Invalid request arguments");
			break;
			
		case SmfPluginErrorParsingFailed:				//26
		    str.append("Plugin Error: Parsing failed");
			break;
			
		case SmfPluginErrorNetworkError:				//27
		    str.append("Plugin Error: Network Error");
			break;
			
		case SmfPluginErrorCancelComplete:				//28
		    str.append("Smf : Operation Cancelled");
			break;
			
		case SmfTMConnectionRefusedError:				//29
		    str.append("Network Error: Connection Refused");
			break;
			
		case SmfTMRemoteHostClosedError:				//30
		    str.append("Network Error: Remote Host Closed");
			break;
			
		case SmfTMHostNotFoundError:					//31
		    str.append("Network Error: Host Not Found");
			break;
			
		case SmfTMTimeoutError:							//32
		    str.append("Network Error: Time Out");
			break;
			
		case SmfTMOperationCanceledError:				//33
		    str.append("Network Error: Operation cancelled");
			break;
			
		case SmfTMSslHandshakeFailedError:				//34
		    str.append("Network Error: Handshake Failed");
			break;
			
		case SmfTMProxyConnectionRefusedError:			//35
		    str.append("Network Error: Proxy Connection Refused");
			break;
			
		case SmfTMProxyConnectionClosedError:			//36
		    str.append("Network Error: Proxy Connection Closed");
			break;
			
		case SmfTMProxyNotFoundError:					//37
		    str.append("Network Error: Proxy Not Found");
			break;
			
		case SmfTMProxyTimeoutError:					//38
		    str.append("Network Error: Proxy Timeout");
			break;
			
		case SmfTMProxyAuthenticationRequiredError:		//39
			str.append("Network Error: Proxy Authentication Required");
			break;
			
		case SmfTMContentAccessDenied:					//40
		    str.append("Network Error: Content Access Denied");
			break;
			
		case SmfTMContentOperationNotPermittedError:	//41
		    str.append("Network Error: Content Operation Not Permitted");
			break;
			
		case SmfTMContentNotFoundError:					//42
		    str.append("Network Error: Content Not Found");
			break;
			
		case SmfTMAuthenticationRequiredError:			//43
		    str.append("Network Error: Remote Server Required Authentication");
			break;
			
		case SmfTMContentReSendError:					//44
		    str.append("Network Error: Content Resend Error");
			break;
			
		case SmfTMProtocolUnknownError:					//45
		    str.append("Network Error: Protocol Unknown");
			break;
			
		case SmfTMProtocolInvalidOperationError:		//46
		    str.append("Network Error: Protocol Invalid Operation");
			break;
			
		case SmfTMUnknownNetworkError:					//47
		    str.append("Network Error: Unknown Network Error");
			break;
			
		case SmfTMUnknownProxyError:					//48
		    str.append("Network Error: Unknown Proxy Error");
			break;
			
		case SmfTMUnknownContentError:					//49
		    str.append("Network Error: Unknown Content Error");
			break;
			
		case SmfTMProtocolFailure:						//50
		    str.append("Network Error: Protocol Failure");
			break;
			
		case SmfTMUnknownError:							//51
		    str.append("Network Error: Unknown Error");
			break;
			
		case SmfTMIAPChanged:							//52
		    str.append("Network Error: IAP Changed");
			break;
			
		case SmfTMCancelled:							//53
		    str.append("Network Error: Cancelled");
			break;
			
		case SmfTMUnsupportedContentEncodingFormat:		//54
		    str.append("Smf Error: Unsupported Content Encoding Format");
			break;
			
		case SmfTMInitNetworkNotAvailable :				//55
		    str.append("Smf Error: Network Not Available");
			break;
			
		case SmfTMInitRoamingNetworkUsageNotEnabled:	//56
		    str.append("Smf Error: Network Usage While Roaming is Disabled");
			break;
			
		case SmfTMGzipMemoryError:						//57
		    str.append("Smf Error: Gzip Memory Error");
			break;
			
		case SmfTMGzipStreamError:						//58
		    str.append("Smf Error: Gzip Stream Error");
			break;
			
		case SmfTMGzipDataError:						//59
		    str.append("Smf Error: Gzip Data Error");
			break;
			
		case SmfMemoryAllocationFailure:				//60
		    str.append("Smf Error: Memory failed");
			break;
			
		case SmfDbOpeningError:                         //61
		    str.append("Smf Database Error: Opening Error");
			break;
			
		case SmfDbQueryExecutonError:					//61
		    str.append("Smf Database Error: Query Error");
			break;
			
		case SmfDbContactNotExist:						//63
		    str.append("Smf Database Error: No contacts in Database");
			break;
			
		case SmfErrItemNotInRelation:                   //64
		    str.append("Smf Error: Item not in relation");
			break;
			
		case SmfErrInvalidRelation:                     //65
			str.append("Smf Error: Invalid relation");
			break;
			
		case SmfUnknownError:							//66
		default:
			str.append("Smf Error: Unknown Error");
		}
		
	return str;
	}

