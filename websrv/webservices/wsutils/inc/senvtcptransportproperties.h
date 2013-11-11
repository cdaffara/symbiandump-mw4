/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:    Header declaration
*
*/












#ifndef SEN_VTCP_TRANSPORT_PROPERTIES_H
#define SEN_VTCP_TRANSPORT_PROPERTIES_H

// INCLUDE
#include <SenTransportProperties.h>


// CONSTANTS
_LIT8(KDeviceLCIDLocalName,              "DeviceLCID");
_LIT8(KConnectionBoundLocalName,         "ConnectionBound");
_LIT8(KConnectionTimeOutLocalName,       "ConnectionTimeOut");

// CLASS DECLARATION
class CSenVtcpTransportProperties : public CSenTransportProperties
    {
    public:
                
        /**
        * Basic constructor.
        * @return a pointer to new CSenVtcpTransportProperties class instance.
        */
        IMPORT_C static CSenVtcpTransportProperties* NewL();
        /**
        * Basic constructor.
        * @return a pointer to new CSenVtcpTransportProperties class instance.
        */
        IMPORT_C static CSenVtcpTransportProperties* NewLC();

        /**
        * Basic constructor.
		* @param aXmlUtf8 
    	* @param aParser It is a XML reader        
        * @return a pointer to new CSenVtcpTransportProperties class instance.
        */
        IMPORT_C static CSenVtcpTransportProperties* NewL(const TDesC8& aXmlUtf8,
                                                          CSenXmlReader& aParser);

        /**
        * Basic constructor.
		* @param aXmlUtf8 
    	* @param aParser It is a XML reader        
        * @return a pointer to new CSenVtcpTransportProperties class instance.
        */
        IMPORT_C static CSenVtcpTransportProperties* NewLC(const TDesC8& aXmlUtf8,
                                                           CSenXmlReader& aParser);

        /**
        * Basic constructor.
		* @param aElement
        * @return a pointer to new CSenVtcpTransportProperties class instance.
        */
        IMPORT_C static CSenVtcpTransportProperties* NewL(const CSenElement& aElement);
        /**
        * Basic constructor.
		* @param aElement
        * @return a pointer to new CSenVtcpTransportProperties class instance.
        */
        IMPORT_C static CSenVtcpTransportProperties* NewLC(const CSenElement& aElement);

        // From MSenProperties
        virtual void SetReader(CSenXmlReader& aReader);
        virtual TSenPropertiesClassType PropertiesClassType();
        virtual void WriteToL(RWriteStream& aWriteStream);
        virtual void ReadFromL(const TDesC8& aBuffer);
        virtual HBufC8* AsUtf8L();
        virtual HBufC8* AsUtf8LC();
        virtual TBool IsSafeToCast(TSenPropertiesClassType aClass);
        virtual MSenProperties* CloneL() const;
        
        virtual TInt SetPropertyL(const TDesC8& aName, const TDesC8& aValue);
        virtual TInt PropertyL(const TDesC8& aName, TPtrC8& aValue);        
        virtual TInt SetPropertyL(const TDesC8& aName, const TDesC8& aValue, const TDesC8& aType);
        virtual TInt PropertyL(const TDesC8& aName, TPtrC8& aValue, TPtrC8& aType);
        virtual TInt SetIntPropertyL(const TDesC8& aName, const TInt aValue);
        virtual TInt IntPropertyL(const TDesC8& aName, TInt& aValue);
        virtual TInt SetBoolPropertyL(const TDesC8& aName, const TBool aValue);        
        virtual TInt BoolPropertyL(const TDesC8& aName, TBool& aValue);

       /**
        * Gets the IAP ID.
        * @param aCurrentIapId  A TUint32 reference to be filled in with the
        *                       value of the IAP ID.
        * @return               KErrNone if no error, or some of the system
        *                       wide error codes.
        */
        virtual TInt IapIdL(TUint32& aCurrentIapId);

        /**
        * Sets the IAP ID.
        * @param aIapId is the new IAP ID.
        */
        virtual void SetIapIdL(TUint32 aIapId);

        /**
        * Gets the Proxy Port.
        * @param aProxyPort  A TInt reference to be filled in with the
        *                    value of the Proxy Port.
        * @return            KErrNone if no error, or some of the system
        *                    wide error codes.
        */
        virtual TInt ProxyPortL(TInt& aProxyPort);

        /**
        * Sets the Proxy Port.
        * @param aProxyPort is the new Proxy Port.
        */
        virtual void SetProxyPortL(TInt aProxyPort);

        /**
        * Gets the Proxy Host.
        * @param aProxyHost  A TPtrC8 reference to be filled in with the
        *                    value of the Proxy Host.
        * @return            KErrNone if no error, or some of the system
        *                    wide error codes.
        */
        virtual TInt ProxyHostL(TPtrC8& aProxyHost);

        /**
        * Sets the Proxy Host.
        * @param aProxyHost is the new Proxy Host.
        */
        virtual void SetProxyHostL(const TDesC8& aProxyHost);
        
        /**
        * Gets the Proxy Usage flag.
        * @param aProxyUsage A TBool reference to be filled in with the
        *                    value of the Proxy Usage.
        * @return            KErrNone if no error, or some of the system
        *                    wide error codes.
        */
        virtual TInt ProxyUsageL(TBool& aProxyUsage);

        /**
        * Sets the Proxy Usage flag.
        * @param aProxyUsage is the new value for Proxy Usage.
        */
        virtual void SetProxyUsageL(TBool aProxyUsage);

        /**
        * Gets the information if SecureDialog is shown or not.
        * @param aProxyUsage A TBool reference to be filled in with the
        *                    value of the SecureDialog flag.
        * @return            KErrNone if no error, or some of the system
        *                    wide error codes.
        */
        virtual TInt SecureDialogL(TBool& aSecureDialog);

        /**
        * Sets the flag which controls showing of SecureDialog.
        * @param aSecureDialog is the new value for SecureDialog flag.
        */
        virtual void SetSecureDialogL(TBool aSecureDialog);

        /**
        * Gets the UserAgent.
        * @param aUserAgent  A TPtrC8 reference to be filled in with the
        *                    value of the UserAgent.
        * @return            KErrNone if no error, or some of the system
        *                    wide error codes.
        */
        virtual TInt UserAgentL(TPtrC8& aUserAgent);
        
        /**
        * Sets the UserAgent.
        * @param aUserAgent is the new User Agent.
        */
        virtual void SetUserAgentL(const TDesC8& aUserAgent);


        /**
        * Gets the device ID
        * @param aDeviceID  A TPtrC8 reference to be filled in with the
        *                    value of the Device ID.
        * @return            KErrNone if no error, or some of the system
        *                    wide error codes.
        */
        virtual TInt DeviceIDL(TPtrC8& aDeviceID);

        /**
        * Sets the Device ID.
        * @param aDeviceID is the new Device ID.
        */
        virtual void SetDeviceIDL(const TDesC8& aDeviceID);

        /**
        * Gets the Action of message
        * @param aAction A TPtrC8 reference to be filled in with the
        *                    value of the Action.
        * @return            KErrNone if no error, or some of the system
        *                    wide error codes.
        */
        virtual TInt SoapActionL(TPtrC8& aAction);
        
        /**
        * Sets the Action.
        * @param aAction is the new Soap Action.
        */
        virtual void SetSoapActionL(const TDesC8& aAction);

        /**
        * Apply binding.
        * @param aSoapVersion is the version of Soap (1.1 or 1.2).
        */
        virtual void ApplyBindingL(TSOAPVersion aSoapVersion);

        /**
        * Gets the device LCID
        * @param aDeviceLCID  A TPtrC8 reference to be filled in with the
        *                    value of the Device LCID.
        * @return            KErrNone if no error, or some of the system
        *                    wide error codes.
        */
        virtual TInt DeviceLCIDL(TPtrC8& aDeviceLCID);
        
        /**
        * Sets the Device LCID.
        * @param aDeviceLCID is the new Device LCID.
        */
        virtual void SetDeviceLCIDL(const TDesC8& aDeviceLCID);
        
        /**
        * Gets connection bound mode
        * @param aConnectionBound  A boolean reference used to return the
        *                    connection value (ETrue if bounded).
        *                    With virtual TCP, default is EFalse 
        *                    (unbound == persistant connection).
        * @return            KErrNone if no error, or some of the system
        *                    wide error codes.
        */
        virtual TInt ConnectionBoundL(TBool& aConnectionBound);
        
        /**
        * Sets the connection bound mode.
        * @param aConnectionBound is the connection bound mode
        */
        virtual void SetConnectionBoundL(const TBool& aConnectionBound);

        /**
        * Gets virtual connection timeout
        * @param aConnectionTimeOut  A TPtrC8 reference to be filled in with the
        *                    value of the connection time out
        * @return            KErrNone if no error, or some of the system
        *                    wide error codes.
        */
        virtual TInt ConnectionTimeOutL(TInt& aConnectionTimeOut);
        
        /**
        * Sets the connection time out
        * @param aConnectionTimeOut is the connection time out
        */
        virtual void SetConnectionTimeOutL(const TInt& aConnectionTimeOut);
       
        /**
        * Gets download folder for incoming BLOB (binary large objects)
        * @param aDownloadFolder - A TPtrC8 reference to be filled in with the
        *                    value of the shared, public folder for downloaded 
        *                    content
        * @return            KErrNone if no error, or some of the system
        *                    wide error codes.
        */
        virtual TInt DownloadFolderL(TPtrC8& aDownloadFolder);
        
        /**
        * Sets download folder for incoming BLOB (binary large objects)
        * @param aDownloadFolder - shared, public folder for downloaded content
        */
        virtual void SetDownloadFolderL(const TDesC8& aDownloadFolder);

        /**
        * Gets filename of file attachment
        * @param aCid - cid for filename
        * @param aFileName - filename of file attachment with current cid
        * @return            KErrNone if no error, or some of the system
        *                    wide error codes.
        */
        virtual TInt FileAttachmentL(const TDesC8& aCid, HBufC8*& aFileName);
        
        /**
        * Sets filename of file attachment
        * @param aCid - cid for filename
        * @param aFileName - filename of file attachment with current cid
        * @return            KErrNone if no error, or some of the system
        *                    wide error codes.
        */
        virtual TInt SetFileAttachmentL(const TDesC8& aCid, const TDesC8& aFileName);

        /**
        * Gets namespace of Microsoft schema
        * @param aMwsNamespace - namespace 
        * @return            KErrNone if no error, or some of the system
        *                    wide error codes.
        */
        virtual TInt MwsNamespaceL(TPtrC8& aMwsNamespace);
        
        /**
        * Sets namespace of Microsoft schema
        * @param aMwsNamespace - namespace
        * @return            KErrNone if no error, or some of the system
        *                    wide error codes.
        */
        virtual void SetMwsNamespaceL(const TDesC8& aMwsNamespace);

        /**
        * Gets message ID
        * @param aMessageId - is the id of the message
        * @return            KErrNone if no error, or some of the system
        *                    wide error codes.
        */
        virtual TInt MessageIdL(TPtrC8& aMessageId);

        /**
        * Sets message ID. Typically, this property is set per each message, instead of
        * applying the same message ID for whole consumer session (service connection)
        * @param aMessageID - is the ID of the message
        * @return            KErrNone if no error, or some of the system
        *                    wide error codes.
        */
        virtual void SetMessageIdL(const TDesC8& aMessageId);

        /**
        * Gets one-way message mode.
        * @param aOnewayMsgOnOff A TBool reference to be filled in with the
        *                    value of the one-way message mode.
        * @return            KErrNone if no error, or some of the system
        *                    wide error codes.
        */
        IMPORT_C virtual TInt OnewayMessageOnOffL(TBool& aOnewayMessageOnOff);

        /**
        * Sets the one-way message mode on/off. Typically, one-way
        * messages are rarely used by default: instead, a request-
        * response pair (consumer[request]<->[response]provider)
        * transaction takes place.
        * However, if one-way message mode is enabled, sending such
        * message will typically result immediate callback from
        * the transport (plug-in) itself. This means, that the
        * actual service response from the provider is not being
        * waited. 
        * Common use case to enable one-way message is when service
        * connection is used in transmitting *responses* to notification
        * request(s) that some remote consumer (from outside device) has 
        * sent earlier.
        * Such notification requests may thus be received via hostlet 
        * connection (hc), and by enabling one-way message, once can reply
        * via service connection, instead of calling hc->RespondL method.
        * Furthermore, application might wish to enable one-way message
        * mode to hostlet connection, if it is not interested of replying
        * to these two-way notifications via hostlet connection, but wishes
        * to use service connection instead.
        * @param aOnewayMessageOnOff is the new value of one-way message mode.
        */
        IMPORT_C virtual void SetOnewayMessageOnOffL(TBool aOnewayMessageOnOff);


       /**
        * Destructor.
        */
        virtual ~CSenVtcpTransportProperties();

    protected: // base class functions

        virtual void BaseConstructL(const TDesC8& aLocalname, 
                                    const TDesC8& aXml,
                                    CSenXmlReader* aParser = NULL);

        virtual void BaseConstructL(const CSenElement& aElement);


    public: // Extensions
      
      
      
        /**
        * Gets the (connection) heartbeat property value as int.
        * @param aDelta     A TInt reference to be filled in with the
        *                       value of the heartbeat (in seconds).
        *                       Positive value means that a heartbeat
        *                       messages are sent to backend, keeping
        *                       the socket connection open (longlived
        *                       connection). Keeping heartbeat may be
        *                       costy, but increases performance in
        *                       scenarios, where multiple network
        *                       transactions take place within short
        *                       period (time window).
        * @return             KErrNone if no error, or some of the system
        *                       wide error codes.
        *                     KErrNotFound if shortlive connection defined
        */
        virtual TInt HeartbeatL(TInt& aDelta);


        /**
        * Sets the (connection) heartbeat property value as int.
        * Note, that with some transports, like vTCP, the heartbeat
        * can also be used to control connection modes (longlive
        * vs. shortlive connection). 
        * Hertbeat value is called also as delta.
        * 
        * Together with delta You can also set parameters Min & Max TTL
        * Long and short connection is set in different way, below guider.
        *
        * To setup longlive connection:
        *   delta   : user defined
        *   min TTL : If not specified then the default value of 90 sec will be used
        *   max TTL : If not specified then the default value 1800 sec will be used
        *
        * To setup shortlive connection:
        *   delta   : -1
        *   min TTL : If not specified then the default value of 90 sec will be used
        *   max TTL : ignored
        *
        * So, providing a negative value
        * (-1) can be utilized to disable heartbeat and to switch
        * the connection mode to a short lived state. With vTCP
        * transport, if heartbeat of -1 is set, the socket will be
        * kept open only for effective timeout value (shortlived), 
        * 
        * @param aDelta    TInt reference to be filled in with 
        *                      the value of the heartbeat (in seconds)
        * @return              KErrNone if no error, or some of the system
        *                      wide error codes.
        */
        virtual TInt SetHeartbeatL(TInt aDelta);
        
        /**
        * Sets the (connection) Max TTL for longlive connection.
        *
        * @see SetHeartbeatL(TInt aDelta)
        *
        * @param aMaxTTL      TInt reference to be filled in with 
        *                      the value of the max TTL (in seconds)
        * @return              KErrNone if no error, or some of the system
        *                       wide error codes.
        */
        IMPORT_C virtual TInt SetMaxTimeToLiveL(TInt aMaxTTL);
        
        /**
        * Gets the (connection) max TTL property value as int.
        *
        * @see SetHeartbeatL(TInt aDelta)
        *
        * @param aMaxTTL       A TInt reference to be filled in with the
        *                       value of the max TTL (in seconds).
        *
        * @return           KErrNotFound if shortlive connection defined
        *                   KErrNone if no error, or some of the system
        *                       wide error codes.
        */
        IMPORT_C virtual TInt MaxTimeToLiveL(TInt& aMaxTTL);

        /**
        * Sets the (connection) Min TTL
        *
        * @see SetHeartbeatL(TInt aDelta)
        *
        * @param aMaxTTL      TInt reference to be filled in with 
        *                      the value of the min TTL (in seconds)
        * @return              KErrNone if no error, or some of the system
        *                       wide error codes.
        */
        IMPORT_C virtual TInt SetMinTimeToLiveL(TInt aMinTTL);

        /**
        * Gets the (connection) min TTL property value as int.
        *
        * @see SetHeartbeatL(TInt aDelta)
        *
        * @param aMinTTL     A TInt reference to be filled in with the
        *                       value of the min TTL (in seconds).
        *
        * @return           KErrNone if no error, or some of the system
        *                       wide error codes.
        */
        IMPORT_C virtual TInt MinTimeToLiveL(TInt& aMinTTL);

	public:
        /**
        * Sets the SNAP ID.
        * @param aSnapId is the new SNAP ID.
        */
		virtual void SetSnapIdL(TUint32 aSnapId);

       /**
        * Gets the SNAP ID.
        * @param aCurrentSnapId  A TUint32 reference to be filled in with the
        *                       value of the SNAP ID.
        * @return               KErrNone if no error, or some of the system
        *                       wide error codes.
        */
       	virtual TInt SnapIdL(TUint32& aCurrentSnapId);	
    /**
        * Sets the (LongLived Retry) Long Lived Retry property value as int.
        * Note, that with some transports, like vTCP, the RetryMaxTTL
        * can also be used to control only long lived connection retry modes 
        * RetryMaxTTL value is called also as delta.
        * Together with delta You can also set parameters RetryMinTTL & RetryMaxTTL
        * Long and short connection is set in different way, below guider.
        * @param aDelta    TInt reference to be filled in with 
        *                      the value of the RetryMaxTTL (in seconds)
        * @return              KErrNone if no error, or some of the system
        *                      wide error codes.
        */
        IMPORT_C virtual TInt SetMaxRetryTTLL(TInt aMaxRetryTTL);
        
	    /**
        * Gets the (Long Lived Retry)RetryMaxTTL property value as int.
        * @param aDelta     A TInt reference to be filled in with the
        *                       value of the RetryMaxTTL (in seconds).
        *                       Positive value means that a RetryMaxTTL
        *                       messages are sent to backend, keeping
        *                       the socket connection open (longlived
        *                       connection). Keeping RetryMaxTTL may be
        *                       costy, but increases performance in
        *                       scenarios, where multiple network
        *                       transactions take place within short
        *                       period (time window).
        * @return             KErrNone if no error, or some of the system
        *                       wide error codes.
        *                     KErrNotFound if shortlive connection defined
        */
        IMPORT_C virtual TInt MaxRetryTTLL(TInt& aMaxRetryTTL);
        
        
		/**
        * Sets the (LongLived Retry) RetryDelta
        *
        * @see SetMaxRetryTTLL(TInt aDelta)
        *
        * @param aMaxTTL      TInt reference to be filled in with 
        *                      the value of the min TTL (in seconds)
        * @return              KErrNone if no error, or some of the system
        *                       wide error codes.
        */
 		IMPORT_C virtual TInt SetRetryDeltaTimeoutL(TInt aTimeout);
 		
		/**
        * Gets the (LongLived Retry) RetryDelta
        *
        * @see SetMaxRetryTTLL(TInt aDelta)
        *
        * @param aMaxTTL      TInt reference to be filled in with 
        *                      the value of the min TTL (in seconds)
        * @return              KErrNone if no error, or some of the system
        *                       wide error codes.
        */
        IMPORT_C virtual TInt RetryDeltaTimeoutL(TInt& aTimeout);
        
		/**
        * Sets the (LongLived Retry) RetryMinTTL
        *
        * @see SetMaxRetryTTLL(TInt aDelta)
        *
        * @param aMaxTTL      TInt reference to be filled in with 
        *                      the value of the min TTL (in seconds)
        * @return              KErrNone if no error, or some of the system
        *                       wide error codes.
        */
        IMPORT_C virtual TInt SetMinRetryTTLL(TInt aMinRetryTTL);
        
		/**
        * Gets the (LongLived Retry) RetryMinTTL
        *
        * @see SetMaxRetryTTLL(TInt aDelta)
        *
        * @param aMaxTTL      TInt reference to be filled in with 
        *                      the value of the min TTL (in seconds)
        * @return              KErrNone if no error, or some of the system
        *                       wide error codes.
        */
        IMPORT_C virtual TInt MinRetryTTLL(TInt& aMinRetryTTL);

    protected:
        /**
        * Constructor.
        */
        CSenVtcpTransportProperties();
    };

#endif // SEN_VTCP_TRANSPORT_PROPERTIES_H
