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
* Description:         
*
*/











#include <s32strm.h> // RWriteStream
#include <SenXmlReader.h>
#include <SenXmlUtils.h>
#include <SenTransportProperties.h>
#include "senpropertiesfragment.h"
#include "SenBaseAttribute.h"
#include <xml/attribute.h> // From syslibs, needed for RAttributeArray

#include "senlogger.h"
//#include "senxmldebug.h"

namespace
    {
    _LIT8(KCid,"cid");
    }

EXPORT_C CSenTransportProperties* CSenTransportProperties::NewL()
    {
    CSenTransportProperties* pNew = NewLC();
    CleanupStack::Pop(); // pNew
    return pNew;
    }

EXPORT_C CSenTransportProperties* CSenTransportProperties::NewLC()
    {
    CSenTransportProperties* pNew = new (ELeave) CSenTransportProperties;
    CleanupStack::PushL(pNew);

    // omit parsing by passing zero-length descriptor
    pNew->BaseConstructL(KSenXmlPropertiesLocalname,
                         KNullDesC8,
                         NULL);
    return pNew;
    }

EXPORT_C CSenTransportProperties* CSenTransportProperties::NewL(const TDesC8& aXmlUtf8,
                                                               CSenXmlReader& aParser)
    {
    CSenTransportProperties* pNew = NewLC(aXmlUtf8, aParser);
    CleanupStack::Pop(); // pNew
    return pNew;
    }

EXPORT_C CSenTransportProperties* CSenTransportProperties::NewLC(const TDesC8& aXmlUtf8,
                                                                CSenXmlReader& aParser)
    {
    CSenTransportProperties* pNew = new (ELeave) CSenTransportProperties;
    CleanupStack::PushL(pNew);

    // parse the XML document  descriptor into DOM fragment
    pNew->BaseConstructL(KSenXmlPropertiesLocalname,
                         aXmlUtf8,
                         &aParser);
    return pNew;
    }

EXPORT_C CSenTransportProperties* CSenTransportProperties::NewL(const CSenElement& aElement)
    {
    CSenTransportProperties* pNew = NewLC(aElement);
    CleanupStack::Pop(); // pNew
    return pNew;
    }

EXPORT_C CSenTransportProperties* CSenTransportProperties::NewLC(const CSenElement& aElement)
    {
    CSenTransportProperties* pNew = new (ELeave) CSenTransportProperties;
    CleanupStack::PushL(pNew);

    // parse the XML document  descriptor into DOM fragment
    pNew->BaseConstructL(aElement);
    return pNew;
    }
    
EXPORT_C void CSenTransportProperties::SetReader(CSenXmlReader& aReader)
    {
    CSenXmlProperties::SetReader(aReader);
    }      

EXPORT_C MSenProperties::TSenPropertiesClassType CSenTransportProperties::PropertiesClassType() 
    {
    return ESenTransportProperties;
    }

EXPORT_C void CSenTransportProperties::WriteToL(RWriteStream& aWriteStream)
    {
    CSenXmlProperties::WriteToL(aWriteStream);
    }
    
EXPORT_C void CSenTransportProperties::ReadFromL(const TDesC8& aBuffer)
    {
    CSenXmlProperties::ReadFromL(aBuffer);
    }    

EXPORT_C HBufC8* CSenTransportProperties::AsUtf8L()
    {
    return CSenXmlProperties::AsUtf8L();
    }

EXPORT_C HBufC8* CSenTransportProperties::AsUtf8LC()
    {
    return CSenXmlProperties::AsUtf8LC();
    }
    
EXPORT_C TBool CSenTransportProperties::IsSafeToCast(TSenPropertiesClassType aClass)
    {
    if ( aClass == MSenProperties::ESenTransportProperties )
        {
        return ETrue;
        }
    else
        {
        return CSenXmlProperties::IsSafeToCast(aClass);
        }
    }      
    
EXPORT_C TInt CSenTransportProperties::SetPropertyL(const TDesC8& aName,
                                                       const TDesC8& aValue)
    {
    return CSenXmlProperties::SetPropertyL(aName, aValue);
    }
    
