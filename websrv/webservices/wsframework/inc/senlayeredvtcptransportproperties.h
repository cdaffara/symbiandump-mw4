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








#ifndef SEN_LAYERED_VTCP_TRANSPORT_PROPERTIES_H
#define SEN_LAYERED_VTCP_TRANSPORT_PROPERTIES_H

#include "senlayeredtransportproperties.h"

class CSenLayeredVtcpTransportProperties : public CSenLayeredTransportProperties
    {
    public:
        IMPORT_C static CSenLayeredVtcpTransportProperties* NewL();
        IMPORT_C static CSenLayeredVtcpTransportProperties* NewLC();
        IMPORT_C static CSenLayeredVtcpTransportProperties* NewL(const CSenElement& aElement);
        IMPORT_C static CSenLayeredVtcpTransportProperties* NewLC(const CSenElement& aElement);        

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
        * Gets the UserAgent.
        * @param aUserAgent  A TPtrC8 reference to be filled in with the
        *                    value of the UserAgent.
        * @return            KErrNone if no error, or some of the system
        *                    wide error codes.
        */
        virtual TInt UserAgentL(TPtrC8& aUserAgent);
        
        /**
        * Gets the Device ID.
        * @param aDeviceID   A TPtrC8 reference to be filled in with the
        *                    value of the DeviceId.
        * @return            KErrNone if no error, or some of the system
        *                    wide error codes.
        */
        virtual TInt DeviceIDL(TPtrC8& aDeviceID);

        /**
        * Gets the Action of message
        * @param aAction A TPtrC8 reference to be filled in with the
        *                    value of the Action.
        * @return            KErrNone if no error, or some of the system
        *                    wide error codes.
        */
        virtual TInt SoapActionL(TPtrC8& aAction);

        /**
        * Gets the Device LCID.
        * @param aDeviceLCId   A TPtrC8 reference to be filled in with the
        *                    value of the DeviceLCID.
        * @return            KErrNone if no error, or some of the system
        *                    wide error codes.
        */
        virtual TInt DeviceLCIDL(TPtrC8& aDeviceLCID);
        
        /**
        * Gets the connection mode (Bound/Unbound)
        * @param aConnectionBound A Bool reference to be filled in with the
        *                    value (true if connection is bound).
        * @return            KErrNone if no error, or some of the system
        *                    wide error codes.
        */
        virtual TInt ConnectionBoundL(TBool& aConnectionBound);
        
        /**
        * Gets the connection time out (in minutes)
        * @param aConnectionBound A TInt reference to be filled in with the
        *                    value of time out for virtual connection
        * @return            KErrNone if no error, or some of the system
        *                    wide error codes.
        */
        virtual TInt ConnectionTimeOutL(TInt& aConnectionTimeOut);
        
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
        virtual ~CSenLayeredVtcpTransportProperties();

		/**
        * Gets the SNAP ID.
        * @param aCurrentSnapId  A TUint32 reference to be filled in with the
        *                       value of the SNAP ID.
        * @return               KErrNone if no error, or some of the system
        *                       wide error codes.
        */
        virtual TInt SnapIdL(TUint32& aCurrentSnapId);	

    protected:
        /**
        * Constructor.
        */
        CSenLayeredVtcpTransportProperties();
    };

#endif // SEN_LAYERED_VTCP_TRANSPORT_PROPERTIES_H

// End of File

