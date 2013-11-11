/*
 * lastfmmusiceventplugin.cpp
 *
 *  Created on: Jun 21, 2010
 *      Author: ankitg
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
#include <QSettings>
#include "lastfmmusiceventplugin.h"



//Added for testing - start
//static const QString apiKey = "8823ece35e487bada68fa893d7ecf91e";
//static const QString apiKey = "50ac6d6581c3784efe40748fe87d8413";
//static const QString apiSecret = "20ccb1cc62aa50caf7d8aae31808d429";
//static const QString apiSecret = "623f4274bcea772fd6f411cf298b6bfa";
//static const QString miniToken = "773193698 ";

//static const QString apiKey = "eb3a2d8361718782ebf3887f2bb8775b";
//static const QString apiSecret = "7a390c4c1ec4fef41cef4d77354b30f8"; // created on 12th Aug

static const QString apiKey = "50ac6d6581c3784efe40748fe87d8413";
static const QString apiSecret = "623f4274bcea772fd6f411cf298b6bfa"; // created on 23th Aug

//QString fullToken = "72157624000326005-e6cba1c7665a778e";

//Payload data array
QByteArray payload;
QString uids;
/**
 * Method called by plugins for logging
 * @param log string to be logged
 */
void MusicEventPlugin::writeLog(QString log) const
	{
	QFile file("c:\\data\\PluginLogs.txt");
    if (!file.open(QIODevice::Append | QIODevice::Text))
	         ;
    QTextStream out(&file);
    out << log << "\n";
    file.close();
	}
/**
 * Method to interpret the key sets obtained from credential manager 
 * @param aApiKey [out] The api key
 * @param aApiSecret [out] The api secret
 * @param aSessionKey [out] The session key
 * @param aSessionSecret [out] The session secret
 * @param aAppId [out] The application ID
 */
void MusicEventPlugin::fetchKeys( QString &aApiKey, 
		QString &aApiSecret, 
		QString &aSessionKey, 
		QString &aSessionSecret )
	{
	writeLog("Inside MusicEventPlugin::fetchKeys()");

	writeLog("Reg Token = "+m_provider->m_smfRegToken);
	writeLog("Expiry Date as int = "+m_provider->m_validity.toTime_t());
	
	SmfAuthParams keys;
	SmfPluginUtil util;
	util.getAuthKeys(keys, m_provider->m_smfRegToken, 
			m_provider->m_validity, m_provider->m_pluginId);
	
	writeLog("Number of key-value pairs = "+keys.count());
	
    QByteArray keyName;
    keyName.append("ApiKey");
	aApiKey.append(keys.value(keyName));
	
    keyName.clear();
    keyName.append("ApiSecret");
	aApiSecret.append(keys.value(keyName));
	
	keyName.clear();
    keyName.append("SessionKey");
	aSessionKey.append(keys.value(keyName));
	
	keyName.clear();
    keyName.append("SessionSecret");
	aSessionSecret.append(keys.value(keyName));
	
	writeLog("Api Key = "+aApiKey);
	writeLog("Api Secret = "+aApiSecret);
	writeLog("session Key = "+aSessionKey);
	writeLog("session Secret = "+aSessionSecret);
	}
/**
 * Destructor
 */
MusicEventPlugin::~MusicEventPlugin()
	{
	if(m_provider)
		delete m_provider;
	}


/**
 * Method called by plugins to generate a signature string from a base string
 * @param aBaseString The base string
 * @return The md5 hash of the base string
 */
QString MusicEventPlugin::generateSignature(const QString aBaseString)
	{
	writeLog("MusicEventPlugin::generateSignature");
	
	// Create md5 hash of the signature string
    QByteArray byteArray;
    byteArray.insert(0, aBaseString.toAscii());

    QByteArray md5Hash = QCryptographicHash::hash(byteArray,QCryptographicHash::Md5 ).toHex();
    QString returnString (md5Hash);
    return returnString;
	}