EXPORT_C TInt CSenTransportProperties::PropertyL(const TDesC8& aName,
                                                   TPtrC8& aValue)
    {
    return CSenXmlProperties::PropertyL(aName, aValue);
    }
    
EXPORT_C TInt CSenTransportProperties::SetIntPropertyL(const TDesC8& aName,
                                                       const TInt aValue)
    {
    return CSenXmlProperties::SetIntPropertyL(aName, aValue);
    }

EXPORT_C TInt CSenTransportProperties::IntPropertyL(const TDesC8& aName,
                                                    TInt& aValue)
    {
    return CSenXmlProperties::IntPropertyL(aName, aValue);
    }

EXPORT_C TInt CSenTransportProperties::SetBoolPropertyL(const TDesC8& aName,
                                                        const TBool aValue)
    {
    return CSenXmlProperties::SetBoolPropertyL(aName, aValue);
    }

EXPORT_C TInt CSenTransportProperties::BoolPropertyL(const TDesC8& aName,
                                                     TBool& aValue)
    {
    return CSenXmlProperties::BoolPropertyL(aName, aValue);
    }
EXPORT_C TInt CSenTransportProperties::SetOmittedL(const TDesC8& aName, TBool aValue)
    {
    return CSenXmlProperties::SetOmittedL(aName, aValue);
    }

EXPORT_C TInt CSenTransportProperties::RemovePropertyL(const TDesC8& aName)
    {
    return CSenXmlProperties::RemovePropertyL(aName);
    }


EXPORT_C CSenTransportProperties::~CSenTransportProperties()
    {
    }

EXPORT_C void CSenTransportProperties::BaseConstructL(const TDesC8& aLocalname, 
                                                 const TDesC8& aXml,
                                                 CSenXmlReader* aParser)
    {
    CSenXmlProperties::BaseConstructL(aLocalname, aXml, aParser);
    }

EXPORT_C void CSenTransportProperties::BaseConstructL(const TDesC8& aNamespace, 
                                                 const TDesC8& aLocalname, 
                                                 const TDesC8& aXml,
                                                 CSenXmlReader* aParser)
    {
    CSenXmlProperties::BaseConstructL(aNamespace, aLocalname, aXml, aParser);
    }

EXPORT_C void CSenTransportProperties::BaseConstructL(const TDesC8& aNamespace, 
                                                 const TDesC8& aLocalname, 
                                                 const TDesC8& aQualifiedName, 
                                                 const TDesC8& aXml,
                                                 CSenXmlReader* aParser)
    {
    CSenXmlProperties::BaseConstructL(aNamespace, aLocalname, aQualifiedName, aXml, aParser);
    }

EXPORT_C void CSenTransportProperties::BaseConstructL(const CSenElement& aElement)
    {
    CSenXmlProperties::BaseConstructL(aElement);
    }

EXPORT_C CSenTransportProperties::CSenTransportProperties()
: iFileAttachmentNum(0)
    {
    }
    
EXPORT_C TInt CSenTransportProperties::SetPropertyL(const TDesC8& aName,
                                                    const TDesC8& aValue,
                                                    const TDesC8& aType)
    {
    return CSenXmlProperties::SetPropertyL(aName, aValue, aType);
    }

EXPORT_C TInt CSenTransportProperties::PropertyL(const TDesC8& aName,
                                                    TPtrC8& aValue,
                                                    TPtrC8& aType)
    {
    return CSenXmlProperties::PropertyL(aName, aValue, aType);
    }
//common properties
EXPORT_C TInt CSenTransportProperties::IapIdL(TUint32& aCurrentIapId)
    {
    TPtrC8 value;
    TInt retVal = PropertyL(KIapIdLocalName, value);
    
    if ( retVal == KErrNone )
        {
        TLex8 lex(value);
        retVal = lex.Val(aCurrentIapId, EDecimal);
        }
        
    if(aCurrentIapId > 0) return retVal;
    else return KErrNotFound;
    }

