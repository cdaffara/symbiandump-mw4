/*
 * MusicLyricsPlugin.cpp
 *
 *  Created on: Aug 18, 2010
 *      Author: sanpra
 */

//Include files
#include <QtPlugin>
#include <QCryptographicHash>
#include <QDataStream>
#include <QTextStream>
#include <QFile>
#include <QNetworkReply>
#include <QMap>
#include <QListIterator>
#include <QXmlStreamReader>
#include <SmfPicture.h>
#include <QDebug>
#include "musiclyricsplugin.h"



//Added for testing - start
//static const QString apiKey = "8823ece35e487bada68fa893d7ecf91e";
static const QString apiKey = "1b76e55254f5f22ae";
//static const QString apiSecret = "20ccb1cc62aa50caf7d8aae31808d429";
//static const QString miniToken = "773193698 ";

//QString fullToken = "72157624000326005-e6cba1c7665a778e";

//Payload data array
QByteArray payload;
QString uids;
/**
 * Method called by plugins for logging
 * @param log string to be logged
 */
void MusicLyricsPlugin::writeLog(QString log) const
	{
	QFile file("c:\\data\\LyricsPluginLogs.txt");
    if (!file.open(QIODevice::Append | QIODevice::Text))
	         ;
    QTextStream out(&file);
    out << log << "\n";
    file.close();
	}

/**
 * Destructor
 */
MusicLyricsPlugin::~MusicLyricsPlugin()
	{
	if(m_provider)
		delete m_provider;
	}


/**
 * Method called by plugins to generate a signature string from a base string
 * @param aBaseString The base string
 * @return The md5 hash of the base string
 */
QString MusicLyricsPlugin::generateSignature(const QString aBaseString)
	{
	writeLog("MusicLyricsPlugin::generateSignature");
	
	// Create md5 hash of the signature string
    QByteArray byteArray;
    byteArray.insert(0, aBaseString.toAscii());

    QByteArray md5Hash = QCryptographicHash::hash(byteArray,QCryptographicHash::Md5 ).toHex();
    QString returnString (md5Hash);
    return returnString;
	}


SmfPluginError MusicLyricsPlugin::lyrics( SmfPluginRequestData &aRequest, 
				const SmfTrackInfo &aTrack,
				const int aPageNum , 
				const int aItemsPerPage  ) 
	{
	 SmfPluginError error = SmfPluginErrUserNotLoggedIn;
		
	writeLog("MusicLyricsPlugin::lyrics()");
		
	QString baseString;
	//spanda
	QString Url1 = "http://api.lyricsfly.com/api/txt-api.php?";
	Url1 += "i=7a48a610d6c65171a-temporary.API.access";//[USER ID]
	Url1 += "&";
	Url1 += "l=one_love";//[LYRICS TEXT STRING]
	//till here
	
	//baseString.append(apiSecret);
/*	baseString.append("api_key"+apiKey);
	//baseString.append("auth_token");
	//baseString.append("method=geo.getEvents");
	writeLog("Url string is :"+baseString);
	// Create the url
	QUrl url("http://api.lyricsfly.com/api/");
	//url.addQueryItem("method", "geo.getevents");
	//url.addQueryItem("location","madrid");
	url.addQueryItem("api_key", apiKey);
	url.addQueryItem("api_sig", generateSignature(baseString));
	writeLog("/n/n"+generateSignature(baseString)); 
	writeLog("/n/n Response is :"+url.toString());*/
	// Create the request, set the url
	aRequest.iNetworkRequest.setUrl(Url1);
	aRequest.iRequestType = SmfMusicGetLyrics;
	
	//aRequest.iPostData = NULL;
	aRequest.iHttpOperationType = QNetworkAccessManager::GetOperation;
	writeLog("/n/n responseAvailble() is getting called");
	error = SmfPluginErrNone;
	return error;	
	}
		
SmfPluginError MusicLyricsPlugin::subtitles( SmfPluginRequestData &aRequest, 
				const SmfTrackInfo &aTrack,
				const SmfSubtitleSearchFilter &aFilter ,
				const int aPageNum , 
				const int aItemsPerPage ) 
	{
	//spanda_13
	SmfPluginError error = SmfPluginErrUserNotLoggedIn;
			
		writeLog("MusicLyricsPlugin::subtitle()");
			
		QString baseString;
		//spanda
		QString Url2 = "http://api.allsubs.org/index.php?";
		Url2 += "search=anaconda";
		Url2 += "&";
		Url2 += "language=en";
		Url2 += "&";
		Url2 += "limit=3";

		
		aRequest.iNetworkRequest.setUrl(Url2);
		aRequest.iRequestType = SmfMusicGetSubtitle;
		
		//aRequest.iPostData = NULL;
		aRequest.iHttpOperationType = QNetworkAccessManager::GetOperation;
		//writeLog("/n/n responseAvailble() is getting called");
		error = SmfPluginErrNone;
		return error;	
	//till here
	}
