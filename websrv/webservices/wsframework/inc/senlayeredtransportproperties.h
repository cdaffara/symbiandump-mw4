/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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








#ifndef SEN_LAYERED_TRANSPORT_PROPERTIES_H
#define SEN_LAYERED_TRANSPORT_PROPERTIES_H

#include "senlayeredxmlproperties.h"

class CSenLayeredTransportProperties : public CSenLayeredXmlProperties
    {
    public:
    public:
        IMPORT_C static CSenLayeredTransportProperties* NewL();
        IMPORT_C static CSenLayeredTransportProperties* NewLC();

        IMPORT_C static CSenLayeredTransportProperties* NewL(const TDesC8& aXmlUtf8, CSenXmlReader& aParser);
        IMPORT_C static CSenLayeredTransportProperties* NewLC(const TDesC8& aXmlUtf8, CSenXmlReader& aParser);

        IMPORT_C static CSenLayeredTransportProperties* NewL(const CSenElement& aElement);
        IMPORT_C static CSenLayeredTransportProperties* NewLC(const CSenElement& aElement);

        IMPORT_C static CSenLayeredTransportProperties* NewL(RStringPool& aStringPool);
        IMPORT_C static CSenLayeredTransportProperties* NewLC(RStringPool& aStringPool);

        IMPORT_C static CSenLayeredTransportProperties* NewL(const TDesC8& aXmlUtf8, CSenXmlReader& aParser, RStringPool& aStringPool);
        IMPORT_C static CSenLayeredTransportProperties* NewLC(const TDesC8& aXmlUtf8, CSenXmlReader& aParser, RStringPool& aStringPool);

        IMPORT_C static CSenLayeredTransportProperties* NewL(const CSenElement& aElement, RStringPool& aStringPool);
        IMPORT_C static CSenLayeredTransportProperties* NewLC(const CSenElement& aElement, RStringPool& aStringPool);

        // From MSenProperties 
        IMPORT_C virtual void SetReader(CSenXmlReader& aReader);
        IMPORT_C virtual TSenPropertiesClassType PropertiesClassType();
        IMPORT_C virtual void WriteToL(RWriteStream& aWriteStream);
        IMPORT_C virtual void ReadFromL(const TDesC8& aBuffer);
        IMPORT_C virtual HBufC8* AsUtf8L();
        IMPORT_C virtual HBufC8* AsUtf8LC();
        IMPORT_C virtual TInt SetPropertyL(const TDesC8& aName, const TDesC8& aValue);
        IMPORT_C virtual TInt PropertyL(const TDesC8& aName, TPtrC8& aValue);
        IMPORT_C virtual TInt SetIntPropertyL(const TDesC8& aName, const TInt aValue);
        IMPORT_C virtual TInt IntPropertyL(const TDesC8& aName, TInt& aValue);
        IMPORT_C virtual TInt SetBoolPropertyL(const TDesC8& aName, const TBool aValue);
        IMPORT_C virtual TInt BoolPropertyL(const TDesC8& aName, TBool& aValue);        
        IMPORT_C virtual TBool IsSafeToCast(TSenPropertiesClassType aClass);
        
        // From MSenLayeredProperties 
        IMPORT_C virtual void WriteToL(RWriteStream& aWriteStream, TSenPropertiesLayer aLayer);
        IMPORT_C virtual void ReadFromL(const TDesC8& aBuffer, TSenPropertiesLayer aLayer);
        IMPORT_C virtual HBufC8* AsUtf8L(TSenPropertiesLayer aLayer);
        IMPORT_C virtual HBufC8* AsUtf8LC(TSenPropertiesLayer aLayer);
        IMPORT_C virtual TInt ShrinkTo(TSenPropertiesLayer aLayer);
        IMPORT_C virtual void ExpandToL(TSenPropertiesLayer aLayer);
        IMPORT_C virtual TSenPropertiesLayer TopLayer();
        IMPORT_C virtual TInt SetPropertyL(const TDesC8& aName, const TDesC8& aValue, TSenPropertiesLayer aLayer);
        IMPORT_C virtual TInt PropertyL(const TDesC8& aName, TPtrC8& aValue, TSenPropertiesLayer aLayer);
        IMPORT_C virtual TInt PropertyL(const TDesC8& aName, MSenProperty*& aValue, TSenPropertiesLayer aLayer);
        IMPORT_C virtual TInt PropertyL(const TDesC8& aName, MSenProperty*& aValue);                               
        IMPORT_C virtual TInt PropertiesByTypeL(const TDesC8& aTypeName, RPointerArray<MSenProperty>& aArray);                                        
        IMPORT_C virtual TInt SetIntPropertyL(const TDesC8& aName, const TInt aValue, TSenPropertiesLayer aLayer);
        IMPORT_C virtual TInt IntPropertyL(const TDesC8& aName, TInt& aValue, TSenPropertiesLayer aLayer);
        IMPORT_C virtual MSenProperties* CloneL() const;
        IMPORT_C virtual MSenProperties* Clone(TInt& aOkOrError) const;
        IMPORT_C virtual TInt SetBoolPropertyL(const TDesC8& aName, const TBool aValue, TSenPropertiesLayer aLayer);
        IMPORT_C virtual TInt BoolPropertyL(const TDesC8& aName, TBool& aValue, TSenPropertiesLayer aLayer);
        IMPORT_C virtual TInt SetPropertyL(const TDesC8& aName, const TDesC8& aValue, const TDesC8& aType);
        IMPORT_C virtual TInt PropertyL(const TDesC8& aName, TPtrC8& aValue, TPtrC8& aType);  
        
        //common transport properties
         // New Functions       
        /**
        * Gets the IAP ID.
        * @param aCurrentIapId  A TUint32 reference to be filled in with the
        *                       value of the IAP ID.
        * @return               KErrNone if no error, or some of the system
        *                       wide error codes.
        */
        IMPORT_C virtual TInt IapIdL(TUint32& aCurrentIapId);

        /**
        * Gets the Proxy Port.
        * @param aProxyPort  A TInt reference to be filled in with the
        *                    value of the Proxy Port.
        * @return            KErrNone if no error, or some of the system
        *                    wide error codes.
        */
        IMPORT_C virtual TInt ProxyPortL(TInt& aProxyPort);

        /**
        * Gets the Proxy Host.
        * @param aProxyHost  A TPtrC8 reference to be filled in with the
        *                    value of the Proxy Host.
        * @return            KErrNone if no error, or some of the system
        *                    wide error codes.
        */
        IMPORT_C virtual TInt ProxyHostL(TPtrC8& aProxyHost);

        /**
        * Gets the Proxy Usage.
        * @param aProxyUsage A TBool reference to be filled in with the
        *                    value of the Proxy Usage.
        * @return            KErrNone if no error, or some of the system
        *                    wide error codes.
        */
        IMPORT_C virtual TInt ProxyUsageL(TBool& aProxyUsage);

        /**
        * Gets the IAP Dialog.
        * @param aIAPDialog A TBool reference to be filled in with the
        *                   value of the IAPDialog.
        * @return           KErrNone if no error, or some of the system
        *                   wide error codes.
        */
        IMPORT_C virtual TInt IAPDialogL(TBool& aIAPDialog);

        /**
        * Gets the UserAgent.
        * @param aUserAgent  A TPtrC8 reference to be filled in with the
        *                    value of the UserAgent.
        * @return            KErrNone if no error, or some of the system
        *                    wide error codes.
        */
        IMPORT_C virtual TInt UserAgentL(TPtrC8& aUserAgent);
        
        /**
        * Gets the Device ID.
        * @param aDeviceID   A TPtrC8 reference to be filled in with the
        *                    value of the DeviceId.
        * @return            KErrNone if no error, or some of the system
        *                    wide error codes.
        */
        IMPORT_C virtual TInt DeviceIDL(TPtrC8& aDeviceID);
        
        /**
        * Gets the Action of message
        * @param aAction A TPtrC8 reference to be filled in with the
        *                    value of the Action.
        * @return            KErrNone if no error, or some of the system
        *                    wide error codes.
        */
        IMPORT_C virtual TInt SoapActionL(TPtrC8& aAction);
        
        /**
        * Gets download folder for incoming BLOB (binary large objects)
        * @param aDownloadFolder - A TPtrC8 reference to be filled in with the
        *                    value of the shared, public folder for downloaded 
        *                    content
        * @return            KErrNone if no error, or some of the system
        *                    wide error codes.
        */
        IMPORT_C virtual TInt DownloadFolderL(TPtrC8& aDownloadFolder);

        /**
        * Gets filename of file attachment
        * @param aCid - cid for filename
        * @param aFileName - filename of file attachment with current cid
        * @return            KErrNone if no error, or some of the system
        *                    wide error codes.
        */
        IMPORT_C virtual TInt FileAttachmentL(const TDesC8& aCid, HBufC8*& aFileName);
        
        /**
        * Gets namespace of Microsoft schema
        * @param aMwsNamespace - namespace
        * @return            KErrNone if no error, or some of the system
        *                    wide error codes.
        */
        IMPORT_C virtual TInt MwsNamespaceL(TPtrC8& aMwsNamespace);
        
        IMPORT_C virtual TInt HeartbeatL(TInt& aHeartbeat);

        IMPORT_C virtual TInt MaxTimeToLiveL(TInt& aMax);

        IMPORT_C virtual TInt MinTimeToLiveL(TInt& aMin);
		IMPORT_C virtual TInt SetMaxRetryTTLL(TInt aMaxRetryTTL);
		IMPORT_C virtual TInt MaxRetryTTLL(TInt& aMaxRetryTTL);
		IMPORT_C virtual TInt SetRetryDeltaTimeoutL(TInt aTimeout);
		IMPORT_C virtual TInt RetryDeltaTimeoutL(TInt& aTimeout);
		IMPORT_C virtual TInt SetMinRetryTTLL(TInt aMinRetryTTL);
		IMPORT_C virtual TInt MinRetryTTLL(TInt& aMinRetryTTL);

        /**
        * Destructor.
        */
        IMPORT_C virtual ~CSenLayeredTransportProperties();

    protected: // base class functions

        IMPORT_C virtual void BaseConstructL(const TDesC8& aLocalname, 
                                    const TDesC8& aXml,
                                    CSenXmlReader* aParser = NULL,
                                    RStringPool* aStringPool = NULL);

        IMPORT_C virtual void BaseConstructL(const TDesC8& aNamespace, 
                                    const TDesC8& aLocalname, 
                                    const TDesC8& aXml,
                                    CSenXmlReader* aParser = NULL,
                                    RStringPool* aStringPool = NULL);

        IMPORT_C virtual void BaseConstructL(const TDesC8& aNamespace, 
                                    const TDesC8& aLocalname, 
                                    const TDesC8& aQualifiedName, 
                                    const TDesC8& aXml,
                                    CSenXmlReader* aParser = NULL,
                                    RStringPool* aStringPool = NULL);

        IMPORT_C virtual void BaseConstructL(const CSenElement& aElement, RStringPool* aStringPool = NULL);

	public:
	/**
        * Gets the SNAP ID.
        * @param aCurrentSnapId  A TUint32 reference to be filled in with the
        *                       value of the SNAP ID.
        * @return               KErrNone if no error, or some of the system
        *                       wide error codes.
        */
        IMPORT_C virtual TInt SnapIdL(TUint32& aCurrentSnapId);		

	/**
        * Gets the SNAP Dialog.
        * @param aSNAPDialog A TBool reference to be filled in with the
        *                   value of the IAPDialog.
        * @return           KErrNone if no error, or some of the system
        *                   wide error codes.
        */
        IMPORT_C virtual TInt SnapDialogL(TBool& aSNAPDialog);		
    protected:
        /**
        * Constructor.
        */
        IMPORT_C CSenLayeredTransportProperties();
    };

#endif // SEN_LAYERED_TRANSPORT_PROPERTIES_H

// End of File