EXPORT_C void CSenTransportProperties::SetIapIdL(TUint32 aIapId)
    {
    TBuf8<128> buf;
    buf.AppendFormat(_L8("%u"), aIapId);
    SetPropertyL(KIapIdLocalName, buf);
    }

// Setter for Service Network Access Point (SNAP) ID
EXPORT_C void CSenTransportProperties::SetSnapIdL(TUint32 aSnapId)
    {
	TBuf8<128> buf;
    buf.AppendFormat(_L8("%u"), aSnapId);
    SetPropertyL(KSnapIdLocalName, buf);
    }

// Getter for SNAP ID
EXPORT_C TInt CSenTransportProperties::SnapIdL(TUint32& aCurrentSnapId)
    {
    TPtrC8 value;
    TInt retVal = PropertyL(KSnapIdLocalName, value);
    
    if ( retVal == KErrNone )
        {
        TLex8 lex(value);
        retVal = lex.Val(aCurrentSnapId, EDecimal);
        }
        
    if(aCurrentSnapId > 0) return retVal;
    else return KErrNotFound;
    }
EXPORT_C TInt CSenTransportProperties::ProxyPortL(TInt& aProxyPort)
    {
    TPtrC8 value;
    TInt retVal = PropertyL(KProxyPortLocalName, value);
    if ( retVal == KErrNone )
        {
        TLex8 lex(value);
        TUint proxyPort;
        retVal = lex.Val(proxyPort, EDecimal);
        if ( retVal == KErrNone )
            {
            aProxyPort = proxyPort;
            }
        }

    return retVal;
    }

EXPORT_C void CSenTransportProperties::SetProxyPortL(TInt aProxyPort)
    {
    SetIntPropertyL(KProxyPortLocalName, aProxyPort);
    }

EXPORT_C TInt CSenTransportProperties::ProxyHostL(TPtrC8& aProxyHost)
    {
    return PropertyL(KProxyHostLocalName, aProxyHost);
    }

EXPORT_C void CSenTransportProperties::SetProxyHostL(const TDesC8& aProxyHost)
    {
    SetPropertyL(KProxyHostLocalName, aProxyHost);
    }

EXPORT_C TInt CSenTransportProperties::ProxyUsageL(TBool& aValue)
    {
    return BoolPropertyL(KProxyUsageLocalName, aValue);
    }

EXPORT_C void CSenTransportProperties::SetProxyUsageL(TBool aProxyUsage)
    {
    SetBoolPropertyL(KProxyUsageLocalName, aProxyUsage);
    }
    
EXPORT_C TInt CSenTransportProperties::SecureDialogL(TBool& aValue)
    {
    return BoolPropertyL(KSecureDialogLocalName, aValue);
    }

EXPORT_C void CSenTransportProperties::SetSecureDialogL(TBool aSecureDialog)
    {
    SetBoolPropertyL(KSecureDialogLocalName, aSecureDialog);
    }
/*
EXPORT_C TInt CSenTransportProperties::IAPDialogL(TBool& aValue)
    {
    return BoolPropertyL(KSenIAPDoNotPrompt, aValue);
    }

EXPORT_C void CSenTransportProperties::SetIapDialogL(TBool aIapDialog)
    {
    SetBoolPropertyL(KSenIAPDoNotPrompt, aIapDialog);
    }
*/
EXPORT_C TInt CSenTransportProperties::UserAgentL(TPtrC8& aUserAgent)
    {
    return PropertyL(KUserAgentLocalName, aUserAgent);
    }

EXPORT_C void CSenTransportProperties::SetUserAgentL(const TDesC8& aUserAgent)
    {
    SetPropertyL(KUserAgentLocalName, aUserAgent);
    }

EXPORT_C TInt CSenTransportProperties::DeviceIDL(TPtrC8& aDeviceID)
    {
    return PropertyL(KDeviceIDLocalName, aDeviceID);
    }

EXPORT_C void CSenTransportProperties::SetDeviceIDL(const TDesC8& aDeviceID)
    {
    SetPropertyL(KDeviceIDLocalName, aDeviceID);
    }    

EXPORT_C TInt CSenTransportProperties::SoapActionL(TPtrC8& aAction)
    {
    return PropertyL(KSoapActionLocalName, aAction);
    }