/**
 * Method to get the events based on location
 * @param aRequest [out] The request data to be sent to network
 * @param aLocation Location of the event
 * @param aPageNum The page to be extracted
 * @param aItemsPerPage Number of items per page
 * @return SmfPluginError Plugin error if any, else SmfPluginErrNone
 */
/*SmfPluginError MusicEventPlugin::events( SmfPluginRequestData &aRequest,
		const QGeoPositionInfo &aLocation,const int aPageNum, 
		const int aItemsPerPage)
	{
	Q_UNUSED(aItemsPerPage);
	SmfPluginError error = SmfPluginErrUserNotLoggedIn;
	
	writeLog("MusicEventPlugin::events()");
	
	QString baseString;
	baseString.append(apiSecret);
	baseString.append("api_key"+apiKey);
	baseString.append("auth_token");
	baseString.append("formatjson");
	baseString.append("location=Delhi");
	baseString.append("method=geo.getEvents");
	writeLog("Url string is :"+baseString);
	
	// Create the url
	 QUrl url("http://ws.audioscrobbler.com/2.0/?");
	 url.addQueryItem("method", "geo.getevents");
	 url.addQueryItem("location","madrid");
	 url.addQueryItem("api_key", apiKey);
	 url.addQueryItem("format","json");				
	 url.addQueryItem("auth_token","");
	 url.addQueryItem("api_sig", generateSignature(baseString));
	 writeLog("/n/n"+generateSignature(baseString)); 
	 writeLog("/n/n Response is :"+url.toString());
				// Create the request, set the url
	 aRequest.iNetworkRequest.setUrl(url);
	 aRequest.iRequestType = SmfMusicGetEventsOnLoc;
	 aRequest.iPostData = NULL;
	 aRequest.iHttpOperationType = QNetworkAccessManager::GetOperation;
	 error = SmfPluginErrNone;
	 return error;	
	}
		*/
/**
 * Method to get the venues based on location
 * @param aRequest [out] The request data to be sent to network
 * @param aLocation Location of the venue
 * @param aPageNum The page to be extracted
 * @param aItemsPerPage Number of items per page
 * @return SmfPluginError Plugin error if any, else SmfPluginErrNone
 */
SmfPluginError MusicEventPlugin::venues( SmfPluginRequestData &aRequest,
		const SmfLocation &aLocation,const int aPageNum, 
		const int aItemsPerPage)
	{
		Q_UNUSED(aItemsPerPage);
		Q_UNUSED(aPageNum);
		
		SmfPluginError error = SmfPluginErrUserNotLoggedIn;
		 	
		// Get the key sets from SMF Plugin Utility class.
		QString apiKey;
		QString apiSecret;
		QString sessionKey;
		QString sessionSecret;
		fetchKeys(apiKey, apiSecret, sessionKey, sessionSecret);
		writeLog("API KEY"+apiKey);
		writeLog("API SECRET"+apiSecret);
		writeLog("SessionKey"+sessionKey);
		writeLog("sessionSecret"+sessionSecret);
		writeLog("MusicEventPlugin::Venues()");
		//QString qName("New York");
		//aLocation.setCity(qName);
		//QString qLocation = aLocation.city();
		//640418
		//QString qLocation("New York");
/*		QString baseString;
		baseString.append(apiSecret);
		baseString.append("api_key"+apiKey);
		baseString.append("auth_token");
		baseString.append("formatjson");
		baseString.append("location ="+aLocation.city());
		baseString.append("method=geo.getEvents");
		writeLog("Url string is :"+baseString);
		 	
		//Create the url
		QUrl url("http://ws.audioscrobbler.com/2.0/?");
		url.addQueryItem("method", "geo.getevents");
		url.addQueryItem("location",aLocation.city());
		url.addQueryItem("api_key", apiKey);
		url.addQueryItem("api_key", apiKey);
		url.addQueryItem("format","json");				
		url.addQueryItem("auth_token","");
		url.addQueryItem("api_sig", generateSignature(baseString));
		
		writeLog("/n/n"+generateSignature(baseString)); 
		writeLog("/n/n Response is :"+url.toString());
		//Create the request, set the url
		aRequest.iNetworkRequest.setUrl(url);
		aRequest.iRequestType = SmfEventsGetVenues;
		aRequest.iPostData = NULL;
		aRequest.iHttpOperationType = QNetworkAccessManager::GetOperation;
		error = SmfPluginErrNone;
		return error;	*/
		

	    QString baseString;
		baseString.append(apiSecret);
		baseString.append("api_key"+apiKey);
		baseString.append("auth_token");
		baseString.append("formatjson");
		baseString.append("location=Newyork");
		baseString.append("method=geo.getEvents");
		writeLog("Url string is :"+baseString);
		
		// Create the url
		 QUrl url("http://ws.audioscrobbler.com/2.0/?");
		 url.addQueryItem("method", "geo.getevents");
		 url.addQueryItem("location","Newyork");
		 url.addQueryItem("api_key", apiKey);
		 url.addQueryItem("format","json");				
		 url.addQueryItem("auth_token","");
		 url.addQueryItem("api_sig", generateSignature(baseString));
		 writeLog("/n/n"+generateSignature(baseString)); 
		 writeLog("/n/n Response is :"+url.toString());
					// Create the request, set the url
		 aRequest.iNetworkRequest.setUrl(url);
		 aRequest.iRequestType = SmfEventsGetEvents;
		 aRequest.iPostData = NULL;
		 aRequest.iHttpOperationType = QNetworkAccessManager::GetOperation;
		 error = SmfPluginErrNone;
		 return error;	

	
	}