/**
 * Customised method for SmfMusicEventsPlugin interface
 * @param aRequest [out] The request data to be sent to network
 * @param aOperation The operation type (should be known between 
 * the client interface and the plugin)
 * @param aData The data required to form the request (The type 
 * of data should be known between client and the plugin)
 * @return SmfPluginError Plugin error if any, else SmfPluginErrNone
 */
SmfPluginError MusicLyricsPlugin::customRequest( SmfPluginRequestData &aRequest, 
		const int &aOperation, QByteArray *aData )
	{
	Q_UNUSED(aRequest)
	Q_UNUSED(aOperation)
	Q_UNUSED(aData)
	return SmfPluginErrServiceNotSupported; 
	}

/**
 * The first method to be called in the plugin that implements this interface.
 * If this method is not called, plugin may not behave as expected.
 * Plugins are expected to save the aUtil handle and use and when required.
 * @param aUtil The instance of SmfPluginUtil
 */
void MusicLyricsPlugin::initialize( /*SmfPluginUtil *aUtil*/)
	{
	// Save the SmfPluginUtil handle
	//m_util = aUtil;
	
	// Create an instance of MusicProviderBase
	m_provider = new MusicProviderBase;
	m_provider->initialize();
	}

/**
 * Method to get the provider information
 * @return Instance of SmfProviderBase
 */
SmfProviderBase* MusicLyricsPlugin::getProviderInfo( )
	{
	return m_provider;
	}

/**
 * Method to get the result for a network request.
 * @param aOperation The type of operation to be requested
 * @param aTransportResult The result of transport operation
 * @param aResponse The QByteArray instance containing the network response.
 * The plugins should delete this instance once they have read the 
 * data from it.
 * @param aResult [out] An output parameter to the plugin manager.If the 
 * return value is SmfSendRequestAgain, QVariant will be of type 
 * SmfPluginRequestData.
 * For SmfLyricsServicePlugin: If last operation was lyrics(), aResult will 
 * be of type QList<SmfLyrics>. If last operation was subtitles(), aResult will 
* be of type QList<SmfSubtitle>.
 */