EXPORT_C void CSenTransportProperties::SetSoapActionL(const TDesC8& aAction)
    {
    SetPropertyL(KSoapActionLocalName, aAction);
    }    

EXPORT_C void CSenTransportProperties::ApplyBindingL(TSOAPVersion /*aSoapVersion*/)
    {
    }

EXPORT_C TInt CSenTransportProperties::DownloadFolderL(TPtrC8& aDownloadFolder)
    {
    return PropertyL(KDownloadFolderLocalName, aDownloadFolder);
    }
EXPORT_C void CSenTransportProperties::SetDownloadFolderL(const TDesC8& aDownloadFolder)
    {
    SetPropertyL(KDownloadFolderLocalName, aDownloadFolder);
    }

EXPORT_C TInt CSenTransportProperties::FileAttachmentL(const TDesC8& aCid, HBufC8*& aFileName)
    {
    TLSLOG_L(KSenUtilsLogChannel, KMinLogLevel, "CSenTransportProperties::FileAttachmentL:");
    TLSLOG_FORMAT((KSenUtilsLogChannel, KMinLogLevel, _L8("- CID: '%S'"), &aCid));        
    
//    _LIT8(KAttachment, "Attachment");
    TPtrC8 pFileAttachments;
    TInt retVal( KErrNotFound );
    retVal = PropertyL( KFileAttachmentsLocalName, pFileAttachments );    
    if ( retVal == KErrNone )
        {
        CSenXmlReader* reader = CSenXmlReader::NewL(KXmlParserMimeType); // use libxml2 sax parser
        CleanupStack::PushL( reader );
        CSenDomFragment* pFragment = CSenDomFragment::NewL();
        CleanupStack::PushL( pFragment );
        pFragment->SetReader( *reader );    
        pFragment->BuildFrom( pFileAttachments );  
        
        if (aFileName)
            {
            delete aFileName;
            aFileName = NULL;
            }
        
        RPointerArray<CSenElement>& elements = pFragment->AsElement().ElementsL();
        TInt count(elements.Count());
        for ( TInt i=0; i<count && !aFileName; i++ )
            {
            CSenElement* pElement = elements[i];
            TLSLOG_FORMAT((KSenUtilsLogChannel, KNormalLogLevel, _L8("- Processing file attachment element: '%S'"), &pElement->LocalName()));        

            RPointerArray<CSenBaseAttribute>& attributes = pElement->AttributesL();
            TInt attrCount(attributes.Count());
            for ( TInt j=0; j<attrCount; j++ )            
                {
                TPtrC8 name = attributes[j]->Name();
                TPtrC8 value = attributes[j]->Value();

                TLSLOG_FORMAT((KSenUtilsLogChannel, KNormalLogLevel, _L8("- Processing file attachment attribute, name: '%S', value: '%S'"), &name, &value));        
                if ( name == KCid && value == aCid )
                    {
                    TLSLOG_L(KSenUtilsLogChannel, KNormalLogLevel, "- Filename for CID found.");
                    aFileName = HBufC8::NewL( pElement->Content().Length() );
                    TPtr8 aFileNameDes = aFileName->Des();                
                    aFileNameDes.Append( pElement->Content() );
                    retVal = KErrNone;
                    break; // no need to check rest of the attributes
                    }
                }
                // if ( aFileName ) break; // no need to check rest of the child elements
            }
            
        CleanupStack::PopAndDestroy(pFragment);
        CleanupStack::PopAndDestroy(reader);
        }
    return retVal;
        /*
        HBufC8* pLocalName = HBufC8::NewLC(KAttachment().Length()+aCid.Length());
        TPtr8 localNameDes = pLocalName->Des();
        localNameDes.Append(KAttachment);
        localNameDes.Append(aCid);
        CSenElement* pElement = pFragment->AsElement().Element(*pLocalName);
        if(pElement)
            {
            if (aFileName)
                {
                delete aFileName;
                aFileName = NULL;
                }
            aFileName = HBufC8::NewL(pElement->Content().Length());
            TPtr8 aFileNameDes = aFileName->Des();                
            aFileNameDes.Append(pElement->Content());
            CleanupStack::PopAndDestroy(pLocalName);
            CleanupStack::PopAndDestroy(pFragment);
            CleanupStack::PopAndDestroy(reader);
            return KErrNone;
            }
        else
            {
            CleanupStack::PopAndDestroy(pLocalName);
            CleanupStack::PopAndDestroy(pFragment);
            CleanupStack::PopAndDestroy(reader);
            return KErrNotFound;
            }
        }
            
        */            
    }