/**
 * Method to get the events based on venues
 * @param aRequest [out] The request data to be sent to network
 * @param aVenue Venue of the event
 * @param aPageNum The page to be extracted
 * @param aItemsPerPage Number of items per page
 * @return SmfPluginError Plugin error if any, else SmfPluginErrNone
 */
SmfPluginError MusicEventPlugin::events( SmfPluginRequestData &aRequest,
		const SmfLocation &aVenue,const int aPageNum, 
		const int aItemsPerPage)
	{
	Q_UNUSED(aItemsPerPage);
	Q_UNUSED(aPageNum);
	
	SmfPluginError error = SmfPluginErrUserNotLoggedIn;
		
	
	// Get the key sets from SMF Plugin Utility class.
		QString apiKey;
		QString apiSecret;
		QString sessionKey;
		QString sessionSecret;
		fetchKeys(apiKey, apiSecret, sessionKey, sessionSecret);
		writeLog("API KEY"+apiKey);
		writeLog("API SECRET"+apiSecret);
		writeLog("SessionKey"+sessionKey);
		writeLog("sessionSecret"+sessionSecret);
	
	    QString baseString;
		baseString.append(apiSecret);
		baseString.append("api_key"+apiKey);
		baseString.append("auth_token");
		baseString.append("formatjson");
		baseString.append("location="+aVenue.city());
		baseString.append("method=geo.getEvents");
		writeLog("Url string is :"+baseString);
		
		// Create the url
		 QUrl url("http://ws.audioscrobbler.com/2.0/?");
		url.addQueryItem("method", "geo.getevents");
		url.addQueryItem("location",aVenue.city());
		url.addQueryItem("api_key", apiKey);
		url.addQueryItem("format","json");				
		url.addQueryItem("auth_token","");
		url.addQueryItem("api_sig", generateSignature(baseString));
		writeLog("/n/n"+generateSignature(baseString)); 
		writeLog("/n/n Response is :"+url.toString());
		//Create the request, set the url
		 aRequest.iNetworkRequest.setUrl(url);
		 aRequest.iRequestType = SmfEventsGetEvents;
		 aRequest.iPostData = NULL;
		 aRequest.iHttpOperationType = QNetworkAccessManager::GetOperation;
		 error = SmfPluginErrNone;
		 return error;	

	}

/**
 * Method to post events
 * @param aRequest [out] The request data to be sent to network
 * @param aEventList The list of events to be posted
 * @return SmfPluginError Plugin error if any, else SmfPluginErrNone
 */