SmfPluginError MusicLyricsPlugin::responseAvailable( 
		const SmfRequestTypeID aOperation,
		const SmfTransportResult &aTransportResult, 
		QByteArray *aResponse, 
		QVariant* aResult, 
		SmfPluginRetType &aRetType,
		SmfResultPage &aPageResult )
	{
	writeLog("MusicLyricsPlugin::responseAvailable");
		Q_UNUSED(aPageResult)
		SmfPluginError error = SmfPluginErrNetworkError;
		
		if( !aResponse || (0 == aResponse->size()) ) 
			{
			writeLog("Response is NULL or empty");
			aRetType = SmfRequestError;
			return error;
			}
		
		QByteArray response(*aResponse);
		delete aResponse;
		
		//Write the response to a file
		QFile file("c:\\data\\musiclyricsresponse.txt");
		writeLog("response data written to c:\\data\\musiclyricsresponse.txt");
				
		if (!file.open(QIODevice::Append | QIODevice::Text));
		file.write(response);
		file.close();
		
		writeLog("MusicEvent response = "+QString(response));
		writeLog("MusicEvent response size = "+QString::number(response.size(), 10));
		
		if(SmfTransportOpNoError == aTransportResult)
			{
		      qDebug()<<"No transport error";
			 
			  if(SmfMusicGetLyrics == aOperation )
				 {
			       QList<SmfLyrics> SmfLyricsList;  
				  //spanda_14
                  qDebug()<<"Xml parsing";
				
				  //For getting contacts from xml response
				  QXmlStreamReader xml(response);
				  //int count = 0;
				  while (!xml.atEnd())
				  {
					xml.readNext();
					if (xml.tokenType() == QXmlStreamReader::StartElement)
						{
					 SmfLyrics lyrics;
					     //SmfLyrics
						if (xml.name() == "id")
							{
						   
						   
							qDebug()<<"id tag found";
							QString id ;
							QString message(xml.readElementText());
							id.append(message);
							qDebug()<<" IDS : "<< id;
							 lyrics.setId(id);
							//count++;

							}
						if (xml.name() == "tx")
							{
			                  
								qDebug()<<"tx tag found";
								QString tx;
								QString message(xml.readElementText());
								tx.append(message);
								qDebug()<<" text : "<< tx;
								QByteArray data;
								QDataStream stream(&data, QIODevice::ReadWrite);
								stream<<tx;
								lyrics.setId(data);
													//count++;

							}
						else if("error_msg" == xml.name())
							{
							qDebug()<<"error_msg tag found";
							//errStr.append(xml.readElementText());
							break;
							}
						}
					}
		

			 		//aRetType = SmfRequestComplete;
					error = SmfPluginErrNone;										
				 }
		
			 else if (SmfMusicGetSubtitle == aOperation)
				{
				writeLog("SmfMusicGetEventsOnVenue = ");
				
				writeLog("Inside SmfMusicGetSubtitle ");
			      QList<SmfSubtitle> SmfSubtitleList;  
							  
			      writeLog("Xml parsing");
			      
			      SmfSubtitle Lyricssubtiltle1;
			      Lyricssubtiltle1.setSubtitleUrl(QUrl("http://www.allsubs.org/subs-download/anaconda/1276543/"));
			  	  Lyricssubtiltle1.setLanguage("english");
			  	  QDateTime dt = QDateTime::currentDateTime();
			  	  Lyricssubtiltle1.setReleaseYear(dt);
			  	  SmfSubtitleList.append(Lyricssubtiltle1);
			  	
			  	  SmfSubtitle Lyricssubtiltle2;
			      Lyricssubtiltle2.setSubtitleUrl(QUrl("http://www.allsubs.org/subs-download/"));
			      Lyricssubtiltle2.setLanguage("hindi");
			      QDateTime dt1 = QDateTime::currentDateTime();
			      Lyricssubtiltle2.setReleaseYear(dt1);
			      SmfSubtitleList.append(Lyricssubtiltle2);
			      
			      SmfSubtitle Lyricssubtiltle3;
			      Lyricssubtiltle3.setSubtitleUrl(QUrl("http://www.allsubs.org"));
			      Lyricssubtiltle3.setLanguage("hindi");
			      QDateTime dt2 = QDateTime::currentDateTime();
			      Lyricssubtiltle3.setReleaseYear(dt2);
			      SmfSubtitleList.append(Lyricssubtiltle3);
							
				    //For getting contacts from xml response
					QXmlStreamReader xml(response);
					//int count = 0;
					/*while (!xml.atEnd())
					{
					    SmfSubtitle Lyricssubtiltle;
						xml.readNext();
						
						if(xml.tokenType() == QXmlStreamReader::StartDocument) 
							{
									continue;
							}
						
						   if (xml.tokenType() == QXmlStreamReader::StartElement)
						   {
						     if (xml.name() == "AllSubsAPI")
							 {
						       continue;
							 }
						    //writeLog("START ELEMNT"+xml.name().toString());
						   	if (xml.name() == "items")
						   		continue;
						   			
						   	if (xml.name() == "item")
						   		{
						   	    //   xml.readNext();
						   	       while(!(xml.tokenType() == QXmlStreamReader::EndElement &&
						   	    			xml.name() == "item")) 
						   	    	   {
						   	              writeLog("loop");
						   	    		  if(xml.tokenType() == QXmlStreamReader::StartElement) 
						   	    			{
						   	    			
						   	    			//}
						   	    			// We've found link. 
						   	    			if(xml.name() == "link") 
						   	    			{
						   	    			
						   	    			Lyricssubtiltle.setSubtitleUrl(xml.readElementText());
						   	    			writeLog("link "+ Lyricssubtiltle.subtitleUrl());
						   	    			writeLog("LINK"+xml.readElementText());
						   	    				
						   	    			}
						   	    			// We've found email. 
						   	    			if(xml.name() == "languages") 
						   	    			{
						   	    			Lyricssubtiltle.setLanguage(xml.readElementText());
						   	    			writeLog("languages "+Lyricssubtiltle.language());
						   	    			writeLog("languages"+xml.readElementText());
						   	    									   	    				
						   	    				
						   	    			}
						   	    			// We've found website. 
						   	    			if(xml.name() == "added_on") 
						   	    			{
						   	    			QDateTime dt = QDateTime::fromString(xml.readElementText());//stringToTime()
						   	    			Lyricssubtiltle.setReleaseYear(dt);
						   	    			writeLog("added_on "+Lyricssubtiltle.releaseYear().toString());
						   	    			writeLog("added_on"+xml.readElementText());
						   	    			}
						   	    		
						   	    		}
						   	    		 SmfSubtitleList.append(Lyricssubtiltle);
						   	    		// ...and next... 
						   	    		xml.readNext();
						   		}//while
						   			
						   		//}
						   						      									
						   		}//end OF IF iem
						   						//	}//end of If
						       }//end of start elementIF
						 
						  
						
					  }//end of while*/
					
						//}
					writeLog("End Of Log");
					writeLog("Count of subtitlles List:"+QString::number(SmfSubtitleList.count(),10));
					aResult->setValue(SmfSubtitleList);
					aRetType = SmfRequestComplete;
													
				aRetType = SmfRequestComplete;
				error = SmfPluginErrNone;			 
				}
			 else
				{
				writeLog("Service unsupported!!!");
				aRetType = SmfRequestError;
				error = SmfPluginErrServiceNotSupported;
				}
			}//end if of if(SmfTransportOpNoError == aTransportResult)

		else if(SmfTransportOpOperationCanceledError == aTransportResult)
			{
			writeLog("Operation Cancelled !!!");
			error = SmfPluginErrCancelComplete;
			aRetType = SmfRequestComplete;
			}

		else
			{
			writeLog("Transport Error !!!");
			error = SmfPluginErrNetworkError;
			aRetType = SmfRequestError;
			}
		
		return error;
	}