EXPORT_C TInt CSenTransportProperties::SetFileAttachmentL(const TDesC8& aCid, const TDesC8& aFileName)
    {
    TLSLOG_L(KSenUtilsLogChannel, KMinLogLevel, "CSenTransportProperties::SetFileAttachmentL:");
    TLSLOG_FORMAT((KSenUtilsLogChannel, KMinLogLevel, _L8("- CID: '%S'"), &aCid));        
    
    TBuf<32> num;
    num.AppendNum( iFileAttachmentNum );

    _LIT8(KAttachments,"Attachments");
    _LIT8(KAttachment,"Attachment");
    TPtrC8 pFileAttachments;
    TInt err(KErrNone);
    err = PropertyL(KFileAttachmentsLocalName, pFileAttachments);
    if (err != KErrNotFound)
        {
        // Add to existing <Attachements/> fragment:
        CSenXmlReader* reader = CSenXmlReader::NewL(KXmlParserMimeType); // use libxml2 sax parser
        CleanupStack::PushL(reader);
        CSenDomFragment* pFragment = CSenDomFragment::NewL();
        CleanupStack::PushL(pFragment);
        pFragment->SetReader(*reader);    
        pFragment->BuildFrom(pFileAttachments);  
        
        // --- //
        HBufC8* pLocalName = HBufC8::NewLC( KAttachment().Length() + num.Length() );
        TPtr8 localNameDes = pLocalName->Des();
        localNameDes.Append( KAttachment );
        localNameDes.Append( num );
        CSenElement* pElement = pFragment->AsElement().Element(*pLocalName);
        if (pElement)  
            {
            CleanupStack::PopAndDestroy(pLocalName);
            CleanupStack::PopAndDestroy(pFragment);
            CleanupStack::PopAndDestroy(reader);
            return KErrAlreadyExists; 
            }
        else
            {
            CSenElement& pElement = pFragment->AsElement().AddElementL(*pLocalName);
            pElement.SetContentL(aFileName);
            SenXmlUtils::AddAttributeL(pElement, KCid, aCid );	//CodeScannerWarnings
            HBufC8* pFragmentAsXml = pFragment->AsXmlL();
            CleanupStack::PushL(pFragmentAsXml);
            SetPropertyL( KFileAttachmentsLocalName, *pFragmentAsXml );
            CleanupStack::PopAndDestroy( pFragmentAsXml );
            }
        CleanupStack::PopAndDestroy(pLocalName);
        CleanupStack::PopAndDestroy(pFragment);
        CleanupStack::PopAndDestroy(reader);
        }
    else
        {
        
        // Create new <Attachements/> -fragment (container) for fileattachent properties
        
        CSenDomFragment* pFragment = CSenDomFragment::NewL( KAttachments );
        CleanupStack::PushL(pFragment);
        
        // --- //        
        //HBufC8* pLocalName = HBufC8::NewLC(KAttachment().Length()+aCid.Length());
        HBufC8* pLocalName = HBufC8::NewLC( KAttachment().Length() + num.Length() );
        TPtr8 localNameDes = pLocalName->Des();
        localNameDes.Append(KAttachment);
        localNameDes.Append( num );
        CSenElement& pElement = pFragment->AsElement().AddElementL(*pLocalName);
        pElement.SetContentL( aFileName );
        SenXmlUtils::AddAttributeL(pElement, KCid, aCid );
        HBufC8* pFragmentAsXml = pFragment->AsXmlL();
        CleanupStack::PushL(pFragmentAsXml);
        SetPropertyL( KFileAttachmentsLocalName, *pFragmentAsXml );
        CleanupStack::PopAndDestroy(pFragmentAsXml);
        
        CleanupStack::PopAndDestroy(pLocalName);
        CleanupStack::PopAndDestroy(pFragment);
        }  
        
    // Increase internal count of file attachements ONLY if method is successful:         
    iFileAttachmentNum++; 
    return KErrNone;    
    }
    
   