SmfPluginError MusicEventPlugin::postEvents( SmfPluginRequestData &aRequest,
		const QList<SmfEvent> &aEventList )
	{
	       Q_UNUSED(aEventList)
		   SmfPluginError error = SmfPluginErrUserNotLoggedIn;
		   writeLog("MusicEventPlugin::events()");
		   int count =aEventList.count();
		   for(int i=0;i<count;i++)
			   { 
		        SmfEvent ievent=aEventList[i];
		        writeLog("Event List"+ievent.id());
			   }
			// Get the key sets from SMF Plugin Utility class.
			QString apiKey;
			QString apiSecret;
			QString sessionKey;
			QString sessionSecret;
			fetchKeys(apiKey, apiSecret, sessionKey, sessionSecret);
			writeLog("API KEY"+apiKey);
			writeLog("API SECRET"+apiSecret);
			writeLog("SessionKey"+sessionKey);
			writeLog("sessionSecret"+sessionSecret);
		/*	QString baseString;
			
			baseString.append(apiSecret);
			baseString.append("api_key"+apiKey);
			baseString.append("auth_token");
			baseString.append("formatjson");
		//	baseString.append("method=event.shout"); // needs authentication
		//	baseString.append("Event"+"aEventList.at(0)");
			writeLog("Url string is :"+baseString);
			
		   //Create the url
			 QUrl url("http://ws.audioscrobbler.com/2.0/?");
			 url.addQueryItem("method", "event.shout");   // needs authentication
			 url.addQueryItem("api_key", apiKey);
			 url.addQueryItem("format","json");	
		//	 url.addQueryItem("Event","aEventList.at(0)");
			 url.addQueryItem("auth_token","");
			 url.addQueryItem("api_sig", generateSignature(baseString));
			 writeLog("/n/n Response is :"+url.toString());
						// Create the request, set the url
			 aRequest.iNetworkRequest.setUrl(url);
			 aRequest.iRequestType = SmfEventsPostEvents;
			 aRequest.iPostData = NULL;
			 
			 aRequest.iHttpOperationType = QNetworkAccessManager::GetOperation;
			 error = SmfPluginErrNone;
			 return error;	*/
			
		   //For Testing Only      
		    QString baseString;
					
		    //QString strId("328799");
			
		    QString eventId("328799");//= aEvent.id();
			//QString eventId;
			//eventId = aEvent.id();
			baseString.append(apiSecret);
		    baseString.append("api_key"+apiKey);
					//baseString.append("auth_token");
			baseString.append("formatjson");
			baseString.append("method=event.getattendees");
			baseString.append("event"+eventId);
			writeLog("Url string is :"+baseString);
					
			//Create the url
			QUrl url("http://ws.audioscrobbler.com/2.0/?");
			url.addQueryItem("method", "event.getattendees");
			url.addQueryItem("event",eventId);
			url.addQueryItem("api_key", apiKey);
			url.addQueryItem("format","json");					 
			//url.addQueryItem("auth_token","");
			//url.addQueryItem("api_sig", generateSignature(baseString));
			writeLog("/n/n Response is :"+url.toString());
			//Create the request, set the url
			aRequest.iNetworkRequest.setUrl(url);
			aRequest.iRequestType = SmfEventsPostEvents;
			aRequest.iPostData = NULL;
		
			aRequest.iHttpOperationType = QNetworkAccessManager::GetOperation;
			error = SmfPluginErrNone;
			return error;	
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
SmfPluginError MusicEventPlugin::attendees( SmfPluginRequestData &aRequest,
			const SmfEvent &aEvent, 
			const int aPageNum ,
			const int aItemsPerPage )
	{
		Q_UNUSED(aItemsPerPage);
		SmfPluginError error = SmfPluginErrUserNotLoggedIn;
		writeLog("MusicEventPlugin::events()");
				
		// Get the key sets from SMF Plugin Utility class.
		QString apiKey;
		QString apiSecret;
		QString sessionKey;
		QString sessionSecret;
		fetchKeys(apiKey, apiSecret, sessionKey, sessionSecret);
		writeLog("API KEY"+apiKey);
		writeLog("API SECRET"+apiSecret);
		writeLog("SessionKey"+sessionKey);
		writeLog("sessionSecret"+sessionSecret);
	    QString baseString;
				
	    //QString strId("328799");
		//aEvent.setId(strId);
	    //QString eventId = aEvent.id();
		QString eventId;
		eventId = aEvent.id();
		baseString.append(apiSecret);
	    baseString.append("api_key"+apiKey);
				//baseString.append("auth_token");
		baseString.append("formatjson");
		baseString.append("method=event.getattendees");
		baseString.append("event"+eventId);
		writeLog("Url string is :"+baseString);
				
		//Create the url
		QUrl url("http://ws.audioscrobbler.com/2.0/?");
		url.addQueryItem("method", "event.getattendees");
		url.addQueryItem("event",eventId);
		url.addQueryItem("api_key", apiKey);
		url.addQueryItem("format","json");					 
				// url.addQueryItem("auth_token","");
				// url.addQueryItem("api_sig", generateSignature(baseString));
		writeLog("/n/n Response is :"+url.toString());
		//Create the request, set the url
		aRequest.iNetworkRequest.setUrl(url);
		aRequest.iRequestType = SmfEventsGetEventAttendees;
		aRequest.iPostData = NULL;
				 
				
				 
		aRequest.iHttpOperationType = QNetworkAccessManager::GetOperation;
		error = SmfPluginErrNone;
		return error;	
	}
SmfPluginError MusicEventPlugin::customRequest( SmfPluginRequestData &aRequest, 
		const int &aOperation, QByteArray *aData )
	{
	Q_UNUSED(aRequest)
	Q_UNUSED(aOperation)
	Q_UNUSED(aData)
    //SmfEventsCustomRequest
	return SmfPluginErrServiceNotSupported; 
	}

/**
 * The first method to be called in the plugin that implements this interface.
 * If this method is not called, plugin may not behave as expected.
 * Plugins are expected to save the aUtil handle and use and when required.
 * @param aUtil The instance of SmfPluginUtil
 */
void MusicEventPlugin::initialize(/* SmfPluginUtil *aUtil */)
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
SmfProviderBase* MusicEventPlugin::getProviderInfo( )
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
 * For SmfGalleryPlugin: If last operation was pictures(), aResult will 
 * be of type QList<SmfPicture>. If last operation was description(), 
 * aResult will be of type QString. If last operation was upload() or 
 * postComment(), aResult will be of type bool.
 * @param aRetType [out] SmfPluginRetType
 * @param aPageResult [out] The SmfResultPage structure variable
 */
SmfPluginError MusicEventPlugin::responseAvailable( 
		const SmfRequestTypeID aOperation,
		const SmfTransportResult &aTransportResult, 
		QByteArray *aResponse, 
		QVariant* aResult, 
		SmfPluginRetType &aRetType,
		SmfResultPage &aPageResult )
	{
	writeLog("MusicEventPlugin::responseAvailable");
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
		QFile file("c:\\data\\musiceventresponse.txt");
		writeLog("response data written to c:\\data\\musiceventresponse.txt");
				
		if (!file.open(QIODevice::Append | QIODevice::Text));
		file.write(response);
		file.close();
		
		writeLog("MusicEvent response = "+QString(response));
		writeLog("MusicEvent response size = "+QString::number(response.size(), 10));
		
		if(SmfTransportOpNoError == aTransportResult)
			{
			  writeLog("No transport error");
			//  QVariantMap map1;
			 // bool resType=response.startsWith("<?xml");
			 /* if(resType) //XML REsponse
				{
				   writeLog("Before XML Parser--");
				  
			  }
			  else //JSON RESPONSE
			  {
				  bool ok;
				  writeLog("Before JSON Parser--");
		
				  SmfPluginUtil util;
				  QVariant result = util.parse(response, &ok);
				  if (!ok) 
				   {
						writeLog("An error occurred during json parsing");
						aRetType = SmfRequestError;
						return SmfPluginErrParsingFailed;
				   }
													
			 //map1 = result.toMap();*/
			  //}
			  if(SmfEventsGetEvents == aOperation /*|| SmfMusicGetEventsOnVenue == aOperation*/)
				 {
			       
			        QDateTime dt = QDateTime::currentDateTime();
			        QList<SmfEvent> eventlist;
			      
			        
			        
			        //Tested By Hardcoding
			        
			        SmfEvent event;
			        event.setId("111111");
			        event.setTitle("bangalore");
			      	event.setEventDateTime(dt);
			        QStringList stringList; //for artist
			        stringList.append("sssss");
			        stringList.append("aaaa");
			        SmfArtists artists;
			        artists.setNames(stringList);
					event.setArtists(artists);
					
					SmfLocation Location;
					Location.setCity("Bangalore");
					Location.setCountry("india");
					
					event.setVenue(Location);
					eventlist.append(event);
					
					//2nd Event
					SmfEvent event1;
					event1.setId("2222");
					event1.setTitle("delhi");
					event1.setEventDateTime(dt);
					QStringList stringList1; //for artist
					stringList1.append("ddddd");
				    stringList1.append("rrrrrr");
				    SmfArtists artists1;
					artists1.setNames(stringList1);
					event1.setArtists(artists1);
										
					SmfLocation Location1;
					Location1.setCity("Delhi");
					Location1.setCountry("india");
										
					event1.setVenue(Location1);
					eventlist.append(event1);
										
					writeLog("SmfEventsGetEvents = ");
					/*QVariantMap eventmap = map1["events"].toMap();
					QList<QVariant> list1 = eventmap["event"].toList();
					
					QListIterator<QVariant> iter(list1);
					writeLog("SmfEventsGetEvents123 = ");
					while(iter.hasNext())  //not runing for one event
					{
					  SmfEvent event;
					  QVariantMap map2 = iter.next().toMap();
					  writeLog("id = "+map2["id"].toString());
					  event.setId(map2["id"].toString());
					  writeLog("title = "+map2["title"].toString());
					  event.setTitle(map2["title"].toString());
					  writeLog("EventDateTime= "+map2["startDate"].toString());
					  event.setEventDateTime(map2["startDate"].toDateTime());
					 // writeLog("SetdUration= "+map2["startDate"].toDateTime());  //didn't find that
					  //for Getting the Artist List
					  QVariantMap map4 = map2["artists"].toMap();
					  QList<QVariant> artistList =map4["artist"].toList();
					  
					  writeLog("artist"+map4["artist"].toString());
					 					  
					 //  writeLog("artists"+artistList["artist"].toString());
					 // QList<QVariant> artistList =map2["artists"].toList();
					
					  
					 QListIterator<QVariant> iterartists(artistList);
					  SmfArtists artists;
					  QStringList stringList;
					  while(iterartists.hasNext())
					  {
						
						//QString artistName;
						QString artistName = iterartists.next().toString();
						writeLog("artist Name = "+ artistName);
						//artistName.append(map2["artist"].toString());
						stringList.append(artistName);
						
					  }//end while
					  //writeLog("artists= "+stringList[0]);
					  
					  artists.setNames(stringList);
					  event.setArtists(artists);
					  
					  //all Artist list we got 
					  //For Venue
					  SmfLocation Location;
					  QVariantMap map3 = map2["venue"].toMap();
					  writeLog("LocationId : "+map3["id"].toString());
					  Location.setId(map3["id"].toString());
					  //for city
					  QVariantMap map6 = map3["location"].toMap();
					  writeLog("Location city : "+map6["city"].toString());
					  Location.setCity(map6["city"].toString());
					  writeLog("Location city : "+map6["country"].toString());
					  Location.setCountry(map6["country"].toString());
					 // Location.geoPositionInfo()  dont know how to set 
					  event.setVenue(Location);
					  eventlist.append(event);
					}//end While*/
					aResult->setValue(eventlist);
					aRetType = SmfRequestComplete;
					error = SmfPluginErrNone;										
				 }
			 else if(SmfEventsGetVenues == aOperation)
				{
				writeLog("SmfEventsGetVenues = ");
				QList<SmfLocation> LocationList;
				SmfLocation Location;
			    Location.setId("111111");
				//for city
				Location.setCity("delhi");
				Location.setCountry("India");
				Location.setUrl(QUrl("http://wwwqqqqq/ee"));
				LocationList.append(Location);
				
				SmfLocation Location1;
				Location1.setId("22222");
				//for city
				Location1.setCity("bangalore");
				Location1.setCountry("India");
				LocationList.append(Location1);
				
				aResult->setValue(LocationList);
				aRetType = SmfRequestComplete;
				error = SmfPluginErrNone;
				 
				
				}
			
			 else if (SmfEventsPostEvents == aOperation)
				{
				writeLog("SmfEventsPostEvents = ");
				bool postEvent=true;	
				aResult->setValue(postEvent);
				aRetType = SmfRequestComplete;
				error = SmfPluginErrNone;
				
				}
			 else if (SmfEventsGetEventAttendees == aOperation)
				 {
			  
				        QList<SmfContact> contactList;
				        
			             writeLog("SmfMusicGetEventsAttendees = ");
			             bool ok;
			           	writeLog("Before Parser--");

			           	SmfPluginUtil util;
			           	QVariantMap result = util.parse(response, &ok).toMap();
			           	if (!ok) 
			           	{
			           	writeLog("An error occurred during json parsing");
			           	aRetType = SmfRequestError;
			           	return SmfPluginErrParsingFailed;
			           		
			           	}
			           							
			           	//QVariantMap map1 = result.toMap();
			        	writeLog("MAP1");
			            
			           
			           	QVariantMap map1 = result["attendees"].toMap();
			        	//QVariantMap map2 = map1["attendees"].toMap();
			           	writeLog("MAP2");
			           	//int page = map2["page"].toInt(&ok);
			           	QList<QVariant> list1 = map1["user"].toList();
			           	writeLog("list count"+QString::number(list1.count(),10));
			           				
			           	QListIterator<QVariant> i(list1);
			           	writeLog("iterate list"); //While loop not work if there is only one attendee
			            while(i.hasNext())
			           	{
			                writeLog("inside loop");
			           		SmfContact contact;
			           		QVariantMap map2 = i.next().toMap();
			           		writeLog("name= "+map2["name"].toString());
			           		writeLog("realname = "+map2["realname"].toString());
			           					
			           		//Contact Name
			           		QContactName contactname;
			           		QString username = map2["name"].toString();
			           		writeLog("name = "+username);
			           		contactname.setFirstName(username);
			           		contactname.setLastName(username);
			           		QVariant nameVar = QVariant::fromValue(contactname);
			           		contact.setValue("Name",nameVar);
			           		contactList.append(contact);
			           	}
			             aResult->setValue(contactList);										
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
	
	  m_serviceName = "Last.fm";
	  m_description = "Description";
	  m_serviceUrl =  QUrl(QString("http://ws.audioscrobbler.com"));
	 // m_serviceType << m_serviceName << m_description;// << (QString)(m_serviceUrl);
	  m_supportedLanguages << "English" ;
	  m_pluginId =    "lastfmmusiceventplugin.qtplugin";
	  m_authAppId =   "0xE1D8C7D8";
	  QSettings iSettings;
	  m_smfRegToken = iSettings.value("CMLastFmRegToken").toString();
	  m_validity = iSettings.value("LastFmExpiryTime").toDateTime();
	  //m_smfRegToken = "hsdfusdghf";  //yet to be recvd

	}


/*
 * Export Macro
 * plugin name : musiceventplugin
 * plugin class : MusicEventPlugin
 */
Q_EXPORT_PLUGIN2( musiceventplugin, MusicEventPlugin )
