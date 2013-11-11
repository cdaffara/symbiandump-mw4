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
* Description: Header declaration
*
*/








#ifndef SEN_LAYERED_HTTP_TRANSPORT_PROPERTIES_H
#define SEN_LAYERED_HTTP_TRANSPORT_PROPERTIES_H

#include "senlayeredtransportproperties.h"

class CSenLayeredHttpTransportProperties : public CSenLayeredTransportProperties
    {
    public:
        enum TSenHttpMethod
        	{
        	ESenHttpPut,
        	ESenHttpDelete,
        	ESenHttpGet,
            ESenHttpPost        	
            };
            
        enum TSenHttpVersion
        	{
        	ESenHttp10,
        	ESenHttp11
            };
            
        IMPORT_C static CSenLayeredHttpTransportProperties* NewL();
        IMPORT_C static CSenLayeredHttpTransportProperties* NewLC();

        IMPORT_C static CSenLayeredHttpTransportProperties* NewL(const TDesC8& aXmlUtf8, CSenXmlReader& aParser);
        IMPORT_C static CSenLayeredHttpTransportProperties* NewLC(const TDesC8& aXmlUtf8, CSenXmlReader& aParser);

        IMPORT_C static CSenLayeredHttpTransportProperties* NewL(const CSenElement& aElement);
        IMPORT_C static CSenLayeredHttpTransportProperties* NewLC(const CSenElement& aElement);
                
        IMPORT_C static CSenLayeredHttpTransportProperties* NewL(RStringPool& aStringPool);
        IMPORT_C static CSenLayeredHttpTransportProperties* NewLC(RStringPool& aStringPool);

        IMPORT_C static CSenLayeredHttpTransportProperties* NewL(const TDesC8& aXmlUtf8, CSenXmlReader& aParser, RStringPool& aStringPool);
        IMPORT_C static CSenLayeredHttpTransportProperties* NewLC(const TDesC8& aXmlUtf8, CSenXmlReader& aParser, RStringPool& aStringPool);

        IMPORT_C static CSenLayeredHttpTransportProperties* NewL(const CSenElement& aElement, RStringPool& aStringPool);
        IMPORT_C static CSenLayeredHttpTransportProperties* NewLC(const CSenElement& aElement, RStringPool& aStringPool);
                
        // From MSenProperties
        virtual void SetReader(CSenXmlReader& aReader);
        virtual TSenPropertiesClassType PropertiesClassType();
        virtual void WriteToL(RWriteStream& aWriteStream);
        virtual void ReadFromL(const TDesC8& aBuffer);
        virtual HBufC8* AsUtf8L();
        virtual HBufC8* AsUtf8LC();
        virtual TInt SetPropertyL(const TDesC8& aName, const TDesC8& aValue);
        virtual TInt PropertyL(const TDesC8& aName, TPtrC8& aValue);
        virtual TInt SetIntPropertyL(const TDesC8& aName, const TInt aValue);
        virtual TInt IntPropertyL(const TDesC8& aName, TInt& aValue);
        virtual TInt SetBoolPropertyL(const TDesC8& aName, const TBool aValue);
        virtual TInt BoolPropertyL(const TDesC8& aName, TBool& aValue);
        virtual TBool IsSafeToCast(TSenPropertiesClassType aClass);

        // From MSenLayeredProperties 
        virtual void WriteToL(RWriteStream& aWriteStream, TSenPropertiesLayer aLayer);
        virtual void ReadFromL(const TDesC8& aBuffer, TSenPropertiesLayer aLayer);
        virtual HBufC8* AsUtf8L(TSenPropertiesLayer aLayer);
        virtual HBufC8* AsUtf8LC(TSenPropertiesLayer aLayer);
        virtual TInt ShrinkTo(TSenPropertiesLayer aLayer);
        virtual void ExpandToL(TSenPropertiesLayer aLayer);
        virtual TSenPropertiesLayer TopLayer();
        virtual TInt SetPropertyL(const TDesC8& aName, const TDesC8& aValue, TSenPropertiesLayer aLayer);
        virtual TInt PropertyL(const TDesC8& aName, TPtrC8& aValue, TSenPropertiesLayer aLayer);
        virtual TInt PropertyL(const TDesC8& aName, MSenProperty*& aValue, TSenPropertiesLayer aLayer);
        virtual TInt PropertyL(const TDesC8& aName, MSenProperty*& aValue);                               
        virtual TInt PropertiesByTypeL(const TDesC8& aTypeName, RPointerArray<MSenProperty>& aArray);
        virtual TInt SetIntPropertyL(const TDesC8& aName, const TInt aValue, TSenPropertiesLayer aLayer);
        virtual TInt IntPropertyL(const TDesC8& aName, TInt& aValue, TSenPropertiesLayer aLayer);
        virtual MSenProperties* CloneL() const;
        virtual TInt SetBoolPropertyL(const TDesC8& aName, const TBool aValue, TSenPropertiesLayer aLayer);
        virtual TInt BoolPropertyL(const TDesC8& aName, TBool& aValue, TSenPropertiesLayer aLayer);     
        virtual TInt SetPropertyL(const TDesC8& aName, const TDesC8& aValue, const TDesC8& aType);
        virtual TInt PropertyL(const TDesC8& aName, TPtrC8& aValue, TPtrC8& aType);   
        
  
        
        // New Functions       
        /**
        * Gets the IAP ID.
        * @param aCurrentIapId  A TUint32 reference to be filled in with the
        *                       value of the IAP ID.
        * @return               KErrNone if no error, or some of the system
        *                       wide error codes.
        */
        virtual TInt IapIdL(TUint32& aCurrentIapId);

        /**
        * Gets the Proxy Port.
        * @param aProxyPort  A TInt reference to be filled in with the
        *                    value of the Proxy Port.
        * @return            KErrNone if no error, or some of the system
        *                    wide error codes.
        */
        virtual TInt ProxyPortL(TInt& aProxyPort);

        /**
        * Gets the Proxy Host.
        * @param aProxyHost  A TPtrC8 reference to be filled in with the
        *                    value of the Proxy Host.
        * @return            KErrNone if no error, or some of the system
        *                    wide error codes.
        */
        virtual TInt ProxyHostL(TPtrC8& aProxyHost);

        /**
        * Gets the Proxy Usage.
        * @param aProxyUsage A TBool reference to be filled in with the
        *                    value of the Proxy Usage.
        * @return            KErrNone if no error, or some of the system
        *                    wide error codes.
        */
        virtual TInt ProxyUsageL(TBool& aProxyUsage);

        /**
        * Gets the IAP Dialog.
        * @param aIAPDialog A TBool reference to be filled in with the
        *                   value of the IAPDialog.
        * @return           KErrNone if no error, or some of the system
        *                   wide error codes.
        */
        virtual TInt IAPDialogL(TBool& aIAPDialog);

        /**
        * Gets the Content Type.
        * @param aContentType A TPtrC8 reference to be filled in with the
        *                     value of the Content Type.
        * @return             KErrNone if no error, or some of the system
        *                     wide error codes.
        */
        virtual TInt ContentTypeL(TPtrC8& aContentType);
        
        /**
        * Gets the SoapAction.
        * @param aSoapAction A TPtrC8 reference to be filled in with the
        *                    value of the SoapAction.
        * @return            KErrNone if no error, or some of the system
        *                    wide error codes.
        */
        virtual TInt SoapActionL(TPtrC8& aSoapAction);
        
        /**
        * Gets the UserAgent.
        * @param aUserAgent  A TPtrC8 reference to be filled in with the
        *                    value of the UserAgent.
        * @return            KErrNone if no error, or some of the system
        *                    wide error codes.
        */
        virtual TInt UserAgentL(TPtrC8& aUserAgent);
        
        /**
        * Gets the Accept.
        * @param aAccept     A MSenProperty reference to be filled in with the
        *                    value of the Accept.
        * @return            KErrNone if no error, or some of the system
        *                    wide error codes.
        */
        virtual TInt AcceptL(MSenProperty*& aAccept);
        
        /**
        * Gets the Http method.
        * @param aHttpMethod A TSenHttpMethod reference to be filled in with
        *                    the value of the Http method.
        * @return            KErrNone if no error, or some of the system
        *                    wide error codes.
        */
        virtual TInt HttpMethodL(TSenHttpMethod& aHttpMethod);
        
        /**
        * Gets the Http version.
        * @param aHttpVersion A TSenHttpVersion reference to be filled in with
        *                     the value of the Http version.
        * @return             KErrNone if no error, or some of the system
        *                     wide error codes.
        */
        virtual TInt HttpVersionL(TSenHttpVersion& aHttpVersion);
        
        /**
        * Gets the Http version.
        * @param aHttpVersion A TSenHttpVersion reference to be filled in with
        *                     the value of the Http version.
        * @return             KErrNone if no error, or some of the system
        *                     wide error codes.
        */
        virtual TInt HttpHeadersL(RPointerArray<MSenProperty>& aArray);
        /**
        * Gets the Device ID.
        * @param aDeviceID   A TPtrC8 reference to be filled in with the
        *                    value of the DeviceId.
        * @return            KErrNone if no error, or some of the system
        *                    wide error codes.
        */
        virtual TInt DeviceIDL(TPtrC8& aDeviceID);

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
        * Gets filename of file attachment
        * @param aCid - cid for filename
        * @param aFileName - filename of file attachment with current cid
        * @return            KErrNone if no error, or some of the system
        *                    wide error codes.
        */
        virtual TInt FileAttachmentL(const TDesC8& aCid, HBufC8*& aFileName);

        /**
        * Gets namespace of Microsoft schema
        * @param aMwsNamespace - namespace
        * @return            KErrNone if no error, or some of the system
        *                    wide error codes.
        */
        virtual TInt MwsNamespaceL(TPtrC8& aMwsNamespace);

        /**
        * Destructor.
        */
        virtual ~CSenLayeredHttpTransportProperties();

    protected: // base class functions


        virtual void BaseConstructL(const TDesC8& aLocalname, 
                                    const TDesC8& aXml,
                                    CSenXmlReader* aParser = NULL,
                                    RStringPool* aStringPool = NULL);


        virtual void BaseConstructL(const TDesC8& aNamespace, 
                                    const TDesC8& aLocalname, 
                                    const TDesC8& aXml,
                                    CSenXmlReader* aParser = NULL,
                                    RStringPool* aStringPool = NULL);


        virtual void BaseConstructL(const TDesC8& aNamespace, 
                                    const TDesC8& aLocalname, 
                                    const TDesC8& aQualifiedName, 
                                    const TDesC8& aXml,
                                    CSenXmlReader* aParser = NULL,
                                    RStringPool* aStringPool = NULL);

        virtual void BaseConstructL(const CSenElement& aElement,
                                    RStringPool* aStringPool = NULL);

	public:
        /**
        * Gets the SNAP ID.
        * @param aCurrentSnapId  A TUint32 reference to be filled in with the
        *                       value of the SNAP ID.
        * @return               KErrNone if no error, or some of the system
        *                       wide error codes.
        */
		virtual  TInt SnapIdL(TUint32& aCurrentSnapId) ;

		/**
        * Gets the SNAP Dialog.
        * @param aSNAPDialog A TBool reference to be filled in with the
        *                   value of the IAPDialog.
        * @return           KErrNone if no error, or some of the system
        *                   wide error codes.
        */
        virtual TInt SnapDialogL(TBool& aSnapDialog);		
    protected:
        /**
        * Constructor.
        */
        CSenLayeredHttpTransportProperties();
    };

#endif // SEN_LAYERED_HTTP_TRANSPORT_PROPERTIES_H

// End of File