EXPORT_C MSenProperties* CSenTransportProperties::CloneL() const
    {
    CSenElement& element = ipFragment->AsElement();
    
    CSenTransportProperties* pProperties = NewLC(element);
    CleanupStack::Pop(pProperties);
    return pProperties;
    }

EXPORT_C MSenProperties* CSenTransportProperties::Clone(TInt& aOkOrError) const
    {
    MSenProperties* pClone = NULL;
    TRAP( aOkOrError, pClone = CloneL(); )
    return pClone;
    }
        
EXPORT_C TInt CSenTransportProperties::MwsNamespaceL( TPtrC8& aMwsNamespace )
    {
    return PropertyL( KMwsNamespaceLocalName, aMwsNamespace );
    }
    
EXPORT_C void CSenTransportProperties::SetMwsNamespaceL( const TDesC8& aMwsNamespace )
    {
    SetPropertyL( KMwsNamespaceLocalName, aMwsNamespace );
    }

EXPORT_C TInt CSenTransportProperties::MessageIdL( TPtrC8& aMessageId )
    {
    return PropertyL( KSenClientGeneratedMessageId, aMessageId );
    }      

EXPORT_C void CSenTransportProperties::SetMessageIdL( const TDesC8& aMessageId )
    {
    SetPropertyL( KSenClientGeneratedMessageId, aMessageId );
    }      

EXPORT_C TInt CSenTransportProperties::OnewayMessageOnOffL( TBool& aOnewayMessageOnOff )
    {
    return BoolPropertyL( KSenOnewayMessageOnOff, aOnewayMessageOnOff );
    }

EXPORT_C void CSenTransportProperties::SetOnewayMessageOnOffL( TBool aOnewayMessageOnOff )
    {
    SetBoolPropertyL( KSenOnewayMessageOnOff, aOnewayMessageOnOff );
    }
    
EXPORT_C TInt CSenTransportProperties::HeartbeatL(TInt& aHeartbeat) 
    { 
    return IntPropertyL( KSenConnectionHeartbeat, aHeartbeat ); 
    }
    
EXPORT_C TInt CSenTransportProperties::SetHeartbeatL(TInt aHeartbeat) 
    { 
    return SetIntPropertyL( KSenConnectionHeartbeat, aHeartbeat ); 
    }
    

EXPORT_C TInt CSenTransportProperties::SetMaxTimeToLiveL(TInt aMaxTTL) 
    { 
    return SetIntPropertyL( KSenConnectionHeartbeatMaxTTL, aMaxTTL ); 
    }

EXPORT_C TInt CSenTransportProperties::MaxTimeToLiveL(TInt& aMaxTTL) 
    { 
    return IntPropertyL( KSenConnectionHeartbeatMaxTTL, aMaxTTL ); 
    }

EXPORT_C TInt CSenTransportProperties::SetMinTimeToLiveL(TInt aMinTTL) 
    { 
    return SetIntPropertyL( KSenConnectionHeartbeatMinTTL, aMinTTL ); 
    }

EXPORT_C TInt CSenTransportProperties::MinTimeToLiveL(TInt& aMinTTL) 
    { 
    return IntPropertyL( KSenConnectionHeartbeatMinTTL, aMinTTL ); 
    }

EXPORT_C TInt CSenTransportProperties::SetEndpointResourceL(const TDesC8& aEndpointResource)
    {
    return SetPropertyL(KSenEndpointResource, aEndpointResource);
    }

EXPORT_C TInt CSenTransportProperties::EndpointResourceL(TPtrC8& aEndpointResource)
    {
    return PropertyL(KSenEndpointResource, aEndpointResource);
    }


// END OF FILE