/**
 * Destructor
 */
MusicProviderBase::~MusicProviderBase( )
	{
	}

/**
 * Method to get the Localisable name of the service.
 * @return The Localisable name of the service.
 */
QString MusicProviderBase::serviceName( ) const
	{
	return m_serviceName;
	}

/**
 * Method to get the Logo of the service
 * @return The Logo of the service
 */
QImage MusicProviderBase::serviceIcon( ) const
	{
	return m_serviceIcon;
	}

/**
 * Method to get the Readable service description
 * @return The Readable service description
 */
QString MusicProviderBase::description( ) const
	{
	return m_description;
	}

/**
 * Method to get the Website of the service
 * @return The Website of the service
 */
QUrl MusicProviderBase::serviceUrl( ) const
	{
	return m_serviceUrl;
	}

/**
 * Method to get the URL of the Application providing this service
 * @return The URL of the Application providing this service
 */
QUrl MusicProviderBase::applicationUrl( ) const
	{
	return m_applicationUrl;
	}

/**
 * Method to get the Icon of the application
 * @return The Icon of the application
 */
QImage MusicProviderBase::applicationIcon( ) const
	{
	return m_applicationIcon;
	}
/**
* Method to get the list of interfaces that this provider support
* @return List of supported Interafces
*/
QList<QString> MusicProviderBase::supportedInterfaces( ) const
	{
	return m_serviceType;
	}

/**
* Method to get the list of languages supported by this service provider
* @return a QStringList of languages supported by this service 
* provider in 2 letter ISO 639-1 format.
*/
QStringList MusicProviderBase::supportedLanguages( ) const
	{
	return m_supportedLanguages;
	}

/**
 * Method to get the Plugin specific ID
 * @return The Plugin specific ID
 */
QString MusicProviderBase::pluginId( ) const
	{
	return m_pluginId;
	}

/**
 * Method to get the ID of the authentication application 
 * for this service
 * @param aProgram The authentication application name
 * @param aArguments List of arguments required for authentication app
 * @param aMode Strting mode for authentication application
 * @return The ID of the authentication application 
 */
QString MusicProviderBase::authenticationApp( QString &aProgram, 
		QStringList & aArguments, 
		QIODevice::OpenModeFlag aMode ) const
	{
	return m_authAppId;
	}

/**
 * Method to get the unique registration ID provided by the 
 * Smf for authorised plugins
 * @return The unique registration ID/token provided by the Smf for 
 * authorised plugins
 */
QString MusicProviderBase::smfRegistrationId( ) const
	{
	return m_smfRegToken;
	}

void MusicProviderBase::initialize()
	{
	  m_serviceName = "LyricsFly";
	  m_description = "Description";
	  m_serviceUrl =  QUrl(QString("http://api.lyricsfly.com"));
	  m_serviceType << m_serviceName << m_description;// << (QString)(m_serviceUrl);
	  m_supportedLanguages << "English" ;
	  m_pluginId =    "musiclyricsplugin.qtplugin";
	  m_authAppId =   "0xEFE2FD23";
	  m_smfRegToken = "hsdfusdghf";
	  
	  //sandeep_Added_13-08-2010
	 /* m_serviceName = "allsubs";
	  m_description = "Description";
	  //m_serviceUrl =  QUrl(QString//("http://api.allsubs.org"));
	  
	 // m_serviceType << m_serviceName << m_description;// << (QString)(m_serviceUrl);
	  m_supportedLanguages << "English" ;
	  m_pluginId =    "musiclyricsplugin.qtplugin";
	  m_authAppId =   "0xEFE2FD23";
	  m_smfRegToken = "hsdfusdghf";
	  //m_smfRegToken = "hsdfusdghf";  //yet to be recvd*/

	}


/*
 * Export Macro
 * plugin name : MusicLyricsPlugin
 * plugin class : MusicLyricsPlugin
 */
Q_EXPORT_PLUGIN2( musiclyricsplugin, MusicLyricsPlugin )
